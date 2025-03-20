// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#ifndef _BRepClass3d_SolidPassiveClassifier_HeaderFile
#define _BRepClass3d_SolidPassiveClassifier_HeaderFile

#include <BRepClass3d_Intersector3d.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Lin.hxx>

class Standard_DomainError;
class BRepClass3d_Intersector3d;
class gp_Lin;
class TopoDS_Face;

class BRepClass3d_SolidPassiveClassifier
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an undefined classifier.
  Standard_EXPORT BRepClass3d_SolidPassiveClassifier();

  //! Starts  a  classification process.   The  point to
  //! classify is the origin of  the  line <L>.  <P>  is
  //! the original length of the segment on <L>  used to
  //! compute  intersections.   <Tol> is the   tolerance
  //! attached to the intersections.
  Standard_EXPORT void Reset(const gp_Lin& L, const Standard_Real P, const Standard_Real Tol);

  //! Updates  the classification process with  the face
  //! <F> from the boundary.
  Standard_EXPORT void Compare(const TopoDS_Face& F, const TopAbs_Orientation Or);

  //! Returns the current value of the parameter.
  Standard_Real Parameter() const { return myParam; }

  //! Returns True if an intersection is computed.
  Standard_Boolean HasIntersection() const { return hasIntersect; }

  //! Returns the intersecting algorithm.
  BRepClass3d_Intersector3d& Intersector() { return myIntersector; }

  //! Returns the current state of the point.
  TopAbs_State State() const { return myState; }

private:
  Standard_Boolean          isSet;
  TopoDS_Face               myFace;
  gp_Lin                    myLin;
  Standard_Real             myParam;
  Standard_Real             myTolerance;
  TopAbs_State              myState;
  Standard_Boolean          hasIntersect;
  BRepClass3d_Intersector3d myIntersector;
};

#endif // _BRepClass3d_SolidPassiveClassifier_HeaderFile
