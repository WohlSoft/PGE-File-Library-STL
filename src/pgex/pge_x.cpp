/*
 * PGE File Library - a library to process file formats, part of Moondust project
 *
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef PGE_FILES_QT
#include <QMutex>
#include <QMutexLocker>
#ifndef PGEFL_QRegExp
#   if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#       include <QRegularExpression>
#       define PGEFL_QRegExp QRegularExpression
#       define PGEFL_Match(x) match(x).hasMatch()
#   else
#       include <QRegExp>
#       define PGEFL_QRegExp QRegExp
#       define PGEFL_Match(x) exactMatch(x)
#   endif
#endif
#else
#include <regex>
#endif

#include "pge_x.h"
#include "pgex/file_strlist.h"

namespace PGEExtendedFormat
{
    static const char *heximal_valid_chars    = "0123456789ABCDEFabcdef";
    static const pge_size_t heximal_valid_chars_len = 22;

    static bool isDegit(PGEChar c)
    {
        return ((c >= '0') && (c <= '9'));
    }

    static bool isValid(const PGESTRING &s, const char *valid_chars, const pge_size_t valid_chars_len, bool allow_empty = false)
    {
        if(IsEmpty(s))
            return allow_empty;

        pge_size_t i, j;

        for(i = 0; i < s.size(); i++)
        {
            bool found = false;

            for(j = 0; j < valid_chars_len; j++)
            {
                if(PGEGetChar(s[i]) == valid_chars[j])
                {
                    found = true;
                    break;
                }
            }

            if(!found)
                return false;
        }

        return true;
    }
}


#ifdef PGE_FILES_QT
PGEFile::PGEFile(QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile()
#endif
{
    m_lastError.clear();
    m_rawData.clear();
}

#ifdef PGE_FILES_QT
PGEFile::PGEFile(const PGEFile &pgeFile, QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile(const PGEFile &pgeFile)
#endif
{
    m_rawData = pgeFile.m_rawData;
    m_rawDataTree = pgeFile.m_rawDataTree;
    m_lastError = pgeFile.m_lastError;
}

PGEFile &PGEFile::operator=(const PGEFile &other)
{
    m_rawData = other.m_rawData;
    m_rawDataTree = other.m_rawDataTree;
    m_lastError = other.m_lastError;
    return *this;
}

PGEFile::PGEFile(const PGESTRING &_rawData) :
    m_rawData(_rawData)
{}

PGESTRING PGEFile::removeQuotes(PGESTRING str)
{
    PGESTRING target = PGE_RemStrRng(str, 0, 1);
    target = PGE_RemStrRng(target, static_cast<int>(target.size() - 1), 1);
    return target;
}

void PGEFile::setRawData(const PGESTRING &_rawData)
{
    m_rawData = _rawData;
}

bool PGEFile::buildTreeFromRaw()
{
    PGEXSct PGEXsection;

    FileStringList in;
    in.addData(m_rawData);

    //Read raw data sections
    bool sectionOpened = false;
    while(!in.atEnd())
    {
        PGEXsection.first = in.readLine();
        PGEXsection.second.clear();

        // ignore line if all spaces
        bool all_spaces = true;
        for(const auto c : PGEXsection.first)
        {
            if(c != ' ')
                all_spaces = false;
        }
        if(all_spaces)
            continue;

        // ban section name including null characters
#ifndef PGE_FILES_QT
        if(PGEXsection.first.size() != strlen(PGEXsection.first.c_str()))
#else
        int found = PGEXsection.first.indexOf(QChar('\0'));
        if(found != -1)
#endif
        {
            PGESTRING errSect = PGEXsection.first;
            PGE_CutLength(errSect, 20);
            PGE_FilterBinary(errSect);
            m_lastError = PGESTRING("Section [" + errSect + "] has invalid name");
            return false;
        }

        sectionOpened = true;
        PGESTRING data;
        while(!in.atEnd())
        {
            data = in.readLine();

            if(IsEmpty(data))
                continue;

            if(data == PGEXsection.first + "_END")
            {
                sectionOpened = false;    // Close Section
                break;
            }
            PGEXsection.second.push_back(data);
        }
        m_rawDataTree.push_back(PGEXsection);
    }

    if(sectionOpened)
    {
        PGESTRING errSect = PGEXsection.first;
        PGE_CutLength(errSect, 20);
        PGE_FilterBinary(errSect);
        m_lastError = PGESTRING("Section [" + errSect + "] is not closed");
        return false;
    }

    //Building tree

    for(pge_size_t z = 0; z < m_rawDataTree.size(); z++)
    {
        bool valid = true;
        PGEX_Entry subTree = buildTree(m_rawDataTree[z].second, &valid);
        if(valid)
        {
            //Store like subtree
            subTree.type = PGEX_Struct;
            subTree.name = m_rawDataTree[z].first;
            dataTree.push_back(subTree);
        }
        else
        {
            //Store like plain text
            PGEX_Item dataItem;
            dataItem.type = PGEX_PlainText;
            subTree.data.clear();
            subTree.subTree.clear();
            PGEX_Val dataValue;
            dataValue.marker = "PlainText";
            for(pge_size_t i = 0; i < m_rawDataTree[z].second.size(); i++)
                dataValue.value += m_rawDataTree[z].second[i] + "\n";
            dataItem.values.push_back(dataValue);
            subTree.name = m_rawDataTree[z].first;
            subTree.type = PGEX_PlainText;
            subTree.data.push_back(dataItem);
            dataTree.push_back(subTree);
            valid = true;
        }
    }

    return true;
}


PGEFile::PGEX_Entry PGEFile::buildTree(PGESTRINGList &src_data, bool *_valid)
{
    PGEX_Entry entryData;

    bool valid = true;
    for(pge_size_t q = 0; q < src_data.size(); q++)
    {
#if 0
        if(IsSectionTitle(removeSpaces(src_data[q])))
        {
            //Build and store subTree
            PGESTRING nameOfTree = removeSpaces(src_data[q]);
            PGESTRINGList rawSubTree;
            q++;
            for(; q < src_data.size() && src_data[q] != nameOfTree + "_END" ; q++)
            {
                rawSubTree.push_back(src_data[q]);
            }
            PGEX_Entry subTree = buildTree(rawSubTree, &valid);
            if(valid)
            {
                //Store like subtree
                subTree.name = nameOfTree;
                entryData.subTree.push_back(subTree);
                entryData.type = PGEX_Struct;
            }
            else
            {
                //Store like plain text
                subTree.name = nameOfTree;
                subTree.subTree.clear();
                subTree.type = PGEX_PlainText;
                subTree.data.clear();

                PGEX_Item dataItem;
                PGEX_Val dataValue;
                dataItem.type = PGEX_PlainText;

                dataValue.marker = nameOfTree; //-V820
                //foreach(PGESTRING x, rawSubTree) dataValue.value += x+"\n";
                for(auto &st : rawSubTree)
                    dataValue.value += st + "\n";
                dataItem.values.push_back(dataValue);
                subTree.data.push_back(dataItem);
                entryData.subTree.push_back(subTree);
                entryData.type = PGEX_Struct;
                valid = true;
            }

        }
        else
#endif
        {
            PGEX_Item dataItem;
            PGESTRING &srcData_nc = src_data[q];
            enum States
            {
                STATE_MARKER = 0,
                STATE_VALUE = 1,
                STATE_ERROR = 2
            };
            pge_size_t state = 0;
#ifndef PGE_FILES_QT
            pge_size_t size = strlen(srcData_nc.c_str());
#else
            int first_nul = srcData_nc.indexOf(QChar('\0'));
            pge_size_t size = (first_nul == -1) ? srcData_nc.size() : first_nul;
#endif
            pge_size_t tail = size - 1;

            // even if there is a nul in the line, it must still end with a semicolon
            // (so two semicolons will be required for a valid parse if there is a nul)
            if(srcData_nc.size() > 0 && Back(srcData_nc) != ';')
                state = STATE_ERROR;

            PGEX_Val dataValue;
            int escape = 0;
            for(pge_size_t i = 0; i < size; i++)
            {
                if(state == STATE_ERROR)
                {
                    valid = false;
                    break;
                }
                PGEChar c = srcData_nc[i];
                if(escape > 0)
                {
                    escape--;
                }
                switch(state)
                {
                case STATE_MARKER:
                    if(i == tail)
                    {
                        valid = false;
                        break;
                    }
                    if(c == ';' || c == '\\')
                    {
                        state = STATE_ERROR;
                        continue;
                    }
                    if(c == ':')
                    {
                        state = STATE_VALUE;
                        continue;
                    }
                    dataValue.marker.push_back(c);
                    break;
                case STATE_VALUE:
                    if((c == '\\') && (escape == 0))
                    {
                        //Skip escape sequence
                        escape = 2;
                    }
                    if((c == ':') && (escape == 0))
                    {
                        state = STATE_ERROR;
                        continue;
                    }
                    if((c == ';') && (escape == 0))
                    {
                        //STORE DATA
                        dataItem.values.push_back(dataValue);
                        dataValue.marker.clear();
                        dataValue.value.clear();
                        state = STATE_MARKER;
                        continue;
                    }
                    else if(i == tail)
                    {
                        valid = false;
                        break;
                    }
                    dataValue.value.push_back(c);
                    break;
                    //case STATE_ERROR: //Dead code
                    //break;
                }
            }

            if(state == STATE_ERROR)
                valid = false;

            dataItem.type = PGEX_Struct;
            entryData.type = PGEX_Struct;
            entryData.data.push_back(dataItem);
            //            PGE_SPLITSTRING(fields, srcData_nc, ";");
            //            PGEX_Item dataItem;
            //            dataItem.type = PGEX_Struct;
            //            for(int i=0;i<(signed)fields.size(); i++)
            //            {
            //                PGESTRING fields_ns = removeSpaces(fields[i]);
            //                if(IsEmpty(fields_ns)) continue;

            //                //Store data into list
            //                PGESTRINGList value;
            //                PGE_SPLITSTRING(value, fields[i], ":");

            //                if( value.size() != 2 )
            //                {
            //                    valid = false; break;
            //                }

            //                PGEX_Val dataValue;
            //                    dataValue.marker = value[0];
            //                    dataValue.value = value[1];
            //                dataItem.values.push_back(dataValue);
            //            }
            //            entryData.type = PGEX_Struct;
            //            entryData.data.push_back(dataItem);
        }
        if(!valid) break;
    }

    if(_valid) *_valid = valid;
    return entryData;
}



PGESTRING PGEFile::lastError()
{
    return m_lastError;
}


bool PGEFile::IsSectionTitle(const PGESTRING &in)
{
    for(pge_size_t i = 0; i < in.size(); i++)
    {
#ifdef PGE_FILES_QT
        char cc = in[i].toLatin1();
#else
        const char &cc = in[i];
#endif
        if(
            ((cc < 'A') || (cc > 'Z')) &&
            ((cc < '0') || (cc > '9')) &&
            (cc != '_')
        )
            return false;
    }
    return true;
}


//validatos
bool PGEFile::IsQoutedString(const PGESTRING &in) // QUOTED STRING
{
    //return QRegExp("^\"(?:[^\"\\\\]|\\\\.)*\"$").exactMatch(in);
    pge_size_t i = 0;
    bool escape = false;
    for(i = 0; i < in.size(); i++)
    {
        if(i == 0)
        {
            if(in[i] != '"')
                return false;
        }
        else if(i == in.size() - 1)
        {
            if((in[i] != '"') || escape)
                return false;
        }
        else if((in[i] == '\\') && !escape)
        {
            escape = true;
            continue;
        }
        else if((in[i] == '"') && !escape)
        {
            return false;
        }

        escape = false;
    }

    if(i == 0 || i == 1)
        return false;

    return true;
}

bool PGEFile::IsHex(const PGESTRING &in) // Heximal string
{
    using namespace PGEExtendedFormat;
    return isValid(in, heximal_valid_chars, heximal_valid_chars_len);
}

bool PGEFile::IsBool(const PGESTRING &in) // Boolean
{
    if((in.size() != 1) || (IsEmpty(in)))
        return false;
    return ((PGEGetChar(in[0]) == '1') || (PGEGetChar(in[0]) == '0'));
}

bool PGEFile::IsIntU(const PGESTRING &in) // Unsigned Int
{
    using namespace PGEExtendedFormat;

    if(IsEmpty(in))
        return false;

#ifdef PGE_FILES_QT
    const PGEChar *data = in.data();
#else
    const PGEChar *data = in.data();
#endif
    pge_size_t strSize = in.size();

    for(pge_size_t i = 0; i < strSize; i++)
    {
        PGEChar c = *data++;
        if((c < '0') || (c > '9')) return false;
    }

    return true;
}

bool PGEFile::IsIntS(const PGESTRING &in) // Signed Int
{
    using namespace PGEExtendedFormat;

    if(IsEmpty(in))
        return false;

    if((in.size() == 1) && (!isDegit(in[0])))           return false;
    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-')) return false;

#ifdef PGE_FILES_QT
    const PGEChar *data = in.data() + 1;
#else
    const PGEChar *data = in.data() + 1;
#endif

    pge_size_t strSize = in.size();
    for(pge_size_t i = 1; i < strSize; i++)
    {
        PGEChar c = *data++;
        if((c < '0') || (c > '9'))
            return false;
    }

    return true;
}

bool PGEFile::IsFloat(const PGESTRING &in) // Float Point numeric
{
    using namespace PGEExtendedFormat;

    if(IsEmpty(in))
        return false;

    if((in.size() == 1) && (!isDegit(in[0])))
        return false;
    if((!isDegit(in[0])) && (PGEGetChar(in[0]) != '-') && (PGEGetChar(in[0]) != '.'))
        return false;

    bool has_digit = false;
    bool decimal = false;
    bool pow10  = false;
    int pow10_digits = 0;
    for(pge_size_t i = ((PGEGetChar(in[0]) == '-') ? 1 : 0); i < in.size(); i++)
    {
        if((!decimal) && (!pow10))
        {
            if((PGEGetChar(in[i]) == '.'))
            {
                decimal = true;
                continue;
            }
        }
        if(!pow10)
        {
            if((PGEGetChar(in[i]) == 'e'))
            {
                pow10 = true;
                if(i == (in.size() - 1)) return false;
                // allow exponent sign (negative or positive)
                if(PGEGetChar(in[i + 1]) == '-' || PGEGetChar(in[i + 1]) == '+') i++;
                if(i == (in.size() - 1)) return false;
                continue;
            }
        }
        if(!isDegit(in[i])) return false;
        if(!pow10)
            has_digit = true;
        else
            pow10_digits++;
    }

    return has_digit && (pow10_digits <= 4);
}

bool PGEFile::IsBoolArray(const PGESTRING &in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return isValid(in, "01", 2, true);
}

bool PGEFile::IsIntArray(const PGESTRING &in) // Boolean array
{
    using namespace PGEExtendedFormat;
#ifdef PGE_FILES_QT
    return PGEFL_QRegExp("^\\[(\\-?\\d+,?)*\\]$").PGEFL_Match(in);
#else
    //FIXME
    std::regex rx("^\\[(\\-?\\d+,?)*\\]$");
    return std::regex_match(in, rx);
#endif
}

bool PGEFile::IsStringArray(const PGESTRING &in) // String array
{
    bool valid = true;
    pge_size_t i = 0, depth = 0, comma = 0;
    bool escape = false;
    while(i < in.size())
    {
        switch(depth)
        {
        case 0://outside array
            if(in[i] == '[')
            {
                depth = 1;
                comma = 2;
            }
            else valid = false;
            break;

        case 1://between entries
            switch(comma)
            {
            case 0: // terminated string
                if(in[i] == ']') depth = 3; //Array terminated
                else if(in[i] == ',') comma++; //Ready for term
                else valid = false;
                break;

            case 1: // comma present
                if(in[i] == '"')     depth = 2; //Open value
                else valid = false;
                break;

            case 2: // opened brackets
                if(in[i] == ']') depth = 3; //Array terminated
                else if(in[i] == '"') depth = 2; //Open value
                else valid = false;
                break;

            default:
                valid = false;
                break;
            }
            break;

        case 2://Inside entry
            if((in[i] == '"') && (!escape))
            {
                depth = 1;    //Close value
                comma = 0;
            }
            // else if(!escape && (in[i] == '[' || in[i] == ']' || in[i] == ','))
            // {
            //     valid = false;
            // }
            else if((in[i] == '\\') && (!escape))
            {
                escape = true;
                break;
            }
            escape = false;
            break;

        case 3://Array terminated
            valid = false;
            break;

        default:
            valid = false;
            break;
        }

        if(!valid)
            break;//Stop parsing on invalid
        i++;
    }

    // require closing ]
    if(depth != 3)
        valid = false;

    return valid;
}


PGESTRINGList PGEFile::X2STRArr(const PGESTRING &in, bool *_valid)
{
    PGESTRINGList strArr;
    PGESTRING entry;
    bool valid = true;
    pge_size_t i = 0, depth = 0, comma = 0;
    bool escape = false;

    while(i < in.size())
    {
        switch(depth)
        {
        case 0://outside array
            if(in[i] == '[')
            {
                depth = 1;
                comma = 2;
            }
            else
                valid = false;
            break;

        case 1://between entries
            switch(comma)
            {
            case 0: // terminated string
                if(in[i] == ']') depth = 3; //Array terminated
                else if(in[i] == ',') comma++; //Ready for term
                else valid = false;
                break;

            case 1: // comma present
                if(in[i] == '"')     depth = 2; //Open value
                else valid = false;
                break;

            case 2: // opened brackets
                if(in[i] == ']') depth = 3; //Array terminated
                else if(in[i] == '"') depth = 2; //Open value
                else valid = false;
                break;

            default:
                valid = false;
                break;
            }
            break;

        case 2://Inside entry
            if((in[i] == '"') && (!escape))
            {
                strArr.push_back(X2STRING(entry));    //Close value //-V823
                entry.clear();
                depth = 1;
                comma = 0;
                break;
            }
            // else if((!escape) && (in[i] == '[' || in[i] == ']' || in[i] == ','))
            // {
            //     valid = false;
            //     break;
            // }
            else if((in[i] == '\\') && (!escape))
            {
                escape = true;
            }
            else
            {
                escape = false;
            }

            entry.push_back(in[i]);
            break;

        case 3: //Array terminated
            valid = false;
            break;

        default:
            valid = false;
            break;
        }

        if(!valid)
            break;//Stop parsing on invalid
        i++;
    }

    // require closing ]
    if(depth != 3)
        valid = false;

    if(_valid)
        *_valid = valid;

    return strArr;
}

PGELIST<long> PGEFile::X2IntArr(const PGESTRING &in, bool *_valid)
{
    PGELIST<long> intArr;
    PGESTRINGList strArr;

    if(in.size() < 2 || in[0] != '[' || in[in.size() - 1] != ']')
    {
        if(_valid)
            *_valid = false;
        return intArr;
    }

//    PGE_RemStrRng(in, 0, 1);
//    PGE_RemStrRng(in, in.size() - 1, 1);
    PGE_SPLITSTRING(strArr, PGE_SubStr(in, 1, (int)in.size() - 2), ",");

    for(auto &s : strArr)
    {
        if(!IsIntS(s))
        {
            if(_valid) *_valid = false;
            return intArr;
        }
        intArr.push_back(toLong(s));
    }

    if(_valid)
        *_valid = true;

    return intArr;
}

PGELIST<bool > PGEFile::X2BollArr(const PGESTRING &src)
{
    PGELIST<bool > arr;
    for(PGEChar i : src)
        arr.push_back(i == '1');
    return arr;
}

PGELIST<PGESTRINGList > PGEFile::splitDataLine(const PGESTRING &src_data, bool *_valid)
{
    PGELIST<PGESTRINGList > entryData;
    bool valid = true;
    enum States
    {
        STATE_MARKER = 0,
        STATE_VALUE = 1,
        STATE_ERROR = 2
    };

    pge_size_t state = 0;
#ifndef PGE_FILES_QT
    pge_size_t size = strlen(src_data.c_str());
#else
    int first_nul = src_data.indexOf(QChar('\0'));
    pge_size_t size = (first_nul == -1) ? src_data.size() : first_nul;
#endif
    pge_size_t tail = size - 1;

    // even if there is a nul in the line, it must still end with a semicolon
    // (so two semicolons will be required for a valid parse if there is a nul)
    if(src_data.size() > 0 && Back(src_data) != ';')
        state = STATE_ERROR;

    PGESTRING marker;
    PGESTRING value;
    int escape = 0;

    for(pge_size_t i = 0; i < size; i++)
    {
        if(state == STATE_ERROR)
        {
            valid = false;
            break;
        }

        PGEChar c = src_data[i];
        if(escape > 0)
        {
            escape--;
        }

        switch(state)
        {
        default:
            abort(); // unexpected behavior!
            break;

        case STATE_MARKER:
            if(i == tail)
            {
                valid = false;
                break;
            }
            if(c == ';' || c == '\\')
            {
                state = STATE_ERROR;
                continue;
            }
            if(c == ':')
            {
                state = STATE_VALUE;
                continue;
            }
            marker.push_back(c);
            break;

        case STATE_VALUE:
            if((c == '\\') && (escape == 0))
            {
                //Skip escape sequence
                escape = 2;
            }
            if((c == ':') && (escape == 0))
            {
                state = STATE_ERROR;
                continue;
            }
            if((c == ';') && (escape == 0))
            {
                //STORE ENTRY!
                PGESTRINGList fields;
                fields.push_back(marker);
                fields.push_back(value);
                entryData.push_back(fields);
                marker.clear();
                value.clear();
                state = STATE_MARKER;
                continue;
            }
            else if(i == tail)
            {
                valid = false;
                break;
            }
            value.push_back(c);
            break;
            //case STATE_ERROR:  //Dead code
            //break;
        }
    }

    if(state == STATE_ERROR)
        valid = false;

    if(_valid)
        *_valid = valid;

    return entryData;
}

PGESTRING PGEFile::hStrS(const PGESTRING &input)
{
    return PGEGetChar(input);
}

PGESTRING PGEFile::WriteStrArr(const PGESTRINGList &input)
{
    PGESTRING output;
    if(IsEmpty(input))
        return PGESTRING();

    output.append("[");
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(WriteStr(input[i]));
        output.append(i < input.size() - 1 ? "," : "");
    }
    output.append("]");

    return output;
}

PGESTRING PGEFile::WriteIntArr(const PGELIST<int> &input)
{
    PGESTRING output;
    if(input.empty())
        return PGESTRING();

    output.append("[");
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(fromNum(input[i]) + (i < input.size() - 1 ? "," : ""));
    }
    output.append("]");

    return output;
}

PGESTRING PGEFile::WriteIntArr(const PGELIST<long> &input)
{
    PGESTRING output;
    if(input.empty())
        return PGESTRING();

    output.append("[");
    for(pge_size_t i = 0; i < input.size(); i++)
    {
        output.append(fromNum(input[i]) + (i < input.size() - 1 ? "," : ""));
    }
    output.append("]");

    return output;
}

PGESTRING PGEFile::WriteBoolArr(const PGELIST<bool> &input)
{
    PGESTRING output;
    for(bool i : input)
        output.append(i ? "1" : "0");
    return output;
}

PGESTRING PGEFile::X2STRING(PGESTRING input)
{
    restoreString(input, true);
    return input;
}

void PGEFile::restoreString(PGESTRING &input, bool removeQuotes)
{
    PGESTRING &output = input;
    const pge_size_t first = 0;//For convenience to understand
    pge_size_t j = 0, size = input.size(), tail = input.size() - 1;
    for(pge_size_t i = 0; i < size; i++, j++)
    {
        if(removeQuotes && ((i == first) || (i == tail)))
        {
ReCheckQuotie:
            //Skip quotie character at begin/end
            if(input[i] == '\"')
            {
                i++;
                if(i == tail) goto ReCheckQuotie;
            }
            if(i >= size)
                break;
        }
        if(input[i] == '\\')
        {
            pge_size_t k = i + 1;
            if(k >= size)
            {
                output[j] = '\\';
                continue;
            }
            char c = PGEGetChar(input[k]);
            switch(c)
            {
            case 'n':
                output[j] = '\n';
                i++;
                break;
            case 'r':
                output[j] = '\r';
                i++;
                break;
            case '\"':
                output[j] = '\"';
                i++;
                break;
            case ';':
                output[j] = ';';
                i++;
                break;
            case ':':
                output[j] = ':';
                i++;
                break;
            case '[':
                output[j] = '[';
                i++;
                break;
            case ']':
                output[j] = ']';
                i++;
                break;
            case ',':
                output[j] = ',';
                i++;
                break;
            case '%':
                output[j] = '%';
                i++;
                break;
            case '\\':
                output[j] = '\\';
                i++;
                break;
            default:
                // invalid escape sequence, simply ignore the backslash
                // output[j++] = input[i];
                output[j]   = input[k];
                i++;
                break;
            }
        }
        else
        {
            output[j] = input[i];
        }
    }
    output.resize(j);
}

void PGEFile::escapeString(PGESTRING &output, const PGESTRING &input, bool addQuotes)
{
    pge_size_t j = 0, size = input.size();
    output.resize(size * 2 + (addQuotes ? 2 : 0));
    if(addQuotes)
        output[j++] = '\"';
    for(pge_size_t i = 0; i < size; i++, j++)
    {
        char c = PGEGetChar(input[i]);
        switch(c)
        {
        case '\n':
            output[j++] = '\\';
            output[j] = 'n';
            break;
        case '\r':
            output[j++] = '\\';
            output[j] = 'r';
            break;
        case '\"':
            output[j++] = '\\';
            output[j] = '\"';
            break;
        case ';':
            output[j++] = '\\';
            output[j] = ';';
            break;
        case ':':
            output[j++] = '\\';
            output[j] = ':';
            break;
        case '[':
            output[j++] = '\\';
            output[j] = '[';
            break;
        case ']':
            output[j++] = '\\';
            output[j] = ']';
            break;
        case ',':
            output[j++] = '\\';
            output[j] = ',';
            break;
        case '%':
            output[j++] = '\\';
            output[j] = '%';
            break;
        case '\\':
            output[j++] = '\\';
            output[j] = '\\';
            break;
        default:
            output[j] = input[i];
            break;
        }
    }

    if(addQuotes)
        output[j++] = '\"';

    output.resize(j);
}
