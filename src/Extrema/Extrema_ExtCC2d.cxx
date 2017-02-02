// Created on: 1994-07-06
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Adaptor2d_Curve2d.hxx>
#include <ElCLib.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_ECC2d.hxx>
#include <Extrema_ExtCC2d.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtElC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Extrema_SequenceOfPOnCurv2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>

Extrema_ExtCC2d::Extrema_ExtCC2d()
: myIsFindSingleSolution(Standard_False)
{
}


Extrema_ExtCC2d::Extrema_ExtCC2d(const Adaptor2d_Curve2d&       C1,
                                 const Adaptor2d_Curve2d&       C2,
                                 const Standard_Real TolC1,
                                 const Standard_Real TolC2)
: myIsFindSingleSolution(Standard_False)
{
  Initialize(C2, Extrema_Curve2dTool::FirstParameter(C2), Extrema_Curve2dTool::LastParameter(C2), TolC1, TolC2);
  Perform(C1, Extrema_Curve2dTool::FirstParameter(C1), Extrema_Curve2dTool::LastParameter(C1));
}

Extrema_ExtCC2d::Extrema_ExtCC2d(const Adaptor2d_Curve2d&        C1, 
                                 const Adaptor2d_Curve2d&        C2,
                                 const Standard_Real  U1,
                                 const Standard_Real  U2,
                                 const Standard_Real  V1,
                                 const Standard_Real  V2,
                                 const Standard_Real  TolC1,
                                 const Standard_Real  TolC2)
: myIsFindSingleSolution(Standard_False)
{
  Initialize(C2, V1, V2, TolC1, TolC2);
  Perform(C1, U1, U2);
}



void Extrema_ExtCC2d::Initialize(const Adaptor2d_Curve2d&        C2,
				  const Standard_Real  V1,
				  const Standard_Real  V2,
				  const Standard_Real  TolC1,
				  const Standard_Real  TolC2)
{
  myC = (Standard_Address)&C2;
  myv1 = V1;
  myv2 = V2;
  mytolc1 = TolC1;
  mytolc2 = TolC2;
}



void Extrema_ExtCC2d::Perform (const Adaptor2d_Curve2d&       C1,
				const Standard_Real U1,
				const Standard_Real U2)
{
  mypoints.Clear();
  mySqDist.Clear();
  GeomAbs_CurveType type1 = Extrema_Curve2dTool::GetType(C1), type2 = Extrema_Curve2dTool::GetType(*((Adaptor2d_Curve2d*)myC));
  Standard_Real U11, U12, U21, U22, Tol = Min(mytolc1, mytolc2);
//  Extrema_POnCurv2d P1, P2;
  mynbext = 0;
  inverse = Standard_False;
  myIsPar = Standard_False;

  U11 = U1;
  U12 = U2;
  U21 = myv1;
  U22 = myv2;
  P1f = Extrema_Curve2dTool::Value(C1, U11);
  P1l = Extrema_Curve2dTool::Value(C1, U12);
  P2f = Extrema_Curve2dTool::Value(*((Adaptor2d_Curve2d*)myC), U21);
  P2l = Extrema_Curve2dTool::Value(*((Adaptor2d_Curve2d*)myC), U22);


  switch(type1) {
    //
    //  La premiere courbe est un cercle:
    //
  case GeomAbs_Circle: {

    switch(type2) {
      case GeomAbs_Line: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Circle(C1), Tol);
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 0.);
        }
	break;
      case GeomAbs_Circle: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(C1), Extrema_Curve2dTool::Circle(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 2*M_PI);
        }
	break;
      case GeomAbs_Ellipse: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(C1), Extrema_Curve2dTool::Ellipse(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 2*M_PI );
        }
	break;
      case GeomAbs_Parabola: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(C1), Extrema_Curve2dTool::Parabola(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 0.);
      }
	break;
      case GeomAbs_Hyperbola: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(C1), Extrema_Curve2dTool::Hyperbola(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 0. );
      }
	break;
      default: {
          Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
          aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
          aParamSolver.Perform();
          Standard_Real Period2 = 0.;
          if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
          Results(aParamSolver, U11, U12, U21, U22, 2*M_PI,Period2);
        }
	break;
      };  // switch(type2)
    }
    break;

    //
    // La premiere courbe est une ellipse:
    //
  case GeomAbs_Ellipse: {

    switch(type2) {
      case GeomAbs_Line: {
        inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Ellipse(C1));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 0.);
        }
	break;
      case GeomAbs_Circle: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Ellipse(C1));
	Results(Xtrem, U11, U12, U21, U22, 2*M_PI, 2*M_PI);
        }
	break;
      case GeomAbs_Ellipse:
        {
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22,2*M_PI, 2*M_PI);
        }
	break;
      case GeomAbs_Parabola: {
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Ellipse(C1), Extrema_Curve2dTool::Parabola(*((Adaptor2d_Curve2d*)myC)));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 2*M_PI, 0.);
      }
	break;
      case GeomAbs_Hyperbola: {
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Ellipse(C1), Extrema_Curve2dTool::Hyperbola(*((Adaptor2d_Curve2d*)myC)));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 2*M_PI, 0.);
      }
	break;
      default: {
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Standard_Real Period2 = 0.;
	if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
	Results(aParamSolver, U11, U12, U21, U22, 2*M_PI,Period2);
        }
	break;
      };  // switch(type2)
    }
    break;

    //
    // La premiere courbe est une parabole: 
    //
  case GeomAbs_Parabola: {

    switch(type2) {
      case GeomAbs_Line: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Parabola(C1));
	Results(Xtrem, U11, U12, U21, U22, 0., 0.);
        }
	break;
      case GeomAbs_Circle: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Parabola(C1));
	Results(Xtrem, U11, U12, U21, U22, 0., 2*M_PI);
        }
	break;
      case GeomAbs_Ellipse: {
	//inverse = Standard_True;
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Ellipse(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Parabola(C1));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 2*M_PI);
        }
	break;
      case GeomAbs_Parabola: {
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Parabola(C1), Extrema_Curve2dTool::Parabola(*((Adaptor2d_Curve2d*)myC)));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 0.);
      }
	break;
      case GeomAbs_Hyperbola: {
	//inverse = Standard_True;
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Hyperbola(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Parabola(C1));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 0.);
      }
	break;
      default: {
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Standard_Real Period2 = 0.;
	if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
	Results(aParamSolver, U11, U12, U21, U22, 0., Period2);
        }
	break;
      };  // switch(type2)
    }
    break;

    //
    // La premiere courbe est une hyperbole:
    //
  case GeomAbs_Hyperbola: {

    switch(type2) {
      case GeomAbs_Line: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Hyperbola(C1));
	Results(Xtrem, U11, U12, U21, U22, 0., 0.);
        }
	break;
      case GeomAbs_Circle: {
	inverse = Standard_True;
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Circle(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Hyperbola(C1));
	Results(Xtrem, U11, U12, U21, U22, 0., 2*M_PI);
        }
	break;
      case GeomAbs_Ellipse: {
	//inverse = Standard_True;
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Ellipse(*((Adaptor2d_Curve2d*)myC)), Extrema_Curve2dTool::Hyperbola(C1));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 2*M_PI );
        }
	break;
      case GeomAbs_Parabola: {
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Hyperbola(C1), Extrema_Curve2dTool::Parabola(*((Adaptor2d_Curve2d*)myC)));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 0.);
      }
	break;
      case GeomAbs_Hyperbola: {
	//Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Hyperbola(C1), Extrema_Curve2dTool::Hyperbola(*((Adaptor2d_Curve2d*)myC)));
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Results(aParamSolver, U11, U12, U21, U22, 0., 0.);
      }
	break;
      default: {
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Standard_Real Period2 = 0.;
	if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
	Results(aParamSolver, U11, U12, U21, U22, 0., Period2);
        }
	break;
      };  // switch(type2)
    }
    break;

    //
    // La premiere courbe est une Line:
    //
  case GeomAbs_Line: {

    switch(type2) {
      case GeomAbs_Line: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(C1), Extrema_Curve2dTool::Line(*((Adaptor2d_Curve2d*)myC)), Tol);
	Results(Xtrem, U11, U12, U21, U22, 0., 0.);
        } 
	break;
      case GeomAbs_Circle: {
  	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(C1), Extrema_Curve2dTool::Circle(*((Adaptor2d_Curve2d*)myC)), Tol);
	Results(Xtrem, U11, U12, U21, U22, 0., 2*M_PI);
        }
	break;
      case GeomAbs_Ellipse: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(C1), Extrema_Curve2dTool::Ellipse(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 0., 2*M_PI);
        }
	break;
      case GeomAbs_Parabola: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(C1), Extrema_Curve2dTool::Parabola(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 0., 0.);
      }
	break;
      case GeomAbs_Hyperbola: {
	Extrema_ExtElC2d Xtrem(Extrema_Curve2dTool::Line(C1), Extrema_Curve2dTool::Hyperbola(*((Adaptor2d_Curve2d*)myC)));
	Results(Xtrem, U11, U12, U21, U22, 0., 0.);
      }
	break;
      default: {
	Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
        aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
        aParamSolver.Perform();
	Standard_Real Period2 = 0.;
	if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
	Results(aParamSolver, U11, U12, U21, U22, 0., Period2);
        }
	break;
      };  // switch(type2)
    }
    break;

    //
    // La premiere courbe est une BezierCurve ou une BSplineCurve:
    //
  default: {
    Extrema_ECC2d aParamSolver(C1, *((Adaptor2d_Curve2d*)myC));
    aParamSolver.SetSingleSolutionFlag(GetSingleSolutionFlag());
    aParamSolver.Perform();
    Standard_Real Period1 = 0.;
    if (Extrema_Curve2dTool::IsPeriodic(C1)) Period1 = Extrema_Curve2dTool::Period(C1);
    Standard_Real Period2 = 0.;
    if (Extrema_Curve2dTool::IsPeriodic(*((Adaptor2d_Curve2d*)myC))) Period2 = Extrema_Curve2dTool::Period(*((Adaptor2d_Curve2d*)myC));
    Results(aParamSolver, U11, U12, U21, U22, Period1, Period2);
    }
    break;

  };
    
}


Standard_Boolean Extrema_ExtCC2d::IsDone() const
{
  return myDone;
}


Standard_Real Extrema_ExtCC2d::SquareDistance(const Standard_Integer N) const 
{
  if(!myDone) throw StdFail_NotDone();
  if ((N <= 0) || (N > mynbext)) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


Standard_Integer Extrema_ExtCC2d::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return mynbext;
}


void Extrema_ExtCC2d::Points(const Standard_Integer N, 
			      Extrema_POnCurv2d& P1,
			      Extrema_POnCurv2d& P2) const
{
  if(!myDone) throw StdFail_NotDone();
  if ((N <= 0) || (N > mynbext)) throw Standard_OutOfRange();
  P1 = mypoints.Value(2*N-1);
  P2 = mypoints.Value(2*N);
}



void Extrema_ExtCC2d::TrimmedSquareDistances(Standard_Real& dist11,
					Standard_Real& dist12,
					Standard_Real& dist21,
					Standard_Real& dist22,
					gp_Pnt2d&      P11,
					gp_Pnt2d&      P12,
					gp_Pnt2d&      P21,
					gp_Pnt2d&      P22) const 
{
  dist11 = mydist11;
  dist12 = mydist12;
  dist21 = mydist21;
  dist22 = mydist22;
  P11 = P1f;
  P12 = P1l;
  P21 = P2f;
  P22 = P2l;
}



void Extrema_ExtCC2d::Results(const Extrema_ExtElC2d&  AlgExt,
			       const Standard_Real      Ut11,
			       const Standard_Real      Ut12,
			       const Standard_Real      Ut21,
			       const Standard_Real      Ut22,
			       const Standard_Real      Period1,
			       const Standard_Real      Period2)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv2d P1, P2;
  
  myDone = AlgExt.IsDone();
  myIsPar = AlgExt.IsParallel();
  if (myDone) {
    if (!myIsPar) {
      NbExt = AlgExt.NbExt();
      for (i = 1; i <= NbExt; i++) {
	// Verification de la validite des parametres pour le cas trimme:
	AlgExt.Points(i, P1, P2);
	if (!inverse) {
	  U = P1.Parameter();
	  if (Period1 != 0.0) U = ElCLib::InPeriod(U,Ut11,Ut11+Period1);
	  U2 = P2.Parameter();
	  if (Period2 != 0.0) U2 = ElCLib::InPeriod(U2,Ut21,Ut21+Period2);
	}
	else {
	  U2 = P1.Parameter();
	  if (Period2 != 0.0) U2 = ElCLib::InPeriod(U2,Ut21,Ut21+Period2);
	  U = P2.Parameter();
	  if (Period1 != 0.0) U = ElCLib::InPeriod(U,Ut11,Ut11+Period1);
	}
	if ((U  >= Ut11 - Precision::PConfusion())  && 
	    (U  <= Ut12 + Precision::PConfusion())  &&
	    (U2 >= Ut21 - Precision::PConfusion())  &&
	    (U2 <= Ut22 + Precision::PConfusion())) {
	  mynbext++;
	  Val = AlgExt.SquareDistance(i);
	  mySqDist.Append(Val);
	  if (!inverse) {
	    P1.SetValues(U, P1.Value());
	    P2.SetValues(U2, P2.Value());
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  else {
	    P1.SetValues(U2, P1.Value());
	    P2.SetValues(U, P2.Value());
	    mypoints.Append(P2);
	    mypoints.Append(P1);
	  }
	}
      }
    }

    mydist11 = P1f.SquareDistance(P2f);
    mydist12 = P1f.SquareDistance(P2l);
    mydist21 = P1l.SquareDistance(P2f);
    mydist22 = P1l.SquareDistance(P2l);
  }
}


void Extrema_ExtCC2d::Results(const Extrema_ECC2d& AlgExt,
                              const Standard_Real  Ut11,
                              const Standard_Real  Ut12,
                              const Standard_Real  Ut21,
                              const Standard_Real  Ut22,
                              const Standard_Real  Period1,
                              const Standard_Real  Period2)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv2d P1, P2;

  myDone = AlgExt.IsDone();
  if (myDone)
  {
    myIsPar = AlgExt.IsParallel();
    NbExt = AlgExt.NbExt();
    for (i = 1; i <= NbExt; i++)
    {
      // Verification de la validite des parametres pour le cas trimme:
      AlgExt.Points(i, P1, P2);
      U = P1.Parameter();
      if (Period1 != 0.0) 
        U = ElCLib::InPeriod(U,Ut11,Ut11+Period1);
      U2 = P2.Parameter();
      if (Period2 != 0.0) 
        U2 = ElCLib::InPeriod(U2,Ut21,Ut21+Period2);

      if ((U  >= Ut11 - Precision::PConfusion())  && 
        (U  <= Ut12 + Precision::PConfusion())  &&
        (U2 >= Ut21 - Precision::PConfusion())  &&
        (U2 <= Ut22 + Precision::PConfusion()))
      {
        mynbext++;
        Val = AlgExt.SquareDistance(i);
        P1.SetValues(U, P1.Value());
        P2.SetValues(U2, P2.Value());
        mySqDist.Append(Val);
        mypoints.Append(P1);
        mypoints.Append(P2);
      }
    }

    mydist11 = P1f.SquareDistance(P2f);
    mydist12 = P1f.SquareDistance(P2l);
    mydist21 = P1l.SquareDistance(P2f);
    mydist22 = P1l.SquareDistance(P2l);
  }
}


Standard_Boolean Extrema_ExtCC2d::IsParallel() const
{
  if (!myDone) throw StdFail_NotDone();
  return myIsPar;
}

//=======================================================================
//function : SetSingleSolutionFlag
//purpose  : 
//=======================================================================
void Extrema_ExtCC2d::SetSingleSolutionFlag(const Standard_Boolean theFlag)
{
  myIsFindSingleSolution = theFlag;
}

//=======================================================================
//function : GetSingleSolutionFlag
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtCC2d::GetSingleSolutionFlag() const
{
  return myIsFindSingleSolution;
}
