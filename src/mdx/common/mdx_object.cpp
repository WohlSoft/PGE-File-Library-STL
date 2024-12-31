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

#include <vector>
#include "mdx/common/mdx_field.h"
#include "mdx/common/mdx_object.h"

void MDX_BaseObject::load_object(void* dest, const char* line) const
{
    const char* cur_data = line;
    size_t next_field = 0;

    while(*cur_data != '\0')
    {
        size_t try_field = next_field;
        while(true)
        {
            if(m_fields[try_field]->try_load(dest, cur_data))
            {
                next_field++;
                if(next_field == m_fields.size())
                    next_field = 0;

                break;
            }

            try_field++;
            if(try_field == m_fields.size())
                try_field = 0;

            // couldn't find field
            if(try_field == next_field)
            {
                cur_data = MDX_skip_field(cur_data);
                break;
            }
        }
    }
}

bool MDX_BaseObject::save_object(std::string& out, const void* src, const void* ref) const
{
    size_t out_size_pre = out.size();

    bool any_field = false;
    for(const auto* field : m_fields)
    {
        bool not_only = (field->m_save_mode == MDX_BaseField::SaveMode::not_only);
        any_field |= field->try_save(out, src, ref) && !not_only;
    }

    if(!any_field)
        out.resize(out_size_pre);

    return any_field;
}
