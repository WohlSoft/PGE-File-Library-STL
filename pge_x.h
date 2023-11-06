/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file pge_x.h
 *
 *  \brief Contains PGE-X Format worker class allows to parse, generate and validate PGE-X based files
 *
 */

#pragma once
#ifndef PGE_X_H
#define PGE_X_H

#include "pge_file_lib_globs.h"
#include "pge_file_lib_private.h"

/*!
 * \brief Container of raw PGE-X data section
 */
typedef PGEPAIR<PGESTRING, PGESTRINGList> PGEXSct;

/*!
 * \brief Provides parsing, generation and validating tools for PGE-X baded file formats such as LVLX, WLDX, SAVX and many other
 */
class PGEFile PGE_FILES_INHERED
{
#ifdef PGE_FILES_QT
    Q_OBJECT
#endif
public:
    /*!
     * \brief Defines type of section or sub-section type of items
     */
    enum PGEX_Item_type
    {
        //! Structured data
        PGEX_Struct = 0,
        //! Any random plain text data
        PGEX_PlainText
    };

    /*!
     * \brief PGE-X Encoded Item field
     */
    struct PGEX_Val
    {
        //! Name of the entry field
        PGESTRING marker;
        //! Encoded value of the entry field
        PGESTRING value;
    };

    /*!
     * \brief PGE-X Data Item
     */
    struct PGEX_Item
    {
        //! Type of entry
        PGEX_Item_type type;
        //! List of available values
        PGELIST<PGEX_Val > values;
    };

    /*!
     * \brief PGE-X Data tree branch
     */
    struct PGEX_Entry
    {
        //! Name of branch
        PGESTRING name;
        //! Type of contained items
        PGEX_Item_type type;
        //! List of contained items except subtrees
        PGELIST<PGEX_Item > data;
        //! List of contained sub-branches
        PGELIST<PGEX_Entry > subTree;
    };

#ifdef PGE_FILES_QT
    /*!
     * \brief QObject-based constructor Constructor
     * \param parent Parent QObject
     */
    PGEFile(QObject *parent = NULL);
    /*!
     * \brief QObject-based constructor copy Constructor
     * \param pgeFile Another PGEFile object
     * \param parent Parent QObject
     */
    PGEFile(const PGEFile &pgeFile, QObject *parent = NULL);
#else
    /*!
     * \brief Constructor
     */
    PGEFile();

    /*!
     * \brief Copy Constructor
     * \param pgeFile Another PGEFile object
     */
    PGEFile(const PGEFile &pgeFile);
#endif

    /*!
     * \brief Assignment operator
     * \param other Another PGEFile object
     */
    PGEFile &operator=(const PGEFile &other);

    /*!
     * \brief Constructor with pre-storing of raw data
     * \param _rawData
     */
    explicit PGEFile(const PGESTRING &_rawData);

    /*!
     * \brief Stores raw data string
     * \param _rawData String contains raw data of entire file
     */
    void setRawData(const PGESTRING &_rawData);

    /*!
     * \brief Parses stored raw data into the data tree
     * \return
     */
    bool buildTreeFromRaw();

    /*!
     * \brief Returns last occouped error
     * \return Last occouped error
     */
    PGESTRING lastError();

    //! Full data tree of all parsed data
    PGELIST<PGEX_Entry > dataTree;

private:
    //! Last occouped error
    PGESTRING m_lastError;
    //! Stored raw data set
    PGESTRING m_rawData;
    //! Unparsed data separated to their data sections
    PGELIST<PGEXSct > m_rawDataTree;

    //Static functions
public:
    /*!
     * \brief Builds a branch of PGE-X data tree
     * \param List of raw data lines
     * \param _valid given value will accept 'true' if everything is fine or false if error was occouped
     * \return Parsed PGE-X tree branch
     */
    static PGEX_Entry buildTree(PGESTRINGList &src_data, bool *_valid = 0);


    // /////////////Validators///////////////
    /*!
     * \brief Validates title of data section
     * \param in Input string which would be a title of the data section
     * \return True if data section title is valid
     */
    static bool IsSectionTitle(const PGESTRING &in);

    /*!
     * \brief Is given value is a quoted string?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsQoutedString(const PGESTRING &in);// QUOTED STRING
    /*!
     * \brief Is given value is a heximal number?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsHex(const PGESTRING &in);// Hex Encoded String
    /*!
     * \brief Is given value is an unsigned integer number?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsIntU(const PGESTRING &in);// UNSIGNED INT
    /*!
     * \brief Is given value is a signed integer number?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsIntS(const PGESTRING &in);// SIGNED INT
    /*!
     * \brief Is given value is a floating point number?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsFloat(PGESTRING &in);// FLOAT
    /*!
     * \brief Is given value is a boolean degit?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsBool(const PGESTRING &in);//BOOL
    /*!
     * \brief Is given value is a boolean array (string contains 0 or 1 degits only)?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsBoolArray(const PGESTRING &in);//Boolean array
    /*!
     * \brief Is given value is an integer array?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsIntArray(const PGESTRING &in);//Integer array
    /*!
     * \brief Is given value is a string array?
     * \param in Input data string with data required to valitade
     * \return true if given value is passed or false if value is invalid
     */
    static bool IsStringArray(const PGESTRING &in);//String array

    //Split string into data values
    static PGELIST<PGESTRINGList> splitDataLine(const PGESTRING &src_data, bool *valid = nullptr);

    //PGE Extended File parameter string generators
    /*!
     * \brief Encode integer numeric value into PGE-X string
     * \param input signed or unsigned integer
     * \return Encoded PGE-X raw string
     */
    template<typename T>
    static inline PGESTRING WriteInt(const T &input)
    {
        return fromNum(input);
    }
    /*!
     * \brief Encode floating point numeric value with rounding into PGE-X string
     * \param input floating point number
     * \return Encoded PGE-X raw string
     */
    template<typename T>
    static inline PGESTRING WriteRoundFloat(const T &input)
    {
        return fromNum(std::round(input));
    }
    /*!
     * \brief Encode boolean value into PGE-X string
     * \param input boolean flag
     * \return Encoded PGE-X raw string
     */
    static inline PGESTRING WriteBool(bool input)
    {
        return PGESTRING((input) ? "1" : "0");
    }

    /*!
     * \brief Encode floating point numeric value into PGE-X string
     * \param input floating point number
     * \return Encoded PGE-X raw string
     */
    template<typename T>
    static PGESTRING WriteFloat(const T &input)
    {
        return fromNum(input);
    }

    /*!
     * \brief Encode string into PGE-X escaped string
     * \param input Plain text string
     * \return Encoded PGE-X raw string
     */
    static inline PGESTRING WriteStr(const PGESTRING &input)
    {
        PGESTRING output;
        escapeString(output, input, true);
        return output;
    }
    static inline PGESTRING WriteStr(const char *input)
    {
        PGESTRING in(input);
        PGESTRING output;
        escapeString(output, in, true);
        return output;
    }
    /*!
     * \brief [WIP] This function must encode string into heximal line
     * \param input Plain text string
     * \return Encoded PGE-X raw string
     */
    static PGESTRING hStrS(const PGESTRING &input);
    /*!
     * \brief Encode string-array into PGE-X escaped string
     * \param input List of plain text strings
     * \return Encoded PGE-X raw string
     */
    static PGESTRING WriteStrArr(const PGESTRINGList &input);
    /*!
     * \brief Encode array of integers into PGE-X escaped string
     * \param input List of integer numbers
     * \return Encoded PGE-X raw string
     */
    static PGESTRING WriteIntArr(const PGELIST<int > &input);
    /*!
     * \brief Encode array of integers into PGE-X escaped string
     * \param input List of integer numbers
     * \return Encoded PGE-X raw string
     */
    static PGESTRING WriteIntArr(const PGELIST<long > &input);
    /*!
     * \brief Encode array of booleans into PGE-X escaped string
     * \param input List of boolean flags
     * \return Encoded PGE-X raw string
     */
    static PGESTRING WriteBoolArr(const PGELIST<bool > &input);

    /*!
     * \brief Decodes PGE-X string into plain text string
     * \param input Encoded PGE-X string value
     * \return Plain text string
     */
    static PGESTRING X2STRING(PGESTRING input);
    /*!
     * \brief Decodes PGE-X String array into array of plain text strings
     * \param src Encoded PGE-X string value
     * \return List of plain text strings
     */
    static PGESTRINGList X2STRArr(const PGESTRING &in, bool *_valid = nullptr);
    /*!
     * \brief Decodes PGE-X String array into array of plain text strings
     * \param src Encoded PGE-X string value
     * \return List of plain text strings
     */
    static PGELIST<long> X2IntArr(const PGESTRING &in, bool *_valid = nullptr);
    /*!
     * \brief Decodes PGE-X Boolean array into array of boolean flags
     * \param src Encoded PGE-X boolean array
     * \return List of boolean flags
     */
    static PGELIST<bool> X2BollArr(const PGESTRING &src);

    /*!
     * \brief Applies PGE-X escape sequensions to the plain text string
     * \param [__out] output Target string where result will be recorded
     * \param [__in]  input plain text string
     * \param [__in]  addQuotes adds quotes to begin and end of the output string
     */
    static void escapeString(PGESTRING &output, const PGESTRING &input, bool addQuotes = false);
    /*!
     * \brief Decodes PGE-X escape-sequensions
     * \param [__inout] input Plain text string with applied escape sequensions
     */
    static void restoreString(PGESTRING &input, bool removeQuotes = false);

    /*!
     * \brief Builds PGE-X raw value with specific marker and raw data string
     * \param marker Name of field
     * \param data PGE-X raw string
     * \return PGE-X Entry field
     */
    static inline PGESTRING value(PGESTRING &&marker, PGESTRING &&data)
    {
        PGESTRING out;
        out += marker + ":" + data + ";";
        return out;
    }

    /*!
     * \brief Removed double quites from begin and end of string if there are exists
     * \param str Plain text string
     * \return Plain text string with removed double quotes at begin and at end
     */
    static PGESTRING removeQuotes(PGESTRING str);
};


#endif // PGE_X_H
