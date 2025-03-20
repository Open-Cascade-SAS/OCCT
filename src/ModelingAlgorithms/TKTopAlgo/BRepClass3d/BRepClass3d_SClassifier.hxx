// Created on: 1996-07-15
// Created by: Laurent BUCHARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepClass3d_SClassifier_HeaderFile
#define _BRepClass3d_SClassifier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
class BRepClass3d_SolidExplorer;
class gp_Pnt;

//! Provides an algorithm to classify a point in a solid.
class BRepClass3d_SClassifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT BRepClass3d_SClassifier();

  //! Constructor to classify the point P with the
  //! tolerance Tol on the solid S.
  Standard_EXPORT BRepClass3d_SClassifier(BRepClass3d_SolidExplorer& S,
                                          const gp_Pnt&              P,
                                          const Standard_Real        Tol);

  //! Classify the point P with the
  //! tolerance Tol on the solid S.
  Standard_EXPORT void Perform(BRepClass3d_SolidExplorer& S,
                               const gp_Pnt&              P,
                               const Standard_Real        Tol);

  //! Classify an infinite point with the
  //! tolerance Tol on the solid S.
  Standard_EXPORT void PerformInfinitePoint(BRepClass3d_SolidExplorer& S, const Standard_Real Tol);

  //! Returns True if the classification has been
  //! computed by rejection.
  //! The State is then OUT.
  Standard_EXPORT Standard_Boolean Rejected() const;

  //! Returns the result of the classification.
  Standard_EXPORT TopAbs_State State() const;

  //! Returns True when the point is a point of a face.
  Standard_EXPORT Standard_Boolean IsOnAFace() const;

  //! Returns the face used to determine the
  //! classification. When the state is ON, this is the
  //! face containing the point.
  //!
  //! When Rejected() returns True, Face() has no signification.
  Standard_EXPORT TopoDS_Face Face() const;

protected:
  Standard_EXPORT void ForceIn();

  Standard_EXPORT void ForceOut();

private:
  TopoDS_Face myFace;

  //! This variable stores information about algorithm internal state.
  //! Type of this variable differs from TopAbs_State since it contains
  //! additional information about error status.
  //! 1 - Error inside of the algorithm.
  //! 2 - ON.
  //! 3 - IN.
  //! 4 - OUT.
  Standard_Integer myState;
};

#endif // _BRepClass3d_SClassifier_HeaderFile
