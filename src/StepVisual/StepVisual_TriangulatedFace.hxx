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

#ifndef _StepVisual_TriangulatedFace_HeaderFile_
#define _StepVisual_TriangulatedFace_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedFace.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TriangulatedFace, StepVisual_TessellatedFace)

//! Representation of STEP entity TriangulatedFace
class StepVisual_TriangulatedFace : public StepVisual_TessellatedFace
{

public:
  //! Default constructor
  Standard_EXPORT StepVisual_TriangulatedFace();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
    const Handle(StepVisual_CoordinatesList)& theTessellatedFace_Coordinates,
    const Standard_Integer                    theTessellatedFace_Pnmax,
    const Handle(TColStd_HArray2OfReal)&      theTessellatedFace_Normals,
    const Standard_Boolean                    theHasTessellatedFace_GeometricLink,
    const StepVisual_FaceOrSurface&           theTessellatedFace_GeometricLink,
    const Handle(TColStd_HArray1OfInteger)&   thePnindex,
    const Handle(TColStd_HArray2OfInteger)&   theTriangles);

  //! Returns field Pnindex
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) Pnindex() const;

  //! Sets field Pnindex
  Standard_EXPORT void SetPnindex(const Handle(TColStd_HArray1OfInteger)& thePnindex);

  //! Returns number of Pnindex
  Standard_EXPORT Standard_Integer NbPnindex() const;

  //! Returns value of Pnindex by its num
  Standard_EXPORT Standard_Integer PnindexValue(const Standard_Integer theNum) const;

  //! Returns field Triangles
  Standard_EXPORT Handle(TColStd_HArray2OfInteger) Triangles() const;

  //! Sets field Triangles
  Standard_EXPORT void SetTriangles(const Handle(TColStd_HArray2OfInteger)& theTriangles);

  //! Returns number of Triangles
  Standard_EXPORT Standard_Integer NbTriangles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TriangulatedFace, StepVisual_TessellatedFace)

private:
  Handle(TColStd_HArray1OfInteger) myPnindex;
  Handle(TColStd_HArray2OfInteger) myTriangles;
};

#endif // _StepVisual_TriangulatedFace_HeaderFile_
