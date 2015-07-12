// Copyright (c) 1997-1999 Matra Datavision
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


#include <Adaptor3d_HSurface.hxx>
#include <AdvApp2Var_ApproxAFunc2Var.hxx>
#include <AdvApprox_PrefAndRec.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Real.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

class GeomConvert_ApproxSurface_Eval : public AdvApp2Var_EvaluatorFunc2Var
{

public:

  GeomConvert_ApproxSurface_Eval (const Handle(Adaptor3d_HSurface)& theAdaptor)
  : myAdaptor (theAdaptor) {}

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

  mutable Handle(Adaptor3d_HSurface) myAdaptor;

};


void GeomConvert_ApproxSurface_Eval::Evaluate (Standard_Integer * Dimension,
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
//  Standard_Integer idim;
  Standard_Integer jpar;
  Standard_Real Upar,Vpar;

// Dimension incorrecte
  if (*Dimension!=3) {
    *ErrorCode = 1;
  }

// Parametres incorrects
/* if (*FavorIso==1) {
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
 }*/

// Initialisation

  myAdaptor = myAdaptor->UTrim (UStartEnd[0], UStartEnd[1], Precision::PConfusion());
  myAdaptor = myAdaptor->VTrim (VStartEnd[0], VStartEnd[1], Precision::PConfusion());
/*
  for (idim=1;idim<=*Dimension;idim++) {
    for (jpar=1;jpar<=*NbParams;jpar++) {
      Result[idim-1+(jpar-1)*(*Dimension)] = 0.;
    }
  }*/
 

 Standard_Integer Order = *UOrder + *VOrder;
 gp_Pnt pnt;
 gp_Vec vect, v1, v2, v3, v4, v5, v6, v7, v8, v9;

 if (*FavorIso==1) {
  Upar = *ConstParam;
  switch (Order) {
  case 0 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	pnt = myAdaptor->Value (Upar, Vpar);
	Result[(jpar-1)*(*Dimension)] = pnt.X();
	Result[1+(jpar-1)*(*Dimension)] = pnt.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = pnt.Z();
    }
    break;
  case 1 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	myAdaptor->D1 (Upar, Vpar, pnt, v1, v2);
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
	myAdaptor->D2 (Upar, Vpar, pnt, v1, v2, v3, v4, v5);
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
  case 3 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	myAdaptor->D3 (Upar, Vpar, pnt, v1, v2, v3, v4, v5, v6, v7, v8, v9);
        if (*UOrder==2) {
	  Result[(jpar-1)*(*Dimension)] = v8.X();
	  Result[1+(jpar-1)*(*Dimension)] = v8.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v8.Z();
	}
	else if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v9.X();
	  Result[1+(jpar-1)*(*Dimension)] = v9.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v9.Z();
	}
    }
    break;
  case 4 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Vpar = Parameters[jpar-1];
	vect = myAdaptor->DN (Upar, Vpar, *UOrder, *VOrder);
	Result[(jpar-1)*(*Dimension)] = vect.X();
	Result[1+(jpar-1)*(*Dimension)] = vect.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = vect.Z();
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
	pnt = myAdaptor->Value (Upar, Vpar);
	Result[(jpar-1)*(*Dimension)] = pnt.X();
	Result[1+(jpar-1)*(*Dimension)] = pnt.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = pnt.Z();
    }
    break;
  case 1 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	myAdaptor->D1 (Upar, Vpar, pnt, v1, v2);
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
	myAdaptor->D2 (Upar, Vpar, pnt, v1, v2, v3, v4, v5);
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
  case 3 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	myAdaptor->D3 (Upar, Vpar, pnt, v1, v2, v3, v4, v5, v6, v7, v8, v9);
        if (*UOrder==2) {
	  Result[(jpar-1)*(*Dimension)] = v8.X();
	  Result[1+(jpar-1)*(*Dimension)] = v8.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v8.Z();
	}
	else if (*UOrder==1) {
	  Result[(jpar-1)*(*Dimension)] = v9.X();
	  Result[1+(jpar-1)*(*Dimension)] = v9.Y(); 
	  Result[2+(jpar-1)*(*Dimension)] = v9.Z();
	}
    }
    break;
  case 4 :
    for (jpar=1;jpar<=*NbParams;jpar++) {
	Upar = Parameters[jpar-1];
	vect = myAdaptor->DN (Upar, Vpar, *UOrder, *VOrder);
	Result[(jpar-1)*(*Dimension)] = vect.X();
	Result[1+(jpar-1)*(*Dimension)] = vect.Y(); 
	Result[2+(jpar-1)*(*Dimension)] = vect.Z();
    }
    break;
  }
 }    	

}


//=======================================================================
//function : GeomConvert_ApproxSurface
//purpose  : 
//=======================================================================

GeomConvert_ApproxSurface::GeomConvert_ApproxSurface(const Handle(Geom_Surface)& Surf,
                                                     const Standard_Real Tol3d,
                                                     const GeomAbs_Shape UContinuity,
                                                     const GeomAbs_Shape VContinuity,
                                                     const Standard_Integer MaxDegU,
                                                     const Standard_Integer MaxDegV,
                                                     const Standard_Integer MaxSegments,
                                                     const Standard_Integer PrecisCode)
{
  Handle(Adaptor3d_HSurface) aSurfAdaptor = new GeomAdaptor_HSurface (Surf);
  Approximate(aSurfAdaptor, Tol3d, UContinuity, VContinuity, MaxDegU, MaxDegV, MaxSegments, PrecisCode);
}

GeomConvert_ApproxSurface::GeomConvert_ApproxSurface(const Handle(Adaptor3d_HSurface)& Surf,
                                                     const Standard_Real Tol3d,
                                                     const GeomAbs_Shape UContinuity,
                                                     const GeomAbs_Shape VContinuity,
                                                     const Standard_Integer MaxDegU,
                                                     const Standard_Integer MaxDegV,
                                                     const Standard_Integer MaxSegments,
                                                     const Standard_Integer PrecisCode)
{
  Approximate(Surf, Tol3d, UContinuity, VContinuity, MaxDegU, MaxDegV, MaxSegments, PrecisCode);
}

void GeomConvert_ApproxSurface::Approximate(const Handle(Adaptor3d_HSurface)& theSurf,
                                            const Standard_Real theTol3d,
                                            const GeomAbs_Shape theUContinuity,
                                            const GeomAbs_Shape theVContinuity,
                                            const Standard_Integer theMaxDegU,
                                            const Standard_Integer theMaxDegV,
                                            const Standard_Integer theMaxSegments,
                                            const Standard_Integer thePrecisCode)
{
  Standard_Real U0 = theSurf->FirstUParameter();
  Standard_Real U1 = theSurf->LastUParameter();
  Standard_Real V0 = theSurf->FirstVParameter();
  Standard_Real V1 = theSurf->LastVParameter();

// " Init des nombres de sous-espaces et des tolerances"
  Standard_Integer nb1 = 0, nb2 = 0, nb3 = 1;
  Handle(TColStd_HArray1OfReal) nul1 = new TColStd_HArray1OfReal(1,1);
  nul1->SetValue(1, 0.);
  Handle(TColStd_HArray2OfReal) nul2 = new TColStd_HArray2OfReal(1,1,1,4);
  nul2->SetValue(1, 1, 0.);
  nul2->SetValue(1, 2, 0.);
  nul2->SetValue(1, 3, 0.);
  nul2->SetValue(1, 4, 0.);
  Handle(TColStd_HArray1OfReal) eps3D = new TColStd_HArray1OfReal(1,1);
  eps3D->SetValue(1, theTol3d);
  Handle(TColStd_HArray2OfReal) epsfr = new TColStd_HArray2OfReal(1,1,1,4);
  epsfr->SetValue(1, 1, theTol3d);
  epsfr->SetValue(1, 2, theTol3d);
  epsfr->SetValue(1, 3, theTol3d);
  epsfr->SetValue(1, 4, theTol3d);

// " Init du type d'iso"
  GeomAbs_IsoType IsoType = GeomAbs_IsoV; 
  Standard_Integer NbDec;

  NbDec = theSurf->NbUIntervals(GeomAbs_C2);
  TColStd_Array1OfReal UDec_C2(1, NbDec+1);
  theSurf->UIntervals(UDec_C2, GeomAbs_C2);
  NbDec = theSurf->NbVIntervals(GeomAbs_C2);
  TColStd_Array1OfReal VDec_C2(1, NbDec+1);
  theSurf->VIntervals(VDec_C2, GeomAbs_C2);

  NbDec = theSurf->NbUIntervals(GeomAbs_C3);
  TColStd_Array1OfReal UDec_C3(1, NbDec+1);
  theSurf->UIntervals(UDec_C3, GeomAbs_C3);

  NbDec = theSurf->NbVIntervals(GeomAbs_C3);
  TColStd_Array1OfReal VDec_C3(1, NbDec+1);
  theSurf->VIntervals(VDec_C3, GeomAbs_C3);
  // Approximation avec decoupe preferentiel 
  // aux lieux de discontinuitees C2
  AdvApprox_PrefAndRec pUDec(UDec_C2,UDec_C3);
  AdvApprox_PrefAndRec pVDec(VDec_C2,VDec_C3);

//POP pour WNT
  GeomConvert_ApproxSurface_Eval ev (theSurf);
  AdvApp2Var_ApproxAFunc2Var approx(nb1, nb2, nb3,
                                    nul1,nul1,eps3D,
                                    nul2,nul2,epsfr,
                                    U0,U1,V0,V1,
                                    IsoType,theUContinuity,theVContinuity,thePrecisCode,
                                    theMaxDegU,theMaxDegV,theMaxSegments,ev,
                                    pUDec,pVDec);

  myMaxError  = approx.MaxError(3,1);
  myBSplSurf  = approx.Surface(1);
  myIsDone    = approx.IsDone();
  myHasResult = approx.HasResult();
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

 Handle(Geom_BSplineSurface) GeomConvert_ApproxSurface::Surface() const
{
  return myBSplSurf;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

 Standard_Boolean GeomConvert_ApproxSurface::IsDone() const
{
  return myIsDone;
}

//=======================================================================
//function : HasResult
//purpose  : 
//=======================================================================

 Standard_Boolean GeomConvert_ApproxSurface::HasResult() const
{
  return myHasResult;
}

//=======================================================================
//function : MaxError
//purpose  : 
//=======================================================================

 Standard_Real GeomConvert_ApproxSurface::MaxError() const
{
  return myMaxError;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void GeomConvert_ApproxSurface::Dump(Standard_OStream& o) const 
{
  o<<endl;
  if (!myHasResult) { o<<"No result"<<endl; }
  else {
    o<<"Result max error :"<< myMaxError <<endl;
    }
  o<<endl;
}







