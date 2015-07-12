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

#ifndef _AIS_HeaderFile
#define _AIS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <AIS_KindOfSurface.hxx>
#include <Prs3d_Drawer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
class gp_Pnt;
class TopoDS_Shape;
class gp_Lin;
class Geom_Curve;
class TopoDS_Edge;
class Geom_Plane;
class TopoDS_Vertex;
class TopoDS_Face;
class gp_Pln;
class Geom_Surface;
class gp_Dir;
class Bnd_Box;
class gp_Elips;
class Prs3d_Presentation;
class AIS_Triangulation;
class AIS_InteractiveContext;
class AIS_GraphicTool;
class AIS_LocalContext;
class AIS_LocalStatus;
class AIS_GlobalStatus;
class AIS_InteractiveObject;
class AIS_Point;
class AIS_Axis;
class AIS_Trihedron;
class AIS_PlaneTrihedron;
class AIS_Line;
class AIS_Circle;
class AIS_Plane;
class AIS_Shape;
class AIS_ConnectedInteractive;
class AIS_MultipleConnectedInteractive;
class AIS_DimensionOwner;
class AIS_Relation;
class AIS_EllipseRadiusDimension;
class AIS_MaxRadiusDimension;
class AIS_MinRadiusDimension;
class AIS_Chamf2dDimension;
class AIS_Chamf3dDimension;
class AIS_OffsetDimension;
class AIS_FixRelation;
class AIS_PerpendicularRelation;
class AIS_ParallelRelation;
class AIS_TangentRelation;
class AIS_ConcentricRelation;
class AIS_IdenticRelation;
class AIS_SymmetricRelation;
class AIS_MidPointRelation;
class AIS_EqualRadiusRelation;
class AIS_EqualDistanceRelation;
class AIS_TypeFilter;
class AIS_SignatureFilter;
class AIS_ExclusionFilter;
class AIS_AttributeFilter;
class AIS_C0RegularityFilter;
class AIS_BadEdgeFilter;
class AIS_Selection;


//! Application Interactive Services provide the means to
//! create links between an application GUI viewer and
//! the packages which are used to manage selection
//! and presentation. The tools AIS defined in order to
//! do this include different sorts of entities: both the
//! selectable viewable objects themselves and the
//! context and attribute managers to define their
//! selection and display.
//! To orient the user as he works in a modeling
//! environment, views and selections must be
//! comprehensible. There must be several different sorts
//! of selectable and viewable object defined. These must
//! also be interactive, that is, connecting graphic
//! representation and the underlying reference
//! geometry. These entities are called Interactive
//! Objects, and are divided into four types:
//! -   the Datum
//! -   the Relation
//! -   the Object
//! -   None.
//! The Datum groups together the construction elements
//! such as lines, circles, points, trihedra, plane trihedra,
//! planes and axes.
//! The Relation is made up of constraints on one or
//! more interactive shapes and the corresponding
//! reference geometry. For example, you might want to
//! constrain two edges in a parallel relation. This
//! contraint is considered as an object in its own right,
//! and is shown as a sensitive primitive. This takes the
//! graphic form of a perpendicular arrow marked with
//! the || symbol and lying between the two edges.
//! The Object type includes topological shapes, and
//! connections between shapes.
//! None, in order not to eliminate the object, tells the
//! application to look further until it finds an object
//! definition in its generation which is accepted.
//! Inside these categories, you have the possibility
//! of   an additional characterization by means of a
//! signature. The signature provides an index to the
//! further characterization. By default, the   Interactive
//! Object has a None type and a signature of 0
//! (equivalent to None.) If you want to give a particular
//! type and signature to your interactive object, you must
//! redefine the two virtual methods: Type and Signature.
//! In the C++ inheritance structure of the package, each
//! class representing a specific Interactive Object
//! inherits AIS_InteractiveObject. Among these
//! inheriting classes, AIS_Relation functions as the
//! abstract mother class for tinheriting classes defining
//! display of specific relational constraints and types of
//! dimension. Some of these include:
//! -   display of constraints based on relations of
//! symmetry, tangency, parallelism and concentricity
//! -   display of dimensions for angles, offsets,
//! diameters, radii and chamfers.
//! No viewer can show everything at once with any
//! coherence or clarity. Views must be managed
//! carefully both sequentially and at any given instant.
//! Another function of the view is that of a context to
//! carry out design in. The design changes are applied
//! to the objects in the view and then extended to the
//! underlying reference geometry by a solver. To make
//! sense of this complicated visual data, several display
//! and selection tools are required. To facilitate
//! management, each object and each construction
//! element has a selection priority. There are also
//! means to modify the default priority.
//! To define an environment of dynamic detection, you
//! can use standard filter classes or create your own. A
//! filter questions the owner of the sensitive primitive in
//! local context to determine if it has the the desired
//! qualities. If it answers positively, it is kept. If not, it is rejected.
//! The standard filters supplied in AIS include:
//! AIS_AttributeFilter
//! AIS_SignatureFilter
//! AIS_TypeFilter.
//! Only the type filter can be used in the default
//! operating mode, the neutral point. The others can
//! only be used in open local contexts.
//! Neutral point and local context constitute the two
//! operating modes of the central entity which pilots
//! visualizations and selections, the Interactive Context.
//! It is linked to a main viewer and if you like, a trash bin
//! viewer as well.
//! The neutral point, which is the default mode, allows
//! you to easily visualize and select interactive objects
//! which have been loaded into the context. Opening
//! local contexts allows you to prepare and use a
//! temporary selection environment without disturbing
//! the neutral point. A set of functions allows you to
//! choose the interactive objects which you want to act
//! on, the selection modes which you want to activate,
//! and the temporary visualizations which you will
//! execute. When the operation is finished, you close the
//! current local context and return to the state in which
//! you were before opening it (neutral point or previous
//! local context).
//! An interactive object can have a certain number of
//! graphic attributes which are specific to it, such as
//! visualization mode, color, and material. By the same
//! token, the interactive context has a set of graphic
//! attributes, the Drawer which is valid by default for the
//! objects it controls.   When an interactive object is
//! visualized, the required graphic attributes are first
//! taken from the object's own Drawer if one exists, or
//! from the context drawer for the others.
class AIS 
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
  Standard_EXPORT static Standard_Boolean GetPlaneFromFace (const TopoDS_Face& aFace, gp_Pln& aPlane, Handle(Geom_Surface)& aSurf, AIS_KindOfSurface& aSurfType, Standard_Real& Offset);
  
  Standard_EXPORT static void InitFaceLength (const TopoDS_Face& aFace, gp_Pln& aPlane, Handle(Geom_Surface)& aSurface, AIS_KindOfSurface& aSurfaceType, Standard_Real& anOffset);
  
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
  Standard_EXPORT static Standard_Boolean InitAngleBetweenCurvilinearFaces (const TopoDS_Face& theFirstFace, const TopoDS_Face& theSecondFace, const AIS_KindOfSurface theFirstSurfType, const AIS_KindOfSurface theSecondSurfType, gp_Pnt& theCenter, gp_Pnt& theFirstAttach, gp_Pnt& theSecondAttach, const Standard_Boolean theIsFirstPointSet = Standard_False);
  
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




protected:





private:




friend class AIS_Triangulation;
friend class AIS_InteractiveContext;
friend class AIS_GraphicTool;
friend class AIS_LocalContext;
friend class AIS_LocalStatus;
friend class AIS_GlobalStatus;
friend class AIS_InteractiveObject;
friend class AIS_Point;
friend class AIS_Axis;
friend class AIS_Trihedron;
friend class AIS_PlaneTrihedron;
friend class AIS_Line;
friend class AIS_Circle;
friend class AIS_Plane;
friend class AIS_Shape;
friend class AIS_ConnectedInteractive;
friend class AIS_MultipleConnectedInteractive;
friend class AIS_DimensionOwner;
friend class AIS_Relation;
friend class AIS_EllipseRadiusDimension;
friend class AIS_MaxRadiusDimension;
friend class AIS_MinRadiusDimension;
friend class AIS_Chamf2dDimension;
friend class AIS_Chamf3dDimension;
friend class AIS_OffsetDimension;
friend class AIS_FixRelation;
friend class AIS_PerpendicularRelation;
friend class AIS_ParallelRelation;
friend class AIS_TangentRelation;
friend class AIS_ConcentricRelation;
friend class AIS_IdenticRelation;
friend class AIS_SymmetricRelation;
friend class AIS_MidPointRelation;
friend class AIS_EqualRadiusRelation;
friend class AIS_EqualDistanceRelation;
friend class AIS_TypeFilter;
friend class AIS_SignatureFilter;
friend class AIS_ExclusionFilter;
friend class AIS_AttributeFilter;
friend class AIS_C0RegularityFilter;
friend class AIS_BadEdgeFilter;
friend class AIS_Selection;

};







#endif // _AIS_HeaderFile
