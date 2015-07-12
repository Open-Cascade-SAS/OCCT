// Created on: 1992-11-05
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

#ifndef _BRepPrim_Revolution_HeaderFile
#define _BRepPrim_Revolution_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_OneAxis.hxx>
#include <Standard_Real.hxx>
class Geom_Curve;
class Geom2d_Curve;
class gp_Ax2;
class TopoDS_Face;
class TopoDS_Edge;
class gp_Pnt2d;


//! Implement  the OneAxis algoritm   for a revolution
//! surface.
class BRepPrim_Revolution  : public BRepPrim_OneAxis
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create a  revolution body <M>  is the  meridian nd
  //! must   be in the XZ  plane   of <A>. <PM>  is  the
  //! meridian in the XZ plane.
  Standard_EXPORT BRepPrim_Revolution(const gp_Ax2& A, const Standard_Real VMin, const Standard_Real VMax, const Handle(Geom_Curve)& M, const Handle(Geom2d_Curve)& PM);
  
  //! The surface normal should be directed  towards the
  //! outside.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const;
  
  //! Returns  an  edge with  a 3D curve   made from the
  //! meridian  in the XZ  plane rotated by <Ang> around
  //! the Z-axis. Ang may be 0 or myAngle.
  Standard_EXPORT virtual TopoDS_Edge MakeEmptyMeridianEdge (const Standard_Real Ang) const;
  
  //! Returns the meridian point at parameter <V> in the
  //! plane XZ.
  Standard_EXPORT virtual gp_Pnt2d MeridianValue (const Standard_Real V) const;
  
  //! Sets the  parametric urve of  the edge <E>  in the
  //! face <F>   to be  the  2d  representation  of  the
  //! meridian.
  Standard_EXPORT virtual void SetMeridianPCurve (TopoDS_Edge& E, const TopoDS_Face& F) const;




protected:

  
  //! Create a  revolution   body.  The meridian  is set
  //! later. Reserved for derivated classes.
  Standard_EXPORT BRepPrim_Revolution(const gp_Ax2& A, const Standard_Real VMin, const Standard_Real VMax);
  
  Standard_EXPORT void Meridian (const Handle(Geom_Curve)& M, const Handle(Geom2d_Curve)& PM);




private:



  Handle(Geom_Curve) myMeridian;
  Handle(Geom2d_Curve) myPMeridian;


};







#endif // _BRepPrim_Revolution_HeaderFile
