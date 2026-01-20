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

#include <StepGeom_Direction.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_Direction, StepGeom_GeometricRepresentationItem)

StepGeom_Direction::StepGeom_Direction() {}

void StepGeom_Direction::Init(const occ::handle<TCollection_HAsciiString>& theName,
                              const occ::handle<NCollection_HArray1<double>>&    theDirectionRatios)
{
  // --- classe own fields ---
  SetDirectionRatios(theDirectionRatios);
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_Direction::Init3D(const occ::handle<TCollection_HAsciiString>& theName,
                                const double                     theDirectionRatios1,
                                const double                     theDirectionRatios2,
                                const double                     theDirectionRatios3)
{
  myNbCoord   = 3;
  myCoords[0] = theDirectionRatios1;
  myCoords[1] = theDirectionRatios2;
  myCoords[2] = theDirectionRatios3;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_Direction::Init2D(const occ::handle<TCollection_HAsciiString>& theName,
                                const double                     theDirectionRatios1,
                                const double                     theDirectionRatios2)
{
  myNbCoord   = 2;
  myCoords[0] = theDirectionRatios1;
  myCoords[1] = theDirectionRatios2;
  myCoords[2] = 0.0;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_Direction::SetDirectionRatios(const occ::handle<NCollection_HArray1<double>>& theDirectionRatios)
{
  myNbCoord = theDirectionRatios->Length();
  if (myNbCoord > 0)
    myCoords[0] = theDirectionRatios->Value(1);
  if (myNbCoord > 1)
    myCoords[1] = theDirectionRatios->Value(2);
  if (myNbCoord > 2)
    myCoords[2] = theDirectionRatios->Value(3);
}

void StepGeom_Direction::SetDirectionRatios(const std::array<double, 3>& theDirectionRatios)
{
  myCoords[0] = theDirectionRatios[0];
  myCoords[1] = theDirectionRatios[1];
  myCoords[2] = theDirectionRatios[2];
}

const std::array<double, 3>& StepGeom_Direction::DirectionRatios() const
{
  return myCoords;
}

double StepGeom_Direction::DirectionRatiosValue(const int theInd) const
{
  return myCoords[theInd - 1];
}

void StepGeom_Direction::SetNbDirectionRatios(const int theSize)
{
  myNbCoord = theSize;
}

int StepGeom_Direction::NbDirectionRatios() const
{
  return myNbCoord;
}
