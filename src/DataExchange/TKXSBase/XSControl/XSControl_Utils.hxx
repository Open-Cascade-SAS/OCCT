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
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfHExtendedString.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
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
  Standard_EXPORT void TraceLine(const Standard_CString line) const;

  //! Just prints a line or a set of lines into the current Trace
  //! File. <lines> can be a HAscii/ExtendedString (produces a print
  //! without ending line) or a HSequence or HArray1 Of ..
  //! (one new line per item)
  Standard_EXPORT void TraceLines(const Handle(Standard_Transient)& lines) const;

  Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Transient)& item,
                                          const Handle(Standard_Type)&      what) const;

  //! Returns the name of the dynamic type of an object, i.e. :
  //! If it is a Type, its Name
  //! If it is a object not a type, the Name of its DynamicType
  //! If it is Null, an empty string
  //! If <nopk> is False (D), gives complete name
  //! If <nopk> is True, returns class name without package
  Standard_EXPORT Standard_CString TypeName(const Handle(Standard_Transient)& item,
                                            const Standard_Boolean nopk = Standard_False) const;

  Standard_EXPORT Handle(Standard_Transient) TraValue(const Handle(Standard_Transient)& list,
                                                      const Standard_Integer            num) const;

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) NewSeqTra() const;

  Standard_EXPORT void AppendTra(const Handle(TColStd_HSequenceOfTransient)& seqval,
                                 const Handle(Standard_Transient)&           traval) const;

  Standard_EXPORT Standard_CString DateString(const Standard_Integer yy,
                                              const Standard_Integer mm,
                                              const Standard_Integer dd,
                                              const Standard_Integer hh,
                                              const Standard_Integer mn,
                                              const Standard_Integer ss) const;

  Standard_EXPORT void DateValues(const Standard_CString text,
                                  Standard_Integer&      yy,
                                  Standard_Integer&      mm,
                                  Standard_Integer&      dd,
                                  Standard_Integer&      hh,
                                  Standard_Integer&      mn,
                                  Standard_Integer&      ss) const;

  Standard_EXPORT Standard_CString ToCString(const Handle(TCollection_HAsciiString)& strval) const;

  Standard_EXPORT Standard_CString ToCString(const TCollection_AsciiString& strval) const;

  Standard_EXPORT Handle(TCollection_HAsciiString) ToHString(const Standard_CString strcon) const;

  Standard_EXPORT TCollection_AsciiString ToAString(const Standard_CString strcon) const;

  Standard_EXPORT Standard_ExtString
    ToEString(const Handle(TCollection_HExtendedString)& strval) const;

  Standard_EXPORT Standard_ExtString ToEString(const TCollection_ExtendedString& strval) const;

  Standard_EXPORT Handle(TCollection_HExtendedString) ToHString(
    const Standard_ExtString strcon) const;

  Standard_EXPORT TCollection_ExtendedString ToXString(const Standard_ExtString strcon) const;

  Standard_EXPORT Standard_ExtString AsciiToExtended(const Standard_CString str) const;

  Standard_EXPORT Standard_Boolean IsAscii(const Standard_ExtString str) const;

  Standard_EXPORT Standard_CString ExtendedToAscii(const Standard_ExtString str) const;

  Standard_EXPORT Standard_CString CStrValue(const Handle(Standard_Transient)& list,
                                             const Standard_Integer            num) const;

  Standard_EXPORT Standard_ExtString EStrValue(const Handle(Standard_Transient)& list,
                                               const Standard_Integer            num) const;

  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) NewSeqCStr() const;

  Standard_EXPORT void AppendCStr(const Handle(TColStd_HSequenceOfHAsciiString)& seqval,
                                  const Standard_CString                         strval) const;

  Standard_EXPORT Handle(TColStd_HSequenceOfHExtendedString) NewSeqEStr() const;

  Standard_EXPORT void AppendEStr(const Handle(TColStd_HSequenceOfHExtendedString)& seqval,
                                  const Standard_ExtString                          strval) const;

  //! Converts a list of Shapes to a Compound (a kind of Shape)
  Standard_EXPORT TopoDS_Shape
    CompoundFromSeq(const Handle(TopTools_HSequenceOfShape)& seqval) const;

  //! Returns the type of a Shape : true type if <compound> is False
  //! If <compound> is True and <shape> is a Compound, iterates on
  //! its items. If all are of the same type, returns this type.
  //! Else, returns COMPOUND. If it is empty, returns SHAPE
  //! For a Null Shape, returns SHAPE
  Standard_EXPORT TopAbs_ShapeEnum ShapeType(const TopoDS_Shape&    shape,
                                             const Standard_Boolean compound) const;

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
                                              const Standard_Boolean explore,
                                              const Standard_Boolean compound) const;

  Standard_EXPORT TopoDS_Shape ShapeValue(const Handle(TopTools_HSequenceOfShape)& seqv,
                                          const Standard_Integer                   num) const;

  Standard_EXPORT Handle(TopTools_HSequenceOfShape) NewSeqShape() const;

  Standard_EXPORT void AppendShape(const Handle(TopTools_HSequenceOfShape)& seqv,
                                   const TopoDS_Shape&                      shape) const;

  //! Creates a Transient Object from a Shape : it is either a Binder
  //! (used by functions which require a Transient but can process
  //! a Shape, such as viewing functions) or a HShape (according to hs)
  //! Default is a HShape
  Standard_EXPORT Handle(Standard_Transient) ShapeBinder(
    const TopoDS_Shape&    shape,
    const Standard_Boolean hs = Standard_True) const;

  //! From a Transient, returns a Shape.
  //! In fact, recognizes ShapeBinder ShapeMapper and HShape
  Standard_EXPORT TopoDS_Shape BinderShape(const Handle(Standard_Transient)& tr) const;

  Standard_EXPORT Standard_Integer SeqLength(const Handle(Standard_Transient)& list) const;

  Standard_EXPORT Handle(Standard_Transient) SeqToArr(const Handle(Standard_Transient)& seq,
                                                      const Standard_Integer first = 1) const;

  Standard_EXPORT Handle(Standard_Transient) ArrToSeq(const Handle(Standard_Transient)& arr) const;

  Standard_EXPORT Standard_Integer SeqIntValue(const Handle(TColStd_HSequenceOfInteger)& list,
                                               const Standard_Integer                    num) const;

protected:
private:
};

#endif // _XSControl_Utils_HeaderFile
