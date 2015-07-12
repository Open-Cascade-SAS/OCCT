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

//-------------------------------------------------------------------
//                Algorithm concerns the constant arrow
// cases processed : parameterized curve 
//               the curve should be C2
// provide a max arrow
// algorithm of parameterized curve:
//   calculation of the step of advancement is 
//             du = sqrt(8*fleche*||P'(u)||/||P'(u)^P''(u)||
//   calculate each point such as u+Du
//   check if the arrow is actually taken into account, if yes, continue
//   otherwise correct the step
//   si du cannot be calculated (null curvature, singularity on the curve) 
//   take a constant step to reach the last point or to go past it
//   The last point is readjusted by the following criteria:
//     if the last calculated parameter is <2*resolution, reframe the last point found
//     between itself and the previous point and add the end point 
//     (avoid a concentration at the end)
//     otherwise if the distance (last calculated point, end point)<arrow, 
//     replace the last calculated point by the end point
//     otherwise calculate max arrow between the last but one calculated point
//     and the end point; if this arrow is greater than the arrow
//     replace the last point by this one and the end point
//    CONTROLS OF ARROW AND THE LAST POINT ARE DONE ONLY IF withControl=true
//   each iteration calculates at maximum 3 points
//-------------------------------------------------------------------------

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <CPnts_UniformDeflection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

static inline void D03d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& P)
{
  ((Adaptor3d_Curve*)C)->D0(U,P);
}

static  void D02d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& PP)
{
  gp_Pnt2d P;
  ((Adaptor2d_Curve2d*)C)->D0(U,P);
  PP.SetCoord(P.X(),P.Y(),0.);
}

static inline void D23d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{
  ((Adaptor3d_Curve*)C)->D2(U,P,V1,V2);
}

static  void D22d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& PP, gp_Vec& VV1, gp_Vec& VV2)
{
  gp_Pnt2d P;
  gp_Vec2d V1,V2;
  ((Adaptor2d_Curve2d*)C)->D2(U,P,V1,V2);
  PP.SetCoord(P.X(),P.Y(),0.);
  VV1.SetCoord(V1.X(),V1.Y(),0.);
  VV2.SetCoord(V2.X(),V2.Y(),0.);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void CPnts_UniformDeflection::Perform()
{
  gp_Pnt P, P1, P2;
//  gp_Vec V1, V2, VV1, VV2, VV;
  gp_Vec V1, V2, VV;
  Standard_Real Un1; 
  Standard_Real NormD1, NormD2;

  myIPoint   = -1;
  myNbPoints = -1;
  
  while ( (myNbPoints<2) && (!myFinish) ) {
    
    myNbPoints = myNbPoints + 1; 
    myParams[myNbPoints] = myFirstParam;

    if (my3d)
      D23d(myCurve, myFirstParam, myPoints[myNbPoints], V1, V2);
    else
      D22d(myCurve, myFirstParam, myPoints[myNbPoints], V1, V2);
    P = myPoints[myNbPoints] ;
    NormD1 = V1.Magnitude();
    if (NormD1 < myTolCur || V2.Magnitude() < myTolCur) {   
      // singularity on the tangent or null curvature
      myDu = Min(myDwmax, 1.5 * myDu);
    }
    else { 
      NormD2 = V2.CrossMagnitude(V1);
      if (NormD2 / NormD1 < myDeflection) {  // collinearity of derivatives
	myDu = Min(myDwmax, 1.5 * myDu);            
      }
      else {
	myDu = Sqrt(8.* myDeflection * NormD1 / NormD2 );
	myDu = Min(Max(myDu, myTolCur), myDwmax);             
      }
    }
    
    // check if the arrow is observed if WithControl
    
    if (myControl) {
      myDu = Min(myDu, myLastParam-myFirstParam);
      if (my3d) {
	
	D03d(myCurve, myFirstParam + myDu,P);
	D03d(myCurve, myFirstParam + (myDu / 2.0),P1);
      }
      else {
	
	D02d(myCurve, myFirstParam + myDu,P);
	D02d(myCurve, myFirstParam + (myDu / 2.0),P1);
      }
      V1= gp_Vec(myPoints[myNbPoints], P);
      NormD1 = V1.Magnitude(); 
      if (NormD1 >= myDeflection) {
	V2 = gp_Vec(myPoints[myNbPoints], P1);
	NormD2 = V2.CrossMagnitude(V1) / NormD1;
	
	// passing of arrow starting from which the redivision is done is arbitrary
	// probably it will be necessary to readjust it (differenciate the first point 
	// from the others) this test does not work on the points of inflexion
	
	if (NormD2 > myDeflection / 5.0) {
	  NormD2 = Max(NormD2, 1.1 * myDeflection);
	  myDu = myDu * Sqrt(myDeflection / NormD2);
	  myDu = Min(Max(myDu, myTolCur), myDwmax);             
	}
      }
    }
    myFirstParam = myFirstParam + myDu;
    myFinish = (myLastParam - myFirstParam < myTolCur) || (myDu == 0.);
  }
  if (myFinish) {
    // the last point is corrected if control
    if (myControl && (myNbPoints == 1) ) {
      Un1 = myParams[0];
      if (myLastParam - Un1 < 0.33*(myLastParam-myFirstParam)) {
	myFirstParam = (myLastParam + Un1) / 2.0;
	myParams[0]= myFirstParam;
	myParams[1]= myLastParam;
	if (my3d) {
	  D03d(myCurve, myParams[0], myPoints[0]);
	  D03d(myCurve, myParams[1], myPoints[1]);
	}
	else {
	  D02d(myCurve, myParams[0], myPoints[0]);
          D02d(myCurve, myParams[1], myPoints[1]);
	}
      } 
      else {
	if (my3d) {
	  D23d(myCurve, myLastParam, P1, V1, V2);
	}
	else {
	  D22d(myCurve, myLastParam, P1, V1, V2);
	}
        P = myPoints[0] ;
	VV = gp_Vec(P1, P);
	NormD1 = VV.Magnitude();
	if ( NormD1 < myDeflection) {
	  myParams[1]= myLastParam;
          myPoints[1]= P1 ;
	}
	else {
	  myFirstParam = (myLastParam * (myParams[1] - Un1) + Un1 * myDu)
	      /(myFirstParam -Un1);
	  if (my3d)
	    D03d(myCurve, myFirstParam, P2);
	  else
	    D02d(myCurve, myFirstParam, P2);

	  if ((VV.CrossMagnitude(gp_Vec(P2, P)) / NormD1 < myDeflection) && 
	      (Un1 >= myLastParam - myDwmax) ) {
	    // point n is removed
	    myParams[1]= myLastParam;
            myPoints[1] = P1 ;
	  }
	  else {
	    myParams[1]=myFirstParam;
            myPoints[1] = P2 ;
	    myParams[2]=myLastParam;
            myPoints[2] = P1 ;
	    myNbPoints = myNbPoints +1;  
	  }
	}
      }
    }
    else {
      myNbPoints = myNbPoints +1 ;
      if (myNbPoints >= 3) myNbPoints = 2;
      myParams[myNbPoints]= myLastParam;
      if (my3d) {
	  D03d(myCurve, myLastParam, myPoints[myNbPoints]);
	}
	else {
	  D02d(myCurve, myLastParam, myPoints[myNbPoints]);
	}
    }
  }
}

//=======================================================================
//function : CPnts_UniformDeflection
//purpose  : 
//=======================================================================

CPnts_UniformDeflection::CPnts_UniformDeflection () 
{ 
  myDone = Standard_False;
} 

//=======================================================================
//function : CPnts_UniformDeflection
//purpose  : 
//=======================================================================

CPnts_UniformDeflection::CPnts_UniformDeflection 
                                       (const Adaptor3d_Curve& C, 
					const Standard_Real Deflection,
					const Standard_Real Resolution,
					const Standard_Boolean WithControl)
{
  Initialize(C, Deflection, Resolution, WithControl);
}

//=======================================================================
//function : CPnts_UniformDeflection
//purpose  : 
//=======================================================================

CPnts_UniformDeflection::CPnts_UniformDeflection 
                                       (const Adaptor2d_Curve2d& C, 
					const Standard_Real Deflection,
					const Standard_Real Resolution,
					const Standard_Boolean WithControl)
{
  Initialize(C, Deflection, Resolution, WithControl);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void CPnts_UniformDeflection::Initialize(const Adaptor3d_Curve& C, 
					 const Standard_Real Deflection,
					 const Standard_Real Resolution,
					 const Standard_Boolean WithControl)
{
  Initialize(C,Deflection,C.FirstParameter(),C.LastParameter(),
	     Resolution,WithControl);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void CPnts_UniformDeflection::Initialize(const Adaptor2d_Curve2d& C, 
					 const Standard_Real Deflection,
					 const Standard_Real Resolution,
					 const Standard_Boolean WithControl)
{
  Initialize(C,Deflection,C.FirstParameter(),C.LastParameter(),
	     Resolution,WithControl);
}

//=======================================================================
//function : CPnts_UniformDeflection
//purpose  : 
//=======================================================================

CPnts_UniformDeflection ::CPnts_UniformDeflection
                                      (const Adaptor3d_Curve& C,
				       const Standard_Real Deflection, 
				       const Standard_Real U1,
				       const Standard_Real U2,
				       const Standard_Real Resolution,
				       const Standard_Boolean WithControl)
{
  Initialize(C, Deflection, U1, U2, Resolution, WithControl);
}

//=======================================================================
//function : CPnts_UniformDeflection
//purpose  : 
//=======================================================================

CPnts_UniformDeflection ::CPnts_UniformDeflection
                                      (const Adaptor2d_Curve2d& C,
				       const Standard_Real Deflection, 
				       const Standard_Real U1,
				       const Standard_Real U2,
				       const Standard_Real Resolution,
				       const Standard_Boolean WithControl)
{
  Initialize(C, Deflection, U1, U2, Resolution, WithControl);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void CPnts_UniformDeflection::Initialize (const Adaptor3d_Curve& C,
					  const Standard_Real Deflection, 
					  const Standard_Real U1,
					  const Standard_Real U2,
					  const Standard_Real Resolution,
					  const Standard_Boolean WithControl)
{
  if (U1 > U2) {
    myFirstParam = U2;
    myLastParam  = U1;
  }
  else {
    myFirstParam = U1;
    myLastParam  = U2;
  }
  my3d         = Standard_True;
  myDwmax      = myLastParam-myFirstParam;
  myDu         = myDwmax/2. ;
  myDone       = Standard_True;
  myCurve      = (Standard_Address) &C;
  myFinish     = Standard_False;
  myTolCur     = Resolution;
  myDeflection = Deflection;
  myControl    = WithControl;
  Perform();
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void CPnts_UniformDeflection::Initialize (const Adaptor2d_Curve2d& C,
					  const Standard_Real Deflection, 
					  const Standard_Real U1,
					  const Standard_Real U2,
					  const Standard_Real Resolution,
					  const Standard_Boolean WithControl)
{
  if (U1 > U2) {
    myFirstParam = U2;
    myLastParam  = U1;
  }
  else {
    myFirstParam = U1;
    myLastParam  = U2;
  }
  my3d         = Standard_False;
  myDwmax      = myLastParam-myFirstParam;
  myDu         = myDwmax/2. ;
  myDone       = Standard_True;
  myCurve      = (Standard_Address) &C;
  myFinish     = Standard_False;
  myTolCur     = Resolution;
  myDeflection = Deflection;
  myControl    = WithControl;
  Perform();
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean CPnts_UniformDeflection::More()
{
  if(!myDone) {
    return Standard_False;
  }
  else if (myIPoint == myNbPoints) {
    if (myFinish) {
      return Standard_False;
    }
    else {
      Perform();
      return myDone;
    }
  }
  else {
    return myIPoint < myNbPoints;
  }
}







