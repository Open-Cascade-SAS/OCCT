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
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <StepVisual_FaceOrSurface.hxx>

//! Representation of STEP entity TessellatedFace
class StepVisual_TessellatedFace : public StepVisual_TessellatedStructuredItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedFace();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
                            const occ::handle<StepVisual_CoordinatesList>&  theCoordinates,
                            const int                                       thePnmax,
                            const occ::handle<NCollection_HArray2<double>>& theNormals,
                            const bool                                      theHasGeometricLink,
                            const StepVisual_FaceOrSurface&                 theGeometricLink);

  //! Returns field Coordinates
  Standard_EXPORT occ::handle<StepVisual_CoordinatesList> Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(
    const occ::handle<StepVisual_CoordinatesList>& theCoordinates);

  //! Returns field Pnmax
  Standard_EXPORT int Pnmax() const;

  //! Sets field Pnmax
  Standard_EXPORT void SetPnmax(const int thePnmax);

  //! Returns field Normals
  Standard_EXPORT occ::handle<NCollection_HArray2<double>> Normals() const;

  //! Sets field Normals
  Standard_EXPORT void SetNormals(const occ::handle<NCollection_HArray2<double>>& theNormals);

  //! Returns number of Normals
  Standard_EXPORT int NbNormals() const;

  //! Returns field GeometricLink
  Standard_EXPORT StepVisual_FaceOrSurface GeometricLink() const;

  //! Sets field GeometricLink
  Standard_EXPORT void SetGeometricLink(const StepVisual_FaceOrSurface& theGeometricLink);

  //! Returns True if optional field GeometricLink is defined
  Standard_EXPORT bool HasGeometricLink() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedFace, StepVisual_TessellatedStructuredItem)

private:
  occ::handle<StepVisual_CoordinatesList>  myCoordinates;
  int                                      myPnmax;
  occ::handle<NCollection_HArray2<double>> myNormals;
  StepVisual_FaceOrSurface                 myGeometricLink;    //!< optional
  bool                                     myHasGeometricLink; //!< flag "is GeometricLink defined"
};

#endif // _StepVisual_TessellatedFace_HeaderFile_
