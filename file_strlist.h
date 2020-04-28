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

/*!
 * \file file_strlist.h
 * \brief Contains defition of a FileStringList class - a string container
 *        which automatically splits lines by line feeds
 */

#pragma once
#ifndef FILE_STRLIST_H
#define FILE_STRLIST_H

#ifdef PGE_FILES_QT
#include <QString>
#include <QStringList>
#include <QObject>
typedef QString PGESTRING;
typedef QStringList PGESTRINGList;
#else
#include <vector>
#include <string>
typedef std::string PGESTRING;
typedef std::vector<std::string> PGESTRINGList;
#endif

/*!
 * \brief Provides string container automatically splited from entire file data
 */
#ifdef PGE_FILES_QT
class FileStringList:public QObject
{
    Q_OBJECT
#else
class FileStringList
{
#endif

public:
    /*!
     * \brief Constructor
     */
    FileStringList();

    /*!
     * \brief Constructor with pre-set data
     * \param fileData file data which will be splited by line-feeds
     */
    FileStringList(PGESTRING fileData);

    /*!
     * Destructor
     */
    ~FileStringList();

    /*!
     * \brief Changes filedata and rebuilds list of lines
     * \param fileData file data which will be splited by line-feeds
     */
    void addData(const PGESTRING& fileData);

    /*!
     * \brief Returns current line contents and incements internal line counter
     * \return contents of current line
     */
    PGESTRING readLine();

    /*!
     * \brief Are all lines was gotten?
     * \return true if internal line counter is equal or more than total number of lines
     */
    bool isEOF();

    /*!
     * \brief Are all lines was gotten?
     * \return true if internal line counter is equal or more than total number of lines
     */
    bool atEnd();
private:
    /*!
     * \brief Contains splited list of string lines
     */
    PGESTRINGList buffer;

    /*!
     * \brief Internal counter of gotten lines
     */
    long lineID;
};

#endif // FILE_STRLIST_H

