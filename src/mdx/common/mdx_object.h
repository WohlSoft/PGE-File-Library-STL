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

#pragma once
#ifndef MDX_OBJECT_H
#define MDX_OBJECT_H

/*! \file mdx_object.h
 *
 *  \brief Code to represent objects using sequences of fields
 *
 * Used as rows in sections or items in object arrays
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include <vector>

struct MDX_BaseObject
{
    template<class obj_loader_t> friend struct MDX_Value_ObjectList;
    template<class obj_loader_t> friend struct MDX_Value_Object;
    friend struct MDX_BaseSection;
    friend struct MDX_BaseField;

protected:
    std::vector<MDX_BaseField*> m_fields;

    void load_object(void* dest, const char* line) const;

    bool save_object(std::string& out, const void* src, const void* ref) const;
};

/* This gets defined by the user for each object type, and then gets used by sections or item lists.
   In those contents it is sometimes called obj_loader_t. */
template<class obj_t>
struct MDX_Object;

#endif // #ifndef MDX_OBJECT_H
