// File:	StepRepr_ProductConcept.cxx
// Created:	Fri Nov 26 16:26:38 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepRepr_ProductConcept.ixx>

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
