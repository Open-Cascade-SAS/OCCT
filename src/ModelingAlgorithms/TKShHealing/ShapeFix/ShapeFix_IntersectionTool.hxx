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

#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box2d.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
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
  Standard_EXPORT ShapeFix_IntersectionTool(const occ::handle<ShapeBuild_ReShape>& context,
                                            const double               preci,
                                            const double               maxtol = 1.0);

  //! Returns context
  occ::handle<ShapeBuild_ReShape> Context() const;

  //! Split edge on two new edges using new vertex "vert"
  //! and "param" - parameter for splitting
  //! The "face" is necessary for pcurves and using TransferParameterProj
  Standard_EXPORT bool SplitEdge(const TopoDS_Edge&   edge,
                                             const double  param,
                                             const TopoDS_Vertex& vert,
                                             const TopoDS_Face&   face,
                                             TopoDS_Edge&         newE1,
                                             TopoDS_Edge&         newE2,
                                             const double  preci) const;

  //! Cut edge by parameters pend and cut
  Standard_EXPORT bool CutEdge(const TopoDS_Edge&  edge,
                                           const double pend,
                                           const double cut,
                                           const TopoDS_Face&  face,
                                           bool&   iscutline) const;

  Standard_EXPORT bool FixSelfIntersectWire(occ::handle<ShapeExtend_WireData>& sewd,
                                                        const TopoDS_Face&            face,
                                                        int&             NbSplit,
                                                        int&             NbCut,
                                                        int& NbRemoved) const;

  Standard_EXPORT bool FixIntersectingWires(TopoDS_Face& face) const;

private:
  Standard_EXPORT bool SplitEdge1(const occ::handle<ShapeExtend_WireData>& sewd,
                                              const TopoDS_Face&                  face,
                                              const int              num,
                                              const double                 param,
                                              const TopoDS_Vertex&                vert,
                                              const double                 preci,
                                              NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>&       boxes) const;

  Standard_EXPORT bool SplitEdge2(const occ::handle<ShapeExtend_WireData>& sewd,
                                              const TopoDS_Face&                  face,
                                              const int              num,
                                              const double                 param1,
                                              const double                 param2,
                                              const TopoDS_Vertex&                vert,
                                              const double                 preci,
                                              NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>&       boxes) const;

  Standard_EXPORT bool UnionVertexes(const occ::handle<ShapeExtend_WireData>& sewd,
                                                 TopoDS_Edge&                        edge1,
                                                 TopoDS_Edge&                        edge2,
                                                 const int              num2,
                                                 NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>&       boxes,
                                                 const Bnd_Box2d&                    B2) const;

  Standard_EXPORT bool FindVertAndSplitEdge(const double         param1,
                                                        const TopoDS_Edge&          edge1,
                                                        const TopoDS_Edge&          edge2,
                                                        const occ::handle<Geom2d_Curve>& Crv1,
                                                        double&              MaxTolVert,
                                                        int&           num1,
                                                        const occ::handle<ShapeExtend_WireData>& sewd,
                                                        const TopoDS_Face&                  face,
                                                        NCollection_DataMap<TopoDS_Shape, Bnd_Box2d, TopTools_ShapeMapHasher>&       boxes,
                                                        const bool aTmpKey) const;

  occ::handle<ShapeBuild_ReShape> myContext;
  double              myPreci;
  double              myMaxTol;
};

#include <ShapeFix_IntersectionTool.lxx>

#endif // _ShapeFix_IntersectionTool_HeaderFile
