// Created on: 2004-07-14
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

#ifndef _ShapeFix_SplitTool_HeaderFile
#define _ShapeFix_SplitTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class ShapeBuild_ReShape;

//! Tool for splitting and cutting edges; includes methods
//! used in OverlappingTool and IntersectionTool
class ShapeFix_SplitTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT ShapeFix_SplitTool();

  //! Split edge on two new edges using new vertex "vert"
  //! and "param" - parameter for splitting
  //! The "face" is necessary for pcurves and using TransferParameterProj
  Standard_EXPORT bool SplitEdge(const TopoDS_Edge&   edge,
                                 const double         param,
                                 const TopoDS_Vertex& vert,
                                 const TopoDS_Face&   face,
                                 TopoDS_Edge&         newE1,
                                 TopoDS_Edge&         newE2,
                                 const double         tol3d,
                                 const double         tol2d) const;

  //! Split edge on two new edges using new vertex "vert"
  //! and "param1" and "param2" - parameter for splitting and cutting
  //! The "face" is necessary for pcurves and using TransferParameterProj
  Standard_EXPORT bool SplitEdge(const TopoDS_Edge&   edge,
                                 const double         param1,
                                 const double         param2,
                                 const TopoDS_Vertex& vert,
                                 const TopoDS_Face&   face,
                                 TopoDS_Edge&         newE1,
                                 TopoDS_Edge&         newE2,
                                 const double         tol3d,
                                 const double         tol2d) const;

  //! Cut edge by parameters pend and cut
  Standard_EXPORT bool CutEdge(const TopoDS_Edge& edge,
                               const double       pend,
                               const double       cut,
                               const TopoDS_Face& face,
                               bool&              iscutline) const;

  //! Split edge on two new edges using two new vertex V1 and V2
  //! and two parameters for splitting - fp and lp correspondingly
  //! The "face" is necessary for pcurves and using TransferParameterProj
  //! aNum - number of edge in SeqE which corresponding to [fp,lp]
  Standard_EXPORT bool SplitEdge(const TopoDS_Edge&                     edge,
                                 const double                           fp,
                                 const TopoDS_Vertex&                   V1,
                                 const double                           lp,
                                 const TopoDS_Vertex&                   V2,
                                 const TopoDS_Face&                     face,
                                 NCollection_Sequence<TopoDS_Shape>&    SeqE,
                                 int&                                   aNum,
                                 const occ::handle<ShapeBuild_ReShape>& context,
                                 const double                           tol3d,
                                 const double                           tol2d) const;
};

#endif // _ShapeFix_SplitTool_HeaderFile
