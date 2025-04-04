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

#ifndef _StepGeom_CartesianPoint_HeaderFile
#define _StepGeom_CartesianPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepGeom_Point.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <array>

class TCollection_HAsciiString;

class StepGeom_CartesianPoint;
DEFINE_STANDARD_HANDLE(StepGeom_CartesianPoint, StepGeom_Point)

class StepGeom_CartesianPoint : public StepGeom_Point
{

public:
  //! Returns a CartesianPoint
  Standard_EXPORT StepGeom_CartesianPoint();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName,
                            const Handle(TColStd_HArray1OfReal)&    theCoordinates);

  Standard_EXPORT void Init2D(const Handle(TCollection_HAsciiString)& theName,
                              const Standard_Real                     theX,
                              const Standard_Real                     theY);

  Standard_EXPORT void Init3D(const Handle(TCollection_HAsciiString)& theName,
                              const Standard_Real                     theX,
                              const Standard_Real                     theY,
                              const Standard_Real                     theZ);

  Standard_EXPORT void SetCoordinates(const Handle(TColStd_HArray1OfReal)& theCoordinates);

  Standard_EXPORT void SetCoordinates(const std::array<Standard_Real, 3>& theCoordinates);

  Standard_EXPORT const std::array<Standard_Real, 3>& Coordinates() const;

  Standard_EXPORT Standard_Real CoordinatesValue(const Standard_Integer theInd) const;

  Standard_EXPORT void SetNbCoordinates(const Standard_Integer theSize);

  Standard_EXPORT Standard_Integer NbCoordinates() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CartesianPoint, StepGeom_Point)

private:
  Standard_Integer             myNbCoord;
  std::array<Standard_Real, 3> myCoords;
};

#endif // _StepGeom_CartesianPoint_HeaderFile
