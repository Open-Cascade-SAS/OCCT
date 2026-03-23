// Created on: 1992-11-18
// Created by: Remi LEQUETTE
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

#include <BRepClass_FacePassiveClassifier.hxx>

#include <BRepClass_Edge.hxx>
#include <BRepClass_Intersector.hxx>
#include <Standard_DomainError.hxx>
#include <TopClass_Classifier2d.pxx>
#include <gp_Lin2d.hxx>

//=================================================================================================

BRepClass_FacePassiveClassifier::BRepClass_FacePassiveClassifier()
    : myIsSet(false),
      myFirstCompare(true),
      myFirstTrans(true),
      myParam(0.0),
      myTolerance(0.0),
      myClosest(0),
      myState(TopAbs_UNKNOWN),
      myIsHeadOrEnd(false)
{
}

//=================================================================================================

void BRepClass_FacePassiveClassifier::Reset(const gp_Lin2d& L, const double P, const double Tol)
{
  TopClass_Classifier2d::Reset(myLin, myParam, myTolerance, myState, myFirstCompare, myFirstTrans,
                               myClosest, myIsSet, myIsHeadOrEnd, L, P, Tol);
}

//=================================================================================================

void BRepClass_FacePassiveClassifier::Compare(const BRepClass_Edge&    E,
                                              const TopAbs_Orientation Or)
{
  TopClass_Classifier2d::Compare(myClosest, myIntersector, myLin, myParam, myTolerance, myState,
                                 myFirstCompare, myFirstTrans, myTrans, myIsHeadOrEnd, E, Or);
}
