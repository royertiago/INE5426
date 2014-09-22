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

#include "either_helper.h"
#include "utility/type_traits.h"

template< typename ... Ts >
class either {
public:
    /* Constrói a classe com o tipo especificado. */
    template< typename T,
        typename = typename std::enable_if<
            mp::type_index<T, Ts...>::value != -1u
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
            mp::type_index<T, Ts...>::value != -1u
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
     * São ignorados erros caso o tipo interno seja diferente do operador
     * de conversão especificado. */
    template< typename T,
        typename = typename std::enable_if<
            mp::type_index<T, Ts...>::value != -1u
        >::type
    >
    operator T() const;

    /* Retorna o objeto armazenado internamente.
     * Caso o tipo interno seja diferente do tipo requisitado,
     * std::bad_cast é lançado.
     *
     * Como esta função não possui parâmetro, invoque-a como:
     *  int i = x.get<int>(); */
    template< typename T >
    const T& get() const;
    template< typename T >
    T& get();

    static_assert( !mp::logical_or< std::is_reference<Ts>::value...  >::value,
            "There is no support to references." );
    static_assert( !mp::logical_or< std::is_const<Ts>::value... >::value,
            "There is no support to const types." );
    static_assert( !mp::logical_or< std::is_volatile<Ts>::value... >::value,
            "There is no support to volatile types." );

private:
    // value holds the actual value of the object.
    std::aligned_storage_t<sizeof(either_helper::common_alignment<Ts...>)> value;

    /* type is the index mp::type_index<T, Ts...>::value. */
    unsigned type;

    typedef void (*destructor)( void * );
    static destructor destroy[sizeof...(Ts)];
};

// Implementação

template< typename ... Ts >
typename either<Ts...>::destructor either<Ts...>::destroy[sizeof...(Ts)] = {
    either_helper::destroy<Ts>...
};

// Construtor - a partir de elemento
template< typename ... Ts > template< typename T, typename >
either<Ts...>::either( T&& t ) :
    type( mp::type_index<T, Ts...>::value )
{
    new(&value) T( std::forward<T>( t ) );
}

// Construtor padrão
template< typename ... Ts >
either<Ts...>::either() :
    type( 0 )
{
    new(&value) mp::head_t<Ts...>();
}

// Construtor de cópia/movimento
template< typename ... Ts >
either<Ts...>::either( const either<Ts...>& e ) :
    type( e.type )
{
    typedef void (*f)( void *, const void * );
    static f copy_construct[sizeof...(Ts)] = {
        either_helper::copy_construct<Ts>...
    };
    copy_construct[type]( &value, &e.value );
}

template< typename ... Ts >
either<Ts...>::either( either<Ts...>&& e ) :
    type( e.type )
{
    typedef void (*f)( void *, void * );
    static f move_construct[sizeof...(Ts)] = {
        either_helper::move_construct<Ts>...
    };
    move_construct[type]( &value, &e.value );
}

// Destrutor
template< typename ... Ts >
either<Ts...>::~either() {
    destroy[type]( &value );
}

// Atribuição de T's
template< typename ... Ts > template< typename T, typename >
either<Ts...>& either<Ts...>::operator=( T&& t ) {
    if( type == mp::type_index<T, Ts...>::value ) {
        *(T*)&value = std::forward<T>(t);
    } else {
        destroy[type]( &value );
        new(&value) T( std::forward<T>(t) );
        type = mp::type_index<T, Ts...>::value;
    }
    return *this;
}

// Atribuição de either's
template< typename ... Ts >
either<Ts...>& either<Ts...>::operator=( const either<Ts...>& e ) {
    typedef void (*f)( void *, const void * );
    static f copy_assign[sizeof...(Ts)] = {
        either_helper::copy_assign<Ts>...
    };
    static f copy_construct[sizeof...(Ts)] = {
        either_helper::copy_assign<Ts>...
    }; // TODO: this code is copied from constructor

    if( type == e.type )
        copy_assign[type]( &value, &e.value );
    else {
        destroy[type]( &value );
        type = e.type;
        copy_construct[type]( &value, &e.value );
    }
    return *this;
}

template< typename ... Ts >
either<Ts...>& either<Ts...>::operator=( either<Ts...>&& e ) {
    typedef void (*f)( void *, void * );
    static f move_assign[sizeof...(Ts)] = {
        either_helper::move_assign<Ts>...
    };
    static f move_construct[sizeof...(Ts)] = {
        either_helper::move_construct<Ts>...
    }; // TODO: this code is copied from constructor

    if( type == e.type )
        move_assign[type]( &value, &e.value );
    else {
        destroy[type]( &value );
        type = e.type;
        move_construct[type]( &value, &e.value );
    }
    return *this;
}

// Funcionalidade básica
template< typename ... Ts > template< typename T >
bool either<Ts...>::is() const {
    return type == mp::type_index<T, Ts...>::value;
}

template< typename ... Ts > template< typename T, typename >
either<Ts...>::operator T() const {
    return *(T*) &value;
}

template< typename ... Ts > template< typename T >
T& either<Ts...>::get() {
    if( is<T>() )
        return *(T*) &value;
    throw std::runtime_error( "Wrong type either cast\n" );
}
template< typename ... Ts > template< typename T >
const T& either<Ts...>::get() const {
    if( is<T>() )
        return *(const T*) &value;
    throw std::runtime_error( "Wrong type either cast\n" );
}
#endif // EITHER_H
