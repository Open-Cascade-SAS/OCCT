// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _PrsDim_HeaderFile
#define _PrsDim_HeaderFile

#include <PrsDim_KindOfSurface.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Bnd_Box;
class Geom_Curve;
class Geom_Plane;
class Geom_Surface;
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Shape;
class TopoDS_Vertex;

//! Auxiliary methods for computing dimensions.
class PrsDim 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Returns the nearest point in a shape. This is used by
  //! several classes in calculation of dimensions.
  Standard_EXPORT static gp_Pnt Nearest (const TopoDS_Shape& aShape, const gp_Pnt& aPoint);

  //! @return the nearest point on the line.
  Standard_EXPORT static gp_Pnt Nearest (const gp_Lin& theLine, const gp_Pnt& thePoint);

  //! For the given point finds nearest point on the curve,
  //! @return TRUE if found point is belongs to the curve
  //! and FALSE otherwise.
  Standard_EXPORT static Standard_Boolean Nearest (const Handle(Geom_Curve)& theCurve, const gp_Pnt& thePoint, const gp_Pnt& theFirstPoint, const gp_Pnt& theLastPoint, gp_Pnt& theNearestPoint);
  
  Standard_EXPORT static gp_Pnt Farest (const TopoDS_Shape& aShape, const gp_Pnt& aPoint);
  
  //! Used by 2d Relation only
  //! Computes the 3d geometry of <anEdge> in the current WorkingPlane
  //! and the extremities if any
  //! Return TRUE if ok.
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theEdge, Handle(Geom_Curve)& theCurve, gp_Pnt& theFirstPnt, gp_Pnt& theLastPnt);
  
  //! Used by dimensions only.
  //! Computes the 3d geometry of <anEdge>.
  //! Return TRUE if ok.
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theEdge, Handle(Geom_Curve)& theCurve, gp_Pnt& theFirstPnt, gp_Pnt& theLastPnt, Standard_Boolean& theIsInfinite);
  
  //! Used by 2d Relation only
  //! Computes the 3d geometry of <anEdge> in the current WorkingPlane
  //! and the extremities if any.
  //! If <aCurve> is not in the current plane, <extCurve> contains
  //! the not projected curve associated to <anEdge>.
  //! If <anEdge> is infinite, <isinfinite> = true and the 2
  //! parameters <FirstPnt> and <LastPnt> have no signification.
  //! Return TRUE if ok.
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theEdge, Handle(Geom_Curve)& theCurve, gp_Pnt& theFirstPnt, gp_Pnt& theLastPnt, Handle(Geom_Curve)& theExtCurve, Standard_Boolean& theIsInfinite, Standard_Boolean& theIsOnPlane, const Handle(Geom_Plane)& thePlane);
  
  //! Used by 2d Relation only
  //! Computes the 3d geometry of <anEdge> in the current WorkingPlane
  //! and the extremities if any
  //! Return TRUE if ok.
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theFirstEdge, const TopoDS_Edge& theSecondEdge, Handle(Geom_Curve)& theFirstCurve, Handle(Geom_Curve)& theSecondCurve, gp_Pnt& theFirstPnt1, gp_Pnt& theLastPnt1, gp_Pnt& theFirstPnt2, gp_Pnt& theLastPnt2, const Handle(Geom_Plane)& thePlane);
  
  //! Used  by  dimensions  only.Computes  the  3d geometry
  //! of<anEdge1> and <anEdge2> and checks if they are infinite.
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theFirstEdge, const TopoDS_Edge& theSecondEdge, Handle(Geom_Curve)& theFirstCurve, Handle(Geom_Curve)& theSecondCurve, gp_Pnt& theFirstPnt1, gp_Pnt& theLastPnt1, gp_Pnt& theFirstPnt2, gp_Pnt& theLastPnt2, Standard_Boolean& theIsinfinite1, Standard_Boolean& theIsinfinite2);
  
  //! Used  by  2d Relation  only Computes  the  3d geometry
  //! of<anEdge1> and <anEdge2> in the current Plane and the
  //! extremities if any.   Return in ExtCurve  the 3d curve
  //! (not projected  in the  plane)  of the  first edge  if
  //! <indexExt> =1 or of the 2nd edge if <indexExt> = 2. If
  //! <indexExt> = 0, ExtCurve is Null.  if there is an edge
  //! external to the  plane,  <isinfinite> is true if  this
  //! edge is infinite.  So, the extremities of it are not
  //! significant.  Return TRUE if ok
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Edge& theFirstEdge, const TopoDS_Edge& theSecondEdge, Standard_Integer& theExtIndex, Handle(Geom_Curve)& theFirstCurve, Handle(Geom_Curve)& theSecondCurve, gp_Pnt& theFirstPnt1, gp_Pnt& theLastPnt1, gp_Pnt& theFirstPnt2, gp_Pnt& theLastPnt2, Handle(Geom_Curve)& theExtCurve, Standard_Boolean& theIsinfinite1, Standard_Boolean& theIsinfinite2, const Handle(Geom_Plane)& thePlane);
  
  //! Checks if aCurve belongs to aPlane; if not, projects aCurve in aPlane
  //! and returns aCurve;
  //! Return TRUE if ok
  Standard_EXPORT static Standard_Boolean ComputeGeomCurve (Handle(Geom_Curve)& aCurve, const Standard_Real first1, const Standard_Real last1, gp_Pnt& FirstPnt1, gp_Pnt& LastPnt1, const Handle(Geom_Plane)& aPlane, Standard_Boolean& isOnPlane);
  
  Standard_EXPORT static Standard_Boolean ComputeGeometry (const TopoDS_Vertex& aVertex, gp_Pnt& point, const Handle(Geom_Plane)& aPlane, Standard_Boolean& isOnPlane);
  
  //! Tryes to get Plane from Face.  Returns Surface of Face
  //! in aSurf.  Returns Standard_True  and Plane of Face in
  //! aPlane in following  cases:
  //! Face is Plane, Offset of Plane,
  //! Extrusion of Line  and Offset of  Extrusion of Line
  //! Returns pure type of Surface which can be:
  //! Plane, Cylinder, Cone, Sphere, Torus,
  //! SurfaceOfRevolution, SurfaceOfExtrusion
  Standard_EXPORT static Standard_Boolean GetPlaneFromFace (const TopoDS_Face& aFace, gp_Pln& aPlane, Handle(Geom_Surface)& aSurf, PrsDim_KindOfSurface& aSurfType, Standard_Real& Offset);
  
  Standard_EXPORT static void InitFaceLength (const TopoDS_Face& aFace, gp_Pln& aPlane, Handle(Geom_Surface)& aSurface, PrsDim_KindOfSurface& aSurfaceType, Standard_Real& anOffset);
  
  //! Finds attachment points on two curvilinear faces for length dimension.
  //! @param thePlaneDir [in] the direction on the dimension plane to
  //! compute the plane automatically. It will not be taken into account if
  //! plane is defined by user.
  Standard_EXPORT static void InitLengthBetweenCurvilinearFaces (const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace, Handle(Geom_Surface)& theFirstSurf, Handle(Geom_Surface)& theSecondSurf, gp_Pnt& theFirstAttach, gp_Pnt& theSecondAttach, gp_Dir& theDirOnPlane);
  
  //! Finds three points for the angle dimension between
  //! two planes.
  Standard_EXPORT static Standard_Boolean InitAngleBetweenPlanarFaces (const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace, gp_Pnt& theCenter, gp_Pnt& theFirstAttach, gp_Pnt& theSecondAttach, const Standard_Boolean theIsFirstPointSet = Standard_False);
  
  //! Finds three points for the angle dimension between
  //! two curvilinear surfaces.
  Standard_EXPORT static Standard_Boolean InitAngleBetweenCurvilinearFaces (const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace, const PrsDim_KindOfSurface theFirstSurfType, const PrsDim_KindOfSurface theSecondSurfType, gp_Pnt& theCenter, gp_Pnt& theFirstAttach, gp_Pnt& theSecondAttach, const Standard_Boolean theIsFirstPointSet = Standard_False);
  
  Standard_EXPORT static gp_Pnt ProjectPointOnPlane (const gp_Pnt& aPoint, const gp_Pln& aPlane);
  
  Standard_EXPORT static gp_Pnt ProjectPointOnLine (const gp_Pnt& aPoint, const gp_Lin& aLine);
  
  Standard_EXPORT static gp_Pnt TranslatePointToBound (const gp_Pnt& aPoint, const gp_Dir& aDir, const Bnd_Box& aBndBox);
  
  //! returns  True  if  point  with anAttachPar  is
  //! in  domain  of  arc
  Standard_EXPORT static Standard_Boolean InDomain (const Standard_Real aFirstPar, const Standard_Real aLastPar, const Standard_Real anAttachPar);
  
  //! computes  nearest  to  ellipse  arc  apex
  Standard_EXPORT static gp_Pnt NearestApex (const gp_Elips& elips, const gp_Pnt& pApex, const gp_Pnt& nApex, const Standard_Real fpara, const Standard_Real lpara, Standard_Boolean& IsInDomain);
  
  //! computes  length  of  ellipse  arc  in  parametric  units
  Standard_EXPORT static Standard_Real DistanceFromApex (const gp_Elips& elips, const gp_Pnt& Apex, const Standard_Real par);
  
  Standard_EXPORT static void ComputeProjEdgePresentation (const Handle(Prs3d_Presentation)& aPres, const Handle(Prs3d_Drawer)& aDrawer, const TopoDS_Edge& anEdge, const Handle(Geom_Curve)& ProjCurve, const gp_Pnt& FirstP, const gp_Pnt& LastP, const Quantity_NameOfColor aColor = Quantity_NOC_PURPLE, const Standard_Real aWidth = 2, const Aspect_TypeOfLine aProjTOL = Aspect_TOL_DASH, const Aspect_TypeOfLine aCallTOL = Aspect_TOL_DOT);
  
  Standard_EXPORT static void ComputeProjVertexPresentation (const Handle(Prs3d_Presentation)& aPres, const Handle(Prs3d_Drawer)& aDrawer, const TopoDS_Vertex& aVertex, const gp_Pnt& ProjPoint, const Quantity_NameOfColor aColor = Quantity_NOC_PURPLE, const Standard_Real aWidth = 2, const Aspect_TypeOfMarker aProjTOM = Aspect_TOM_PLUS, const Aspect_TypeOfLine aCallTOL = Aspect_TOL_DOT);

};

#endif // _PrsDim_HeaderFile
