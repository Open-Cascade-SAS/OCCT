// File:	StepElement_ElementDescriptor.cxx
// Created:	Thu Dec 12 17:29:01 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_ElementDescriptor.ixx>

//=======================================================================
//function : StepElement_ElementDescriptor
//purpose  : 
//=======================================================================

StepElement_ElementDescriptor::StepElement_ElementDescriptor ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_ElementDescriptor::Init (const StepElement_ElementOrder aTopologyOrder,
                                          const Handle(TCollection_HAsciiString) &aDescription)
{

  theTopologyOrder = aTopologyOrder;

  theDescription = aDescription;
}

//=======================================================================
//function : TopologyOrder
//purpose  : 
//=======================================================================

StepElement_ElementOrder StepElement_ElementDescriptor::TopologyOrder () const
{
  return theTopologyOrder;
}

//=======================================================================
//function : SetTopologyOrder
//purpose  : 
//=======================================================================

void StepElement_ElementDescriptor::SetTopologyOrder (const StepElement_ElementOrder aTopologyOrder)
{
  theTopologyOrder = aTopologyOrder;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_ElementDescriptor::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepElement_ElementDescriptor::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}
