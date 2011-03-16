// File:	StepElement_VolumeElementPurposeMember.cxx
// Created:	Tue Dec 10 18:13:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

#include <StepElement_VolumeElementPurposeMember.ixx>
#include <TCollection_HAsciiString.hxx>

static Standard_CString EVEP = "ENUMERATED_VOLUME_ELEMENT_PURPOSE";
static Standard_CString ADEP = "APPLICATION_DEFINED_ELEMENT_PURPOSE";


//=======================================================================
//function : StepElement_VolumeElementPurposeMember
//purpose  : 
//=======================================================================

StepElement_VolumeElementPurposeMember::StepElement_VolumeElementPurposeMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_VolumeElementPurposeMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepElement_VolumeElementPurposeMember::Name() const
{
  switch(mycase)  {
    case 1  : return EVEP;
    case 2  : return ADEP;
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
   else if(!strcmp (name,EVEP)) { 
     thecase = 1;
   }
   else if(!strcmp (name,ADEP)) { 
     thecase = 1;
   }
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepElement_VolumeElementPurposeMember::SetName(const Standard_CString name) 
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

Standard_Boolean StepElement_VolumeElementPurposeMember::Matches(const Standard_CString name) const
{
  Standard_Integer numit =0;
  Standard_Integer thecase = CompareNames(name,numit);
  return (mycase==thecase);
}
