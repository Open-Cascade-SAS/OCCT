// Created on: 1996-11-08
// Created by: Jean Claude VAUTHIER
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

#ifndef _GCPnts_TangentialDeflection_HeaderFile
#define _GCPnts_TangentialDeflection_HeaderFile

#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <gp_Pnt.hxx>
#include <math_Function.hxx>
#include <math_MultipleVarFunction.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>

//! Computes a set of  points on a curve from package
//! Adaptor3d  such  as between  two successive   points
//! P1(u1)and P2(u2) :
//! @code
//! . ||P1P3^P3P2||/||P1P3||*||P3P2||<AngularDeflection
//! . ||P1P2^P1P3||/||P1P2||<CurvatureDeflection
//! @endcode
//! where P3 is the point of abscissa ((u1+u2)/2), with
//! u1 the abscissa of the point P1 and u2 the abscissa
//! of the point P2.
//!
//! ^ is the cross product of two vectors, and ||P1P2||
//! the magnitude of the vector P1P2.
//!
//! The conditions AngularDeflection > gp::Resolution()
//! and CurvatureDeflection > gp::Resolution() must be
//! satisfied at the construction time.
//!
//! A minimum number of points can be fixed for a linear or circular element.
//! Example:
//! @code
//! Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve (thePoles);
//! GeomAdaptor_Curve aCurveAdaptor (aCurve);
//! double aCDeflect  = 0.01; // Curvature deflection
//! double anADeflect = 0.09; // Angular   deflection
//!
//! GCPnts_TangentialDeflection aPointsOnCurve;
//! aPointsOnCurve.Initialize (aCurveAdaptor, anADeflect, aCDeflect);
//! for (int i = 1; i <= aPointsOnCurve.NbPoints(); ++i)
//! {
//!   double aU   = aPointsOnCurve.Parameter (i);
//!   gp_Pnt aPnt = aPointsOnCurve.Value (i);
//! }
//! @endcode
class GCPnts_TangentialDeflection 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  //! @sa Initialize()
  Standard_EXPORT GCPnts_TangentialDeflection();

  //! Constructor for 3D curve.
  //! @param theC [in] 3d curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT GCPnts_TangentialDeflection (const Adaptor3d_Curve& theC,
                                               const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                               const Standard_Integer theMinimumOfPoints = 2,
                                               const Standard_Real theUTol = 1.0e-9,
                                               const Standard_Real theMinLen = 1.0e-7);

  //! Constructor for 3D curve with restricted range.
  //! @param theC [in] 3d curve
  //! @param theFirstParameter [in] first parameter on curve
  //! @param theLastParameter  [in] last  parameter on curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTo  l [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT GCPnts_TangentialDeflection (const Adaptor3d_Curve& theC,
                                               const Standard_Real theFirstParameter, const Standard_Real theLastParameter,
                                               const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                               const Standard_Integer theMinimumOfPoints = 2,
                                               const Standard_Real theUTol = 1.0e-9,
                                               const Standard_Real theMinLen = 1.0e-7);

  //! Constructor for 2D curve.
  //! @param theC [in] 2d curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT GCPnts_TangentialDeflection (const Adaptor2d_Curve2d& theC,
                                               const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                               const Standard_Integer theMinimumOfPoints = 2,
                                               const Standard_Real theUTol = 1.0e-9,
                                               const Standard_Real theMinLen = 1.0e-7);

  //! Constructor for 2D curve with restricted range.
  //! @param theC [in] 2d curve
  //! @param theFirstParameter [in] first parameter on curve
  //! @param theLastParameter  [in] last  parameter on curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT GCPnts_TangentialDeflection (const Adaptor2d_Curve2d& theC,
                                               const Standard_Real theFirstParameter, const Standard_Real theLastParameter,
                                               const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                               const Standard_Integer theMinimumOfPoints = 2,
                                               const Standard_Real theUTol = 1.0e-9,
                                               const Standard_Real theMinLen = 1.0e-7);

  //! Initialize algorithm for 3D curve.
  //! @param theC [in] 3d curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& theC,
                                   const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                   const Standard_Integer theMinimumOfPoints = 2,
                                   const Standard_Real theUTol = 1.0e-9,
                                   const Standard_Real theMinLen = 1.0e-7);

  //! Initialize algorithm for 3D curve with restricted range.
  //! @param theC [in] 3d curve
  //! @param theFirstParameter [in] first parameter on curve
  //! @param theLastParameter  [in] last  parameter on curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& theC,
                                   const Standard_Real theFirstParameter, const Standard_Real theLastParameter,
                                   const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                   const Standard_Integer theMinimumOfPoints = 2,
                                   const Standard_Real theUTol = 1.0e-9,
                                   const Standard_Real theMinLen = 1.0e-7);

  //! Initialize algorithm for 2D curve.
  //! @param theC [in] 2d curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& theC,
                                   const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                   const Standard_Integer theMinimumOfPoints = 2,
                                   const Standard_Real theUTol = 1.0e-9,
                                   const Standard_Real theMinLen = 1.0e-7);

  //! Initialize algorithm for 2D curve with restricted range.
  //! @param theC [in] 2d curve
  //! @param theFirstParameter [in] first parameter on curve
  //! @param theLastParameter  [in] last  parameter on curve
  //! @param theAngularDeflection   [in] angular deflection in radians
  //! @param theCurvatureDeflection [in] linear deflection
  //! @param theMinimumOfPoints [in] minimum number of points
  //! @param theUTol   [in] tolerance in curve parametric scope
  //! @param theMinLen [in] minimal length
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& theC,
                                   const Standard_Real theFirstParameter, const Standard_Real theLastParameter,
                                   const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                                   const Standard_Integer theMinimumOfPoints = 2,
                                   const Standard_Real theUTol = 1.0e-9,
                                   const Standard_Real theMinLen = 1.0e-7);

  //! Add point to already calculated points (or replace existing)
  //! Returns index of new added point
  //! or founded with parametric tolerance (replaced if theIsReplace is true)
  Standard_EXPORT Standard_Integer AddPoint (const gp_Pnt& thePnt,
                                             const Standard_Real theParam,
                                             const Standard_Boolean theIsReplace = Standard_True);

  Standard_Integer NbPoints () const
  {
    return myParameters.Length ();
  }

  Standard_Real Parameter (const Standard_Integer I) const
  {
    return myParameters.Value (I);
  }

  gp_Pnt Value (const Standard_Integer I) const
  {
    return myPoints.Value (I);
  }

  //! Computes angular step for the arc using the given parameters.
  Standard_EXPORT static Standard_Real ArcAngularStep (const Standard_Real theRadius,
                                                       const Standard_Real theLinearDeflection,
                                                       const Standard_Real theAngularDeflection,
                                                       const Standard_Real theMinLength);

private:

  template<class TheCurve>
  void initialize (const TheCurve& theC,
                   const Standard_Real theFirstParameter, const Standard_Real theLastParameter,
                   const Standard_Real theAngularDeflection, const Standard_Real theCurvatureDeflection,
                   const Standard_Integer theMinimumOfPoints,
                   const Standard_Real theUTol,
                   const Standard_Real theMinLen);

  template<class TheCurve>
  void PerformLinear (const TheCurve& theC);

  template<class TheCurve>
  void PerformCircular (const TheCurve& theC);

  //! Respecting the angle and the deflection,
  //! we impose a minimum number of points on a curve.
  template<class TheCurve>
  void PerformCurve (const TheCurve& theC);

  template<class TheCurve>
  void EvaluateDu (const TheCurve& theC,
                   const Standard_Real theU,
                   gp_Pnt& theP,
                   Standard_Real& theDu,
                   Standard_Boolean& theNotDone) const;

  //! Estimation of maximal deflection for interval [theU1, theU2]
  template<class TheCurve>
  void EstimDefl (const TheCurve& theC,
                  const Standard_Real theU1, const Standard_Real theU2,
                  Standard_Real& theMaxDefl, Standard_Real& theUMax);

private:
  Standard_Real myAngularDeflection;
  Standard_Real myCurvatureDeflection;
  Standard_Real myUTol;
  Standard_Integer myMinNbPnts;
  Standard_Real myMinLen;
  Standard_Real myLastU;
  Standard_Real myFirstu;
  TColgp_SequenceOfPnt   myPoints;
  TColStd_SequenceOfReal myParameters;
};

#endif // _GCPnts_TangentialDeflection_HeaderFile
