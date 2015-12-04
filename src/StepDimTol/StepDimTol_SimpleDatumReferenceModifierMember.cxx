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

#include <StepDimTol_SimpleDatumReferenceModifierMember.hxx>
#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_SimpleDatumReferenceModifierMember,StepData_SelectInt)

static StepData_EnumTool tool
  (".ANY_CROSS_SECTION.",
   ".ANY_LONGITUDINAL_SECTION.",
   ".BASIC.",
   ".CONTACTING_FEATURE.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_U.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_V.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_W.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_X.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_Y.",
   ".DEGREE_OF_FREEDOM_CONSTRAINT_Z.",
   ".DISTANCE_VARIABLE.",
   ".FREE_STATE.",
   ".LEAST_MATERIAL_REQUIREMENT.",
   ".LINE.",
   ".MAJOR_DIAMETER.",
   ".MAXIMUM_MATERIAL_REQUIREMENT.",
   ".MINOR_DIAMETER.",
   ".ORIENTATION.",
   ".PITCH_DIAMETER.",
   ".PLANE.",
   ".POINT.",
   ".TRANSLATION.");

//=======================================================================
//function : StepDimTol_SimpleDatumReferenceModifierMember
//purpose  : 
//=======================================================================

StepDimTol_SimpleDatumReferenceModifierMember::StepDimTol_SimpleDatumReferenceModifierMember ()   {  }

//=======================================================================
//function : EnumText
//purpose  : 
//=======================================================================

Standard_CString  StepDimTol_SimpleDatumReferenceModifierMember::EnumText () const
  {  return tool.Text(Int()).ToCString();  }

//=======================================================================
//function : SetEnumText
//purpose  : 
//=======================================================================

void  StepDimTol_SimpleDatumReferenceModifierMember::SetEnumText (const Standard_Integer /*theValue*/, 
                                                                  const Standard_CString theText)
{
    Standard_Integer aVal = tool.Value (theText);
    if (aVal >= 0) SetInt (aVal);
}

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void  StepDimTol_SimpleDatumReferenceModifierMember::SetValue (const StepDimTol_SimpleDatumReferenceModifier theValue)
{
    SetInt ( Standard_Integer (theValue) );  
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

StepDimTol_SimpleDatumReferenceModifier StepDimTol_SimpleDatumReferenceModifierMember::Value () const
{
    return StepDimTol_SimpleDatumReferenceModifier (Int());  
}


