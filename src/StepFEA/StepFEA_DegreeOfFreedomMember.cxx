// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepFEA_DegreeOfFreedomMember.ixx>
#include <TCollection_HAsciiString.hxx>

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

static Standard_Integer CompareNames(const Standard_CString name,Standard_Integer &numen) 
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
