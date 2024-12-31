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

/*! \file mdx_field.hpp
 *
 *  \brief Code to represent single fields (marker:value pairs)
 *
 * Written as an hpp because inlining these significantly improves performance.
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#pragma once
#ifndef PGE_FIELD_HPP
#define PGE_FIELD_HPP

#include "mdx/common/mdx_field.h"
#include "mdx/common/mdx_object.h"

#include "pge_file_lib_globs.h"


inline const char* MDX_skip_field(const char* line)
{
    bool escape = false;
    const char* tag_begin = line;
    const char* tag_end = nullptr;

    try
    {
        while(true)
        {
            if(*line == '\0')
            {
                if(tag_end)
                    throw MDX_missing_delimiter(';');
                else
                    throw MDX_missing_delimiter(':');
            }
            else if(escape)
            {
                // ignore character
                escape = false;
            }
            else if(*line == ';')
            {
                if(!tag_end)
                    throw MDX_missing_delimiter(':');
                else
                    return line + 1;
            }
            else if(*line == ':')
            {
                if(!tag_end)
                    tag_end = line;
                else
                    throw MDX_unexpected_character(':');
            }
            else if(*line == '\\')
            {
                if(!tag_end)
                    throw MDX_unexpected_character('\\');
                else
                    escape = true;
            }

            line++;
        }
    }
    catch(...)
    {
        if(tag_end)
            std::throw_with_nested(MDX_bad_field(tag_begin, tag_end - tag_begin));
        else
            std::throw_with_nested(MDX_bad_field(tag_begin, line - tag_begin));
    }
}

inline const char* MDX_finish_term(const char* line)
{
    if(*line != ';')
        throw MDX_missing_delimiter(';');

    return line + 1;
}


inline MDX_BaseField::MDX_BaseField(MDX_BaseObject* parent, const char* field_name, SaveMode save_mode)
    : m_save_mode(save_mode), m_field_name(field_name)
{
    parent->m_fields.push_back(this);
}

inline bool MDX_BaseField::try_load(void* dest, const char*& field_name) const
{
    int i;

    for(i = 0; m_field_name[i] != '\0'; i++)
    {
        if(field_name[i] != m_field_name[i])
            return false;
    }

    if(field_name[i] == ':')
    {
        try
        {
            field_name = MDX_finish_term(do_load(dest, field_name + i + 1));
        }
        catch(const MDX_parse_error&)
        {
            std::throw_with_nested(MDX_bad_field(m_field_name));
        }
        return true;
    }

    return false;
}

inline bool MDX_BaseField::try_save(std::string& out, const void* src, const void* ref) const
{
    if(m_save_mode != SaveMode::no_skip && !can_save(src, ref))
        return false;

    auto old_size = out.size();

    out += m_field_name;
    out += ':';

    bool do_skip = !do_save(out, src);
    if(do_skip)
    {
        out.resize(old_size);
        return false;
    }

    out += ';';

    return true;
}

#endif // #ifndef PGE_FIELD_HPP
