// File:	StepElement_CurveElementFreedomMember.cxx
// Created:	Tue Dec 10 18:12:57 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

#include <StepElement_CurveElementFreedomMember.ixx>
#include <TCollection_HAsciiString.hxx>

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

static Standard_Integer CompareNames(const Standard_CString name,Standard_Integer &numen) 
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
