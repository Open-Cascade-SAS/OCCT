// Created on: 1994-10-21
// Created by: Bruno DUMORTIER
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

#ifndef _BRepFill_TrimSurfaceTool_HeaderFile
#define _BRepFill_TrimSurfaceTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <Standard_Boolean.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
class Geom2d_Curve;
class gp_Pnt2d;
class Geom_Curve;


//! Compute the Pcurves and  the 3d curves resulting
//! of the trimming of a face by an extruded surface.
class BRepFill_TrimSurfaceTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_TrimSurfaceTool(const Handle(Geom2d_Curve)& Bis, const TopoDS_Face& Face1, const TopoDS_Face& Face2, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Boolean Inv1, const Standard_Boolean Inv2);
  
  //! Intersect <Bis>  with the  projection of the edges
  //! <EdgeOnFi> and returns the intersecting parameters
  //! on Bis and on the edges
  //! P.X() : Parameter on Bis
  //! P.Y() : Parameter on EdgeOnF1
  //! P.Z() : Parameter on EdgeOnF2
  //! raises if <Edge> is not a edge of Face1 or Face2.
  Standard_EXPORT void IntersectWith (const TopoDS_Edge& EdgeOnF1, const TopoDS_Edge& EdgeOnF2, TColgp_SequenceOfPnt& Points) const;
  
  //! returns True if the Line (P, DZ) intersect the Faces
  Standard_EXPORT Standard_Boolean IsOnFace (const gp_Pnt2d& Point) const;
  
  //! returns the parameter of the  point <Point> on the
  //! Edge <Edge>, assuming that the point is on the edge.
  Standard_EXPORT Standard_Real ProjOn (const gp_Pnt2d& Point, const TopoDS_Edge& Edge) const;
  
  Standard_EXPORT void Project (const Standard_Real U1, const Standard_Real U2, Handle(Geom_Curve)& Curve, Handle(Geom2d_Curve)& PCurve1, Handle(Geom2d_Curve)& PCurve2, GeomAbs_Shape& myCont) const;




protected:





private:



  TopoDS_Face myFace1;
  TopoDS_Face myFace2;
  TopoDS_Edge myEdge1;
  TopoDS_Edge myEdge2;
  Standard_Boolean myInv1;
  Standard_Boolean myInv2;
  Handle(Geom2d_Curve) myBis;


};







#endif // _BRepFill_TrimSurfaceTool_HeaderFile
