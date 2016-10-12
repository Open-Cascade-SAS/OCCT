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


#include <Interface_InterfaceError.hxx>
#include <Interface_TypedValue.hxx>
#include <OSD_Path.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(Interface_TypedValue,MoniTool_TypedValue)

//static  char defmess[30]; svv #2
//  Fonctions Satisfies offertes en standard ...
// svv#2
//static Standard_Boolean StaticPath(const Handle(TCollection_HAsciiString)& val)
//{
//  OSD_Path apath;
//  return apath.IsValid (TCollection_AsciiString(val->ToCString()));
//}
Interface_TypedValue::Interface_TypedValue
  (const Standard_CString name,
   const Interface_ParamType type, const Standard_CString init)
    : MoniTool_TypedValue (name,
			   Interface_TypedValue::ParamTypeToValueType(type),
			   init)    {  }

    Interface_ParamType  Interface_TypedValue::Type () const
      {  return ValueTypeToParamType (ValueType());  }

    MoniTool_ValueType  Interface_TypedValue::ParamTypeToValueType
  (const Interface_ParamType type)
      {  return (MoniTool_ValueType) type;  }  // meme valeurs

    Interface_ParamType  Interface_TypedValue::ValueTypeToParamType
  (const MoniTool_ValueType type)
      {  return (Interface_ParamType) type;  }  // meme valeurs
