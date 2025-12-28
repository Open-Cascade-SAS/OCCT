// Created on: 1998-08-26
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_Filling_HeaderFile
#define _BRepFill_Filling_HeaderFile

#include <BRepFill_EdgeFaceAndOrder.hxx>
#include <NCollection_Sequence.hxx>
#include <BRepFill_FaceAndOrder.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>

#include <memory>

class TopoDS_Edge;
class gp_Pnt;

//! N-Side Filling
//! This algorithm avoids to build a face from:
//! * a set of edges defining the bounds of the face and some
//! constraints the surface support has to satisfy
//! * a set of edges and points defining some constraints
//! the support surface has to satisfy
//! * an initial surface to deform for satisfying the constraints
//! * a set of parameters to control the constraints.
//!
//! The support surface of the face is computed by deformation
//! of the initial surface in order to satisfy the given constraints.
//! The set of bounding edges defines the wire of the face.
//!
//! If no initial surface is given, the algorithm computes it
//! automatically.
//! If the set of edges is not connected (Free constraint)
//! missing edges are automatically computed.
//!
//! Limitations:
//! * If some constraints are not compatible
//! The algorithm does not take them into account.
//! So the constraints will not be satisfied in an area containing
//! the incompatibilities.
//! * The constraints defining the bound of the face have to be
//! entered in order to have a continuous wire.
//!
//! Other Applications:
//! * Deformation of a face to satisfy internal constraints
//! * Deformation of a face to improve Gi continuity with
//! connected faces
class BRepFill_Filling
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  Standard_EXPORT BRepFill_Filling(const int    Degree      = 3,
                                   const int    NbPtsOnCur  = 15,
                                   const int    NbIter      = 2,
                                   const bool   Anisotropie = false,
                                   const double Tol2d       = 0.00001,
                                   const double Tol3d       = 0.0001,
                                   const double TolAng      = 0.01,
                                   const double TolCurv     = 0.1,
                                   const int    MaxDeg      = 8,
                                   const int    MaxSegments = 9);

  //! Sets the values of Tolerances used to control the constraint.
  //! Tol2d:
  //! Tol3d:   it is the maximum distance allowed between the support surface
  //! and the constraints
  //! TolAng:  it is the maximum angle allowed between the normal of the surface
  //! and the constraints
  //! TolCurv: it is the maximum difference of curvature allowed between
  //! the surface and the constraint
  Standard_EXPORT void SetConstrParam(const double Tol2d   = 0.00001,
                                      const double Tol3d   = 0.0001,
                                      const double TolAng  = 0.01,
                                      const double TolCurv = 0.1);

  //! Sets the parameters used for resolution.
  //! The default values of these parameters have been chosen for a good
  //! ratio quality/performance.
  //! Degree:      it is the order of energy criterion to minimize for computing
  //! the deformation of the surface.
  //! The default value is 3
  //! The recommended value is i+2 where i is the maximum order of the
  //! constraints.
  //! NbPtsOnCur:  it is the average number of points for discretisation
  //! of the edges.
  //! NbIter:      it is the maximum number of iterations of the process.
  //! For each iteration the number of discretisation points is
  //! increased.
  //! Anisotropie:
  Standard_EXPORT void SetResolParam(const int  Degree      = 3,
                                     const int  NbPtsOnCur  = 15,
                                     const int  NbIter      = 2,
                                     const bool Anisotropie = false);

  //! Sets the parameters used for approximation of the surface
  Standard_EXPORT void SetApproxParam(const int MaxDeg = 8, const int MaxSegments = 9);

  //! Loads the initial Surface
  //! The initial surface must have orthogonal local coordinates,
  //! i.e. partial derivatives dS/du and dS/dv must be orthogonal
  //! at each point of surface.
  //! If this condition breaks, distortions of resulting surface
  //! are possible.
  Standard_EXPORT void LoadInitSurface(const TopoDS_Face& aFace);

  //! Adds a new constraint which also defines an edge of the wire
  //! of the face
  //! Order: Order of the constraint:
  //! GeomAbs_C0 : the surface has to pass by 3D representation
  //! of the edge
  //! GeomAbs_G1 : the surface has to pass by 3D representation
  //! of the edge and to respect tangency with the first
  //! face of the edge
  //! GeomAbs_G2 : the surface has to pass by 3D representation
  //! of the edge and to respect tangency and curvature
  //! with the first face of the edge.
  Standard_EXPORT int Add(const TopoDS_Edge&  anEdge,
                          const GeomAbs_Shape Order,
                          const bool          IsBound = true);

  //! Adds a new constraint which also defines an edge of the wire
  //! of the face
  //! Order: Order of the constraint:
  //! GeomAbs_C0 : the surface has to pass by 3D representation
  //! of the edge
  //! GeomAbs_G1 : the surface has to pass by 3D representation
  //! of the edge and to respect tangency with the
  //! given face
  //! GeomAbs_G2 : the surface has to pass by 3D representation
  //! of the edge and to respect tangency and curvature
  //! with the given face.
  Standard_EXPORT int Add(const TopoDS_Edge&  anEdge,
                          const TopoDS_Face&  Support,
                          const GeomAbs_Shape Order,
                          const bool          IsBound = true);

  //! Adds a free constraint on a face. The corresponding edge has to
  //! be automatically recomputed.
  //! It is always a bound.
  Standard_EXPORT int Add(const TopoDS_Face& Support, const GeomAbs_Shape Order);

  //! Adds a punctual constraint
  Standard_EXPORT int Add(const gp_Pnt& Point);

  //! Adds a punctual constraint.
  Standard_EXPORT int Add(const double        U,
                          const double        V,
                          const TopoDS_Face&  Support,
                          const GeomAbs_Shape Order);

  //! Builds the resulting faces
  Standard_EXPORT void Build();

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT TopoDS_Face Face() const;

  //! Returns the list of shapes generated from the
  //! shape <S>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(const TopoDS_Shape& S);

  Standard_EXPORT double G0Error() const;

  Standard_EXPORT double G1Error() const;

  Standard_EXPORT double G2Error() const;

  Standard_EXPORT double G0Error(const int Index);

  Standard_EXPORT double G1Error(const int Index);

  Standard_EXPORT double G2Error(const int Index);

private:
  //! Adds constraints to builder
  Standard_EXPORT void AddConstraints(
    const NCollection_Sequence<BRepFill_EdgeFaceAndOrder>& SeqOfConstraints);

  //! Builds wires of maximum length
  Standard_EXPORT void BuildWires(NCollection_List<TopoDS_Shape>& EdgeList,
                                  NCollection_List<TopoDS_Shape>& WireList);

  //! Finds extremities of future edges to fix the holes between wires.
  //! Can properly operate only with convex contour
  Standard_EXPORT void FindExtremitiesOfHoles(const NCollection_List<TopoDS_Shape>& WireList,
                                              NCollection_Sequence<TopoDS_Shape>&   VerSeq) const;

private:
  std::shared_ptr<GeomPlate_BuildPlateSurface>                             myBuilder;
  NCollection_Sequence<BRepFill_EdgeFaceAndOrder>                          myBoundary;
  NCollection_Sequence<BRepFill_EdgeFaceAndOrder>                          myConstraints;
  NCollection_Sequence<BRepFill_FaceAndOrder>                              myFreeConstraints;
  NCollection_Sequence<occ::handle<GeomPlate_PointConstraint>>             myPoints;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myOldNewMap;
  NCollection_List<TopoDS_Shape>                                           myGenerated;
  TopoDS_Face                                                              myFace;
  TopoDS_Face                                                              myInitFace;
  double                                                                   myTol2d;
  double                                                                   myTol3d;
  double                                                                   myTolAng;
  double                                                                   myTolCurv;
  int                                                                      myMaxDeg;
  int                                                                      myMaxSegments;
  int                                                                      myDegree;
  int                                                                      myNbPtsOnCur;
  int                                                                      myNbIter;
  bool                                                                     myAnisotropie;
  bool                                                                     myIsInitFaceGiven;
  bool                                                                     myIsDone;
};

#endif // _BRepFill_Filling_HeaderFile
