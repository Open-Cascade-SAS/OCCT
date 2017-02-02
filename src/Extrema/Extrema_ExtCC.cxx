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

// Modified by MPS (june 96) : correction du trap dans le cas droite/Bezier 
// Modified by MPS (mai 97) : PRO 7598 
//                            tri des solutions pour eviter de rendre plusieurs
//                            fois la meme solution 

#include <Adaptor3d_Curve.hxx>
#include <ElCLib.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_ECC.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TColStd_SequenceOfReal.hxx>

//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================
Extrema_ExtCC::Extrema_ExtCC (const Standard_Real TolC1,
                              const Standard_Real TolC2)
: myIsFindSingleSolution(Standard_False),
  myDone (Standard_False)
{
  myC[0] = 0; myC[1] = 0;
  myTol[0] = TolC1; myTol[1] = TolC2;
}

//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================

Extrema_ExtCC::Extrema_ExtCC(const Adaptor3d_Curve& C1,
                             const Adaptor3d_Curve& C2,
                             const Standard_Real      U1,
                             const Standard_Real      U2,
                             const Standard_Real      V1,
                             const Standard_Real      V2,
                             const Standard_Real      TolC1,
                             const Standard_Real      TolC2)
: myIsFindSingleSolution(Standard_False),
  myECC(C1, C2, U1, U2, V1, V2),
  myDone (Standard_False)
{
  SetCurve (1, C1, U1, U2);
  SetCurve (2, C2, V1, V2);
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  Perform();
}


//=======================================================================
//function : Extrema_ExtCC
//purpose  : 
//=======================================================================

Extrema_ExtCC::Extrema_ExtCC(const Adaptor3d_Curve& C1, 
                             const Adaptor3d_Curve& C2,
                             const Standard_Real      TolC1,
                             const Standard_Real      TolC2)
: myIsFindSingleSolution(Standard_False),
  myECC(C1, C2),
  myDone (Standard_False)
{
  SetCurve (1, C1, C1.FirstParameter(), C1.LastParameter());
  SetCurve (2, C2, C2.FirstParameter(), C2.LastParameter());
  SetTolerance (1, TolC1);
  SetTolerance (2, TolC2);
  Perform();
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetCurve()")
  Standard_Integer anInd = theRank - 1;
  myC[anInd] = (Standard_Address)&C;
}

//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetCurve (const Standard_Integer theRank, const Adaptor3d_Curve& C,
                               const Standard_Real Uinf, const Standard_Real Usup)
{
  SetCurve (theRank, C);
  SetRange (theRank, Uinf, Usup);
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetRange (const Standard_Integer theRank, 
                               const Standard_Real Uinf, const Standard_Real Usup)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetRange()")
  Standard_Integer anInd = theRank - 1;
  myInf[anInd] = Uinf;
  mySup[anInd] = Usup;
}

//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================

void Extrema_ExtCC::SetTolerance (const Standard_Integer theRank, const Standard_Real theTol)
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > 2, "Extrema_ExtCC::SetTolerance()")
  Standard_Integer anInd = theRank - 1;
  myTol[anInd] = theTol;
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Perform()
{  
  Standard_NullObject_Raise_if (!myC[0] || !myC[1], "Extrema_ExtCC::Perform()")
  myECC.SetParams(*((Adaptor3d_Curve*)myC[0]), 
                  *((Adaptor3d_Curve*)myC[1]), myInf[0], mySup[0], myInf[1], mySup[1]);
  myECC.SetTolerance(Min(myTol[0], myTol[1]));
  myECC.SetSingleSolutionFlag(GetSingleSolutionFlag());
  myDone = Standard_False;
  mypoints.Clear();
  mySqDist.Clear();
  myIsPar = Standard_False;

  GeomAbs_CurveType type1 = (*((Adaptor3d_Curve*)myC[0])).GetType();
  GeomAbs_CurveType type2 = (*((Adaptor3d_Curve*)myC[1])).GetType();
  Standard_Real U11, U12, U21, U22, Tol = Min(myTol[0], myTol[1]);
  mynbext = 0;
  inverse = Standard_False;

  U11 = myInf[0];
  U12 = mySup[0];
  U21 = myInf[1];
  U22 = mySup[1];

  if (!Precision::IsInfinite(U11)) P1f = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]),  U11); 
  if (!Precision::IsInfinite(U12)) P1l = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]),  U12);
  if (!Precision::IsInfinite(U21)) P2f = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), U21);
  if (!Precision::IsInfinite(U22)) P2l = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), U22);
  

  if (Precision::IsInfinite(U11) || Precision::IsInfinite(U21)) mydist11 = RealLast();
  else mydist11 = P1f.SquareDistance(P2f);
  if (Precision::IsInfinite(U11) || Precision::IsInfinite(U22)) mydist12 = RealLast();
  else mydist12 = P1f.SquareDistance(P2l);
  if (Precision::IsInfinite(U12) || Precision::IsInfinite(U21)) mydist21 = RealLast();
  else mydist21 = P1l.SquareDistance(P2f);
  if (Precision::IsInfinite(U12) || Precision::IsInfinite(U22)) mydist22 = RealLast();
  else mydist22 = P1l.SquareDistance(P2l);

  //Depending on the types of curves, the algorithm is chosen:
  //- _ExtElC, when one of the curves is a line and the other is elementary,
  //   or there are two circles;
  //- _GenExtCC, in all other cases
  if ( (type1 == GeomAbs_Line && type2 <= GeomAbs_Parabola) ||
       (type2 == GeomAbs_Line && type1 <= GeomAbs_Parabola) ) {
    //analytical case - one curve is always a line
    Standard_Integer anInd1 = 0, anInd2 = 1;
    GeomAbs_CurveType aType2 = type2;
    inverse = (type1 > type2);
    if (inverse) {
      //algorithm uses inverse order of arguments
      anInd1 = 1;
      anInd2 = 0;
      aType2 = type1;
    }
    switch (aType2) {
    case GeomAbs_Line: {
      Extrema_ExtElC Xtrem((*((Adaptor3d_Curve*)myC[anInd1])).Line(), (*((Adaptor3d_Curve*)myC[anInd2])).Line(), Tol);
      Results(Xtrem, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Circle: {
      Extrema_ExtElC Xtrem((*((Adaptor3d_Curve*)myC[anInd1])).Line(), (*((Adaptor3d_Curve*)myC[anInd2])).Circle(), Tol);
      Results(Xtrem, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Ellipse: {
      Extrema_ExtElC Xtrem((*((Adaptor3d_Curve*)myC[anInd1])).Line(), (*((Adaptor3d_Curve*)myC[anInd2])).Ellipse());
      Results(Xtrem, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Hyperbola: {
      Extrema_ExtElC Xtrem((*((Adaptor3d_Curve*)myC[anInd1])).Line(), (*((Adaptor3d_Curve*)myC[anInd2])).Hyperbola());
      Results(Xtrem, U11, U12, U21, U22);
      break;
    }
    case GeomAbs_Parabola: {
      Extrema_ExtElC Xtrem((*((Adaptor3d_Curve*)myC[anInd1])).Line(), (*((Adaptor3d_Curve*)myC[anInd2])).Parabola());
      Results(Xtrem, U11, U12, U21, U22);
      break;
    }
    default: break;
    }
  } else if (type1 == GeomAbs_Circle && type2 == GeomAbs_Circle) {
    //analytical case - two circles
    Standard_Boolean bIsDone;
    Extrema_ExtElC CCXtrem ((*((Adaptor3d_Curve*)myC[0])).Circle(), (*((Adaptor3d_Curve*)myC[1])).Circle());
    bIsDone = CCXtrem.IsDone();
    if(bIsDone) {
      Results(CCXtrem, U11, U12, U21, U22);
    }
    else {
      myECC.Perform();
      Results(myECC, U11, U12, U21, U22);
    }
  } else {
    myECC.Perform();
    Results(myECC, U11, U12, U21, U22);
  }
}


//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_ExtCC::IsDone() const
{
  return myDone;
}

//=======================================================================
//function : IsParallel
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_ExtCC::IsParallel() const
{
  return myIsPar;
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Extrema_ExtCC::SquareDistance(const Standard_Integer N) const 
{
  if(!myDone) throw StdFail_NotDone();
  if ((N <= 0) || (N > mynbext)) throw Standard_OutOfRange();
  return mySqDist.Value(N);
}


//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_ExtCC::NbExt() const
{
  if(!myDone) throw StdFail_NotDone();
  return mynbext;
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Points(const Standard_Integer N, 
			    Extrema_POnCurv& P1,
			    Extrema_POnCurv& P2) const
{
  if(!myDone) throw StdFail_NotDone();
  if ((N <= 0) || (N > mynbext)) throw Standard_OutOfRange();
  P1 = mypoints.Value(2*N-1);
  P2 = mypoints.Value(2*N);
}



//=======================================================================
//function : TrimmedDistances
//purpose  : 
//=======================================================================

void Extrema_ExtCC::TrimmedSquareDistances(Standard_Real& dist11,
				      Standard_Real& dist12,
				      Standard_Real& dist21,
				      Standard_Real& dist22,
				      gp_Pnt&        P11   ,
				      gp_Pnt&        P12   ,
				      gp_Pnt&        P21   ,
				      gp_Pnt&        P22    ) const {
					
  dist11 = mydist11;
  dist12 = mydist12;
  dist21 = mydist21;
  dist22 = mydist22;
  P11 = P1f;
  P12 = P1l;
  P21 = P2f;
  P22 = P2l;
}



//=======================================================================
//function : Results
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Results(const Extrema_ExtElC&  AlgExt,
			     const Standard_Real    Ut11,
			     const Standard_Real    Ut12,
			     const Standard_Real    Ut21,
			     const Standard_Real    Ut22)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv P1, P2;

  myDone = AlgExt.IsDone();
  if (myDone) {
    myIsPar = AlgExt.IsParallel();
    if (myIsPar) {
      GeomAbs_CurveType type = Extrema_CurveTool::GetType(*((Adaptor3d_Curve*)myC[0]));
      GeomAbs_CurveType type2 = Extrema_CurveTool::GetType(*((Adaptor3d_Curve*)myC[1]));
      // Parallel case is only for line-line, circle-circle and circle-line!!!
      // But really for trimmed curves extremas can not exist!
      Extrema_POnCurv dummypoint(0., gp_Pnt(0.,0.,0.));
      if(type != type2) {
	mySqDist.Append(AlgExt.SquareDistance(1));
	if(type == GeomAbs_Circle) {
	  gp_Pnt PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), Ut11);
	  P1.SetValues(Ut11, PonC1);
	  Extrema_ExtPElC ExtPLin(PonC1, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	  if(ExtPLin.IsDone()) {
	    mynbext = 1;
	    P2 = ExtPLin.Point(1);
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  else {
	    myIsPar = Standard_False;
	    mynbext = 0;
	    mypoints.Append(dummypoint);
	    mypoints.Append(dummypoint);
	  }
	}
	else {
	  gp_Pnt PonC2 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), Ut21);
	  P2.SetValues(Ut21, PonC2);
	  Extrema_ExtPElC ExtPLin(PonC2, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[0])), Precision::Confusion(), Ut11, Ut12);
	  if(ExtPLin.IsDone()) {
	    mynbext = 1;
	    P1 = ExtPLin.Point(1);
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  else {
	    myIsPar = Standard_False;
	    mynbext = 0;
	    mypoints.Append(dummypoint);
	    mypoints.Append(dummypoint);
	  }
	}
	return;
      }
	  
      if(type == GeomAbs_Line) {
	Standard_Boolean infinite = Precision::IsInfinite(Ut11) &&
	                            Precision::IsInfinite(Ut12) &&
			            Precision::IsInfinite(Ut21) &&
			            Precision::IsInfinite(Ut22);

	if(infinite) {
	  mynbext = 1;
	  mySqDist.Append(AlgExt.SquareDistance(1));
	  gp_Pnt PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), 0.); 
	  P1.SetValues(0., PonC1);
	  Extrema_ExtPElC ExtPLin(PonC1, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	  if(ExtPLin.IsDone()) {
	    P2 = ExtPLin.Point(1);
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  else {
	    myIsPar = Standard_False;
	    mypoints.Append(dummypoint);
	    mypoints.Append(dummypoint);
	  }
	}
	else {
	  Standard_Boolean finish = Standard_False;
	  if(!Precision::IsInfinite(Ut11)) {
	    gp_Pnt PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), Ut11);  
	    Extrema_ExtPElC ExtPLin(PonC1, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	    if(ExtPLin.IsDone() && ExtPLin.NbExt() > 0) {
	      mynbext = 1;
	      mySqDist.Append(AlgExt.SquareDistance(1));
	      P1.SetValues(Ut11, PonC1);
	      P2 = ExtPLin.Point(1);
	      mypoints.Append(P1);
	      mypoints.Append(P2);
	      finish = Standard_True;
	    }
	  }
	  if(!finish) {
	    if(!Precision::IsInfinite(Ut12)) {
	      gp_Pnt PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), Ut12);  
	      Extrema_ExtPElC ExtPLin(PonC1, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	      if(ExtPLin.IsDone() && ExtPLin.NbExt() > 0) {
		mynbext = 1;
		mySqDist.Append(AlgExt.SquareDistance(1));
		P1.SetValues(Ut12, PonC1);
		P2 = ExtPLin.Point(1);
		mypoints.Append(P1);
		mypoints.Append(P2);
		finish = Standard_True;
	      }
	    }
	  }
	  if(!finish) {
	    if(!Precision::IsInfinite(Ut21)) {
	      gp_Pnt PonC2 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), Ut21);  
	      Extrema_ExtPElC ExtPLin(PonC2, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[0])), Precision::Confusion(), Ut11, Ut12);
	      if(ExtPLin.IsDone() && ExtPLin.NbExt() > 0) {
		mynbext = 1;
		mySqDist.Append(AlgExt.SquareDistance(1));
		P2.SetValues(Ut21, PonC2);
		P1 = ExtPLin.Point(1);
		mypoints.Append(P1);
		mypoints.Append(P2);
		finish = Standard_True;
	      }
	    }
	  }
	  if(!finish) {
	    if(!Precision::IsInfinite(Ut22)) {
	      gp_Pnt PonC2 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), Ut22);  
	      Extrema_ExtPElC ExtPLin(PonC2, Extrema_CurveTool::Line(*((Adaptor3d_Curve*)myC[0])), Precision::Confusion(), Ut11, Ut12);
	      if(ExtPLin.IsDone() && ExtPLin.NbExt() > 0) {
		mynbext = 1;
		mySqDist.Append(AlgExt.SquareDistance(1));
		P2.SetValues(Ut22, PonC2);
		P1 = ExtPLin.Point(1);
		mypoints.Append(P1);
		mypoints.Append(P2);
		finish = Standard_True;
	      }
	    }
	  }
	  if(!finish) {
	    mynbext = 0;
	    myIsPar = Standard_False;
	    mySqDist.Append(AlgExt.SquareDistance(1));
	    mypoints.Append(dummypoint);
	    mypoints.Append(dummypoint);
	  }
	}
	  
      }
      else {
	Standard_Boolean finish = Standard_False;
	gp_Pnt PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), Ut11);  
	P1.SetValues(Ut11, PonC1);
	Extrema_ExtPElC ExtPCir(PonC1, Extrema_CurveTool::Circle(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	if(ExtPCir.IsDone() && ExtPCir.NbExt() > 0) {
	  for(i = 1; i <= ExtPCir.NbExt(); i++) {
	    mynbext++;
	    P2 = ExtPCir.Point(i);
	    mySqDist.Append(ExtPCir.SquareDistance(i));
	    mypoints.Append(P1);
	    mypoints.Append(P2);
	  }
	  if(mynbext == 2)  finish = Standard_True;
	}
	if(!finish) {
	  PonC1 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[0]), Ut12);  
	  ExtPCir.Perform(PonC1, Extrema_CurveTool::Circle(*((Adaptor3d_Curve*)myC[1])), Precision::Confusion(), Ut21, Ut22);
	  P1.SetValues(Ut12, PonC1);
	  if(ExtPCir.IsDone() && ExtPCir.NbExt() > 0) {
	    if(mynbext == 0) {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		mynbext++;
		P2 = ExtPCir.Point(i);
		mySqDist.Append(ExtPCir.SquareDistance(i));
		mypoints.Append(P1);
		mypoints.Append(P2);
	      }
	    }
	    else {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		Standard_Real dist = mySqDist(1);
		if(Abs(dist - ExtPCir.SquareDistance(i)) > Precision::Confusion()) {
		  mynbext++;
		  P2 = ExtPCir.Point(i);
		  mySqDist.Append(ExtPCir.SquareDistance(i));
		  mypoints.Append(P1);
		  mypoints.Append(P2);
		}
	      }
	    }
		
	    if(mynbext == 2)  finish = Standard_True;
	  }
	}  
	if(!finish) {
	  gp_Pnt PonC2 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), Ut21);  
	  ExtPCir.Perform(PonC2, Extrema_CurveTool::Circle(*((Adaptor3d_Curve*)myC[0])), Precision::Confusion(), Ut11, Ut12);
	  P2.SetValues(Ut21, PonC2);
	  if(ExtPCir.IsDone() && ExtPCir.NbExt() > 0) {
	    if(mynbext == 0) {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		mynbext++;
		P1 = ExtPCir.Point(i);
		mySqDist.Append(ExtPCir.SquareDistance(i));
		mypoints.Append(P1);
		mypoints.Append(P2);
	      }
	    }
	    else {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		Standard_Real dist = mySqDist(1);
		if(Abs(dist - ExtPCir.SquareDistance(i)) > Precision::Confusion()) {
		  mynbext++;
		  P1 = ExtPCir.Point(i);
		  mySqDist.Append(ExtPCir.SquareDistance(i));
		  mypoints.Append(P1);
		  mypoints.Append(P2);
		}
	      }
	    }
		
	    if(mynbext == 2)  finish = Standard_True;
	  }
	}  
	if(!finish) {
	  gp_Pnt PonC2 = Extrema_CurveTool::Value(*((Adaptor3d_Curve*)myC[1]), Ut22);  
	  ExtPCir.Perform(PonC2, Extrema_CurveTool::Circle(*((Adaptor3d_Curve*)myC[0])), Precision::Confusion(), Ut11, Ut12);
	  P2.SetValues(Ut22, PonC2);
	  if(ExtPCir.IsDone() && ExtPCir.NbExt() > 0) {
	    if(mynbext == 0) {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		mynbext++;
		P1 = ExtPCir.Point(i);
		mySqDist.Append(ExtPCir.SquareDistance(i));
		mypoints.Append(P1);
		mypoints.Append(P2);
	      }
	    }
	    else {
	      for(i = 1; i <= ExtPCir.NbExt(); i++) {
		Standard_Real dist = mySqDist(1);
		if(Abs(dist - ExtPCir.SquareDistance(i)) > Precision::Confusion()) {
		  mynbext++;
		  P1 = ExtPCir.Point(i);
		  mySqDist.Append(ExtPCir.SquareDistance(i));
		  mypoints.Append(P1);
		  mypoints.Append(P2);
		}
	      }
	    }
		
	    if(mynbext == 2)  finish = Standard_True;
	  }
	}  
	if(mynbext == 0) {
	  myIsPar = Standard_False;
	  mySqDist.Append(AlgExt.SquareDistance(1));
	  mypoints.Append(dummypoint);
	  mypoints.Append(dummypoint);
	  mySqDist.Append(AlgExt.SquareDistance(2));
	  mypoints.Append(dummypoint);
	  mypoints.Append(dummypoint);
	}
      }	
    }
    else {
      NbExt = AlgExt.NbExt();
      for (i = 1; i <= NbExt; i++) {
	// Verification de la validite des parametres
	AlgExt.Points(i, P1, P2);
	if (!inverse) {
	  U = P1.Parameter();
	  U2 = P2.Parameter();
	}
	else {
	  U2 = P1.Parameter();
	  U = P2.Parameter();
	}

	if (Extrema_CurveTool::IsPeriodic(*((Adaptor3d_Curve*)myC[0]))) {
	  U = ElCLib::InPeriod(U, Ut11, Ut11+Extrema_CurveTool::Period(*((Adaptor3d_Curve*)myC[0])));
	}
	if (Extrema_CurveTool::IsPeriodic(*((Adaptor3d_Curve*)myC[1]))) {
	  U2 = ElCLib::InPeriod(U2, Ut21, Ut21+Extrema_CurveTool::Period(*((Adaptor3d_Curve*)myC[1])));
	}

	if ((U  >= Ut11 - RealEpsilon())  && 
	    (U  <= Ut12 + RealEpsilon())  &&
	    (U2 >= Ut21 - RealEpsilon())  &&
	    (U2 <= Ut22 + RealEpsilon())) {
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
  }

}


//=======================================================================
//function : Results
//purpose  : 
//=======================================================================

void Extrema_ExtCC::Results(const Extrema_ECC&   AlgExt,
			     const Standard_Real  Ut11,
			     const Standard_Real  Ut12,
			     const Standard_Real  Ut21,
			     const Standard_Real  Ut22)
{
  Standard_Integer i, NbExt;
  Standard_Real Val, U, U2;
  Extrema_POnCurv P1, P2;

  myDone = AlgExt.IsDone();
  if (myDone)
  {
    myIsPar = AlgExt.IsParallel();
    NbExt = AlgExt.NbExt();
    for (i = 1; i <= NbExt; i++)
    {
      AlgExt.Points(i, P1, P2);
      U = P1.Parameter();
      U2 = P2.Parameter();

      // Check points to be into param space.
      if (Extrema_CurveTool::IsPeriodic(*((Adaptor3d_Curve*)myC[0])))
      {
        U = ElCLib::InPeriod(U, Ut11, Ut11+Extrema_CurveTool::Period(*((Adaptor3d_Curve*)myC[0])));
      }
      if (Extrema_CurveTool::IsPeriodic(*((Adaptor3d_Curve*)myC[1])))
      {
        U2 = ElCLib::InPeriod(U2, Ut21, Ut21+Extrema_CurveTool::Period(*((Adaptor3d_Curve*)myC[1])));
      }

      if ((U  >= Ut11 - RealEpsilon())  &&
          (U  <= Ut12 + RealEpsilon())  &&
          (U2 >= Ut21 - RealEpsilon())  &&
          (U2 <= Ut22 + RealEpsilon())   )
      {
        mynbext++;
        Val = AlgExt.SquareDistance(i);
        mySqDist.Append(Val);
        P1.SetValues(U, P1.Value());
        P2.SetValues(U2, P2.Value());
        mypoints.Append(P1);
        mypoints.Append(P2);
      }
    }
  }
}

//=======================================================================
//function : SetSingleSolutionFlag
//purpose  : 
//=======================================================================
void Extrema_ExtCC::SetSingleSolutionFlag(const Standard_Boolean theFlag)
{
  myIsFindSingleSolution = theFlag;
}

//=======================================================================
//function : GetSingleSolutionFlag
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_ExtCC::GetSingleSolutionFlag() const
{
  return myIsFindSingleSolution;
}
