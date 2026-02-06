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

// Modified:	Wed Jan 15 10:04:41 1997
//   by:	Joelle CHAUVET
//		G1135 : Methods CutSense with criterion, Coefficients,
//                              CritValue, SetCritValue
// Modified:	Tue May 19 10:22:44 1998
//   by:	Joelle CHAUVET / Jean-Marc LACHAUME
//		Initialisation de myCritValue pour OSF

#include <AdvApp2Var_ApproxF2var.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Criterion.hxx>
#include <AdvApp2Var_Framework.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_MathBase.hxx>
#include <AdvApp2Var_Node.hxx>
#include <AdvApp2Var_Patch.hxx>
#include <Convert_GridPolynomialToPoles.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AdvApp2Var_Patch, Standard_Transient)

//=================================================================================================

AdvApp2Var_Patch::AdvApp2Var_Patch()
    : myU0(0.),
      myU1(1.),
      myV0(0.),
      myV1(1.),
      myOrdInU(0),
      myOrdInV(0),
      myNbCoeffInU(0),
      myNbCoeffInV(0),
      myApprIsDone(false),
      myHasResult(false),
      myCutSense(0),
      myDiscIsDone(false),
      myCritValue(0.)
{
}

//=================================================================================================

AdvApp2Var_Patch::AdvApp2Var_Patch(const double U0,
                                   const double U1,
                                   const double V0,
                                   const double V1,
                                   const int    iu,
                                   const int    iv)
    : myU0(U0),
      myU1(U1),
      myV0(V0),
      myV1(V1),
      myOrdInU(iu),
      myOrdInV(iv),
      myNbCoeffInU(0),
      myNbCoeffInV(0),
      myApprIsDone(false),
      myHasResult(false),
      myCutSense(0),
      myDiscIsDone(false),
      myCritValue(0.)
{
}

//=================================================================================================

bool AdvApp2Var_Patch::IsDiscretised() const
{
  return myDiscIsDone;
}

//=================================================================================================

void AdvApp2Var_Patch::Discretise(const AdvApp2Var_Context&           Conditions,
                                  const AdvApp2Var_Framework&         Constraints,
                                  const AdvApp2Var_EvaluatorFunc2Var& Func)
{

  // data stored in the Context
  int NDIMEN, ISOFAV;
  NDIMEN = Conditions.TotalDimension();
  // Attention : works only for 3D
  ISOFAV = Conditions.FavorIso();

  // data related to the patch to be discretized
  int                                      NBPNTU, NBPNTV;
  int                                      IORDRU = myOrdInU, IORDRV = myOrdInV;
  occ::handle<NCollection_HArray1<double>> HUROOT = Conditions.URoots();
  occ::handle<NCollection_HArray1<double>> HVROOT = Conditions.VRoots();
  double*                                  UROOT;
  UROOT  = (double*)&HUROOT->ChangeArray1()(HUROOT->Lower());
  NBPNTU = (Conditions.URoots())->Length();
  if (myOrdInU > -1)
    NBPNTU -= 2;
  double* VROOT;
  VROOT  = (double*)&HVROOT->ChangeArray1()(HVROOT->Lower());
  NBPNTV = (Conditions.VRoots())->Length();
  if (myOrdInV > -1)
    NBPNTV -= 2;

  // data stored in the Framework Constraints cad Nodes and Isos
  // C1, C2, C3 and C4 are dimensionnes in FORTRAN with (NDIMEN,IORDRU+2,IORDRV+2)
  int                                      SIZE   = NDIMEN * (IORDRU + 2) * (IORDRV + 2);
  occ::handle<NCollection_HArray1<double>> HCOINS = new NCollection_HArray1<double>(1, SIZE * 4);
  HCOINS->Init(0.);

  int    iu, iv;
  double du = (myU1 - myU0) / 2, dv = (myV1 - myV0) / 2, rho, valnorm;

  for (iu = 0; iu <= myOrdInU; iu++)
  {
    for (iv = 0; iv <= myOrdInV; iv++)
    {
      // factor of normalization
      rho = pow(du, iu) * pow(dv, iv);

      // F(U0,V0) and its derivatives normalized on (-1,1)
      valnorm = rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).X();
      HCOINS->SetValue(1 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);

      // F(U1,V0) and its derivatives normalized on (-1,1)
      valnorm = rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).X();
      HCOINS->SetValue(SIZE + 1 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).Y();
      HCOINS->SetValue(SIZE + 2 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).Z();
      HCOINS->SetValue(SIZE + 3 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);

      // F(U0,V1) and its derivatives normalized on (-1,1)
      valnorm = rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).X();
      HCOINS->SetValue(2 * SIZE + 1 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 * SIZE + 2 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).Z();
      HCOINS->SetValue(2 * SIZE + 3 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);

      // F(U1,V1) and its derivatives normalized on (-1,1)
      valnorm = rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).X();
      HCOINS->SetValue(3 * SIZE + 1 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).Y();
      HCOINS->SetValue(3 * SIZE + 2 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
      valnorm = rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 * SIZE + 3 + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv, valnorm);
    }
  }
  double* C1 = (double*)&HCOINS->ChangeArray1()(HCOINS->Lower());
  double* C2 = C1 + SIZE;
  double* C3 = C2 + SIZE;
  double* C4 = C3 + SIZE;

  // tables SomTab and Diftab of discretization of isos U=U0 and U=U1
  // SU0, SU1, DU0 and DU1 are dimensioned in FORTRAN to
  // (1+NBPNTV/2)*NDIMEN*(IORDRU+1)

  SIZE = (1 + NBPNTV / 2) * NDIMEN;

  occ::handle<NCollection_HArray1<double>> HSU0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HSU0->ChangeArray1() = ((Constraints.IsoU(myU0, myV0, myV1)).SomTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HDU0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HDU0->ChangeArray1() = ((Constraints.IsoU(myU0, myV0, myV1)).DifTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HSU1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HSU1->ChangeArray1() = ((Constraints.IsoU(myU1, myV0, myV1)).SomTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HDU1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HDU1->ChangeArray1() = ((Constraints.IsoU(myU1, myV0, myV1)).DifTab())->Array1();

  // normalization
  int ideb1, ideb2, ideb3, ideb4, jj;
  for (iu = 1; iu <= IORDRU; iu++)
  {
    rho   = pow(du, iu);
    ideb1 = HSU0->Lower() + iu * SIZE - 1;
    ideb2 = HDU0->Lower() + iu * SIZE - 1;
    ideb3 = HSU1->Lower() + iu * SIZE - 1;
    ideb4 = HDU1->Lower() + iu * SIZE - 1;
    for (jj = 1; jj <= SIZE; jj++)
    {
      HSU0->SetValue(ideb1 + jj, rho * HSU0->Value(ideb1 + jj));
      HDU0->SetValue(ideb2 + jj, rho * HDU0->Value(ideb2 + jj));
      HSU1->SetValue(ideb3 + jj, rho * HSU1->Value(ideb3 + jj));
      HDU1->SetValue(ideb4 + jj, rho * HDU1->Value(ideb4 + jj));
    }
  }

  double* SU0 = (double*)&HSU0->ChangeArray1()(HSU0->Lower());
  double* DU0 = (double*)&HDU0->ChangeArray1()(HDU0->Lower());
  double* SU1 = (double*)&HSU1->ChangeArray1()(HSU1->Lower());
  double* DU1 = (double*)&HDU1->ChangeArray1()(HDU1->Lower());

  // tables SomTab and Diftab of discretization of isos V=V0 and V=V1
  // SU0, SU1, DU0 and DU1 are dimensioned in FORTRAN at
  // (1+NBPNTU/2)*NDIMEN*(IORDRV+1)

  SIZE = (1 + NBPNTU / 2) * NDIMEN;

  occ::handle<NCollection_HArray1<double>> HSV0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HSV0->ChangeArray1() = ((Constraints.IsoV(myU0, myU1, myV0)).SomTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HDV0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HDV0->ChangeArray1() = ((Constraints.IsoV(myU0, myU1, myV0)).DifTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HSV1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HSV1->ChangeArray1() = ((Constraints.IsoV(myU0, myU1, myV1)).SomTab())->Array1();

  occ::handle<NCollection_HArray1<double>> HDV1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HDV1->ChangeArray1() = ((Constraints.IsoV(myU0, myU1, myV1)).DifTab())->Array1();

  // normalisation
  for (iv = 1; iv <= IORDRV; iv++)
  {
    rho   = pow(dv, iv);
    ideb1 = HSV0->Lower() + iv * SIZE - 1;
    ideb2 = HDV0->Lower() + iv * SIZE - 1;
    ideb3 = HSV1->Lower() + iv * SIZE - 1;
    ideb4 = HDV1->Lower() + iv * SIZE - 1;
    for (jj = 1; jj <= SIZE; jj++)
    {
      HSV0->SetValue(ideb1 + jj, rho * HSV0->Value(ideb1 + jj));
      HDV0->SetValue(ideb2 + jj, rho * HDV0->Value(ideb2 + jj));
      HSV1->SetValue(ideb3 + jj, rho * HSV1->Value(ideb3 + jj));
      HDV1->SetValue(ideb4 + jj, rho * HDV1->Value(ideb4 + jj));
    }
  }

  double* SV0 = (double*)&HSV0->ChangeArray1()(HSV0->Lower());
  double* DV0 = (double*)&HDV0->ChangeArray1()(HDV0->Lower());
  double* SV1 = (double*)&HSV1->ChangeArray1()(HSV1->Lower());
  double* DV1 = (double*)&HDV1->ChangeArray1()(HDV1->Lower());

  // SOSOTB and DIDITB are dimensioned in FORTRAN at
  // (0:NBPNTU/2,0:NBPNTV/2,NDIMEN)

  SIZE = (1 + NBPNTU / 2) * (1 + NBPNTV / 2) * NDIMEN;

  occ::handle<NCollection_HArray1<double>> HSOSO  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  SOSOTB = (double*)&HSOSO->ChangeArray1()(HSOSO->Lower());
  HSOSO->Init(0.);
  occ::handle<NCollection_HArray1<double>> HDIDI  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  DIDITB = (double*)&HDIDI->ChangeArray1()(HDIDI->Lower());
  HDIDI->Init(0.);

  // SODITB and DISOTB are dimensioned in FORTRAN at
  // (1:NBPNTU/2,1:NBPNTV/2,NDIMEN)

  SIZE = (NBPNTU / 2) * (NBPNTV / 2) * NDIMEN;

  occ::handle<NCollection_HArray1<double>> HSODI  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  SODITB = (double*)&HSODI->ChangeArray1()(HSODI->Lower());
  HSODI->Init(0.);
  occ::handle<NCollection_HArray1<double>> HDISO  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  DISOTB = (double*)&HDISO->ChangeArray1()(HDISO->Lower());
  HDISO->Init(0.);

  int IERCOD = 0;

  //  discretization of polynoms of interpolation
  AdvApp2Var_ApproxF2var::mma2cdi_(&NDIMEN,
                                   &NBPNTU,
                                   UROOT,
                                   &NBPNTV,
                                   VROOT,
                                   &IORDRU,
                                   &IORDRV,
                                   C1,
                                   C2,
                                   C3,
                                   C4,
                                   SU0,
                                   SU1,
                                   DU0,
                                   DU1,
                                   SV0,
                                   SV1,
                                   DV0,
                                   DV1,
                                   SOSOTB,
                                   SODITB,
                                   DISOTB,
                                   DIDITB,
                                   &IERCOD);

  //  discretization of the square
  double UDBFN[2], VDBFN[2];
  UDBFN[0] = myU0;
  UDBFN[1] = myU1;
  VDBFN[0] = myV0;
  VDBFN[1] = myV1;

  SIZE                                            = std::max(NBPNTU, NBPNTV);
  occ::handle<NCollection_HArray1<double>> HTABLE = new NCollection_HArray1<double>(1, SIZE);
  double*                                  TAB = (double*)&HTABLE->ChangeArray1()(HTABLE->Lower());

  occ::handle<NCollection_HArray1<double>> HPOINTS =
    new NCollection_HArray1<double>(1, SIZE * NDIMEN);
  double* PTS = (double*)&HPOINTS->ChangeArray1()(HPOINTS->Lower());

  // GCC 3.0 would not accept this line without the void
  // pointer cast.  Perhaps the real problem is a definition
  // somewhere that has a void * in it.
  AdvApp2Var_ApproxF2var::mma2ds1_(&NDIMEN,
                                   UDBFN,
                                   VDBFN,
                                   /*(void *)*/ Func,
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

  // the results are stored
  if (IERCOD == 0)
  {
    myDiscIsDone = true;
    mySosoTab    = HSOSO;
    myDisoTab    = HDISO;
    mySodiTab    = HSODI;
    myDidiTab    = HDIDI;
  }
  else
  {
    myDiscIsDone = false;
  }
}

//=================================================================================================

bool AdvApp2Var_Patch::HasResult() const
{
  return myHasResult;
}

//=================================================================================================

bool AdvApp2Var_Patch::IsApproximated() const
{
  return myApprIsDone;
}

//=================================================================================================

void AdvApp2Var_Patch::AddConstraints(const AdvApp2Var_Context&   Conditions,
                                      const AdvApp2Var_Framework& Constraints)
{
  // data stored in the  Context
  int NDIMEN;
  int IERCOD, NCFLMU, NCFLMV, NDegU, NDegV;
  NDIMEN = Conditions.TotalDimension();
  // Attention : works only for 3D
  NCFLMU = Conditions.ULimit();
  NCFLMV = Conditions.VLimit();
  NDegU  = NCFLMU - 1;
  NDegV  = NCFLMV - 1;

  // data relative to the patch
  int     IORDRU = myOrdInU, IORDRV = myOrdInV;
  double* PATCAN = (double*)&myEquation->ChangeArray1()(myEquation->Lower());

  // curves of approximation of Isos U
  int                                      SIZE = NCFLMV * NDIMEN;
  occ::handle<NCollection_HArray1<double>> HIsoU0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HIsoU0->ChangeArray1() = (Constraints.IsoU(myU0, myV0, myV1)).Polynom()->Array1();
  double*                               IsoU0 = (double*)&HIsoU0->ChangeArray1()(HIsoU0->Lower());
  occ::handle<NCollection_HArray1<int>> HCFU0 = new NCollection_HArray1<int>(1, IORDRU + 1);
  int*                                  NCFU0 = (int*)&HCFU0->ChangeArray1()(HCFU0->Lower());
  HCFU0->Init((Constraints.IsoU(myU0, myV0, myV1)).NbCoeff());

  occ::handle<NCollection_HArray1<double>> HIsoU1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRU + 1));
  HIsoU1->ChangeArray1() = (Constraints.IsoU(myU1, myV0, myV1)).Polynom()->Array1();
  double*                               IsoU1 = (double*)&HIsoU1->ChangeArray1()(HIsoU1->Lower());
  occ::handle<NCollection_HArray1<int>> HCFU1 = new NCollection_HArray1<int>(1, IORDRU + 1);
  int*                                  NCFU1 = (int*)&HCFU1->ChangeArray1()(HCFU1->Lower());
  HCFU1->Init((Constraints.IsoU(myU1, myV0, myV1)).NbCoeff());

  // normalization of Isos U
  int    iu, iv;
  double du = (myU1 - myU0) / 2, dv = (myV1 - myV0) / 2, rho, valnorm;
  int    ideb0, ideb1, jj;

  for (iu = 1; iu <= IORDRU; iu++)
  {
    rho   = pow(du, iu);
    ideb0 = HIsoU0->Lower() + iu * SIZE - 1;
    ideb1 = HIsoU1->Lower() + iu * SIZE - 1;
    for (jj = 1; jj <= SIZE; jj++)
    {
      HIsoU0->SetValue(ideb0 + jj, rho * HIsoU0->Value(ideb0 + jj));
      HIsoU1->SetValue(ideb1 + jj, rho * HIsoU1->Value(ideb1 + jj));
    }
  }

  // curves of approximation of Isos V
  SIZE = NCFLMU * NDIMEN;
  occ::handle<NCollection_HArray1<double>> HIsoV0 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HIsoV0->ChangeArray1() = (Constraints.IsoV(myU0, myU1, myV0)).Polynom()->Array1();
  double*                               IsoV0 = (double*)&HIsoV0->ChangeArray1()(HIsoV0->Lower());
  occ::handle<NCollection_HArray1<int>> HCFV0 = new NCollection_HArray1<int>(1, IORDRV + 1);
  int*                                  NCFV0 = (int*)&HCFV0->ChangeArray1()(HCFV0->Lower());
  HCFV0->Init((Constraints.IsoV(myU0, myU1, myV0)).NbCoeff());

  occ::handle<NCollection_HArray1<double>> HIsoV1 =
    new NCollection_HArray1<double>(1, SIZE * (IORDRV + 1));
  HIsoV1->ChangeArray1() = (Constraints.IsoV(myU0, myU1, myV1)).Polynom()->Array1();
  double*                               IsoV1 = (double*)&HIsoV1->ChangeArray1()(HIsoV1->Lower());
  occ::handle<NCollection_HArray1<int>> HCFV1 = new NCollection_HArray1<int>(1, IORDRV + 1);
  int*                                  NCFV1 = (int*)&HCFV1->ChangeArray1()(HCFV1->Lower());
  HCFV1->Init((Constraints.IsoV(myU0, myU1, myV1)).NbCoeff());

  //  normalization of Isos V
  for (iv = 1; iv <= IORDRV; iv++)
  {
    rho   = pow(dv, iv);
    ideb0 = HIsoV0->Lower() + iv * SIZE - 1;
    ideb1 = HIsoV1->Lower() + iv * SIZE - 1;
    for (jj = 1; jj <= SIZE; jj++)
    {
      HIsoV0->SetValue(ideb0 + jj, rho * HIsoV0->Value(ideb0 + jj));
      HIsoV1->SetValue(ideb1 + jj, rho * HIsoV1->Value(ideb1 + jj));
    }
  }

  // add constraints to constant V
  occ::handle<NCollection_HArray1<double>> HHERMV =
    new NCollection_HArray1<double>(1, (2 * IORDRV + 2) * (2 * IORDRV + 2));
  double* HermV = (double*)&HHERMV->ChangeArray1()(HHERMV->Lower());
  if (IORDRV >= 0)
  {
    AdvApp2Var_ApproxF2var::mma1her_(&IORDRV, HermV, &IERCOD);
    if (IERCOD != 0)
    {
      throw Standard_ConstructionError("AdvApp2Var_Patch::AddConstraints : Error in FORTRAN");
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

  // add constraints to constant U
  occ::handle<NCollection_HArray1<double>> HHERMU =
    new NCollection_HArray1<double>(1, (2 * IORDRU + 2) * (2 * IORDRU + 2));
  double* HermU = (double*)&HHERMU->ChangeArray1()(HHERMU->Lower());
  if (IORDRU >= 0)
  {
    AdvApp2Var_ApproxF2var::mma1her_(&IORDRU, HermU, &IERCOD);
    if (IERCOD != 0)
    {
      throw Standard_ConstructionError("AdvApp2Var_Patch::AddConstraints : Error in FORTRAN");
    }
    AdvApp2Var_ApproxF2var::mma2ac3_(&NDIMEN,
                                     &NDegU,
                                     &NDegV,
                                     &IORDRU,
                                     &NCFLMV,
                                     NCFU0,
                                     IsoU0,
                                     NCFU1,
                                     IsoU1,
                                     HermU,
                                     PATCAN);
  }

  // add constraints at the corners
  int ideb;
  SIZE                                            = NDIMEN * (IORDRU + 2) * (IORDRV + 2);
  occ::handle<NCollection_HArray1<double>> HCOINS = new NCollection_HArray1<double>(1, SIZE * 4);

  for (iu = 0; iu <= myOrdInU; iu++)
  {
    for (iv = 0; iv <= myOrdInV; iv++)
    {
      rho = pow(du, iu) * pow(dv, iv);

      // -F(U0,V0) and its derivatives normalized on (-1,1)
      ideb    = HCOINS->Lower() + NDIMEN * iu + NDIMEN * (IORDRU + 2) * iv - 1;
      valnorm = -rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).X();
      HCOINS->SetValue(1 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU0, myV0))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 + ideb, valnorm);

      // -F(U1,V0) and its derivatives normalized on (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).X();
      HCOINS->SetValue(1 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU1, myV0))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 + ideb, valnorm);

      // -F(U0,V1) and its derivatives normalized on (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).X();
      HCOINS->SetValue(1 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU0, myV1))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 + ideb, valnorm);

      // -F(U1,V1) and its derivatives normalized on (-1,1)
      ideb += SIZE;
      valnorm = -rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).X();
      HCOINS->SetValue(1 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).Y();
      HCOINS->SetValue(2 + ideb, valnorm);
      valnorm = -rho * ((Constraints.Node(myU1, myV1))->Point(iu, iv)).Z();
      HCOINS->SetValue(3 + ideb, valnorm);
    }
  }

  //  tables required for FORTRAN
  int                                      IORDMX = std::max(IORDRU, IORDRV);
  occ::handle<NCollection_HArray1<double>> HEXTR =
    new NCollection_HArray1<double>(1, 2 * IORDMX + 2);
  double*                                  EXTR  = (double*)&HEXTR->ChangeArray1()(HEXTR->Lower());
  occ::handle<NCollection_HArray1<double>> HFACT = new NCollection_HArray1<double>(1, IORDMX + 1);
  double*                                  FACT  = (double*)&HFACT->ChangeArray1()(HFACT->Lower());

  int     idim, ncf0, ncf1, iun = 1;
  double* Is;

  // add extremities of isos U
  for (iu = 1; iu <= IORDRU + 1; iu++)
  {
    ncf0 = HCFU0->Value(HCFU0->Lower() + iu - 1);
    ncf1 = HCFU1->Value(HCFU1->Lower() + iu - 1);
    for (idim = 1; idim <= NDIMEN; idim++)
    {
      Is = IsoU0 + NCFLMV * (idim - 1) + NCFLMV * NDIMEN * (iu - 1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRV, &iun, &ncf0, Is, EXTR, FACT);
      for (iv = 1; iv <= IORDRV + 1; iv++)
      {
        ideb = HCOINS->Lower() + NDIMEN * (iu - 1) + NDIMEN * (IORDRU + 2) * (iv - 1) - 1;
        HCOINS->ChangeValue(idim + ideb) += HEXTR->Value(1 + 2 * (iv - 1));
        HCOINS->ChangeValue(2 * SIZE + idim + ideb) += HEXTR->Value(2 + 2 * (iv - 1));
      }
      Is = IsoU1 + NCFLMV * (idim - 1) + NCFLMV * NDIMEN * (iu - 1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRV, &iun, &ncf1, Is, EXTR, FACT);
      for (iv = 1; iv <= IORDRV + 1; iv++)
      {
        ideb = HCOINS->Lower() + NDIMEN * (iu - 1) + NDIMEN * (IORDRU + 2) * (iv - 1) - 1;
        HCOINS->ChangeValue(SIZE + idim + ideb) += HEXTR->Value(1 + 2 * (iv - 1));
        HCOINS->ChangeValue(3 * SIZE + idim + ideb) += HEXTR->Value(2 + 2 * (iv - 1));
      }
    }
  }

  // add extremities of isos V
  for (iv = 1; iv <= IORDRV + 1; iv++)
  {
    ncf0 = HCFV0->Value(HCFV0->Lower() + iv - 1);
    ncf1 = HCFV1->Value(HCFV1->Lower() + iv - 1);
    for (idim = 1; idim <= NDIMEN; idim++)
    {
      Is = IsoV0 + NCFLMU * (idim - 1) + NCFLMU * NDIMEN * (iv - 1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRU, &iun, &ncf0, Is, EXTR, FACT);
      for (iu = 1; iu <= IORDRU + 1; iu++)
      {
        ideb = HCOINS->Lower() + NDIMEN * (iu - 1) + NDIMEN * (IORDRU + 2) * (iv - 1) - 1;
        HCOINS->ChangeValue(idim + ideb) += HEXTR->Value(1 + 2 * (iu - 1));
        HCOINS->ChangeValue(SIZE + idim + ideb) += HEXTR->Value(2 + 2 * (iu - 1));
      }
      Is = IsoV1 + NCFLMU * (idim - 1) + NCFLMU * NDIMEN * (iv - 1);
      AdvApp2Var_MathBase::mmdrc11_(&IORDRU, &iun, &ncf1, Is, EXTR, FACT);
      for (iu = 1; iu <= IORDRU + 1; iu++)
      {
        ideb = HCOINS->Lower() + NDIMEN * (iu - 1) + NDIMEN * (IORDRU + 2) * (iv - 1) - 1;
        HCOINS->ChangeValue(2 * SIZE + idim + ideb) += HEXTR->Value(1 + 2 * (iu - 1));
        HCOINS->ChangeValue(3 * SIZE + idim + ideb) += HEXTR->Value(2 + 2 * (iu - 1));
      }
    }
  }

  // add all to PATCAN
  double* C1 = (double*)&HCOINS->ChangeArray1()(HCOINS->Lower());
  double* C2 = C1 + SIZE;
  double* C3 = C2 + SIZE;
  double* C4 = C3 + SIZE;
  if (IORDRU >= 0 && IORDRV >= 0)
  {
    AdvApp2Var_ApproxF2var::mma2ac1_(&NDIMEN,
                                     &NDegU,
                                     &NDegV,
                                     &IORDRU,
                                     &IORDRV,
                                     C1,
                                     C2,
                                     C3,
                                     C4,
                                     HermU,
                                     HermV,
                                     PATCAN);
  }
}

//=================================================================================================

void AdvApp2Var_Patch::AddErrors(const AdvApp2Var_Framework& Constraints)
{
  int NBSESP = 1, iesp;
  int iu, iv;

  double errU, errV, error, hmax[4];
  hmax[0] = 0;
  hmax[1] = 1;
  hmax[2] = 1.5;
  hmax[3] = 1.75;

  for (iesp = 1; iesp <= NBSESP; iesp++)
  {
    //  error max in sub-space iesp
    errU = 0.;
    for (iv = 1; iv <= myOrdInV + 1; iv++)
    {
      error = ((Constraints.IsoV(myU0, myU1, myV0)).MaxErrors())->Value(iesp, iv);
      errU  = std::max(errU, error);
      error = ((Constraints.IsoV(myU0, myU1, myV1)).MaxErrors())->Value(iesp, iv);
      errU  = std::max(errU, error);
    }
    errV = 0.;
    for (iu = 1; iu <= myOrdInU + 1; iu++)
    {
      error = ((Constraints.IsoU(myU0, myV0, myV1)).MaxErrors())->Value(iesp, iu);
      errV  = std::max(errV, error);
      error = ((Constraints.IsoU(myU1, myV0, myV1)).MaxErrors())->Value(iesp, iu);
      errV  = std::max(errV, error);
    }
    myMaxErrors->ChangeValue(iesp) += errU * hmax[myOrdInV + 1] + errV * hmax[myOrdInU + 1];

    // average error in sub-space iesp
    errU = 0.;
    for (iv = 1; iv <= myOrdInV + 1; iv++)
    {
      error = ((Constraints.IsoV(myU0, myU1, myV0)).MoyErrors())->Value(iesp, iv);
      errU  = std::max(errU, error);
      error = ((Constraints.IsoV(myU0, myU1, myV1)).MoyErrors())->Value(iesp, iv);
      errU  = std::max(errU, error);
    }
    errV = 0.;
    for (iu = 1; iu <= myOrdInU + 1; iu++)
    {
      error = ((Constraints.IsoU(myU0, myV0, myV1)).MoyErrors())->Value(iesp, iu);
      errV  = std::max(errV, error);
      error = ((Constraints.IsoU(myU1, myV0, myV1)).MoyErrors())->Value(iesp, iu);
      errV  = std::max(errV, error);
    }
    error = myMoyErrors->Value(iesp);
    error *= error;
    error += errU * hmax[myOrdInV + 1] * errU * hmax[myOrdInV + 1]
             + errV * hmax[myOrdInU + 1] * errV * hmax[myOrdInU + 1];
    myMoyErrors->SetValue(iesp, std::sqrt(error));

    // max errors at iso-borders
    occ::handle<NCollection_HArray2<double>> HERISO =
      new NCollection_HArray2<double>(1, NBSESP, 1, 4);
    HERISO->SetValue(iesp, 1, ((Constraints.IsoV(myU0, myU1, myV0)).MaxErrors())->Value(iesp, 1));
    HERISO->SetValue(iesp, 2, ((Constraints.IsoV(myU0, myU1, myV1)).MaxErrors())->Value(iesp, 1));
    HERISO->SetValue(iesp, 3, ((Constraints.IsoU(myU0, myV0, myV1)).MaxErrors())->Value(iesp, 1));
    HERISO->SetValue(iesp, 4, ((Constraints.IsoU(myU1, myV0, myV1)).MaxErrors())->Value(iesp, 1));

    // calculate max errors at the corners
    double emax1 = 0., emax2 = 0., emax3 = 0., emax4 = 0., err1, err2, err3, err4;
    for (iu = 0; iu <= myOrdInU; iu++)
    {
      for (iv = 0; iv <= myOrdInV; iv++)
      {
        error = (Constraints.Node(myU0, myV0))->Error(iu, iv);
        emax1 = std::max(emax1, error);
        error = (Constraints.Node(myU1, myV0))->Error(iu, iv);
        emax2 = std::max(emax2, error);
        error = (Constraints.Node(myU0, myV1))->Error(iu, iv);
        emax3 = std::max(emax3, error);
        error = (Constraints.Node(myU1, myV1))->Error(iu, iv);
        emax4 = std::max(emax4, error);
      }
    }

    // calculate max errors on borders
    err1 = std::max(emax1, emax2);
    err2 = std::max(emax3, emax4);
    err3 = std::max(emax1, emax3);
    err4 = std::max(emax2, emax4);

    //   calculate final errors on internal isos
    if ((Constraints.IsoV(myU0, myU1, myV0)).Position() == 0)
    {
      HERISO->ChangeValue(iesp, 1) += err1 * hmax[myOrdInU + 1];
    }
    if ((Constraints.IsoV(myU0, myU1, myV1)).Position() == 0)
    {
      HERISO->ChangeValue(iesp, 2) += err2 * hmax[myOrdInU + 1];
    }
    if ((Constraints.IsoU(myU0, myV0, myV1)).Position() == 0)
    {
      HERISO->ChangeValue(iesp, 3) += err3 * hmax[myOrdInV + 1];
    }
    if ((Constraints.IsoU(myU1, myV0, myV1)).Position() == 0)
    {
      HERISO->ChangeValue(iesp, 4) += err4 * hmax[myOrdInV + 1];
    }
    myIsoErrors = HERISO;
  }
}

//=================================================================================================

void AdvApp2Var_Patch::MakeApprox(const AdvApp2Var_Context&   Conditions,
                                  const AdvApp2Var_Framework& Constraints,
                                  const int                   NumDec)
{

  // data stored in the Context
  int NDIMEN, NBSESP, NDIMSE;
  int NBPNTU, NBPNTV, NCFLMU, NCFLMV, NDJACU, NDJACV;
  int NDegU, NDegV, NJacU, NJacV;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
  NDIMSE = 3;
  NBPNTU = (Conditions.URoots())->Length();
  if (myOrdInU > -1)
    NBPNTU -= 2;
  NBPNTV = (Conditions.VRoots())->Length();
  if (myOrdInV > -1)
    NBPNTV -= 2;
  NCFLMU = Conditions.ULimit();
  NCFLMV = Conditions.VLimit();
  NDegU  = NCFLMU - 1;
  NDegV  = NCFLMV - 1;
  NDJACU = Conditions.UJacDeg();
  NDJACV = Conditions.VJacDeg();
  NJacU  = NDJACU + 1;
  NJacV  = NDJACV + 1;

  // data relative to the processed patch
  int IORDRU = myOrdInU, IORDRV = myOrdInV, NDMINU = 1, NDMINV = 1, NCOEFU, NCOEFV;
  // NDMINU and NDMINV depend on the nb of coeff of neighboring isos
  // and of the required order of continuity
  NDMINU = std::max(1, 2 * IORDRU + 1);
  NCOEFU = (Constraints.IsoV(myU0, myU1, myV0)).NbCoeff() - 1;
  NDMINU = std::max(NDMINU, NCOEFU);
  NCOEFU = (Constraints.IsoV(myU0, myU1, myV1)).NbCoeff() - 1;
  NDMINU = std::max(NDMINU, NCOEFU);

  NDMINV = std::max(1, 2 * IORDRV + 1);
  NCOEFV = (Constraints.IsoU(myU0, myV0, myV1)).NbCoeff() - 1;
  NDMINV = std::max(NDMINV, NCOEFV);
  NCOEFV = (Constraints.IsoU(myU1, myV0, myV1)).NbCoeff() - 1;
  NDMINV = std::max(NDMINV, NCOEFV);

  // tables of approximations
  occ::handle<NCollection_HArray1<double>> HEPSAPR = new NCollection_HArray1<double>(1, NBSESP);
  occ::handle<NCollection_HArray1<double>> HEPSFRO = new NCollection_HArray1<double>(1, NBSESP * 8);
  int                                      iesp;
  for (iesp = 1; iesp <= NBSESP; iesp++)
  {
    HEPSAPR->SetValue(iesp, (Conditions.IToler())->Value(iesp));
    HEPSFRO->SetValue(iesp, (Conditions.FToler())->Value(iesp, 1));
    HEPSFRO->SetValue(iesp + NBSESP, (Conditions.FToler())->Value(iesp, 2));
    HEPSFRO->SetValue(iesp + 2 * NBSESP, (Conditions.FToler())->Value(iesp, 3));
    HEPSFRO->SetValue(iesp + 3 * NBSESP, (Conditions.FToler())->Value(iesp, 4));
    HEPSFRO->SetValue(iesp + 4 * NBSESP, (Conditions.CToler())->Value(iesp, 1));
    HEPSFRO->SetValue(iesp + 5 * NBSESP, (Conditions.CToler())->Value(iesp, 2));
    HEPSFRO->SetValue(iesp + 6 * NBSESP, (Conditions.CToler())->Value(iesp, 3));
    HEPSFRO->SetValue(iesp + 7 * NBSESP, (Conditions.CToler())->Value(iesp, 4));
  }
  double* EPSAPR = (double*)&HEPSAPR->ChangeArray1()(HEPSAPR->Lower());
  double* EPSFRO = (double*)&HEPSFRO->ChangeArray1()(HEPSFRO->Lower());

  int                                      SIZE   = (1 + NDJACU) * (1 + NDJACV) * NDIMEN;
  occ::handle<NCollection_HArray1<double>> HPJAC  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  PATJAC = (double*)&HPJAC->ChangeArray1()(HPJAC->Lower());
  SIZE                                            = 2 * SIZE;
  occ::handle<NCollection_HArray1<double>> HPAUX  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  PATAUX = (double*)&HPAUX->ChangeArray1()(HPAUX->Lower());
  SIZE                                            = NCFLMU * NCFLMV * NDIMEN;
  occ::handle<NCollection_HArray1<double>> HPCAN  = new NCollection_HArray1<double>(1, SIZE);
  double*                                  PATCAN = (double*)&HPCAN->ChangeArray1()(HPCAN->Lower());
  occ::handle<NCollection_HArray1<double>> HERRMAX = new NCollection_HArray1<double>(1, NBSESP);
  double* ERRMAX = (double*)&HERRMAX->ChangeArray1()(HERRMAX->Lower());
  occ::handle<NCollection_HArray1<double>> HERRMOY = new NCollection_HArray1<double>(1, NBSESP);
  double* ERRMOY = (double*)&HERRMOY->ChangeArray1()(HERRMOY->Lower());

  // tables of discretization of the square
  double* SOSOTB = (double*)&mySosoTab->ChangeArray1()(mySosoTab->Lower());
  double* DISOTB = (double*)&myDisoTab->ChangeArray1()(myDisoTab->Lower());
  double* SODITB = (double*)&mySodiTab->ChangeArray1()(mySodiTab->Lower());
  double* DIDITB = (double*)&myDidiTab->ChangeArray1()(myDidiTab->Lower());

  //  approximation
  int ITYDEC = 0, IERCOD = 0;
  int iun = 1, itrois = 3;
  NCOEFU = 0;
  NCOEFV = 0;
  AdvApp2Var_ApproxF2var::mma2ce1_((integer*)&NumDec,
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

  // results
  myCutSense = ITYDEC;
  if (ITYDEC == 0 && IERCOD <= 0)
  {
    myHasResult  = true;
    myApprIsDone = (IERCOD == 0);
    myNbCoeffInU = NCOEFU + 1;
    myNbCoeffInV = NCOEFV + 1;
    myMaxErrors  = HERRMAX;
    myMoyErrors  = HERRMOY;

    // Passage to canonic on [-1,1]
    AdvApp2Var_MathBase::mmfmca9_(&NJacU,
                                  &NJacV,
                                  &NDIMEN,
                                  &myNbCoeffInU,
                                  &myNbCoeffInV,
                                  &NDIMEN,
                                  PATJAC,
                                  PATJAC);
    AdvApp2Var_ApproxF2var::mma2can_(&NCFLMU,
                                     &NCFLMV,
                                     &NDIMEN,
                                     &myOrdInU,
                                     &myOrdInV,
                                     &myNbCoeffInU,
                                     &myNbCoeffInV,
                                     PATJAC,
                                     PATAUX,
                                     PATCAN,
                                     &IERCOD);
    if (IERCOD != 0)
    {
      throw Standard_ConstructionError("AdvApp2Var_Patch::MakeApprox : Error in FORTRAN");
    }
    myEquation = HPCAN;

    // Add constraints and errors
    AddConstraints(Conditions, Constraints);
    AddErrors(Constraints);

    // Reduction of degrees if possible
    PATCAN = (double*)&myEquation->ChangeArray1()(myEquation->Lower());

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
    int aIU, aIN, dim, ii, jj;
    for (dim = 1; dim <= NDIMEN; dim++)
    {
      aIN = (dim - 1) * NCFLMU * NCFLMV;
      for (ii = 1; ii <= NCFLMU; ii++)
      {
        aIU = (ii - 1) * NDIMEN * NCFLMV;
        for (jj = 1; jj <= NCFLMV; jj++)
        {
          HPAUX->SetValue(dim + NDIMEN * (jj - 1) + aIU,
                          myEquation->Value(ii + NCFLMU * (jj - 1) + aIN));
        }
      }
    }
    myEquation = HPAUX;
  }
  else
  {
    myApprIsDone = false;
    myHasResult  = false;
  }
}

//=================================================================================================

void AdvApp2Var_Patch::ChangeDomain(const double a, const double b, const double c, const double d)
{
  myU0 = a;
  myU1 = b;
  myV0 = c;
  myV1 = d;
}

//============================================================================
// function : ResetApprox
// purpose  : allows removing a result when it is necessary to cut
//============================================================================

void AdvApp2Var_Patch::ResetApprox()
{
  myApprIsDone = false;
  myHasResult  = false;
}

//============================================================================
// function : OverwriteApprox
// purpose  : allows preserving a result even if the precision is not satisfactory
//============================================================================

void AdvApp2Var_Patch::OverwriteApprox()
{
  if (myHasResult)
    myApprIsDone = true;
}

//=================================================================================================

double AdvApp2Var_Patch::U0() const
{
  return myU0;
}

//=================================================================================================

double AdvApp2Var_Patch::U1() const
{
  return myU1;
}

//=================================================================================================

double AdvApp2Var_Patch::V0() const
{
  return myV0;
}

//=================================================================================================

double AdvApp2Var_Patch::V1() const
{
  return myV1;
}

//=================================================================================================

int AdvApp2Var_Patch::UOrder() const
{
  return myOrdInU;
}

//=================================================================================================

int AdvApp2Var_Patch::VOrder() const
{
  return myOrdInV;
}

//============================================================================
// function : CutSense without Critere
// purpose  : 0 : OK; 1 : required cut by U;
//           2 : required cut by V; 3 : required cut by U and by V
//============================================================================

int AdvApp2Var_Patch::CutSense() const
{
  return myCutSense;
}

//============================================================================
// function : CutSense with critere
// purpose  : 0 : OK; 1 : required cut by U;
//           2 : required cut by V; 3 : required cut by U and by V
//============================================================================

int AdvApp2Var_Patch::CutSense(const AdvApp2Var_Criterion& Crit, const int NumDec) const
{
  bool CritRel = (Crit.Type() == AdvApp2Var_Relative);
  if (CritRel && !IsApproximated())
  {
    return myCutSense;
  }
  else
  {
    if (Crit.IsSatisfied(*this))
    {
      return 0;
    }
    else
    {
      return NumDec;
    }
  }
}

//=================================================================================================

int AdvApp2Var_Patch::NbCoeffInU() const
{
  return myNbCoeffInU;
}

//=================================================================================================

int AdvApp2Var_Patch::NbCoeffInV() const
{
  return myNbCoeffInV;
}

//============================================================================
// function : ChangeNbCoeff
// purpose  : allows increasing the nb of coeff (cf Network)
//============================================================================

void AdvApp2Var_Patch::ChangeNbCoeff(const int NbCoeffU, const int NbCoeffV)
{
  if (myNbCoeffInU < NbCoeffU)
    myNbCoeffInU = NbCoeffU;
  if (myNbCoeffInV < NbCoeffV)
    myNbCoeffInV = NbCoeffV;
}

//============================================================================
// function : MaxErrors
// purpose  : returns max errors of polynomial approximation
//============================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Patch::MaxErrors() const
{
  return myMaxErrors;
}

//============================================================================
// function : AverageErrors
// purpose  : returns average errors of polynomial approximation
//============================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Patch::AverageErrors() const
{
  return myMoyErrors;
}

//============================================================================
// function : IsoErrors
// purpose  : returns max errors on borders of polynomial approximation
//============================================================================

occ::handle<NCollection_HArray2<double>> AdvApp2Var_Patch::IsoErrors() const
{
  return myIsoErrors;
}

//============================================================================
// function : Poles
// purpose  : returns poles of the polynomial approximation
//============================================================================

occ::handle<NCollection_HArray2<gp_Pnt>> AdvApp2Var_Patch::Poles(
  const int                 SSPIndex,
  const AdvApp2Var_Context& Cond) const
{
  occ::handle<NCollection_HArray1<double>> SousEquation;
  if (Cond.TotalNumberSSP() == 1 && SSPIndex == 1)
  {
    SousEquation = myEquation;
  }
  else
  {
    throw Standard_ConstructionError("AdvApp2Var_Patch::Poles :  SSPIndex out of range");
  }
  occ::handle<NCollection_HArray1<double>> Intervalle = new (NCollection_HArray1<double>)(1, 2);
  Intervalle->SetValue(1, -1);
  Intervalle->SetValue(2, 1);

  occ::handle<NCollection_HArray1<int>> NbCoeff = new (NCollection_HArray1<int>)(1, 2);
  NbCoeff->SetValue(1, myNbCoeffInU);
  NbCoeff->SetValue(2, myNbCoeffInV);

  // Conversion
  Convert_GridPolynomialToPoles Conv(Cond.ULimit() - 1,
                                     Cond.VLimit() - 1,
                                     NbCoeff,
                                     SousEquation,
                                     Intervalle,
                                     Intervalle);

  return new NCollection_HArray2<gp_Pnt>(Conv.Poles());
}

//============================================================================
// function : Coefficients
// purpose  : returns coeff. of the equation of polynomial approximation
//============================================================================

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Patch::Coefficients(
  const int                 SSPIndex,
  const AdvApp2Var_Context& Cond) const
{
  occ::handle<NCollection_HArray1<double>> SousEquation;
  if (Cond.TotalNumberSSP() == 1 && SSPIndex == 1)
  {
    SousEquation = myEquation;
  }
  else
  {
    throw Standard_ConstructionError("AdvApp2Var_Patch::Poles :  SSPIndex out of range");
  }
  return SousEquation;
}

//=================================================================================================

double AdvApp2Var_Patch::CritValue() const
{
  return myCritValue;
}

//=================================================================================================

void AdvApp2Var_Patch::SetCritValue(const double dist)
{
  myCritValue = dist;
}
