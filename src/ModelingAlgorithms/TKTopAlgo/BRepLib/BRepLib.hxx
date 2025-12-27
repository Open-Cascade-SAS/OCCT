// Created on: 1993-12-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepLib_HeaderFile
#define _BRepLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <NCollection_List.hxx>

class Geom2d_Curve;
class Adaptor3d_Curve;
class Geom_Plane;
class TopoDS_Shape;
class TopoDS_Solid;
class TopoDS_Face;
class BRepTools_ReShape;

//! The BRepLib package provides general utilities for
//! BRep.
//!
//! * FindSurface : Class to compute a surface through
//! a set of edges.
//!
//! * Compute missing 3d curve on an edge.
class BRepLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computes the max distance between edge
  //! and its 2d representation on the face.
  //! Sets the default precision. The current Precision
  //! is returned.
  Standard_EXPORT static void Precision(const double P);

  //! Returns the default precision.
  Standard_EXPORT static double Precision();

  //! Sets the current plane to P.
  Standard_EXPORT static void Plane(const occ::handle<Geom_Plane>& P);

  //! Returns the current plane.
  Standard_EXPORT static const occ::handle<Geom_Plane>& Plane();

  //! checks if the Edge is same range IGNORING
  //! the same range flag of the edge
  //! Confusion argument is to compare real numbers
  //! idenpendently of any model space tolerance
  Standard_EXPORT static bool CheckSameRange(const TopoDS_Edge&  E,
                                                         const double Confusion = 1.0e-12);

  //! will make all the curve representation have
  //! the same range domain for the parameters.
  //! This will IGNORE the same range flag value
  //! to proceed.
  //! If there is a 3D curve there it will the
  //! range of that curve. If not the first curve representation
  //! encountered in the list will give its range to
  //! the all the other curves.
  Standard_EXPORT static void SameRange(const TopoDS_Edge&  E,
                                        const double Tolerance = 1.0e-5);

  //! Computes the 3d curve for the edge <E> if it does
  //! not exist. Returns True if the curve was computed
  //! or existed. Returns False if there is no planar
  //! pcurve or the computation failed.
  //! <MaxSegment> >= 30 in approximation
  Standard_EXPORT static bool BuildCurve3d(const TopoDS_Edge&  E,
                                                       const double Tolerance  = 1.0e-5,
                                                       const GeomAbs_Shape Continuity = GeomAbs_C1,
                                                       const int MaxDegree  = 14,
                                                       const int MaxSegment = 0);

  //! Computes the 3d curves for all the edges of <S>
  //! return False if one of the computation failed.
  //! <MaxSegment> >= 30 in approximation
  Standard_EXPORT static bool BuildCurves3d(const TopoDS_Shape& S,
                                                        const double Tolerance,
                                                        const GeomAbs_Shape Continuity = GeomAbs_C1,
                                                        const int MaxDegree  = 14,
                                                        const int MaxSegment = 0);

  //! Computes the 3d curves for all the edges of <S>
  //! return False if one of the computation failed.
  Standard_EXPORT static bool BuildCurves3d(const TopoDS_Shape& S);

  //! Builds pcurve of edge on face if the surface is plane, and updates the edge.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane(const TopoDS_Edge& theE,
                                                        const TopoDS_Face& theF);

  //! Builds pcurve of edge on face if the surface is plane, but does not update the edge.
  //! The output are the pcurve and the flag telling that pcurve was built.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane(const TopoDS_Edge&    theE,
                                                        const TopoDS_Face&    theF,
                                                        occ::handle<Geom2d_Curve>& aC2D,
                                                        bool&     bToUpdate);

  //! Builds pcurves of edges on face if the surface is plane, and update the edges.
  template <class TCont>
  static void BuildPCurveForEdgesOnPlane(const TCont& theLE, const TopoDS_Face& theF)
  {
    for (typename TCont::Iterator aIt(theLE); aIt.More(); aIt.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(aIt.Value());
      if (!aE.IsNull())
        BRepLib::BuildPCurveForEdgeOnPlane(aE, theF);
    }
  }

  //! Checks if the edge has a Tolerance smaller than
  //! MaxToleranceToCheck if so it will compute the
  //! radius of the cylindrical pipe surface that
  //! MinToleranceRequest is the minimum tolerance before it
  //! is useful to start testing.
  //! Usually it should be around 10e-5
  //! contains all the curve representation of the edge
  //! returns True if the Edge tolerance had to be updated
  Standard_EXPORT static bool UpdateEdgeTol(const TopoDS_Edge&  E,
                                                        const double MinToleranceRequest,
                                                        const double MaxToleranceToCheck);

  //! Checks all the edges of the shape whose
  //! Tolerance is smaller than MaxToleranceToCheck
  //! Returns True if at least one edge was updated
  //! MinToleranceRequest is the minimum tolerance before
  //! it is useful to start testing.
  //! Usually it should be around 10e-5
  //!
  //! Warning: The method is very slow as it checks all.
  //! Use only in interfaces or processing assimilate batch
  Standard_EXPORT static bool UpdateEdgeTolerance(
    const TopoDS_Shape& S,
    const double MinToleranceRequest,
    const double MaxToleranceToCheck);

  //! Computes new 2d curve(s) for the edge <theEdge> to have
  //! the same parameter as the 3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True on the Edge.
  Standard_EXPORT static void SameParameter(const TopoDS_Edge&  theEdge,
                                            const double Tolerance = 1.0e-5);

  //! Computes new 2d curve(s) for the edge <theEdge> to have
  //! the same parameter as the 3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True on the Edge.
  //! theNewTol is a new tolerance of vertices of the input edge
  //! (not applied inside the algorithm, but pre-computed).
  //! If IsUseOldEdge is true then the input edge will be modified,
  //! otherwise the new copy of input edge will be created.
  //! Returns the new edge as a result, can be ignored if IsUseOldEdge is true.
  Standard_EXPORT static TopoDS_Edge SameParameter(const TopoDS_Edge&     theEdge,
                                                   const double    theTolerance,
                                                   double&         theNewTol,
                                                   const bool IsUseOldEdge);

  //! Computes new 2d curve(s) for all the edges of <S>
  //! to have the same parameter as the 3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True on an Edge.
  Standard_EXPORT static void SameParameter(const TopoDS_Shape&    S,
                                            const double    Tolerance = 1.0e-5,
                                            const bool forced    = false);

  //! Computes new 2d curve(s) for all the edges of <S>
  //! to have the same parameter as the 3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True on an Edge.
  //! theReshaper is used to record the modifications of input shape <S> to prevent any
  //! modifications on the shape itself.
  //! Thus the input shape (and its subshapes) will not be modified, instead the reshaper will
  //! contain a modified empty-copies of original subshapes as substitutions.
  Standard_EXPORT static void SameParameter(const TopoDS_Shape&    S,
                                            BRepTools_ReShape&     theReshaper,
                                            const double    Tolerance = 1.0e-5,
                                            const bool forced    = false);

  //! Replaces tolerance of FACE EDGE VERTEX by the
  //! tolerance Max of their connected handling shapes.
  //! It is not necessary to use this call after
  //! SameParameter. (called in)
  Standard_EXPORT static void UpdateTolerances(
    const TopoDS_Shape&    S,
    const bool verifyFaceTolerance = false);

  //! Replaces tolerance of FACE EDGE VERTEX by the
  //! tolerance Max of their connected handling shapes.
  //! It is not necessary to use this call after
  //! SameParameter. (called in)
  //! theReshaper is used to record the modifications of input shape <S> to prevent any
  //! modifications on the shape itself.
  //! Thus the input shape (and its subshapes) will not be modified, instead the reshaper will
  //! contain a modified empty-copies of original subshapes as substitutions.
  Standard_EXPORT static void UpdateTolerances(
    const TopoDS_Shape&    S,
    BRepTools_ReShape&     theReshaper,
    const bool verifyFaceTolerance = false);

  //! Checks tolerances of edges (including inner points) and vertices
  //! of a shape and updates them to satisfy "SameParameter" condition
  Standard_EXPORT static void UpdateInnerTolerances(const TopoDS_Shape& S);

  //! Orients the solid forward and the shell with the
  //! orientation to have matter in the solid. Returns
  //! False if the solid is unOrientable (open or incoherent)
  Standard_EXPORT static bool OrientClosedSolid(TopoDS_Solid& solid);

  //! Returns the order of continuity between two faces
  //! connected by an edge
  Standard_EXPORT static GeomAbs_Shape ContinuityOfFaces(const TopoDS_Edge&  theEdge,
                                                         const TopoDS_Face&  theFace1,
                                                         const TopoDS_Face&  theFace2,
                                                         const double theAngleTol);

  //! Encodes the Regularity of edges on a Shape.
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edges's regularity are coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity(const TopoDS_Shape& S,
                                               const double TolAng = 1.0e-10);

  //! Encodes the Regularity of edges in list <LE> on the shape <S>
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edges's regularity are coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity(const TopoDS_Shape&         S,
                                               const NCollection_List<TopoDS_Shape>& LE,
                                               const double         TolAng = 1.0e-10);

  //! Encodes the Regularity between <F1> and <F2> by <E>
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edge's regularity is coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity(TopoDS_Edge&        E,
                                               const TopoDS_Face&  F1,
                                               const TopoDS_Face&  F2,
                                               const double TolAng = 1.0e-10);

  //! Sorts in LF the Faces of S on the complexity of
  //! their surfaces
  //! (Plane,Cylinder,Cone,Sphere,Torus,other)
  Standard_EXPORT static void SortFaces(const TopoDS_Shape& S, NCollection_List<TopoDS_Shape>& LF);

  //! Sorts in LF the Faces of S on the reverse
  //! complexity of their surfaces
  //! (other,Torus,Sphere,Cone,Cylinder,Plane)
  Standard_EXPORT static void ReverseSortFaces(const TopoDS_Shape& S, NCollection_List<TopoDS_Shape>& LF);

  //! Corrects the normals in Poly_Triangulation of faces,
  //! in such way that normals at nodes lying along smooth
  //! edges have the same value on both adjacent triangulations.
  //! Returns TRUE if any correction is done.
  Standard_EXPORT static bool EnsureNormalConsistency(
    const TopoDS_Shape&    S,
    const double    theAngTol           = 0.001,
    const bool ForceComputeNormals = false);

  //! Updates value of deflection in Poly_Triangulation of faces
  //! by the maximum deviation measured on existing triangulation.
  Standard_EXPORT static void UpdateDeflection(const TopoDS_Shape& S);

  //! Calculates the bounding sphere around the set of vertexes from the theLV list.
  //! Returns the center (theNewCenter) and the radius (theNewTol) of this sphere.
  //! This can be used to construct the new vertex which covers the given set of
  //! other vertices.
  Standard_EXPORT static void BoundingVertex(const NCollection_List<TopoDS_Shape>& theLV,
                                             gp_Pnt&                               theNewCenter,
                                             double&                        theNewTol);

  //! For an edge defined by 3d curve and tolerance and vertices defined by points,
  //! parameters on curve and tolerances,
  //! finds a range of curve between vertices not covered by vertices tolerances.
  //! Returns false if there is no such range. Otherwise, sets theFirst and
  //! theLast as its bounds.
  Standard_EXPORT static bool FindValidRange(const Adaptor3d_Curve& theCurve,
                                                         const double    theTolE,
                                                         const double    theParV1,
                                                         const gp_Pnt&          thePntV1,
                                                         const double    theTolV1,
                                                         const double    theParV2,
                                                         const gp_Pnt&          thePntV2,
                                                         const double    theTolV2,
                                                         double&         theFirst,
                                                         double&         theLast);

  //! Finds a range of 3d curve of the edge not covered by vertices tolerances.
  //! Returns false if there is no such range. Otherwise, sets theFirst and
  //! theLast as its bounds.
  Standard_EXPORT static bool FindValidRange(const TopoDS_Edge& theEdge,
                                                         double&     theFirst,
                                                         double&     theLast);

  //! Enlarges the face on the given value.
  //! @param[in] theF  The face to extend
  //! @param[in] theExtVal  The extension value
  //! @param[in] theExtUMin  Defines whether to extend the face in UMin direction
  //! @param[in] theExtUMax  Defines whether to extend the face in UMax direction
  //! @param[in] theExtVMin  Defines whether to extend the face in VMin direction
  //! @param[in] theExtVMax  Defines whether to extend the face in VMax direction
  //! @param[in] theFExtended  The extended face
  Standard_EXPORT static void ExtendFace(const TopoDS_Face&     theF,
                                         const double    theExtVal,
                                         const bool theExtUMin,
                                         const bool theExtUMax,
                                         const bool theExtVMin,
                                         const bool theExtVMax,
                                         TopoDS_Face&           theFExtended);
};

#endif // _BRepLib_HeaderFile
