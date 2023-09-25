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

#include <StepVisual_ComplexTriangulatedFace.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepVisual_ComplexTriangulatedFace, StepVisual_TessellatedFace)

//=======================================================================
//function : StepVisual_ComplexTriangulatedFace
//purpose  : 
//=======================================================================

StepVisual_ComplexTriangulatedFace::StepVisual_ComplexTriangulatedFace ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepVisual_ComplexTriangulatedFace::Init (const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                                               const Handle(StepVisual_CoordinatesList)& theTessellatedFace_Coordinates,
                                               const Standard_Integer theTessellatedFace_Pnmax,
                                               const Handle(TColStd_HArray2OfReal)& theTessellatedFace_Normals,
                                               const Standard_Boolean theHasTessellatedFace_GeometricLink,
                                               const StepVisual_FaceOrSurface& theTessellatedFace_GeometricLink,
                                               const Handle(TColStd_HArray1OfInteger)& thePnindex,
                                               const Handle(TColStd_HArray1OfTransient)& theTriangleStrips,
                                               const Handle(TColStd_HArray1OfTransient)& theTriangleFans)
{
  StepVisual_TessellatedFace::Init(theRepresentationItem_Name,
                                   theTessellatedFace_Coordinates,
                                   theTessellatedFace_Pnmax,
                                   theTessellatedFace_Normals,
                                   theHasTessellatedFace_GeometricLink,
                                   theTessellatedFace_GeometricLink);

  myPnindex = thePnindex;

  myTriangleStrips = theTriangleStrips;

  myTriangleFans = theTriangleFans;
}

//=======================================================================
//function : Pnindex
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfInteger) StepVisual_ComplexTriangulatedFace::Pnindex () const
{
  return myPnindex;
}

//=======================================================================
//function : SetPnindex
//purpose  : 
//=======================================================================

void StepVisual_ComplexTriangulatedFace::SetPnindex(const Handle(TColStd_HArray1OfInteger)& thePnindex)
{
  myPnindex = thePnindex;
}


//=======================================================================
//function : NbPnindex
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_ComplexTriangulatedFace::NbPnindex() const
{
  if (myPnindex.IsNull())
  {
    return 0;
  }
  return myPnindex->Length();
}


//=======================================================================
//function : PnindexValue
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_ComplexTriangulatedFace::PnindexValue(const Standard_Integer theNum) const
{
  return myPnindex->Value(theNum);
}

//=======================================================================
//function : TriangleStrips
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfTransient) StepVisual_ComplexTriangulatedFace::TriangleStrips () const
{
  return myTriangleStrips;
}

//=======================================================================
//function : SetTriangleStrips
//purpose  : 
//=======================================================================

void StepVisual_ComplexTriangulatedFace::SetTriangleStrips(const Handle(TColStd_HArray1OfTransient)& theTriangleStrips)
{
  myTriangleStrips = theTriangleStrips;
}

//=======================================================================
//function : NbTriangleStrips
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_ComplexTriangulatedFace::NbTriangleStrips() const
{
  if (myTriangleStrips.IsNull())
  {
    return 0;
  }
  return myTriangleStrips->Length();
}

//=======================================================================
//function : TriangleFans
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfTransient) StepVisual_ComplexTriangulatedFace::TriangleFans () const
{
  return myTriangleFans;
}

//=======================================================================
//function : SetTriangleFans
//purpose  : 
//=======================================================================

void StepVisual_ComplexTriangulatedFace::SetTriangleFans(const Handle(TColStd_HArray1OfTransient)& theTriangleFans)
{
  myTriangleFans = theTriangleFans;
}

//=======================================================================
//function : NbTriangleFans
//purpose  : 
//=======================================================================

Standard_Integer StepVisual_ComplexTriangulatedFace::NbTriangleFans() const
{
  if (myTriangleFans.IsNull())
  {
    return 0;
  }
  return myTriangleFans->Length();
}
