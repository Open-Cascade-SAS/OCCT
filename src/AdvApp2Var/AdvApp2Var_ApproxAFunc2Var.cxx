// File:	AdvApp2Var_ApproxAFunc2Var.cxx
// Created:	Wed Jul  3 15:34:08 1996
// Author:	Joelle CHAUVET
//		<jct@sgi38>
// Modified:	Wed Jan 15 10:04:41 1997
//    by:	Joelle CHAUVET
//		G1135 : Constructor with criterion
//		        Private methods 'Init','InitGrid','Perform','ConvertBS',
//		                        'ComputePatches','ComputeConstraints',
//		                        'Compute3DErrors','ComputeCritError'
//		        Public method 'CritError'
//		        Fields 'myConditions','myResults','myConstraints'
// Modified:	Fri Oct  3 14:58:05 1997
//    by:	Joelle CHAUVET
//              GeomConvert_CompBezierSurfacesToBSplineSurface est remplace par 
//              Convert_GridPolynomialToPoles dans ConvertBS 

#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Patch.hxx>
#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Node.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_Strip.hxx>
#include <AdvApp2Var_Framework.hxx>
#include <AdvApprox_Cutting.hxx>

#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray2OfPnt.hxx>

#include <Convert_GridPolynomialToPoles.hxx>

#include <Geom_BezierSurface.hxx>


//=======================================================================
//function : AdvApp2Var_ApproxAFunc2Var
//purpose  : 
//=======================================================================

 AdvApp2Var_ApproxAFunc2Var::
AdvApp2Var_ApproxAFunc2Var(const Standard_Integer Num1DSS,
			   const Standard_Integer Num2DSS,
			   const Standard_Integer Num3DSS,
			   const Handle(TColStd_HArray1OfReal)& OneDTol,
			   const Handle(TColStd_HArray1OfReal)& TwoDTol,
			   const Handle(TColStd_HArray1OfReal)& ThreeDTol,
			   const Handle(TColStd_HArray2OfReal)& OneDTolFr,
			   const Handle(TColStd_HArray2OfReal)& TwoDTolFr,
			   const Handle(TColStd_HArray2OfReal)& ThreeDTolFr,
			   const Standard_Real FirstInU,
			   const Standard_Real LastInU,
			   const Standard_Real FirstInV,
			   const Standard_Real LastInV,
			   const GeomAbs_IsoType FavorIso,
			   const GeomAbs_Shape ContInU,
			   const GeomAbs_Shape ContInV,
			   const Standard_Integer PrecisCode,
			   const Standard_Integer MaxDegInU,
			   const Standard_Integer MaxDegInV,
			   const Standard_Integer MaxPatch,
			   const AdvApp2Var_EvaluatorFunc2Var& Func,
			   AdvApprox_Cutting& UChoice,
			   AdvApprox_Cutting& VChoice) :
my1DTolerances(OneDTol),
my2DTolerances(TwoDTol),
my3DTolerances(ThreeDTol),
my1DTolOnFront(OneDTolFr),
my2DTolOnFront(TwoDTolFr),
my3DTolOnFront(ThreeDTolFr),
myFirstParInU(FirstInU),
myLastParInU(LastInU),
myFirstParInV(FirstInV),
myLastParInV(LastInV),
myFavoriteIso(FavorIso),
myContInU(ContInU),
myContInV(ContInV),
myPrecisionCode(PrecisCode),
myMaxDegInU(MaxDegInU),
myMaxDegInV(MaxDegInV),
myMaxPatches(MaxPatch),
myEvaluator(Func),
myDone(Standard_False),
myHasResult(Standard_False)
{
  myNumSubSpaces[0] = Num1DSS;
  myNumSubSpaces[1] = Num2DSS;
  myNumSubSpaces[2] = Num3DSS;
  Init();
  Perform(UChoice,VChoice,Func);
  ConvertBS();
}

//=======================================================================
//function : AdvApp2Var_ApproxAFunc2Var
//purpose  : 
//=======================================================================

 AdvApp2Var_ApproxAFunc2Var::
AdvApp2Var_ApproxAFunc2Var(const Standard_Integer Num1DSS,
			   const Standard_Integer Num2DSS,
			   const Standard_Integer Num3DSS,
			   const Handle(TColStd_HArray1OfReal)& OneDTol,
			   const Handle(TColStd_HArray1OfReal)& TwoDTol,
			   const Handle(TColStd_HArray1OfReal)& ThreeDTol,
			   const Handle(TColStd_HArray2OfReal)& OneDTolFr,
			   const Handle(TColStd_HArray2OfReal)& TwoDTolFr,
			   const Handle(TColStd_HArray2OfReal)& ThreeDTolFr,
			   const Standard_Real FirstInU,
			   const Standard_Real LastInU,
			   const Standard_Real FirstInV,
			   const Standard_Real LastInV,
			   const GeomAbs_IsoType FavorIso,
			   const GeomAbs_Shape ContInU,
			   const GeomAbs_Shape ContInV,
			   const Standard_Integer PrecisCode,
			   const Standard_Integer MaxDegInU,
			   const Standard_Integer MaxDegInV,
			   const Standard_Integer MaxPatch,
			   const AdvApp2Var_EvaluatorFunc2Var& Func,
			   const AdvApp2Var_Criterion& Crit,
			   AdvApprox_Cutting& UChoice,
			   AdvApprox_Cutting& VChoice) :
my1DTolerances(OneDTol),
my2DTolerances(TwoDTol),
my3DTolerances(ThreeDTol),
my1DTolOnFront(OneDTolFr),
my2DTolOnFront(TwoDTolFr),
my3DTolOnFront(ThreeDTolFr),
myFirstParInU(FirstInU),
myLastParInU(LastInU),
myFirstParInV(FirstInV),
myLastParInV(LastInV),
myFavoriteIso(FavorIso),
myContInU(ContInU),
myContInV(ContInV),
myPrecisionCode(PrecisCode),
myMaxDegInU(MaxDegInU),
myMaxDegInV(MaxDegInV),
myMaxPatches(MaxPatch),
myEvaluator(Func),
myDone(Standard_False),
myHasResult(Standard_False)
{
  myNumSubSpaces[0] = Num1DSS;
  myNumSubSpaces[1] = Num2DSS;
  myNumSubSpaces[2] = Num3DSS;
  Init();
  Perform(UChoice,VChoice,Func,Crit);
  ConvertBS();
}

//=======================================================================
//function : Init
//purpose  : Initialisation of the approximation
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Init()
{
  Standard_Integer ifav,iu=0,iv=0,ndu,ndv;
  switch (myFavoriteIso) {
  case GeomAbs_IsoU :
    ifav = 1;
    break;
  case GeomAbs_IsoV :
    ifav = 2;
    break;
  default :
    ifav = 2;
    break;
  }
  switch (myContInU) {
  case GeomAbs_C0 :
    iu = 0;
    break;
  case GeomAbs_C1 :
    iu = 1;
    break;
  case GeomAbs_C2 :
    iu = 2;
    break;
  default :
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : UContinuity Error");
  }
  switch (myContInV) {
  case GeomAbs_C0 :
    iv = 0;
    break;
  case GeomAbs_C1 :
    iv = 1;
    break;
  case GeomAbs_C2 :
    iv = 2;
    break;
  default :
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : VContinuity Error");
  }
  ndu = Max(myMaxDegInU+1,2*iu+2);
  ndv = Max(myMaxDegInV+1,2*iv+2);
  if (ndu<2*iu+2)
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : UMaxDegree Error");
  if (ndv<2*iv+2)
    Standard_ConstructionError::Raise("AdvApp2Var_ApproxAFunc2Var : VMaxDegree Error");
  myPrecisionCode = Max(0,Min(myPrecisionCode,3));
  AdvApp2Var_Context Conditions(ifav,iu,iv,ndu,ndv,
				myPrecisionCode,
				myNumSubSpaces[0],
                                myNumSubSpaces[1],
                                myNumSubSpaces[2],
                                my1DTolerances,
                                my2DTolerances,
                                my3DTolerances,
                                my1DTolOnFront,
                                my2DTolOnFront,
                                my3DTolOnFront);
  myConditions = Conditions;
  InitGrid(1);
}


//=======================================================================
//function : InitGrid
//purpose  : Initialisation of the approximation with regular cuttings
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::InitGrid(const Standard_Integer NbInt)
{
  Standard_Integer iu=myConditions.UOrder(),iv=myConditions.VOrder(),iint;

  AdvApp2Var_Patch M0(myFirstParInU,myLastParInU,myFirstParInV,myLastParInV,iu,iv);

  AdvApp2Var_SequenceOfPatch Net;
  Net.Append(M0);

  TColStd_SequenceOfReal TheU,TheV;
  TheU.Append(myFirstParInU);
  TheV.Append(myFirstParInV);
  TheU.Append(myLastParInU);
  TheV.Append(myLastParInV);

  AdvApp2Var_Network Result(Net,TheU,TheV);


  gp_XY UV1(myFirstParInU,myFirstParInV);
  AdvApp2Var_Node C1(UV1,iu,iv);
  gp_XY UV2(myLastParInU,myFirstParInV);
  AdvApp2Var_Node C2(UV2,iu,iv);
  gp_XY UV4(myLastParInU,myLastParInV);
  AdvApp2Var_Node C4(UV4,iu,iv);
  gp_XY UV3(myFirstParInU,myLastParInV);
  AdvApp2Var_Node C3(UV3,iu,iv);
  AdvApp2Var_SequenceOfNode Bag;
  Bag.Append(C1);
  Bag.Append(C2);
  Bag.Append(C3);
  Bag.Append(C4);

  AdvApp2Var_Iso V0(GeomAbs_IsoV,myFirstParInV,
		    myFirstParInU,myLastParInU,myFirstParInV,myLastParInV,
		    1,iu,iv);
  AdvApp2Var_Iso V1(GeomAbs_IsoV,myLastParInV,
		    myFirstParInU,myLastParInU,myFirstParInV,myLastParInV,
		    2,iu,iv);
  AdvApp2Var_Iso U0(GeomAbs_IsoU,myFirstParInU,
		    myFirstParInU,myLastParInU,myFirstParInV,myLastParInV,
		    3,iu,iv);
  AdvApp2Var_Iso U1(GeomAbs_IsoU,myLastParInU,
		    myFirstParInU,myLastParInU,myFirstParInV,myLastParInV,
		    4,iu,iv);

  AdvApp2Var_Strip BU0,BV0;
  BU0.Append(V0);
  BU0.Append(V1);
  BV0.Append(U0);
  BV0.Append(U1);

  AdvApp2Var_SequenceOfStrip UStrip,VStrip;
  UStrip.Append(BU0);
  VStrip.Append(BV0);

  AdvApp2Var_Framework Constraints(Bag,UStrip,VStrip);

// decoupes regulieres si NbInt>1
  Standard_Real deltu = (myLastParInU-myFirstParInU)/NbInt,
                deltv = (myLastParInV-myFirstParInV)/NbInt;
  for (iint=1;iint<=NbInt-1;iint++) {
    Result.UpdateInU(myFirstParInU+iint*deltu);
    Constraints.UpdateInU(myFirstParInU+iint*deltu);
    Result.UpdateInV(myFirstParInV+iint*deltv);
    Constraints.UpdateInV(myFirstParInV+iint*deltv);
  }
  myResult = Result;
  myConstraints = Constraints;
}

//=======================================================================
//function : Perform
//purpose  : Computation of the approximation
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Perform(const AdvApprox_Cutting& UChoice,
					 const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func)
{
  ComputePatches(UChoice,VChoice,Func);
  myHasResult = myDone = Standard_True;
  Compute3DErrors();
}

//=======================================================================
//function : Perform
//purpose  : Computation of the approximation
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Perform(const AdvApprox_Cutting& UChoice,
					 const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func,
					 const AdvApp2Var_Criterion& Crit)
{
  ComputePatches(UChoice,VChoice,Func,Crit);
  myHasResult = myDone = Standard_True;
  Compute3DErrors();
  ComputeCritError();
}

//=======================================================================
//function : ComputePatches
//purpose  : Computation of the polynomial approximations
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputePatches(const AdvApprox_Cutting& UChoice,
						const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func)
{
  Standard_Real Udec, Vdec;
  Standard_Boolean Umore, Vmore;
  Standard_Integer NbPatch, NbU, NbV, NumDec;
  Standard_Integer FirstNA;

  while (myResult.FirstNotApprox(FirstNA)) {

// completude de l'ensemble des contraintes 
    ComputeConstraints(UChoice, VChoice, Func);

// discretisation des contraintes relatives au carreau
    myResult(FirstNA).Discretise(myConditions,myConstraints,Func);
    if ( ! myResult(FirstNA).IsDiscretised() ) {
      myHasResult =  myDone = Standard_False;
      Standard_ConstructionError::Raise
              ("AdvApp2Var_ApproxAFunc2Var : Surface Discretisation Error");
    }

// calcul du nombre et du type de decoupes autorisees
// en fonction du nombre de carreaux max et de la validite des decoupes suiv.
    NbU = myResult.NbPatchInU();
    NbV = myResult.NbPatchInV();
    NbPatch = NbU*NbV;
    Umore = UChoice.Value(myResult(FirstNA).U0(), myResult(FirstNA).U1(),Udec);
    Vmore = VChoice.Value(myResult(FirstNA).V0(), myResult(FirstNA).V1(),Vdec);

    NumDec = 0;
    if ( ((NbPatch+NbV)<=myMaxPatches) && ((NbPatch+NbU)>myMaxPatches)
         && (Umore) ) NumDec = 1;
    if ( ((NbPatch+NbV)>myMaxPatches) && ((NbPatch+NbU)<=myMaxPatches) 
         && (Vmore) ) NumDec = 2;
    if ( ((NbPatch+NbV)<=myMaxPatches) && ((NbPatch+NbU)<=myMaxPatches) ) {
      if ( Umore ) NumDec = 3;
      if ( (NbV>NbU) && Vmore ) NumDec = 4;
    }
    if ( (NbU+1)*(NbV+1)<=myMaxPatches ) {
      if ( !Umore && !Vmore ) NumDec=0;
      if ( Umore && !Vmore ) NumDec=3;
      if ( !Umore && Vmore ) NumDec=4;
      if ( Umore && Vmore ) NumDec=5;
    }

// approximation du carreau
    myResult(FirstNA).MakeApprox(myConditions,myConstraints,NumDec);

    if ( ! myResult(FirstNA).IsApproximated() ) {
      switch (myResult(FirstNA).CutSense()) {
      case 0 :
//	On ne peut plus decouper : on garde le resultat
        if ( myResult(FirstNA).HasResult()) {
	  myResult(FirstNA).OverwriteApprox();
        }
        else {
          myHasResult =  myDone = Standard_False;
          Standard_ConstructionError::Raise
              ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
        }
	break;
      case 1 :
//      Il faut decouper en U
	myResult.UpdateInU(Udec);
	myConstraints.UpdateInU(Udec);
	break;
      case 2 :
//      Il faut decouper en V
	myResult.UpdateInV(Vdec);
	myConstraints.UpdateInV(Vdec);
	break;
      case 3 :
//      Il faut decouper en U et en V
	myResult.UpdateInU(Udec);
	myConstraints.UpdateInU(Udec);
	myResult.UpdateInV(Vdec);
	myConstraints.UpdateInV(Vdec);
	break;
      default :
        myHasResult =  myDone = Standard_False;
        Standard_ConstructionError::Raise
              ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
      }
    }
  }
}

//=======================================================================
//function : ComputePatches
//purpose  : Computation of the polynomial approximations
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputePatches(const AdvApprox_Cutting& UChoice,
					 const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func,
					 const AdvApp2Var_Criterion& Crit)
{
  Standard_Real Udec, Vdec, CritValue, m0=0., m1=0.;
  Standard_Boolean Umore, Vmore, CritAbs = (Crit.Type() == AdvApp2Var_Absolute);
  Standard_Integer NbPatch, NbU, NbV, NbInt, NumDec;
  Standard_Integer FirstNA, decision=0;

  while (myResult.FirstNotApprox(FirstNA)) {

// completude de l'ensemble des contraintes 
    ComputeConstraints(UChoice, VChoice, Func, Crit);
    if (decision>0) {
      m0 = m1;
      m1 = 0.;
    }

// discretisation des contraintes relatives au carreau
    myResult(FirstNA).Discretise(myConditions,myConstraints,Func);
    if ( ! myResult(FirstNA).IsDiscretised() ) {
      myHasResult =  myDone = Standard_False;
      Standard_ConstructionError::Raise
              ("AdvApp2Var_ApproxAFunc2Var : Surface Discretisation Error");
    }

// calcul du nombre et du type de decoupes autorisees
// en fonction du nombre de carreaux max et de la validite des decoupes suiv.
    NbU = myResult.NbPatchInU();
    NbV = myResult.NbPatchInV();
    NbPatch = NbU*NbV;
    NbInt = NbU;
    Umore = UChoice.Value(myResult(FirstNA).U0(), myResult(FirstNA).U1(),Udec);
    Vmore = VChoice.Value(myResult(FirstNA).V0(), myResult(FirstNA).V1(),Vdec);

    NumDec = 0;
    if ( ((NbPatch+NbV)<=myMaxPatches) && ((NbPatch+NbU)>myMaxPatches)
         && (Umore) ) NumDec = 1;
    if ( ((NbPatch+NbV)>myMaxPatches) && ((NbPatch+NbU)<=myMaxPatches) 
         && (Vmore) ) NumDec = 2;
    if ( ((NbPatch+NbV)<=myMaxPatches) && ((NbPatch+NbU)<=myMaxPatches) ) {
      if ( Umore ) NumDec = 3;
      if ( (NbV>NbU) && Vmore ) NumDec = 4;
    }
    if ( (NbU+1)*(NbV+1)<=myMaxPatches ) {
      if ( !Umore && !Vmore ) NumDec=0;
      if ( Umore && !Vmore ) NumDec=1;
      if ( !Umore && Vmore ) NumDec=2;
      if ( Umore && Vmore ) NumDec=5;
    }

// approximation du carreau
    if ( CritAbs ) {
      myResult(FirstNA).MakeApprox(myConditions,myConstraints,0);
    }
    else {
      myResult(FirstNA).MakeApprox(myConditions,myConstraints,NumDec);
    }
    if (NumDec>=3) NumDec = NumDec - 2;

// evaluation du critere sur le carreau
    if ( myResult(FirstNA).HasResult() ) {
      Crit.Value(myResult(FirstNA),myConditions);
      CritValue = myResult(FirstNA).CritValue();
      if (m1<CritValue) m1 = CritValue;
    }
// doit-on decouper ?
    decision = myResult(FirstNA).CutSense(Crit,NumDec);
    Standard_Boolean Regular = (Crit.Repartition() ==  AdvApp2Var_Regular);
//    Standard_Boolean Regular = Standard_True;
    if (Regular && decision>0) {
      NbInt++;
      InitGrid(NbInt);
    }
    else {
      switch (decision) {
      case 0 :
//	On ne peut plus decouper : on garde le resultat
	if ( myResult(FirstNA).HasResult() ) {
	  myResult(FirstNA).OverwriteApprox();
	}
	else {
	  myHasResult =  myDone = Standard_False;
	  Standard_ConstructionError::Raise
	    ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
	}
	break;
      case 1 :
//      Il faut decouper en U
	myResult.UpdateInU(Udec);
	myConstraints.UpdateInU(Udec);
	break;
      case 2 :
//      Il faut decouper en V
	myResult.UpdateInV(Vdec);
	myConstraints.UpdateInV(Vdec);
	break;
      case 3 :
//      Il faut decouper en U et en V
	myResult.UpdateInU(Udec);
	myConstraints.UpdateInU(Udec);
	myResult.UpdateInV(Vdec);
	myConstraints.UpdateInV(Vdec);
	break;
	default :
	myHasResult =  myDone = Standard_False;
	Standard_ConstructionError::Raise
	  ("AdvApp2Var_ApproxAFunc2Var : Surface Approximation Error");
      }
    }
  }
}

//=======================================================================
//function : ComputeConstraints without Criterion
//purpose  : Approximation of the constraints
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputeConstraints(const AdvApprox_Cutting& UChoice,
					 const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func)
{
  Standard_Real dec;
  Standard_Boolean more;
  Standard_Integer ind1, ind2, NbPatch, NbU, NbV;
  AdvApp2Var_Iso Is;
  Standard_Integer indN1, indN2;
  Standard_Integer iu = myConditions.UOrder(), iv = myConditions.VOrder();
  AdvApp2Var_Node N1(iu,iv), N2(iu,iv);

  while ( myConstraints.FirstNotApprox(ind1, ind2, Is) ) {

// approximation de l'iso et calcul des contraintes aux extremites
    indN1 = myConstraints.FirstNode(Is.Type(),ind1,ind2);
    N1 = myConstraints.Node(indN1);
    indN2 = myConstraints.LastNode(Is.Type(),ind1,ind2);
    N2 = myConstraints.Node(indN2);

    Is.MakeApprox(myConditions,
		  myFirstParInU, myLastParInU,
		  myFirstParInV, myLastParInV,
		  Func, N1 , N2);

    if (Is.IsApproximated()) {
// L'iso est approchee a la tolerance voulue
      myConstraints.ChangeIso(ind1,ind2,Is);
      myConstraints.ChangeNode(indN1) = N1;
      myConstraints.ChangeNode(indN2) = N2;
    }

    else {
// Pas d'approximation satisfaisante
      NbU = myResult.NbPatchInU();
      NbV = myResult.NbPatchInV();
      if (Is.Type()==GeomAbs_IsoV) {
	NbPatch = (NbU+1)*NbV;
	more = UChoice.Value(Is.T0(),Is.T1(),dec);
      }
      else {
	NbPatch = (NbV+1)*NbU;
	more = VChoice.Value(Is.T0(),Is.T1(),dec);
      }

      if (NbPatch<=myMaxPatches && more) {
//	On peut decouper l'iso
	if (Is.Type()==GeomAbs_IsoV) {
	  myResult.UpdateInU(dec);
	  myConstraints.UpdateInU(dec);
	}
	else {
	  myResult.UpdateInV(dec);
	  myConstraints.UpdateInV(dec);
	}
      }

      else {
//	On ne peut plus decouper : on garde le resultat
	if (Is.HasResult()) {
	  Is.OverwriteApprox();
	  myConstraints.ChangeIso(ind1,ind2,Is);
	  myConstraints.ChangeNode(indN1) = N1;
	  myConstraints.ChangeNode(indN2) = N2;	
	}
	else {
	  myHasResult =  myDone = Standard_False;
	  Standard_ConstructionError::Raise
	    ("AdvApp2Var_ApproxAFunc2Var : Curve Approximation Error");
	}
      }
    }
  }
}


//=======================================================================
//function : ComputeConstraints with Criterion
//purpose  : Approximation of the constraints
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputeConstraints(const AdvApprox_Cutting& UChoice,
					 const AdvApprox_Cutting& VChoice,
					 const AdvApp2Var_EvaluatorFunc2Var& Func,
					 const AdvApp2Var_Criterion& Crit)
{
  Standard_Real dec;
  Standard_Boolean more, CritRel = (Crit.Type() == AdvApp2Var_Relative);
  Standard_Integer ind1, ind2, NbPatch, NbU, NbV;
  AdvApp2Var_Iso Is;
  Standard_Integer indN1, indN2;
  Standard_Integer iu = myConditions.UOrder(), iv = myConditions.VOrder();
  AdvApp2Var_Node N1(iu,iv), N2(iu,iv);

    while ( myConstraints.FirstNotApprox(ind1, ind2, Is) ) {

// approximation de l'iso et calcul des contraintes aux extremites
      indN1 = myConstraints.FirstNode(Is.Type(),ind1,ind2);
      N1 = myConstraints.Node(indN1);
      indN2 = myConstraints.LastNode(Is.Type(),ind1,ind2);
      N2 = myConstraints.Node(indN2);

      Is.MakeApprox(myConditions,
                    myFirstParInU, myLastParInU,
                    myFirstParInV, myLastParInV,
                    Func, N1 , N2);

      if (Is.IsApproximated()) {
// L'iso est approchee a la tolerance voulue
	myConstraints.ChangeIso(ind1,ind2,Is);
	myConstraints.ChangeNode(indN1) = N1;
	myConstraints.ChangeNode(indN2) = N2;
      }

      else {
// Pas d'approximation satisfaisante
	NbU = myResult.NbPatchInU();
	NbV = myResult.NbPatchInV();
	if (Is.Type()==GeomAbs_IsoV) {
	  NbPatch = (NbU+1)*NbV;
	  more = UChoice.Value(Is.T0(),Is.T1(),dec);
	}
	else {
	  NbPatch = (NbV+1)*NbU;
	  more = VChoice.Value(Is.T0(),Is.T1(),dec);
	}

//      Pour forcer l'Overwrite si le critere est Absolu
	more = more && (CritRel);

	if (NbPatch<=myMaxPatches && more) {
//	On peut decouper l'iso
	  if (Is.Type()==GeomAbs_IsoV) {
	    myResult.UpdateInU(dec);
	    myConstraints.UpdateInU(dec);
	  }
	  else {
	    myResult.UpdateInV(dec);
	    myConstraints.UpdateInV(dec);
	  }
	}

	else {
//	On ne peut plus decouper : on garde le resultat
          if (Is.HasResult()) {
	    Is.OverwriteApprox();
	    myConstraints.ChangeIso(ind1,ind2,Is);
	    myConstraints.ChangeNode(indN1) = N1;
	    myConstraints.ChangeNode(indN2) = N2;	
          }
          else {
	    myHasResult =  myDone = Standard_False;
	    Standard_ConstructionError::Raise
              ("AdvApp2Var_ApproxAFunc2Var : Curve Approximation Error");
          }
	}
      }
    }
}

//=======================================================================
//function : Compute3DErrors
//purpose  : Computation of the 3D errors
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Compute3DErrors()
{

  Standard_Integer iesp,ipat;
  Standard_Real error_max,error_moy,error_U0,error_V0,error_U1,error_V1;
  Standard_Real Tol,F1Tol,F2Tol,F3Tol,F4Tol;
  if ( myNumSubSpaces[2] > 0 ) {
    my3DMaxError = new (TColStd_HArray1OfReal) (1,myNumSubSpaces[2]);
    my3DAverageError = new (TColStd_HArray1OfReal) (1,myNumSubSpaces[2]);
    my3DUFrontError = new (TColStd_HArray1OfReal) (1,myNumSubSpaces[2]);
    my3DVFrontError = new (TColStd_HArray1OfReal) (1,myNumSubSpaces[2]);
    for (iesp=1;iesp<=myNumSubSpaces[2];iesp++) {
      error_max = 0;
      error_moy = 0.;
      error_U0 = 0.;
      error_V0 = 0.;
      error_U1 = 0.;
      error_V1 = 0.;
      Tol = my3DTolerances->Value(iesp);
      F1Tol = my3DTolOnFront->Value(iesp,1);
      F2Tol = my3DTolOnFront->Value(iesp,2);
      F3Tol = my3DTolOnFront->Value(iesp,3);
      F4Tol = my3DTolOnFront->Value(iesp,4);
      for (ipat=1;ipat<=myResult.NbPatch();ipat++) {
	error_max = Max((myResult(ipat).MaxErrors())->Value(iesp),error_max);
	error_U0 = Max((myResult(ipat).IsoErrors())->Value(iesp,3),error_U0);
	error_U1 = Max((myResult(ipat).IsoErrors())->Value(iesp,4),error_U1);
	error_V0 = Max((myResult(ipat).IsoErrors())->Value(iesp,1),error_V0);
	error_V1 = Max((myResult(ipat).IsoErrors())->Value(iesp,2),error_V1);
	error_moy += (myResult(ipat).AverageErrors())->Value(iesp);
      }
      my3DMaxError->SetValue(iesp,error_max);
      my3DUFrontError->SetValue(iesp,Max(error_U0,error_U1));
      my3DVFrontError->SetValue(iesp,Max(error_V0,error_V1));
      error_moy /= (Standard_Real) myResult.NbPatch();
      my3DAverageError->SetValue(iesp,error_moy);
      if ( error_max>Tol 
	  || error_U0>F3Tol || error_U1>F4Tol 
	  || error_V0>F1Tol || error_V1>F2Tol) {
	myDone = Standard_False;
      }
    }
  }
}


//=======================================================================
//function : ComputeCritError
//purpose  : Computation of the max value of the Criterion
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ComputeCritError()
{

  Standard_Integer iesp,ipat;
  Standard_Real crit_max;
  if ( myNumSubSpaces[2] > 0 ) {
    for (iesp=1;iesp<=myNumSubSpaces[2];iesp++) {
      crit_max = 0.;
      for (ipat=1;ipat<=myResult.NbPatch();ipat++) {
	crit_max = Max((myResult(ipat).CritValue()),crit_max);
      }
      myCriterionError = crit_max;
    }
  }
}

//=======================================================================
//function : ConvertBS
//purpose  : Convertion of the approximation in BSpline Surface
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::ConvertBS()
{
 // Homogeneisation des degres
  Standard_Integer iu = myConditions.UOrder(), iv = myConditions.VOrder();
  Standard_Integer ncfu = myConditions.ULimit(), ncfv = myConditions.VLimit();
  myResult.SameDegree(iu,iv,ncfu,ncfv);
  myDegreeInU = ncfu - 1;
  myDegreeInV = ncfv - 1;

 // Calcul des surfaces resultats
  mySurfaces = new ( TColGeom_HArray1OfSurface) (1,  myNumSubSpaces[2]);

  Standard_Integer j;
  TColStd_Array1OfReal UKnots (1, myResult.NbPatchInU()+1);
  for (j=1; j<=UKnots.Length(); j++) { UKnots.SetValue(j, myResult.UParameter(j)); } 
 
  TColStd_Array1OfReal VKnots (1, myResult.NbPatchInV()+1);
  for (j=1; j<=VKnots.Length(); j++) { VKnots.SetValue(j, myResult.VParameter(j)); }

 // Preparation des donnees pour la conversion grille de polynomes --> poles
  Handle(TColStd_HArray1OfReal) Uint1 = 
    new (TColStd_HArray1OfReal) (1,2);
  Uint1->SetValue(1, -1);
  Uint1->SetValue(2, 1);
  Handle(TColStd_HArray1OfReal) Vint1 = 
    new (TColStd_HArray1OfReal) (1,2);
  Vint1->SetValue(1, -1);
  Vint1->SetValue(2, 1);

  Handle(TColStd_HArray1OfReal) Uint2 = 
    new (TColStd_HArray1OfReal) (1,myResult.NbPatchInU()+1);
  for (j=1; j<=Uint2->Length(); j++) { Uint2->SetValue(j, myResult.UParameter(j)); } 
  Handle(TColStd_HArray1OfReal) Vint2 = 
    new (TColStd_HArray1OfReal) (1,myResult.NbPatchInV()+1);
  for (j=1; j<=Vint2->Length(); j++) { Vint2->SetValue(j, myResult.VParameter(j)); } 

  Standard_Integer nmax = myResult.NbPatchInU()*myResult.NbPatchInV(),
                   Size_eq = myConditions.ULimit() * myConditions.VLimit() * 3;

  Handle(TColStd_HArray2OfInteger) NbCoeff = 
    new (TColStd_HArray2OfInteger) (1, nmax, 1, 2);
  Handle(TColStd_HArray1OfReal) Poly = 
    new (TColStd_HArray1OfReal) (1, nmax * Size_eq);

  Standard_Integer SSP, i;
  for (SSP=1; SSP <= myNumSubSpaces[2]; SSP++) { 

    // Creation de la grille de polynomes
    Standard_Integer n=0,icf=1,ieq;
    for (j=1; j<=myResult.NbPatchInV(); j++) {
      for (i=1; i<=myResult.NbPatchInU(); i++) {
	n++;
	NbCoeff->SetValue(n,1, myResult.Patch(i,j).NbCoeffInU());
	NbCoeff->SetValue(n,2, myResult.Patch(i,j).NbCoeffInV());
	for (ieq=1; ieq<=Size_eq;ieq++) {
	  Poly->SetValue(icf,(myResult.Patch(i,j).Coefficients(SSP,myConditions))
                                  ->Value(ieq));
	  icf++;
	}
      }
    }
  
    // Conversion en poles
    Convert_GridPolynomialToPoles CvP (myResult.NbPatchInU(),myResult.NbPatchInV(),
				       iu,iv,myMaxDegInU,myMaxDegInV,NbCoeff,
				       Poly,Uint1,Vint1,Uint2,Vint2);
    if ( !CvP.IsDone() ) { myDone = Standard_False; }
   
    // Conversion en BSpline
    mySurfaces->ChangeValue(SSP) = new (Geom_BSplineSurface) 
	( CvP.Poles()->Array2(),   
	  CvP.UKnots()->Array1(),  CvP.VKnots()->Array1(),
	  CvP.UMultiplicities()->Array1(), CvP.VMultiplicities()->Array1(),
	  CvP.UDegree(), CvP.VDegree() );
  }
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
 AdvApp2Var_ApproxAFunc2Var::MaxError(const Standard_Integer Dimension) const
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension <1 || Dimension >3) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::MaxError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DMaxError;
    break;
  case 2:
    EPtr = my2DMaxError;
    break;
  case 3:
    EPtr = my3DMaxError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
 AdvApp2Var_ApproxAFunc2Var::AverageError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension <1 || Dimension >3) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::AverageError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DAverageError;
    break;
  case 2:
    EPtr = my2DAverageError;
    break;
  case 3:
    EPtr = my3DAverageError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : UFrontError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
 AdvApp2Var_ApproxAFunc2Var::UFrontError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension <1 || Dimension >3) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::UFrontError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DUFrontError;
    break;
  case 2:
    EPtr = my2DUFrontError;
    break;
  case 3:
    EPtr = my3DUFrontError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : VFrontError
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal)
 AdvApp2Var_ApproxAFunc2Var::VFrontError(const Standard_Integer Dimension) const 
{
  Handle (TColStd_HArray1OfReal) EPtr;
  if (Dimension <=0 || Dimension >3) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::VFrontError : Dimension must be equal to 1,2 or 3 !");
  }
  switch (Dimension) {
  case 1:
    EPtr = my1DVFrontError;
    break;
  case 2:
    EPtr = my2DVFrontError;
    break;
  case 3:
    EPtr = my3DVFrontError;
    break;
  }
  return EPtr;
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

Standard_Real
 AdvApp2Var_ApproxAFunc2Var::MaxError(const Standard_Integer Dimension,
				      const Standard_Integer SSPIndex) const 
{
  if (Dimension !=3 || SSPIndex !=1) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::MaxError: ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = MaxError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : AverageError
//purpose  : 
//=======================================================================

Standard_Real
 AdvApp2Var_ApproxAFunc2Var::AverageError(const Standard_Integer Dimension,
					  const Standard_Integer SSPIndex) const 
{
  if (Dimension !=3 || SSPIndex !=1) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::AverageError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = AverageError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : UFrontError
//purpose  : 
//=======================================================================

Standard_Real
 AdvApp2Var_ApproxAFunc2Var::UFrontError(const Standard_Integer Dimension,
					 const Standard_Integer SSPIndex) const 
{
  if (Dimension !=3 || SSPIndex !=1) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::UFrontError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = UFrontError(Dimension);
  return EPtr->Value(SSPIndex);
}

//=======================================================================
//function : VFrontError
//purpose  : 
//=======================================================================

Standard_Real
 AdvApp2Var_ApproxAFunc2Var::VFrontError(const Standard_Integer Dimension,
					 const Standard_Integer SSPIndex) const 
{
  if (Dimension !=3 || SSPIndex !=1) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::VFrontError : ONE Surface 3D only !");
  }
  Handle (TColStd_HArray1OfReal) EPtr = VFrontError(Dimension);
  return EPtr->Value(SSPIndex);
}


//=======================================================================
//function : CritError
//purpose  : 
//=======================================================================

Standard_Real
 AdvApp2Var_ApproxAFunc2Var::CritError(const Standard_Integer Dimension,
				       const Standard_Integer SSPIndex) const 
{
  if (Dimension !=3 || SSPIndex !=1) {
    Standard_OutOfRange::Raise
      ("AdvApp2Var_ApproxAFunc2Var::CritError: ONE Surface 3D only !");
  }
  return myCriterionError;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void AdvApp2Var_ApproxAFunc2Var::Dump(Standard_OStream& o) const 
{
  Standard_Integer iesp=1,NbKU,NbKV,ik;
  o<<endl;
  if (!myHasResult) { o<<"No result"<<endl; }
  else {
    o<<"There is a result";
    if (myDone) {
      o<<" within the requested tolerance "<<my3DTolerances->Value(iesp)<<endl;
    }
    else if (my3DMaxError->Value(iesp)>my3DTolerances->Value(iesp)) {
      o<<" WITHOUT the requested tolerance "<<my3DTolerances->Value(iesp)<<endl;
    }
    else {
      o<<" WITHOUT the requested continuities "<<endl;
    }
    o<<endl;
    o<<"Result max error :"<<my3DMaxError->Value(iesp)<<endl;
    o<<"Result average error :"<<my3DAverageError->Value(iesp)<<endl;
    o<<"Result max error on U frontiers :"<<my3DUFrontError->Value(iesp)<<endl;
    o<<"Result max error on V frontiers :"<<my3DVFrontError->Value(iesp)<<endl;
    o<<endl;
    o<<"Degree of Bezier patches in U : "<<myDegreeInU
      <<"  in V : "<<myDegreeInV<<endl;
    o<<endl;
    Handle(Geom_BSplineSurface) S
      = Handle(Geom_BSplineSurface)::DownCast(mySurfaces->Value(iesp));
    o<<"Number of poles in U : "<<S->NbUPoles()
      <<"  in V : "<<S->NbVPoles()<<endl;
    o<<endl;
    NbKU = S->NbUKnots();
    NbKV = S->NbVKnots();
    o<<"Number of knots in U : "<<NbKU<<endl;
    for (ik=1;ik<=NbKU;ik++) {
      o<<"   "<<ik<<" : "<<S->UKnot(ik)<<"   mult : "<<S->UMultiplicity(ik)<<endl;
    }
    o<<endl;
    o<<"Number of knots in V : "<<NbKV<<endl;
    for (ik=1;ik<=NbKV;ik++) {
      o<<"   "<<ik<<" : "<<S->VKnot(ik)<<"   mult : "<<S->VMultiplicity(ik)<<endl;
    }
    o<<endl;
  }
}
