// Created on: 1992-10-20
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dGcc_Circ2dTanOnRad_HeaderFile
#define _Geom2dGcc_Circ2dTanOnRad_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfCirc2d.hxx>
#include <GccEnt_Array1OfPosition.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <GccEnt_Position.hxx>
class Geom2dGcc_QualifiedCurve;
class Geom2dAdaptor_Curve;
class Geom2d_Point;
class GccAna_Circ2dTanOnRad;
class Geom2dGcc_Circ2dTanOnRadGeo;
class gp_Circ2d;
class gp_Pnt2d;


//! This class implements the algorithms used to
//! create a 2d circle tangent to a 2d entity,
//! centered on a 2d entity and with a given radius.
//! More than one argument must be a curve.
//! The arguments of all construction methods are :
//! - The qualified element for the tangency constrains
//! (QualifiedCirc, QualifiedLin, QualifiedCurvPoints).
//! - The Center element (circle, line, curve).
//! - A real Tolerance.
//! Tolerance is only used in the limits cases.
//! For example :
//! We want to create a circle tangent to an OutsideCurv Cu1
//! centered on a line OnLine with a radius Radius and with
//! a tolerance Tolerance.
//! If we did not used Tolerance it is impossible to
//! find a solution in the following case : OnLine is
//! outside Cu1. There is no intersection point between Cu1
//! and OnLine. The distance between the line and the
//! circle is greater than Radius.
//! With Tolerance we will give a solution if the
//! distance between Cu1 and OnLine is lower than or
//! equal Tolerance.
class Geom2dGcc_Circ2dTanOnRad 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs one or more 2D circles of radius Radius,
  //! centered on the 2D curve OnCurv and:
  //! -   tangential to the curve Qualified1
  Standard_EXPORT Geom2dGcc_Circ2dTanOnRad(const Geom2dGcc_QualifiedCurve& Qualified1, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Radius, const Standard_Real Tolerance);
  
  //! Constructs one or more 2D circles of radius Radius,
  //! centered on the 2D curve OnCurv and:
  //! passing through the point Point1.
  //! OnCurv is an adapted curve, i.e. an object which is an
  //! interface between:
  //! -   the services provided by a 2D curve from the package Geom2d,
  //! -   and those required on the curve by the construction algorithm.
  //! Similarly, the qualified curve Qualified1 is created from
  //! an adapted curve.
  //! Adapted curves are created in the following way:
  //! Handle(Geom2d_Curve) myCurveOn = ... ;
  //! Geom2dAdaptor_Curve OnCurv ( myCurveOn ) ;
  //! The algorithm is then constructed with this object:
  //! Handle(Geom2d_Curve) myCurve1 = ...
  //! ;
  //! Geom2dAdaptor_Curve Adapted1 ( myCurve1 ) ;
  //! Geom2dGcc_QualifiedCurve
  //! Qualified1 = Geom2dGcc::Outside(Adapted1);
  //! Standard_Real Radius = ... , Tolerance = ... ;
  //! Geom2dGcc_Circ2dTanOnRad
  //! myAlgo ( Qualified1 , OnCurv , Radius , Tolerance ) ;
  //! if ( myAlgo.IsDone() )
  //! { Standard_Integer Nbr = myAlgo.NbSolutions() ;
  //! gp_Circ2d Circ ;
  //! for ( Standard_Integer i = 1 ;
  //! i <= nbr ; i++ )
  //! { Circ = myAlgo.ThisSolution (i) ;
  //! ...
  //! }
  //! }
  Standard_EXPORT Geom2dGcc_Circ2dTanOnRad(const Handle(Geom2d_Point)& Point1, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Radius, const Standard_Real Tolerance);
  
  Standard_EXPORT void Results (const GccAna_Circ2dTanOnRad& Circ);
  
  Standard_EXPORT void Results (const Geom2dGcc_Circ2dTanOnRadGeo& Circ);
  
  //! Returns true if the construction algorithm does not fail
  //! (even if it finds no solution).
  //! Note: IsDone protects against a failure arising from a
  //! more internal intersection algorithm which has reached
  //! its numeric limits.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of circles, representing solutions
  //! computed by this algorithm.
  //! Exceptions: StdFail_NotDone if the construction fails.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions.
  //! Be careful: the Index is only a way to get all the
  //! solutions, but is not associated to these outside the context
  //! of the algorithm-object.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT gp_Circ2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns the qualifier Qualif1 of the tangency argument
  //! for the solution of index Index computed by this algorithm.
  //! The returned qualifier is:
  //! -   that specified at the start of construction when the
  //! solutions are defined as enclosed, enclosing or
  //! outside with respect to the arguments, or
  //! -   that computed during construction (i.e. enclosed,
  //! enclosing or outside) when the solutions are defined
  //! as unqualified with respect to the arguments, or
  //! -   GccEnt_noqualifier if the tangency argument is a point.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void WhichQualifier (const Standard_Integer Index, GccEnt_Position& Qualif1) const;
  
  //! Returns information about the tangency point between the
  //! result number Index and the first argument.
  //! ParSol is the intrinsic parameter of the point on the solution curv.
  //! ParArg is the intrinsic parameter of the point on the argument curv.
  //! PntSol is the tangency point on the solution curv.
  //! PntArg is the tangency point on the argument curv.
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void Tangency1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns the center PntSol on the second argument (i.e.
  //! line or circle) of the solution of index Index computed by
  //! this algorithm.
  //! ParArg is the intrinsic parameter of the point on the argument curv.
  //! PntSol is the center point of the solution curv.
  //! PntArg is the projection of PntSol on the argument curv.
  //! Exceptions:
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  //! StdFail_NotDone if the construction fails.
  Standard_EXPORT void CenterOn3 (const Standard_Integer Index, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns true if the solution of index Index and the first
  //! argument of this algorithm are the same (i.e. there are 2
  //! identical circles).
  //! If Rarg is the radius of the first argument, Rsol is the
  //! radius of the solution and dist is the distance between
  //! the two centers, we consider the two circles to be
  //! identical if |Rarg - Rsol| and dist are less than
  //! or equal to the tolerance criterion given at the time of
  //! construction of this algorithm.
  //! OutOfRange is raised if Index is greater than the number of solutions.
  //! notDone is raised if the construction algorithm did not succeed.
  Standard_EXPORT Standard_Boolean IsTheSame1 (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfCirc2d cirsol;
  GccEnt_Array1OfPosition qualifier1;
  TColStd_Array1OfInteger TheSame1;
  TColgp_Array1OfPnt2d pnttg1sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal pararg1;
  TColgp_Array1OfPnt2d pntcen3;
  TColStd_Array1OfReal parcen3;


};







#endif // _Geom2dGcc_Circ2dTanOnRad_HeaderFile
