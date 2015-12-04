// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <Standard_Type.hxx>
#include <StepElement_CurveElementFreedomMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_CurveElementFreedomMember,StepData_SelectNamed)

static Standard_CString ECEF = "ENUMERATED_CURVE_ELEMENT_FREEDOM";
static Standard_CString ADDF = "APPLICATION_DEFINED_DEGREE_OF_FREEDOM";

//=======================================================================
//function : StepElement_CurveElementFreedomMember
//purpose  : 
//=======================================================================

StepElement_CurveElementFreedomMember::StepElement_CurveElementFreedomMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_CurveElementFreedomMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepElement_CurveElementFreedomMember::Name() const
{
  switch(mycase)  {
    case 1  : return ECEF;
    case 2  : return ADDF;
    default : break;
  }
  return "";
}

//=======================================================================
//function : CompareNames
//purpose  : 
//=======================================================================

static Standard_Integer CompareNames(const Standard_CString name,Standard_Integer &/*numen*/) 
{
  Standard_Integer thecase =0;
  if (!name || name[0] == '\0') thecase = 0;
  
  else if(!strcmp (name,ECEF)){
    thecase = 1;
  }
  else if(!strcmp (name,ADDF)){
    thecase = 2;
  }
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_CurveElementFreedomMember::SetName(const Standard_CString name) 
{
  Standard_Integer numit =0;
  mycase = CompareNames(name,numit);
  if(numit) SetInteger(numit);
  return (mycase >0);
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_CurveElementFreedomMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit =0;
  Standard_Integer thecase = CompareNames(name,numit);
  return (mycase==thecase);
}
