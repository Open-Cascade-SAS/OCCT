// Created on: 1999-06-22
// Created by: Roman LYGIN
// Copyright (c) 1999-1999 Matra Datavision
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

//    pdn 13.07.99 Derivatives are scaled in accordance with local/global parameter transition

#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <ShapeExtend_ComplexCurve.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_ComplexCurve, Geom_Curve)

//=================================================================================================

ShapeExtend_ComplexCurve::ShapeExtend_ComplexCurve()
{
  myClosed = false;
}

//=================================================================================================

void ShapeExtend_ComplexCurve::Transform(const gp_Trsf& T)
{
  for (int i = 1; i <= NbCurves(); i++)
    Curve(i)->Transform(T);
}

//=================================================================================================

gp_Pnt ShapeExtend_ComplexCurve::EvalD0(const double U) const
{
  double    UOut;
  const int anInd = LocateParameter(U, UOut);
  return Curve(anInd)->EvalD0(UOut);
}

//=================================================================================================

Geom_Curve::ResD1 ShapeExtend_ComplexCurve::EvalD1(const double U) const
{
  double            UOut;
  const int         anInd   = LocateParameter(U, UOut);
  Geom_Curve::ResD1 aResult = Curve(anInd)->EvalD1(UOut);
  TransformDN(aResult.D1, anInd, 1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 ShapeExtend_ComplexCurve::EvalD2(const double U) const
{
  double            UOut;
  const int         anInd   = LocateParameter(U, UOut);
  Geom_Curve::ResD2 aResult = Curve(anInd)->EvalD2(UOut);
  TransformDN(aResult.D1, anInd, 1);
  TransformDN(aResult.D2, anInd, 2);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 ShapeExtend_ComplexCurve::EvalD3(const double U) const
{
  double            UOut;
  const int         anInd   = LocateParameter(U, UOut);
  Geom_Curve::ResD3 aResult = Curve(anInd)->EvalD3(UOut);
  TransformDN(aResult.D1, anInd, 1);
  TransformDN(aResult.D2, anInd, 2);
  TransformDN(aResult.D3, anInd, 3);
  return aResult;
}

//=================================================================================================

gp_Vec ShapeExtend_ComplexCurve::EvalDN(const double U, const int N) const
{
  double    UOut;
  const int anInd   = LocateParameter(U, UOut);
  gp_Vec    aResult = Curve(anInd)->EvalDN(UOut, N);
  if (N)
    TransformDN(aResult, anInd, N);
  return aResult;
}

//=================================================================================================

bool ShapeExtend_ComplexCurve::CheckConnectivity(const double Preci)
{
  int  NbC = NbCurves();
  bool ok  = true;
  for (int i = 1; i < NbC; i++)
  {
    if (i == 1)
      myClosed = Value(FirstParameter()).IsEqual(Value(LastParameter()), Preci);
    ok &= Curve(i)
            ->Value(Curve(i)->LastParameter())
            .IsEqual(Curve(i + 1)->Value(Curve(i + 1)->FirstParameter()), Preci);
  }
#ifdef OCCT_DEBUG
  if (!ok)
    std::cout << "Warning: ShapeExtend_ComplexCurve: not connected in 3d" << std::endl;
#endif
  return ok;
}

//=================================================================================================

void ShapeExtend_ComplexCurve::TransformDN(gp_Vec& V, const int ind, const int N) const
{
  double fact = GetScaleFactor(ind);
  for (int i = 1; i <= N; i++)
    V *= fact;
}
