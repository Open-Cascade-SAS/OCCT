// Created on : Thu Mar 24 18:30:11 2022 
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

#include <StepVisual_CubicBezierTriangulatedFace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_CubicBezierTriangulatedFace, StepVisual_TessellatedFace)

//=======================================================================
//function : StepVisual_CubicBezierTriangulatedFace
//purpose  : 
//=======================================================================

StepVisual_CubicBezierTriangulatedFace::StepVisual_CubicBezierTriangulatedFace ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepVisual_CubicBezierTriangulatedFace::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                                   const Handle(StepVisual_CoordinatesList)& theTessellatedFace_Coordinates,
                                                   const Standard_Integer theTessellatedFace_Pnmax,
                                                   const Handle(TColStd_HArray2OfReal)& theTessellatedFace_Normals,
                                                   const Standard_Boolean theHasTessellatedFace_GeometricLink,
                                                   const StepVisual_FaceOrSurface& theTessellatedFace_GeometricLink,
                                                   const Handle(TColStd_HArray2OfInteger)& theCtriangles)
{
  StepVisual_TessellatedFace::Init(theRepresentationItem_Name,
                                   theTessellatedFace_Coordinates,
                                   theTessellatedFace_Pnmax,
                                   theTessellatedFace_Normals,
                                   theHasTessellatedFace_GeometricLink,
                                   theTessellatedFace_GeometricLink);

  myCtriangles = theCtriangles;
}

//=======================================================================
//function : Ctriangles
//purpose  : 
//=======================================================================

Handle(TColStd_HArray2OfInteger) StepVisual_CubicBezierTriangulatedFace::Ctriangles () const
{
  return myCtriangles;
}

//=======================================================================
//function : SetCtriangles
//purpose  : 
//=======================================================================

void StepVisual_CubicBezierTriangulatedFace::SetCtriangles(const Handle(TColStd_HArray2OfInteger)& theCtriangles)
{
  myCtriangles = theCtriangles;
}

Standard_Integer StepVisual_CubicBezierTriangulatedFace::NbCtriangles() const
{
  if (myCtriangles.IsNull())
  {
    return 0;
  }
  return myCtriangles->ColLength();
}