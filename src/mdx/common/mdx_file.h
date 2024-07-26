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
#ifndef MDX_FILE_H
#define MDX_FILE_H

/*! \file mdx_file.h
 *
 *  \brief Code representing entire PGE-X files
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include <vector>
#include <string>

#include "pge_base_callbacks.h"

#include "pge_file_lib_globs.h"
#include "mdx/common/mdx_field.h"
#include "mdx/common/mdx_section.h"

struct MDX_BaseFile
{
protected:
    friend struct MDX_BaseSection;

    std::vector<MDX_BaseSection*> m_sections;

    bool load_file(PGE_FileFormats_misc::TextInput& inf, const PGE_FileFormats_misc::LoadCallbacks& cb);

    bool save_file(PGE_FileFormats_misc::TextOutput& outf, const PGE_FileFormats_misc::SaveCallbacks& cb);
};

template<class _load_callbacks_t, class _save_callbacks_t>
struct MDX_File : public MDX_BaseFile
{
    using load_callbacks_t = _load_callbacks_t;
    using save_callbacks_t = _save_callbacks_t;
    template<class obj_t> using section = MDX_Section<load_callbacks_t, save_callbacks_t, obj_t>;

    bool load_file(PGE_FileFormats_misc::TextInput& inf, const load_callbacks_t& cb)
    {
        return MDX_BaseFile::load_file(inf, cb);
    }

    bool save_file(PGE_FileFormats_misc::TextOutput& outf, const save_callbacks_t& cb)
    {
        return MDX_BaseFile::save_file(outf, cb);
    }
};

#endif // #ifndef MDX_FILE_H
