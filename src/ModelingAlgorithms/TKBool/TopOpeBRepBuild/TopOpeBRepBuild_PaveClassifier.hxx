// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_PaveClassifier_HeaderFile
#define _TopOpeBRepBuild_PaveClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepBuild_LoopClassifier.hxx>
#include <TopAbs_State.hxx>
class TopoDS_Shape;
class TopOpeBRepBuild_Loop;

//! This class compares vertices on an edge.
//!
//! A vertex V1 is inside a vertex V2 if V1 is on the
//! part of the curve defined by V2.
//!
//! If V2 is FORWARD V1 must be after V2 on the curve.
//! If V2 is REVERSED V1 must be before V2 on the curve.
//! If V2 is INTERNAL V1 is always inside.
//! If V2 is EXTERNAL V1 is never inside.
class TopOpeBRepBuild_PaveClassifier : public TopOpeBRepBuild_LoopClassifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Create a Pave classifier to compare vertices on edge <E>.
  Standard_EXPORT TopOpeBRepBuild_PaveClassifier(const TopoDS_Shape& E);

  //! Returns state of vertex <L1> compared with <L2>.
  Standard_EXPORT TopAbs_State Compare(const Handle(TopOpeBRepBuild_Loop)& L1,
                                       const Handle(TopOpeBRepBuild_Loop)& L2);

  Standard_EXPORT void SetFirstParameter(const Standard_Real P);

  Standard_EXPORT void ClosedVertices(const Standard_Boolean B);

  Standard_EXPORT static Standard_Real AdjustCase(const Standard_Real      p1,
                                                  const TopAbs_Orientation o,
                                                  const Standard_Real      first,
                                                  const Standard_Real      period,
                                                  const Standard_Real      tol,
                                                  Standard_Integer&        cas);

protected:
private:
  Standard_EXPORT TopAbs_State CompareOnNonPeriodic();

  Standard_EXPORT TopAbs_State CompareOnPeriodic();

  Standard_EXPORT void AdjustOnPeriodic();

  Standard_EXPORT Standard_Boolean ToAdjustOnPeriodic() const;

  TopoDS_Edge        myEdge;
  Standard_Boolean   myEdgePeriodic;
  Standard_Real      myFirst;
  Standard_Real      myPeriod;
  Standard_Boolean   mySameParameters;
  Standard_Boolean   myClosedVertices;
  Standard_Real      myP1;
  Standard_Real      myP2;
  TopAbs_Orientation myO1;
  TopAbs_Orientation myO2;
  Standard_Integer   myCas1;
  Standard_Integer   myCas2;
};

#endif // _TopOpeBRepBuild_PaveClassifier_HeaderFile
