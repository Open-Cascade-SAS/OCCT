// Created on: 1991-03-29
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Geom2dGcc_Circ2d2TanOnIter_HeaderFile
#define _Geom2dGcc_Circ2d2TanOnIter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Circ2d.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Pnt2d.hxx>
class GccEnt_QualifiedCirc;
class Geom2dGcc_QCurve;
class gp_Lin2d;
class GccEnt_QualifiedLin;
class Geom2dAdaptor_Curve;


//! This class implements the algorithms used to
//! create 2d circles TANgent to 2 entities and
//! having the center ON a curv.
//! The order of the tangency argument is always
//! QualifiedCirc, QualifiedLin, QualifiedCurv, Pnt2d.
//! the arguments are :
//! - The two tangency arguments.
//! - The center line.
//! - The parameter for each tangency argument which
//! is a curve.
//! - The tolerance.
class Geom2dGcc_Circ2d2TanOnIter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a curve and
  //! having the center ON a 2d line.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Lin2d& OnLine, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a curve and
  //! having the center ON a 2d line.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedLin& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Lin2d& OnLine, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two curves and
  //! having the center ON a 2d line.
  //! Param1 is the initial guess on the first QualifiedCurv.
  //! Param2 is the initial guess on the first QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Lin2d& OnLine, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d point and a curve and
  //! having the center ON a 2d line.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& Point2, const gp_Lin2d& OnLine, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a curve and
  //! having the center ON a 2d circle.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Circ2d& OnCirc, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a curve and
  //! having the center ON a 2d circle.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedLin& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Circ2d& OnCirc, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two curves and
  //! having the center ON a 2d circle.
  //! Param1 is the initial guess on the first QualifiedCurv.
  //! Param2 is the initial guess on the first QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const gp_Circ2d& OnCirc, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Param3, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d point and a curve and
  //! having the center ON a 2d circle.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& Point2, const gp_Circ2d& OnCirc, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d circle and a curve and
  //! having the center ON a 2d curve.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedCirc& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dAdaptor_Curve& OnCurv, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real ParamOn, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d line and a curve and
  //! having the center ON a 2d curve.
  //! Param2 is the initial guess on the curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const GccEnt_QualifiedLin& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real ParamOn, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to a 2d Point and a curve and
  //! having the center ON a 2d curve.
  //! Param1 is the initial guess on the curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const gp_Pnt2d& Point2, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Param1, const Standard_Real ParamOn, const Standard_Real Tolerance);
  
  //! This method implements the algorithms used to
  //! create 2d circles TANgent to two curves and
  //! having the center ON a 2d curve.
  //! Param1 is the initial guess on the first curve QualifiedCurv.
  //! Param1 is the initial guess on the second curve QualifiedCurv.
  //! ParamOn is the initial guess on the center curve OnCurv.
  //! Tolerance is used for the limit cases.
  Standard_EXPORT Geom2dGcc_Circ2d2TanOnIter(const Geom2dGcc_QCurve& Qualified1, const Geom2dGcc_QCurve& Qualified2, const Geom2dAdaptor_Curve& OnCurve, const Standard_Real Param1, const Standard_Real Param2, const Standard_Real ParamOn, const Standard_Real Tolerance);
  
  //! This method returns True if the construction
  //! algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the solution.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT gp_Circ2d ThisSolution() const;
  
  Standard_EXPORT void WhichQualifier (GccEnt_Position& Qualif1, GccEnt_Position& Qualif2) const;
  
  //! Returns information about the tangency point between
  //! the result and the first argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void Tangency1 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the tangency point between
  //! the result and the second argument.
  //! ParSol is the intrinsic parameter of the point PntSol
  //! on the solution curv.
  //! ParArg is the intrinsic parameter of the point PntSol
  //! on the argument curv.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void Tangency2 (Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the center (on the curv) of the
  //! result and the third argument.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT void CenterOn3 (Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Boolean IsTheSame1() const;
  
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Standard_Boolean IsTheSame2() const;




protected:





private:



  Standard_Boolean WellDone;
  gp_Circ2d cirsol;
  GccEnt_Position qualifier1;
  GccEnt_Position qualifier2;
  Standard_Boolean TheSame1;
  Standard_Boolean TheSame2;
  gp_Pnt2d pnttg1sol;
  gp_Pnt2d pnttg2sol;
  gp_Pnt2d pntcen;
  Standard_Real par1sol;
  Standard_Real par2sol;
  Standard_Real pararg1;
  Standard_Real pararg2;
  Standard_Real parcen3;


};







#endif // _Geom2dGcc_Circ2d2TanOnIter_HeaderFile
