// Created on: 1993-08-06
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _StepToTopoDS_PointPair_HeaderFile
#define _StepToTopoDS_PointPair_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class StepGeom_CartesianPoint;

//! Stores a pair of Points from step
class StepToTopoDS_PointPair
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT StepToTopoDS_PointPair(const Handle(StepGeom_CartesianPoint)& P1,
                                         const Handle(StepGeom_CartesianPoint)& P2);

  const Handle(StepGeom_CartesianPoint)& GetPoint1() const { return myP1; }

  const Handle(StepGeom_CartesianPoint)& GetPoint2() const { return myP2; }

  bool operator==(const StepToTopoDS_PointPair& thePointPair) const
  {
    return (((myP1 == thePointPair.myP1) && (myP2 == thePointPair.myP2))
            || ((myP1 == thePointPair.myP2) && (myP2 == thePointPair.myP1)));
  }

protected:
private:
  Handle(StepGeom_CartesianPoint) myP1;
  Handle(StepGeom_CartesianPoint) myP2;
};

namespace std
{
template <>
struct hash<StepToTopoDS_PointPair>
{
  size_t operator()(const StepToTopoDS_PointPair& thePointPair) const noexcept
  {
    // Combine two int values into a single hash value.
    size_t aCombination[2];
    aCombination[0] = std::hash<Handle(StepGeom_CartesianPoint)>{}(thePointPair.GetPoint1());
    aCombination[1] = std::hash<Handle(StepGeom_CartesianPoint)>{}(thePointPair.GetPoint2());
    if (aCombination[0] > aCombination[1])
    {
      std::swap(aCombination[0], aCombination[1]);
    }
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};
} // namespace std

#endif // _StepToTopoDS_PointPair_HeaderFile
