// Created on: 1992-02-18
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRAlgo_Interference_HeaderFile
#define _HLRAlgo_Interference_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <HLRAlgo_Intersection.hxx>
#include <HLRAlgo_Coincidence.hxx>
#include <TopAbs_Orientation.hxx>
class HLRAlgo_Intersection;
class HLRAlgo_Coincidence;

class HLRAlgo_Interference
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRAlgo_Interference();

  Standard_EXPORT HLRAlgo_Interference(const HLRAlgo_Intersection& Inters,
                                       const HLRAlgo_Coincidence&  Bound,
                                       const TopAbs_Orientation    Orient,
                                       const TopAbs_Orientation    Trans,
                                       const TopAbs_Orientation    BTrans);

  void Intersection(const HLRAlgo_Intersection& I);

  void Boundary(const HLRAlgo_Coincidence& B);

  void Orientation(const TopAbs_Orientation O);

  void Transition(const TopAbs_Orientation Tr);

  void BoundaryTransition(const TopAbs_Orientation BTr);

  const HLRAlgo_Intersection& Intersection() const;

  HLRAlgo_Intersection& ChangeIntersection();

  const HLRAlgo_Coincidence& Boundary() const;

  HLRAlgo_Coincidence& ChangeBoundary();

  TopAbs_Orientation Orientation() const;

  TopAbs_Orientation Transition() const;

  TopAbs_Orientation BoundaryTransition() const;

protected:
private:
  HLRAlgo_Intersection myIntersection;
  HLRAlgo_Coincidence  myBoundary;
  TopAbs_Orientation   myOrientation;
  TopAbs_Orientation   myTransition;
  TopAbs_Orientation   myBTransition;
};

#define TheSubShape HLRAlgo_Intersection
#define TheSubShape_hxx <HLRAlgo_Intersection.hxx>
#define TheShape HLRAlgo_Coincidence
#define TheShape_hxx <HLRAlgo_Coincidence.hxx>
#define TopBas_Interference HLRAlgo_Interference
#define TopBas_Interference_hxx <HLRAlgo_Interference.hxx>

#include <TopBas_Interference.lxx>

#undef TheSubShape
#undef TheSubShape_hxx
#undef TheShape
#undef TheShape_hxx
#undef TopBas_Interference
#undef TopBas_Interference_hxx

#endif // _HLRAlgo_Interference_HeaderFile
