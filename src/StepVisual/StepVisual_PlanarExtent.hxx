// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _StepVisual_PlanarExtent_HeaderFile
#define _StepVisual_PlanarExtent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;

class StepVisual_PlanarExtent;
DEFINE_STANDARD_HANDLE(StepVisual_PlanarExtent, StepGeom_GeometricRepresentationItem)

class StepVisual_PlanarExtent : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a PlanarExtent
  Standard_EXPORT StepVisual_PlanarExtent();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Standard_Real                     aSizeInX,
                            const Standard_Real                     aSizeInY);

  Standard_EXPORT void SetSizeInX(const Standard_Real aSizeInX);

  Standard_EXPORT Standard_Real SizeInX() const;

  Standard_EXPORT void SetSizeInY(const Standard_Real aSizeInY);

  Standard_EXPORT Standard_Real SizeInY() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_PlanarExtent, StepGeom_GeometricRepresentationItem)

protected:
private:
  Standard_Real sizeInX;
  Standard_Real sizeInY;
};

#endif // _StepVisual_PlanarExtent_HeaderFile
