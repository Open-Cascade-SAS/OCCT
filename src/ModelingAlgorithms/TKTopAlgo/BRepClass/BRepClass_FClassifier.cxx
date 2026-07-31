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

#include <BRepClass_FClassifier.hxx>

#include <BRepClass_Edge.hxx>
#include <BRepClass_FClass2dOfFClassifier.hxx>
#include <BRepClass_FaceExplorer.hxx>
#include <BRepClass_Intersector.hxx>
#include <Standard_DomainError.hxx>
#include <TopClass_FaceClassifier.pxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================

BRepClass_FClassifier::BRepClass_FClassifier()
    : myEdgeParameter(0.0),
      rejected(false),
      nowires(true)
{
}

//=================================================================================================

BRepClass_FClassifier::BRepClass_FClassifier(BRepClass_FaceExplorer& F,
                                             const gp_Pnt2d&         P,
                                             const double            Tol)
    : myEdgeParameter(0.0),
      rejected(false),
      nowires(true)
{
  Perform(F, P, Tol);
}

//=================================================================================================

void BRepClass_FClassifier::Perform(BRepClass_FaceExplorer& F, const gp_Pnt2d& P, const double Tol)
{
  TopClass_FaceClassifier::Perform(rejected,
                                   nowires,
                                   myClassifier,
                                   myEdge,
                                   myPosition,
                                   myEdgeParameter,
                                   F,
                                   P,
                                   Tol);
}

//=================================================================================================

TopAbs_State BRepClass_FClassifier::State() const
{
  return TopClass_FaceClassifier::State(rejected, nowires, myClassifier);
}

//=================================================================================================

const BRepClass_Edge& BRepClass_FClassifier::Edge() const
{
  return TopClass_FaceClassifier::Edge(rejected, myEdge);
}

//=================================================================================================

double BRepClass_FClassifier::EdgeParameter() const
{
  return TopClass_FaceClassifier::EdgeParameter(rejected, myEdgeParameter);
}
