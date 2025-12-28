// Created on: 2000-11-16
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_Tools_HeaderFile
#define _IntTools_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopAbs_State.hxx>
#include <IntTools_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
class TopoDS_Vertex;
class TopoDS_Wire;
class TopoDS_Face;
class gp_Pnt2d;
class TopoDS_Edge;
class IntTools_CommonPrt;
class gp_Pnt;
class IntTools_Curve;
class gp_Dir;
class Geom_Curve;
class Bnd_Box;
class IntTools_Range;
class gp_Lin;
class gp_Pln;
class Geom2d_Curve;
class Geom_Surface;

//! The class contains handy static functions
//! dealing with the geometry and topology.
class IntTools_Tools
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes distance between vertex V1 and vertex V2,
  //! if the distance is less than sum of vertex tolerances
  //! returns zero,
  //! otherwise returns negative value
  Standard_EXPORT static int ComputeVV(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);

  //! Returns True if wire aW contains edges
  //! with INTERNAL orientation
  Standard_EXPORT static bool HasInternalEdge(const TopoDS_Wire& aW);

  //! Build a face based on surface of given face aF
  //! and bounded by wire aW
  Standard_EXPORT static void MakeFaceFromWireAndFace(const TopoDS_Wire& aW,
                                                      const TopoDS_Face& aF,
                                                      TopoDS_Face&       aFNew);

  Standard_EXPORT static TopAbs_State ClassifyPointByFace(const TopoDS_Face& aF, const gp_Pnt2d& P);

  //! Computes square distance between a point on the edge E
  //! corresponded to parameter t and vertices of edge E.
  //! Returns True if this distance is less than square
  //! tolerance of vertex, otherwise returns false.
  Standard_EXPORT static bool IsVertex(const TopoDS_Edge& E, const double t);

  //! Returns True if square distance between vertex V
  //! and a point on the edge E corresponded to parameter t
  //! is less than square tolerance of V
  Standard_EXPORT static bool IsVertex(const TopoDS_Edge&   E,
                                       const TopoDS_Vertex& V,
                                       const double         t);

  //! Returns True if IsVertx for middle parameter of fist range
  //! and first edge returns True
  //! and if IsVertex for middle parameter of second range and
  //! second range returns True,
  //! otherwise returns False
  Standard_EXPORT static bool IsVertex(const IntTools_CommonPrt& aCmnPrt);

  //! Gets boundary of parameters of E1 and E2.
  //! Computes 3d points on each corresponded to average parameters.
  //! Returns True if distance between computed points is less than
  //! sum of edge tolerance, otherwise returns False.
  Standard_EXPORT static bool IsMiddlePointsEqual(const TopoDS_Edge& E1, const TopoDS_Edge& E2);

  //! Returns True if the distance between point aP and
  //! vertex aV is less or equal to sum of aTolPV and
  //! vertex tolerance, otherwise returns False
  Standard_EXPORT static bool IsVertex(const gp_Pnt&        aP,
                                       const double         aTolPV,
                                       const TopoDS_Vertex& aV);

  //! Returns some value between aFirst and aLast
  Standard_EXPORT static double IntermediatePoint(const double aFirst, const double aLast);

  //! Split aC by average parameter if aC is closed in 3D.
  //! Returns positive value if splitting has been done,
  //! otherwise returns zero.
  Standard_EXPORT static int SplitCurve(const IntTools_Curve&                 aC,
                                        NCollection_Sequence<IntTools_Curve>& aS);

  //! Puts curves from aSIn to aSOut except those curves that
  //! are coincide with first curve from aSIn.
  Standard_EXPORT static void RejectLines(const NCollection_Sequence<IntTools_Curve>& aSIn,
                                          NCollection_Sequence<IntTools_Curve>&       aSOut);

  //! Returns True if D1 and D2 coincide
  Standard_EXPORT static bool IsDirsCoinside(const gp_Dir& D1, const gp_Dir& D2);

  //! Returns True if D1 and D2 coincide with given tolerance
  Standard_EXPORT static bool IsDirsCoinside(const gp_Dir& D1, const gp_Dir& D2, const double aTol);

  //! Returns True if aC is BoundedCurve from Geom and
  //! the distance between first point
  //! of the curve aC and last point
  //! is less than 1.e-12
  Standard_EXPORT static bool IsClosed(const occ::handle<Geom_Curve>& aC);

  //! Returns adaptive tolerance for given aTolBase
  //! if aC is trimmed curve and basis curve is parabola,
  //! otherwise returns value of aTolBase
  Standard_EXPORT static double CurveTolerance(const occ::handle<Geom_Curve>& aC,
                                               const double                   aTolBase);

  //! Checks if the curve is not covered by the default tolerance (confusion).
  //! Builds bounding box for the curve and stores it into <theBox>.
  Standard_EXPORT static bool CheckCurve(const IntTools_Curve& theCurve, Bnd_Box& theBox);

  Standard_EXPORT static bool IsOnPave(const double          theT,
                                       const IntTools_Range& theRange,
                                       const double          theTol);

  Standard_EXPORT static void VertexParameters(const IntTools_CommonPrt& theCP,
                                               double&                   theT1,
                                               double&                   theT2);

  Standard_EXPORT static void VertexParameter(const IntTools_CommonPrt& theCP, double& theT);

  Standard_EXPORT static bool IsOnPave1(const double          theT,
                                        const IntTools_Range& theRange,
                                        const double          theTol);

  //! Checks if the range <theR> interfere with the range <theRRef>
  Standard_EXPORT static bool IsInRange(const IntTools_Range& theRRef,
                                        const IntTools_Range& theR,
                                        const double          theTol);

  Standard_EXPORT static int SegPln(const gp_Lin& theLin,
                                    const double  theTLin1,
                                    const double  theTLin2,
                                    const double  theTolLin,
                                    const gp_Pln& thePln,
                                    const double  theTolPln,
                                    gp_Pnt&       theP,
                                    double&       theT,
                                    double&       theTolP,
                                    double&       theTmin,
                                    double&       theTmax);

  //! Computes the max distance between points
  //! taken from 3D and 2D curves by the same parameter
  Standard_EXPORT static bool ComputeTolerance(const occ::handle<Geom_Curve>&   theCurve3D,
                                               const occ::handle<Geom2d_Curve>& theCurve2D,
                                               const occ::handle<Geom_Surface>& theSurf,
                                               const double                     theFirst,
                                               const double                     theLast,
                                               double&                          theMaxDist,
                                               double&                          theMaxPar,
                                               const double theTolRange = Precision::PConfusion(),
                                               const bool   theToRunParallel = false);

  //! Computes the correct Intersection range for
  //! Line/Line, Line/Plane and Plane/Plane intersections
  Standard_EXPORT static double ComputeIntRange(const double theTol1,
                                                const double theTol2,
                                                const double theAngle);
};

#endif // _IntTools_Tools_HeaderFile
