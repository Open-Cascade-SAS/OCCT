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

#ifndef _StepVisual_TessellatedSurfaceSet_HeaderFile_
#define _StepVisual_TessellatedSurfaceSet_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedItem.hxx>

#include <StepVisual_CoordinatesList.hxx>
#include <TColStd_HArray2OfReal.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedSurfaceSet, StepVisual_TessellatedItem)

//! Representation of STEP entity TessellatedSurfaceSet
class StepVisual_TessellatedSurfaceSet : public StepVisual_TessellatedItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedSurfaceSet();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&   theRepresentationItem_Name,
                            const Handle(StepVisual_CoordinatesList)& theCoordinates,
                            const Standard_Integer                    thePnmax,
                            const Handle(TColStd_HArray2OfReal)&      theNormals);

  //! Returns field Coordinates
  Standard_EXPORT Handle(StepVisual_CoordinatesList) Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(const Handle(StepVisual_CoordinatesList)& theCoordinates);

  //! Returns field Pnmax
  Standard_EXPORT Standard_Integer Pnmax() const;

  //! Sets field Pnmax
  Standard_EXPORT void SetPnmax(const Standard_Integer thePnmax);

  //! Returns field Normals
  Standard_EXPORT Handle(TColStd_HArray2OfReal) Normals() const;

  //! Sets field Normals
  Standard_EXPORT void SetNormals(const Handle(TColStd_HArray2OfReal)& theNormals);

  //! Returns number of Normals
  Standard_EXPORT Standard_Integer NbNormals() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedSurfaceSet, StepVisual_TessellatedItem)

private:
  Handle(StepVisual_CoordinatesList) myCoordinates;
  Standard_Integer                   myPnmax;
  Handle(TColStd_HArray2OfReal)      myNormals;
};

#endif // _StepVisual_TessellatedSurfaceSet_HeaderFile_
