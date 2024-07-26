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
#ifndef MDX_META_OBJECTS_HPP
#define MDX_META_OBJECTS_HPP

/*! \file mdx_meta_objects.hpp
 *
 *  \brief Implements shared MDX structures for all types of files
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#include "meta_filedata.h"
#include "pge_file_lib_private.h"

#include "mdx/common/mdx_file.h"
#include "mdx/common/mdx_macros.h"

MDX_SETUP_OBJECT(Bookmark,
    MDX_FIELD("BM", bookmarkName); //Bookmark name
    MDX_FIELD_NO_SKIP("X", x); // Position X
    MDX_FIELD("Y", y); // Position Y
);

MDX_SETUP_OBJECT(CrashData,
    MDX_FIELD("UT", untitled); //Untitled
    MDX_FIELD("MD", modifyed); //Modyfied
    MDX_FIELD("FF", fmtID); //Recent File format
    MDX_FIELD("FV", fmtVer); //Recent File format version
    MDX_FIELD("N",  filename);  //Filename
    MDX_FIELD("P",  path);  //Path
    MDX_FIELD("FP", fullPath);  //Full file Path
);

#endif // MDX_META_OBJECTS_HPP
