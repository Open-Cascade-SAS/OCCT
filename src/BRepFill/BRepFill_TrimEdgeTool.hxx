// Created on: 1995-04-24
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepFill_TrimEdgeTool_HeaderFile
#define _BRepFill_TrimEdgeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <Bisector_Bisec.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAbs_JoinType.hxx>
#include <TColgp_SequenceOfPnt.hxx>
class Geom2d_Curve;
class Bisector_Bisec;
class Geom2d_Geometry;
class TopoDS_Edge;
class TopoDS_Shape;
class TopoDS_Vertex;
class gp_Pnt2d;


//! Geometric Tool using to construct Offset Wires.
class BRepFill_TrimEdgeTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_TrimEdgeTool();
  
  Standard_EXPORT BRepFill_TrimEdgeTool(const Bisector_Bisec& Bisec, const Handle(Geom2d_Geometry)& S1, const Handle(Geom2d_Geometry)& S2, const Standard_Real Offset);
  
  Standard_EXPORT   void IntersectWith (const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const TopoDS_Shape& InitShape1, const TopoDS_Shape& InitShape2, const TopoDS_Vertex& End1, const TopoDS_Vertex& End2, const GeomAbs_JoinType theJoinType, const Standard_Boolean IsOpenResult, TColgp_SequenceOfPnt& Params) ;
  
  Standard_EXPORT void AddOrConfuse (const Standard_Boolean Start, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, TColgp_SequenceOfPnt& Params) const;
  
  Standard_EXPORT Standard_Boolean IsInside (const gp_Pnt2d& P) const;




protected:





private:



  Standard_Boolean isPoint1;
  Standard_Boolean isPoint2;
  gp_Pnt2d myP1;
  gp_Pnt2d myP2;
  Handle(Geom2d_Curve) myC1;
  Handle(Geom2d_Curve) myC2;
  Standard_Real myOffset;
  Bisector_Bisec myBisec;
  Geom2dAdaptor_Curve myBis;


};







#endif // _BRepFill_TrimEdgeTool_HeaderFile
