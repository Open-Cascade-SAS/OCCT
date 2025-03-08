// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_GeomTool_HeaderFile
#define _BRepMesh_GeomTool_HeaderFile

#include <BRepAdaptor_Surface.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <GeomAbs_IsoType.hxx>
#include <TopoDS_Edge.hxx>
#include <Precision.hxx>

class BRepAdaptor_Curve;
class gp_Pnt2d;
class BRepMesh_DefaultRangeSplitter;

//! Tool class accumulating common geometrical functions as well as
//! functionality using shape geometry to produce data necessary for
//! tessellation.
//! General aim is to calculate discretization points for the given
//! curve or iso curve of surface according to the specified parameters.
class BRepMesh_GeomTool
{
public:
  //! Enumerates states of segments intersection check.
  enum IntFlag
  {
    NoIntersection,
    Cross,
    EndPointTouch,
    PointOnSegment,
    Glued,
    Same
  };

public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! Initiates discretization of the given geometric curve.
  //! @param theCurve curve to be discretized.
  //! @param theFirstParam first parameter of the curve.
  //! @param theLastParam last parameter of the curve.
  //! @param theLinDeflection linear deflection.
  //! @param theAngDeflection angular deflection.
  //! @param theMinPointsNb minimum number of points to be produced.
  Standard_EXPORT BRepMesh_GeomTool(const BRepAdaptor_Curve& theCurve,
                                    const Standard_Real      theFirstParam,
                                    const Standard_Real      theLastParam,
                                    const Standard_Real      theLinDeflection,
                                    const Standard_Real      theAngDeflection,
                                    const Standard_Integer   theMinPointsNb = 2,
                                    const Standard_Real      theMinSize = Precision::Confusion());

  //! Constructor.
  //! Initiates discretization of geometric curve corresponding
  //! to iso curve of the given surface.
  //! @param theSurface surface the iso curve to be taken from.
  //! @param theIsoType type of iso curve to be used, U or V.
  //! @param theParamIso parameter on the surface specifying the iso curve.
  //! @param theFirstParam first parameter of the curve.
  //! @param theLastParam last parameter of the curve.
  //! @param theLinDeflection linear deflection.
  //! @param theAngDeflection angular deflection.
  //! @param theMinPointsNb minimum number of points to be produced.
  Standard_EXPORT BRepMesh_GeomTool(const Handle(BRepAdaptor_Surface)& theSurface,
                                    const GeomAbs_IsoType              theIsoType,
                                    const Standard_Real                theParamIso,
                                    const Standard_Real                theFirstParam,
                                    const Standard_Real                theLastParam,
                                    const Standard_Real                theLinDeflection,
                                    const Standard_Real                theAngDeflection,
                                    const Standard_Integer             theMinPointsNb = 2,
                                    const Standard_Real theMinSize = Precision::Confusion());

  //! Adds point to already calculated points (or replaces existing).
  //! @param thePoint point to be added.
  //! @param theParam parameter on the curve corresponding to the given point.
  //! @param theIsReplace if TRUE replaces existing point lying within
  //! parameteric tolerance of the given point.
  //! @return index of new added point or found with parametric tolerance
  Standard_Integer AddPoint(const gp_Pnt&          thePoint,
                            const Standard_Real    theParam,
                            const Standard_Boolean theIsReplace = Standard_True)
  {
    return myDiscretTool.AddPoint(thePoint, theParam, theIsReplace);
  }

  //! Returns number of discretization points.
  Standard_Integer NbPoints() const { return myDiscretTool.NbPoints(); }

  //! Gets parameters of discretization point with the given index.
  //! @param theIndex index of discretization point.
  //! @param theIsoParam parameter on surface to be used as second coordinate
  //! of resulting 2d point.
  //! @param[out] theParam parameter of the point on the iso curve.
  //! @param[out] thePoint discretization point.
  //! @param[out] theUV discretization point in parametric space of the surface.
  //! @return TRUE on success, FALSE elsewhere.
  Standard_EXPORT Standard_Boolean Value(const Standard_Integer theIndex,
                                         const Standard_Real    theIsoParam,
                                         Standard_Real&         theParam,
                                         gp_Pnt&                thePoint,
                                         gp_Pnt2d&              theUV) const;

  //! Gets parameters of discretization point with the given index.
  //! @param theIndex index of discretization point.
  //! @param theSurface surface the curve is lying onto.
  //! @param[out] theParam parameter of the point on the curve.
  //! @param[out] thePoint discretization point.
  //! @param[out] theUV discretization point in parametric space of the surface.
  //! @return TRUE on success, FALSE elsewhere.
  Standard_EXPORT Standard_Boolean Value(const Standard_Integer             theIndex,
                                         const Handle(BRepAdaptor_Surface)& theSurface,
                                         Standard_Real&                     theParam,
                                         gp_Pnt&                            thePoint,
                                         gp_Pnt2d&                          theUV) const;

public: //! @name static API
  //! Computes normal to the given surface at the specified
  //! position in parametric space.
  //! @param theSurface surface the normal should be found for.
  //! @param theParamU U parameter in parametric space of the surface.
  //! @param theParamV V parameter in parametric space of the surface.
  //! @param[out] thePoint 3d point corresponding to the given parameters.
  //! @param[out] theNormal normal vector at the point specified by the parameters.
  //! @return FALSE if the normal can not be computed, TRUE elsewhere.
  Standard_EXPORT static Standard_Boolean Normal(const Handle(BRepAdaptor_Surface)& theSurface,
                                                 const Standard_Real                theParamU,
                                                 const Standard_Real                theParamV,
                                                 gp_Pnt&                            thePoint,
                                                 gp_Dir&                            theNormal);

  //! Checks intersection between two lines defined by two points.
  //! @param theStartPnt1 start point of first line.
  //! @param theEndPnt1 end point of first line.
  //! @param theStartPnt2 start point of second line.
  //! @param theEndPnt2 end point of second line.
  //! @param[out] theIntPnt point of intersection.
  //! @param[out] theParamOnSegment parameters of intersection point
  //! corresponding to first and second segment.
  //! @return status of intersection check.
  Standard_EXPORT static IntFlag IntLinLin(const gp_XY& theStartPnt1,
                                           const gp_XY& theEndPnt1,
                                           const gp_XY& theStartPnt2,
                                           const gp_XY& theEndPnt2,
                                           gp_XY&       theIntPnt,
                                           Standard_Real (&theParamOnSegment)[2]);

  //! Checks intersection between the two segments.
  //! Checks that intersection point lies within ranges of both segments.
  //! @param theStartPnt1 start point of first segment.
  //! @param theEndPnt1 end point of first segment.
  //! @param theStartPnt2 start point of second segment.
  //! @param theEndPnt2 end point of second segment.
  //! @param isConsiderEndPointTouch if TRUE EndPointTouch status will be
  //! returned in case if segments are touching by end points, if FALSE
  //! returns NoIntersection flag.
  //! @param isConsiderPointOnSegment if TRUE PointOnSegment status will be
  //! returned in case if end point of one segment lies onto another one,
  //! if FALSE returns NoIntersection flag.
  //! @param[out] theIntPnt point of intersection.
  //! @return status of intersection check.
  Standard_EXPORT static IntFlag IntSegSeg(const gp_XY&           theStartPnt1,
                                           const gp_XY&           theEndPnt1,
                                           const gp_XY&           theStartPnt2,
                                           const gp_XY&           theEndPnt2,
                                           const Standard_Boolean isConsiderEndPointTouch,
                                           const Standard_Boolean isConsiderPointOnSegment,
                                           gp_Pnt2d&              theIntPnt);

  //! Compute deflection of the given segment.
  static Standard_Real SquareDeflectionOfSegment(const gp_Pnt& theFirstPoint,
                                                 const gp_Pnt& theLastPoint,
                                                 const gp_Pnt& theMidPoint)
  {
    // 23.03.2010 skl for OCC21645 - change precision for comparison
    if (theFirstPoint.SquareDistance(theLastPoint) > Precision::SquareConfusion())
    {
      gp_Lin aLin(theFirstPoint, gp_Dir(gp_Vec(theFirstPoint, theLastPoint)));
      return aLin.SquareDistance(theMidPoint);
    }

    return theFirstPoint.SquareDistance(theMidPoint);
  }

  // For better meshing performance we try to estimate the acceleration circles grid structure
  // sizes: For each parametric direction (U, V) we estimate firstly an approximate distance between
  // the future points - this estimation takes into account the required face deflection and the
  // complexity of the face. Particularly, the complexity of the faces based on BSpline curves and
  // surfaces requires much more points. At the same time, for planar faces and linear parts of the
  // arbitrary surfaces usually no intermediate points are necessary. The general idea for each
  // parametric direction: cells_count = 2 ^ log10 ( estimated_points_count ) For linear parametric
  // direction we fall back to the initial vertex count: cells_count = 2 ^ log10 (
  // initial_vertex_count )
  Standard_EXPORT static std::pair<Standard_Integer, Standard_Integer> CellsCount(
    const Handle(Adaptor3d_Surface)&     theSurface,
    const Standard_Integer               theVerticesNb,
    const Standard_Real                  theDeflection,
    const BRepMesh_DefaultRangeSplitter* theRangeSplitter);

private:
  //! Classifies the point in case of coincidence of two vectors.
  //! @param thePoint1 the start point of a segment (base point).
  //! @param thePoint2 the end point of a segment.
  //! @param thePointToCheck the point to classify.
  //! @return zero value if point is out of segment and non zero value
  //! if point is between the first and the second point of segment.
  static Standard_Integer classifyPoint(const gp_XY& thePoint1,
                                        const gp_XY& thePoint2,
                                        const gp_XY& thePointToCheck);

private:
  const TopoDS_Edge*          myEdge;
  GCPnts_TangentialDeflection myDiscretTool;
  GeomAbs_IsoType             myIsoType;
};

#endif
