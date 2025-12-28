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

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

//! Representation of STEP entity TriangulatedFace
class StepVisual_TriangulatedFace : public StepVisual_TessellatedFace
{

public:
  //! Default constructor
  Standard_EXPORT StepVisual_TriangulatedFace();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&    theRepresentationItem_Name,
    const occ::handle<StepVisual_CoordinatesList>&  theTessellatedFace_Coordinates,
    const int                                       theTessellatedFace_Pnmax,
    const occ::handle<NCollection_HArray2<double>>& theTessellatedFace_Normals,
    const bool                                      theHasTessellatedFace_GeometricLink,
    const StepVisual_FaceOrSurface&                 theTessellatedFace_GeometricLink,
    const occ::handle<NCollection_HArray1<int>>&    thePnindex,
    const occ::handle<NCollection_HArray2<int>>&    theTriangles);

  //! Returns field Pnindex
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> Pnindex() const;

  //! Sets field Pnindex
  Standard_EXPORT void SetPnindex(const occ::handle<NCollection_HArray1<int>>& thePnindex);

  //! Returns number of Pnindex
  Standard_EXPORT int NbPnindex() const;

  //! Returns value of Pnindex by its num
  Standard_EXPORT int PnindexValue(const int theNum) const;

  //! Returns field Triangles
  Standard_EXPORT occ::handle<NCollection_HArray2<int>> Triangles() const;

  //! Sets field Triangles
  Standard_EXPORT void SetTriangles(const occ::handle<NCollection_HArray2<int>>& theTriangles);

  //! Returns number of Triangles
  Standard_EXPORT int NbTriangles() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TriangulatedFace, StepVisual_TessellatedFace)

private:
  occ::handle<NCollection_HArray1<int>> myPnindex;
  occ::handle<NCollection_HArray2<int>> myTriangles;
};

#endif // _StepVisual_TriangulatedFace_HeaderFile_
