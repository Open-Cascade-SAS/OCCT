// File:	StepBasic_VersionedActionRequest.cxx
// Created:	Fri Nov 26 16:26:40 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_VersionedActionRequest.ixx>

//=======================================================================
//function : StepBasic_VersionedActionRequest
//purpose  : 
//=======================================================================

StepBasic_VersionedActionRequest::StepBasic_VersionedActionRequest ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_VersionedActionRequest::Init (const Handle(TCollection_HAsciiString) &aId,
                                             const Handle(TCollection_HAsciiString) &aVersion,
                                             const Handle(TCollection_HAsciiString) &aPurpose,
                                             const Standard_Boolean hasDescription,
                                             const Handle(TCollection_HAsciiString) &aDescription)
{

  theId = aId;

  theVersion = aVersion;

  thePurpose = aPurpose;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_VersionedActionRequest::Id () const
{
  return theId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepBasic_VersionedActionRequest::SetId (const Handle(TCollection_HAsciiString) &aId)
{
  theId = aId;
}

//=======================================================================
//function : Version
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_VersionedActionRequest::Version () const
{
  return theVersion;
}

//=======================================================================
//function : SetVersion
//purpose  : 
//=======================================================================

void StepBasic_VersionedActionRequest::SetVersion (const Handle(TCollection_HAsciiString) &aVersion)
{
  theVersion = aVersion;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_VersionedActionRequest::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepBasic_VersionedActionRequest::SetPurpose (const Handle(TCollection_HAsciiString) &aPurpose)
{
  thePurpose = aPurpose;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_VersionedActionRequest::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_VersionedActionRequest::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_VersionedActionRequest::HasDescription () const
{
  return defDescription;
}
