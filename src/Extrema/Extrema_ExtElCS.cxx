// File Extrema_ExtElCS.cxx

//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134

#include <Extrema_ExtElCS.ixx>
#include <Extrema_ExtPElS.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_InfiniteSolutions.hxx>
#include <Precision.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>
#include <gp_Vec.hxx>


Extrema_ExtElCS::Extrema_ExtElCS() 
{
  myDone = Standard_False;
  myIsPar = Standard_False;
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
			      const gp_Pln& S)
{
  myDone = Standard_True;
  myIsPar = Standard_False;

  if (C.Direction().IsNormal(S.Axis().Direction(), 
			     Precision::Angular())) {
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S.SquareDistance(C));
    myIsPar = Standard_True;
  }
  else {
    myNbExt = 0;
  }
  
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Cylinder& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
			      const gp_Cylinder& S)
{
  myDone = Standard_False;
  myNbExt = 0;
  myIsPar = Standard_False;

  gp_Ax3 Pos = S.Position();
#ifdef DEB
  gp_Pnt O = Pos.Location();
#else
  Pos.Location();
#endif
  Standard_Real radius = S.Radius();
  Extrema_ExtElC Extrem(gp_Lin(Pos.Axis()), C, Precision::Angular());
  if (Extrem.IsParallel()) {
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    Standard_Real aDist = sqrt (Extrem.SquareDistance(1)) - radius;
    mySqDist->SetValue(1, aDist * aDist);
    myDone = Standard_True;
    myIsPar = Standard_True;
  }
  else {
    Standard_Integer i;
    if (Extrem.IsDone()) {
      Extrema_POnCurv myPOnC1, myPOnC2;
      Extrem.Points(1, myPOnC1, myPOnC2);
      gp_Pnt PC = myPOnC2.Value();

      if ((gp_Lin(Pos.Axis())).Contains(PC, Precision::Confusion())) {
	gp_Dir D = C.Direction();
	gp_Vec Dp(-D.Dot(Pos.YDirection()), D.Dot(Pos.XDirection()), 0.0);
	Standard_Real U, V;
	gp_Pnt P1(PC.Translated(radius*Dp));
	gp_Pnt P2(PC.Translated(-radius*Dp));
	
	myNbExt = 2;
	mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
	myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
	myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);
	ElSLib::CylinderParameters(Pos, radius, P1, U, V);
	Extrema_POnSurf P1S(U, V, P1);
	ElSLib::CylinderParameters(Pos, radius, P2, U, V);
	Extrema_POnSurf P2S(U, V, P2);
	mySqDist->SetValue(1, PC.SquareDistance(P1));
	mySqDist->SetValue(2, PC.SquareDistance(P2));
	myPoint1->SetValue(1, myPOnC2);
	myPoint1->SetValue(2, myPOnC2);
	myPoint2->SetValue(1, P1S);
	myPoint2->SetValue(2, P2S);
      }
      else {
	Extrema_ExtPElS ExPS(myPOnC2.Value(), S, Precision::Confusion());
	if (ExPS.IsDone()) {
	  myNbExt = ExPS.NbExt();
	  mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
	  myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
	  myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);
	  for (i = 1; i <= myNbExt; i++) {
	    myPoint1->SetValue(i, myPOnC2);
	    myPoint2->SetValue(i, ExPS.Point(i));
	    mySqDist->SetValue(i,(myPOnC2.Value()).SquareDistance(ExPS.Point(i).Value()));
	  }
	}
      }
      myDone = Standard_True;
    }
  }

}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Cone& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Lin& C,
//			      const gp_Cone& S)
void Extrema_ExtElCS::Perform(const gp_Lin& ,
			      const gp_Cone& )
{
  Standard_NotImplemented::Raise();

}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Sphere& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
			      const gp_Sphere& S)
{
  myDone = Standard_False;
  myNbExt = 0;
  myIsPar = Standard_False;

  gp_Pnt O = S.Location();

  Extrema_ExtPElC Extrem(O, C, Precision::Angular(), RealFirst(), RealLast());

  Standard_Integer i;
  if (Extrem.IsDone()) {
    Extrema_POnCurv myPOnC1 =  Extrem.Point(1);
    Extrema_ExtPElS ExPS(myPOnC1.Value(), S, Precision::Confusion());
    if (ExPS.IsDone()) {
      myNbExt = ExPS.NbExt();
      mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
      myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);
      for (i = 1; i <= myNbExt; i++) {
	myPoint1->SetValue(i, myPOnC1);
	myPoint2->SetValue(i, ExPS.Point(i));
	mySqDist->SetValue(i,(myPOnC1.Value()).SquareDistance(ExPS.Point(i).Value()));
	myDone = Standard_True;
      }
    }
  }
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Torus& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Lin& C,
//			      const gp_Torus& S)
void Extrema_ExtElCS::Perform(const gp_Lin& ,
			      const gp_Torus& )
{
  Standard_NotImplemented::Raise();

}


//        Circle-?

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			      const gp_Pln& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			      const gp_Pln& )
{
  Standard_NotImplemented::Raise();

}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Cylinder& S)
{
  Perform(C, S);
}



//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134 Begin
// Implementation of the method.
void Extrema_ExtElCS::Perform(const gp_Circ& C,
			      const gp_Cylinder& S)
{
  myDone  = Standard_False;
  myIsPar = Standard_False;
  myNbExt = 0;

  // Get an axis line of the cylinder.
  gp_Lin anAxis(S.Axis());

  // Compute extrema between the circle and the line.
  Extrema_ExtElC anExtC(anAxis, C, 0.);

  if (anExtC.IsDone()) {
    if (anExtC.IsParallel()) {
      myIsPar =     Standard_True;
      mySqDist = new TColStd_HArray1OfReal(1, 1);
      Standard_Real aDist = sqrt (anExtC.SquareDistance(1)) - S.Radius();
      mySqDist->SetValue(1, aDist * aDist);
    } else {
      Standard_Integer aNbExt   = anExtC.NbExt();
      gp_Pnt           aCenter  = C.Location();
      Standard_Integer i;
      Standard_Integer aCurI    = 1;
      Standard_Real    aTolConf = Precision::Confusion();
      Standard_Real    aCylRad  = S.Radius();

      // Compute the extremas.
      myNbExt  =     2*aNbExt;
      mySqDist  = new TColStd_HArray1OfReal(1, myNbExt);
      myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);

      for (i = 1; i <= aNbExt; i++) {
	Extrema_POnCurv aPOnAxis;
	Extrema_POnCurv aPOnCirc;
	Standard_Real   aSqDist = anExtC.SquareDistance(i);
	Standard_Real   aDist = sqrt (aSqDist);

	anExtC.Points(i, aPOnAxis, aPOnCirc);

	if (aSqDist <= (aTolConf * aTolConf) || aCenter.IsEqual(aPOnAxis.Value(), aTolConf)) {
	  myNbExt -= 2;
	  continue;
	}

	gp_Dir           aDir(aPOnAxis.Value().XYZ().
			      Subtracted(aPOnCirc.Value().XYZ()));
	Standard_Real    aShift[2] = { aDist + aCylRad, aDist - aCylRad };
	Standard_Integer j;

	for (j = 0; j < 2; j++) {
	  gp_Vec aVec(aDir);
	  gp_Pnt aPntOnCyl;

	  aVec.Multiply(aShift[j]);
	  aPntOnCyl = aPOnCirc.Value().Translated(aVec);

	  Standard_Real aU;
	  Standard_Real aV;

	  ElSLib::Parameters(S, aPntOnCyl, aU, aV);

	  Extrema_POnSurf aPOnSurf(aU, aV, aPntOnCyl);

	  myPoint1->SetValue(aCurI, aPOnCirc);
	  myPoint2->SetValue(aCurI, aPOnSurf);
	  mySqDist->SetValue(aCurI++, aShift[j] * aShift[j]);
	}
      }
    }

    myDone = Standard_True;
  }
}
//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134 End



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Cone& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			 const gp_Cone& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			 const gp_Cone& )
{
  Standard_NotImplemented::Raise();

}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Sphere& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			      const gp_Sphere& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			      const gp_Sphere& )
{
  Standard_NotImplemented::Raise();

}

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Torus& S)
{
  Perform(C, S);
}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			      const gp_Torus& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			      const gp_Torus& )
{
  Standard_NotImplemented::Raise();

}

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Hypr& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Hypr& C,
			      const gp_Pln& S)
{
  myDone = Standard_True;
  myIsPar = Standard_False;

  gp_Ax2 Pos = C.Position();
  gp_Dir NHypr = Pos.Direction();
  gp_Dir NPln = S.Axis().Direction();

  if (NHypr.IsParallel(NPln, Precision::Angular())) {

    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S.SquareDistance(C.Location()));
    myIsPar = Standard_True;

  }
  else {

    gp_Dir XDir = Pos.XDirection();
    gp_Dir YDir = Pos.YDirection();

    Standard_Real A = C.MinorRadius()*(NPln.Dot(YDir)); 
    Standard_Real B = C.MajorRadius()*(NPln.Dot(XDir)); 

    if(Abs(B) > Abs(A)) {
      Standard_Real T = -0.5 * Log((A+B)/(B-A));
      gp_Pnt Ph = ElCLib::HyperbolaValue(T, Pos, C.MajorRadius(), C.MinorRadius());
      Extrema_POnCurv PC(T, Ph);
      myPoint1 = new Extrema_HArray1OfPOnCurv(1,1);
      myPoint1->SetValue(1, PC);

      mySqDist = new TColStd_HArray1OfReal(1, 1);
      mySqDist->SetValue(1, S.SquareDistance(Ph));

      Standard_Real U, V;
      ElSLib::PlaneParameters(S.Position(), Ph, U, V);
      gp_Pnt Pp = ElSLib::PlaneValue(U, V, S.Position());
      Extrema_POnSurf PS(U, V, Pp);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1,1);
      myPoint2->SetValue(1, PS);

      myNbExt = 1;
    }
    else {
      myNbExt = 0;
    }

  }
  
}


Standard_Boolean Extrema_ExtElCS::IsDone() const
{
  return myDone;
}


Standard_Integer Extrema_ExtElCS::NbExt() const
{
  if (myIsPar) StdFail_InfiniteSolutions::Raise();
  return myNbExt;
}

Standard_Real Extrema_ExtElCS::SquareDistance(const Standard_Integer N) const
{
  if (myIsPar && N != 1) StdFail_InfiniteSolutions::Raise();
  return mySqDist->Value(N);
}


void Extrema_ExtElCS::Points(const Standard_Integer N,
			     Extrema_POnCurv& P1,
			     Extrema_POnSurf& P2) const
{
  if (myIsPar) StdFail_InfiniteSolutions::Raise();
  P1 = myPoint1->Value(N);
  P2 = myPoint2->Value(N);
}


Standard_Boolean Extrema_ExtElCS::IsParallel() const
{
  return myIsPar;
}
