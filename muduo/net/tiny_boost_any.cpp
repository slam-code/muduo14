// See http://www.boost.org/libs/any for Documentation.

#ifndef BOOST_ANY_INCLUDED
#define BOOST_ANY_INCLUDED


#include <algorithm>

#include "boost/config.hpp"
#include <boost/type_index.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/throw_exception.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/mpl/if.hpp>

namespace boost
{
    class any
    {
    public: // structors

        any() noexcept
                : content(0)
        {
        }

        template<typename ValueType>
        any(const ValueType &value)
                : content(new holder<ValueType>(value))
        {
        }

        any(const any &other)
                : content(other.content ? other.content->clone() : 0)
        {
        }


        ~any() noexcept
        {
            delete content;
        }

    public: // modifiers

        any &swap(any &rhs) noexcept
        {
            std::swap(content, rhs.content);
            return *this;
        }


        any &operator=(const any &rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        // move assignement
        any &operator=(any &&rhs) noexcept
        {
            rhs.swap(*this);
            any().swap(rhs);
            return *this;
        }

        // Perfect forwarding of ValueType
        template<class ValueType>
        any &operator=(ValueType &&rhs)
        {
            any(static_cast<ValueType &&>(rhs)).swap(*this);
            return *this;
        }

    public: // queries

        bool empty() const noexcept
        {
            return !content;
        }

        void clear() noexcept
        {
            any().swap(*this);
        }

        const std::type_info &type() const noexcept
        {
            return content ? content->type() : std::type_id(nullptr).type_info();
        }

    private:

        class placeholder
        {
        public: // structors

            virtual ~placeholder()
            {
            }

        public: // queries

            virtual const std::type_info &type() const noexcept = 0;

            virtual placeholder *clone() const = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: // structors

            holder(const ValueType &value)
                    : held(value)
            {
            }

        public: // queries

            virtual const std::type_info &type() const noexcept
            {
                return std::type_id(ValueType).type_info();
            }

            virtual placeholder *clone() const
            {
                return new holder(held);
            }

        public: // representation

            ValueType held;

        private: // intentionally left unimplemented
            holder &operator=(const holder &);
        };


    private: // representation

        template<typename ValueType>
        friend ValueType *any_cast(any *) noexcept;

        template<typename ValueType>
        friend ValueType *unsafe_any_cast(any *) noexcept;


        placeholder *content;

    };

    inline void swap(any &lhs, any &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    class bad_any_cast : public std::exception
    {
    public:
        virtual const char *what() const noexcept
        {
            return "boost::bad_any_cast: failed conversion using boost::any_cast";
        }
    };

    template<typename ValueType>
    ValueType *any_cast(any *operand) noexcept
    {
        return operand && operand->type() ==std::type_id(ValueType)
               ? &static_cast<any::holder<ValueType>::type *>(operand->content)->held
               : 0;
    }

    template<typename ValueType>
    inline const ValueType *any_cast(const any *operand) noexcept
    {
        return any_cast<ValueType>(const_cast<any *>(operand));
    }

    template<typename ValueType>
    ValueType any_cast(any &operand)
    {
        typedef typename remove_reference<ValueType>::type nonref;


        nonref *result = any_cast<nonref>(&operand);
        if (!result)
            boost::throw_exception(bad_any_cast());

        // Attempt to avoid construction of a temporary object in cases when 
        // `ValueType` is not a reference. Example:
        // `static_cast<std::string>(*result);` 
        // which is equal to `std::string(*result);`
        typedef typename boost::mpl::if_<
                boost::is_reference<ValueType>,
                ValueType,
                typename boost::add_reference<ValueType>::type
        >::type ref_type;

        return static_cast<ref_type>(*result);
    }

    template<typename ValueType>
    inline ValueType any_cast(const any &operand)
    {
        typedef typename remove_reference<ValueType>::type nonref;
        return any_cast<const nonref &>(const_cast<any &>(operand));
    }


    // Note: The "unsafe" versions of any_cast are not part of the
    // public interface and may be removed at any time. They are
    // required where we know what type is stored in the any and can't
    // use typeid() comparison, e.g., when our types may travel across
    // different shared libraries.
    template<typename ValueType>
    inline ValueType *unsafe_any_cast(any *operand) noexcept
    {
        return &static_cast<any::holder<ValueType> *>(operand->content)->held;
    }

    template<typename ValueType>
    inline const ValueType *unsafe_any_cast(const any *operand) noexcept
    {
        return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
    }
}

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif
