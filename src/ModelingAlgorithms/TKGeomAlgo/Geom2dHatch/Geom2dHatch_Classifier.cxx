// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Geom2dHatch_Classifier.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Elements.hxx>
#include <Geom2dHatch_FClass2dOfClassifier.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Standard_DomainError.hxx>
#include <TopClass_FaceClassifier.pxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================

Geom2dHatch_Classifier::Geom2dHatch_Classifier()
    : myEdgeParameter(0.0),
      rejected(false),
      nowires(true)
{
}

//=================================================================================================

Geom2dHatch_Classifier::Geom2dHatch_Classifier(Geom2dHatch_Elements& F,
                                               const gp_Pnt2d&       P,
                                               const double          Tol)
    : myEdgeParameter(0.0),
      rejected(false),
      nowires(true)
{
  Perform(F, P, Tol);
}

//=================================================================================================

void Geom2dHatch_Classifier::Perform(Geom2dHatch_Elements& F, const gp_Pnt2d& P, const double Tol)
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

TopAbs_State Geom2dHatch_Classifier::State() const
{
  return TopClass_FaceClassifier::State(rejected, nowires, myClassifier);
}

//=================================================================================================

const Geom2dAdaptor_Curve& Geom2dHatch_Classifier::Edge() const
{
  return TopClass_FaceClassifier::Edge(rejected, myEdge);
}

//=================================================================================================

double Geom2dHatch_Classifier::EdgeParameter() const
{
  return TopClass_FaceClassifier::EdgeParameter(rejected, myEdgeParameter);
}
