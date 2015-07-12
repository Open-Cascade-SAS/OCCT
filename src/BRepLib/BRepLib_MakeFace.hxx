// Created on: 1993-07-12
// Created by: Remi LEQUETTE
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

#ifndef _BRepLib_MakeFace_HeaderFile
#define _BRepLib_MakeFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_FaceError.hxx>
#include <BRepLib_MakeShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class StdFail_NotDone;
class TopoDS_Face;
class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class Geom_Surface;
class TopoDS_Wire;
class Geom_Curve;


//! Provides methods to build faces.
//!
//! A face may be built :
//!
//! * From a surface.
//!
//! - Elementary surface from gp.
//!
//! - Surface from Geom.
//!
//! * From a surface and U,V values.
//!
//! * From a wire.
//!
//! - Find the surface automatically if possible.
//!
//! * From a surface and a wire.
//!
//! - A flag Inside is given, when this flag is True
//! the  wire is  oriented to bound a finite area on
//! the surface.
//!
//! * From a face and a wire.
//!
//! - The new wire is a perforation.
class BRepLib_MakeFace  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Not done.
  Standard_EXPORT BRepLib_MakeFace();
  
  //! Load a face. Usefull to add wires.
  Standard_EXPORT BRepLib_MakeFace(const TopoDS_Face& F);
  
  //! Make a face from a plane.
  Standard_EXPORT BRepLib_MakeFace(const gp_Pln& P);
  
  //! Make a face from a cylinder.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cylinder& C);
  
  //! Make a face from a cone.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cone& C);
  
  //! Make a face from a sphere.
  Standard_EXPORT BRepLib_MakeFace(const gp_Sphere& S);
  
  //! Make a face from a torus.
  Standard_EXPORT BRepLib_MakeFace(const gp_Torus& C);
  
  //! Make a face from a Surface. Accepts tolerance value (TolDegen)
  //! for resolution of degenerated edges.
  Standard_EXPORT BRepLib_MakeFace(const Handle(Geom_Surface)& S, const Standard_Real TolDegen);
  
  //! Make a face from a plane.
  Standard_EXPORT BRepLib_MakeFace(const gp_Pln& P, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax);
  
  //! Make a face from a cylinder.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cylinder& C, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax);
  
  //! Make a face from a cone.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cone& C, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax);
  
  //! Make a face from a sphere.
  Standard_EXPORT BRepLib_MakeFace(const gp_Sphere& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax);
  
  //! Make a face from a torus.
  Standard_EXPORT BRepLib_MakeFace(const gp_Torus& C, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax);
  
  //! Make a face from a Surface. Accepts min & max parameters
  //! to construct the face's bounds. Also accepts tolerance value (TolDegen)
  //! for resolution of degenerated edges.
  Standard_EXPORT BRepLib_MakeFace(const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Real TolDegen);
  
  //! Find a surface from the wire and make a face.
  //! if <OnlyPlane> is true, the computed surface will be
  //! a plane. If it is not possible to find a plane, the
  //! flag NotDone will be set.
  Standard_EXPORT BRepLib_MakeFace(const TopoDS_Wire& W, const Standard_Boolean OnlyPlane = Standard_False);
  
  //! Make a face from a plane and a wire.
  Standard_EXPORT BRepLib_MakeFace(const gp_Pln& P, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Make a face from a cylinder and a wire.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cylinder& C, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Make a face from a cone and a wire.
  Standard_EXPORT BRepLib_MakeFace(const gp_Cone& C, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Make a face from a sphere and a wire.
  Standard_EXPORT BRepLib_MakeFace(const gp_Sphere& S, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Make a face from a torus and a wire.
  Standard_EXPORT BRepLib_MakeFace(const gp_Torus& C, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Make a face from a Surface and a wire.
  Standard_EXPORT BRepLib_MakeFace(const Handle(Geom_Surface)& S, const TopoDS_Wire& W, const Standard_Boolean Inside = Standard_True);
  
  //! Adds the wire <W> in the face <F>
  Standard_EXPORT BRepLib_MakeFace(const TopoDS_Face& F, const TopoDS_Wire& W);
  
  //! Load the face.
  Standard_EXPORT void Init (const TopoDS_Face& F);
  
  //! Creates the face  from the  surface. If Bound is
  //! True a wire is made from the natural bounds.
  //! Accepts tolerance value (TolDegen) for resolution
  //! of degenerated edges.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S, const Standard_Boolean Bound, const Standard_Real TolDegen);
  
  //! Creates the face from the surface and the min-max
  //! values. Accepts tolerance value (TolDegen) for resolution
  //! of degenerated edges.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Real TolDegen);
  
  //! Adds the wire <W> in the current face.
  Standard_EXPORT void Add (const TopoDS_Wire& W);
  
  Standard_EXPORT BRepLib_FaceError Error() const;
  
  //! Returns the new face.
  Standard_EXPORT const TopoDS_Face& Face() const;
Standard_EXPORT operator TopoDS_Face() const;
  
  //! Checks the specified curve is degenerated
  //! according to specified tolerance.
  //! Returns <theActTol> less than <theMaxTol>, which shows
  //! actual tolerance to decide the curve is degenerated.
  //! Warning: For internal use of BRepLib_MakeFace and BRepLib_MakeShell.
  Standard_EXPORT static Standard_Boolean IsDegenerated (const Handle(Geom_Curve)& theCurve, const Standard_Real theMaxTol, Standard_Real& theActTol);




protected:





private:

  
  //! Reorient the current face if  the boundary  is not
  //! finite.
  Standard_EXPORT void CheckInside();


  BRepLib_FaceError myError;


};







#endif // _BRepLib_MakeFace_HeaderFile
