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


#include <Standard_Type.hxx>
#include <StepData_EnumTool.hxx>
#include <StepVisual_MarkerMember.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_MarkerMember,StepData_SelectInt)

static StepData_EnumTool tool
  (".DOT.",".X.",".PLUS.",".ASTERISK.",".RING.",".SQUARE.",".TRIANGLE.");

    StepVisual_MarkerMember::StepVisual_MarkerMember ()   {  }

    Standard_Boolean  StepVisual_MarkerMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::Name    () const
      {  return "MARKER_TYPE";  }

    Standard_Boolean  StepVisual_MarkerMember::SetName (const Standard_CString /*name*/)
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::EnumText () const
      {  return tool.Text(Int()).ToCString();  }

void  StepVisual_MarkerMember::SetEnumText (const Standard_Integer /*val*/, 
                                            const Standard_CString text)
{
    Standard_Integer vl = tool.Value (text);
    if (vl >= 0) SetInt (vl);
}

void  StepVisual_MarkerMember::SetValue (const StepVisual_MarkerType val)
{
    SetInt ( Standard_Integer (val) );  
}

StepVisual_MarkerType  StepVisual_MarkerMember::Value () const
{
    return StepVisual_MarkerType (Int());  
}
