// Created on: 1996-03-05
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


#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApprox_Cutting.hxx>
#include <AdvApprox_DichoCutting.hxx>
#include <AdvApprox_PrefCutting.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PlateG0Criterion.hxx>
#include <GeomPlate_PlateG1Criterion.hxx>
#include <GeomPlate_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XY.hxx>
#include <Plate_Plate.hxx>
#include <PLib.hxx>
#include <TColgp_SequenceOfXY.hxx>
#include <TColgp_SequenceOfXYZ.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

class GeomPlate_MakeApprox_Eval : public AdvApp2Var_EvaluatorFunc2Var
{

public:

  GeomPlate_MakeApprox_Eval (const Handle(Geom_Surface)& theSurf)
  : mySurf (theSurf) {}

  virtual void Evaluate (Standard_Integer* theDimension,
                         Standard_Real*    theUStartEnd,
                         Standard_Real*    theVStartEnd,
                         Standard_Integer* theFavorIso,
                         Standard_Real*    theConstParam,
                         Standard_Integer* theNbParams,
                         Standard_Real*    theParameters,
                         Standard_Integer* theUOrder,
                         Standard_Integer* theVOrder,
                         Standard_Real*    theResult,
                         Standard_Integer* theErrorCode) const;

private:

  Handle(Geom_Surface) mySurf; 

};

void GeomPlate_MakeApprox_Eval::Evaluate (Standard_Integer * Dimension,
		    	        // Dimension
			        Standard_Real    * UStartEnd,
				// StartEnd[2] in U
				Standard_Real    * VStartEnd,
				// StartEnd[2] in V
				Standard_Integer * FavorIso,
				// Choice of constante, 1 for U, 2 for V
				Standard_Real    * ConstParam,
				// Value of constant parameter
				Standard_Integer * NbParams,
				// Number of parameters N
				Standard_Real    * Parameters,
				// Values of parameters,
				Standard_Integer * UOrder,
				// Derivative Request in U
				Standard_Integer * VOrder,
				// Derivative Request in V
				Standard_Real    * Result, 
				// Result[Dimension,N]
				Standard_Integer * ErrorCode) const
                        	// Error Code
{
  *ErrorCode = 0;
  Standard_Integer idim,jpar;
  Standard_Real Upar,Vpar;

// Dimension incorrecte
  if (*Dimension!=3) {
    *ErrorCode = 1;
  }

// Parametres incorrects
 if (*FavorIso==1) {
    Upar = *ConstParam;
    if (( Upar < UStartEnd[0] ) || ( Upar > UStartEnd[1] )) {
      *ErrorCode = 2;
    }
    for (jpar=1;jpar<=*NbParams;jpar++) {
      Vpar = Parameters[jpar-1];
      if (( Vpar < VStartEnd[0] ) || ( Vpar > VStartEnd[1] )) {
        *ErrorCode = 2;
      }
    }
 }
 else {
    Vpar = *ConstParam;
    if (( Vpar < VStartEnd[0] ) || ( Vpar > VStartEnd[1] )) {
      *ErrorCode = 2;
    }
    for (jpar=1;jpar<=*NbParams;jpar++) {
      Upar = Parameters[jpar-1];
      if (( Upar < UStartEnd[0] ) || ( Upar > UStartEnd[1] )) {
        *ErrorCode = 2;
      }
    }
 }

// Initialisation
  for (idim=1;idim<=*Dimension;idim++) {
    for (jpar=1;jpar<=*NbParams;jpar++) {
      Result[idim-1+(jpar-1)*(*Dimension)] = 0.;
    }
  }
 

 Standard_Integer Order = *UOrder + *VOrder;
 gp_Pnt pnt;
// gp_Vec vect, v1, v2, v3, v4, v5, v6, v7, v8, v9;
 gp_Vec v1, v2, v3, v4, v5;

 if (*FavorIso==1) {
  Upar = *ConstParam;
  switch (Order) {
  case 0 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	pnt = mySurf->Value (Upar, Vpar);
	Result[(jpar-1)*(*Dimension)] = pnt.X();
	Result[1+(jpar-1)*(*Dimension)] = pnt.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = pnt.Z();
    }
    break;
  case 1 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	mySurf->D1 (Upar, Vpar, pnt, v1, v2);
        if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v1.X();
	  Result[1+(jpar-1)*(*Dimension)] = v1.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v1.Z();
	}
	else {
	  Result[(jpar-1)*(*Dimension)] = v2.X();
	  Result[1+(jpar-1)*(*Dimension)] = v2.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v2.Z();
	}
    }
    break;
  case 2 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	mySurf->D2 (Upar, Vpar, pnt, v1, v2, v3, v4, v5);
        if (*UOrder==2) {
	  Result[(jpar-1)*(*Dimension)] = v3.X();
	  Result[1+(jpar-1)*(*Dimension)] = v3.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v3.Z();
	}
	else if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v5.X();
	  Result[1+(jpar-1)*(*Dimension)] = v5.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v5.Z();
	}
	else if (*UOrder==0) {
	  Result[(jpar-1)*(*Dimension)] = v4.X();
	  Result[1+(jpar-1)*(*Dimension)] = v4.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v4.Z();
	}
    }
    break;
  }
 }
 else { 
  Vpar = *ConstParam;
  switch (Order) {
  case 0 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	pnt = mySurf->Value (Upar, Vpar);
	Result[(jpar-1)*(*Dimension)] = pnt.X();
	Result[1+(jpar-1)*(*Dimension)] = pnt.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = pnt.Z();
    }
    break;
  case 1 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	mySurf->D1 (Upar, Vpar, pnt, v1, v2);
        if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v1.X();
	  Result[1+(jpar-1)*(*Dimension)] = v1.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v1.Z();
	}
	else {
	  Result[(jpar-1)*(*Dimension)] = v2.X();
	  Result[1+(jpar-1)*(*Dimension)] = v2.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v2.Z();
	}
    }
    break;
  case 2 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	mySurf->D2 (Upar, Vpar, pnt, v1, v2, v3, v4, v5);
        if (*UOrder==2) {
	  Result[(jpar-1)*(*Dimension)] = v3.X();
	  Result[1+(jpar-1)*(*Dimension)] = v3.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v3.Z();
	}
	else if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v5.X();
	  Result[1+(jpar-1)*(*Dimension)] = v5.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v5.Z();
	}
	else if (*UOrder==0) {
	  Result[(jpar-1)*(*Dimension)] = v4.X();
	  Result[1+(jpar-1)*(*Dimension)] = v4.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v4.Z();
	}
    }
    break;
  }
 }    	

}


//=======================================================================
//function : GeomPlate_MakeApprox
//purpose  : 
//=======================================================================

GeomPlate_MakeApprox::GeomPlate_MakeApprox(const Handle(GeomPlate_Surface)& SurfPlate,
					   const AdvApp2Var_Criterion& PlateCrit,
                                           const Standard_Real Tol3d,
					   const Standard_Integer Nbmax,
					   const Standard_Integer dgmax,
					   const GeomAbs_Shape Continuity,
					   const Standard_Real EnlargeCoeff)
{
  myPlate = SurfPlate;

  Standard_Real U0=0., U1=0., V0=0., V1=0.;
  myPlate->RealBounds(U0, U1, V0, V1);
  U0 = EnlargeCoeff * U0;
  U1 = EnlargeCoeff * U1;
  V0 = EnlargeCoeff * V0;
  V1 = EnlargeCoeff * V1;

  Standard_Integer nb1 = 0, nb2 = 0, nb3 = 1;
  Handle(TColStd_HArray1OfReal) nul1 =
  		 new TColStd_HArray1OfReal(1,1);
  nul1->Init(0.);
  Handle(TColStd_HArray2OfReal) nul2 =
  		 new TColStd_HArray2OfReal(1,1,1,4);
  nul2->Init(0.);
  Handle(TColStd_HArray1OfReal) eps3D =
  		 new TColStd_HArray1OfReal(1,1);
  eps3D->Init(Tol3d);
  Handle(TColStd_HArray2OfReal) epsfr =
  		 new TColStd_HArray2OfReal(1,1,1,4);
  epsfr->Init(Tol3d);
  GeomAbs_IsoType myType = GeomAbs_IsoV;
  Standard_Integer myPrec = 0;

  AdvApprox_DichoCutting myDec;

//POP pour WNT
  GeomPlate_MakeApprox_Eval ev (myPlate);
  AdvApp2Var_ApproxAFunc2Var AppPlate(nb1, nb2, nb3,
			              nul1,nul1,eps3D,
				      nul2,nul2,epsfr,
				      U0,U1,V0,V1,
				      myType,
				      Continuity, Continuity,
				      myPrec, 
				      dgmax,dgmax,Nbmax,ev,
//				      dgmax,dgmax,Nbmax,myPlateSurfEval,
				      PlateCrit,myDec,myDec);
  mySurface = AppPlate.Surface(1);
  myAppError = AppPlate.MaxError(3,1);
  myCritError = AppPlate.CritError(3,1);
#ifdef OCCT_DEBUG
  cout<<"Approximation results"<<endl;
  cout<<"  Approximation error : "<<myAppError<<endl;
  cout<<"  Criterium error : "<<myCritError<<endl;
#endif
}


//=======================================================================
//function : GeomPlate_MakeApprox
//purpose  : 
//=======================================================================

GeomPlate_MakeApprox::GeomPlate_MakeApprox(const Handle(GeomPlate_Surface)& SurfPlate,
					   const Standard_Real Tol3d,
					   const Standard_Integer Nbmax,
					   const Standard_Integer dgmax,
					   const Standard_Real dmax,
					   const Standard_Integer CritOrder,
					   const GeomAbs_Shape Continuity,
					   const Standard_Real EnlargeCoeff)
{
  myPlate = SurfPlate;

  TColgp_SequenceOfXY Seq2d;
  TColgp_SequenceOfXYZ Seq3d;

  if (CritOrder>=0) {

//    contraintes 2d d'ordre 0
    myPlate->Constraints(Seq2d);

//    contraintes 3d correspondantes sur plate
    Standard_Integer i,nbp=Seq2d.Length();
    for(i=1;i<=nbp;i++){
      gp_XY P2d=Seq2d.Value(i);
      gp_Pnt PP;
      gp_Vec v1h,v2h,v3h;
      if (CritOrder==0) {
//    a l'ordre 0
	myPlate->D0 (P2d.X(), P2d.Y(), PP);
	gp_XYZ P3d(PP.X(),PP.Y(),PP.Z());
	Seq3d.Append(P3d);
      }
      else {
//    a l'ordre 1
	myPlate->D1 (P2d.X(), P2d.Y(), PP, v1h, v2h);
	v3h=v1h^v2h;
	gp_XYZ P3d(v3h.X(),v3h.Y(),v3h.Z());
	Seq3d.Append(P3d);
      }
    }
  }

  Standard_Real U0=0., U1=0., V0=0., V1=0.;
  myPlate->RealBounds(U0, U1, V0, V1);
  U0 = EnlargeCoeff * U0;
  U1 = EnlargeCoeff * U1;
  V0 = EnlargeCoeff * V0;
  V1 = EnlargeCoeff * V1;

  Standard_Real seuil = Tol3d;
  if (CritOrder==0&&Tol3d<10*dmax) {
    seuil=10*dmax;
#ifdef OCCT_DEBUG
    cout<<"Seuil G0 choisi trop faible par rapport au contour. On prend "<<seuil<<endl;
#endif
  }
  if (CritOrder==1&&Tol3d<10*dmax) {
    seuil=10*dmax;
#ifdef OCCT_DEBUG
    cout<<"Seuil G1 choisi trop faible par rapport au contour. On prend "<<seuil<<endl;
#endif
  }
  Standard_Integer nb1 = 0, nb2 = 0, nb3 = 1;
  Handle(TColStd_HArray1OfReal) nul1 =
  		 new TColStd_HArray1OfReal(1,1);
  nul1->Init(0.);
  Handle(TColStd_HArray2OfReal) nul2 =
  		 new TColStd_HArray2OfReal(1,1,1,4);
  nul2->Init(0.);
  Handle(TColStd_HArray1OfReal) eps3D =
  		 new TColStd_HArray1OfReal(1,1);
  eps3D->Init(Tol3d);
  Handle(TColStd_HArray2OfReal) epsfr =
  		 new TColStd_HArray2OfReal(1,1,1,4);
  epsfr->Init(Tol3d);

  GeomAbs_IsoType myType = GeomAbs_IsoV;
  Standard_Integer myPrec = 0;

  AdvApprox_DichoCutting myDec;

  if (CritOrder==-1) {
    myPrec = 1;
// POP pour NT
    GeomPlate_MakeApprox_Eval ev (myPlate);
    AdvApp2Var_ApproxAFunc2Var AppPlate(nb1, nb2, nb3,
					nul1,nul1,eps3D,
					nul2,nul2,epsfr,
					U0,U1,V0,V1,
					myType,
					Continuity, Continuity,
					myPrec,
					dgmax,dgmax,Nbmax,ev,
					myDec,myDec);
    mySurface = AppPlate.Surface(1);
    myAppError = AppPlate.MaxError(3,1);
    myCritError = 0.;
#ifdef OCCT_DEBUG
    cout<<"Approximation results"<<endl;
    cout<<"  Approximation error : "<<myAppError<<endl;
#endif
  }
  else if (CritOrder==0) {
    GeomPlate_PlateG0Criterion Crit0(Seq2d,Seq3d,seuil);
// POP pour NT
    GeomPlate_MakeApprox_Eval ev (myPlate);
    AdvApp2Var_ApproxAFunc2Var AppPlate(nb1, nb2, nb3,
					nul1,nul1,eps3D,
					nul2,nul2,epsfr,
					U0,U1,V0,V1,
					myType,
					Continuity, Continuity,
					myPrec,
					dgmax,dgmax,Nbmax,ev,
//					dgmax,dgmax,Nbmax,myPlateSurfEval,
					Crit0,myDec,myDec);
    mySurface = AppPlate.Surface(1);
    myAppError = AppPlate.MaxError(3,1);
    myCritError = AppPlate.CritError(3,1);
#ifdef OCCT_DEBUG
    cout<<"Approximation results"<<endl;
    cout<<"  Approximation error : "<<myAppError<<endl;
    cout<<"  Criterium error : "<<myCritError<<endl;
#endif
  }
  else if (CritOrder==1) {
    GeomPlate_PlateG1Criterion Crit1(Seq2d,Seq3d,seuil);
// POP pour NT
    GeomPlate_MakeApprox_Eval ev (myPlate);
    AdvApp2Var_ApproxAFunc2Var AppPlate(nb1, nb2, nb3,
					nul1,nul1,eps3D,
					nul2,nul2,epsfr,
					U0,U1,V0,V1,
					myType,
					Continuity, Continuity,
					myPrec,
					dgmax,dgmax,Nbmax,ev,
//					dgmax,dgmax,Nbmax,myPlateSurfEval,
					Crit1,myDec,myDec);
    mySurface = AppPlate.Surface(1);
    myAppError = AppPlate.MaxError(3,1);
    myCritError = AppPlate.CritError(3,1);
#ifdef OCCT_DEBUG
    cout<<"Approximation results"<<endl;
    cout<<"  Approximation error : "<<myAppError<<endl;
    cout<<"  Criterium error : "<<myCritError<<endl;
#endif
  }
}



//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomPlate_MakeApprox::Surface() const
{
  return mySurface;
}


//=======================================================================
//function : ApproxError
//purpose  : 
//=======================================================================

Standard_Real GeomPlate_MakeApprox::ApproxError() const
{
  return myAppError;
}


//=======================================================================
//function : CriterionError
//purpose  : 
//=======================================================================

Standard_Real GeomPlate_MakeApprox::CriterionError() const
{
  return myCritError;
}
