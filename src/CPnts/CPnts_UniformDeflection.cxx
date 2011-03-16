//-------------------------------------------------------------------
//                algorihme lieu a fleche constante
//
// cas traites :  courbe parametree
//               la courbe doit etre C2
// on assure une fleche maxi
//
// algorithme courbe parametree:
//
// 
//   le calcul du pas d avancement est 
//             du = sqrt(8*fleche*||P'(u)||/||P'(u)^P''(u)||
//
//   on calcule chaque point t.q. u+Du
//
//   on verifie si la fleche est effectivement respectee ,si oui on continue
//   sinon on rectifie le pas
//
//   si du ne peut etre calculer (courbure nulle ,singularite sur la courbe) on
//   prendra alors un pas constant pour atteindre le dernier point ou le depass//   er
//
//   le dernier point est reajuste selon les criteres suivants:
//
//     si le dernier parametre calcule <2*resolution,on recadre le dernier
//     point trouve entre lui meme et le precedent et on rajoute le point
//     de fin (eviter une concentration a la fin)
//
//     sinon si la distance (dernier point calcule ,point de fin)<fleche,on
//     remplace le dernier point calcule par le point de fin
//
//     sinon on calcule une fleche max entre l avant dernier point calcule
//     et le point de fin ;si cette fleche est superieure a la fleche on 
//     remplace le dernier point par celui ci et le point de fin
//
//
//    LES CONTROLES DE FLECHE ET DERNIER POINT NE SONT FAITS QUE SI
//      withControl=true
// 
//   chaque iteration calcule au maximum 3 points
//
//         
//-------------------------------------------------------------------------

#include <CPnts_UniformDeflection.ixx>

#include  <StdFail_NotDone.hxx>
#include  <Standard_DomainError.hxx>
#include  <Standard_OutOfRange.hxx>
#include  <Standard_ConstructionError.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

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

static inline void D13d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& P, gp_Vec& V1)
{
  ((Adaptor3d_Curve*)C)->D1(U,P,V1);
}

// Unused :
#ifdef DEB
static  void D12d(const Standard_Address C, const Standard_Real U,
		      gp_Pnt& PP, gp_Vec& VV1)
{
  gp_Pnt2d P;
  gp_Vec2d V1;
  ((Adaptor2d_Curve2d*)C)->D1(U,P,V1);
  PP.SetCoord(P.X(),P.Y(),0.);
  VV1.SetCoord(V1.X(),V1.Y(),0.);
}
#endif

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
      // singularite sur la tangente ou courbure nulle
      myDu = Min(myDwmax, 1.5 * myDu);
    }
    else { 
      NormD2 = V2.CrossMagnitude(V1);
      if (NormD2 / NormD1 < myDeflection) {  // collinearite des derivees
	myDu = Min(myDwmax, 1.5 * myDu);            
      }
      else {
	myDu = Sqrt(8.* myDeflection * NormD1 / NormD2 );
	myDu = Min(Max(myDu, myTolCur), myDwmax);             
      }
    }
    
    // verifier si la fleche est respectee si WithControl
    
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
	
	// le depassement de fleche a partir duquel on redivise est arbitraire
	// il faudra peut etre le reajuster (differencier le premier point des
	// autres) ce test ne marche pas sur les points d inflexion
	
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
    // le dernier point est corrige si control
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
	    // on supprime le point n
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







