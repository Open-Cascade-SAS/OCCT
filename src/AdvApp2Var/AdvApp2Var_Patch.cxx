// File:	AdvApp2Var_Patch.cxx
// Created:	Tue Jul  2 12:12:24 1996
// Author:	Joelle CHAUVET
//		<jct@sgi38>
// Modified:	Wed Jan 15 10:04:41 1997
//   by:	Joelle CHAUVET
//		G1135 : Methods CutSense with criterion, Coefficients,
//                              CritValue, SetCritValue
// Modified:	Tue May 19 10:22:44 1998
//   by:	Joelle CHAUVET / Jean-Marc LACHAUME
//		Initialisation de myCritValue pour OSF

#include <AdvApp2Var_Patch.ixx>
#include <AdvApp2Var_Node.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <Standard_ConstructionError.hxx>

#include <AdvApp2Var_ApproxF2var.hxx>
#include <AdvApp2Var_MathBase.hxx>


//============================================================================
//function : AdvApp2Var_Patch
//purpose  :
//============================================================================

AdvApp2Var_Patch::AdvApp2Var_Patch() :
myU0(0.),
myU1(1.),
myV0(0.),
myV1(1.),
myOrdInU(0),
myOrdInV(0),
myApprIsDone(Standard_False),
myHasResult(Standard_False),
myCutSense(0),
myDiscIsDone(Standard_False),
myCritValue(0.)
{
}

//============================================================================
//function : AdvApp2Var_Patch
//purpose  :
//============================================================================

AdvApp2Var_Patch::AdvApp2Var_Patch(const Standard_Real U0,
				   const Standard_Real U1,
				   const Standard_Real V0,
				   const Standard_Real V1,
				   const Standard_Integer iu,
				   const Standard_Integer iv) :
myU0(U0),
myU1(U1),
myV0(V0),
myV1(V1),
myOrdInU(iu),
myOrdInV(iv),
myApprIsDone(Standard_False),
myHasResult(Standard_False),
myCutSense(0),
myDiscIsDone(Standard_False),
myCritValue(0.)
{
}

//============================================================================
//function : IsDiscretised
//purpose  :
//============================================================================

Standard_Boolean AdvApp2Var_Patch::IsDiscretised() const 
{
  return myDiscIsDone; 
}

//============================================================================
//function : Discretise
//purpose  : 
//============================================================================

void AdvApp2Var_Patch::Discretise(const AdvApp2Var_Context& Conditions,
				  const AdvApp2Var_Framework& Constraints,
				  const AdvApp2Var_EvaluatorFunc2Var& Func) 
{

// les donnees stockees dans le Context
  Standard_Integer NDIMEN, NBSESP, NDIMSE, ISOFAV;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
// Attention : ne marche que pour le 3D
  NDIMSE = 3;
  ISOFAV = Conditions.FavorIso();

// les donnees relatives au patch a discretiser
  Standard_Integer NBPNTU, NBPNTV;
  Standard_Integer IORDRU = myOrdInU, IORDRV = myOrdInV;
  Handle (TColStd_HArray1OfReal) HUROOT  = Conditions.URoots();
  Handle (TColStd_HArray1OfReal) HVROOT  = Conditions.VRoots();
  Standard_Real * UROOT;
    UROOT =  (Standard_Real *) &HUROOT ->ChangeArray1()(HUROOT ->Lower());
  NBPNTU = (Conditions.URoots())->Length();
  if (myOrdInU>-1) NBPNTU -= 2;
  Standard_Real * VROOT;
    VROOT =  (Standard_Real *) &HVROOT ->ChangeArray1()(HVROOT ->Lower());
  NBPNTV = (Conditions.VRoots())->Length();
  if (myOrdInV>-1) NBPNTV -= 2;

// les donnees stockees dans le Framework Constraints cad Noeuds et Isos 
// C1, C2, C3 et C4 sont dimensionnes en FORTRAN a (NDIMEN,IORDRU+2,IORDRV+2)
  Standard_Integer SIZE=NDIMEN*(IORDRU+2)*(IORDRV+2);
  Handle (TColStd_HArray1OfReal) HCOINS  =
    new TColStd_HArray1OfReal(1,SIZE*4);
  HCOINS->Init(0.);

  Standard_Integer iu,iv;
  Standard_Real du=(myU1-myU0)/2,dv=(myV1-myV0)/2,rho,valnorm;

  for (iu=0;iu<=myOrdInU;iu++) {
    for (iv=0;iv<=myOrdInV;iv++) {
// facteur de normalisation
      rho = pow(du,iu)*pow(dv,iv);

// F(U0,V0) et ses derivees normalisees sur (-1,1)
      valnorm = rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).X();
      HCOINS->SetValue( 1+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv , valnorm );
      valnorm = rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );

// F(U1,V0) et ses derivees normalisees sur (-1,1)
      valnorm = rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).X();
      HCOINS->SetValue( SIZE+1+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).Y();
      HCOINS->SetValue( SIZE+2+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).Z();
      HCOINS->SetValue( SIZE+3+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );

// F(U0,V1) et ses derivees normalisees sur (-1,1)
      valnorm = rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).X();
      HCOINS->SetValue( 2*SIZE+1+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2*SIZE+2+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).Z();
      HCOINS->SetValue( 2*SIZE+3+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );

// F(U1,V1) et ses derivees normalisees sur (-1,1)
      valnorm = rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).X();
      HCOINS->SetValue( 3*SIZE+1+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).Y();
      HCOINS->SetValue( 3*SIZE+2+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
      valnorm = rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3*SIZE+3+NDIMEN*iu+NDIMEN*(IORDRU+2)*iv, valnorm );
    }
  }
  Standard_Real *C1 = 
    (Standard_Real *) &HCOINS ->ChangeArray1()(HCOINS ->Lower());
  Standard_Real *C2 = C1 + SIZE;
  Standard_Real *C3 = C2 + SIZE;
  Standard_Real *C4 = C3 + SIZE;

// tableaux SomTab et Diftab de discretisation des isos U=U0 et U=U1
// SU0, SU1, DU0 et DU1 sont dimensionnes en FORTRAN a 
// (1+NBPNTV/2)*NDIMEN*(IORDRU+1)

  SIZE = (1+NBPNTV/2)*NDIMEN;

  Handle (TColStd_HArray1OfReal) HSU0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HSU0 ->ChangeArray1() =
    ( (Constraints.IsoU(myU0,myV0,myV1)).SomTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HDU0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HDU0 ->ChangeArray1() =
    ( (Constraints.IsoU(myU0,myV0,myV1)).DifTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HSU1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HSU1 ->ChangeArray1() =
    ( (Constraints.IsoU(myU1,myV0,myV1)).SomTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HDU1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HDU1 ->ChangeArray1() =
    ( (Constraints.IsoU(myU1,myV0,myV1)).DifTab() ) ->Array1();

// normalisation
  Standard_Integer ideb1,ideb2,ideb3,ideb4,jj;
  for (iu=1;iu<=IORDRU;iu++) {
    rho = pow(du,iu);
    ideb1 = HSU0->Lower() + iu*SIZE -1;
    ideb2 = HDU0->Lower() + iu*SIZE -1;
    ideb3 = HSU1->Lower() + iu*SIZE -1;
    ideb4 = HDU1->Lower() + iu*SIZE -1;
    for (jj=1;jj<=SIZE;jj++) {
      HSU0 ->SetValue(ideb1+jj,rho*HSU0->Value(ideb1+jj));
      HDU0 ->SetValue(ideb2+jj,rho*HDU0->Value(ideb2+jj));
      HSU1 ->SetValue(ideb3+jj,rho*HSU1->Value(ideb3+jj));
      HDU1 ->SetValue(ideb4+jj,rho*HDU1->Value(ideb4+jj));
    }
  }

  Standard_Real *SU0 = 
    (Standard_Real *) &HSU0 ->ChangeArray1()(HSU0 ->Lower());
  Standard_Real *DU0 = 
    (Standard_Real *) &HDU0 ->ChangeArray1()(HDU0 ->Lower());
  Standard_Real *SU1 = 
    (Standard_Real *) &HSU1 ->ChangeArray1()(HSU1 ->Lower());
  Standard_Real *DU1 = 
    (Standard_Real *) &HDU1 ->ChangeArray1()(HDU1 ->Lower());

// tableaux SomTab et Diftab de discretisation des isos V=V0 et V=V1
// SU0, SU1, DU0 et DU1 sont dimensionnes en FORTRAN a
// (1+NBPNTU/2)*NDIMEN*(IORDRV+1)

  SIZE = (1+NBPNTU/2)*NDIMEN;

  Handle (TColStd_HArray1OfReal) HSV0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HSV0 ->ChangeArray1() =
    ( (Constraints.IsoV(myU0,myU1,myV0)).SomTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HDV0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HDV0 ->ChangeArray1() =
    ( (Constraints.IsoV(myU0,myU1,myV0)).DifTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HSV1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HSV1 ->ChangeArray1() =
    ( (Constraints.IsoV(myU0,myU1,myV1)).SomTab() ) ->Array1();

  Handle (TColStd_HArray1OfReal) HDV1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HDV1 ->ChangeArray1() =
    ( (Constraints.IsoV(myU0,myU1,myV1)).DifTab() ) ->Array1();

// normalisation
  for (iv=1;iv<=IORDRV;iv++) {
    rho = pow(dv,iv);
    ideb1 = HSV0->Lower() + iv*SIZE -1;
    ideb2 = HDV0->Lower() + iv*SIZE -1;
    ideb3 = HSV1->Lower() + iv*SIZE -1;
    ideb4 = HDV1->Lower() + iv*SIZE -1;
    for (jj=1;jj<=SIZE;jj++) {
      HSV0 ->SetValue(ideb1+jj,rho*HSV0->Value(ideb1+jj));
      HDV0 ->SetValue(ideb2+jj,rho*HDV0->Value(ideb2+jj));
      HSV1 ->SetValue(ideb3+jj,rho*HSV1->Value(ideb3+jj));
      HDV1 ->SetValue(ideb4+jj,rho*HDV1->Value(ideb4+jj));
    }
  }

  Standard_Real *SV0 = 
    (Standard_Real *) &HSV0 ->ChangeArray1()(HSV0 ->Lower());
  Standard_Real *DV0 = 
    (Standard_Real *) &HDV0 ->ChangeArray1()(HDV0 ->Lower());
  Standard_Real *SV1 = 
    (Standard_Real *) &HSV1 ->ChangeArray1()(HSV1 ->Lower());
  Standard_Real *DV1 = 
    (Standard_Real *) &HDV1 ->ChangeArray1()(HDV1 ->Lower());

// SOSOTB et DIDITB sont dimensionnes en FORTRAN a 
// (0:NBPNTU/2,0:NBPNTV/2,NDIMEN)

  SIZE=(1+NBPNTU/2)*(1+NBPNTV/2)*NDIMEN;

  Handle (TColStd_HArray1OfReal) HSOSO  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *SOSOTB  =  
    (Standard_Real *) &HSOSO ->ChangeArray1()(HSOSO ->Lower());
  HSOSO->Init(0.);
  Handle (TColStd_HArray1OfReal) HDIDI  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *DIDITB  =  
    (Standard_Real *) &HDIDI ->ChangeArray1()(HDIDI ->Lower());
  HDIDI->Init(0.);

// SODITB et DISOTB sont dimensionnes en FORTRAN a
// (1:NBPNTU/2,1:NBPNTV/2,NDIMEN)

  SIZE=(NBPNTU/2)*(NBPNTV/2)*NDIMEN;

  Handle (TColStd_HArray1OfReal) HSODI  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *SODITB  =  
    (Standard_Real *) &HSODI ->ChangeArray1()(HSODI ->Lower());
  HSODI->Init(0.);
  Handle (TColStd_HArray1OfReal) HDISO  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *DISOTB  =  
    (Standard_Real *) &HDISO ->ChangeArray1()(HDISO ->Lower());
  HDISO->Init(0.);

  Standard_Integer IERCOD=0;

//  discretisation des polynomes d'interpolation
  AdvApp2Var_ApproxF2var::mma2cdi_(&NDIMEN,&NBPNTU,UROOT,&NBPNTV,VROOT,&IORDRU,&IORDRV,
	 C1,C2,C3,C4,SU0,SU1,DU0,DU1,SV0,SV1,DV0,DV1,
         SOSOTB,SODITB,DISOTB,DIDITB,&IERCOD);

//  discretisation du carreau
  Standard_Real UDBFN[2],VDBFN[2];
  UDBFN[0] = myU0;
  UDBFN[1] = myU1;
  VDBFN[0] = myV0;
  VDBFN[1] = myV1;

  SIZE = Max(NBPNTU,NBPNTV);
  Handle (TColStd_HArray1OfReal) HTABLE  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *TAB  =  
    (Standard_Real *) &HTABLE ->ChangeArray1()(HTABLE ->Lower());

  Handle (TColStd_HArray1OfReal) HPOINTS  =
    new TColStd_HArray1OfReal(1,SIZE*NDIMEN);
  Standard_Real *PTS  =  
    (Standard_Real *) &HPOINTS ->ChangeArray1()(HPOINTS ->Lower());

  // GCC 3.0 would not accept this line without the void
  // pointer cast.  Perhaps the real problem is a definition
  // somewhere that has a void * in it.
  AdvApp2Var_ApproxF2var::mma2ds1_(&NDIMEN,
				   UDBFN,
				   VDBFN,
				   /*(void *)*/Func,
				   &NBPNTU,
				   &NBPNTV,
				   UROOT,
				   VROOT,
				   &ISOFAV,
				   SOSOTB,
				   DISOTB,
				   SODITB,
				   DIDITB,
				   PTS,
				   TAB,
				   &IERCOD);

// on stocke les resultats
  if (IERCOD == 0) {
    myDiscIsDone = Standard_True;
    mySosoTab = HSOSO;
    myDisoTab = HDISO;
    mySodiTab = HSODI;
    myDidiTab = HDIDI;
  }
  else {
    myDiscIsDone = Standard_False;
  }
}

//============================================================================
//function : HasResult
//purpose  :
//============================================================================

Standard_Boolean AdvApp2Var_Patch::HasResult() const 
{
  return myHasResult; 
}

//============================================================================
//function : IsApproximated
//purpose  :
//============================================================================

Standard_Boolean AdvApp2Var_Patch::IsApproximated() const 
{
  return myApprIsDone;
}

//============================================================================
//function : AddConstraints
//purpose  :
//============================================================================

void AdvApp2Var_Patch::AddConstraints(const AdvApp2Var_Context& Conditions,
				      const AdvApp2Var_Framework& Constraints)
{
// les donnees stockees dans le Context
  Standard_Integer NDIMEN, NBSESP, NDIMSE;
  Standard_Integer IERCOD, NCFLMU, NCFLMV, NDegU, NDegV;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
// Attention : ne marche que pour le 3D
  NDIMSE = 3;
  NCFLMU = Conditions.ULimit();
  NCFLMV = Conditions.VLimit();
  NDegU = NCFLMU - 1;
  NDegV = NCFLMV - 1;

// les donnees relatives au patch a approcher
  Standard_Integer IORDRU = myOrdInU, IORDRV = myOrdInV;
  Standard_Real *PATCAN  =  
    (Standard_Real *) &myEquation ->ChangeArray1()(myEquation ->Lower());

// les courbes d'approximation des Isos U
  Standard_Integer SIZE = NCFLMV*NDIMEN;
  Handle (TColStd_HArray1OfReal) HIsoU0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HIsoU0 -> ChangeArray1() =
    (Constraints.IsoU(myU0,myV0,myV1)).Polynom() -> Array1();
  Standard_Real *IsoU0 = 
    (Standard_Real *) &HIsoU0 ->ChangeArray1()(HIsoU0 ->Lower());
  Handle (TColStd_HArray1OfInteger) HCFU0
    = new TColStd_HArray1OfInteger(1,IORDRU+1);
  Standard_Integer *NCFU0 = 
    (Standard_Integer *) &HCFU0 ->ChangeArray1()(HCFU0 ->Lower());
  HCFU0->Init( (Constraints.IsoU(myU0,myV0,myV1)).NbCoeff() );

  Handle (TColStd_HArray1OfReal) HIsoU1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRU+1));
  HIsoU1 -> ChangeArray1() =
    (Constraints.IsoU(myU1,myV0,myV1)).Polynom() -> Array1();
  Standard_Real *IsoU1 = 
    (Standard_Real *) &HIsoU1 ->ChangeArray1()(HIsoU1 ->Lower());
  Handle (TColStd_HArray1OfInteger) HCFU1
    = new TColStd_HArray1OfInteger(1,IORDRU+1);
  Standard_Integer *NCFU1 = 
    (Standard_Integer *) &HCFU1 ->ChangeArray1()(HCFU1 ->Lower());
  HCFU1->Init( (Constraints.IsoU(myU1,myV0,myV1)).NbCoeff() );

// normalisation des Isos U
  Standard_Integer iu,iv;
  Standard_Real du=(myU1-myU0)/2,dv=(myV1-myV0)/2,rho,valnorm;
  Standard_Integer ideb0,ideb1,jj;

  for (iu=1;iu<=IORDRU;iu++) {
    rho = pow(du,iu);
    ideb0 = HIsoU0->Lower() + iu*SIZE -1;
    ideb1 = HIsoU1->Lower() + iu*SIZE -1;
    for (jj=1;jj<=SIZE;jj++) {
      HIsoU0->SetValue(ideb0+jj,rho*HIsoU0->Value(ideb0+jj));
      HIsoU1->SetValue(ideb1+jj,rho*HIsoU1->Value(ideb1+jj));
    }
  }

// les courbes d'approximation des Isos V
  SIZE = NCFLMU*NDIMEN;
  Handle (TColStd_HArray1OfReal) HIsoV0
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HIsoV0 -> ChangeArray1() =
    (Constraints.IsoV(myU0,myU1,myV0)).Polynom() -> Array1();
  Standard_Real *IsoV0 = 
    (Standard_Real *) &HIsoV0 ->ChangeArray1()(HIsoV0 ->Lower());
  Handle (TColStd_HArray1OfInteger) HCFV0
    = new TColStd_HArray1OfInteger(1,IORDRV+1);
  Standard_Integer *NCFV0 = 
    (Standard_Integer *) &HCFV0 ->ChangeArray1()(HCFV0 ->Lower());
  HCFV0->Init( (Constraints.IsoV(myU0,myU1,myV0)).NbCoeff() );

  Handle (TColStd_HArray1OfReal) HIsoV1
    = new TColStd_HArray1OfReal(1,SIZE*(IORDRV+1));
  HIsoV1 -> ChangeArray1() =
    (Constraints.IsoV(myU0,myU1,myV1)).Polynom() -> Array1();
  Standard_Real *IsoV1 = 
    (Standard_Real *) &HIsoV1 ->ChangeArray1()(HIsoV1 ->Lower());
  Handle (TColStd_HArray1OfInteger) HCFV1
    = new TColStd_HArray1OfInteger(1,IORDRV+1);
  Standard_Integer *NCFV1 = 
    (Standard_Integer *) &HCFV1 ->ChangeArray1()(HCFV1 ->Lower());
  HCFV1->Init( (Constraints.IsoV(myU0,myU1,myV1)).NbCoeff() );

//  normalisation des Isos V
  for (iv=1;iv<=IORDRV;iv++) {
    rho = pow(dv,iv);
    ideb0 = HIsoV0->Lower() + iv*SIZE -1;
    ideb1 = HIsoV1->Lower() + iv*SIZE -1;
    for (jj=1;jj<=SIZE;jj++) {
      HIsoV0 ->SetValue(ideb0+jj,rho*HIsoV0->Value(ideb0+jj));
      HIsoV1->SetValue(ideb1+jj,rho*HIsoV1->Value(ideb1+jj));
    }
  }

// ajout des contraintes a V constant
  Handle (TColStd_HArray1OfReal) HHERMV 
    = new TColStd_HArray1OfReal(1,(2*IORDRV+2)*(2*IORDRV+2));
  Standard_Real *HermV = 
    (Standard_Real *) &HHERMV ->ChangeArray1()(HHERMV ->Lower());
  if (IORDRV>=0) {
    AdvApp2Var_ApproxF2var::mma1her_(&IORDRV,HermV,&IERCOD);
    if (IERCOD!=0) {
      Standard_ConstructionError::Raise
	   ("AdvApp2Var_Patch::AddConstraints : Error in FORTRAN");
    }
    AdvApp2Var_ApproxF2var::mma2ac2_(&NDIMEN,
				     &NDegU,
				     &NDegV,
				     &IORDRV,
				     &NCFLMU,
				     NCFV0,
				     IsoV0,
				     NCFV1,
				     IsoV1,
				     HermV,
				     PATCAN);
  }

// ajout des contraintes a U constant
  Handle (TColStd_HArray1OfReal) HHERMU 
    = new TColStd_HArray1OfReal(1,(2*IORDRU+2)*(2*IORDRU+2));
  Standard_Real *HermU = 
    (Standard_Real *) &HHERMU ->ChangeArray1()(HHERMU ->Lower());
  if (IORDRU>=0) {
    AdvApp2Var_ApproxF2var::mma1her_(&IORDRU,HermU,&IERCOD);
    if (IERCOD!=0) {
      Standard_ConstructionError::Raise
	   ("AdvApp2Var_Patch::AddConstraints : Error in FORTRAN");
    }
    AdvApp2Var_ApproxF2var::mma2ac3_(&NDIMEN,&NDegU,&NDegV,&IORDRU,&NCFLMV,
				     NCFU0,IsoU0,NCFU1,IsoU1,HermU,PATCAN);
  }

// ajout des contraintes de coins
  Standard_Integer ideb;
  SIZE=NDIMEN*(IORDRU+2)*(IORDRV+2);
  Handle (TColStd_HArray1OfReal) HCOINS  =
    new TColStd_HArray1OfReal(1,SIZE*4);

  for (iu=0;iu<=myOrdInU;iu++) {
    for (iv=0;iv<=myOrdInV;iv++) {
      rho = pow(du,iu)*pow(dv,iv);

// -F(U0,V0) et ses derivees normalisees sur (-1,1)
      ideb = HCOINS->Lower() + NDIMEN*iu+NDIMEN*(IORDRU+2)*iv - 1;
      valnorm = -rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).X();
      HCOINS->SetValue( 1+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU0,myV0)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3+ideb , valnorm );

// -F(U1,V0) et ses derivees normalisees sur (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).X();
      HCOINS->SetValue( 1+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU1,myV0)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3+ideb , valnorm );

// -F(U0,V1) et ses derivees normalisees sur (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).X();
      HCOINS->SetValue( 1+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU0,myV1)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3+ideb , valnorm );

// -F(U1,V1) et ses derivees normalisees sur (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).X();
      HCOINS->SetValue( 1+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).Y();
      HCOINS->SetValue( 2+ideb , valnorm );
      valnorm = -rho * ((Constraints.Node(myU1,myV1)).Point(iu,iv)).Z();
      HCOINS->SetValue( 3+ideb , valnorm );
    }
  }

//  tableaux necessaires pour le FORTRAN
  Standard_Integer IORDMX = Max(IORDRU,IORDRV);
  Handle (TColStd_HArray1OfReal) HEXTR =
    new TColStd_HArray1OfReal(1,2*IORDMX+2);
  Standard_Real *EXTR = 
    (Standard_Real *) &HEXTR ->ChangeArray1()(HEXTR ->Lower());
  Handle (TColStd_HArray1OfReal) HFACT =
    new TColStd_HArray1OfReal(1,IORDMX+1);
  Standard_Real *FACT = 
    (Standard_Real *) &HFACT ->ChangeArray1()(HFACT ->Lower());

  Standard_Integer idim,ncf0,ncf1,iun=1;
  Standard_Real *Is;

// ajout des extremites des isos U
  for (iu=1;iu<=IORDRU+1;iu++) {
    ncf0 = HCFU0->Value(HCFU0->Lower()+iu-1);
    ncf1 = HCFU1->Value(HCFU1->Lower()+iu-1);
    for (idim=1;idim<=NDIMEN;idim++) {
      Is = IsoU0 + NCFLMV*(idim-1) + NCFLMV*NDIMEN*(iu-1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRV,&iun,&ncf0,Is,EXTR,FACT);
      for (iv=1;iv<=IORDRV+1;iv++) {
	ideb = HCOINS->Lower() + NDIMEN*(iu-1)+NDIMEN*(IORDRU+2)*(iv-1) - 1;
	HCOINS->ChangeValue(idim+ideb) += HEXTR->Value(1+2*(iv-1));
	HCOINS->ChangeValue(2*SIZE+idim+ideb) += HEXTR->Value(2+2*(iv-1));
      }
      Is = IsoU1 + NCFLMV*(idim-1) + NCFLMV*NDIMEN*(iu-1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRV,&iun,&ncf1,Is,EXTR,FACT);
      for (iv=1;iv<=IORDRV+1;iv++) {
	ideb = HCOINS->Lower() + NDIMEN*(iu-1)+NDIMEN*(IORDRU+2)*(iv-1) - 1;
	HCOINS->ChangeValue(SIZE+idim+ideb) += HEXTR->Value(1+2*(iv-1));
	HCOINS->ChangeValue(3*SIZE+idim+ideb) += HEXTR->Value(2+2*(iv-1));
      }
    }
  }

// ajout des extremites des isos V
  for (iv=1;iv<=IORDRV+1;iv++) {
    ncf0 = HCFV0->Value(HCFV0->Lower()+iv-1);
    ncf1 = HCFV1->Value(HCFV1->Lower()+iv-1);
    for (idim=1;idim<=NDIMEN;idim++) {
      Is = IsoV0 + NCFLMU*(idim-1) + NCFLMU*NDIMEN*(iv-1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRU,&iun,&ncf0,Is,EXTR,FACT);
      for (iu=1;iu<=IORDRU+1;iu++) {
	ideb = HCOINS->Lower() + NDIMEN*(iu-1)+NDIMEN*(IORDRU+2)*(iv-1) - 1;
	HCOINS->ChangeValue(idim+ideb) += HEXTR->Value(1+2*(iu-1));
	HCOINS->ChangeValue(SIZE+idim+ideb) += HEXTR->Value(2+2*(iu-1));
      }
      Is = IsoV1 + NCFLMU*(idim-1) + NCFLMU*NDIMEN*(iv-1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRU,&iun,&ncf1,Is,EXTR,FACT);
      for (iu=1;iu<=IORDRU+1;iu++) {
	ideb = HCOINS->Lower() + NDIMEN*(iu-1)+NDIMEN*(IORDRU+2)*(iv-1) - 1;
	HCOINS->ChangeValue(2*SIZE+idim+ideb) += HEXTR->Value(1+2*(iu-1));
	HCOINS->ChangeValue(3*SIZE+idim+ideb) += HEXTR->Value(2+2*(iu-1));
      }
    }
  }

// ajout du tout a PATCAN
  Standard_Real *C1 = 
    (Standard_Real *) &HCOINS ->ChangeArray1()(HCOINS ->Lower());
  Standard_Real *C2 = C1 + SIZE;
  Standard_Real *C3 = C2 + SIZE;
  Standard_Real *C4 = C3 + SIZE;
  if ( IORDRU>=0 && IORDRV>=0 ) {
    AdvApp2Var_ApproxF2var::mma2ac1_(&NDIMEN,&NDegU,&NDegV,&IORDRU,&IORDRV,
				     C1,C2,C3,C4,HermU,HermV,PATCAN);
  }
}

//============================================================================
//function : AddErrors
//purpose  :
//============================================================================

void AdvApp2Var_Patch::AddErrors(const AdvApp2Var_Framework& Constraints)
{
  Standard_Integer NBSESP = 1, iesp;
  Standard_Integer iu,iv;

  Standard_Real errU,errV,error,hmax[4];
  hmax[0] = 0;
  hmax[1] = 1;
  hmax[2] = 1.5;
  hmax[3] = 1.75;

  for (iesp=1;iesp<=NBSESP;iesp++) {
    //  erreur max dans le sous-espace iesp
    errU=0.;
    for (iv=1;iv<=myOrdInV+1;iv++) {
      error = ((Constraints.IsoV(myU0,myU1,myV0)).MaxErrors())->Value(iesp,iv);
      errU = Max(errU,error);
      error = ((Constraints.IsoV(myU0,myU1,myV1)).MaxErrors())->Value(iesp,iv);
      errU = Max(errU,error);
    }
    errV=0.;
    for (iu=1;iu<=myOrdInU+1;iu++) {
      error = ((Constraints.IsoU(myU0,myV0,myV1)).MaxErrors())->Value(iesp,iu);
      errV = Max(errV,error);
      error = ((Constraints.IsoU(myU1,myV0,myV1)).MaxErrors())->Value(iesp,iu);
      errV = Max(errV,error);
    }
    myMaxErrors->ChangeValue(iesp) +=
      errU * hmax[myOrdInV+1]  +  errV * hmax[myOrdInU+1];

    // erreur moyenne dans le sous-espace iesp
    errU=0.;
    for (iv=1;iv<=myOrdInV+1;iv++) {
      error = ((Constraints.IsoV(myU0,myU1,myV0)).MoyErrors())->Value(iesp,iv);
      errU = Max(errU,error);
      error = ((Constraints.IsoV(myU0,myU1,myV1)).MoyErrors())->Value(iesp,iv);
      errU = Max(errU,error);
    }
    errV=0.;
    for (iu=1;iu<=myOrdInU+1;iu++) {
      error = ((Constraints.IsoU(myU0,myV0,myV1)).MoyErrors())->Value(iesp,iu);
      errV = Max(errV,error);
      error = ((Constraints.IsoU(myU1,myV0,myV1)).MoyErrors())->Value(iesp,iu);
      errV = Max(errV,error);
    }
    error = myMoyErrors->Value(iesp);
    error *= error;
    error += errU*hmax[myOrdInV+1] * errU*hmax[myOrdInV+1]
                   + errV*hmax[myOrdInU+1] * errV*hmax[myOrdInU+1];
    myMoyErrors->SetValue(iesp,Sqrt(error));

    // erreurs maxi aux iso-frontieres
    Handle (TColStd_HArray2OfReal) HERISO
      = new TColStd_HArray2OfReal(1,NBSESP,1,4);
    HERISO->SetValue(iesp,1,
	      ((Constraints.IsoV(myU0,myU1,myV0)).MaxErrors())->Value(iesp,1));
    HERISO->SetValue(iesp,2,
	      ((Constraints.IsoV(myU0,myU1,myV1)).MaxErrors())->Value(iesp,1));
    HERISO->SetValue(iesp,3,
	      ((Constraints.IsoU(myU0,myV0,myV1)).MaxErrors())->Value(iesp,1));
    HERISO->SetValue(iesp,4,
	      ((Constraints.IsoU(myU1,myV0,myV1)).MaxErrors())->Value(iesp,1));

// calcul des erreurs max aux coins
    Standard_Real emax1=0.,emax2=0.,emax3=0.,emax4=0.,err1,err2,err3,err4;
    for (iu=0;iu<=myOrdInU;iu++) {
      for (iv=0;iv<=myOrdInV;iv++) {
	error = (Constraints.Node(myU0,myV0)).Error(iu,iv);
	emax1 = Max(emax1,error);
	error = (Constraints.Node(myU1,myV0)).Error(iu,iv);
	emax2 = Max(emax2,error);
	error = (Constraints.Node(myU0,myV1)).Error(iu,iv);
	emax3 = Max(emax3,error);
	error = (Constraints.Node(myU1,myV1)).Error(iu,iv);
	emax4 = Max(emax4,error);
      }
    }

// calcul des erreurs max sur les bords
    err1 = Max(emax1,emax2);
    err2 = Max(emax3,emax4);
    err3 = Max(emax1,emax3);
    err4 = Max(emax2,emax4);

//   calcul des erreurs finales sur les isos internes
    if ( (Constraints.IsoV(myU0,myU1,myV0)).Position() == 0 ) {
      HERISO ->ChangeValue(iesp,1) += err1*hmax[myOrdInU+1];
    }
    if ( (Constraints.IsoV(myU0,myU1,myV1)).Position() == 0 ) {
      HERISO ->ChangeValue(iesp,2) += err2*hmax[myOrdInU+1];
    }
    if ( (Constraints.IsoU(myU0,myV0,myV1)).Position() == 0 ) {
      HERISO ->ChangeValue(iesp,3) += err3*hmax[myOrdInV+1];
    }
    if ( (Constraints.IsoU(myU1,myV0,myV1)).Position() == 0 ) {
      HERISO ->ChangeValue(iesp,4) += err4*hmax[myOrdInV+1];
    }
    myIsoErrors = HERISO;
  }
}

//============================================================================
//function : MakeApprox
//purpose  :
//============================================================================

void AdvApp2Var_Patch::MakeApprox(const AdvApp2Var_Context& Conditions,
				  const AdvApp2Var_Framework& Constraints,
				  const Standard_Integer NumDec)
{

// les donnees stockees dans le Context
  Standard_Integer NDIMEN, NBSESP, NDIMSE;
  Standard_Integer NBPNTU, NBPNTV, NCFLMU, NCFLMV, NDJACU, NDJACV;
  Standard_Integer NDegU, NDegV, NJacU, NJacV;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
  NDIMSE = 3;
  NBPNTU = (Conditions.URoots())->Length();
  if (myOrdInU>-1) NBPNTU -= 2;
  NBPNTV = (Conditions.VRoots())->Length();
  if (myOrdInV>-1) NBPNTV -= 2;
  NCFLMU = Conditions.ULimit();
  NCFLMV = Conditions.VLimit();
  NDegU = NCFLMU - 1;
  NDegV = NCFLMV - 1;
  NDJACU = Conditions.UJacDeg();
  NDJACV = Conditions.VJacDeg();
  NJacU = NDJACU + 1;
  NJacV = NDJACV + 1;

// les donnees relatives au patch a approcher
  Standard_Integer IORDRU = myOrdInU, IORDRV = myOrdInV,
                   NDMINU = 1, NDMINV = 1, NCOEFU, NCOEFV;
// NDMINU et NDMINV dependent du nb de coeff des isos voisines
// et de l'ordre de continuite souhaite
  NDMINU = Max(1,2*IORDRU+1);
  NCOEFU = (Constraints.IsoV(myU0,myU1,myV0)).NbCoeff()-1;
  NDMINU = Max(NDMINU,NCOEFU);
  NCOEFU = (Constraints.IsoV(myU0,myU1,myV1)).NbCoeff()-1;
  NDMINU = Max(NDMINU,NCOEFU);

  NDMINV = Max(1,2*IORDRV+1);
  NCOEFV = (Constraints.IsoU(myU0,myV0,myV1)).NbCoeff()-1;
  NDMINV = Max(NDMINV,NCOEFV);
  NCOEFV = (Constraints.IsoU(myU1,myV0,myV1)).NbCoeff()-1;
  NDMINV = Max(NDMINV,NCOEFV);

// les tableaux des approximations
  Handle (TColStd_HArray1OfReal) HEPSAPR =
    new TColStd_HArray1OfReal(1,NBSESP);
  Handle (TColStd_HArray1OfReal) HEPSFRO =
    new TColStd_HArray1OfReal(1,NBSESP*8);
  Standard_Integer iesp;
  for (iesp=1;iesp<=NBSESP;iesp++) {
    HEPSAPR->SetValue(iesp,(Conditions.IToler())->Value(iesp));
    HEPSFRO->SetValue(iesp,(Conditions.FToler())->Value(iesp,1));
    HEPSFRO->SetValue(iesp+NBSESP,(Conditions.FToler())->Value(iesp,2));
    HEPSFRO->SetValue(iesp+2*NBSESP,(Conditions.FToler())->Value(iesp,3));
    HEPSFRO->SetValue(iesp+3*NBSESP,(Conditions.FToler())->Value(iesp,4));
    HEPSFRO->SetValue(iesp+4*NBSESP,(Conditions.CToler())->Value(iesp,1));
    HEPSFRO->SetValue(iesp+5*NBSESP,(Conditions.CToler())->Value(iesp,2));
    HEPSFRO->SetValue(iesp+6*NBSESP,(Conditions.CToler())->Value(iesp,3));
    HEPSFRO->SetValue(iesp+7*NBSESP,(Conditions.CToler())->Value(iesp,4));
  }
  Standard_Real *EPSAPR  =
    (Standard_Real *) &HEPSAPR ->ChangeArray1()(HEPSAPR ->Lower());
  Standard_Real *EPSFRO  =
    (Standard_Real *) &HEPSFRO ->ChangeArray1()(HEPSFRO ->Lower());

  Standard_Integer SIZE=(1+NDJACU)*(1+NDJACV)*NDIMEN;
  Handle (TColStd_HArray1OfReal) HPJAC  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *PATJAC  =  
    (Standard_Real *) &HPJAC ->ChangeArray1()(HPJAC ->Lower());
  SIZE=2*SIZE;
  Handle (TColStd_HArray1OfReal) HPAUX  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *PATAUX  =  
    (Standard_Real *) &HPAUX ->ChangeArray1()(HPAUX ->Lower());
  SIZE=NCFLMU*NCFLMV*NDIMEN;
  Handle (TColStd_HArray1OfReal) HPCAN  =
    new TColStd_HArray1OfReal(1,SIZE);
  Standard_Real *PATCAN  =  
    (Standard_Real *) &HPCAN ->ChangeArray1()(HPCAN ->Lower());
  Handle (TColStd_HArray1OfReal) HERRMAX  =  
    new TColStd_HArray1OfReal(1,NBSESP);
  Standard_Real *ERRMAX =
    (Standard_Real *) &HERRMAX ->ChangeArray1()(HERRMAX ->Lower());
  Handle (TColStd_HArray1OfReal) HERRMOY  =  
    new TColStd_HArray1OfReal(1,NBSESP);
  Standard_Real *ERRMOY =
    (Standard_Real *) &HERRMOY ->ChangeArray1()(HERRMOY ->Lower());

// les tableaux de discretisation du carreau
  Standard_Real *SOSOTB  =  
    (Standard_Real *) &mySosoTab ->ChangeArray1()(mySosoTab ->Lower());
  Standard_Real *DISOTB  =  
    (Standard_Real *) &myDisoTab ->ChangeArray1()(myDisoTab ->Lower());
  Standard_Real *SODITB  =  
    (Standard_Real *) &mySodiTab ->ChangeArray1()(mySodiTab ->Lower());
  Standard_Real *DIDITB  =  
    (Standard_Real *) &myDidiTab ->ChangeArray1()(myDidiTab ->Lower());

//  approximation
  Standard_Integer ITYDEC=0, IERCOD=0;
  Standard_Integer iun=1,itrois=3;
  NCOEFU=0;
  NCOEFV=0;
  AdvApp2Var_ApproxF2var::mma2ce1_((integer *)&NumDec,
				   &NDIMEN,
				   &NBSESP,
				   &NDIMSE,
				   &NDMINU,
				   &NDMINV,
				   &NDegU,
				   &NDegV,
				   &NDJACU,
				   &NDJACV,
				   &IORDRU,
				   &IORDRV,
				   &NBPNTU,
				   &NBPNTV,
				   EPSAPR,
				   SOSOTB,
				   DISOTB,
				   SODITB,
				   DIDITB,
				   PATJAC,
				   ERRMAX,
				   ERRMOY,
				   &NCOEFU,
				   &NCOEFV,
				   &ITYDEC,
				   &IERCOD);

// les resultats
  myCutSense = ITYDEC;
  if (ITYDEC == 0 && IERCOD<=0) {
    myHasResult  = Standard_True;
    myApprIsDone = (IERCOD==0);
    myNbCoeffInU  = NCOEFU+1;
    myNbCoeffInV  = NCOEFV+1;
    myMaxErrors = HERRMAX;
    myMoyErrors = HERRMOY;

// Passage en canonique sur [-1,1]
    AdvApp2Var_MathBase::mmfmca9_(&NJacU,&NJacV,&NDIMEN,&myNbCoeffInU,&myNbCoeffInV,
				  &NDIMEN,PATJAC,PATJAC);
    AdvApp2Var_ApproxF2var::mma2can_(&NCFLMU,&NCFLMV,&NDIMEN,
				     &myOrdInU,&myOrdInV,&myNbCoeffInU,
				     &myNbCoeffInV,
				     PATJAC,PATAUX,PATCAN,&IERCOD);
    if (IERCOD !=0) {
      Standard_ConstructionError::Raise
	      ("AdvApp2Var_Patch::MakeApprox : Error in FORTRAN");
    }
    myEquation = HPCAN;

// Ajout des contraintes et des erreurs
    AddConstraints(Conditions,Constraints);
    AddErrors(Constraints);

// Reduction des degres si possible
    PATCAN = (Standard_Real *) 
      &myEquation->ChangeArray1()(myEquation ->Lower());

    AdvApp2Var_ApproxF2var::mma2fx6_(&NCFLMU,
				     &NCFLMV,
				     &NDIMEN,
				     &NBSESP,
				     &itrois,
				     &iun,
				     &iun,
				     &IORDRU,
				     &IORDRV,
				     EPSAPR,
				     EPSFRO,
				     PATCAN,
				     ERRMAX,
				     &myNbCoeffInU,
				     &myNbCoeffInV);
    
// transposition (NCFLMU,NCFLMV,NDIMEN)Fortran-C++
    Standard_Integer aIU, aIN, dim, ii, jj;
    for (dim=1; dim<=NDIMEN; dim++){
       aIN = (dim-1)*NCFLMU*NCFLMV;
       for (ii=1; ii<=NCFLMU; ii++) {
	 aIU = (ii-1)*NDIMEN*NCFLMV;
	 for (jj=1; jj<=NCFLMV; jj++) {
	   HPAUX->SetValue(dim+NDIMEN*(jj-1)+aIU ,
			   myEquation->Value(ii+NCFLMU*(jj-1)+aIN) );
	 }
       }
     }
    myEquation = HPAUX;
  } 
  else { 
    myApprIsDone = Standard_False;
    myHasResult  = Standard_False;
  } 
}

//============================================================================
//function : ChangeDomain
//purpose  :
//============================================================================

void AdvApp2Var_Patch::ChangeDomain(const Standard_Real a,
				    const Standard_Real b,
				    const Standard_Real c,
				    const Standard_Real d)
{
  myU0 = a;
  myU1 = b;
  myV0 = c;
  myV1 = d;
}

//============================================================================
//function : ResetApprox
//purpose  : permet d'effacer un resultat lorsqu'il faut decouper
//============================================================================

void AdvApp2Var_Patch::ResetApprox()
{
  myApprIsDone = Standard_False;
  myHasResult = Standard_False;
}

//============================================================================
//function : OverwriteApprox
//purpose  : permet de conserver un resultat 
//           meme si la precision n'est pas satisfaite
//============================================================================

void AdvApp2Var_Patch::OverwriteApprox()
{
  if (myHasResult) myApprIsDone = Standard_True;
}

//============================================================================
//function : U0
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Patch::U0() const 
{
  return myU0;
}

//============================================================================
//function : U1
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Patch::U1() const 
{
  return myU1;
}

//============================================================================
//function : V0
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Patch::V0() const 
{
  return myV0;
}

//============================================================================
//function : V1
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Patch::V1() const 
{
  return myV1;
}

//============================================================================
//function : UOrder
//purpose  : 
//============================================================================

Standard_Integer AdvApp2Var_Patch::UOrder() const 
{
  return myOrdInU;
}


//============================================================================
//function : VOrder
//purpose  : 
//============================================================================

Standard_Integer AdvApp2Var_Patch::VOrder() const 
{
  return myOrdInV;
}


//============================================================================
//function : CutSense sans Critere
//purpose  : 0 : OK; 1 : decoupe necessaire en U;
//           2 : dec. nec. en V; 3 : dec. nec. en U et en V
//============================================================================

Standard_Integer AdvApp2Var_Patch::CutSense() const 
{
  return myCutSense;
}


//============================================================================
//function : CutSense avec critere
//purpose  : 0 : OK; 1 : decoupe necessaire en U;
//           2 : dec. nec. en V; 3 : dec. nec. en U et en V
//============================================================================

Standard_Integer AdvApp2Var_Patch::CutSense(const AdvApp2Var_Criterion& Crit,
					    const Standard_Integer NumDec) const 
{
  Standard_Boolean CritRel = (Crit.Type() == AdvApp2Var_Relative);
  if ( CritRel && !IsApproximated()) {
    return myCutSense;
  }
  else {
    if (Crit.IsSatisfied(*this)) {
      return 0;
    } 
    else {
      return NumDec;
    } 
  }
}


//============================================================================
//function : NbCoeffInU
//purpose  :
//============================================================================

Standard_Integer AdvApp2Var_Patch::NbCoeffInU() const 
{
  return myNbCoeffInU;
}

//============================================================================
//function : NbCoeffInV
//purpose  :
//============================================================================

Standard_Integer AdvApp2Var_Patch::NbCoeffInV() const 
{
  return myNbCoeffInV;
}

//============================================================================
//function : ChangeNbCoeff
//purpose  : permet d'augmenter le nombre de coeff (cf Network)
//============================================================================

void AdvApp2Var_Patch::ChangeNbCoeff(const Standard_Integer NbCoeffU,
				     const Standard_Integer NbCoeffV)
{
  if (myNbCoeffInU<NbCoeffU) myNbCoeffInU = NbCoeffU;
  if (myNbCoeffInV<NbCoeffV) myNbCoeffInV = NbCoeffV;
}

//============================================================================
//function : MaxErrors
//purpose  : retourne les erreurs max de l'approximation polynomiale
//============================================================================

Handle(TColStd_HArray1OfReal) 
AdvApp2Var_Patch::MaxErrors() const 
{
  return myMaxErrors;
}

//============================================================================
//function : AverageErrors
//purpose  : retourne les erreurs moyennes de l'approximation polynomiale
//============================================================================

Handle(TColStd_HArray1OfReal) 
AdvApp2Var_Patch::AverageErrors() const 
{
  return myMoyErrors;
}

//============================================================================
//function : IsoErrors
//purpose  : retourne les erreurs max sur les frontieres de l'approx. polyn.
//============================================================================

Handle(TColStd_HArray2OfReal) 
AdvApp2Var_Patch::IsoErrors() const 
{
  return myIsoErrors;
}

//============================================================================
//function : Poles
//purpose  : retourne les poles de l'approximation polynomiale
//============================================================================

Handle(TColgp_HArray2OfPnt) 
AdvApp2Var_Patch::Poles(const Standard_Integer SSPIndex,
			const  AdvApp2Var_Context & Cond) const 
{
  Handle(TColStd_HArray1OfReal) SousEquation;
  if ( Cond.TotalNumberSSP( ) == 1 && SSPIndex == 1 ) {
    SousEquation = myEquation;
  }
  else {
    Standard_ConstructionError::Raise
               ("AdvApp2Var_Patch::Poles :  SSPIndex out of range");
  }
  Handle(TColStd_HArray1OfReal) Intervalle = 
    new (TColStd_HArray1OfReal) (1,2);
  Intervalle->SetValue(1, -1);
  Intervalle->SetValue(2, 1);


  Handle(TColStd_HArray1OfInteger) NbCoeff = 
    new (TColStd_HArray1OfInteger) (1,2);
  NbCoeff->SetValue(1, myNbCoeffInU);
  NbCoeff->SetValue(2, myNbCoeffInV);

// Conversion
  Convert_GridPolynomialToPoles 
     Conv (Cond.ULimit()-1,
	   Cond.VLimit()-1,
	   NbCoeff,
	   SousEquation,
	   Intervalle,
	   Intervalle);

  return Conv.Poles(); 	
}


//============================================================================
//function : Coefficients
//purpose  : retourne les coeff. de l'equation de l'approximation polynomiale
//============================================================================

Handle(TColStd_HArray1OfReal) 
AdvApp2Var_Patch::Coefficients(const Standard_Integer SSPIndex,
			       const  AdvApp2Var_Context & Cond) const 
{
  Handle(TColStd_HArray1OfReal) SousEquation;
  if ( Cond.TotalNumberSSP( ) == 1 && SSPIndex == 1 ) {
    SousEquation = myEquation;
  }
  else {
    Standard_ConstructionError::Raise
               ("AdvApp2Var_Patch::Poles :  SSPIndex out of range");
  }
  return SousEquation;
}


//============================================================================
//function : CritValue
//purpose  :
//============================================================================

Standard_Real AdvApp2Var_Patch::CritValue() const 
{
  return myCritValue;
}


//============================================================================
//function : SetCritValue
//purpose  :
//============================================================================

void AdvApp2Var_Patch::SetCritValue(const Standard_Real dist) 
{
  myCritValue = dist;
}











