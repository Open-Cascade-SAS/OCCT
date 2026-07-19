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

#ifndef _StepVisual_TessellatedEdge_HeaderFile_
#define _StepVisual_TessellatedEdge_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>

#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_EdgeOrCurve.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//! Representation of STEP entity TessellatedEdge
class StepVisual_TessellatedEdge : public StepVisual_TessellatedStructuredItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedEdge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
                            const occ::handle<StepVisual_CoordinatesList>& theCoordinates,
                            const bool                                     theHasGeometricLink,
                            const StepVisual_EdgeOrCurve&                  theGeometricLink,
                            const occ::handle<NCollection_HArray1<int>>&   theLineStrip);

  //! Returns field Coordinates
  Standard_EXPORT occ::handle<StepVisual_CoordinatesList> Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(
    const occ::handle<StepVisual_CoordinatesList>& theCoordinates);

  //! Returns field GeometricLink
  Standard_EXPORT StepVisual_EdgeOrCurve GeometricLink() const;

  //! Sets field GeometricLink
  Standard_EXPORT void SetGeometricLink(const StepVisual_EdgeOrCurve& theGeometricLink);

  //! Returns True if optional field GeometricLink is defined
  Standard_EXPORT bool HasGeometricLink() const;

  //! Returns field LineStrip
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> LineStrip() const;

  //! Sets field LineStrip
  Standard_EXPORT void SetLineStrip(const occ::handle<NCollection_HArray1<int>>& theLineStrip);

  //! Returns number of LineStrip
  Standard_EXPORT int NbLineStrip() const;

  //! Returns value of LineStrip by its num
  Standard_EXPORT int LineStripValue(const int theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedEdge, StepVisual_TessellatedStructuredItem)

private:
  occ::handle<StepVisual_CoordinatesList> myCoordinates;
  StepVisual_EdgeOrCurve                  myGeometricLink; //!< optional
  occ::handle<NCollection_HArray1<int>>   myLineStrip;
  bool                                    myHasGeometricLink; //!< flag "is GeometricLink defined"
};

#endif // _StepVisual_TessellatedEdge_HeaderFile_
