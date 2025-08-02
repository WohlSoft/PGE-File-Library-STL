/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef MDX_VALUE_H
#define MDX_VALUE_H

#include <string>

#include "../../../pge_file_lib_globs.h"

/*! \file mdx_value.h
 *
 *  \brief Code to support saving/loading values (also known as "data" in PGE-X)
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

template<class value_t>
struct MDX_Value
{
    static const char* load(value_t& dest, const char* field_data);

    /* tries to save the field, and returns false (restoring out to its original state) if this is impossible */
    static bool save(std::string& out, const value_t& src);

    /* checks if src matches a reference (which is assumed to be a default value) */
    static bool is_ref(const value_t& src, const value_t& reference)
    {
        return src == reference;
    }
};

template<>
struct MDX_Value<PGELIST<bool>>
{
    static const char* load(PGELIST<bool>& dest, const char* field_data);
    static bool save(std::string& out, const PGELIST<bool>& src);
    static bool is_ref(const PGELIST<bool>& src, const PGELIST<bool>& /*reference*/)
    {
        return src.size() == 0;
    }
};

template<class value_t>
inline const char* MDX_load_value(value_t& dest, const char* field_data)
{
    return MDX_Value<value_t>::load(dest, field_data);
}

template<class value_t>
inline bool MDX_save_value(std::string& out, const value_t& src)
{
    return MDX_Value<value_t>::save(out, src);
}

#include "value/mdx_value_list.hpp"
#include "value/mdx_value_object.hpp"
#include "value/mdx_value_objectlist.hpp"

#endif // #ifndef MDX_VALUE_H
