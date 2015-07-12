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
#include <TopTools_ListOfShape.hxx>
class Geom_Plane;
class TopoDS_Edge;
class TopoDS_Shape;
class TopoDS_Solid;
class TopoDS_Face;
class BRepLib_Command;
class BRepLib_MakeShape;
class BRepLib_MakeVertex;
class BRepLib_MakeEdge;
class BRepLib_MakeEdge2d;
class BRepLib_MakePolygon;
class BRepLib_MakeFace;
class BRepLib_MakeWire;
class BRepLib_MakeShell;
class BRepLib_MakeSolid;
class BRepLib_FindSurface;
class BRepLib_FuseEdges;
class BRepLib_CheckCurveOnSurface;


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
  
  //! Checks if the edge has a  Tolerance smaller than -- --
  //! -- -- MaxToleranceToCheck  if  so it will compute  the
  //! radius    of  -- the   cylindrical  pipe  surface that
  //! MinToleranceRequest is the minimum tolerance before it
  //! is usefull to start testing. Usually it should be arround
  //! 10e-5
  //! contains all  --  the curve represenation of  the edge
  //! returns True if the Edge tolerance had to be updated
  Standard_EXPORT static Standard_Boolean UpdateEdgeTol (const TopoDS_Edge& E, const Standard_Real MinToleranceRequest, const Standard_Real MaxToleranceToCheck);
  
  //! -- Checks all the edges of the shape whose -- -- --
  //! Tolerance  is  smaller than  MaxToleranceToCheck --
  //! Returns True if at  least  one edge was updated --
  //! MinToleranceRequest is the minimum tolerance before
  //! --  it -- is  usefull to start  testing. Usually it
  //! should be arround -- 10e-5--
  //!
  //! Warning :The  method is  very  slow  as it  checks all.
  //! Use  only  in interfaces or  processing assimilate batch
  Standard_EXPORT static Standard_Boolean UpdateEdgeTolerance (const TopoDS_Shape& S, const Standard_Real MinToleranceRequest, const Standard_Real MaxToleranceToCheck);
  
  //! Computes new 2d curve(s)  for the edge <E> to have
  //! the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on the  Edge.
  Standard_EXPORT static void SameParameter (const TopoDS_Edge& E, const Standard_Real Tolerance = 1.0e-5);
  
  //! Computes new 2d curve(s) for all the edges of  <S>
  //! to have the same parameter  as  the  3d curve.
  //! The algorithm is not done if the flag SameParameter
  //! was True  on an  Edge.
  Standard_EXPORT static void SameParameter (const TopoDS_Shape& S, const Standard_Real Tolerance = 1.0e-5, const Standard_Boolean forced = Standard_False);
  
  //! Replaces tolerance   of  FACE EDGE VERTEX  by  the
  //! tolerance Max of their connected handling shapes.
  //! It is not necessary to use this call after
  //! SameParameter. (called in)
  Standard_EXPORT static void UpdateTolerances (const TopoDS_Shape& S, const Standard_Boolean verifyFaceTolerance = Standard_False);
  
  //! Orients the solid forward  and the  shell with the
  //! orientation to have  matter in the solid. Returns
  //! False if the solid is unOrientable (open or incoherent)
  Standard_EXPORT static Standard_Boolean OrientClosedSolid (TopoDS_Solid& solid);
  
  //! Encodes the Regularity of edges on a Shape.
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edges's regularity are coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity (const TopoDS_Shape& S, const Standard_Real TolAng = 1.0e-10);
  
  //! Encodes the Regularity beetween <F1> and <F2> by <E>
  //! Warning: <TolAng> is an angular tolerance, expressed in Rad.
  //! Warning: If the edge's regularity is coded before, nothing
  //! is done.
  Standard_EXPORT static void EncodeRegularity (TopoDS_Edge& S, const TopoDS_Face& F1, const TopoDS_Face& F2, const Standard_Real TolAng = 1.0e-10);
  
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




protected:





private:




friend class BRepLib_Command;
friend class BRepLib_MakeShape;
friend class BRepLib_MakeVertex;
friend class BRepLib_MakeEdge;
friend class BRepLib_MakeEdge2d;
friend class BRepLib_MakePolygon;
friend class BRepLib_MakeFace;
friend class BRepLib_MakeWire;
friend class BRepLib_MakeShell;
friend class BRepLib_MakeSolid;
friend class BRepLib_FindSurface;
friend class BRepLib_FuseEdges;
friend class BRepLib_CheckCurveOnSurface;

};







#endif // _BRepLib_HeaderFile
