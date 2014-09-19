/* either.h
 * União discriminada templatizada.
 *
 * either é uma classe capaz de armazenar objetos de tipos diferentes,
 * desenvolvida para o uso em contêiners heterogêneos.
 *
 * Esta classe suporta objetos de quantos tipos forem necessários,
 * especifiados via templates. Objetos desta classe sempre conterão 
 * um elemento válido, de algum dos tipos especificados.
 *
 * Não há suporte para referências ou tipos const/volatile.
 */
// TODO: rewrite entire class to use casts instead of recursion
#ifndef EITHER_H
#define EITHER_H

#include "either_base.h"
#include "utility/type_traits.h"

// Declaração dos operadores relacionais como amigos
template< typename ... Ts >
class either;

/* Igualdade entre valores.
 * Os valores serão considerados iguais apenas se forem do mesmo
 * tipo e os valores internos forem iguais.
 * Exige-se que operator== esteja sobrecarregado para todos
 * os tipos armazenáveis pela classe.
 *
 * Este operador também está sobrecarregado para comparar diretamente
 * um either com um dos tipos, exigindo, neste caso, apenas a sobrecarga
 * de operator== para o tipo comparado.
 *
 * operator!= também está definido para estes casos, apenas não declarado
 * aqui por não haver necessidade de declará-lo como amigo.
 */
template< typename ... Ts >
bool operator==( const either<Ts...>&, const either<Ts...>& );

/* Ordenamento.
 * Se os tipos internos diferirem, o tipo que vem antes na lista
 * de parâmetros para o template é considerado menor.
 * Caso contrário, a comparação é delegada para operator<, que
 * exige-se estar sobrecarregado para todos os tipos da classe. */
template< typename ... Ts >
bool operator<( const either<Ts...>&, const either<Ts...>& );


// Definição
template< typename ... Ts >
class either {
    either_base< Ts... > value;
    unsigned int type;

public:
    /* Constrói a classe com o tipo especificado. */
    template< typename T,
        typename = typename std::enable_if<
            either_base<Ts...>::template type_index<T>() != -1
        >::type
    >
    either( T&& t );

    /* Construtor padrão; inicializa a classe como contendo o primeiro
     * tipo da lista, com o valor nulo deste tipo. */
    either();

    either( const either& );
    either( either&& );
    either& operator=( const either& );
    either& operator=( either&& );
    ~either();

    /* Atribuição de qualquer dos dois tipos de objeto.
     * Caso o tipo interno do objeto mude, o objeto atual é destruído. */
    template< typename T,
        typename = typename std::enable_if<
            either_base<Ts...>::template type_index<T>() != -1
        >::type
    >
    either& operator=( T&& );

    /* Determina se o tipo interno do objeto é o especificado.
     * Como esta função não possui parâmetros, sua chamada deve
     * ser parecida com:
     *  if( x.is<int>() )
     *      ... */
    template< typename T >
    bool is() const;

    /* Retorna o objeto armazenando internamente.
     * Não há erros caso o tipo interno seja diferente do operador
     * de conversão especificado. */
    template< typename T,
        typename = typename std::enable_if<
            either_base<Ts...>::template type_index<T>() != -1
        >::type
    >
    operator T() const;

    /* Retorna o objeto armazenado internamente.
     * Caso o tipo interno seja diferente do tipo requisitado,
     * std::bad_cast é lançado.
     *
     * Como esta função não possui parâmetro, invoque-a como:
     *  int i = x.get_as<int>(); */
    template< typename T >
    T& get_as();
    template< typename T >
    const T& get_as() const;

    friend bool operator== <>( const either&, const either& );
    friend bool operator< <> ( const either&, const either& );
};


// Implementação

// Construtor - a partir de elemento
template< typename ... Ts > template< typename T, typename >
either<Ts...>::either( T&& t ) :
    value( std::forward<T>(t) ),
    type( either_base<Ts...>::template type_index<T>() )
{}

// Construtor padrão
template< typename ... Ts >
either<Ts...>::either() :
    value(),
    type( 0 )
{}

// Construtor de cópia/movimento
template< typename ... Ts >
either<Ts...>::either( const either<Ts...>& e ) :
    value( e.value, e.type ),
    type( e.type )
{}

template< typename ... Ts >
either<Ts...>::either( either<Ts...>&& e ) :
value( std::move(e.value), e.type ),
    type( e.type )
{}

// Destrutor
template< typename ... Ts >
either<Ts...>::~either() {
    value.destroy( type );
}

// Atribuição de T's
template< typename ... Ts > template< typename T, typename >
either<Ts...>& either<Ts...>::operator=( T&& t ) {
    if( either_base<Ts...>::template type_index<T>() == type ) {
        value.get( identity<typename unqualified<T>::type>() )
            = std::forward<T>(t);
        /* Desqualificamos o tipo pois T&& é uma "referência universal",
         * então T pode resolver para const T&, por exemplo. */
    }
    else {
        value.destroy( type );
        new (&value) either_base<Ts...>( std::forward<T>(t) );
        type = either_base<Ts...>::template type_index<T>();
    }
    return *this;
}

// Atribuição de either's
template< typename ... Ts >
either<Ts...>& either<Ts...>::operator=( const either<Ts...>& e ) {
    if( type == e.type )
        value.assign_on_index( type, e.value );
    else {
        value.destroy( type );
        new (&value) either_base<Ts...>( e.value, e.type );
        type = e.type;
    }
    return *this;
}

template< typename ... Ts >
either<Ts...>& either<Ts...>::operator=( either<Ts...>&& e ) {
    if( type == e.type )
        value.assign_on_index( type, e.value );
    else {
        value.destroy( type );
        new (&value) either_base<Ts...>( std::move(e.value), e.type );
        type = e.type;
    }
    return *this;
}

// Funcionalidade básica
template< typename ... Ts > template< typename T >
bool either<Ts...>::is() const {
    return either_base<Ts...>::template type_index<T>() == type;
}

template< typename ... Ts > template< typename T, typename >
either<Ts...>::operator T() const {
    return value.get( identity<T>() );
}

template< typename ... Ts > template< typename T >
T& either<Ts...>::get_as() {
    if( is<T>() )
        return *(T*) &value;
    throw std::runtime_error( "Wrong type either cast\n" );
}
template< typename ... Ts > template< typename T >
const T& either<Ts...>::get_as() const {
    if( is<T>() )
        return *(T*) &value;
    throw std::runtime_error( "Wrong type either cast\n" );
}

// operator==
template< typename ... Ts >
bool operator==( const either<Ts...>& lhs, const either<Ts...>& rhs ) {
    return lhs.type == rhs.type && 
        lhs.value.equals_on_index( rhs.type, rhs.value );
}

template< typename T, typename ... Ts >
typename std::enable_if<
        either_base<Ts...>::template type_index<T>() != -1, bool
    >::type
operator==( const either<Ts...>& lhs, const T& rhs ) {
    if( lhs.template is<T>() )
        return lhs.operator T() == rhs;
    return false;
}

template< typename T, typename ... Ts >
typename std::enable_if<
        either_base<Ts...>::template type_index<T>() != -1, bool
    >::type
operator==( const T& lhs, const either<Ts...>& rhs ) {
    return rhs == lhs;
}

// operator!=
template< typename ... Ts >
bool operator!=( const either<Ts...>& lhs, const either<Ts...>& rhs ) {
    return !(lhs == rhs); 
}

template< typename T, typename ... Ts >
typename std::enable_if<
        either_base<Ts...>::template type_index<T>() != -1, bool
    >::type
operator!=( const either<Ts...>& lhs, const T& rhs ) {
    return !(lhs == rhs); 
}

template< typename T, typename ... Ts >
typename std::enable_if<
        either_base<Ts...>::template type_index<T>() != -1, bool
    >::type
operator!=( const T& lhs, const either<Ts...>& rhs ) {
    return !(lhs == rhs); 
}

// operator<
template< typename ... Ts >
bool operator<( const either<Ts...>& lhs, const either<Ts...>& rhs ) {
    if( lhs.type < rhs.type )
        return true;
    if( rhs.type < lhs.type )
        return false;
    return lhs.value.smaller_on_index( lhs.type, rhs.value );
}
#endif // EITHER_H
