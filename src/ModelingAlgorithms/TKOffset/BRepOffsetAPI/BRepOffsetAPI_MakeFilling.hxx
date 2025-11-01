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

#ifndef _BRepOffsetAPI_MakeFilling_HeaderFile
#define _BRepOffsetAPI_MakeFilling_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_Filling.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Face;
class TopoDS_Edge;
class gp_Pnt;
class TopoDS_Shape;

//! N-Side Filling
//! This algorithm avoids to build a face from:
//! * a set of edges defining the bounds of the face and some
//! constraints the surface of the face has to satisfy
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
//! So the constraints will not be satisfyed in an area containing
//! the incompatibilitries.
//! * The constraints defining the bound of the face have to be
//! entered in order to have a continuous wire.
//!
//! Other Applications:
//! * Deformation of a face to satisfy internal constraints
//! * Deformation of a face to improve Gi continuity with
//! connected faces
class BRepOffsetAPI_MakeFilling : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a wire filling object defined by
  //! - the energy minimizing criterion Degree
  //! - the number of points on the curve NbPntsOnCur
  //! - the number of iterations NbIter
  //! - the Boolean Anisotropie
  //! - the 2D tolerance Tol2d
  //! - the 3D tolerance Tol3d
  //! - the angular tolerance TolAng
  //! - the tolerance for curvature TolCur
  //! - the highest polynomial degree MaxDeg
  //! - the greatest number of segments MaxSeg.
  //! If the Boolean Anistropie is true, the algorithm's
  //! performance is better in cases where the ratio of the
  //! length U and the length V indicate a great difference
  //! between the two. In other words, when the surface is, for
  //! example, extremely long.
  Standard_EXPORT BRepOffsetAPI_MakeFilling(const Standard_Integer Degree      = 3,
                                            const Standard_Integer NbPtsOnCur  = 15,
                                            const Standard_Integer NbIter      = 2,
                                            const Standard_Boolean Anisotropie = Standard_False,
                                            const Standard_Real    Tol2d       = 0.00001,
                                            const Standard_Real    Tol3d       = 0.0001,
                                            const Standard_Real    TolAng      = 0.01,
                                            const Standard_Real    TolCurv     = 0.1,
                                            const Standard_Integer MaxDeg      = 8,
                                            const Standard_Integer MaxSegments = 9);

  //! Sets the values of Tolerances used to control the constraint.
  //! Tol2d:
  //! Tol3d:   it is the maximum distance allowed between the support surface
  //! and the constraints
  //! TolAng:  it is the maximum angle allowed between the normal of the surface
  //! and the constraints
  //! TolCurv: it is the maximum difference of curvature allowed between
  //! the surface and the constraint
  Standard_EXPORT void SetConstrParam(const Standard_Real Tol2d   = 0.00001,
                                      const Standard_Real Tol3d   = 0.0001,
                                      const Standard_Real TolAng  = 0.01,
                                      const Standard_Real TolCurv = 0.1);

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
  Standard_EXPORT void SetResolParam(const Standard_Integer Degree      = 3,
                                     const Standard_Integer NbPtsOnCur  = 15,
                                     const Standard_Integer NbIter      = 2,
                                     const Standard_Boolean Anisotropie = Standard_False);

  //! Sets the parameters used to approximate the filling
  //! surface. These include:
  //! - MaxDeg - the highest degree which the polynomial
  //! defining the filling surface can have
  //! - MaxSegments - the greatest number of segments
  //! which the filling surface can have.
  Standard_EXPORT void SetApproxParam(const Standard_Integer MaxDeg      = 8,
                                      const Standard_Integer MaxSegments = 9);

  //! Loads the initial surface Surf to
  //! begin the construction of the surface.
  //! This optional function is useful if the surface resulting from
  //! construction for the algorithm is likely to be complex.
  //! The support surface of the face under construction is computed by a
  //! deformation of Surf which satisfies the given constraints.
  //! The set of bounding edges defines the wire of the face.
  //! If no initial surface is given, the algorithm computes it
  //! automatically. If the set of edges is not connected (Free constraint),
  //! missing edges are automatically computed.
  //! Important: the initial surface must have orthogonal local coordinates,
  //! i.e. partial derivatives dS/du and dS/dv must be orthogonal
  //! at each point of surface.
  //! If this condition breaks, distortions of resulting surface
  //! are possible.
  Standard_EXPORT void LoadInitSurface(const TopoDS_Face& Surf);

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
  //! Raises ConstructionError if the edge has no representation on a face and Order is
  //! GeomAbs_G1 or GeomAbs_G2.
  Standard_EXPORT Standard_Integer Add(const TopoDS_Edge&     Constr,
                                       const GeomAbs_Shape    Order,
                                       const Standard_Boolean IsBound = Standard_True);

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
  //! Raises ConstructionError if the edge has no 2d representation on the given face
  Standard_EXPORT Standard_Integer Add(const TopoDS_Edge&     Constr,
                                       const TopoDS_Face&     Support,
                                       const GeomAbs_Shape    Order,
                                       const Standard_Boolean IsBound = Standard_True);

  //! Adds a free constraint on a face. The corresponding edge has to
  //! be automatically recomputed. It is always a bound.
  Standard_EXPORT Standard_Integer Add(const TopoDS_Face& Support, const GeomAbs_Shape Order);

  //! Adds a punctual constraint.
  Standard_EXPORT Standard_Integer Add(const gp_Pnt& Point);

  //! Adds a punctual constraint.
  Standard_EXPORT Standard_Integer Add(const Standard_Real U,
                                       const Standard_Real V,
                                       const TopoDS_Face&  Support,
                                       const GeomAbs_Shape Order);

  //! Builds the resulting faces
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Tests whether computation of the filling plate has been completed.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;

  //! Returns the list of shapes generated from the
  //! shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& S)
    Standard_OVERRIDE;

  //! Returns the maximum distance between the result and
  //! the constraints. This is set at construction time.
  Standard_EXPORT Standard_Real G0Error() const;

  //! Returns the maximum angle between the result and the
  //! constraints. This is set at construction time.
  Standard_EXPORT Standard_Real G1Error() const;

  //! Returns the maximum angle between the result and the
  //! constraints. This is set at construction time.
  Standard_EXPORT Standard_Real G2Error() const;

  //! Returns the maximum distance attained between the
  //! result and the constraint Index. This is set at construction time.
  Standard_EXPORT Standard_Real G0Error(const Standard_Integer Index);

  //! Returns the maximum angle between the result and the
  //! constraints. This is set at construction time.
  Standard_EXPORT Standard_Real G1Error(const Standard_Integer Index);

  //! Returns the greatest difference in curvature found
  //! between the result and the constraint Index.
  Standard_EXPORT Standard_Real G2Error(const Standard_Integer Index);

protected:
private:
  BRepFill_Filling myFilling;
};

#endif // _BRepOffsetAPI_MakeFilling_HeaderFile
