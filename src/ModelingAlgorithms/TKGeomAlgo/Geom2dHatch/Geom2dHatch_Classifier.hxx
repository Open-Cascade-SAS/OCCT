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

#ifndef _Geom2dHatch_Classifier_HeaderFile
#define _Geom2dHatch_Classifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Geom2dHatch_FClass2dOfClassifier.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <IntRes2d_Position.hxx>
#include <TopAbs_State.hxx>
class Standard_DomainError;
class Geom2dHatch_Elements;
class Geom2dAdaptor_Curve;
class Geom2dHatch_Intersector;
class Geom2dHatch_FClass2dOfClassifier;
class gp_Pnt2d;

class Geom2dHatch_Classifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor, undefined algorithm.
  Standard_EXPORT Geom2dHatch_Classifier();

  //! Creates an algorithm to classify the Point P with
  //! Tolerance <T> on the face described by <F>.
  Standard_EXPORT Geom2dHatch_Classifier(Geom2dHatch_Elements& F,
                                         const gp_Pnt2d&       P,
                                         const double          Tol);

  //! Classify the Point P with Tolerance <T> on the
  //! face described by <F>.
  Standard_EXPORT void Perform(Geom2dHatch_Elements& F, const gp_Pnt2d& P, const double Tol);

  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;

  //! Returns True when the state was computed by a
  //! rejection. The state is OUT.
  bool Rejected() const { return rejected; }

  //! Returns True if the face contains no wire.
  //! The state is IN.
  bool NoWires() const { return nowires; }

  //! Returns the Edge used to determine the
  //! classification. When the State is ON this is the
  //! Edge containing the point.
  Standard_EXPORT const Geom2dAdaptor_Curve& Edge() const;

  //! Returns the parameter on Edge() used to determine the
  //! classification.
  Standard_EXPORT double EdgeParameter() const;

  //! Returns the position of the point on the edge
  //! returned by Edge.
  IntRes2d_Position Position() const { return myPosition; }

protected:
  Geom2dHatch_FClass2dOfClassifier myClassifier;
  Geom2dAdaptor_Curve              myEdge;
  double                           myEdgeParameter;
  IntRes2d_Position                myPosition;
  bool                             rejected;
  bool                             nowires;
};

#endif // _Geom2dHatch_Classifier_HeaderFile
