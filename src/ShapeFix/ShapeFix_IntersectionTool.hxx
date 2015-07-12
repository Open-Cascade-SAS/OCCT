// Created on: 2004-03-05
// Created by: Sergey KUUL
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _ShapeFix_IntersectionTool_HeaderFile
#define _ShapeFix_IntersectionTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <ShapeFix_DataMapOfShapeBox2d.hxx>
class ShapeBuild_ReShape;
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class ShapeExtend_WireData;
class Bnd_Box2d;
class Geom2d_Curve;


//! Tool for fixing selfintersecting wire
//! and intersecting wires
class ShapeFix_IntersectionTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor
  Standard_EXPORT ShapeFix_IntersectionTool(const Handle(ShapeBuild_ReShape)& context, const Standard_Real preci, const Standard_Real maxtol = 1.0);
  
  //! Returns context
    Handle(ShapeBuild_ReShape) Context() const;
  
  //! Split edge on two new edges using new vertex "vert"
  //! and "param" - parameter for splitting
  //! The "face" is necessary for pcurves and using TransferParameterProj
  Standard_EXPORT Standard_Boolean SplitEdge (const TopoDS_Edge& edge, const Standard_Real param, const TopoDS_Vertex& vert, const TopoDS_Face& face, TopoDS_Edge& newE1, TopoDS_Edge& newE2, const Standard_Real preci) const;
  
  //! Cut edge by parameters pend and cut
  Standard_EXPORT Standard_Boolean CutEdge (const TopoDS_Edge& edge, const Standard_Real pend, const Standard_Real cut, const TopoDS_Face& face, Standard_Boolean& iscutline) const;
  
  Standard_EXPORT Standard_Boolean FixSelfIntersectWire (Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face, Standard_Integer& NbSplit, Standard_Integer& NbCut, Standard_Integer& NbRemoved) const;
  
  Standard_EXPORT Standard_Boolean FixIntersectingWires (TopoDS_Face& face) const;




protected:





private:

  
  Standard_EXPORT Standard_Boolean SplitEdge1 (const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face, const Standard_Integer num, const Standard_Real param, const TopoDS_Vertex& vert, const Standard_Real preci, ShapeFix_DataMapOfShapeBox2d& boxes) const;
  
  Standard_EXPORT Standard_Boolean SplitEdge2 (const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face, const Standard_Integer num, const Standard_Real param1, const Standard_Real param2, const TopoDS_Vertex& vert, const Standard_Real preci, ShapeFix_DataMapOfShapeBox2d& boxes) const;
  
  Standard_EXPORT Standard_Boolean UnionVertexes (const Handle(ShapeExtend_WireData)& sewd, TopoDS_Edge& edge1, TopoDS_Edge& edge2, const Standard_Integer num2, ShapeFix_DataMapOfShapeBox2d& boxes, const Bnd_Box2d& B2) const;
  
  Standard_EXPORT Standard_Boolean FindVertAndSplitEdge (const Standard_Real param1, const TopoDS_Edge& edge1, const TopoDS_Edge& edge2, const Handle(Geom2d_Curve)& Crv1, Standard_Real& MaxTolVert, Standard_Integer& num1, const Handle(ShapeExtend_WireData)& sewd, const TopoDS_Face& face, ShapeFix_DataMapOfShapeBox2d& boxes, const Standard_Boolean aTmpKey) const;


  Handle(ShapeBuild_ReShape) myContext;
  Standard_Real myPreci;
  Standard_Real myMaxTol;


};


#include <ShapeFix_IntersectionTool.lxx>





#endif // _ShapeFix_IntersectionTool_HeaderFile
