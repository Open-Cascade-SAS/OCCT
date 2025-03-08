// Created on: 1993-05-28
// Created by: Modelistation
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

#ifndef _BRepClass_FaceClassifier_HeaderFile
#define _BRepClass_FaceClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepClass_FClassifier.hxx>
class BRepClass_FaceExplorer;
class gp_Pnt2d;
class TopoDS_Face;
class gp_Pnt;

//! Provides Constructors with a Face.
class BRepClass_FaceClassifier : public BRepClass_FClassifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor, undefined algorithm.
  Standard_EXPORT BRepClass_FaceClassifier();

  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face described by <F>.
  Standard_EXPORT BRepClass_FaceClassifier(BRepClass_FaceExplorer& F,
                                           const gp_Pnt2d&         P,
                                           const Standard_Real     Tol);

  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face <F>.
  //! Recommended to use Bnd_Box if the number of edges > 10
  //! and the geometry is mostly spline
  Standard_EXPORT BRepClass_FaceClassifier(const TopoDS_Face&     theF,
                                           const gp_Pnt2d&        theP,
                                           const Standard_Real    theTol,
                                           const Standard_Boolean theUseBndBox   = Standard_False,
                                           const Standard_Real    theGapCheckTol = 0.1);

  //! Classify  the Point  P  with  Tolerance <T> on the
  //! face described by <F>.
  //! Recommended to use Bnd_Box if the number of edges > 10
  //! and the geometry is mostly spline
  Standard_EXPORT void Perform(const TopoDS_Face&     theF,
                               const gp_Pnt2d&        theP,
                               const Standard_Real    theTol,
                               const Standard_Boolean theUseBndBox   = Standard_False,
                               const Standard_Real    theGapCheckTol = 0.1);

  //! Creates an algorithm to classify the Point  P with
  //! Tolerance <T> on the face <F>.
  //! Recommended to use Bnd_Box if the number of edges > 10
  //! and the geometry is mostly spline
  Standard_EXPORT BRepClass_FaceClassifier(const TopoDS_Face&     theF,
                                           const gp_Pnt&          theP,
                                           const Standard_Real    theTol,
                                           const Standard_Boolean theUseBndBox   = Standard_False,
                                           const Standard_Real    theGapCheckTol = 0.1);

  //! Classify  the Point  P  with  Tolerance <T> on the
  //! face described by <F>.
  //! Recommended to use Bnd_Box if the number of edges > 10
  //! and the geometry is mostly spline
  Standard_EXPORT void Perform(const TopoDS_Face&     theF,
                               const gp_Pnt&          theP,
                               const Standard_Real    theTol,
                               const Standard_Boolean theUseBndBox   = Standard_False,
                               const Standard_Real    theGapCheckTol = 0.1);

protected:
private:
};

#endif // _BRepClass_FaceClassifier_HeaderFile
