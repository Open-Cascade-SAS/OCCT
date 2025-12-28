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

#include <AdvApp2Var_Iso.hxx>

#include <AdvApp2Var_ApproxF2var.hxx>
#include <AdvApp2Var_Context.hxx>
#include <AdvApp2Var_Node.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AdvApp2Var_Iso, Standard_Transient)

//=================================================================================================

AdvApp2Var_Iso::AdvApp2Var_Iso()
    : myType(GeomAbs_IsoU),
      myConstPar(0.5),
      myU0(0.),
      myU1(1.),
      myV0(0.),
      myV1(1.),
      myPosition(0),
      myExtremOrder(2),
      myDerivOrder(2),
      myNbCoeff(0),
      myApprIsDone(false),
      myHasResult(false)
{
}

//=================================================================================================

AdvApp2Var_Iso::AdvApp2Var_Iso(const GeomAbs_IsoType  type,
                               const double    cte,
                               const double    Ufirst,
                               const double    Ulast,
                               const double    Vfirst,
                               const double    Vlast,
                               const int pos,
                               const int iu,
                               const int iv)
    : myType(type),
      myConstPar(cte),
      myU0(Ufirst),
      myU1(Ulast),
      myV0(Vfirst),
      myV1(Vlast),
      myPosition(pos),
      myNbCoeff(0),
      myApprIsDone(false),
      myHasResult(false)
{
  if (myType == GeomAbs_IsoU)
  {
    myExtremOrder = iv;
    myDerivOrder  = iu;
  }
  else
  {
    myExtremOrder = iu;
    myDerivOrder  = iv;
  }
}

//=================================================================================================

bool AdvApp2Var_Iso::IsApproximated() const
{
  return myApprIsDone;
}

//=================================================================================================

bool AdvApp2Var_Iso::HasResult() const
{
  return myHasResult;
}

//=================================================================================================

void AdvApp2Var_Iso::MakeApprox(const AdvApp2Var_Context&           Conditions,
                                const double                 U0,
                                const double                 U1,
                                const double                 V0,
                                const double                 V1,
                                const AdvApp2Var_EvaluatorFunc2Var& Func,
                                AdvApp2Var_Node&                    NodeBegin,
                                AdvApp2Var_Node&                    NodeEnd)
{
  // fixed values
  int NBCRMX = 1, NBCRBE;
  // data stored in the Context
  int NDIMEN, NBSESP, NDIMSE;
  NDIMEN = Conditions.TotalDimension();
  NBSESP = Conditions.TotalNumberSSP();
  // Attention : works only in 3D
  NDIMSE = 3;
  // the domain of the grid
  double UVFONC[4];
  UVFONC[0] = U0;
  UVFONC[1] = U1;
  UVFONC[2] = V0;
  UVFONC[3] = V1;

  // data related to the processed iso
  int IORDRE = myExtremOrder, IDERIV = myDerivOrder;
  double    TCONST = myConstPar;

  // data related to the type of the iso
  int              ISOFAV = 0, NBROOT = 0, NDGJAC = 0, NCFLIM = 1;
  double                 TABDEC[2];
  occ::handle<NCollection_HArray1<double>> HUROOT = Conditions.URoots();
  occ::handle<NCollection_HArray1<double>> HVROOT = Conditions.VRoots();
  double*                ROOTLG = NULL;
  switch (myType)
  {
    case GeomAbs_IsoV:
      ISOFAV    = 2;
      TABDEC[0] = myU0;
      TABDEC[1] = myU1;
      UVFONC[0] = myU0;
      UVFONC[1] = myU1;
      NBROOT    = (Conditions.URoots())->Length();
      if (myExtremOrder > -1)
        NBROOT -= 2;
      ROOTLG = (double*)&HUROOT->ChangeArray1()(HUROOT->Lower());
      NDGJAC = Conditions.UJacDeg();
      NCFLIM = Conditions.ULimit();
      break;
    case GeomAbs_IsoU:
      ISOFAV    = 1;
      TABDEC[0] = myV0;
      TABDEC[1] = myV1;
      UVFONC[2] = myV0;
      UVFONC[3] = myV1;
      NBROOT    = (Conditions.VRoots())->Length();
      if (myExtremOrder > -1)
        NBROOT -= 2;
      ROOTLG = (double*)&HVROOT->ChangeArray1()(HVROOT->Lower());
      NDGJAC = Conditions.VJacDeg();
      NCFLIM = Conditions.VLimit();
      break;
      // #ifndef OCCT_DEBUG
      // pkv f
    case GeomAbs_NoneIso:
      // pkv t
    default:
      break;
      // #endif
  }

  // data relative to the position of iso (front or cut line)
  occ::handle<NCollection_HArray1<double>> HEPSAPR = new NCollection_HArray1<double>(1, NBSESP);
  int              iesp;
  switch (myPosition)
  {
    case 0:
      for (iesp = 1; iesp <= NBSESP; iesp++)
      {
        HEPSAPR->SetValue(iesp, (Conditions.CToler())->Value(iesp, 1));
      }
      break;
    case 1:
      for (iesp = 1; iesp <= NBSESP; iesp++)
      {
        HEPSAPR->SetValue(iesp, (Conditions.FToler())->Value(iesp, 1));
      }
      break;
    case 2:
      for (iesp = 1; iesp <= NBSESP; iesp++)
      {
        HEPSAPR->SetValue(iesp, (Conditions.FToler())->Value(iesp, 2));
      }
      break;
    case 3:
      for (iesp = 1; iesp <= NBSESP; iesp++)
      {
        HEPSAPR->SetValue(iesp, (Conditions.FToler())->Value(iesp, 3));
      }
      break;
    case 4:
      for (iesp = 1; iesp <= NBSESP; iesp++)
      {
        HEPSAPR->SetValue(iesp, (Conditions.FToler())->Value(iesp, 4));
      }
      break;
  }
  double* EPSAPR = (double*)&HEPSAPR->ChangeArray1()(HEPSAPR->Lower());

  // the tables of approximations
  int              SZCRB   = NDIMEN * NCFLIM;
  occ::handle<NCollection_HArray1<double>> HCOURBE = new NCollection_HArray1<double>(1, SZCRB * (IDERIV + 1));
  double*                COURBE = (double*)&HCOURBE->ChangeArray1()(HCOURBE->Lower());
  double*                CRBAPP = COURBE;
  int              SZTAB  = (1 + NBROOT / 2) * NDIMEN;
  occ::handle<NCollection_HArray1<double>> HSOMTAB = new NCollection_HArray1<double>(1, SZTAB * (IDERIV + 1));
  double*                SOMTAB = (double*)&HSOMTAB->ChangeArray1()(HSOMTAB->Lower());
  double*                SOMAPP = SOMTAB;
  occ::handle<NCollection_HArray1<double>> HDIFTAB = new NCollection_HArray1<double>(1, SZTAB * (IDERIV + 1));
  double*                DIFTAB = (double*)&HDIFTAB->ChangeArray1()(HDIFTAB->Lower());
  double*                DIFAPP = DIFTAB;
  occ::handle<NCollection_HArray1<double>> HCONTR1 = new NCollection_HArray1<double>(1, (IORDRE + 2) * NDIMEN);
  double*                CONTR1 = (double*)&HCONTR1->ChangeArray1()(HCONTR1->Lower());
  occ::handle<NCollection_HArray1<double>> HCONTR2 = new NCollection_HArray1<double>(1, (IORDRE + 2) * NDIMEN);
  double*                CONTR2 = (double*)&HCONTR2->ChangeArray1()(HCONTR2->Lower());
  occ::handle<NCollection_HArray2<double>> HERRMAX = new NCollection_HArray2<double>(1, NBSESP, 1, IDERIV + 1);
  double*                EMXAPP  = new double[NBSESP];
  occ::handle<NCollection_HArray2<double>> HERRMOY = new NCollection_HArray2<double>(1, NBSESP, 1, IDERIV + 1);
  // #ifdef OCCT_DEBUG
  // double *ERRMOY =
  // #endif
  //   (double *) &HERRMOY->ChangeArray2()(HERRMOY ->LowerRow(),HERRMOY ->LowerCol());
  double* EMYAPP = new double[NBSESP];
  //
  // the approximations
  //
  int IERCOD = 0, NCOEFF = 0;
  int iapp, ncfapp, ierapp;
  //  int id,ic,ideb;
  for (iapp = 0; iapp <= IDERIV; iapp++)
  {
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
                                     /*(void *)*/ Func,
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
    if (ierapp > 0)
    {
      myApprIsDone = false;
      myHasResult  = false;
      goto FINISH;
    }
    if (NCOEFF <= ncfapp)
      NCOEFF = ncfapp;
    if (ierapp == -1)
      IERCOD = -1;
    //   return constraints of order 0 to IORDRE of extremities
    int ider, jpos = HCONTR1->Lower();
    for (ider = 0; ider <= IORDRE; ider++)
    {
      gp_Pnt pt(HCONTR1->Value(jpos), HCONTR1->Value(jpos + 1), HCONTR1->Value(jpos + 2));
      if (ISOFAV == 2)
      {
        NodeBegin.SetPoint(ider, iapp, pt);
      }
      else
      {
        NodeBegin.SetPoint(iapp, ider, pt);
      }
      jpos += 3;
    }
    jpos = HCONTR2->Lower();
    for (ider = 0; ider <= IORDRE; ider++)
    {
      gp_Pnt pt(HCONTR2->Value(jpos), HCONTR2->Value(jpos + 1), HCONTR2->Value(jpos + 2));
      if (ISOFAV == 2)
      {
        NodeEnd.SetPoint(ider, iapp, pt);
      }
      else
      {
        NodeEnd.SetPoint(iapp, ider, pt);
      }
      jpos += 3;
    }
    //   return errors
    for (iesp = 1; iesp <= NBSESP; iesp++)
    {
      HERRMAX->SetValue(iesp, iapp + 1, EMXAPP[iesp - 1]);
      HERRMOY->SetValue(iesp, iapp + 1, EMYAPP[iesp - 1]);
    }
    // passage to the approximation of higher order
    CRBAPP += SZCRB;
    SOMAPP += SZTAB;
    DIFAPP += SZTAB;
  }

  // management of results
  if (IERCOD == 0)
  {
    //   all approximations are correct
    myApprIsDone = true;
    myHasResult  = true;
  }
  else if (IERCOD == -1)
  {
    //   at least one approximation is not correct
    myApprIsDone = false;
    myHasResult  = true;
  }
  else
  {
    myApprIsDone = false;
    myHasResult  = false;
  }
  if (myHasResult)
  {
    myEquation  = HCOURBE;
    myNbCoeff   = NCOEFF;
    myMaxErrors = HERRMAX;
    myMoyErrors = HERRMOY;
    mySomTab    = HSOMTAB;
    myDifTab    = HDIFTAB;
  }
FINISH:
  delete[] EMXAPP;
  delete[] EMYAPP;
}

//=================================================================================================

void AdvApp2Var_Iso::ChangeDomain(const double a, const double b)
{
  if (myType == GeomAbs_IsoU)
  {
    myV0 = a;
    myV1 = b;
  }
  else
  {
    myU0 = a;
    myU1 = b;
  }
}

//=================================================================================================

void AdvApp2Var_Iso::ChangeDomain(const double a,
                                  const double b,
                                  const double c,
                                  const double d)
{
  myU0 = a;
  myU1 = b;
  myV0 = c;
  myV1 = d;
}

//=================================================================================================

void AdvApp2Var_Iso::SetConstante(const double newcte)
{
  myConstPar = newcte;
}

//=================================================================================================

void AdvApp2Var_Iso::SetPosition(const int newpos)
{
  myPosition = newpos;
}

//=================================================================================================

void AdvApp2Var_Iso::ResetApprox()
{
  myApprIsDone = false;
  myHasResult  = false;
}

//=================================================================================================

void AdvApp2Var_Iso::OverwriteApprox()
{
  if (myHasResult)
    myApprIsDone = true;
}

//=================================================================================================

GeomAbs_IsoType AdvApp2Var_Iso::Type() const
{
  return myType;
}

//=================================================================================================

double AdvApp2Var_Iso::Constante() const
{
  return myConstPar;
}

//=================================================================================================

double AdvApp2Var_Iso::T0() const
{
  if (myType == GeomAbs_IsoU)
  {
    return myV0;
  }
  else
  {
    return myU0;
  }
}

//=================================================================================================

double AdvApp2Var_Iso::T1() const
{
  if (myType == GeomAbs_IsoU)
  {
    return myV1;
  }
  else
  {
    return myU1;
  }
}

//=================================================================================================

double AdvApp2Var_Iso::U0() const
{
  return myU0;
}

//=================================================================================================

double AdvApp2Var_Iso::U1() const
{
  return myU1;
}

//=================================================================================================

double AdvApp2Var_Iso::V0() const
{
  return myV0;
}

//=================================================================================================

double AdvApp2Var_Iso::V1() const
{
  return myV1;
}

//=================================================================================================

int AdvApp2Var_Iso::UOrder() const
{
  if (Type() == GeomAbs_IsoU)
    return myDerivOrder;
  else
    return myExtremOrder;
}

//=================================================================================================

int AdvApp2Var_Iso::VOrder() const
{
  if (Type() == GeomAbs_IsoV)
    return myDerivOrder;
  else
    return myExtremOrder;
}

//=================================================================================================

int AdvApp2Var_Iso::Position() const
{
  return myPosition;
}

//=================================================================================================

int AdvApp2Var_Iso::NbCoeff() const
{
  return myNbCoeff;
}

//=================================================================================================

const occ::handle<NCollection_HArray1<double>>& AdvApp2Var_Iso::Polynom() const
{
  return myEquation;
}

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Iso::SomTab() const
{
  return mySomTab;
}

occ::handle<NCollection_HArray1<double>> AdvApp2Var_Iso::DifTab() const
{
  return myDifTab;
}

occ::handle<NCollection_HArray2<double>> AdvApp2Var_Iso::MaxErrors() const
{
  return myMaxErrors;
}

occ::handle<NCollection_HArray2<double>> AdvApp2Var_Iso::MoyErrors() const
{
  return myMoyErrors;
}
