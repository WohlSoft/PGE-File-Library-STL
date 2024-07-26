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

/*!
 *  \file mdx_world_file.h
 *  \brief Contains data structure definitions for the MDX level loader
 */

#pragma once
#ifndef MDX_WORLD_FILE_H
#define MDX_WORLD_FILE_H

#include "pge_file_lib_globs.h"
#include "wld_filedata.h"

bool MDX_load_world(PGE_FileFormats_misc::TextInput& input, const WorldLoadCallbacks& callbacks);
bool MDX_load_world(PGE_FileFormats_misc::TextInput &file, WorldData &FileData);
bool MDX_load_world_header(PGE_FileFormats_misc::TextInput &file, WorldData &FileData);

bool MDX_save_world(PGE_FileFormats_misc::TextOutput& output, const WorldSaveCallbacks& callbacks);
bool MDX_save_world(PGE_FileFormats_misc::TextOutput &file, const WorldData &FileData);

#endif // #ifndef MDX_LEVEL_FILE_H
