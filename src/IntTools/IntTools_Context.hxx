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

#ifndef _IntTools_Context_HeaderFile
#define _IntTools_Context_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BOPCol_BaseAllocator.hxx>
#include <BOPCol_DataMapOfShapeAddress.hxx>
#include <BOPCol_DataMapOfTransientAddress.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <MMgt_TShared.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>
class IntTools_FClass2d;
class TopoDS_Face;
class GeomAPI_ProjectPointOnSurf;
class GeomAPI_ProjectPointOnCurve;
class TopoDS_Edge;
class Geom_Curve;
class IntTools_SurfaceRangeLocalizeData;
class BRepClass3d_SolidClassifier;
class TopoDS_Solid;
class Geom2dHatch_Hatcher;
class gp_Pnt;
class TopoDS_Vertex;
class gp_Pnt2d;
class IntTools_Curve;
class Bnd_Box;
class TopoDS_Shape;


class IntTools_Context;
DEFINE_STANDARD_HANDLE(IntTools_Context, MMgt_TShared)


//! The intersection Context contains geometrical
//! and topological toolkit (classifiers, projectors, etc).
//! The intersection Context is for caching the tools
//! to increase the performance.
class IntTools_Context : public MMgt_TShared
{

public:

  
  Standard_EXPORT IntTools_Context();
Standard_EXPORT virtual  ~IntTools_Context();
  
  Standard_EXPORT IntTools_Context(const BOPCol_BaseAllocator& theAllocator);
  

  //! Returns a reference to point classifier
  //! for given face
  Standard_EXPORT IntTools_FClass2d& FClass2d (const TopoDS_Face& aF);
  

  //! Returns a reference to point projector
  //! for given face
  Standard_EXPORT GeomAPI_ProjectPointOnSurf& ProjPS (const TopoDS_Face& aF);
  

  //! Returns a reference to point projector
  //! for given edge
  Standard_EXPORT GeomAPI_ProjectPointOnCurve& ProjPC (const TopoDS_Edge& aE);
  

  //! Returns a reference to point projector
  //! for given curve
  Standard_EXPORT GeomAPI_ProjectPointOnCurve& ProjPT (const Handle(Geom_Curve)& aC);
  

  //! Returns a reference to surface localization data
  //! for given face
  Standard_EXPORT IntTools_SurfaceRangeLocalizeData& SurfaceData (const TopoDS_Face& aF);
  

  //! Returns a reference to solid classifier
  //! for given solid
  Standard_EXPORT BRepClass3d_SolidClassifier& SolidClassifier (const TopoDS_Solid& aSolid);
  

  //! Returns a reference to 2D hatcher
  //! for given face
  Standard_EXPORT Geom2dHatch_Hatcher& Hatcher (const TopoDS_Face& aF);
  

  //! Computes parameter of the Point theP on
  //! the edge aE.
  //! Returns zero if the distance between point
  //! and edge is less than sum of tolerance value of edge and theTopP,
  //! otherwise and for following conditions returns
  //! negative value
  //! 1. the edge is degenerated (-1)
  //! 2. the edge does not contain 3d curve and pcurves (-2)
  //! 3. projection algorithm failed (-3)
  Standard_EXPORT Standard_Integer ComputePE (const gp_Pnt& theP, const Standard_Real theTolP, const TopoDS_Edge& theE, Standard_Real& theT);
  

  //! Computes parameter of the vertex aV on
  //! the edge aE.
  //! Returns zero if the distance between vertex
  //! and edge is less than sum of tolerances,
  //! otherwise and for following conditions returns
  //! negative value
  //! 1. the edge is degenerated (-1)
  //! 2. the edge does not contain 3d curve and pcurves (-2)
  //! 3. projection algorithm failed (-3)
  Standard_EXPORT Standard_Integer ComputeVE (const TopoDS_Vertex& aV, const TopoDS_Edge& aE, Standard_Real& aT);
  

  //! Computes UV parameters of the vertex aV on face aF
  //! Returns zero if the distance between vertex and face is
  //! less than or equal the sum of tolerances and the projection
  //! point lays inside boundaries of the face.
  //! For following conditions returns negative value
  //! 1. projection algorithm failed (-1)
  //! 2. distance is more than sum of tolerances (-2)
  //! 3. projection point out or on the boundaries of face (-3)
  Standard_EXPORT Standard_Integer ComputeVF (const TopoDS_Vertex& aV, const TopoDS_Face& aF, Standard_Real& U, Standard_Real& V);
  

  //! Returns the state of the point aP2D
  //! relative to face aF
  Standard_EXPORT TopAbs_State StatePointFace (const TopoDS_Face& aF, const gp_Pnt2d& aP2D);
  

  //! Returns true if the point aP2D is
  //! inside the boundaries of the face aF,
  //! otherwise returns false
  Standard_EXPORT Standard_Boolean IsPointInFace (const TopoDS_Face& aF, const gp_Pnt2d& aP2D);
  

  //! Returns true if the point aP2D is
  //! inside the boundaries of the face aF,
  //! otherwise returns false
  Standard_EXPORT Standard_Boolean IsPointInFace (const gp_Pnt& aP3D, const TopoDS_Face& aF, const Standard_Real aTol);
  

  //! Returns true if the point aP2D is
  //! inside or on the boundaries of aF
  Standard_EXPORT Standard_Boolean IsPointInOnFace (const TopoDS_Face& aF, const gp_Pnt2d& aP2D);
  

  //! Returns true if the distance between point aP3D
  //! and face aF is less or equal to tolerance aTol
  //! and projection point is inside or on the boundaries
  //! of the face aF
  Standard_EXPORT Standard_Boolean IsValidPointForFace (const gp_Pnt& aP3D, const TopoDS_Face& aF, const Standard_Real aTol);
  

  //! Returns true if IsValidPointForFace returns true
  //! for both face aF1 and aF2
  Standard_EXPORT Standard_Boolean IsValidPointForFaces (const gp_Pnt& aP3D, const TopoDS_Face& aF1, const TopoDS_Face& aF2, const Standard_Real aTol);
  

  //! Returns true if IsValidPointForFace returns true
  //! for some 3d point that lay on the curve aIC bounded by
  //! parameters aT1 and aT2
  Standard_EXPORT Standard_Boolean IsValidBlockForFace (const Standard_Real aT1, const Standard_Real aT2, const IntTools_Curve& aIC, const TopoDS_Face& aF, const Standard_Real aTol);
  

  //! Returns true if IsValidBlockForFace returns true
  //! for both faces aF1 and aF2
  Standard_EXPORT Standard_Boolean IsValidBlockForFaces (const Standard_Real aT1, const Standard_Real aT2, const IntTools_Curve& aIC, const TopoDS_Face& aF1, const TopoDS_Face& aF2, const Standard_Real aTol);
  

  //! Computes parameter of the vertex aV on
  //! the curve aIC.
  //! Returns true if the distance between vertex and
  //! curve is less than sum of tolerance of aV and aTolC,
  //! otherwise or if projection algorithm failed
  //! returns false (in this case aT isn't significant)
  Standard_EXPORT Standard_Boolean IsVertexOnLine (const TopoDS_Vertex& aV, const IntTools_Curve& aIC, const Standard_Real aTolC, Standard_Real& aT);
  

  //! Computes parameter of the vertex aV on
  //! the curve aIC.
  //! Returns true if the distance between vertex and
  //! curve is less than sum of tolerance of aV and aTolC,
  //! otherwise or if projection algorithm failed
  //! returns false (in this case aT isn't significant)
  Standard_EXPORT Standard_Boolean IsVertexOnLine (const TopoDS_Vertex& aV, const Standard_Real aTolV, const IntTools_Curve& aIC, const Standard_Real aTolC, Standard_Real& aT);
  

  //! Computes parameter of the point aP on
  //! the edge aE.
  //! Returns false if projection algorithm failed
  //! other wiese returns true.
  Standard_EXPORT Standard_Boolean ProjectPointOnEdge (const gp_Pnt& aP, const TopoDS_Edge& aE, Standard_Real& aT);
  
  Standard_EXPORT Bnd_Box& BndBox (const TopoDS_Shape& theS);
  
  //! Returns true if the solid <theFace> has
  //! infinite bounds
  Standard_EXPORT Standard_Boolean IsInfiniteFace (const TopoDS_Face& theFace);
  
  //! Sets tolerance to be used for projection of point on surface.
  //! Clears map of already cached projectors in order to maintain
  //! correct value for all projectors
  Standard_EXPORT void SetPOnSProjectionTolerance (const Standard_Real theValue);




  DEFINE_STANDARD_RTTI(IntTools_Context,MMgt_TShared)

protected:


  BOPCol_BaseAllocator myAllocator;
  BOPCol_DataMapOfShapeAddress myFClass2dMap;
  BOPCol_DataMapOfShapeAddress myProjPSMap;
  BOPCol_DataMapOfShapeAddress myProjPCMap;
  BOPCol_DataMapOfShapeAddress mySClassMap;
  BOPCol_DataMapOfTransientAddress myProjPTMap;
  BOPCol_DataMapOfShapeAddress myHatcherMap;
  BOPCol_DataMapOfShapeAddress myProjSDataMap;
  BOPCol_DataMapOfShapeAddress myBndBoxDataMap;
  Standard_Integer myCreateFlag;
  Standard_Real myPOnSTolerance;


private:

  
  //! Clears map of already cached projectors.
  Standard_EXPORT void clearCachedPOnSProjectors();



};







#endif // _IntTools_Context_HeaderFile
