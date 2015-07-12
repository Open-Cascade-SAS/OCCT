// Created on: 1996-07-02
// Created by: Joelle CHAUVET
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


#include <AdvApp2Var_ApproxF2var.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_Node.hxx>
#include <gp_Pnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

//=======================================================================
//function : AdvApp2Var_Iso
//purpose  : 
//=======================================================================
AdvApp2Var_Iso::AdvApp2Var_Iso()  :
myType(GeomAbs_IsoU),
myConstPar(0.5),
myU0(0.),
myU1(1.),
myV0(0.),
myV1(1.),
myPosition(0),
myExtremOrder(2),
myDerivOrder(2),
myNbCoeff(0),
myApprIsDone(Standard_False),
myHasResult(Standard_False)
{
}

//=======================================================================
//function : AdvApp2Var_Iso
//purpose  : 
//=======================================================================

AdvApp2Var_Iso::AdvApp2Var_Iso(const GeomAbs_IsoType type,
			       const Standard_Real cte,
			       const Standard_Real Ufirst,
			       const Standard_Real Ulast,
			       const Standard_Real Vfirst,
			       const Standard_Real Vlast,
			       const Standard_Integer pos,
			       const Standard_Integer iu,
			       const Standard_Integer iv) :
myType(type),
myConstPar(cte),
myU0(Ufirst),
myU1(Ulast),
myV0(Vfirst),
myV1(Vlast),
myPosition(pos),
myNbCoeff(0),
myApprIsDone(Standard_False),
myHasResult(Standard_False)
{
  if (myType==GeomAbs_IsoU) {
    myExtremOrder = iv;
    myDerivOrder = iu;
  }
  else {
    myExtremOrder = iu;
    myDerivOrder = iv;
  }
}

//=======================================================================
//function : IsApproximated
//purpose  : 
//=======================================================================

Standard_Boolean AdvApp2Var_Iso::IsApproximated() const
{
  return myApprIsDone;
}

//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

Standard_Boolean AdvApp2Var_Iso::HasResult() const
{
  return myHasResult;
}

//=======================================================================
//function : MakeApprox
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::MakeApprox(const AdvApp2Var_Context& Conditions,
                                const Standard_Real U0, 
                                const Standard_Real U1, 
                                const Standard_Real V0, 
                                const Standard_Real V1, 
			     	const AdvApp2Var_EvaluatorFunc2Var& Func,
			     	AdvApp2Var_Node& NodeBegin,
				AdvApp2Var_Node& NodeEnd)
{
// fixed values
  Standard_Integer NBCRMX=1, NBCRBE;
// data stored in the Context
  Standard_Integer NDIMEN, NBSESP, NDIMSE;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
// Attention : works only in 3D
  NDIMSE = 3;
// the domain of the grid
  Standard_Real UVFONC[4];
  UVFONC[0] = U0;
  UVFONC[1] = U1;
  UVFONC[2] = V0;
  UVFONC[3] = V1;

// data related to the processed iso
  Standard_Integer IORDRE = myExtremOrder, IDERIV = myDerivOrder;
  Standard_Real TCONST = myConstPar;

// data related to the type of the iso
  Standard_Integer ISOFAV = 0,NBROOT = 0,NDGJAC = 0,NCFLIM = 1;
  Standard_Real TABDEC[2];
  Handle (TColStd_HArray1OfReal) HUROOT  = Conditions.URoots();
  Handle (TColStd_HArray1OfReal) HVROOT  = Conditions.VRoots();
  Standard_Real * ROOTLG=NULL;
  switch(myType) {
   case GeomAbs_IsoV : 
    ISOFAV = 2;
    TABDEC[0] = myU0;
    TABDEC[1] = myU1;
    UVFONC[0] = myU0;
    UVFONC[1] = myU1;
    NBROOT = (Conditions.URoots())->Length();
    if (myExtremOrder>-1) NBROOT -= 2;
    ROOTLG =  (Standard_Real *) &HUROOT ->ChangeArray1()(HUROOT ->Lower());
    NDGJAC = Conditions.UJacDeg();
    NCFLIM = Conditions.ULimit();
    break;
   case GeomAbs_IsoU : 
    ISOFAV = 1;
    TABDEC[0] = myV0;
    TABDEC[1] = myV1;
    UVFONC[2] = myV0;
    UVFONC[3] = myV1;
    NBROOT = (Conditions.VRoots())->Length();
    if (myExtremOrder>-1) NBROOT -= 2;
    ROOTLG =  (Standard_Real *) &HVROOT ->ChangeArray1()(HVROOT ->Lower());
    NDGJAC = Conditions.VJacDeg();
    NCFLIM = Conditions.VLimit();
    break;
    //#ifndef OCCT_DEBUG
    //pkv f
  case GeomAbs_NoneIso:
    //pkv t
  default:
    break;
    //#endif
  }

// data relative to the position of iso (front or cut line)
  Handle (TColStd_HArray1OfReal) HEPSAPR = new TColStd_HArray1OfReal(1,NBSESP);
  Standard_Integer iesp;
  switch(myPosition) {
   case 0 :
    for (iesp=1;iesp<=NBSESP;iesp++) {
      HEPSAPR->SetValue(iesp,(Conditions.CToler())->Value(iesp,1));
    }
    break;
   case 1 : 
    for (iesp=1;iesp<=NBSESP;iesp++) {
      HEPSAPR->SetValue(iesp,(Conditions.FToler())->Value(iesp,1));
    }
    break;
   case 2 : 
    for (iesp=1;iesp<=NBSESP;iesp++) {
      HEPSAPR->SetValue(iesp,(Conditions.FToler())->Value(iesp,2));
    }
    break;
   case 3 : 
    for (iesp=1;iesp<=NBSESP;iesp++) {
      HEPSAPR->SetValue(iesp,(Conditions.FToler())->Value(iesp,3));
    }
    break;
   case 4 : 
    for (iesp=1;iesp<=NBSESP;iesp++) {
      HEPSAPR->SetValue(iesp,(Conditions.FToler())->Value(iesp,4));
    }
    break;
  }
  Standard_Real *EPSAPR  
    =  (Standard_Real *) &HEPSAPR ->ChangeArray1()(HEPSAPR ->Lower());

// the tables of approximations
  Standard_Integer SZCRB = NDIMEN*NCFLIM;
  Handle (TColStd_HArray1OfReal) HCOURBE  =
    new TColStd_HArray1OfReal(1,SZCRB*(IDERIV+1));
  Standard_Real *COURBE  =  
    (Standard_Real *) &HCOURBE ->ChangeArray1()(HCOURBE ->Lower());
  Standard_Real *CRBAPP = COURBE;
  Standard_Integer SZTAB = (1+NBROOT/2)*NDIMEN;
  Handle (TColStd_HArray1OfReal) HSOMTAB  =
    new TColStd_HArray1OfReal(1,SZTAB*(IDERIV+1));
  Standard_Real *SOMTAB  =  
    (Standard_Real *) &HSOMTAB ->ChangeArray1()(HSOMTAB ->Lower());
  Standard_Real *SOMAPP = SOMTAB;
  Handle (TColStd_HArray1OfReal) HDIFTAB  =
    new TColStd_HArray1OfReal(1,SZTAB*(IDERIV+1));
  Standard_Real *DIFTAB  =  
    (Standard_Real *) &HDIFTAB ->ChangeArray1()(HDIFTAB ->Lower());
  Standard_Real *DIFAPP = DIFTAB;
  Handle (TColStd_HArray1OfReal) HCONTR1 = 
    new TColStd_HArray1OfReal(1,(IORDRE+2)*NDIMEN);
  Standard_Real *CONTR1 = 
    (Standard_Real *) &HCONTR1->ChangeArray1()(HCONTR1->Lower()); 
  Handle (TColStd_HArray1OfReal) HCONTR2 = 
    new TColStd_HArray1OfReal(1,(IORDRE+2)*NDIMEN);
  Standard_Real *CONTR2 = 
    (Standard_Real *) &HCONTR2->ChangeArray1()(HCONTR2->Lower()); 
  Handle (TColStd_HArray2OfReal) HERRMAX  =  
    new TColStd_HArray2OfReal(1,NBSESP,1,IDERIV+1);
  Standard_Real *EMXAPP = new Standard_Real[NBSESP];
  Handle (TColStd_HArray2OfReal) HERRMOY  =  
    new TColStd_HArray2OfReal(1,NBSESP,1,IDERIV+1);
  //#ifdef OCCT_DEBUG
  //Standard_Real *ERRMOY =
  //#endif
  //  (Standard_Real *) &HERRMOY->ChangeArray2()(HERRMOY ->LowerRow(),HERRMOY ->LowerCol());
  Standard_Real *EMYAPP = new Standard_Real[NBSESP];
//
// the approximations
//
  Standard_Integer IERCOD=0, NCOEFF=0;
  Standard_Integer iapp,ncfapp,ierapp;
//  Standard_Integer id,ic,ideb;
  for (iapp=0;iapp<=IDERIV;iapp++) {
//   approximation of the derivative of order iapp
    ncfapp = 0;
    ierapp = 0;
    // GCC 3.0 would not accept this line without the void
    // pointer cast.  Perhaps the real problem is a definition
    // somewhere that has a void * in it.
    AdvApp2Var_ApproxF2var::mma2fnc_(&NDIMEN,
				     &NBSESP,
				     &NDIMSE,
				     UVFONC,
				     /*(void *)*/Func,
				     &TCONST,
				     &ISOFAV,
				     &NBROOT,
				     ROOTLG,
				     &IORDRE,
				     &iapp,
				     &NDGJAC,
				     &NBCRMX,
				     &NCFLIM,
				     EPSAPR,
				     &ncfapp,
				     CRBAPP,
				     &NBCRBE,
				     SOMAPP,
				     DIFAPP,
				     CONTR1,
				     CONTR2,
				     TABDEC,
				     EMXAPP,
				     EMYAPP,
				     &ierapp);
//   error and coefficient management.
    if (ierapp>0) {
      myApprIsDone = Standard_False;
      myHasResult  = Standard_False;
      goto FINISH;
    }
    if (NCOEFF<=ncfapp) NCOEFF=ncfapp;
    if (ierapp==-1) IERCOD = -1;
//   return constraints of order 0 to IORDRE of extremities
    Standard_Integer ider, jpos=HCONTR1->Lower();
    for (ider=0; ider<=IORDRE;ider++) {
      gp_Pnt pt(HCONTR1->Value(jpos),
		HCONTR1->Value(jpos+1),
		HCONTR1->Value(jpos+2));
      if (ISOFAV==2) {
	NodeBegin.SetPoint(ider,iapp, pt);
      }
      else {
	NodeBegin.SetPoint(iapp,ider, pt);
      }
      jpos+=3;
    }
    jpos=HCONTR2->Lower();
    for (ider=0; ider<=IORDRE;ider++) {
      gp_Pnt pt(HCONTR2->Value(jpos),
		HCONTR2->Value(jpos+1),
		HCONTR2->Value(jpos+2));
      if (ISOFAV==2) {
	NodeEnd.SetPoint(ider,iapp, pt);
      }
      else {
	NodeEnd.SetPoint(iapp,ider, pt);
      }
      jpos+=3;
    }
//   return errors
    for (iesp=1; iesp<=NBSESP;iesp++) {
      HERRMAX->SetValue(iesp,iapp+1,EMXAPP[iesp-1]);
      HERRMOY->SetValue(iesp,iapp+1,EMYAPP[iesp-1]);
    }
// passage to the approximation of higher order
    CRBAPP += SZCRB;
    SOMAPP += SZTAB;
    DIFAPP += SZTAB;
  }

// management of results
  if (IERCOD == 0) {
//   all approximations are correct
    myApprIsDone = Standard_True;
    myHasResult  = Standard_True;
  }
  else if (IERCOD == -1) {
//   at least one approximation is not correct
    myApprIsDone = Standard_False;
    myHasResult  = Standard_True;
  } 
  else { 
    myApprIsDone = Standard_False;
    myHasResult  = Standard_False;
  } 
  if ( myHasResult ) {
    myEquation = HCOURBE;
    myNbCoeff  = NCOEFF;
    myMaxErrors = HERRMAX;
    myMoyErrors = HERRMOY;
    mySomTab = HSOMTAB;
    myDifTab = HDIFTAB;
  }
  FINISH:
  delete []EMXAPP;
  delete []EMYAPP;
}

//=======================================================================
//function : ChangeDomain
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::ChangeDomain(const Standard_Real a, const Standard_Real b)
{
  if (myType==GeomAbs_IsoU) {
    myV0 = a;
    myV1 = b;
  }
  else {
    myU0 = a;
    myU1 = b;
  }
}

//=======================================================================
//function : ChangeDomain
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::ChangeDomain(const Standard_Real a,
				  const Standard_Real b,
				  const Standard_Real c,
				  const Standard_Real d)
{
  myU0 = a;
  myU1 = b;
  myV0 = c;
  myV1 = d;
}

//=======================================================================
//function : SetConstante
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::SetConstante(const Standard_Real newcte)
{
  myConstPar = newcte;
}

//=======================================================================
//function : SetPosition
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::SetPosition(const Standard_Integer newpos)
{
  myPosition = newpos;
}

//=======================================================================
//function : ResetApprox
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::ResetApprox()
{
  myApprIsDone = Standard_False;
  myHasResult = Standard_False;
}

//=======================================================================
//function : OverwriteApprox
//purpose  : 
//=======================================================================

void AdvApp2Var_Iso::OverwriteApprox()
{
  if (myHasResult) myApprIsDone = Standard_True;
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

GeomAbs_IsoType AdvApp2Var_Iso::Type() const 
{
  return myType;
}

//=======================================================================
//function : Constante
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::Constante() const 
{
  return myConstPar;
}

//=======================================================================
//function : T0
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::T0() const 
{
  if (myType==GeomAbs_IsoU) {
    return myV0;
  }
  else {
    return myU0;
  }
}

//=======================================================================
//function : T1
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::T1() const 
{
  if (myType==GeomAbs_IsoU) {
    return myV1;
  }
  else {
    return myU1;
  }
}

//=======================================================================
//function : U0
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::U0() const 
{
  return myU0;
}

//=======================================================================
//function : U1
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::U1() const 
{
  return myU1;
}

//=======================================================================
//function : V0
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::V0() const 
{
  return myV0;
}

//=======================================================================
//function : V1
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Iso::V1() const 
{
  return myV1;
}

//=======================================================================
//function : UOrder
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Iso::UOrder() const 
{
  if (Type()==GeomAbs_IsoU) return myDerivOrder;
  else return myExtremOrder;
}


//=======================================================================
//function : VOrder
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Iso::VOrder() const 
{
  if (Type()==GeomAbs_IsoV) return myDerivOrder;
  else return myExtremOrder;
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Iso::Position() const 
{
  return myPosition;
}

//=======================================================================
//function : NbCoeff
//purpose  : 
//=======================================================================


Standard_Integer AdvApp2Var_Iso::NbCoeff() const 
{
  return myNbCoeff;
}

//=======================================================================
//function : Polynom
//purpose  : 
//=======================================================================

const Handle(TColStd_HArray1OfReal)& AdvApp2Var_Iso::Polynom() const 
{
  return myEquation;
}

Handle(TColStd_HArray1OfReal) AdvApp2Var_Iso::SomTab() const 
{
  return mySomTab;
}

Handle(TColStd_HArray1OfReal) AdvApp2Var_Iso::DifTab() const 
{
  return myDifTab;
}

Handle(TColStd_HArray2OfReal) AdvApp2Var_Iso::MaxErrors() const 
{
  return myMaxErrors;
}

Handle(TColStd_HArray2OfReal) AdvApp2Var_Iso::MoyErrors() const 
{
  return myMoyErrors;
}










