// Created on: 1994-04-01
// Created by: Laurent BUCHARD
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

#ifndef _BRepClass3d_Intersector3d_HeaderFile
#define _BRepClass3d_Intersector3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Face.hxx>
class gp_Lin;
class TopoDS_Face;
class gp_Pnt;



class BRepClass3d_Intersector3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT BRepClass3d_Intersector3d();
  
  //! Perform the intersection between the
  //! segment L(0) ... L(Prm) and the Shape <Sh>.
  //!
  //! Only the point with the smallest parameter on the
  //! line is returned.
  //!
  //! The Tolerance <Tol> is used to determine if the
  //! first point of the segment is near the face. In
  //! that case, the parameter of the intersection point
  //! on the line can be a negative value (greater than -Tol).
  Standard_EXPORT void Perform (const gp_Lin& L, const Standard_Real Prm, const Standard_Real Tol, const TopoDS_Face& F);
  
  //! True is returned when the intersection have been computed.
    Standard_Boolean IsDone() const;
  
  //! True is returned if a point has been found.
    Standard_Boolean HasAPoint() const;
  
  //! Returns the U parameter of the intersection point
  //! on the surface.
    Standard_Real UParameter() const;
  
  //! Returns the V parameter of the intersection point
  //! on the surface.
    Standard_Real VParameter() const;
  
  //! Returns the parameter of the intersection point
  //! on the line.
    Standard_Real WParameter() const;
  
  //! Returns the geometric point of the intersection
  //! between the line and the surface.
    const gp_Pnt& Pnt() const;
  
  //! Returns the transition of the line on the surface.
    IntCurveSurface_TransitionOnCurve Transition() const;
  
  //! Returns the state of the point on the face.
  //! The values can be either TopAbs_IN
  //! ( the point is in the face)
  //! or TopAbs_ON
  //! ( the point is on a boudary of the face).
    TopAbs_State State() const;
  
  //! Returns the significant face used to determine
  //! the intersection.
    const TopoDS_Face& Face() const;




protected:





private:



  gp_Pnt pnt;
  Standard_Real U;
  Standard_Real V;
  Standard_Real W;
  IntCurveSurface_TransitionOnCurve transition;
  Standard_Boolean done;
  Standard_Boolean hasapoint;
  TopAbs_State state;
  TopoDS_Face face;


};


#include <BRepClass3d_Intersector3d.lxx>





#endif // _BRepClass3d_Intersector3d_HeaderFile
