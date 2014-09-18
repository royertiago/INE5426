/* either_base.h
 * União templatizada não-discriminada, usada internamente
 * pela classe either.
 */
#ifndef EITHER_BASE_H
#define EITHER_BASE_H

#include <new>
#include <stdexcept>
#include <utility>
#include "utility/identity.h"
#include "utility/type_traits.h"

template< typename ... Ts >
union either_base;

template< typename Head, typename ... Tail >
union either_base< Head, Tail... > {
    Head head;
    either_base< Tail... > tail;

    static_assert( !std::is_reference<Head>::value,
            "There is no support to references." );
    static_assert( !std::is_const<Head>::value,
            "There is no support to const types." );
    static_assert( !std::is_volatile<Head>::value,
            "There is no support to volatile types." );

    /* Construtor padrão. Inicializado com o valor nulo
     * de Head. */
    either_base() :
        either_base( Head() )
    {}

    /* Constrói o primeiro elemento da união que possui
     * o mesmo tipo do objeto especificado.
     *
     * O primeiro construtor delega a construção para o próximo 
     * eitherBase na sequência; os dois últimos efetuam a 
     * construção de fato, quando o tipo correto chegar. 
     *
     * O segundo parâmetro do template abusa do SFINAE para garantir 
     * que apenas tipos que aparecem em Tail... estejam qualificados
     * para o primeiro construtor. */
    template< typename T,
        typename = typename std::enable_if<
            either_base<Tail...>::type_index( 0, 
                identity<
                    typename unqualified< T >::type
                >()
            ) != -1
        >::type
    >
    either_base( T&& obj ) {
        new (&tail) either_base<Tail...>( std::move(obj) );
    }
    either_base( const Head& obj ) {
        new (&head) Head( obj );
    }
    either_base( Head&& obj ) {
        new (&head) Head( std::move(obj) );
    }

    /* Construtores "de cópia/movimento" - copia os dados do elemento
     * especificado, no índice especificado.
     * index deve ser o índice do tipo ativo no momento. */
    either_base( const either_base& e, unsigned index ) {
        if( index == 0 )
            new (&head) Head( e.head );
        else
            new (&tail) either_base<Tail...>( e.tail, index - 1 );
    }
    either_base( either_base&& e, unsigned index ) {
        if( index == 0 )
            new (&head) Head( std::move(e.head) );
        else
            new (&tail) either_base<Tail...>( std::move(e.tail), index - 1 );
    }

    /* O destrutor desta classe faz nada.
     *
     * Ele existe apenas para que a classe seja utilizável se um dos tipos
     * não possuir construtor trivial. */
    ~either_base() {}

    /* Retorna o índice do tipo especificado.
     * 
     * A primeira função apenas redireciona para as outras duas, mas
     * descartando referências e const/volatile do tipo passado; ela
     * pode ser usada como
     *  eitherBase<Ts...>::template type_index<T>()
     *
     * A segunda e terceira funções fazem o "trabalho sujo", sem descartar
     * const/volatile. Caso os tipos desta classe sejam
     *  T0, T1, T2, ..., TN
     * type_index( j, identity<Ti> ) retorna j + i.
     *
     * Caso um tipo ocorra mais de uma vez na lista de tipos,
     * a primeira aparição é considerada.
     * Caso o tipo não ocorra na lista de tipos, -1 é retornado. */
    template< typename T >
    static constexpr unsigned type_index() {
        return type_index( 0, identity< typename unqualified<T>::type >() );
    }
    template< typename T >
    static constexpr unsigned type_index( unsigned current, identity<T> ) {
        return either_base<Tail...>::type_index( current + 1, identity<T>() );
    }
    static constexpr unsigned type_index( unsigned current, identity<Head> ) {
        return current;
    }


    /* Retorna o elemento do tipo especificado na união.
     * Nenhuma checagem é feita para garantir que o tipo especificado
     * é o tipo ativo no momento. */
    template< typename T >
    T& get( identity<T> ) {
        return tail.get( identity<T>() );
    }
    template< typename T >
    const T& get( identity<T> ) const {
        return tail.get( identity<T>() );
    }
    Head& get( identity<Head> ) {
        return head;
    }
    const Head& get( identity<Head> ) const {
        return head;
    }

    /* Destrói a variável do índice especificado.
     * index deve ser o índice da variável ativa no momento. */
    void destroy( unsigned int index ) {
        if( index == 0 )
            head.~Head();
        else
            tail.destroy( index - 1 );
    }

    /* Efetua atribuição de valores no índice especificado.
     * index deve ser o índice da variável ativa no momento. */
    void assign_on_index( unsigned index, const either_base& e ) {
        if( index == 0 )
            head = e.head;
        else
            tail.assign_on_index( index - 1, e.tail );
    }
    void assign_on_index( unsigned index, either_base&& e ) {
        if( index == 0 )
            head = std::move(e.head);
        else
            tail.assign_on_index( index - 1, std::move(e.tail) );
    }

    /* Operadores "relacionais": teste de ordem (operator<)
     * e igualdade (operator==) feitos pelos métodos
     * smaller_on_index e equals_on_index.
     *
     * O teste será feito no índice especificado. */
    bool smaller_on_index( unsigned index, const either_base& e ) const {
        if( index == 0 )
            return head < e.head;
        else
            return tail.smaller_on_index( index - 1, e.tail );
    }
    bool equals_on_index( unsigned index, const either_base& e ) const {
        if( index == 0 )
            return head == e.head;
        else
            return tail.equals_on_index( index - 1, e.tail );
    }
};

// Especialização para either_base<>
template<>
union either_base<> {
    template< typename T >
    either_base( T&& ) {
        static_assert( dependant_false<T>::value,
                "No such type in either_base" );
    }

    // Construtor padrão
    either_base() = default;

    /* Construtores de cópia/movimento */
    either_base( const either_base&, unsigned ) {
        throw std::runtime_error( "Bad const either&& construction." );
    }
    either_base( either_base&&, unsigned ) {
        throw std::runtime_error( "Bad either&& construction." );
    }


    template< typename T >
    static constexpr unsigned type_index( unsigned, identity<T> ) {
        return -1;
    }

    template< typename T >
    const T& get( identity<T> ) const {
        static_assert( dependant_false<T>::value,
                "No such type in either_base" );
    }

    void destroy( unsigned ) {
        throw std::runtime_error( "Empty either destruction\n" );
    }

    void assign_on_index( unsigned, const either_base<>& ) {
        throw std::runtime_error( "Bad const either& assign." );
    }
    void assign_on_index( unsigned, either_base<>&& ) {
        throw std::runtime_error( "Bad either&& assign." );
    }

    bool smaller_on_index( unsigned, const either_base& ) const {
        throw std::runtime_error( "Empty either on operator<" );
    }
    bool equals_on_index( unsigned, const either_base& ) const {
        throw std::runtime_error( "Empty either on operator==" );
    }
};
#endif // EITHER_BASE_H
