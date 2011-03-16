// File:	StepElement_AnalysisItemWithinRepresentation.cxx
// Created:	Thu Dec 12 17:28:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_AnalysisItemWithinRepresentation.ixx>

//=======================================================================
//function : StepElement_AnalysisItemWithinRepresentation
//purpose  : 
//=======================================================================

StepElement_AnalysisItemWithinRepresentation::StepElement_AnalysisItemWithinRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_AnalysisItemWithinRepresentation::Init (const Handle(TCollection_HAsciiString) &aName,
                                                         const Handle(TCollection_HAsciiString) &aDescription,
                                                         const Handle(StepRepr_RepresentationItem) &aItem,
                                                         const Handle(StepRepr_Representation) &aRep)
{

  theName = aName;

  theDescription = aDescription;

  theItem = aItem;

  theRep = aRep;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_AnalysisItemWithinRepresentation::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepElement_AnalysisItemWithinRepresentation::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_AnalysisItemWithinRepresentation::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepElement_AnalysisItemWithinRepresentation::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationItem) StepElement_AnalysisItemWithinRepresentation::Item () const
{
  return theItem;
}

//=======================================================================
//function : SetItem
//purpose  : 
//=======================================================================

void StepElement_AnalysisItemWithinRepresentation::SetItem (const Handle(StepRepr_RepresentationItem) &aItem)
{
  theItem = aItem;
}

//=======================================================================
//function : Rep
//purpose  : 
//=======================================================================

Handle(StepRepr_Representation) StepElement_AnalysisItemWithinRepresentation::Rep () const
{
  return theRep;
}

//=======================================================================
//function : SetRep
//purpose  : 
//=======================================================================

void StepElement_AnalysisItemWithinRepresentation::SetRep (const Handle(StepRepr_Representation) &aRep)
{
  theRep = aRep;
}
