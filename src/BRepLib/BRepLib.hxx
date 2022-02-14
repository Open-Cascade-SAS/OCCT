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
#include <TopTools_ListOfShape.hxx>
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
  //! Sets the default precision.  The current Precision
  //! is returned.
  Standard_EXPORT static void Precision (const Standard_Real P);
  
  //! Returns the default precision.
  Standard_EXPORT static Standard_Real Precision();
  
  //! Sets the current plane to P.
  Standard_EXPORT static void Plane (const Handle(Geom_Plane)& P);
  
  //! Returns the current plane.
  Standard_EXPORT static const Handle(Geom_Plane)& Plane();
  
  //! checks if the Edge is same range IGNORING
  //! the same range flag of the edge
  //! Confusion argument is to compare real numbers
  //! idenpendently of any model space tolerance
  Standard_EXPORT static Standard_Boolean CheckSameRange (const TopoDS_Edge& E, const Standard_Real Confusion = 1.0e-12);
  
  //! will make all the curve representation have
  //! the same range domain for the parameters.
  //! This will IGNORE the same range flag value
  //! to proceed.
  //! If there is a 3D curve there it will the
  //! range of that curve. If not the first curve representation
  //! encountered in the list will give its range to
  //! the all the other curves.
  Standard_EXPORT static void SameRange (const TopoDS_Edge& E, const Standard_Real Tolerance = 1.0e-5);
  
  //! Computes the 3d curve for the edge  <E> if it does
  //! not exist. Returns True  if the curve was computed
  //! or  existed. Returns False  if there is no  planar
  //! pcurve or the computation failed.
  //! <MaxSegment> >= 30 in approximation
  Standard_EXPORT static Standard_Boolean BuildCurve3d (const TopoDS_Edge& E, const Standard_Real Tolerance = 1.0e-5, const GeomAbs_Shape Continuity = GeomAbs_C1, const Standard_Integer MaxDegree = 14, const Standard_Integer MaxSegment = 0);
  
  //! Computes  the 3d curves  for all the  edges of <S>
  //! return False if one of the computation failed.
  //! <MaxSegment> >= 30 in approximation
  Standard_EXPORT static Standard_Boolean BuildCurves3d (const TopoDS_Shape& S, const Standard_Real Tolerance, const GeomAbs_Shape Continuity = GeomAbs_C1, const Standard_Integer MaxDegree = 14, const Standard_Integer MaxSegment = 0);
  
  //! Computes  the 3d curves  for all the  edges of <S>
  //! return False if one of the computation failed.
  Standard_EXPORT static Standard_Boolean BuildCurves3d (const TopoDS_Shape& S);

  //! Builds pcurve of edge on face if the surface is plane, and updates the edge.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane(const TopoDS_Edge& theE, const TopoDS_Face& theF);

  //! Builds pcurve of edge on face if the surface is plane, but does not update the edge.
  //! The output are the pcurve and the flag telling that pcurve was built.
  Standard_EXPORT static void BuildPCurveForEdgeOnPlane(const TopoDS_Edge& theE, const TopoDS_Face& theF,
                                                        Handle(Geom2d_Curve)& aC2D, Standard_Boolean& bToUpdate);

  //! Builds pcurves of edges on face if the surface is plane, and update the edges.
  template<class TCont> static void BuildPCurveForEdgesOnPlane(const TCont& theLE, const TopoDS_Face& theF)
  {
    for (typename TCont::Iterator aIt(theLE); aIt.More(); aIt.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(aIt.Value());
      if (!aE.IsNull())
        BRepLib::BuildPCurveForEdgeOnPlane(aE, theF);
    }
  }

  //! Checks if the edge has a  Tolerance smaller than -- --
  //! -- -- MaxToleranceToCheck  if  so it will compute  the
  //! radius    of  -- the   cylindrical  pipe  surface that
  //! MinToleranceRequest is the minimum tolerance before it
  //! is useful to start testing.
  //! Usually it should be around 10e-5
  //! contains all -- the curve representation of the edge
  //! returns True if the Edge tolerance had to be updated
  Standard_EXPORT static Standard_Boolean UpdateEdgeTol (const TopoDS_Edge& E, const Standard_Real MinToleranceRequest, const Standard_Real MaxToleranceToCheck);
  
  //! -- Checks all the edges of the shape whose -- -- --
  //! Tolerance  is  smaller than  MaxToleranceToCheck --
  //! Returns True if at  least  one edge was updated --
  //! MinToleranceRequest is the minimum tolerance before
  //! --  it -- is  useful to start  testing.
  //! Usually it should be around -- 10e-5--
  //!
  //! Warning :The  method is  very  slow  as it  checks all.
  //! Use  only  in interfaces or  processing assimilate batch
  Standard_EXPORT static Standard_Boolean UpdateEdgeTolerance (const TopoDS_Shape& S, const Standard_Real MinToleranceRequest, const Standard_Real MaxToleranceToCheck);
  
  //! Computes new 2d curve(s)  for the edge <theEdge> to have
  //! the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on the  Edge.
  Standard_EXPORT static void SameParameter (const TopoDS_Edge& theEdge, const Standard_Real Tolerance = 1.0e-5);

  //! Computes new 2d curve(s)  for the edge <theEdge> to have
  //! the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on the  Edge.<br>
  //! theNewTol is a new tolerance of vertices of the input edge
  //! (not applied inside the algorithm, but pre-computed).
  //! If IsUseOldEdge is true then the input edge will be modified,
  //! otherwise the new copy of input edge will be created.
  //! Returns the new edge as a result, can be ignored if IsUseOldEdge is true.
  Standard_EXPORT static TopoDS_Edge SameParameter(const TopoDS_Edge& theEdge,
  const Standard_Real theTolerance, Standard_Real& theNewTol, const Standard_Boolean IsUseOldEdge);
  
  //! Computes new 2d curve(s) for all the edges of  <S>
  //! to have the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on an  Edge.
  Standard_EXPORT static void SameParameter(const TopoDS_Shape& S,
    const Standard_Real Tolerance = 1.0e-5, const Standard_Boolean forced = Standard_False);

  //! Computes new 2d curve(s) for all the edges of  <S>
  //! to have the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on an  Edge.<br>
  //! theReshaper is used to record the modifications of input shape <S> to prevent any 
  //! modifications on the shape itself.
  //! Thus the input shape (and its subshapes) will not be modified, instead the reshaper will 
  //! contain a modified empty-copies of original subshapes as substitutions.
  Standard_EXPORT static void SameParameter(const TopoDS_Shape& S, BRepTools_ReShape& theReshaper,
    const Standard_Real Tolerance = 1.0e-5, const Standard_Boolean forced = Standard_False );
 
  //! Replaces tolerance   of  FACE EDGE VERTEX  by  the
  //! tolerance Max of their connected handling shapes.
  //! It is not necessary to use this call after
  //! SameParameter. (called in)
  Standard_EXPORT static void UpdateTolerances (const TopoDS_Shape& S, const Standard_Boolean verifyFaceTolerance = Standard_False);

  //! Replaces tolerance   of  FACE EDGE VERTEX  by  the
  //! tolerance Max of their connected handling shapes.
  //! It is not necessary to use this call after
  //! SameParameter. (called in)<br>
  //! theReshaper is used to record the modifications of input shape <S> to prevent any 
  //! modifications on the shape itself.
  //! Thus the input shape (and its subshapes) will not be modified, instead the reshaper will 
  //! contain a modified empty-copies of original subshapes as substitutions.
  Standard_EXPORT static void UpdateTolerances (const TopoDS_Shape& S, BRepTools_ReShape& theReshaper, 
    const Standard_Boolean verifyFaceTolerance = Standard_False );
  
  //! Checks tolerances of edges (including inner points) and vertices
  //! of a shape and updates them to satisfy "SameParameter" condition
  Standard_EXPORT static void UpdateInnerTolerances (const TopoDS_Shape& S);
  
  //! Orients the solid forward  and the  shell with the
  //! orientation to have  matter in the solid. Returns
  //! False if the solid is unOrientable (open or incoherent)
  Standard_EXPORT static Standard_Boolean OrientClosedSolid (TopoDS_Solid& solid);

  //! Returns the order of continuity between two faces
  //! connected by an edge
  Standard_EXPORT static GeomAbs_Shape ContinuityOfFaces(const TopoDS_Edge&  theEdge,
                                                         const TopoDS_Face&  theFace1,
                                                         const TopoDS_Face&  theFace2,
                                                         const Standard_Real theAngleTol);

  //! Encodes the Regularity of edges on a Shape.
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edges's regularity are coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity (const TopoDS_Shape& S, const Standard_Real TolAng = 1.0e-10);

  //! Encodes the Regularity of edges in list <LE> on the shape <S>
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edges's regularity are coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity(const TopoDS_Shape& S, const TopTools_ListOfShape& LE, const Standard_Real TolAng = 1.0e-10);
  
  //! Encodes the Regularity between <F1> and <F2> by <E>
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edge's regularity is coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity (TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2, const Standard_Real TolAng = 1.0e-10);
  
  //! Sorts in  LF the Faces of   S on the  complexity of
  //! their                  surfaces
  //! (Plane,Cylinder,Cone,Sphere,Torus,other)
  Standard_EXPORT static void SortFaces (const TopoDS_Shape& S, TopTools_ListOfShape& LF);
  
  //! Sorts in  LF  the   Faces  of S   on the reverse
  //! complexity       of       their      surfaces
  //! (other,Torus,Sphere,Cone,Cylinder,Plane)
  Standard_EXPORT static void ReverseSortFaces (const TopoDS_Shape& S, TopTools_ListOfShape& LF);
  
  //! Corrects the normals in Poly_Triangulation of faces,
  //! in such way that normals at nodes lying along smooth
  //! edges have the same value on both adjacent triangulations.
  //! Returns TRUE if any correction is done.
  Standard_EXPORT static Standard_Boolean EnsureNormalConsistency (const TopoDS_Shape& S, const Standard_Real theAngTol = 0.001, const Standard_Boolean ForceComputeNormals = Standard_False);

  //! Updates value of deflection in Poly_Triangulation of faces
  //! by the maximum deviation measured on existing triangulation.
  Standard_EXPORT static void UpdateDeflection (const TopoDS_Shape& S);

  //! Calculates the bounding sphere around the set of vertexes from the theLV list.
  //! Returns the center (theNewCenter) and the radius (theNewTol) of this sphere.
  //! This can be used to construct the new vertex which covers the given set of
  //! other vertices.
  Standard_EXPORT static  void BoundingVertex(const NCollection_List<TopoDS_Shape>& theLV,
                                              gp_Pnt& theNewCenter, Standard_Real& theNewTol);

  //! For an edge defined by 3d curve and tolerance and vertices defined by points,
  //! parameters on curve and tolerances,
  //! finds a range of curve between vertices not covered by vertices tolerances.
  //! Returns false if there is no such range. Otherwise, sets theFirst and 
  //! theLast as its bounds.
  Standard_EXPORT static Standard_Boolean FindValidRange
    (const Adaptor3d_Curve& theCurve, const Standard_Real theTolE,
     const Standard_Real theParV1, const gp_Pnt& thePntV1, const Standard_Real theTolV1,
     const Standard_Real theParV2, const gp_Pnt& thePntV2, const Standard_Real theTolV2,
     Standard_Real& theFirst, Standard_Real& theLast);

  //! Finds a range of 3d curve of the edge not covered by vertices tolerances.
  //! Returns false if there is no such range. Otherwise, sets theFirst and 
  //! theLast as its bounds.
  Standard_EXPORT static Standard_Boolean FindValidRange
    (const TopoDS_Edge& theEdge, Standard_Real& theFirst, Standard_Real& theLast);


  //! Enlarges the face on the given value.
  //! @param theF [in] The face to extend
  //! @param theExtVal [in] The extension value
  //! @param theExtUMin [in] Defines whether to extend the face in UMin direction
  //! @param theExtUMax [in] Defines whether to extend the face in UMax direction
  //! @param theExtVMin [in] Defines whether to extend the face in VMin direction
  //! @param theExtVMax [in] Defines whether to extend the face in VMax direction
  //! @param theFExtended [in] The extended face
  Standard_EXPORT static void ExtendFace(const TopoDS_Face& theF,
                                         const Standard_Real theExtVal,
                                         const Standard_Boolean theExtUMin,
                                         const Standard_Boolean theExtUMax,
                                         const Standard_Boolean theExtVMin,
                                         const Standard_Boolean theExtVMax,
                                         TopoDS_Face& theFExtended);

};

#endif // _BRepLib_HeaderFile
