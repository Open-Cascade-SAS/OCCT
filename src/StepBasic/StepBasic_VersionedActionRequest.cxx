// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

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
