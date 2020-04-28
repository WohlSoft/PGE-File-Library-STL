/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_file_lib_private.h"
#include "file_strlist.h"

FileStringList::FileStringList()
{
    lineID=0;
}

FileStringList::FileStringList(PGESTRING fileData)
{
    addData(fileData);
}

FileStringList::~FileStringList()
{
    buffer.clear();
}

void FileStringList::addData(const PGESTRING &fileData)
{
    buffer.clear();
    #ifdef PGE_FILES_QT
    buffer = fileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    #else
    PGE_SPLITSTRING(buffer, fileData, "\n");
    #endif
    lineID = 0;
}

PGESTRING FileStringList::readLine()
{
    PGESTRING sent;

    if(!isEOF())
    {
        sent = buffer[static_cast<pge_size_t>(lineID)];
        lineID++;
    }
    return sent;
}

bool FileStringList::isEOF()
{
    return (lineID >= (signed)buffer.size());
}

bool FileStringList::atEnd()
{
    return isEOF();
}

