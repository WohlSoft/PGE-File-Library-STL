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

/*!
 *  \file mdx_level_file.h
 *  \brief Contains data structure definitions for the MDX level loader
 */

#pragma once
#ifndef MDX_LEVEL_FILE_H
#define MDX_LEVEL_FILE_H

#include "pge_file_lib_globs.h"
#include "lvl_filedata.h"

bool MDX_load_level(PGE_FileFormats_misc::TextInput& input, const LevelLoadCallbacks& callbacks);
bool MDX_load_level(PGE_FileFormats_misc::TextInput &file, LevelData &FileData);
bool MDX_load_level_header(PGE_FileFormats_misc::TextInput &file, LevelData &FileData);

bool MDX_save_level(PGE_FileFormats_misc::TextOutput& output, const LevelSaveCallbacks& callbacks);
bool MDX_save_level(PGE_FileFormats_misc::TextOutput &file, const LevelData &FileData);

LevelLoadCallbacks PGEFL_make_load_callbacks(LevelData& target);
LevelLoadCallbacks PGEFL_make_header_load_callbacks(LevelData& target);
LevelSaveCallbacks PGEFL_make_save_callbacks(const LevelData& target);

#endif // #ifndef MDX_LEVEL_FILE_H
