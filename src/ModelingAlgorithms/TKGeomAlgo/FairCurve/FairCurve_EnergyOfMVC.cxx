// Created on: 1996-04-01
// Created by: Philippe MANGIN
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

#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_EnergyOfMVC.hxx>
#include <math_GaussSetIntegration.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=====================================================================================
FairCurve_EnergyOfMVC::FairCurve_EnergyOfMVC(
  const int                                         BSplOrder,
  const occ::handle<NCollection_HArray1<double>>&   FlatKnots,
  const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles,
  const int                                         ContrOrder1,
  const int                                         ContrOrder2,
  const FairCurve_BattenLaw&                        Law,
  const double                                      PhysicalRatio,
  const double                                      LengthSliding,
  const bool                                        FreeSliding,
  const double                                      Angle1,
  const double                                      Angle2,
  const double                                      Curvature1,
  const double                                      Curvature2)
    //=====================================================================================
    : FairCurve_Energy(Poles,
                       ContrOrder1,
                       ContrOrder2,
                       FreeSliding,
                       Angle1,
                       Angle2,
                       BSplOrder - 1,
                       Curvature1,
                       Curvature2),
      MyLengthSliding(LengthSliding),
      OriginalSliding(LengthSliding),
      MyBattenLaw(Law),
      MyPhysicalRatio(PhysicalRatio),
      MyTension(BSplOrder, FlatKnots, Poles, 1, LengthSliding, Law, FreeSliding, true),
      MySagging(BSplOrder, FlatKnots, Poles, 1, Law, FreeSliding),
      MyJerk(BSplOrder, FlatKnots, Poles, 1, Law, FreeSliding),
      MyStatus(FairCurve_OK)
{
  Standard_DomainError_Raise_if(PhysicalRatio < 0 || PhysicalRatio > 1,
                                "FairCurve_EnergyOfMVC: PhysicalRatio error");
}

//=====================================================================================
void FairCurve_EnergyOfMVC::ComputePoles(const math_Vector& X)
//=====================================================================================
{
  FairCurve_Energy::ComputePoles(X);
  if (MyWithAuxValue)
  {
    MyLengthSliding = X(X.Upper());
  }
}

//=====================================================================================
bool FairCurve_EnergyOfMVC::Variable(math_Vector& X) const
//=====================================================================================
{
  bool Ok;
  Ok = FairCurve_Energy::Variable(X);
  if (MyWithAuxValue)
  {
    X(X.Upper()) = MyLengthSliding;
  }
  return Ok;
}

//=====================================================================================
bool FairCurve_EnergyOfMVC::Compute(const int DerivativeOrder, math_Vector& Result)
//=====================================================================================
{
  math_Vector        Debut(1, 1, 0.), Fin(1, 1, 1.);
  math_IntegerVector MyOrder(1, 1, 24);
  bool               Ok = false;

  // Blindage contre les longueur de glissement trop exotique
  MyStatus = FairCurve_OK;
  if (MyLengthSliding > 10 * OriginalSliding)
  {
    MyStatus = FairCurve_InfiniteSliding;
    return false;
  }
  if (MyLengthSliding < OriginalSliding / 100)
  {
    MyLengthSliding = OriginalSliding / 100;
  }

  // Mise a jour des objets sous-fonction
  MyTension.SetDerivativeOrder(DerivativeOrder);
  MyTension.SetLengthSliding(MyLengthSliding);
  MySagging.SetDerivativeOrder(DerivativeOrder);
  MyJerk.SetDerivativeOrder(DerivativeOrder);
  MyBattenLaw.SetSliding(MyLengthSliding);

  //  Integrations

  // on decoupe afin d'avoir au moins 2 points d'integration par poles
  // 24 points de Gauss => 12 poles maximum.

  int    NbInterv = (MyPoles->Length() - 1) / 12 + 1, ii;
  double Delta    = 1. / NbInterv;
  Result.Init(0);

  if (MyPhysicalRatio <= 1.e-12)
  {

    // Cas purement non physique --------------------------

    for (ii = 1; ii <= NbInterv; ii++)
    {
      Debut(1) = (ii - 1) * Delta;
      Fin(1)   = ii * Delta;

      math_GaussSetIntegration SumTension(MyTension, Debut, Fin, MyOrder);
      Ok = SumTension.IsDone();
      if (!Ok)
        return Ok;

      math_GaussSetIntegration SumJerk(MyJerk, Debut, Fin, MyOrder);
      Ok = SumJerk.IsDone();
      if (!Ok)
        return Ok;

      Result += SumJerk.Value() + SumTension.Value(); // Cas purement non physique
    }
  }
  else
  {
    // Cas mixte  --------------------------
    for (ii = 1; ii <= NbInterv; ii++)
    {
      Debut(1) = (ii - 1) * Delta;
      Fin(1)   = ii * Delta;

      math_GaussSetIntegration SumTension(MyTension, Debut, Fin, MyOrder);
      Ok = SumTension.IsDone();
      if (!Ok)
        return Ok;

      math_GaussSetIntegration SumSagging(MySagging, Debut, Fin, MyOrder);
      Ok = SumSagging.IsDone();
      if (!Ok)
        return Ok;

      math_GaussSetIntegration SumJerk(MyJerk, Debut, Fin, MyOrder);
      Ok = SumJerk.IsDone();
      if (!Ok)
        return Ok;

      Result += SumJerk.Value() * (1 - MyPhysicalRatio) + SumSagging.Value() * MyPhysicalRatio
                + SumTension.Value();
    }
  }

  return Ok;
}
