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

#ifndef _StepVisual_TessellatedFace_HeaderFile_
#define _StepVisual_TessellatedFace_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>

#include <StepVisual_CoordinatesList.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <StepVisual_FaceOrSurface.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedFace, StepVisual_TessellatedStructuredItem)

//! Representation of STEP entity TessellatedFace
class StepVisual_TessellatedFace : public StepVisual_TessellatedStructuredItem
{

public :

  //! default constructor
  Standard_EXPORT StepVisual_TessellatedFace();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                            const Handle(StepVisual_CoordinatesList)& theCoordinates,
                            const Standard_Integer thePnmax,
                            const Handle(TColStd_HArray2OfReal)& theNormals,
                            const Standard_Boolean theHasGeometricLink,
                            const StepVisual_FaceOrSurface& theGeometricLink);

  //! Returns field Coordinates
  Standard_EXPORT Handle(StepVisual_CoordinatesList) Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates (const Handle(StepVisual_CoordinatesList)& theCoordinates);

  //! Returns field Pnmax
  Standard_EXPORT Standard_Integer Pnmax() const;

  //! Sets field Pnmax
  Standard_EXPORT void SetPnmax (const Standard_Integer thePnmax);

  //! Returns field Normals
  Standard_EXPORT Handle(TColStd_HArray2OfReal) Normals() const;

  //! Sets field Normals
  Standard_EXPORT void SetNormals (const Handle(TColStd_HArray2OfReal)& theNormals);

  //! Returns number of Normals
  Standard_EXPORT Standard_Integer NbNormals() const;

  //! Returns field GeometricLink
  Standard_EXPORT StepVisual_FaceOrSurface GeometricLink() const;

  //! Sets field GeometricLink
  Standard_EXPORT void SetGeometricLink (const StepVisual_FaceOrSurface& theGeometricLink);

  //! Returns True if optional field GeometricLink is defined
  Standard_EXPORT Standard_Boolean HasGeometricLink() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedFace, StepVisual_TessellatedStructuredItem)

private:

  Handle(StepVisual_CoordinatesList) myCoordinates;
  Standard_Integer myPnmax;
  Handle(TColStd_HArray2OfReal) myNormals;
  StepVisual_FaceOrSurface myGeometricLink; //!< optional
  Standard_Boolean myHasGeometricLink; //!< flag "is GeometricLink defined"

};

#endif // _StepVisual_TessellatedFace_HeaderFile_
