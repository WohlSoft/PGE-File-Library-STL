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

/*! \file mdx_meta_file.cpp
 *
 *  \brief Implements defines MDX structures for the meta objects and file
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "meta_filedata.h"
#include "pge_file_lib_private.h"

#include "mdx/common/mdx_file.h"
#include "mdx/common/mdx_macros.h"
#include "mdx/mdx_meta_objects.hpp"

#include "mdx/mdx_meta_file.h"

struct MDX_MetaFile : MDX_File<MetaLoadCallbacks, MetaSaveCallbacks>
{
    MDX_SECTION("META_BOOKMARKS", Bookmark, bookmark);
};

bool MDX_load_meta(PGE_FileFormats_misc::TextInput& input, const MetaLoadCallbacks& callbacks)
{
    std::unique_ptr<MDX_MetaFile> f(new MDX_MetaFile());
    return f->load_file(input, callbacks);
}

bool MDX_save_meta(PGE_FileFormats_misc::TextOutput& output, const MetaSaveCallbacks& callbacks)
{
    std::unique_ptr<MDX_MetaFile> f(new MDX_MetaFile());
    return f->save_file(output, callbacks);
}
