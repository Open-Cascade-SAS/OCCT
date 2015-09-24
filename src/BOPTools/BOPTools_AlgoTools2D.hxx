// Created by: Peter KURNEV
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

#ifndef _BOPTools_AlgoTools2D_HeaderFile
#define _BOPTools_AlgoTools2D_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Edge;
class TopoDS_Face;
class gp_Vec;
class Geom2d_Curve;
class Geom_Curve;
class BRepAdaptor_Surface;
class ProjLib_ProjectedCurve;
class IntTools_Context;



//! The class contains handy static functions
//! dealing with the topology
//! This is the copy of the BOPTools_AlgoTools2D.cdl
class BOPTools_AlgoTools2D 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Compute P-Curve for the edge <aE> on the face <aF>
  //! Raises exception Standard_ConstructionError if projection algorithm fails
  Standard_EXPORT static void BuildPCurveForEdgeOnFace (const TopoDS_Edge& aE, const TopoDS_Face& aF);
  

  //! Compute tangent for the edge  <aE> [in 3D]  at parameter <aT>
  Standard_EXPORT static Standard_Boolean EdgeTangent (const TopoDS_Edge& anE, const Standard_Real aT, gp_Vec& Tau);
  

  //! Compute surface parameters <U,V> of the face <aF>
  //! for  the point from the edge <aE> at parameter <aT>.
  //! If <aE> has't pcurve on surface, algorithm tries to get it by
  //! projection and can
  //! raise exception Standard_ConstructionError if projection algorithm fails
  Standard_EXPORT static void PointOnSurface (const TopoDS_Edge& aE, const TopoDS_Face& aF, const Standard_Real aT, Standard_Real& U, Standard_Real& V);
  

  //! Get P-Curve <aC>  for the edge <aE> on surface <aF> .
  //! If the P-Curve does not exist, build  it using Make2D().
  //! [aToler] - reached tolerance
  //! Raises exception Standard_ConstructionError if algorithm Make2D() fails
  Standard_EXPORT static void CurveOnSurface (const TopoDS_Edge& aE, const TopoDS_Face& aF, Handle(Geom2d_Curve)& aC, Standard_Real& aToler);
  

  //! Get P-Curve <aC>  for the edge <aE> on surface <aF> .
  //! If the P-Curve does not exist, build  it using Make2D().
  //! [aFirst, aLast] - range of the P-Curve
  //! [aToler] - reached tolerance
  //! Raises exception Standard_ConstructionError if algorithm Make2D() fails
  Standard_EXPORT static void CurveOnSurface (const TopoDS_Edge& aE, const TopoDS_Face& aF, Handle(Geom2d_Curve)& aC, Standard_Real& aFirst, Standard_Real& aLast, Standard_Real& aToler);
  

  //! Returns TRUE if the edge <aE>  has  P-Curve <aC>
  //! on surface <aF> .
  //! [aFirst, aLast] - range of the P-Curve
  //! [aToler] - reached tolerance
  //! If the P-Curve does not exist, aC.IsNull()=TRUE.
  Standard_EXPORT static Standard_Boolean HasCurveOnSurface (const TopoDS_Edge& aE, const TopoDS_Face& aF, Handle(Geom2d_Curve)& aC, Standard_Real& aFirst, Standard_Real& aLast, Standard_Real& aToler);
  

  //! Returns TRUE if the edge <aE>  has  P-Curve <aC>
  //! on surface <aF> .
  //! If the P-Curve does not exist, aC.IsNull()=TRUE.
  Standard_EXPORT static Standard_Boolean HasCurveOnSurface (const TopoDS_Edge& aE, const TopoDS_Face& aF);
  

  //! Adjust P-Curve <aC2D> (3D-curve <C3D>) on surface <aF> .
  Standard_EXPORT static void AdjustPCurveOnFace (const TopoDS_Face& aF, const Handle(Geom_Curve)& C3D, const Handle(Geom2d_Curve)& aC2D, Handle(Geom2d_Curve)& aC2DA);
  

  //! Adjust P-Curve <aC2D> (3D-curve <C3D>) on surface <aF> .
  //! [aT1,  aT2] - range to adjust
  Standard_EXPORT static void AdjustPCurveOnFace (const TopoDS_Face& aF, const Standard_Real aT1, const Standard_Real aT2, const Handle(Geom2d_Curve)& aC2D, Handle(Geom2d_Curve)& aC2DA);
  

  //! Adjust P-Curve <aC2D> (3D-curve <C3D>) on surface <aF> .
  //! [aT1,  aT2] - range to adjust
  Standard_EXPORT static void AdjustPCurveOnFace (const BRepAdaptor_Surface& aF, const Standard_Real aT1, const Standard_Real aT2, const Handle(Geom2d_Curve)& aC2D, Handle(Geom2d_Curve)& aC2DA);
  

  //! Compute intermediate  value in  between [aFirst, aLast] .
  Standard_EXPORT static Standard_Real IntermediatePoint (const Standard_Real aFirst, const Standard_Real aLast);
  

  //! Compute intermediate value of parameter for the edge <anE>.
  Standard_EXPORT static Standard_Real IntermediatePoint (const TopoDS_Edge& anE);
  
  //! Build pcurve of edge on face if the surface is plane, and update the edge.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane (const TopoDS_Edge& theE, const TopoDS_Face& theF);
  
  //! Build pcurve of edge on face if the surface is plane, but do not update the edge.
  //! The output are the pcurve and the flag telling that pcurve was built.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane (const TopoDS_Edge& theE, const TopoDS_Face& theF,
                                                         Handle(Geom2d_Curve)& aC2D, Standard_Boolean& bToUpdate);

  Standard_EXPORT static void BuildPCurveForEdgesOnPlane (const BOPCol_ListOfShape& theLE, const TopoDS_Face& theF);
  

  //! Make P-Curve <aC> for the edge <aE> on surface <aF> .
  //! [aFirst, aLast] - range of the P-Curve
  //! [aToler] - reached tolerance
  //! Raises exception Standard_ConstructionError if algorithm fails
  Standard_EXPORT static void Make2D (const TopoDS_Edge& aE, const TopoDS_Face& aF, Handle(Geom2d_Curve)& aC, Standard_Real& aFirst, Standard_Real& aLast, Standard_Real& aToler);
  

  //! Make P-Curve <aC> for the 3D-curve <C3D> on surface <aF> .
  //! [aToler] - reached tolerance
  //! Raises exception Standard_ConstructionError if projection algorithm fails
  Standard_EXPORT static void MakePCurveOnFace (const TopoDS_Face& aF, const Handle(Geom_Curve)& C3D, Handle(Geom2d_Curve)& aC, Standard_Real& aToler);
  

  //! Make P-Curve <aC> for the 3D-curve <C3D> on surface <aF> .
  //! [aT1,  aT2] - range to build
  //! [aToler] - reached tolerance
  //! Raises exception Standard_ConstructionError if projection algorithm fails
  Standard_EXPORT static void MakePCurveOnFace (const TopoDS_Face& aF, const Handle(Geom_Curve)& C3D, const Standard_Real aT1, const Standard_Real aT2, Handle(Geom2d_Curve)& aC, Standard_Real& aToler);
  

  //! Make empty  P-Curve <aC> of relevant to <PC> type
  Standard_EXPORT static void MakePCurveOfType (const ProjLib_ProjectedCurve& PC, Handle(Geom2d_Curve)& aC);
  

  //! Attach P-Curve from the edge <aEold> on surface <aF>
  //! to the edge <aEnew>
  //! Returns 0 in case of success
  Standard_EXPORT static Standard_Integer AttachExistingPCurve (const TopoDS_Edge& aEold, const TopoDS_Edge& aEnew, const TopoDS_Face& aF, const Handle(IntTools_Context)& aCtx);




protected:





private:





};







#endif // _BOPTools_AlgoTools2D_HeaderFile
