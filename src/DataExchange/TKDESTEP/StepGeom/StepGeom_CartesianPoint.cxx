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

#include <Standard_Type.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepGeom_CartesianPoint, StepGeom_Point)

StepGeom_CartesianPoint::StepGeom_CartesianPoint() {}

void StepGeom_CartesianPoint::Init(const Handle(TCollection_HAsciiString)& theName,
                                   const Handle(TColStd_HArray1OfReal)&    aCoordinates)
{
  SetCoordinates(aCoordinates);
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_CartesianPoint::Init2D(const Handle(TCollection_HAsciiString)& theName,
                                     const Standard_Real                     theX,
                                     const Standard_Real                     theY)
{
  myNbCoord   = 2;
  myCoords[0] = theX;
  myCoords[1] = theY;
  myCoords[2] = 0;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_CartesianPoint::Init3D(const Handle(TCollection_HAsciiString)& theName,
                                     const Standard_Real                     theX,
                                     const Standard_Real                     theY,
                                     const Standard_Real                     theZ)
{
  myNbCoord   = 3;
  myCoords[0] = theX;
  myCoords[1] = theY;
  myCoords[2] = theZ;
  // --- classe inherited fields ---
  StepRepr_RepresentationItem::Init(theName);
}

void StepGeom_CartesianPoint::SetCoordinates(const Handle(TColStd_HArray1OfReal)& aCoordinates)
{
  myNbCoord = aCoordinates->Length();
  if (myNbCoord > 0)
    myCoords[0] = aCoordinates->Value(1);
  if (myNbCoord > 1)
    myCoords[1] = aCoordinates->Value(2);
  if (myNbCoord > 2)
    myCoords[2] = aCoordinates->Value(3);
}

void StepGeom_CartesianPoint::SetCoordinates(const std::array<Standard_Real, 3>& theCoordinates)
{
  myCoords = theCoordinates;
}

const std::array<Standard_Real, 3>& StepGeom_CartesianPoint::Coordinates() const
{
  return myCoords;
}

Standard_Real StepGeom_CartesianPoint::CoordinatesValue(const Standard_Integer num) const
{
  return myCoords[num - 1];
}

void StepGeom_CartesianPoint::SetNbCoordinates(const Standard_Integer num)
{
  myNbCoord = num;
}

Standard_Integer StepGeom_CartesianPoint::NbCoordinates() const
{
  return myNbCoord;
}
