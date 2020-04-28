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

#include "pge_file_lib_sys.h"
#include "pge_file_lib_private.h"

#include "file_formats.h"


PGESTRING FileFormats::removeQuotes(PGESTRING str)
{
    PGESTRING target = str;
#ifdef PGE_FILES_QT
    if(target.isEmpty())
        return target;
    if(target[0] == QChar('\"'))
        target.remove(0, 1);
    if((!target.isEmpty()) && (target[target.size() - 1] == QChar('\"')))
        target.remove(target.size() - 1, 1);
#else
    if(target.empty())
        return target;
    if(target[0] == '\"')
        target.erase(target.begin() + 0);
    if((!target.empty()) && (target[target.size() - 1] == '\"'))
        target.erase(target.begin() + (std::string::difference_type)target.size() - 1);
#endif
    return target;
}

PGESTRING FileFormats::getErrorString(FileFormats::ErrorCodes errCode)
{
    switch(errCode)
    {
    case Success:
        return "";
    case ERROR_NotExist:
        return "File not exist";
    case ERROR_AccessDenied:
        return "Access denied";
    case ERROR_InvalidSyntax:
        return "Invalid file format";
    case ERROR_PGEX_SectionNotClosed:
        return "PGE-X Section is not closed";
    case ERROR_PGEX_InvalidSyntax:
        return "PGE-X Invalid data entry syntax";
    case ERROR_PGEX_InvalidDataType:
        return "PGE-X Invalid data type";
    }
    return "Unknown error";
}

/***************************************************************************/
CrashData::CrashData() : used(false), untitled(false), modifyed(false), strictModeSMBX64(false), fmtID(0), fmtVer(64) {}

CrashData::CrashData(const CrashData &_cd)
{
    this->used = _cd.used;
    this->untitled = _cd.untitled;
    this->modifyed = _cd.modifyed;
    this->strictModeSMBX64 = _cd.strictModeSMBX64;
    this->fmtID   = _cd.fmtID;
    this->fmtVer  = _cd.fmtVer;
    this->fullPath = _cd.fullPath;
    this->filename = _cd.filename;
    this->path = _cd.path;
}

void CrashData::reset()
{
    used = false;
    untitled = false;
    modifyed = false;
    strictModeSMBX64 = false;
    fmtID   = 0;
    fmtVer  = 0;
    fullPath.clear();
    filename.clear();
    path.clear();
}
