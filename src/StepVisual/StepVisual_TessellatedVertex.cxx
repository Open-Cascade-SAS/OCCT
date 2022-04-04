// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#include <StepVisual_TessellatedVertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_TessellatedVertex, StepVisual_TessellatedStructuredItem)

//=======================================================================
//function : StepVisual_TessellatedVertex
//purpose  : 
//=======================================================================

StepVisual_TessellatedVertex::StepVisual_TessellatedVertex ()
{
  myPointIndex = 0;
  myHasTopologicalLink = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepVisual_TessellatedVertex::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                         const Handle(StepVisual_CoordinatesList)& theCoordinates,
                                         const Standard_Boolean theHasTopologicalLink,
                                         const Handle(StepShape_VertexPoint)& theTopologicalLink,
                                         const Standard_Integer thePointIndex)
{
  StepVisual_TessellatedStructuredItem::Init(theRepresentationItem_Name);

  myCoordinates = theCoordinates;

  myHasTopologicalLink = theHasTopologicalLink;
  if (myHasTopologicalLink)
  {
    myTopologicalLink = theTopologicalLink;
  }
  else
  {
    myTopologicalLink.Nullify();
  }

  myPointIndex = thePointIndex;
}

//=======================================================================
//function : Coordinates
//purpose  : 
//=======================================================================

Handle(StepVisual_CoordinatesList) StepVisual_TessellatedVertex::Coordinates () const
{
  return myCoordinates;
}

//=======================================================================
//function : SetCoordinates
//purpose  : 
//=======================================================================

void StepVisual_TessellatedVertex::SetCoordinates(const Handle(StepVisual_CoordinatesList)& theCoordinates)
{
  myCoordinates = theCoordinates;
}

//=======================================================================
//function : TopologicalLink
//purpose  : 
//=======================================================================

Handle(StepShape_VertexPoint) StepVisual_TessellatedVertex::TopologicalLink () const
{
  return myTopologicalLink;
}

//=======================================================================
//function : SetTopologicalLink
//purpose  : 
//=======================================================================

void StepVisual_TessellatedVertex::SetTopologicalLink(const Handle(StepShape_VertexPoint)& theTopologicalLink)
{
  myTopologicalLink = theTopologicalLink;
}

//=======================================================================
//function : HasTopologicalLink
//purpose  : 
//=======================================================================

Standard_Boolean StepVisual_TessellatedVertex::HasTopologicalLink () const
{
  return myHasTopologicalLink;
}

//=======================================================================
//function : PointIndex
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_TessellatedVertex::PointIndex () const
{
  return myPointIndex;
}

//=======================================================================
//function : SetPointIndex
//purpose  : 
//=======================================================================

void StepVisual_TessellatedVertex::SetPointIndex(const Standard_Integer thePointIndex)
{
  myPointIndex = thePointIndex;
}
