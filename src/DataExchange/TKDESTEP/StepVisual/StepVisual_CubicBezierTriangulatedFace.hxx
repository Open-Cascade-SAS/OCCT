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

#ifndef _StepVisual_CubicBezierTriangulatedFace_HeaderFile_
#define _StepVisual_CubicBezierTriangulatedFace_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedFace.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//! Representation of STEP entity CubicBezierTriangulatedFace
class StepVisual_CubicBezierTriangulatedFace : public StepVisual_TessellatedFace
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_CubicBezierTriangulatedFace();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
    const occ::handle<StepVisual_CoordinatesList>& theTessellatedFace_Coordinates,
    const int                    theTessellatedFace_Pnmax,
    const occ::handle<NCollection_HArray2<double>>&      theTessellatedFace_Normals,
    const bool                    theHasTessellatedFace_GeometricLink,
    const StepVisual_FaceOrSurface&           theTessellatedFace_GeometricLink,
    const occ::handle<NCollection_HArray2<int>>&   theCtriangles);

  //! Returns field Ctriangles
  Standard_EXPORT occ::handle<NCollection_HArray2<int>> Ctriangles() const;

  //! Sets field Ctriangles
  Standard_EXPORT void SetCtriangles(const occ::handle<NCollection_HArray2<int>>& theCtriangles);

  //! Returns number of Ctriangles
  Standard_EXPORT int NbCtriangles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_CubicBezierTriangulatedFace, StepVisual_TessellatedFace)

private:
  occ::handle<NCollection_HArray2<int>> myCtriangles;
};

#endif // _StepVisual_CubicBezierTriangulatedFace_HeaderFile_
