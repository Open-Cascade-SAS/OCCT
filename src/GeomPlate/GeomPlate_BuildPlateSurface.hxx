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
#include <Standard_Handle.hxx>

#include <GeomPlate_HSequenceOfCurveConstraint.hxx>
#include <GeomPlate_HArray1OfSequenceOfReal.hxx>
#include <GeomPlate_HSequenceOfPointConstraint.hxx>
#include <Plate_Plate.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_ExtPS.hxx>
#include <GeomPlate_HArray1OfHCurve.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColStd_HArray1OfReal.hxx>
class Geom_Surface;
class GeomPlate_Surface;
class Standard_ConstructionError;
class Standard_RangeError;
class GeomPlate_CurveConstraint;
class GeomPlate_PointConstraint;
class gp_Pnt2d;
class gp_Pnt;
class Geom2d_Curve;
class Adaptor3d_HCurve;
class Adaptor2d_HCurve2d;



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

  
  //! Constructor  compatible  with  the  old  version
  //! with this constructor the constraint are given in a Array of Curve on Surface
  //! The array NbPoints  contains the number of points for each constraint.
  //! The Array Tang contains the order of constraint for each Constraint: The possible values for this
  //! order has to be -1 , 0 , 1 , 2 . Order i means constraint Gi.
  //! NbIter is the maximum number of iteration to optimise the number of points for resolution
  //! Degree is the degree of resolution for Plate
  //! Tol2d is the tolerance used to test if two points of different constraint are identical in the
  //! parametric space of the initial surface
  //! Tol3d is used to test if two identical points in the 2d space are identical in 3d space
  //! TolAng is used to compare the angle between normal of two identical points in the 2d space
  //! Raises  ConstructionError;
  Standard_EXPORT GeomPlate_BuildPlateSurface(const Handle(TColStd_HArray1OfInteger)& NPoints, const Handle(GeomPlate_HArray1OfHCurve)& TabCurve, const Handle(TColStd_HArray1OfInteger)& Tang, const Standard_Integer Degree, const Standard_Integer NbIter = 3, const Standard_Real Tol2d = 0.00001, const Standard_Real Tol3d = 0.0001, const Standard_Real TolAng = 0.01, const Standard_Real TolCurv = 0.1, const Standard_Boolean Anisotropie = Standard_False);
  
  Standard_EXPORT GeomPlate_BuildPlateSurface(const Handle(Geom_Surface)& Surf, const Standard_Integer Degree = 3, const Standard_Integer NbPtsOnCur = 10, const Standard_Integer NbIter = 3, const Standard_Real Tol2d = 0.00001, const Standard_Real Tol3d = 0.0001, const Standard_Real TolAng = 0.01, const Standard_Real TolCurv = 0.1, const Standard_Boolean Anisotropie = Standard_False);
  
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
  Standard_EXPORT GeomPlate_BuildPlateSurface(const Standard_Integer Degree = 3, const Standard_Integer NbPtsOnCur = 10, const Standard_Integer NbIter = 3, const Standard_Real Tol2d = 0.00001, const Standard_Real Tol3d = 0.0001, const Standard_Real TolAng = 0.01, const Standard_Real TolCurv = 0.1, const Standard_Boolean Anisotropie = Standard_False);
  
  //! Resets all constraints
  Standard_EXPORT void Init();
  
  //! Loads the initial Surface
  Standard_EXPORT void LoadInitSurface (const Handle(Geom_Surface)& Surf);
  
  //! Adds the linear constraint cont.
  Standard_EXPORT void Add (const Handle(GeomPlate_CurveConstraint)& Cont);
  
  Standard_EXPORT void SetNbBounds (const Standard_Integer NbBounds);
  
  //! Adds the point constraint cont.
  Standard_EXPORT void Add (const Handle(GeomPlate_PointConstraint)& Cont);
  

  //! Calls the algorithm and computes the plate surface using
  //! the loaded constraints. If no initial surface is given, the
  //! algorithm automatically computes one.
  //! Exceptions
  //! Standard_RangeError if the value of the constraint is
  //! null or if plate is not done.
  Standard_EXPORT void Perform();
  
  //! returns the CurveConstraints of order order
  Standard_EXPORT Handle(GeomPlate_CurveConstraint) CurveConstraint (const Standard_Integer order) const;
  
  //! returns the PointConstraint of order order
  Standard_EXPORT Handle(GeomPlate_PointConstraint) PointConstraint (const Standard_Integer order) const;
  
  Standard_EXPORT void Disc2dContour (const Standard_Integer nbp, TColgp_SequenceOfXY& Seq2d);
  
  Standard_EXPORT void Disc3dContour (const Standard_Integer nbp, const Standard_Integer iordre, TColgp_SequenceOfXYZ& Seq3d);
  

  //! Tests whether computation of the plate has been completed.
  Standard_EXPORT Standard_Boolean IsDone() const;
  

  //! Returns the result of the computation. This surface can
  //! then be used by GeomPlate_MakeApprox for
  //! converting the resulting surface into a BSpline.
  Standard_EXPORT Handle(GeomPlate_Surface) Surface() const;
  
  //! Returns the initial surface
  Standard_EXPORT Handle(Geom_Surface) SurfInit() const;
  

  //! Allows you to ensure that the array of curves returned by
  //! Curves2d has the correct orientation. Returns the
  //! orientation of the curves in the the array returned by
  //! Curves2d. Computation changes the orientation of
  //! these curves. Consequently, this method returns the
  //! orientation prior to computation.
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) Sense() const;
  

  //! Extracts the array of curves on the plate surface which
  //! correspond to the curve constraints set in Add.
  Standard_EXPORT Handle(TColGeom2d_HArray1OfCurve) Curves2d() const;
  

  //! Returns the order of the curves in the array returned by
  //! Curves2d. Computation changes this order.
  //! Consequently, this method returns the order of the
  //! curves prior to computation.
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) Order() const;
  
  //! Returns the max distance betwen the result and the constraints
  Standard_EXPORT Standard_Real G0Error() const;
  
  //! Returns  the max angle betwen the result and the constraints
  Standard_EXPORT Standard_Real G1Error() const;
  
  //! Returns  the max difference of curvature betwen the result and the constraints
  Standard_EXPORT Standard_Real G2Error() const;
  
  //! Returns   the max distance between the result and the constraint Index
  Standard_EXPORT Standard_Real G0Error (const Standard_Integer Index);
  
  //! Returns the max angle between the result and the constraint Index
  Standard_EXPORT Standard_Real G1Error (const Standard_Integer Index);
  
  //! Returns the max difference of curvature between the result and the constraint Index
  Standard_EXPORT Standard_Real G2Error (const Standard_Integer Index);




protected:





private:

  
  //! Evaluates the distance, the angle between normals, and the "courbure"
  //! on middle points of contraints an corresponding points on the GeomPlate_Surface
  //! the results are given for a curve c
  Standard_EXPORT void EcartContraintesMil (const Standard_Integer c, Handle(TColStd_HArray1OfReal)& d, Handle(TColStd_HArray1OfReal)& an, Handle(TColStd_HArray1OfReal)& courb);
  
  Standard_EXPORT gp_Pnt2d ProjectPoint (const gp_Pnt& P);
  
  Standard_EXPORT Handle(Geom2d_Curve) ProjectCurve (const Handle(Adaptor3d_HCurve)& Curv);
  
  Standard_EXPORT Handle(Adaptor2d_HCurve2d) ProjectedCurve (Handle(Adaptor3d_HCurve)& Curv);
  
  Standard_EXPORT void ComputeSurfInit();
  
  Standard_EXPORT void Intersect (Handle(GeomPlate_HArray1OfSequenceOfReal)& PntInter, Handle(GeomPlate_HArray1OfSequenceOfReal)& PntG1G1);
  
  Standard_EXPORT void Discretise (const Handle(GeomPlate_HArray1OfSequenceOfReal)& PntInter, const Handle(GeomPlate_HArray1OfSequenceOfReal)& PntG1G1);
  
  Standard_EXPORT void LoadCurve (const Standard_Integer NbBoucle, const Standard_Integer OrderMax = 2);
  
  Standard_EXPORT void LoadPoint (const Standard_Integer NbBoucle, const Standard_Integer OrderMax = 2);
  
  Standard_EXPORT void CalculNbPtsInit();
  
  Standard_EXPORT Standard_Boolean VerifSurface (const Standard_Integer NbLoop);
  
  Standard_EXPORT void VerifPoints (Standard_Real& dist, Standard_Real& ang, Standard_Real& curv) const;
  
  Standard_EXPORT Standard_Boolean CourbeJointive (const Standard_Real tolerance);
  
  Standard_EXPORT Standard_Real ComputeAnisotropie() const;
  
  Standard_EXPORT Standard_Boolean IsOrderG1() const;


  Handle(GeomPlate_HSequenceOfCurveConstraint) myLinCont;
  Handle(GeomPlate_HArray1OfSequenceOfReal) myParCont;
  Handle(GeomPlate_HArray1OfSequenceOfReal) myPlateCont;
  Handle(GeomPlate_HSequenceOfPointConstraint) myPntCont;
  Handle(Geom_Surface) mySurfInit;
  Handle(Geom_Surface) myPlanarSurfInit;
  Handle(GeomPlate_Surface) myGeomPlateSurface;
  Plate_Plate myPlate;
  Plate_Plate myPrevPlate;
  Standard_Boolean myAnisotropie;
  Handle(TColStd_HArray1OfInteger) mySense;
  Standard_Integer myDegree;
  Handle(TColStd_HArray1OfInteger) myInitOrder;
  Standard_Real myG0Error;
  Standard_Real myG1Error;
  Standard_Real myG2Error;
  Standard_Integer myNbPtsOnCur;
  Standard_Boolean mySurfInitIsGive;
  Standard_Integer myNbIter;
  Extrema_ExtPS myProj;
  Standard_Real myTol2d;
  Standard_Real myTol3d;
  Standard_Real myTolAng;
  Standard_Real myTolU;
  Standard_Real myTolV;
  Standard_Integer myNbBounds;
  Standard_Boolean myIsLinear;
  Standard_Boolean myFree;


};







#endif // _GeomPlate_BuildPlateSurface_HeaderFile
