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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <array>

class TCollection_HAsciiString;

class StepGeom_CartesianPoint : public StepGeom_Point
{

public:
  //! Returns a CartesianPoint
  Standard_EXPORT StepGeom_CartesianPoint();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theName,
                            const occ::handle<NCollection_HArray1<double>>&    theCoordinates);

  Standard_EXPORT void Init2D(const occ::handle<TCollection_HAsciiString>& theName,
                              const double                     theX,
                              const double                     theY);

  Standard_EXPORT void Init3D(const occ::handle<TCollection_HAsciiString>& theName,
                              const double                     theX,
                              const double                     theY,
                              const double                     theZ);

  Standard_EXPORT void SetCoordinates(const occ::handle<NCollection_HArray1<double>>& theCoordinates);

  Standard_EXPORT void SetCoordinates(const std::array<double, 3>& theCoordinates);

  Standard_EXPORT const std::array<double, 3>& Coordinates() const;

  Standard_EXPORT double CoordinatesValue(const int theInd) const;

  Standard_EXPORT void SetNbCoordinates(const int theSize);

  Standard_EXPORT int NbCoordinates() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CartesianPoint, StepGeom_Point)

private:
  int             myNbCoord;
  std::array<double, 3> myCoords;
};

#endif // _StepGeom_CartesianPoint_HeaderFile
