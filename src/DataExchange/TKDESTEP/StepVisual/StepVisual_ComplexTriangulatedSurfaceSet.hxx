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

#ifndef _StepVisual_ComplexTriangulatedSurfaceSet_HeaderFile_
#define _StepVisual_ComplexTriangulatedSurfaceSet_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedSurfaceSet.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>

//! Representation of STEP entity ComplexTriangulatedSurfaceSet
class StepVisual_ComplexTriangulatedSurfaceSet : public StepVisual_TessellatedSurfaceSet
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_ComplexTriangulatedSurfaceSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>&   theRepresentationItem_Name,
    const occ::handle<StepVisual_CoordinatesList>& theTessellatedSurfaceSet_Coordinates,
    const int                    theTessellatedSurfaceSet_Pnmax,
    const occ::handle<TColStd_HArray2OfReal>&      theTessellatedSurfaceSet_Normals,
    const occ::handle<TColStd_HArray1OfInteger>&   thePnindex,
    const occ::handle<TColStd_HArray1OfTransient>& theTriangleStrips,
    const occ::handle<TColStd_HArray1OfTransient>& theTriangleFans);

  //! Returns field Pnindex
  Standard_EXPORT occ::handle<TColStd_HArray1OfInteger> Pnindex() const;

  //! Sets field Pnindex
  Standard_EXPORT void SetPnindex(const occ::handle<TColStd_HArray1OfInteger>& thePnindex);

  //! Returns number of Pnindex
  Standard_EXPORT int NbPnindex() const;

  //! Returns value of Pnindex by its num
  Standard_EXPORT int PnindexValue(const int theNum) const;

  //! Returns field TriangleStrips
  Standard_EXPORT occ::handle<TColStd_HArray1OfTransient> TriangleStrips() const;

  //! Sets field TriangleStrips
  Standard_EXPORT void SetTriangleStrips(
    const occ::handle<TColStd_HArray1OfTransient>& theTriangleStrips);

  //! Returns number of TriangleStrips
  Standard_EXPORT int NbTriangleStrips() const;

  //! Returns field TriangleFans
  Standard_EXPORT occ::handle<TColStd_HArray1OfTransient> TriangleFans() const;

  //! Sets field TriangleFans
  Standard_EXPORT void SetTriangleFans(const occ::handle<TColStd_HArray1OfTransient>& theTriangleFans);

  //! Returns number of TriangleFans
  Standard_EXPORT int NbTriangleFans() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ComplexTriangulatedSurfaceSet,
                          StepVisual_TessellatedSurfaceSet)

private:
  occ::handle<TColStd_HArray1OfInteger>   myPnindex;
  occ::handle<TColStd_HArray1OfTransient> myTriangleStrips;
  occ::handle<TColStd_HArray1OfTransient> myTriangleFans;
};

#endif // _StepVisual_ComplexTriangulatedSurfaceSet_HeaderFile_
