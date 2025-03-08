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
#include <TColStd_HArray1OfInteger.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedEdge, StepVisual_TessellatedStructuredItem)

//! Representation of STEP entity TessellatedEdge
class StepVisual_TessellatedEdge : public StepVisual_TessellatedStructuredItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedEdge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
                            const Handle(StepVisual_CoordinatesList)& theCoordinates,
                            const Standard_Boolean                    theHasGeometricLink,
                            const StepVisual_EdgeOrCurve&             theGeometricLink,
                            const Handle(TColStd_HArray1OfInteger)&   theLineStrip);

  //! Returns field Coordinates
  Standard_EXPORT Handle(StepVisual_CoordinatesList) Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(const Handle(StepVisual_CoordinatesList)& theCoordinates);

  //! Returns field GeometricLink
  Standard_EXPORT StepVisual_EdgeOrCurve GeometricLink() const;

  //! Sets field GeometricLink
  Standard_EXPORT void SetGeometricLink(const StepVisual_EdgeOrCurve& theGeometricLink);

  //! Returns True if optional field GeometricLink is defined
  Standard_EXPORT Standard_Boolean HasGeometricLink() const;

  //! Returns field LineStrip
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) LineStrip() const;

  //! Sets field LineStrip
  Standard_EXPORT void SetLineStrip(const Handle(TColStd_HArray1OfInteger)& theLineStrip);

  //! Returns number of LineStrip
  Standard_EXPORT Standard_Integer NbLineStrip() const;

  //! Returns value of LineStrip by its num
  Standard_EXPORT Standard_Integer LineStripValue(const Standard_Integer theNum) const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedEdge, StepVisual_TessellatedStructuredItem)

private:
  Handle(StepVisual_CoordinatesList) myCoordinates;
  StepVisual_EdgeOrCurve             myGeometricLink; //!< optional
  Handle(TColStd_HArray1OfInteger)   myLineStrip;
  Standard_Boolean                   myHasGeometricLink; //!< flag "is GeometricLink defined"
};

#endif // _StepVisual_TessellatedEdge_HeaderFile_
