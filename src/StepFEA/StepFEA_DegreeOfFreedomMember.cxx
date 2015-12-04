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
#include <StepFEA_DegreeOfFreedomMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_DegreeOfFreedomMember,StepData_SelectNamed)

static Standard_CString anEnumeratedCurveElementFreedom = "ENUMERATED_DEGREE_OF_FREEDOM";
static Standard_CString anApplicationDefinedDegreeOfFreedom ="APPLICATION_DEFINED_DEGREE_OF_FREEDOM";

//=======================================================================
//function : StepFEA_DegreeOfFreedomMember
//purpose  : 
//=======================================================================

StepFEA_DegreeOfFreedomMember::StepFEA_DegreeOfFreedomMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_DegreeOfFreedomMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepFEA_DegreeOfFreedomMember::Name() const
{
  switch(mycase)  {
    case 1  : return anEnumeratedCurveElementFreedom;
    case 2  : return anApplicationDefinedDegreeOfFreedom;
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
   else if(!strcmp (name,anEnumeratedCurveElementFreedom)) {
     thecase = 1;
   }
   else if(!strcmp (name,anApplicationDefinedDegreeOfFreedom)) { 
     thecase = 2;
   }
  /*if (!name || name[0] == '\0') thecase = 0;
   else if(!strcmp (name,"XTranslation")) { 
     thecase = 1;
     numen =  1;
   }
   else if(!strcmp (name,"YTranslation")) { 
     thecase = 1;
     numen =  2;
   }
   else if(!strcmp (name,"ZTranslation")) { 
     thecase = 1;
     numen =  3;
   }
   else if(!strcmp (name,"XRotation")) { 
     thecase = 1;
     numen =  4;
   }
   else if(!strcmp (name,"YRotation")) { 
     thecase = 1;
     numen =  5;
   }
   else if(!strcmp (name,"ZRotation")) { 
     thecase = 1;
     numen =  6;
   }
   else if(!strcmp (name,"Warp")) { 
     thecase = 1;
     numen =  7;
   }
   else if(!strcmp (name,"None")) { 
     thecase = 1;
     numen =  8;
   }
   else if(!strcmp (name,"ApplicationDefinedDegreeOfFreedom")) thecase = 2;*/
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_DegreeOfFreedomMember::SetName(const Standard_CString name) 
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

Standard_Boolean StepFEA_DegreeOfFreedomMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit =0;
  Standard_Integer thecase =CompareNames(name,numit);
  return (mycase==thecase);
}
