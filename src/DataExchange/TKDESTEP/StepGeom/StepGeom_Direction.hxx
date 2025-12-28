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

#ifndef _StepGeom_Direction_HeaderFile
#define _StepGeom_Direction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

#include <array>

class TCollection_HAsciiString;

class StepGeom_Direction : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a Direction
  Standard_EXPORT StepGeom_Direction();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&    theName,
                            const occ::handle<NCollection_HArray1<double>>& theDirectionRatios);

  Standard_EXPORT void Init3D(const occ::handle<TCollection_HAsciiString>& theName,
                              const double                                 theDirectionRatios1,
                              const double                                 theDirectionRatios2,
                              const double                                 theDirectionRatios3);

  Standard_EXPORT void Init2D(const occ::handle<TCollection_HAsciiString>& theName,
                              const double                                 theDirectionRatios1,
                              const double                                 theDirectionRatios2);

  Standard_EXPORT void SetDirectionRatios(
    const occ::handle<NCollection_HArray1<double>>& theDirectionRatios);

  Standard_EXPORT void SetDirectionRatios(const std::array<double, 3>& theDirectionRatios);

  Standard_EXPORT const std::array<double, 3>& DirectionRatios() const;

  Standard_EXPORT double DirectionRatiosValue(const int theInd) const;

  Standard_EXPORT void SetNbDirectionRatios(const int theSize);

  Standard_EXPORT int NbDirectionRatios() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_Direction, StepGeom_GeometricRepresentationItem)

private:
  int                   myNbCoord;
  std::array<double, 3> myCoords;
};

#endif // _StepGeom_Direction_HeaderFile
