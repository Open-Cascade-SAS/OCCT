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

  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face described by <F>.
  Standard_EXPORT Geom2dHatch_Classifier(Geom2dHatch_Elements& F,
                                         const gp_Pnt2d&       P,
                                         const Standard_Real   Tol);

  //! Classify  the Point  P  with  Tolerance <T> on the
  //! face described by <F>.
  Standard_EXPORT void Perform(Geom2dHatch_Elements& F, const gp_Pnt2d& P, const Standard_Real Tol);

  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;

  //! Returns  True when  the   state was computed by  a
  //! rejection. The state is OUT.
  Standard_Boolean Rejected() const;

  //! Returns True if  the face  contains  no wire.  The
  //! state is IN.
  Standard_Boolean NoWires() const;

  //! Returns   the    Edge  used   to    determine  the
  //! classification. When the State is ON  this  is the
  //! Edge containing the point.
  Standard_EXPORT const Geom2dAdaptor_Curve& Edge() const;

  //! Returns the parameter on Edge() used to determine  the
  //! classification.
  Standard_EXPORT Standard_Real EdgeParameter() const;

  //! Returns the  position of  the   point on the  edge
  //! returned by Edge.
  IntRes2d_Position Position() const;

protected:
  Geom2dHatch_FClass2dOfClassifier myClassifier;
  Geom2dAdaptor_Curve              myEdge;
  Standard_Real                    myEdgeParameter;
  IntRes2d_Position                myPosition;
  Standard_Boolean                 rejected;
  Standard_Boolean                 nowires;

private:
};

#define TheFaceExplorer Geom2dHatch_Elements
#define TheFaceExplorer_hxx <Geom2dHatch_Elements.hxx>
#define TheEdge Geom2dAdaptor_Curve
#define TheEdge_hxx <Geom2dAdaptor_Curve.hxx>
#define TheIntersection2d Geom2dHatch_Intersector
#define TheIntersection2d_hxx <Geom2dHatch_Intersector.hxx>
#define TopClass_FClass2d Geom2dHatch_FClass2dOfClassifier
#define TopClass_FClass2d_hxx <Geom2dHatch_FClass2dOfClassifier.hxx>
#define TopClass_FaceClassifier Geom2dHatch_Classifier
#define TopClass_FaceClassifier_hxx <Geom2dHatch_Classifier.hxx>

#include <TopClass_FaceClassifier.lxx>

#undef TheFaceExplorer
#undef TheFaceExplorer_hxx
#undef TheEdge
#undef TheEdge_hxx
#undef TheIntersection2d
#undef TheIntersection2d_hxx
#undef TopClass_FClass2d
#undef TopClass_FClass2d_hxx
#undef TopClass_FaceClassifier
#undef TopClass_FaceClassifier_hxx

#endif // _Geom2dHatch_Classifier_HeaderFile
