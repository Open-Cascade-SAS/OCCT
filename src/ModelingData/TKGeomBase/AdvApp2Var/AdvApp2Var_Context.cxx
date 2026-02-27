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

// Calculation of parameters
static bool lesparam(const int iordre, const int ncflim, const int icodeo, int& nbpnts, int& ndgjac)
{
  // jacobi degree
  ndgjac = ncflim; // it always keeps a reserve coefficient
  if (icodeo < 0)
    return false;
  if (icodeo > 0)
  {
    ndgjac += (9 - (iordre + 1)); // iordre rescales the frequences upwards
    ndgjac += (icodeo - 1) * 10;
  }
  // ---> Min Number of required points.
  if (ndgjac < 8)
  {
    nbpnts = 8;
  }
  else if (ndgjac < 10)
  {
    nbpnts = 10;
  }
  //  else if (ndgjac < 15) { nbpnt = 15; } Bug Uneven number
  else if (ndgjac < 20)
  {
    nbpnts = 20;
  }
  //  else if (ndgjac < 25) { nbpnt = 25; } Bug Uneven number
  else if (ndgjac < 30)
  {
    nbpnts = 30;
  }
  else if (ndgjac < 40)
  {
    nbpnts = 40;
  }
  else if (ndgjac < 50)
  {
    nbpnts = 50;
  }
  //  else if (*ndgjac < 61) { nbpnt = 61;} Bug Uneven number
  else
  {
    nbpnts = 50;
#ifdef OCCT_DEBUG
    std::cout << "F(U, V) : Not enough points of discretization" << std::endl;
#endif
  }

  // If constraints are on borders, this adds 2 points
  if (iordre > -1)
  {
    nbpnts += 2;
  }

  return true;
}

static void appendInternalTolerance(const occ::handle<NCollection_HArray1<double>>& theSource,
                                    const int                                       theCount,
                                    const int                                       theOffset,
                                    const occ::handle<NCollection_HArray1<double>>& theTarget)
{
  for (int anSSPIndex = 1; anSSPIndex <= theCount; ++anSSPIndex)
  {
    theTarget->SetValue(theOffset + anSSPIndex, theSource->Value(anSSPIndex));
  }
}

static void appendFrontierTolerance(const occ::handle<NCollection_HArray2<double>>& theSource,
                                    const int                                       theCount,
                                    const int                                       theOffset,
                                    const occ::handle<NCollection_HArray2<double>>& theFrontierTol,
                                    const occ::handle<NCollection_HArray2<double>>& theCuttingTol)
{
  for (int anSSPIndex = 1; anSSPIndex <= theCount; ++anSSPIndex)
  {
    const int aGlobalSSPIndex = theOffset + anSSPIndex;
    for (int aTolIndex = 1; aTolIndex <= 4; ++aTolIndex)
    {
      theFrontierTol->SetValue(aGlobalSSPIndex, aTolIndex, theSource->Value(anSSPIndex, aTolIndex));
      theCuttingTol->SetValue(aGlobalSSPIndex, aTolIndex, 0.0);
    }
  }
}

static double hMaxFactor(const int theOrder)
{
  switch (theOrder)
  {
    case -1:
      return 0.0;
    case 0:
      return 1.0;
    case 1:
      return 1.5;
    default:
      return 1.75;
  }
}

//=================================================================================================

AdvApp2Var_Context::AdvApp2Var_Context()
    : myFav(0),
      myOrdU(0),
      myOrdV(0),
      myLimU(0),
      myLimV(0),
      myNb1DSS(0),
      myNb2DSS(0),
      myNb3DSS(0),
      myNbURoot(0),
      myNbVRoot(0),
      myJDegU(0),
      myJDegV(0)
{
}

//=================================================================================================

AdvApp2Var_Context::AdvApp2Var_Context(const int                                       ifav,
                                       const int                                       iu,
                                       const int                                       iv,
                                       const int                                       nlimu,
                                       const int                                       nlimv,
                                       const int                                       iprecis,
                                       const int                                       nb1Dss,
                                       const int                                       nb2Dss,
                                       const int                                       nb3Dss,
                                       const occ::handle<NCollection_HArray1<double>>& tol1D,
                                       const occ::handle<NCollection_HArray1<double>>& tol2D,
                                       const occ::handle<NCollection_HArray1<double>>& tol3D,
                                       const occ::handle<NCollection_HArray2<double>>& tof1D,
                                       const occ::handle<NCollection_HArray2<double>>& tof2D,
                                       const occ::handle<NCollection_HArray2<double>>& tof3D)
    : myFav(ifav),
      myOrdU(iu),
      myOrdV(iv),
      myLimU(nlimu),
      myLimV(nlimv),
      myNb1DSS(nb1Dss),
      myNb2DSS(nb2Dss),
      myNb3DSS(nb3Dss)
{
  int anErrorCode = 0, NbPntU = 0, JDegU = 0, NbPntV = 0, JDegV = 0;
  int aCoeffLimit;

  // myNbURoot,myJDegU
  aCoeffLimit = nlimu;
  if (aCoeffLimit < 2 * iu + 2)
    aCoeffLimit = 2 * iu + 2;
  if (!lesparam(iu, aCoeffLimit, iprecis, NbPntU, JDegU))
  {
    throw Standard_ConstructionError("AdvApp2Var_Context");
  }
  myNbURoot = NbPntU;
  myJDegU   = JDegU;
  if (iu > -1)
    NbPntU = myNbURoot - 2;

  // myJMaxU
  int                                      aSize = JDegU - 2 * iu - 1;
  occ::handle<NCollection_HArray1<double>> JMaxU = new NCollection_HArray1<double>(1, aSize);
  double* JU_array                               = (double*)&JMaxU->ChangeArray1()(JMaxU->Lower());
  AdvApp2Var_ApproxF2var::mma2jmx_(&JDegU, (int*)&iu, JU_array);
  myJMaxU = JMaxU;

  // myNbVRoot,myJDegV
  aCoeffLimit = nlimv;
  if (aCoeffLimit < 2 * iv + 2)
    aCoeffLimit = 2 * iv + 2;
  if (!lesparam(iv, aCoeffLimit, iprecis, NbPntV, JDegV))
  {
    throw Standard_ConstructionError("AdvApp2Var_Context");
  }
  myNbVRoot = NbPntV;
  myJDegV   = JDegV;
  if (iv > -1)
    NbPntV = myNbVRoot - 2;

  // myJMaxV
  aSize                                           = JDegV - 2 * iv - 1;
  occ::handle<NCollection_HArray1<double>> JMaxV = new NCollection_HArray1<double>(1, aSize);
  double* JV_array                               = (double*)&JMaxV->ChangeArray1()(JMaxV->Lower());
  AdvApp2Var_ApproxF2var::mma2jmx_(&JDegV, (int*)&iv, JV_array);
  myJMaxV = JMaxV;

  // myURoots, myVRoots
  occ::handle<NCollection_HArray1<double>> URoots = new NCollection_HArray1<double>(1, myNbURoot);
  double* U_array = (double*)&URoots->ChangeArray1()(URoots->Lower());
  occ::handle<NCollection_HArray1<double>> VRoots = new NCollection_HArray1<double>(1, myNbVRoot);
  double* V_array = (double*)&VRoots->ChangeArray1()(VRoots->Lower());
  AdvApp2Var_ApproxF2var::mma2roo_(&NbPntU, &NbPntV, U_array, V_array);
  myURoots = URoots;
  myVRoots = VRoots;

  // myUGauss
  aSize                                            = (NbPntU / 2 + 1) * (myJDegU - 2 * iu - 1);
  occ::handle<NCollection_HArray1<double>> UGauss = new NCollection_HArray1<double>(1, aSize);
  double* UG_array = (double*)&UGauss->ChangeArray1()(UGauss->Lower());
  AdvApp2Var_ApproxF2var::mmapptt_(&JDegU, &NbPntU, &iu, UG_array, &anErrorCode);
  if (anErrorCode != 0)
  {
    throw Standard_ConstructionError("AdvApp2Var_Context : Error in FORTRAN");
  }
  myUGauss = UGauss;

  // myVGauss
  aSize                                            = (NbPntV / 2 + 1) * (myJDegV - 2 * iv - 1);
  occ::handle<NCollection_HArray1<double>> VGauss = new NCollection_HArray1<double>(1, aSize);
  double* VG_array = (double*)&VGauss->ChangeArray1()(VGauss->Lower());
  AdvApp2Var_ApproxF2var::mmapptt_(&JDegV, &NbPntV, &iv, VG_array, &anErrorCode);
  if (anErrorCode != 0)
  {
    throw Standard_ConstructionError("AdvApp2Var_Context : Error in FORTRAN");
  }
  myVGauss = VGauss;

  // myInternalTol, myFrontierTol, myCuttingTol
  const int                                aNbSSP = nb1Dss + nb2Dss + nb3Dss;
  occ::handle<NCollection_HArray1<double>> ITol  = new NCollection_HArray1<double>(1, aNbSSP);
  appendInternalTolerance(tol1D, nb1Dss, 0, ITol);
  appendInternalTolerance(tol2D, nb2Dss, nb1Dss, ITol);
  appendInternalTolerance(tol3D, nb3Dss, nb1Dss + nb2Dss, ITol);
  if (iu > -1 || iv > -1)
  {
    for (int anSSPIndex = 1; anSSPIndex <= aNbSSP; ++anSSPIndex)
    {
      ITol->SetValue(anSSPIndex, ITol->Value(anSSPIndex) / 2);
    }
  }
  occ::handle<NCollection_HArray2<double>> FTol = new NCollection_HArray2<double>(1, aNbSSP, 1, 4);
  occ::handle<NCollection_HArray2<double>> CTol = new NCollection_HArray2<double>(1, aNbSSP, 1, 4);
  appendFrontierTolerance(tof1D, nb1Dss, 0, FTol, CTol);
  appendFrontierTolerance(tof2D, nb2Dss, nb1Dss, FTol, CTol);
  appendFrontierTolerance(tof3D, nb3Dss, nb1Dss + nb2Dss, FTol, CTol);
  if (iu > -1 || iv > -1)
  {
    double aTolMin;
    const double aHMaxU  = hMaxFactor(iu);
    const double aHMaxV  = hMaxFactor(iv);
    const double aWeight = aHMaxU * aHMaxV + aHMaxU + aHMaxV;
    for (int anSSPIndex = 1; anSSPIndex <= aNbSSP; ++anSSPIndex)
    {
      for (int aTolIndex = 1; aTolIndex <= 4; ++aTolIndex)
      {
        aTolMin = ITol->Value(anSSPIndex) / aWeight;
        if (aTolMin < FTol->Value(anSSPIndex, aTolIndex))
          FTol->SetValue(anSSPIndex, aTolIndex, aTolMin);
        CTol->SetValue(anSSPIndex, aTolIndex, aTolMin);
      }
    }
  }
  myInternalTol = ITol;
  myFrontierTol = FTol;
  myCuttingTol  = CTol;
}

//=================================================================================================

int AdvApp2Var_Context::TotalDimension() const
{
  return myNb1DSS + 2 * myNb2DSS + 3 * myNb3DSS;
}

//=================================================================================================

int AdvApp2Var_Context::TotalNumberSSP() const
{
  return myNb1DSS + myNb2DSS + myNb3DSS;
}

//=================================================================================================

int AdvApp2Var_Context::FavorIso() const
{
  return myFav;
}

//=================================================================================================

int AdvApp2Var_Context::UOrder() const
{
  return myOrdU;
}

//=================================================================================================

int AdvApp2Var_Context::VOrder() const
{
  return myOrdV;
}

//=================================================================================================

int AdvApp2Var_Context::ULimit() const
{
  return myLimU;
}

//=================================================================================================

int AdvApp2Var_Context::VLimit() const
{
  return myLimV;
}

//=================================================================================================

int AdvApp2Var_Context::UJacDeg() const
{
  return myJDegU;
}

//=================================================================================================

int AdvApp2Var_Context::VJacDeg() const
{
  return myJDegV;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::UJacMax() const
{
  return myJMaxU;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::VJacMax() const
{
  return myJMaxV;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::URoots() const
{
  return myURoots;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::VRoots() const
{
  return myVRoots;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::UGauss() const
{
  return myUGauss;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::VGauss() const
{
  return myVGauss;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Context::IToler() const
{
  return myInternalTol;
}

//=================================================================================================

occ::handle<NCollection_HArray2<double>> AdvApp2Var_Context::FToler() const
{
  return myFrontierTol;
}

//=================================================================================================

occ::handle<NCollection_HArray2<double>> AdvApp2Var_Context::CToler() const
{
  return myCuttingTol;
}
