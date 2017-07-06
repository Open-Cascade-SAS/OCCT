// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepBasic_ProductConceptContext.hxx>
#include <StepRepr_ProductConcept.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ProductConcept,Standard_Transient)

//=======================================================================
//function : StepRepr_ProductConcept
//purpose  : 
//=======================================================================
StepRepr_ProductConcept::StepRepr_ProductConcept ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ProductConcept::Init (const Handle(TCollection_HAsciiString) &aId,
                                    const Handle(TCollection_HAsciiString) &aName,
                                    const Standard_Boolean hasDescription,
                                    const Handle(TCollection_HAsciiString) &aDescription,
                                    const Handle(StepBasic_ProductConceptContext) &aMarketContext)
{

  theId = aId;

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theMarketContext = aMarketContext;
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ProductConcept::Id () const
{
  return theId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepRepr_ProductConcept::SetId (const Handle(TCollection_HAsciiString) &aId)
{
  theId = aId;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ProductConcept::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_ProductConcept::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_ProductConcept::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_ProductConcept::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_ProductConcept::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : MarketContext
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductConceptContext) StepRepr_ProductConcept::MarketContext () const
{
  return theMarketContext;
}

//=======================================================================
//function : SetMarketContext
//purpose  : 
//=======================================================================

void StepRepr_ProductConcept::SetMarketContext (const Handle(StepBasic_ProductConceptContext) &aMarketContext)
{
  theMarketContext = aMarketContext;
}
