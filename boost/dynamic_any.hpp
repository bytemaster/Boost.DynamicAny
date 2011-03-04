#ifndef BOOST_DYNAMIC_ANY_INCLUDED
#define BOOST_DYNAMIC_ANY_INCLUDED

#include <algorithm>
#include <typeinfo>

#include "boost/config.hpp"
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_scalar.hpp>
#include <boost/throw_exception.hpp>
#include <boost/static_assert.hpp>

// See boost/python/type_id.hpp
// TODO: add BOOST_TYPEID_COMPARE_BY_NAME to config.hpp
# if (defined(__GNUC__) && __GNUC__ >= 3) \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips)) \
 || (defined(__hpux) && defined(__HP_aCC)) \
 || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define BOOST_AUX_DYNAMIC_ANY_TYPE_ID_NAME
#include <cstring>
# endif 

namespace boost
{
    template<bool IsFundamental, typename ValueType>
    struct if_scalar{};

    class dynamic_any
    {
    public: // structors

        dynamic_any()
          : content(0)
        {
        }

        template<typename ValueType>
        dynamic_any(const ValueType & value)
          : content(new holder<ValueType, boost::is_scalar<ValueType>::value >(value))
        {
        }

        dynamic_any(const dynamic_any & other)
          : content(other.content ? other.content->clone() : 0)
        {
        }

        ~dynamic_any()
        {
            delete content;
        }

    public: // modifiers

        dynamic_any & swap(dynamic_any & rhs)
        {
            std::swap(content, rhs.content);
            return *this;
        }

        template<typename ValueType>
        dynamic_any & operator=(const ValueType & rhs)
        {
            dynamic_any(rhs).swap(*this);
            return *this;
        }

        dynamic_any & operator=(dynamic_any rhs)
        {
            rhs.swap(*this);
            return *this;
        }

    public: // queries

        bool empty() const
        {
            return !content;
        }

        const std::type_info & type() const
        {
            return content ? content->type() : typeid(void);
        }

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    private: // types
#else
    public: // types (public so dynamic_any_cast can be non-friend)
#endif

        class placeholder
        {
        public: // structors

            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const std::type_info & type() const = 0;

            virtual placeholder * clone() const = 0;

        };

        template<typename ValueType, bool IsFundamental>
        class holder{};

        template<typename ValueType>
        class holder<ValueType,false> : public ValueType, public placeholder
        {
        public: // structors

            holder(const ValueType & value)
              : ValueType(value)
            {
            }

        public: // queries

            virtual const std::type_info & type() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return dynamic_cast<placeholder*>(new holder(*this));
            }


        private: // intentionally left unimplemented
            holder & operator=(const holder &);
        };

        template<typename ValueType>
        class holder<ValueType,true> : public placeholder
        {
        public: // structors

            holder(const ValueType & value)
              : held(value)
            {
            }

        public: // queries

            virtual const std::type_info & type() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return new holder(held);
            }

        public: // representation

            ValueType held;

        private: // intentionally left unimplemented
            holder & operator=(const holder &);
        };

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

    private: // representation

        template<typename ValueType>
        friend ValueType * dynamic_any_cast(dynamic_any *);

        template<typename ValueType>
        friend ValueType * unsafe_any_cast(dynamic_any *);

        template<bool,typename> friend class if_scalar;
#else

    public: // representation (public so dynamic_any_cast can be non-friend)

#endif

        placeholder * content;

    };

    class bad_dynamic_any_cast : public std::bad_cast
    {
    public:
        virtual const char * what() const throw()
        {
            return "boost::bad_dynamic_any_cast: "
                   "failed conversion using boost::any_cast";
        }
    };

    template<typename ValueType>
    struct if_scalar<false,ValueType>{
        static inline ValueType * dynamic_any_cast(dynamic_any * operand)
        {
            return operand 
                ? dynamic_cast<ValueType*>(operand->content)
                : 0;
        }
    };
    template<typename ValueType>
    struct if_scalar<true,ValueType>{
        static inline ValueType * dynamic_any_cast(dynamic_any * operand)
        {
            return operand && 
    #ifdef BOOST_AUX_ANY_TYPE_ID_NAME
                std::strcmp(operand->type().name(), typeid(ValueType).name()) == 0
    #else
                operand->type() == typeid(ValueType)
    #endif
                ? &static_cast<dynamic_any::holder<ValueType,true> *>(operand->content)->held
                : 0;
        }
    };

    template<typename ValueType>
    ValueType * dynamic_any_cast(dynamic_any * operand)
    {
        return  if_scalar<boost::is_scalar<ValueType>::value,ValueType>::dynamic_any_cast(operand);
    }


    template<typename ValueType>
    inline const ValueType * dynamic_any_cast(const dynamic_any * operand)
    {
        return dynamic_any_cast<ValueType>(const_cast<dynamic_any *>(operand));
    }

    template<typename ValueType>
    ValueType dynamic_any_cast(dynamic_any & operand)
    {
        typedef BOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
        // If 'nonref' is still reference type, it means the user has not
        // specialized 'remove_reference'.

        // Please use BOOST_BROKEN_COMPILER_TYPE_TRAITS_SPECIALIZATION macro
        // to generate specialization of remove_reference for your class
        // See type traits library documentation for details
        BOOST_STATIC_ASSERT(!is_reference<nonref>::value);
#endif

        nonref * result = dynamic_any_cast<nonref>(&operand);
        if(!result)
            boost::throw_exception(bad_dynamic_any_cast());
        return *result;
    }

    template<typename ValueType>
    inline ValueType dynamic_any_cast(const dynamic_any & operand)
    {
        typedef BOOST_DEDUCED_TYPENAME remove_reference<ValueType>::type nonref;

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
        // The comment in the above version of 'any_cast' explains when this
        // assert is fired and what to do.
        BOOST_STATIC_ASSERT(!is_reference<nonref>::value);
#endif

        return dynamic_any_cast<const nonref &>(const_cast<dynamic_any &>(operand));
    }

    // Note: The "unsafe" versions of dynamic_any_cast are not part of the
    // public interface and may be removed at dynamic_any time. They are
    // required where we know what type is stored in the dynamic_any and can't
    // use typeid() comparison, e.g., when our types may travel across
    // different shared libraries.
    template<typename ValueType>
    inline ValueType * unsafe_any_cast(dynamic_any * operand)
    {
        return dynamic_any_cast<ValueType>(operand);
    }

    template<typename ValueType>
    inline const ValueType * unsafe_any_cast(const dynamic_any * operand)
    {
        return unsafe_any_cast<ValueType>(const_cast<dynamic_any *>(operand));
    }
}

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif
