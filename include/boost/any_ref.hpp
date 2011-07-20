#ifndef _BOOST_ANY_REF_HPP_
#define _BOOST_ANY_REF_HPP_
#include <typeinfo>
#include <new>
#include <string.h>
#include <boost/throw_exception.hpp>

namespace boost {

namespace detail {
    namespace any_ref {
        struct place_holder {
            virtual ~place_holder(){}
            virtual const std::type_info & type() const { 
                return typeid(place_holder); 
            }
        };

        template<typename T>
        struct place_holder_impl : private place_holder {
        };

        template<typename T>
        struct place_holder_impl<const T&> : public place_holder {
            place_holder_impl( const T& v ):m_ref(v){}
            virtual const std::type_info & type() const {
                return typeid(const T&);
            }
            const T& m_ref;
        };

        template<typename T>
        struct place_holder_impl<T&> : public place_holder_impl<const T&> {
            place_holder_impl( T& v )
            :place_holder_impl<const T&>(v){}

            virtual const std::type_info & type() const {
                return typeid(T&);
            }
        };
   } // namespace any_ref
} // namespace detail

    class bad_any_ref_cast : public std::bad_cast
    {
    public:
        virtual const char * what() const throw() {
            return "boost::bad_any_ref_cast: "
                   "failed conversion using boost::any_ref";
        }
    };

/**
    @brief stack based implementation of boost::any that works for const reference types.

    The motivation behind creating this class was to provide a polymorphic interface
    to methods of different signatures.  This is useful for runtime invocation of
    methods.
*/
class any_ref 
{
    public:
        any_ref() {
            new (held) detail::any_ref::place_holder();
        }

        template<typename T>
        any_ref( const T& v ) {
            new (held) detail::any_ref::place_holder_impl<const T&>(v);
        }

        template<typename T>
        any_ref( T& v ) {
            new (held) detail::any_ref::place_holder_impl<T&>(v);
        }

        const std::type_info& type()const { return ((detail::any_ref::place_holder*)held)->type(); }

        template<typename T>
        inline operator const T&()const {
            const T* v = const_ptr<T>();
            if( !v )
                boost::throw_exception(bad_any_ref_cast());
            return *v;
        }
        template<typename T>
        inline const T* const_ptr()const {
            detail::any_ref::place_holder_impl<const T&>* p = 
                dynamic_cast<detail::any_ref::place_holder_impl<const T&>* >((detail::any_ref::place_holder*)held);
            if( p ) 
                return &p->m_ref;
            return 0;
        }

        template<typename T>
        inline operator T&()const {
            T* v = ptr<T>();
            if( !v )
                boost::throw_exception(bad_any_ref_cast());
            return *v;
        }
        template<typename T>
        inline T* ptr()const {
            detail::any_ref::place_holder_impl<T&>* p = 
                dynamic_cast<detail::any_ref::place_holder_impl<T&>* >((detail::any_ref::place_holder*)held);
            if( p ) 
                return const_cast<T*>(&p->m_ref);
            return 0;
        }

        any_ref& operator=( const any_ref& r ) {
            if( this != &r )
                memcpy(held,r.held,sizeof(held));
            return *this;
        }
        template<typename T>
        any_ref& operator=( const T& v ) {
            new (held) detail::any_ref::place_holder_impl<const T&>(v);
            return *this;
        }
        template<typename T>
        any_ref& operator=( T& v ) {
            new (held) detail::any_ref::place_holder_impl<T&>(v);
            return *this;
        }

    private:

        char held[sizeof(detail::any_ref::place_holder_impl<const int&>)];
};

} // namespace boost

#endif
