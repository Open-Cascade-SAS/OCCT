// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

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
