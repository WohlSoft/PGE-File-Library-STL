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

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <string>

#include "pge_file_lib_globs.h"
#include "mdx/common/mdx_field.h"
#include "mdx/common/mdx_object.h"
#include "mdx/common/mdx_section.h"
#include "mdx/common/mdx_file.h"

inline bool MDX_line_is_section_end(const std::string& cur_line, const char* section_name)
{
    if(cur_line.size() <= 4)
        return false;

    const char* c = cur_line.c_str();
    const char* s = section_name;

    // scan to find difference between strings
    while(true)
    {
        if(*c != *s || *c == '\0')
            break;

        c++;
        s++;
    }

    // section name should be a prefix to current line
    if(*s != '\0')
        return false;

    // current line must have a suffix of precisely "_END"
    if(c[0] != '_' || c[1] != 'E' || c[2] != 'N' || c[3] != 'D' || c[4] != '\0')
        return false;

    // current line must not have embedded null
    if((size_t)(&c[4] - &cur_line[0]) != cur_line.size())
        return false;

    return true;
}

void MDX_skip_section(PGE_FileFormats_misc::TextInput& inf, std::string& cur_line, const char* section_name)
{
#ifdef PGE_FILES_QT
    QString utf16_cur_line;
#endif

    while(!inf.eof())
    {
#ifndef PGE_FILES_QT
        inf.readLine(cur_line);
#else
        inf.readLine(utf16_cur_line);
        cur_line = utf16_cur_line.toStdString();
#endif

        if(MDX_line_is_section_end(cur_line, section_name))
            return;
    }

    throw MDX_parse_error_misc("Unterminated section");
}

MDX_BaseSection::MDX_BaseSection(struct MDX_BaseFile* parent, const char* section_name, bool combine_objects, const MDX_BaseObject& obj_loader, void* obj_ptr, const void* ref_ptr)
    : m_section_name(section_name), m_obj_loader(obj_loader), m_obj_ptr(obj_ptr), m_ref_ptr(ref_ptr), m_combine_objects(combine_objects)
{
    parent->m_sections.push_back(this);
}

bool MDX_BaseSection::try_load(const void* cb, PGE_FileFormats_misc::TextInput& inf, std::string& cur_line)
{
#ifdef PGE_FILES_QT
    QString utf16_cur_line;
#endif

    // check match
    if(cur_line != m_section_name)
        return false;

    // skip if there is no callback registered
    if(!has_load_callback(cb))
        return false;

    while(true)
    {
#ifndef PGE_FILES_QT
        inf.readLine(cur_line);
#else
        inf.readLine(utf16_cur_line);
        cur_line = utf16_cur_line.toStdString();
#endif

        // empty line (or EOF)
        if(cur_line.empty())
        {
            if(inf.eof())
                throw MDX_parse_error_misc("Unterminated section");
            else
            {
                // allow it because PGE-X does
            }
        }
        // ordinary line
        else if(*(cur_line.end() - 1) == ';')
        {
            if(!m_combine_objects)
                reset();

            m_obj_loader.load_object(m_obj_ptr, cur_line.c_str());

            if(!m_combine_objects)
            {
                if(!load_callback(cb))
                {
                    MDX_skip_section(inf, cur_line, m_section_name);
                    return true;
                }
            }
        }
        // section end line
        else if(MDX_line_is_section_end(cur_line, m_section_name))
        {
            if(m_combine_objects)
                load_callback(cb);

            return true;
        }
        // unterminated line
        else
            throw MDX_missing_delimiter(';');
    }
}

void MDX_BaseSection::do_save(const void* cb, PGE_FileFormats_misc::TextOutput& outf, std::string& out_buffer)
{
#ifdef PGE_FILES_QT
    QString utf16_out;
#endif

    // skip if there is no callback registered
    if(!has_save_callback(cb))
        return;

    size_t out_buffer_size_pre = out_buffer.size();
    bool restore = true;

    out_buffer += m_section_name;
    out_buffer += '\n';

    for(size_t index = 0; save_callback(cb, index); index++)
    {
        if(!m_obj_loader.save_object(out_buffer, m_obj_ptr, m_ref_ptr))
            continue;

        out_buffer += '\n';
        restore = false;

        if(out_buffer.size() > 2048)
        {
#ifdef PGE_FILES_QT
            utf16_out = QString::fromStdString(out_buffer);
            outf.write(utf16_out);
#else
            outf.write(out_buffer);
#endif
            out_buffer.clear();
        }
    }

    if(restore)
        out_buffer.resize(out_buffer_size_pre);
    else
    {
        out_buffer += m_section_name;
        out_buffer += "_END\n";

#ifdef PGE_FILES_QT
        utf16_out = QString::fromStdString(out_buffer);
        outf.write(utf16_out);
#else
        outf.write(out_buffer);
#endif
        out_buffer.clear();
    }
}
