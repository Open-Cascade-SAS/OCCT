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
#include <Standard_ConstructionError.hxx>

// Calculaton of parameters
static Standard_Boolean lesparam(const Standard_Integer iordre,
				 const Standard_Integer ncflim, 
				 const Standard_Integer icodeo, 
				 Standard_Integer& nbpnts, 
				 Standard_Integer& ndgjac)
{
  // jacobi degree
  ndgjac = ncflim; // it always keeps a reserve coefficient
  if (icodeo< 0) return Standard_False;
  if (icodeo > 0) {
    ndgjac += (9 - (iordre+1)); //iordre rescales the frequences upwards
    ndgjac += (icodeo-1)*10;
  }
  // ---> Min Number of required pointss.
  if (ndgjac < 8) { nbpnts = 8;  } 
  else if (ndgjac < 10) { nbpnts = 10; }
  //  else if (ndgjac < 15) { nbpnt = 15; } Bug Uneven number
  else if (ndgjac < 20) { nbpnts = 20;}
  //  else if (ndgjac < 25) { nbpnt = 25; } Bug Uneven number
  else if (ndgjac < 30) { nbpnts = 30;}
  else if (ndgjac < 40) { nbpnts = 40;}
  else if (ndgjac < 50) { nbpnts = 50;}
  //  else if (*ndgjac < 61) { nbpnt = 61;} Bug Uneven number
  else {
    nbpnts = 50;
#ifdef OCCT_DEBUG
    cout << "F(U, V) : Not enough points of discretization" << endl; 
#endif
  }

  // If constraints are on borders, this adds 2 points
  if (iordre>-1) {  nbpnts += 2;}

  return Standard_True;
}
//============================================================================
//function : AdvApp2Var_Context
//purpose  :
//============================================================================

 AdvApp2Var_Context::
AdvApp2Var_Context()
{
}

//============================================================================
//function : AdvApp2Var_Context
//purpose  :
//============================================================================

 AdvApp2Var_Context::
AdvApp2Var_Context(const Standard_Integer ifav,
		   const Standard_Integer iu,
		   const Standard_Integer iv, 
		   const Standard_Integer nlimu,
		   const Standard_Integer nlimv, 
		   const Standard_Integer iprecis,
		   const Standard_Integer nb1Dss,
		   const Standard_Integer nb2Dss,
		   const Standard_Integer nb3Dss,
		   const Handle(TColStd_HArray1OfReal)& tol1D,
		   const Handle(TColStd_HArray1OfReal)& tol2D,
		   const Handle(TColStd_HArray1OfReal)& tol3D,
		   const Handle(TColStd_HArray2OfReal)& tof1D,
		   const Handle(TColStd_HArray2OfReal)& tof2D,
		   const Handle(TColStd_HArray2OfReal)& tof3D) :
myFav(ifav),
myOrdU(iu),
myOrdV(iv),
myLimU(nlimu),
myLimV(nlimv),
myNb1DSS(nb1Dss),
myNb2DSS(nb2Dss),
myNb3DSS(nb3Dss)
{
Standard_Integer ErrorCode=0,NbPntU=0,JDegU=0,NbPntV=0,JDegV=0;
Standard_Integer ncfl;

// myNbURoot,myJDegU
ncfl = nlimu;
if (ncfl<2*iu+2) ncfl = 2*iu+2;
if (!lesparam(iu,ncfl,iprecis,NbPntU,JDegU) )
  { throw Standard_ConstructionError("AdvApp2Var_Context");}
myNbURoot = NbPntU;
myJDegU = JDegU;
if (iu>-1) NbPntU = myNbURoot - 2;

// myJMaxU
Standard_Integer i,j,size = JDegU-2*iu-1;
Handle (TColStd_HArray1OfReal) JMaxU =
  new TColStd_HArray1OfReal(1,size);
Standard_Real *JU_array =
  (Standard_Real *) &JMaxU->ChangeArray1()(JMaxU->Lower());
AdvApp2Var_ApproxF2var::mma2jmx_(&JDegU,(integer *)&iu,JU_array);
myJMaxU = JMaxU;

// myNbVRoot,myJDegV
ncfl = nlimv;
if (ncfl<2*iv+2) ncfl = 2*iv+2;
//Ma1nbp(&iv,&ncfl,&iprec,&NbPntV,&JDegV,&ErrorCode);
if (!lesparam(iv, ncfl, iprecis, NbPntV, JDegV) )
  { throw Standard_ConstructionError("AdvApp2Var_Context");}
myNbVRoot = NbPntV;
myJDegV = JDegV;
if (iv>-1) NbPntV = myNbVRoot - 2;

// myJMaxV
size = JDegV-2*iv-1;
Handle (TColStd_HArray1OfReal) JMaxV =
  new TColStd_HArray1OfReal(1,size);
Standard_Real *JV_array =
  (Standard_Real *) &JMaxV->ChangeArray1()(JMaxV->Lower());
AdvApp2Var_ApproxF2var::mma2jmx_(&JDegV,(integer *)&iv,JV_array);
myJMaxV = JMaxV;

// myURoots, myVRoots
Handle (TColStd_HArray1OfReal) URoots =
  new TColStd_HArray1OfReal(1,myNbURoot);
Standard_Real *U_array =
  (Standard_Real *) &URoots->ChangeArray1()(URoots->Lower());
Handle (TColStd_HArray1OfReal) VRoots =
  new TColStd_HArray1OfReal(1,myNbVRoot);
Standard_Real *V_array =
  (Standard_Real *) &VRoots->ChangeArray1()(VRoots->Lower());
AdvApp2Var_ApproxF2var::mma2roo_(&NbPntU,&NbPntV,U_array,V_array);
myURoots = URoots;
myVRoots = VRoots;

// myUGauss
size = (NbPntU/2+1)*(myJDegU-2*iu-1);
Handle (TColStd_HArray1OfReal) UGauss =
  new TColStd_HArray1OfReal(1,size);
Standard_Real *UG_array =
  (Standard_Real *) &UGauss->ChangeArray1()(UGauss->Lower());
AdvApp2Var_ApproxF2var::mmapptt_(&JDegU,&NbPntU,&iu,UG_array,&ErrorCode);
if (ErrorCode != 0 ) {
  throw Standard_ConstructionError("AdvApp2Var_Context : Error in FORTRAN");
}
myUGauss = UGauss;

// myVGauss
size = (NbPntV/2+1)*(myJDegV-2*iv-1);
Handle (TColStd_HArray1OfReal) VGauss =
  new TColStd_HArray1OfReal(1,size);
Standard_Real *VG_array =
  (Standard_Real *) &VGauss->ChangeArray1()(VGauss->Lower());
AdvApp2Var_ApproxF2var::mmapptt_(&JDegV,&NbPntV,&iv,VG_array,&ErrorCode);
if (ErrorCode != 0 ) {
  throw Standard_ConstructionError("AdvApp2Var_Context : Error in FORTRAN");
}
myVGauss = VGauss;

//myInternalTol, myFrontierTol, myCuttingTol
Standard_Integer nbss = nb1Dss + nb2Dss + nb3Dss;
Handle (TColStd_HArray1OfReal) ITol =
  new TColStd_HArray1OfReal(1,nbss);
for (i=1;i<=nb1Dss;i++) {
  ITol->SetValue(i,tol1D->Value(i));
}
for (i=1;i<=nb2Dss;i++) {
  ITol->SetValue(i+nb1Dss,tol2D->Value(i));
}
for (i=1;i<=nb3Dss;i++) {
  ITol->SetValue(i+nb1Dss+nb2Dss,tol3D->Value(i));
}
if (iu>-1||iv>-1) {
  for (i=1;i<=nbss;i++) { ITol->SetValue(i,ITol->Value(i)/2); }
}
Handle (TColStd_HArray2OfReal) FTol =
  new TColStd_HArray2OfReal(1,nbss,1,4);
Handle (TColStd_HArray2OfReal) CTol =
  new TColStd_HArray2OfReal(1,nbss,1,4);
for (i=1;i<=nb1Dss;i++) {
for (j=1;j<=4;j++) {
  FTol->SetValue(i,j,tof1D->Value(i,j));
  CTol->SetValue(i,j,0);
}
}
for (i=1;i<=nb2Dss;i++) {
for (j=1;j<=4;j++) {
  FTol->SetValue(nb1Dss+i,j,tof2D->Value(i,j));
  CTol->SetValue(nb1Dss+i,j,0);
}
}
for (i=1;i<=nb3Dss;i++) {
for (j=1;j<=4;j++) {
  FTol->SetValue(nb1Dss+nb2Dss+i,j,tof3D->Value(i,j));
  CTol->SetValue(nb1Dss+nb2Dss+i,j,0);
}
}
if (iu>-1||iv>-1) {
  Standard_Real tolmin, poids, hmax[4];
  hmax[0] = 0;
  hmax[1] = 1;
  hmax[2] = 1.5;
  hmax[3] = 1.75;
  poids = hmax[iu+1]*hmax[iv+1] + hmax[iu+1] + hmax[iv+1];
  for (i=1;i<=nbss;i++) {
    for (j=1;j<=4;j++) {
      tolmin = (ITol->Value(i))/poids;
      if (tolmin<FTol->Value(i,j)) FTol->SetValue(i,j,tolmin);
      CTol->SetValue(i,j,tolmin);
    }
  }
}
myInternalTol = ITol;
myFrontierTol = FTol;
myCuttingTol = CTol;
}

//============================================================================
//function : TotalDimension
//purpose  : 
//============================================================================

Standard_Integer AdvApp2Var_Context::TotalDimension() const 
{
  return myNb1DSS + 2*myNb2DSS + 3*myNb3DSS; 
}

//============================================================================
//function : TotalNumberSSP
//purpose  :
//============================================================================

Standard_Integer AdvApp2Var_Context::TotalNumberSSP() const 
{
  return myNb1DSS + myNb2DSS + myNb3DSS; 
}

//============================================================================
//function : FavorIso
//purpose  : return 1 for IsoU, 2 for IsoV, 2 by default
//============================================================================

Standard_Integer AdvApp2Var_Context::FavorIso() const 
{
  return myFav; 
}

//============================================================================
//function : UOrder
//purpose  : return the order of continuity requested in U
//============================================================================

Standard_Integer AdvApp2Var_Context::UOrder() const 
{
  return myOrdU; 
}

//============================================================================
//function : VOrder
//purpose  : return the order of continuity requested in V
//============================================================================

Standard_Integer AdvApp2Var_Context::VOrder() const 
{
  return myOrdV; 
}

//============================================================================
//function : ULimit
//purpose  : return the max number of coeff. in U of the polynomial approx.
//============================================================================

Standard_Integer AdvApp2Var_Context::ULimit() const 
{
  return myLimU; 
}

//============================================================================
//function : VLimit
//purpose  : return the max number of coeff. in V of the polynomial approx.
//============================================================================

Standard_Integer AdvApp2Var_Context::VLimit() const 
{
  return myLimV;
}

//============================================================================
//function : UJacDeg
//purpose  : return the max degree of the Jacobi functions for U parameter
//============================================================================

Standard_Integer AdvApp2Var_Context::UJacDeg() const 
{
  return myJDegU; 
}

//============================================================================
//function : VJacDeg
//purpose  : return the max degree of the Jacobi functions for V parameter
//============================================================================

Standard_Integer AdvApp2Var_Context::VJacDeg() const 
{
  return myJDegV; 
}

//============================================================================
//function : UJacMax
//purpose  : return the max value of the Jacobi functions for U parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::UJacMax() const 
{
  return myJMaxU;
}

//============================================================================
//function : VJacMax
//purpose  : return the max value of the Jacobi functions for V parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::VJacMax() const 
{
  return myJMaxV;
}

//============================================================================
//function : URoots
//purpose  : return Legendre roots for U parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::URoots() const 
{
  return myURoots;
}

//============================================================================
//function : VRoots
//purpose  : return Legendre roots for V parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::VRoots() const 
{
  return myVRoots;
}

//============================================================================
//function : UGauss
//purpose  : return Gauss roots for U parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::UGauss() const 
{
  return myUGauss;
}

//============================================================================
//function : VGauss
//purpose  : return Gauss roots for V parameter
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::VGauss() const 
{
  return myVGauss;
}

//============================================================================
//function : IToler
//purpose  : return tolerances for the approximation of patches
//============================================================================

Handle(TColStd_HArray1OfReal) AdvApp2Var_Context::IToler() const 
{
  return myInternalTol;
}

//============================================================================
//function : FToler
//purpose  : return tolerances for the approximation of frontiers
//============================================================================

Handle(TColStd_HArray2OfReal) AdvApp2Var_Context::FToler() const 
{
  return myFrontierTol;
}

//============================================================================
//function : CToler
//purpose  : return tolerances for the approximation of cutting lines
//============================================================================

Handle(TColStd_HArray2OfReal) AdvApp2Var_Context::CToler() const 
{
  return myCuttingTol;
}

