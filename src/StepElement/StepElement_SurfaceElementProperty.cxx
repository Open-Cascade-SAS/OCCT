// File:	StepElement_SurfaceElementProperty.cxx
// Created:	Thu Dec 12 17:29:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_SurfaceElementProperty.ixx>

//=======================================================================
//function : StepElement_SurfaceElementProperty
//purpose  : 
//=======================================================================

StepElement_SurfaceElementProperty::StepElement_SurfaceElementProperty ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_SurfaceElementProperty::Init (const Handle(TCollection_HAsciiString) &aPropertyId,
                                               const Handle(TCollection_HAsciiString) &aDescription,
                                               const Handle(StepElement_SurfaceSectionField) &aSection)
{

  thePropertyId = aPropertyId;

  theDescription = aDescription;

  theSection = aSection;
}

//=======================================================================
//function : PropertyId
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_SurfaceElementProperty::PropertyId () const
{
  return thePropertyId;
}

//=======================================================================
//function : SetPropertyId
//purpose  : 
//=======================================================================

void StepElement_SurfaceElementProperty::SetPropertyId (const Handle(TCollection_HAsciiString) &aPropertyId)
{
  thePropertyId = aPropertyId;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_SurfaceElementProperty::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepElement_SurfaceElementProperty::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Handle(StepElement_SurfaceSectionField) StepElement_SurfaceElementProperty::Section () const
{
  return theSection;
}

//=======================================================================
//function : SetSection
//purpose  : 
//=======================================================================

void StepElement_SurfaceElementProperty::SetSection (const Handle(StepElement_SurfaceSectionField) &aSection)
{
  theSection = aSection;
}
