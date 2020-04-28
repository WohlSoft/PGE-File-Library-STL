/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef INVOKE_DEFAULT_HHHHH
#define INVOKE_DEFAULT_HHHHH
#include <type_traits>
#include <cstddef>
#include <utility>

namespace idef
{
    namespace detail
    {
        template<typename T>
        struct remove_const_ref
        {
            using type = typename std::remove_reference<typename std::remove_const<T>::type>::type;
        };

        template<typename PossibleFunc, typename DefaultFunc>
        struct default_if_nullptr_impl
        {
            using func_type = PossibleFunc;
            static const func_type &Get(const PossibleFunc &pf, const DefaultFunc & /*df*/)
            {
                return pf;
            }
        };

        template<typename DefaultFunc>
        struct default_if_nullptr_impl<std::nullptr_t, DefaultFunc>
        {
            using func_type = DefaultFunc;
            static const func_type &Get(std::nullptr_t /*pf*/, const DefaultFunc &df)
            {
                return df;
            }
        };


        template<typename Func>
        struct invoke_or_noop_impl
        {
            template<typename Ret, typename... Args>
            static Ret invoke(Func &&f, Args &&... args)
            {
                return f(std::forward<Args>(args)...);
            }
        };

        template<>
        struct invoke_or_noop_impl<std::nullptr_t>
        {
            template<typename Ret, typename Func, typename... Args>
            static Ret invoke(Func && /*f*/, Args &&... /*args*/)
            {
                return Ret();
            }
        };
    }

    template<typename PossibleFunc, typename DefaultFunc>
    auto default_if_nullptr(const PossibleFunc &pf, const DefaultFunc &df) ->
    const typename detail::default_if_nullptr_impl<typename detail::remove_const_ref<PossibleFunc>::type, DefaultFunc>::func_type &
    {
        using impl_type = detail::default_if_nullptr_impl<typename detail::remove_const_ref<PossibleFunc>::type, DefaultFunc>;
        return impl_type::Get(pf, df);
    }

    template<typename Ret, typename Func, typename... Args>
    Ret invoke_or_noop(Func f, Args &&... args)
    {
        static_assert(std::is_default_constructible<Ret>::value || std::is_same<Ret, void>::value, "Return value must be default constructible!");
        return detail::invoke_or_noop_impl<Func>::template invoke<Ret>(std::forward<Func>(f), std::forward<Args>(args)...);
    }
}

#endif
