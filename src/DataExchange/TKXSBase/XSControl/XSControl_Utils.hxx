// Created on: 1995-12-04
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _XSControl_Utils_HeaderFile
#define _XSControl_Utils_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TCollection_HExtendedString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class Standard_Transient;
class TCollection_HAsciiString;
class TCollection_AsciiString;
class TCollection_HExtendedString;
class TCollection_ExtendedString;
class TopoDS_Shape;

//! This class provides various useful utility routines, to
//! facilitate handling of most common data structures :
//! transients (type, type name ...),
//! strings (ascii or extended, pointed or handled or ...),
//! shapes (reading, writing, testing ...),
//! sequences & arrays (of strings, of transients, of shapes ...),
//! ...
//!
//! Also it gives some helps on some data structures from XSTEP,
//! such as printing on standard trace file, recignizing most
//! currently used auxiliary types (Binder,Mapper ...)
class XSControl_Utils
{
public:
  DEFINE_STANDARD_ALLOC

  //! the only use of this, is to allow a frontal to get one
  //! distinct "Utils" set per separate engine
  Standard_EXPORT XSControl_Utils();

  //! Just prints a line into the current Trace File. This allows to
  //! better characterise the various trace outputs, as desired.
  Standard_EXPORT void TraceLine(const char* line) const;

  //! Just prints a line or a set of lines into the current Trace
  //! File. <lines> can be a HAscii/ExtendedString (produces a print
  //! without ending line) or a HSequence or HArray1 Of ..
  //! (one new line per item)
  Standard_EXPORT void TraceLines(const occ::handle<Standard_Transient>& lines) const;

  Standard_EXPORT bool IsKind(const occ::handle<Standard_Transient>& item,
                                          const occ::handle<Standard_Type>&      what) const;

  //! Returns the name of the dynamic type of an object, i.e. :
  //! If it is a Type, its Name
  //! If it is a object not a type, the Name of its DynamicType
  //! If it is Null, an empty string
  //! If <nopk> is False (D), gives complete name
  //! If <nopk> is True, returns class name without package
  Standard_EXPORT const char* TypeName(const occ::handle<Standard_Transient>& item,
                                            const bool nopk = false) const;

  Standard_EXPORT occ::handle<Standard_Transient> TraValue(const occ::handle<Standard_Transient>& list,
                                                      const int            num) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> NewSeqTra() const;

  Standard_EXPORT void AppendTra(const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& seqval,
                                 const occ::handle<Standard_Transient>&           traval) const;

  Standard_EXPORT const char* DateString(const int yy,
                                              const int mm,
                                              const int dd,
                                              const int hh,
                                              const int mn,
                                              const int ss) const;

  Standard_EXPORT void DateValues(const char* text,
                                  int&      yy,
                                  int&      mm,
                                  int&      dd,
                                  int&      hh,
                                  int&      mn,
                                  int&      ss) const;

  Standard_EXPORT const char* ToCString(const occ::handle<TCollection_HAsciiString>& strval) const;

  Standard_EXPORT const char* ToCString(const TCollection_AsciiString& strval) const;

  Standard_EXPORT occ::handle<TCollection_HAsciiString> ToHString(const char* strcon) const;

  Standard_EXPORT TCollection_AsciiString ToAString(const char* strcon) const;

  Standard_EXPORT const char16_t*
    ToEString(const occ::handle<TCollection_HExtendedString>& strval) const;

  Standard_EXPORT const char16_t* ToEString(const TCollection_ExtendedString& strval) const;

  Standard_EXPORT occ::handle<TCollection_HExtendedString> ToHString(
    const char16_t* strcon) const;

  Standard_EXPORT TCollection_ExtendedString ToXString(const char16_t* strcon) const;

  Standard_EXPORT const char16_t* AsciiToExtended(const char* str) const;

  Standard_EXPORT bool IsAscii(const char16_t* str) const;

  Standard_EXPORT const char* ExtendedToAscii(const char16_t* str) const;

  Standard_EXPORT const char* CStrValue(const occ::handle<Standard_Transient>& list,
                                             const int            num) const;

  Standard_EXPORT const char16_t* EStrValue(const occ::handle<Standard_Transient>& list,
                                               const int            num) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> NewSeqCStr() const;

  Standard_EXPORT void AppendCStr(const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& seqval,
                                  const char*                         strval) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>> NewSeqEStr() const;

  Standard_EXPORT void AppendEStr(const occ::handle<NCollection_HSequence<occ::handle<TCollection_HExtendedString>>>& seqval,
                                  const char16_t*                          strval) const;

  //! Converts a list of Shapes to a Compound (a kind of Shape)
  Standard_EXPORT TopoDS_Shape
    CompoundFromSeq(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqval) const;

  //! Returns the type of a Shape : true type if <compound> is False
  //! If <compound> is True and <shape> is a Compound, iterates on
  //! its items. If all are of the same type, returns this type.
  //! Else, returns COMPOUND. If it is empty, returns SHAPE
  //! For a Null Shape, returns SHAPE
  Standard_EXPORT TopAbs_ShapeEnum ShapeType(const TopoDS_Shape&    shape,
                                             const bool compound) const;

  //! From a Shape, builds a Compound as follows :
  //! explores it level by level
  //! If <explore> is False, only COMPOUND items. Else, all items
  //! Adds to the result, shapes which comply to <type>
  //! + if <type> is WIRE, considers free edges (and makes wires)
  //! + if <type> is SHELL, considers free faces (and makes shells)
  //! If <compound> is True, gathers items in compounds which
  //! correspond to starting COMPOUND,SOLID or SHELL containers, or
  //! items directly contained in a Compound
  Standard_EXPORT TopoDS_Shape SortedCompound(const TopoDS_Shape&    shape,
                                              const TopAbs_ShapeEnum type,
                                              const bool explore,
                                              const bool compound) const;

  Standard_EXPORT TopoDS_Shape ShapeValue(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqv,
                                          const int                   num) const;

  Standard_EXPORT occ::handle<NCollection_HSequence<TopoDS_Shape>> NewSeqShape() const;

  Standard_EXPORT void AppendShape(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& seqv,
                                   const TopoDS_Shape&                      shape) const;

  //! Creates a Transient Object from a Shape : it is either a Binder
  //! (used by functions which require a Transient but can process
  //! a Shape, such as viewing functions) or a HShape (according to hs)
  //! Default is a HShape
  Standard_EXPORT occ::handle<Standard_Transient> ShapeBinder(
    const TopoDS_Shape&    shape,
    const bool hs = true) const;

  //! From a Transient, returns a Shape.
  //! In fact, recognizes ShapeBinder ShapeMapper and HShape
  Standard_EXPORT TopoDS_Shape BinderShape(const occ::handle<Standard_Transient>& tr) const;

  Standard_EXPORT int SeqLength(const occ::handle<Standard_Transient>& list) const;

  Standard_EXPORT occ::handle<Standard_Transient> SeqToArr(const occ::handle<Standard_Transient>& seq,
                                                      const int first = 1) const;

  Standard_EXPORT occ::handle<Standard_Transient> ArrToSeq(const occ::handle<Standard_Transient>& arr) const;

  Standard_EXPORT int SeqIntValue(const occ::handle<NCollection_HSequence<int>>& list,
                                               const int                    num) const;

};

#endif // _XSControl_Utils_HeaderFile
