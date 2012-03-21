// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepVisual_MarkerMember.ixx>
#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

static StepData_EnumTool tool
  (".DOT.",".X.",".PLUS.",".ASTERISK.",".RING.",".SQUARE.",".TRIANGLE.");

    StepVisual_MarkerMember::StepVisual_MarkerMember ()   {  }

    Standard_Boolean  StepVisual_MarkerMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::Name    () const
      {  return "MARKER_TYPE";  }

    Standard_Boolean  StepVisual_MarkerMember::SetName (const Standard_CString name)
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::EnumText () const
      {  return tool.Text(Int()).ToCString();  }

    void  StepVisual_MarkerMember::SetEnumText
  (const Standard_Integer val, const Standard_CString text)
{
  Standard_Integer vl = tool.Value (text);
  if (vl >= 0) SetInt (vl);
}

    void  StepVisual_MarkerMember::SetValue (const StepVisual_MarkerType val)
      {  SetInt ( Standard_Integer (val) );  }

    StepVisual_MarkerType  StepVisual_MarkerMember::Value () const
      {  return StepVisual_MarkerType (Int());  }
