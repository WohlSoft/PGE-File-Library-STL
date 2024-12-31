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

/*! \file mdx_value_string.cpp
 *
 *  \brief Code to support saving/loading string values
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/common/mdx_value.h"
#include "mdx/common/mdx_exception.h"

#include "pge_file_lib_globs.h"

template<>
const char* MDX_Value<std::string>::load(std::string& dest, const char* field_data)
{
    dest.clear();

    const char* cur_pos = field_data;
    if(*cur_pos != '"')
        throw MDX_missing_delimiter('"');

    bool escape = false;

    while(true)
    {
        const char cur_byte = *(++cur_pos);

        if(cur_byte == '\0')
            break;

        if(escape)
        {
            char escaped;
            if(cur_byte == 'n')
                escaped = '\n';
            else if(cur_byte == 'r')
                escaped = '\r';
            // something like \ " [ , etc
            else
                escaped = cur_byte;

            dest.push_back(escaped);

            escape = false;
            continue;
        }
        else if(cur_byte == '\\')
        {
            escape = true;
            continue;
        }
        else if(cur_byte == ';' || cur_byte == ':')
            throw MDX_unexpected_character(cur_byte);
        else if(cur_byte == '"')
            break;
        else
        {
            escape = false;
            dest.push_back(cur_byte);
        }
    }

    if(*cur_pos != '"')
        throw MDX_missing_delimiter('"');

    cur_pos++;

    return cur_pos;
}

template<>
bool MDX_Value<std::string>::save(std::string& out, const std::string& src)
{
    out += '"';

    for(char c : src)
    {
        switch(c)
        {
        case '\n':
            out += '\\';
            out += 'n';
            break;
        case '\r':
            out += '\\';
            out += 'r';
            break;
        case '\"':
        case ';':
        case ':':
        case '[':
        case ']':
        case ',':
        case '%':
        case '\\':
            out += '\\';
            out += c;
            break;
        default:
            out += c;
            break;
        }
    }

    out += '"';

    return true;
}

#ifdef PGE_FILES_QT
template<>
const char* MDX_Value<QString>::load(QString& dest, const char* field_data)
{
    std::string dest_utf8;

    const char* ret = MDX_load_value(dest_utf8, field_data);
    dest = QString::fromStdString(dest_utf8);

    return ret;
}

template<>
bool MDX_Value<QString>::save(std::string& out, const QString& src)
{
    std::string src_utf8 = src.toStdString();

    return MDX_save_value(out, src_utf8);
}
#endif
