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

#ifdef PGE_FILES_QT

/*! \file mdx_value_qstringlist.cpp
 *
 *  \brief Code to support saving/loading the QStringList (because it is not actually a PGELIST<PGESTRING> in Qt5)
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/common/mdx_value.h"
#include "mdx/common/mdx_exception.h"

#include "pge_file_lib_globs.h"

template<>
const char* MDX_Value<QStringList>::load(QStringList& dest, const char* field_data)
{
    dest.clear();

    const char* cur_pos = field_data;
    if(*cur_pos != '[')
        throw MDX_missing_delimiter('[');

    cur_pos++;

    std::string got_utf8;

    while(*cur_pos != ']' && *cur_pos != '\0')
    {
        try
        {
            cur_pos = MDX_load_value(got_utf8, cur_pos);
            cur_pos = MDX_finish_list_item(cur_pos);
        }
        catch(const MDX_parse_error&)
        {
            std::throw_with_nested(MDX_bad_array(dest.size()));
        }

        dest.push_back(QString::fromStdString(got_utf8));
    }

    if(*(cur_pos - 1) == ',')
        throw MDX_unexpected_character(']');

    if(*cur_pos != ']')
        throw MDX_missing_delimiter(']');

    cur_pos++;

    return cur_pos;
}

template<>
bool MDX_Value<QStringList>::save(std::string& out, const QStringList& src)
{
    if(src.size() == 0)
        return false;

    std::string src_i_utf8;

    out.push_back('[');

    for(const auto& s : src)
    {
        src_i_utf8 = s.toStdString();

        MDX_save_value(out, src_i_utf8);
        out.push_back(',');
    }

    out.back() = ']';

    return true;
}
#endif // #ifdef PGE_FILES_QT
