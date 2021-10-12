// Created on: 1996-02-26
// Created by: Philippe MANGIN
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

#ifndef _FairCurve_MinimalVariation_HeaderFile
#define _FairCurve_MinimalVariation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <FairCurve_Batten.hxx>
#include <Standard_Boolean.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class gp_Pnt2d;
class gp_Vec2d;


//! Computes a 2D curve using an algorithm which
//! minimizes tension, sagging, and jerk energy. As in
//! FairCurve_Batten, two reference points are used.
//! Unlike that class, FairCurve_MinimalVariation
//! requires curvature settings at the first and second
//! reference points. These are defined by the rays of
//! curvature desired at each point.
class FairCurve_MinimalVariation  : public FairCurve_Batten
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs the two contact points P1 and P2  and the geometrical
  //! characteristics of the batten (elastic beam)
  //! These include the real number values for height of
  //! deformation Height, slope value Slope, and kind of
  //! energy PhysicalRatio. The kinds of energy include:
  //! -   Jerk (0)
  //! -   Sagging (1).
  //! Note that the default setting for Physical Ration is in FairCurve_Batten
  //! Other parameters are initialized as follow :
  //! - FreeSliding = False
  //! - ConstraintOrder1 = 1
  //! - ConstraintOrder2 = 1
  //! - Angle1 = 0
  //! - Angle2 = 0
  //! - Curvature1 = 0
  //! - Curvature2 = 0
  //! - SlidingFactor = 1
  //! Warning
  //! If PhysicalRatio equals 1, you cannot impose constraints on curvature.
  //! Exceptions
  //! NegativeValue if Height is less than or equal to 0.
  //! NullValue if the distance between P1 and P2 is less
  //! than or equal to the tolerance value for distance in
  //! Precision::Confusion: P1.IsEqual(P2,
  //! Precision::Confusion()). The function
  //! gp_Pnt2d::IsEqual tests to see if this is the case.
  //! Definition of the geometricals constraints
  Standard_EXPORT FairCurve_MinimalVariation(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Real Heigth, const Standard_Real Slope = 0, const Standard_Real PhysicalRatio = 0);
  
  //! Allows you to set a new constraint on curvature at the first point.
    void SetCurvature1 (const Standard_Real Curvature);
  
  //! Allows you to set a new constraint on curvature at the second point.
    void SetCurvature2 (const Standard_Real Curvature);
  
  //! Allows you to set the physical ratio Ratio.
  //! The kinds of energy which you can specify include:
  //! 0 is only "Jerk" Energy
  //! 1 is only "Sagging" Energy like batten
  //! Warning: if Ratio is 1 it is impossible to impose curvature constraints.
  //! Raises  DomainError if Ratio < 0 or Ratio > 1
    void SetPhysicalRatio (const Standard_Real Ratio);
  
  //! Computes the curve with respect to the constraints,
  //! NbIterations and Tolerance. The tolerance setting
  //! allows you to control the precision of computation, and
  //! the maximum number of iterations allows you to set a limit on computation time.
  Standard_EXPORT virtual Standard_Boolean Compute (FairCurve_AnalysisCode& ACode, const Standard_Integer NbIterations = 50, const Standard_Real Tolerance = 1.0e-3) Standard_OVERRIDE;
  
  //! Returns the first established curvature.
    Standard_Real GetCurvature1() const;
  
  //! Returns the second established curvature.
    Standard_Real GetCurvature2() const;
  
  //! Returns the physical ratio, or kind of energy.
    Standard_Real GetPhysicalRatio() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT virtual void Dump (Standard_OStream& o) const Standard_OVERRIDE;




protected:





private:

  
  //! compute the curve with respect of the delta-constraints.
  Standard_EXPORT Standard_Boolean Compute (const gp_Vec2d& DeltaP1, const gp_Vec2d& DeltaP2, const Standard_Real DeltaAngle1, const Standard_Real DeltaAngle2, const Standard_Real DeltaCurvature1, const Standard_Real DeltaCurvature2, FairCurve_AnalysisCode& ACode, const Standard_Integer NbIterations, const Standard_Real Tolerance);


  Standard_Real OldCurvature1;
  Standard_Real OldCurvature2;
  Standard_Real OldPhysicalRatio;
  Standard_Real NewCurvature1;
  Standard_Real NewCurvature2;
  Standard_Real NewPhysicalRatio;


};


#include <FairCurve_MinimalVariation.lxx>





#endif // _FairCurve_MinimalVariation_HeaderFile
