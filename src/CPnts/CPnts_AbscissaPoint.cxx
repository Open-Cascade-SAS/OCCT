// Copyright (c) 1995-1999 Matra Datavision
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

//------------------------------------------------------------------------
//  Calculate a point with given abscissa starting from a given point 
//  cases processed: straight segment, arc of circle, parameterized curve
//  curve should be C1
//  for a parameterized curve:
//  calculate the total length of the curve
//  calculate an approached point by assimilating the curve to a staight line
//  calculate the length of the curve between the start point and the approached point
//  by succsessive iteration find the point and its associated parameter
//  call to FunctionRoot

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <CPnts_AbscissaPoint.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionRoot.hxx>
#include <math_GaussSingleIntegration.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

// auxiliary functions to compute the length of the derivative
static Standard_Real f3d(const Standard_Real X, const Standard_Address C)
{
  gp_Pnt P;
  gp_Vec V;
  ((Adaptor3d_Curve*)C)->D1(X,P,V);
  return V.Magnitude();
}

static Standard_Real f2d(const Standard_Real X, const Standard_Address C)
{
  gp_Pnt2d P;
  gp_Vec2d V;
  ((Adaptor2d_Curve2d*)C)->D1(X,P,V);
  return V.Magnitude();
}

static Standard_Integer order(const Adaptor3d_Curve& C)
{
  switch (C.GetType()) {
    
  case GeomAbs_Line :
    return 2;

  case GeomAbs_Parabola :
    return 5;

  case GeomAbs_BezierCurve :
    return Min(24, 2*C.Degree());

  case GeomAbs_BSplineCurve :
    return Min(24, 2*C.NbPoles()-1);
    
    default :
      return 10;
  }
}

static Standard_Integer order(const Adaptor2d_Curve2d& C)
{
  switch (C.GetType()) {
    
  case GeomAbs_Line :
    return 2;

  case GeomAbs_Parabola :
    return 5;

  case GeomAbs_BezierCurve :
    return Min(24, 2*C.Bezier()->Degree());

  case GeomAbs_BSplineCurve :
    return Min(24, 2*C.BSpline()->NbPoles()-1);
    
    default :
      return 10;
  }
}


//=======================================================================
//function : Length
//purpose  : 3d
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor3d_Curve& C) 
{
  return CPnts_AbscissaPoint::Length(C, C.FirstParameter(), 
				        C.LastParameter());
}

//=======================================================================
//function : Length
//purpose  : 2d
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& C) 
{
  return CPnts_AbscissaPoint::Length(C, C.FirstParameter(), 
				        C.LastParameter());
}

//=======================================================================
//function : Length
//purpose  : 3d with tolerance
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor3d_Curve& C, const Standard_Real Tol) 
{
  return CPnts_AbscissaPoint::Length(C, C.FirstParameter(), 
				        C.LastParameter(), Tol);
}

//=======================================================================
//function : Length
//purpose  : 2d with tolerance
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& C, const Standard_Real Tol) 
{
  return CPnts_AbscissaPoint::Length(C, C.FirstParameter(), 
				        C.LastParameter(), Tol);
}


//=======================================================================
//function : Length
//purpose  : 3d with parameters
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor3d_Curve& C,
					  const Standard_Real U1,
					  const Standard_Real U2) 
{
  CPnts_MyGaussFunction FG;
//POP pout WNT
  CPnts_RealFunction rf = f3d;
  FG.Init(rf,(Standard_Address)&C);
//  FG.Init(f3d,(Standard_Address)&C);
  math_GaussSingleIntegration TheLength(FG, U1, U2, order(C));
  if (!TheLength.IsDone()) {
    throw Standard_ConstructionError();
  }
  return Abs(TheLength.Value());
}

//=======================================================================
//function : Length
//purpose  : 2d with parameters
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& C,
					  const Standard_Real U1,
					  const Standard_Real U2) 
{
  CPnts_MyGaussFunction FG;
//POP pout WNT
  CPnts_RealFunction rf = f2d;
  FG.Init(rf,(Standard_Address)&C);
//  FG.Init(f2d,(Standard_Address)&C);
  math_GaussSingleIntegration TheLength(FG, U1, U2, order(C));
  if (!TheLength.IsDone()) {
    throw Standard_ConstructionError();
  }
  return Abs(TheLength.Value());
}

//=======================================================================
//function : Length
//purpose  : 3d with parameters and tolerance
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor3d_Curve& C,
					  const Standard_Real U1,
					  const Standard_Real U2,
					  const Standard_Real Tol) 
{
  CPnts_MyGaussFunction FG;
//POP pout WNT
  CPnts_RealFunction rf = f3d;
  FG.Init(rf,(Standard_Address)&C);
//  FG.Init(f3d,(Standard_Address)&C);
  math_GaussSingleIntegration TheLength(FG, U1, U2, order(C), Tol);
  if (!TheLength.IsDone()) {
    throw Standard_ConstructionError();
  }
  return Abs(TheLength.Value());
}

//=======================================================================
//function : Length
//purpose  : 2d with parameters and tolerance
//=======================================================================

Standard_Real CPnts_AbscissaPoint::Length(const Adaptor2d_Curve2d& C,
					  const Standard_Real U1,
					  const Standard_Real U2,
					  const Standard_Real Tol) 
{
  CPnts_MyGaussFunction FG;
//POP pout WNT
  CPnts_RealFunction rf = f2d;
  FG.Init(rf,(Standard_Address)&C);
//  FG.Init(f2d,(Standard_Address)&C);
  math_GaussSingleIntegration TheLength(FG, U1, U2, order(C), Tol);
  if (!TheLength.IsDone()) {
    throw Standard_ConstructionError();
  }
  return Abs(TheLength.Value());
}

//=======================================================================
//function : CPnts_AbscissaPoint
//purpose  : 
//=======================================================================

CPnts_AbscissaPoint::CPnts_AbscissaPoint() : myDone(Standard_False)
{
}

//=======================================================================
//function : CPnts_AbscissaPoint
//purpose  : 
//=======================================================================

CPnts_AbscissaPoint::CPnts_AbscissaPoint(const Adaptor3d_Curve& C,
					 const Standard_Real   Abscissa,
					 const Standard_Real   U0,
					 const Standard_Real   Resolution)
{
//  Init(C);
  Init(C, Resolution); //rbv's modification
//
  Perform(Abscissa, U0, Resolution);
}

//=======================================================================
//function : CPnts_AbscissaPoint
//purpose  : 
//=======================================================================

CPnts_AbscissaPoint::CPnts_AbscissaPoint(const Adaptor2d_Curve2d& C,
					 const Standard_Real   Abscissa,
					 const Standard_Real   U0,
					 const Standard_Real   Resolution)
{
  Init(C);
  Perform(Abscissa, U0, Resolution);
}


//=======================================================================
//function : CPnts_AbscissaPoint
//purpose  : 
//=======================================================================

CPnts_AbscissaPoint::CPnts_AbscissaPoint(const Adaptor3d_Curve& C,
					 const Standard_Real   Abscissa,
					 const Standard_Real   U0,
					 const Standard_Real   Ui,
					 const Standard_Real   Resolution)
{
  Init(C);
  Perform(Abscissa, U0, Ui, Resolution);
}

//=======================================================================
//function : CPnts_AbscissaPoint
//purpose  : 
//=======================================================================

CPnts_AbscissaPoint::CPnts_AbscissaPoint(const Adaptor2d_Curve2d& C,
					 const Standard_Real   Abscissa,
					 const Standard_Real   U0,
					 const Standard_Real   Ui,
					 const Standard_Real   Resolution)
{
  Init(C);
  Perform(Abscissa, U0, Ui, Resolution);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor3d_Curve& C)
{
  Init(C,C.FirstParameter(),C.LastParameter());
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor2d_Curve2d& C)
{
  Init(C,C.FirstParameter(),C.LastParameter());
}

//=======================================================================
//function : Init
//purpose  : introduced by rbv for curvilinear parametrization
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor3d_Curve& C, const Standard_Real Tol)
{
  Init(C,C.FirstParameter(),C.LastParameter(), Tol);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor2d_Curve2d& C, const Standard_Real Tol)
{
  Init(C,C.FirstParameter(),C.LastParameter(), Tol);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor3d_Curve& C,
			       const Standard_Real  U1,
			       const Standard_Real  U2)
{
//POP pout WNT
  CPnts_RealFunction rf = f3d;
  myF.Init(rf,(Standard_Address)&C,order(C));
//  myF.Init(f3d,(Standard_Address)&C,order(C));
  myL = CPnts_AbscissaPoint::Length(C, U1, U2);
  myUMin = Min(U1, U2);
  myUMax = Max(U1, U2);
  Standard_Real DU = myUMax - myUMin;
  myUMin = myUMin - DU;
  myUMax = myUMax + DU;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor2d_Curve2d& C,
			       const Standard_Real    U1,
			       const Standard_Real    U2)
{
//POP pout WNT
  CPnts_RealFunction rf = f2d;
  myF.Init(rf,(Standard_Address)&C,order(C));
//  myF.Init(f2d,(Standard_Address)&C,order(C));
  myL = CPnts_AbscissaPoint::Length(C, U1, U2);
  myUMin = Min(U1, U2);
  myUMax = Max(U1, U2);
  Standard_Real DU = myUMax - myUMin;
  myUMin = myUMin - DU;
  myUMax = myUMax + DU;
}


//=======================================================================
//function : Init
//purpose  : introduced by rbv for curvilinear parametrization
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor3d_Curve& C,
			       const Standard_Real  U1,
			       const Standard_Real  U2,
			       const Standard_Real  Tol)
{
//POP pout WNT
  CPnts_RealFunction rf = f3d;
  myF.Init(rf,(Standard_Address)&C,order(C));
//  myF.Init(f3d,(Standard_Address)&C,order(C));
  myL = CPnts_AbscissaPoint::Length(C, U1, U2, Tol);
  myUMin = Min(U1, U2);
  myUMax = Max(U1, U2);
  Standard_Real DU = myUMax - myUMin;
  myUMin = myUMin - DU;
  myUMax = myUMax + DU;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Init(const Adaptor2d_Curve2d& C,
			       const Standard_Real    U1,
			       const Standard_Real    U2,
			       const Standard_Real    Tol)
{
//POP pout WNT
  CPnts_RealFunction rf = f2d;
  myF.Init(rf,(Standard_Address)&C,order(C));
//  myF.Init(f2d,(Standard_Address)&C,order(C));
  myL = CPnts_AbscissaPoint::Length(C, U1, U2, Tol);
  myUMin = Min(U1, U2);
  myUMax = Max(U1, U2);
  Standard_Real DU = myUMax - myUMin;
  myUMin = myUMin - DU;
  myUMax = myUMax + DU;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Perform(const Standard_Real   Abscissa,
				  const Standard_Real   U0,
				  const Standard_Real   Resolution) 
{
  if (myL < Precision::Confusion()) {
    //
    //  leave less violently : it is expected that 
    //  the increment of the level of myParam will not be great
    //
    myDone = Standard_True ;
    myParam = U0 ;
  
  }
  else {
    Standard_Real Ui = U0 + (Abscissa / myL) * (myUMax - myUMin) / 3.;
    // exercice : why 3 ?
    Perform(Abscissa,U0,Ui,Resolution);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::Perform(const Standard_Real   Abscissa,
				  const Standard_Real   U0,
				  const Standard_Real   Ui,
				  const Standard_Real   Resolution) 
{
  if (myL < Precision::Confusion()) {
    //
    //  leave less violently :
    //
    myDone = Standard_True ;
    myParam = U0 ;
  }
  else {
    myDone = Standard_False;
    myF.Init(U0, Abscissa);

    math_FunctionRoot Solution(myF, Ui, Resolution, myUMin, myUMax);
    
// Temporarily suspend the validity test of the solution
// it is necessary to make a tolreached as soon as one will make a cdl
// lbo 21/03/97
//    if (Solution.IsDone()) {
//      Standard_Real D;
//      myF.Derivative(Solution.Root(),D);
//      if (Abs(Solution.Value()) < Resolution * D) {
//	myDone = Standard_True;
//	myParam = Solution.Root();
//      }
//    }
    if (Solution.IsDone()) {
      myDone = Standard_True;
      myParam = Solution.Root();
    }
  }
}

//=======================================================================
//function : AdvPerform
//purpose  : 
//=======================================================================

void CPnts_AbscissaPoint::AdvPerform(const Standard_Real   Abscissa,
				  const Standard_Real   U0,
				  const Standard_Real   Ui,
				  const Standard_Real   Resolution) 
{
  if (myL < Precision::Confusion()) {
    //
    //  leave less violently :
    //
    myDone = Standard_True ;
    myParam = U0 ;
  }
  else {
    myDone = Standard_False;
//    myF.Init(U0, Abscissa);
    myF.Init(U0, Abscissa, Resolution/10); // rbv's modification

    math_FunctionRoot Solution(myF, Ui, Resolution, myUMin, myUMax);
    
// Temporarily suspend the validity test of the solution
// it is necessary to make a tolreached as soon as one will make a cdl
// lbo 21/03/97
//    if (Solution.IsDone()) {
//      Standard_Real D;
//      myF.Derivative(Solution.Root(),D);
//      if (Abs(Solution.Value()) < Resolution * D) {
//	myDone = Standard_True;
//	myParam = Solution.Root();
//      }
//    }
    if (Solution.IsDone()) {
      myDone = Standard_True;
      myParam = Solution.Root();
    }
  }
}
