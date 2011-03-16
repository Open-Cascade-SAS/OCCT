// File:	StepFEA_DegreeOfFreedomMember.cxx
// Created:	Tue Dec 10 18:12:57 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

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
