// Created on: 1996-04-03
// Created by: Stagiaire Frederic CALOONE
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

#ifndef _GeomPlate_BuildPlateSurface_HeaderFile
#define _GeomPlate_BuildPlateSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GeomPlate_CurveConstraint.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Plate_Plate.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_ExtPS.hxx>
#include <Adaptor3d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Sequence.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class Geom_Surface;
class GeomPlate_Surface;
class GeomPlate_CurveConstraint;
class GeomPlate_PointConstraint;
class gp_Pnt2d;
class gp_Pnt;
class Geom2d_Curve;

//! This class provides an algorithm for constructing such a plate surface that
//! it conforms to given curve and/or point constraints.
//! The algorithm accepts or constructs an initial surface
//! and looks for a deformation of it satisfying the
//! constraints and minimizing energy input.
//! A BuildPlateSurface object provides a framework for:
//! -   defining or setting constraints
//! -   implementing the construction algorithm
//! -   consulting the result.
class GeomPlate_BuildPlateSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor compatible with the old version
  //! with this constructor the constraint are given in a Array of Curve on Surface
  //! The array NbPoints contains the number of points for each constraint.
  //! The Array Tang contains the order of constraint for each Constraint: The possible values for
  //! this order has to be -1 , 0 , 1 , 2 . Order i means constraint Gi. NbIter is the maximum
  //! number of iteration to optimise the number of points for resolution Degree is the degree of
  //! resolution for Plate Tol2d is the tolerance used to test if two points of different constraint
  //! are identical in the parametric space of the initial surface Tol3d is used to test if two
  //! identical points in the 2d space are identical in 3d space TolAng is used to compare the angle
  //! between normal of two identical points in the 2d space Raises ConstructionError;
  Standard_EXPORT GeomPlate_BuildPlateSurface(const occ::handle<NCollection_HArray1<int>>&  NPoints,
                                              const occ::handle<NCollection_HArray1<occ::handle<Adaptor3d_Curve>>>& TabCurve,
                                              const occ::handle<NCollection_HArray1<int>>&  Tang,
                                              const int                   Degree,
                                              const int                   NbIter = 3,
                                              const double    Tol2d       = 0.00001,
                                              const double    Tol3d       = 0.0001,
                                              const double    TolAng      = 0.01,
                                              const double    TolCurv     = 0.1,
                                              const bool Anisotropie = false);

  Standard_EXPORT GeomPlate_BuildPlateSurface(const occ::handle<Geom_Surface>& Surf,
                                              const int      Degree     = 3,
                                              const int      NbPtsOnCur = 10,
                                              const int      NbIter     = 3,
                                              const double         Tol2d      = 0.00001,
                                              const double         Tol3d      = 0.0001,
                                              const double         TolAng     = 0.01,
                                              const double         TolCurv    = 0.1,
                                              const bool Anisotropie = false);

  //! Initializes the BuildPlateSurface framework for
  //! deforming plate surfaces using curve and point
  //! constraints. You use the first constructor if you have
  //! an initial surface to work with at construction time. If
  //! not, you use the second. You can add one later by
  //! using the method LoadInitSurface. If no initial
  //! surface is loaded, one will automatically be computed.
  //! The curve and point constraints will be defined by
  //! using the method Add.
  //! Before the call to the algorithm, the curve constraints
  //! will be transformed into sequences of discrete points.
  //! Each curve defined as a constraint will be given the
  //! value of NbPtsOnCur as the average number of points on it.
  //! Several arguments serve to improve performance of
  //! the algorithm. NbIter, for example, expresses the
  //! number of iterations allowed and is used to control the
  //! duration of computation. To optimize resolution,
  //! Degree will have the default value of 3.
  //! The surface generated must respect several tolerance values:
  //! -   2d tolerance given by Tol2d, with a default value of 0.00001
  //! -   3d tolerance expressed by Tol3d, with a default value of 0.0001
  //! -   angular tolerance given by TolAng, with a default
  //! value of 0.01, defining the greatest angle allowed
  //! between the constraint and the target surface.
  //! Exceptions
  //! Standard_ConstructionError if NbIter is less than 1 or Degree is less than 3.
  Standard_EXPORT GeomPlate_BuildPlateSurface(const int Degree      = 3,
                                              const int NbPtsOnCur  = 10,
                                              const int NbIter      = 3,
                                              const double    Tol2d       = 0.00001,
                                              const double    Tol3d       = 0.0001,
                                              const double    TolAng      = 0.01,
                                              const double    TolCurv     = 0.1,
                                              const bool Anisotropie = false);

  //! Resets all constraints
  Standard_EXPORT void Init();

  //! Loads the initial Surface
  Standard_EXPORT void LoadInitSurface(const occ::handle<Geom_Surface>& Surf);

  //! Adds the linear constraint cont.
  Standard_EXPORT void Add(const occ::handle<GeomPlate_CurveConstraint>& Cont);

  Standard_EXPORT void SetNbBounds(const int NbBounds);

  //! Adds the point constraint cont.
  Standard_EXPORT void Add(const occ::handle<GeomPlate_PointConstraint>& Cont);

  //! Calls the algorithm and computes the plate surface using
  //! the loaded constraints. If no initial surface is given, the
  //! algorithm automatically computes one.
  //! Exceptions
  //! Standard_RangeError if the value of the constraint is
  //! null or if plate is not done.
  Standard_EXPORT void Perform(const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! returns the CurveConstraints of order order
  Standard_EXPORT occ::handle<GeomPlate_CurveConstraint> CurveConstraint(
    const int order) const;

  //! returns the PointConstraint of order order
  Standard_EXPORT occ::handle<GeomPlate_PointConstraint> PointConstraint(
    const int order) const;

  Standard_EXPORT void Disc2dContour(const int nbp, NCollection_Sequence<gp_XY>& Seq2d);

  Standard_EXPORT void Disc3dContour(const int nbp,
                                     const int iordre,
                                     NCollection_Sequence<gp_XYZ>&  Seq3d);

  //! Tests whether computation of the plate has been completed.
  Standard_EXPORT bool IsDone() const;

  //! Returns the result of the computation. This surface can
  //! then be used by GeomPlate_MakeApprox for
  //! converting the resulting surface into a BSpline.
  Standard_EXPORT occ::handle<GeomPlate_Surface> Surface() const;

  //! Returns the initial surface
  Standard_EXPORT occ::handle<Geom_Surface> SurfInit() const;

  //! Allows you to ensure that the array of curves returned by
  //! Curves2d has the correct orientation. Returns the
  //! orientation of the curves in the array returned by
  //! Curves2d. Computation changes the orientation of
  //! these curves. Consequently, this method returns the
  //! orientation prior to computation.
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> Sense() const;

  //! Extracts the array of curves on the plate surface which
  //! correspond to the curve constraints set in Add.
  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<Geom2d_Curve>>> Curves2d() const;

  //! Returns the order of the curves in the array returned by
  //! Curves2d. Computation changes this order.
  //! Consequently, this method returns the order of the
  //! curves prior to computation.
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> Order() const;

  //! Returns the max distance between the result and the constraints
  Standard_EXPORT double G0Error() const;

  //! Returns the max angle between the result and the constraints
  Standard_EXPORT double G1Error() const;

  //! Returns the max difference of curvature between the result and the constraints
  Standard_EXPORT double G2Error() const;

  //! Returns the max distance between the result and the constraint Index
  Standard_EXPORT double G0Error(const int Index);

  //! Returns the max angle between the result and the constraint Index
  Standard_EXPORT double G1Error(const int Index);

  //! Returns the max difference of curvature between the result and the constraint Index
  Standard_EXPORT double G2Error(const int Index);

private:
  //! Evaluates the distance, the angle between normals, and the "courbure"
  //! on middle points of constraints and corresponding points on the GeomPlate_Surface
  //! the results are given for a curve c
  Standard_EXPORT void EcartContraintesMil(const int         c,
                                           occ::handle<NCollection_HArray1<double>>& d,
                                           occ::handle<NCollection_HArray1<double>>& an,
                                           occ::handle<NCollection_HArray1<double>>& courb);

  Standard_EXPORT gp_Pnt2d ProjectPoint(const gp_Pnt& P);

  Standard_EXPORT occ::handle<Geom2d_Curve> ProjectCurve(const occ::handle<Adaptor3d_Curve>& Curv);

  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> ProjectedCurve(occ::handle<Adaptor3d_Curve>& Curv);

  Standard_EXPORT void ComputeSurfInit(const Message_ProgressRange& theProgress);

  Standard_EXPORT void Intersect(occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>& PntInter,
                                 occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>& PntG1G1);

  Standard_EXPORT void Discretise(const occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>& PntInter,
                                  const occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>& PntG1G1);

  Standard_EXPORT void LoadCurve(const int NbBoucle,
                                 const int OrderMax = 2);

  Standard_EXPORT void LoadPoint(const int NbBoucle,
                                 const int OrderMax = 2);

  Standard_EXPORT void CalculNbPtsInit();

  Standard_EXPORT bool VerifSurface(const int NbLoop);

  Standard_EXPORT void VerifPoints(double& dist,
                                   double& ang,
                                   double& curv) const;

  Standard_EXPORT bool CourbeJointive(const double tolerance);

  Standard_EXPORT double ComputeAnisotropie() const;

  Standard_EXPORT bool IsOrderG1() const;

  occ::handle<NCollection_HSequence<occ::handle<GeomPlate_CurveConstraint>>> myLinCont;
  occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>    myParCont;
  occ::handle<NCollection_HArray1<NCollection_Sequence<double>>>    myPlateCont;
  occ::handle<NCollection_HSequence<occ::handle<GeomPlate_PointConstraint>>> myPntCont;
  occ::handle<Geom_Surface>                         mySurfInit;
  occ::handle<Geom_Surface>                         myPlanarSurfInit;
  occ::handle<GeomPlate_Surface>                    myGeomPlateSurface;
  Plate_Plate                                  myPlate;
  Plate_Plate                                  myPrevPlate;
  bool                             myAnisotropie;
  occ::handle<NCollection_HArray1<int>>             mySense;
  int                             myDegree;
  occ::handle<NCollection_HArray1<int>>             myInitOrder;
  double                                myG0Error;
  double                                myG1Error;
  double                                myG2Error;
  int                             myNbPtsOnCur;
  bool                             mySurfInitIsGive;
  int                             myNbIter;
  Extrema_ExtPS                                myProj;
  double                                myTol2d;
  double                                myTol3d;
  double                                myTolAng;
  double                                myTolU;
  double                                myTolV;
  int                             myNbBounds;
  bool                             myIsLinear;
  bool                             myFree;
};

#endif // _GeomPlate_BuildPlateSurface_HeaderFile
