// File:	StepElement_MeasureOrUnspecifiedValueMember.cxx
// Created:	Tue Dec 10 18:12:58 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

#include <StepElement_MeasureOrUnspecifiedValueMember.ixx>
#include <TCollection_HAsciiString.hxx>


static Standard_CString aContextDependedMeasure = "CONTEXT_DEPENDENT_MEASURE";
static Standard_CString anUnspecifiedValue = "UNSPECIFIED_VALUE";

//=======================================================================
//function : StepElement_MeasureOrUnspecifiedValueMember
//purpose  : 
//=======================================================================

StepElement_MeasureOrUnspecifiedValueMember::StepElement_MeasureOrUnspecifiedValueMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_MeasureOrUnspecifiedValueMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepElement_MeasureOrUnspecifiedValueMember::Name() const
{
  switch(mycase)  {
    case 1  : return aContextDependedMeasure;
    case 2  : return anUnspecifiedValue;
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
   else if(!strcmp (name,aContextDependedMeasure)) {
     thecase = 1;
   }
   else if(!strcmp (name,anUnspecifiedValue)) { 
     thecase = 2;
   }
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_MeasureOrUnspecifiedValueMember::SetName(const Standard_CString name) 
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

Standard_Boolean StepElement_MeasureOrUnspecifiedValueMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit =0;
  Standard_Integer thecase = CompareNames(name,numit);
  return (mycase==thecase);
}
