// Created on: 1995-04-24
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <ChFiDS_ChamfSpine.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_ChamfSpine, ChFiDS_Spine)

//=================================================================================================

ChFiDS_ChamfSpine::ChFiDS_ChamfSpine()
    : d1(0.0),
      d2(0.0),
      angle(0.0),
      mChamf(ChFiDS_Sym)
{
  myMode = ChFiDS_ClassicChamfer;
}

ChFiDS_ChamfSpine::ChFiDS_ChamfSpine(const double Tol)
    : ChFiDS_Spine(Tol),
      d1(0.0),
      d2(0.0),
      angle(0.0),
      mChamf(ChFiDS_Sym)
{
  myMode = ChFiDS_ClassicChamfer;
}

//=================================================================================================

void ChFiDS_ChamfSpine::GetDist(double& Dis) const
{
  if (mChamf != ChFiDS_Sym)
  {
    throw Standard_Failure("Chamfer is not symmetric");
  }
  Dis = d1;
}

//=================================================================================================

void ChFiDS_ChamfSpine::SetDist(const double Dis)
{
  // isconstant->Init(true);
  mChamf = ChFiDS_Sym;
  d1     = Dis;
}

//=================================================================================================

void ChFiDS_ChamfSpine::Dists(double& Dis1, double& Dis2) const
{
  if (mChamf != ChFiDS_TwoDist)
    throw Standard_Failure("Chamfer is not a Two Dists Chamfer");
  Dis1 = d1;
  Dis2 = d2;
}

//=================================================================================================

void ChFiDS_ChamfSpine::SetDists(const double Dis1, const double Dis2)
{
  // isconstant->Init(true);
  mChamf = ChFiDS_TwoDist;
  d1     = Dis1;
  d2     = Dis2;
}

//=================================================================================================

void ChFiDS_ChamfSpine::GetDistAngle(double& Dis, double& Angle) const
// bool& DisOnF1)const
{
  if (mChamf != ChFiDS_DistAngle)
    throw Standard_Failure("Chamfer is not a Two Dists Chamfer");
  Dis   = d1;
  Angle = angle;
  // DisOnF1 = dison1;
}

//=================================================================================================

void ChFiDS_ChamfSpine::SetDistAngle(const double Dis, const double Angle)
// const bool DisOnF1)
{
  // isconstant->Init(true);
  mChamf = ChFiDS_DistAngle;
  d1     = Dis;
  angle  = Angle;
  // dison1 = DisOnF1;
}

//=================================================================================================

void ChFiDS_ChamfSpine::SetMode(const ChFiDS_ChamfMode theMode)
{
  myMode = theMode;
}

//=================================================================================================

ChFiDS_ChamfMethod ChFiDS_ChamfSpine::IsChamfer() const
{

  return mChamf;
}
