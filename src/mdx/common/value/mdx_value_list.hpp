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

/*! \file mdx_value_list.hpp
 *
 *  \brief Code to support saving/loading values which are lists of other values
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#pragma once
#ifndef MDX_VALUE_LIST_HPP
#define MDX_VALUE_LIST_HPP

#include "../mdx_value.h"
#include "../mdx_exception.h"

inline const char* MDX_finish_list_item(const char* line)
{
    if(*line == ']')
        return line;
    else if(*line == ',')
        return line + 1;
    else
        throw MDX_missing_delimiter(',');
}

/**********************
 * MDX_Value<PGELIST> *
 **********************/

template<class subtype_t>
struct MDX_Value<PGELIST<subtype_t>>
{
    static const char* load(PGELIST<subtype_t>& dest, const char* field_data);
    static bool save(std::string& out, const PGELIST<subtype_t>& src);
    static bool is_ref(const PGELIST<subtype_t>& src, const PGELIST<subtype_t>& /*reference*/)
    {
        return src.size() == 0;
    }
};

template<class subtype_t>
const char* MDX_Value<PGELIST<subtype_t>>::load(PGELIST<subtype_t>& dest, const char* field_data)
{
    dest.clear();

    const char* cur_pos = field_data;
    if(*cur_pos != '[')
        throw MDX_missing_delimiter('[');

    cur_pos++;

    while(*cur_pos != ']' && *cur_pos != '\0')
    {
#ifndef PGE_FILES_QT
        dest.emplace_back();
#else
        dest.push_back(subtype_t());
#endif

        try
        {
            cur_pos = MDX_load_value(dest.back(), cur_pos);
            cur_pos = MDX_finish_list_item(cur_pos);
        }
        catch(const MDX_parse_error&)
        {
            std::throw_with_nested(MDX_bad_array(dest.size()));
        }
    }

    if(*(cur_pos - 1) == ',')
        throw MDX_unexpected_character(']');

    if(*cur_pos != ']')
        throw MDX_missing_delimiter(']');

    cur_pos++;

    return cur_pos;
}

template<class subtype_t>
bool MDX_Value<PGELIST<subtype_t>>::save(std::string& out, const PGELIST<subtype_t>& src)
{
    out.push_back('[');

    for(const subtype_t& s : src)
    {
        if(MDX_save_value(out, s))
            out.push_back(',');
    }

    // close the array
    if(out.back() == ',')
    {
        out.back() = ']';
        return true;
    }
    // nothing was written, remove the '['
    else
    {
        out.pop_back();
        return false;
    }
}

#endif // #ifndef MDX_VALUE_LIST_HPP
