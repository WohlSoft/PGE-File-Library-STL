/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file mdx_value_object.hpp
 *
 *  \brief Contains code to save/load objects stored in strings
 *
 */

#pragma once
#ifndef MDX_VALUE_OBJECT_HPP
#define MDX_VALUE_OBJECT_HPP

#include "mdx/common/mdx_value.h"

/********************
 * MDX_Value_Object *
 ********************/

template<class obj_loader_t>
struct MDX_Value_Object
{
    static const obj_loader_t s_obj_loader;
    static const char* load(typename obj_loader_t::obj_t& dest, const char* field_data);
    static bool save(std::string& out, const typename obj_loader_t::obj_t& src);
    static bool is_ref(const PGELIST<bool>& /*src*/, const PGELIST<bool>& /*reference*/)
    {
        return false;
    }
};

template<class obj_loader_t>
const char* MDX_Value_Object<obj_loader_t>::load(typename obj_loader_t::obj_t& dest, const char* field_data)
{
    dest = typename obj_loader_t::obj_t();

    std::string object_string;

    const char* next = MDX_load_value(object_string, field_data);

    s_obj_loader.load_object(&dest, object_string.c_str());

    return next;
}

template<class obj_loader_t>
bool MDX_Value_Object<obj_loader_t>::save(std::string& out, const typename obj_loader_t::obj_t& src)
{
    const typename obj_loader_t::obj_t ref;
    std::string object_string;

    if(!s_obj_loader.save_object(object_string, &src, &ref))
        return false;

    MDX_save_value(out, object_string);

    return true;
}

#endif // #ifndef MDX_VALUE_OBJECT_HPP
