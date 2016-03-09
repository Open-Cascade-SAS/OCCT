// Created on: 2015-07-16
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepVisual_NullStyleMember.hxx>
#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_NullStyleMember,StepData_SelectInt)

static StepData_EnumTool tool
  (".NULL.");

//=======================================================================
//function : StepVisual_NullStyleMember
//purpose  : 
//=======================================================================

StepVisual_NullStyleMember::StepVisual_NullStyleMember ()   {  }

//=======================================================================
//function : EnumText
//purpose  : 
//=======================================================================

Standard_CString  StepVisual_NullStyleMember::EnumText () const
{
  return tool.Text(Int()).ToCString();
}

//=======================================================================
//function : SetEnumText
//purpose  : 
//=======================================================================

void  StepVisual_NullStyleMember::SetEnumText (const Standard_Integer /*theValue*/, 
                                                                  const Standard_CString theText)
{
  Standard_Integer aVal = tool.Value (theText);
  if (aVal >= 0) SetInt (aVal);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void  StepVisual_NullStyleMember::SetValue (const StepVisual_NullStyle theValue)
{
  SetInt ( Standard_Integer (theValue) );  
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

StepVisual_NullStyle StepVisual_NullStyleMember::Value () const
{
  return StepVisual_NullStyle (Int());  
}
