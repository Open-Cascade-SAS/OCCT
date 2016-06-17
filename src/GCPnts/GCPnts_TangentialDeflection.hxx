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
#include <gp_Lin.hxx>
#include <math_Function.hxx>
#include <math_MultipleVarFunction.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>

class Standard_ConstructionError;
class Standard_OutOfRange;

//! Computes a set of  points on a curve from package
//! Adaptor3d  such  as between  two successive   points
//! P1(u1)and P2(u2) :
//!
//! . ||P1P3^P3P2||/||P1P3||*||P3P2||<AngularDeflection
//! . ||P1P2^P1P3||/||P1P2||<CurvatureDeflection
//!
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
//! A  minimum  number of points  can  be fixed   for a
//! linear or circular element.
//! Example:
//! Handle(Geom_BezierCurve) C = new Geom_BezierCurve (Poles);
//! GeomAdaptor_Curve Curve (C);
//! Real CDeflect = 0.01;  //Curvature deflection
//! Real ADeflect = 0.09; //Angular deflection
//!
//! GCPnts_TangentialDeflection PointsOnCurve;
//! PointsOnCurve.Initialize (Curve, ADeflect, CDeflect);
//!
//! Real U;
//! gp_Pnt P;
//! for (Integer i=1; i<=PointsOnCurve.NbPoints();i++) {
//! U = PointsOnCurve.Parameter (i);
//! P = PointsOnCurve.Value (i);
//! }

class GCPnts_TangentialDeflection 
{
public:

//
  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GCPnts_TangentialDeflection();
  
  Standard_EXPORT GCPnts_TangentialDeflection(const Adaptor3d_Curve& C, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT GCPnts_TangentialDeflection(const Adaptor3d_Curve& C, const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT GCPnts_TangentialDeflection(const Adaptor2d_Curve2d& C, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT GCPnts_TangentialDeflection(const Adaptor2d_Curve2d& C, const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real FirstParameter, const Standard_Real LastParameter, const Standard_Real AngularDeflection, const Standard_Real CurvatureDeflection, const Standard_Integer MinimumOfPoints = 2, const Standard_Real UTol = 1.0e-9, const Standard_Real theMinLen = 1.0e-7);
  
  //! Add point to already calculated points (or replace existing)
  //! Returns index of new added point
  //! or founded with parametric tolerance (replaced if theIsReplace is true)
  Standard_EXPORT Standard_Integer AddPoint (const gp_Pnt& thePnt, const Standard_Real theParam, const Standard_Boolean theIsReplace = Standard_True);
  
  Standard_Integer NbPoints () const
  {
    return parameters.Length ();
  }
  
  Standard_Real Parameter (const Standard_Integer I) const
  {
    return parameters.Value (I);
  }
  
  gp_Pnt Value (const Standard_Integer I) const
  {
    return points.Value (I);
  }
  
  //! Computes angular step for the arc using the given parameters.
  Standard_EXPORT static Standard_Real ArcAngularStep (const Standard_Real theRadius, const Standard_Real theLinearDeflection, const Standard_Real theAngularDeflection, const Standard_Real theMinLength);

private:
  
  Standard_EXPORT void PerformLinear (const Adaptor3d_Curve& C);
  
  Standard_EXPORT void PerformLinear (const Adaptor2d_Curve2d& C);
  
  Standard_EXPORT void PerformCircular (const Adaptor3d_Curve& C);
  
  Standard_EXPORT void PerformCircular (const Adaptor2d_Curve2d& C);
  
  Standard_EXPORT void PerformCurve (const Adaptor3d_Curve& C);
  
  Standard_EXPORT void PerformCurve (const Adaptor2d_Curve2d& C);
  
  Standard_EXPORT void EvaluateDu (const Adaptor3d_Curve& C, const Standard_Real U, gp_Pnt& P, Standard_Real& Du, Standard_Boolean& NotDone) const;
  
  Standard_EXPORT void EvaluateDu (const Adaptor2d_Curve2d& C, const Standard_Real U, gp_Pnt& P, Standard_Real& Du, Standard_Boolean& NotDone) const;

  Standard_EXPORT void EstimDefl (const Adaptor3d_Curve& C, const Standard_Real U1, const Standard_Real U2, 
                                  Standard_Real& MaxDefl, Standard_Real& UMax);
  
  Standard_EXPORT void EstimDefl (const Adaptor2d_Curve2d& C, const Standard_Real U1, const Standard_Real U2, 
                                  Standard_Real& MaxDefl, Standard_Real& UMax);

private:
  Standard_Real angularDeflection;
  Standard_Real curvatureDeflection;
  Standard_Real uTol;
  Standard_Integer minNbPnts;
  Standard_Real myMinLen;
  Standard_Real lastu;
  Standard_Real firstu;
  TColgp_SequenceOfPnt points;
  TColStd_SequenceOfReal parameters;
};

#endif // _GCPnts_TangentialDeflection_HeaderFile
