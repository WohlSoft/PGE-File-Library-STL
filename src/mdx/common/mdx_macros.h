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
#ifndef MDX_MACROS_H
#define MDX_MACROS_H

/*! \file mdx_macros.h
 *
 *  \brief Unpleasant macros used for declarative syntax
 *
 * This is a new implementation but supports precisely the same format as PGE-X
 *
 */

#define MDX_FIELD_NAME(MEMBER_NAME) field_ ## MEMBER_NAME
#define MDX_FIELD(NAME, MEMBER_NAME) MDX_Field<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME}
#define MDX_FIELD_NO_SKIP(NAME, MEMBER_NAME) MDX_Field<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME, MDX_BaseField::SaveMode::no_skip}
#define MDX_FIELD_NOT_ONLY(NAME, MEMBER_NAME) MDX_Field<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME, MDX_BaseField::SaveMode::not_only}

#define MDX_FIELD_NONNEG(NAME, MEMBER_NAME) MDX_NonNegField<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME}
#define MDX_FIELD_NONNEG_NO_SKIP(NAME, MEMBER_NAME) MDX_NonNegField<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME, MDX_BaseField::SaveMode::no_skip}
#define MDX_FIELD_NONNEG_NOT_ONLY(NAME, MEMBER_NAME) MDX_NonNegField<obj_t, decltype(obj_t::MEMBER_NAME)> MDX_FIELD_NAME(MEMBER_NAME){this, NAME, &obj_t::MEMBER_NAME, MDX_BaseField::SaveMode::not_only}

#define MDX_UNIQUE_FIELD_NAME(LOAD_FUNC) unique_field_ ## LOAD_FUNC
#define MDX_UNIQUE_FIELD(NAME, LOAD_FUNC, SAVE_FUNC) MDX_UniqueField<obj_t> MDX_UNIQUE_FIELD_NAME(LOAD_FUNC){this, NAME, LOAD_FUNC, SAVE_FUNC}

#define MDX_NESTED_FIELD_NAME(SUBSTRUCT_NAME, MEMBER_NAME) nested_field_ ## SUBSTRUCT_NAME ## MEMBER_NAME
#define MDX_NESTED_FIELD(NAME, SUBSTRUCT_NAME, MEMBER_NAME) MDX_NestedField<obj_t, decltype(obj_t::SUBSTRUCT_NAME), decltype(decltype(obj_t::SUBSTRUCT_NAME)::MEMBER_NAME)> MDX_NESTED_FIELD_NAME(SUBSTRUCT_NAME, MEMBER_NAME){this, NAME, &obj_t::SUBSTRUCT_NAME, &decltype(obj_t::SUBSTRUCT_NAME)::MEMBER_NAME}

#define MDX_NESTED_FIELD_NONNEG(NAME, SUBSTRUCT_NAME, MEMBER_NAME) MDX_NonNegNestedField<obj_t, decltype(obj_t::SUBSTRUCT_NAME), decltype(decltype(obj_t::SUBSTRUCT_NAME)::MEMBER_NAME)> MDX_NESTED_FIELD_NAME(SUBSTRUCT_NAME, MEMBER_NAME){this, NAME, &obj_t::SUBSTRUCT_NAME, &decltype(obj_t::SUBSTRUCT_NAME)::MEMBER_NAME}

#define MDX_FIELD_XTRA() MDX_FieldXtra<obj_t> xtra_field{this}

#define MDX_CALLBACK_LOAD(CALLBACK_NAME) load_ ## CALLBACK_NAME
#define MDX_CALLBACK_SAVE(CALLBACK_NAME) save_ ## CALLBACK_NAME
#define MDX_SECTION_NAME(CALLBACK_NAME) section_ ## CALLBACK_NAME
#define MDX_SECTION(NAME, OBJ_T, CALLBACK_NAME) section<OBJ_T> MDX_SECTION_NAME(CALLBACK_NAME){this, NAME, false, &load_callbacks_t::MDX_CALLBACK_LOAD(CALLBACK_NAME), &save_callbacks_t::MDX_CALLBACK_SAVE(CALLBACK_NAME)}
#define MDX_SECTION_SINGLE(NAME, OBJ_T, CALLBACK_NAME) section<OBJ_T> MDX_SECTION_NAME(OBJ_T){this, NAME, true, &load_callbacks_t::MDX_CALLBACK_LOAD(CALLBACK_NAME), &save_callbacks_t::MDX_CALLBACK_SAVE(CALLBACK_NAME)}

#define MDX_SETUP_OBJECT(OBJ_T, BODY) template<> \
struct MDX_Object<OBJ_T> : MDX_BaseObject \
{ \
    using obj_t = OBJ_T; \
    BODY \
}; \

#define MDX_ENABLE_SUB_LIST(OBJ_T) template<> \
struct MDX_Value<PGELIST<OBJ_T>> : public MDX_Value_ObjectList<MDX_Object<OBJ_T>> {}; \
\
template<> \
const MDX_Object<OBJ_T> MDX_Value_ObjectList<MDX_Object<OBJ_T>>::s_obj_loader{}; \

#define MDX_ENABLE_SUB_STRUCT(OBJ_T) template<> \
struct MDX_Value<OBJ_T> : public MDX_Value_Object<MDX_Object<OBJ_T>> {}; \
\
template<> \
const MDX_Object<OBJ_T> MDX_Value_Object<MDX_Object<OBJ_T>>::s_obj_loader{} \

#endif // #ifndef MDX_MACROS_H
