// Created on: 1998-02-23
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Interface_TypedValue_HeaderFile
#define _Interface_TypedValue_HeaderFile

#include <Standard.hxx>

#include <MoniTool_TypedValue.hxx>
#include <Interface_ParamType.hxx>
#include <MoniTool_ValueType.hxx>
class TCollection_HAsciiString;


class Interface_TypedValue;
DEFINE_STANDARD_HANDLE(Interface_TypedValue, MoniTool_TypedValue)

//! Now strictly equivalent to TypedValue from MoniTool,
//! except for ParamType which remains for compatibility reasons
//!
//! This class allows to dynamically manage .. typed values, i.e.
//! values which have an alphanumeric expression, but with
//! controls. Such as "must be an Integer" or "Enumerative Text"
//! etc
//!
//! Hence, a TypedValue brings a specification (type + constraints
//! if any) and a value. Its basic form is a string, it can be
//! specified as integer or real or enumerative string, then
//! queried as such.
//! Its string content, which is a Handle(HAsciiString) can be
//! shared by other data structures, hence gives a direct on line
//! access to its value.
class Interface_TypedValue : public MoniTool_TypedValue
{

public:

  
  //! Creates a TypedValue, with a name
  //!
  //! type gives the type of the parameter, default is free text
  //! Also available : Integer, Real, Enum, Entity (i.e. Object)
  //! More precise specifications, titles, can be given to the
  //! TypedValue once created
  //!
  //! init gives an initial value. If it is not given, the
  //! TypedValue begins as "not set", its value is empty
  Standard_EXPORT Interface_TypedValue(const Standard_CString name, const Interface_ParamType type = Interface_ParamText, const Standard_CString init = "");
  
  //! Returns the type
  //! I.E. calls ValueType then makes correspondence between
  //! ParamType from Interface (which remains for compatibility
  //! reasons) and ValueType from MoniTool
  Standard_EXPORT Interface_ParamType Type() const;
  
  //! Correspondence ParamType from Interface to ValueType from MoniTool
  Standard_EXPORT static MoniTool_ValueType ParamTypeToValueType (const Interface_ParamType typ);
  
  //! Correspondence ParamType from Interface to ValueType from MoniTool
  Standard_EXPORT static Interface_ParamType ValueTypeToParamType (const MoniTool_ValueType typ);




  DEFINE_STANDARD_RTTIEXT(Interface_TypedValue,MoniTool_TypedValue)

protected:




private:


  TCollection_AsciiString thename;
  TCollection_AsciiString thedef;
  TCollection_AsciiString thelabel;
  Handle(Standard_Type) theotyp;
  TCollection_AsciiString theunidef;
  Handle(TColStd_HArray1OfAsciiString) theenums;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> theeadds;
  TCollection_AsciiString thesatisn;
  Handle(TCollection_HAsciiString) thehval;
  Handle(Standard_Transient) theoval;


};







#endif // _Interface_TypedValue_HeaderFile
