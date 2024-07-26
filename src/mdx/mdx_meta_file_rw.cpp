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

/*! \file mdx_level_file_rw.cpp
 *
 *  \brief Implements defines MDX functions for loading a level object
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "mdx/mdx_meta_file.h"
#include "mdx/common/mdx_exception.h"
#include "file_formats.h"
#include "pge_file_lib_private.h"
#include "pge_file_lib_globs.h"

static void s_on_error(void* _FileData, FileFormatsError& err)
{
    MetaData& FileData = *reinterpret_cast<MetaData*>(_FileData);

    static_cast<FileFormatsError&>(FileData.meta) = std::move(err);
    FileData.meta.ReadFileValid = false;
}

static bool s_load_bookmark(void* _FileData, Bookmark& dest)
{
    MetaData& FileData = *reinterpret_cast<MetaData*>(_FileData);
    FileData.bookmarks.push_back(std::move(dest));

    return true;
}

static bool s_save_bookmark(const void* _FileData, Bookmark& obj, pge_size_t index)
{
    const MetaData& FileData = *reinterpret_cast<const MetaData*>(_FileData);

    if(index >= FileData.bookmarks.size())
        return false;

    obj = FileData.bookmarks[index];

    return true;
}

bool MDX_load_meta(PGE_FileFormats_misc::TextInput &file, MetaData &FileData)
{
    FileData = MetaData();

    MetaLoadCallbacks callbacks;

    callbacks.on_error = s_on_error;

    callbacks.load_bookmark = s_load_bookmark;

    callbacks.userdata = reinterpret_cast<void*>(&FileData);

    return MDX_load_meta(file, callbacks);
}

bool MDX_save_meta(PGE_FileFormats_misc::TextOutput &file, const MetaData &FileData)
{
    MetaSaveCallbacks callbacks;

    callbacks.save_bookmark = s_save_bookmark;

    callbacks.userdata = reinterpret_cast<const void*>(&FileData);

    return MDX_save_meta(file, callbacks);
}
