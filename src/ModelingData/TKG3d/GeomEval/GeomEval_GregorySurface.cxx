// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomEval_GregorySurface.hxx>

#include <Geom_UndefinedDerivative.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

#include <algorithm>
#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(GeomEval_GregorySurface, Geom_BoundedSurface)

namespace
{
  //! Small epsilon for Gregory blending denominator check.
  constexpr double THE_GREGORY_EPS = 1.0e-14;
}

//==================================================================================================

GeomEval_GregorySurface::GeomEval_GregorySurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                                  const gp_Pnt theInteriorPairs[4][2])
    : myPoles(1, 4, 1, 4)
{
  if (thePoles.NbRows() != 4 || thePoles.NbColumns() != 4)
  {
    throw Standard_ConstructionError(
      "GeomEval_GregorySurface: poles array must be 4x4");
  }
  for (int aRow = 1; aRow <= 4; ++aRow)
  {
    for (int aCol = 1; aCol <= 4; ++aCol)
    {
      myPoles.SetValue(aRow, aCol, thePoles.Value(aRow, aCol));
    }
  }
  for (int aCorner = 0; aCorner < 4; ++aCorner)
  {
    myInteriorPairs[aCorner][0] = theInteriorPairs[aCorner][0];
    myInteriorPairs[aCorner][1] = theInteriorPairs[aCorner][1];
  }
}

//==================================================================================================

const NCollection_Array2<gp_Pnt>& GeomEval_GregorySurface::Poles() const
{
  return myPoles;
}

//==================================================================================================

const gp_Pnt& GeomEval_GregorySurface::InteriorPoint(int theCorner, int theIndex) const
{
  if (theCorner < 0 || theCorner > 3 || theIndex < 0 || theIndex > 1)
  {
    throw Standard_ConstructionError(
      "GeomEval_GregorySurface::InteriorPoint: invalid corner or index");
  }
  return myInteriorPairs[theCorner][theIndex];
}

//==================================================================================================

void GeomEval_GregorySurface::UReverse()
{
  // Swap columns: col 1 <-> col 4, col 2 <-> col 3
  for (int aRow = 1; aRow <= 4; ++aRow)
  {
    std::swap(myPoles.ChangeValue(aRow, 1), myPoles.ChangeValue(aRow, 4));
    std::swap(myPoles.ChangeValue(aRow, 2), myPoles.ChangeValue(aRow, 3));
  }

  // Swap interior pairs: corner 0 <-> corner 1, corner 2 <-> corner 3
  // Corner 0 is at (u=0,v=0), corner 1 at (u=1,v=0) -> after U-reverse they swap.
  // Corner 2 is at (u=0,v=1), corner 3 at (u=1,v=1) -> after U-reverse they swap.
  for (int k = 0; k < 2; ++k)
  {
    std::swap(myInteriorPairs[0][k], myInteriorPairs[1][k]);
    std::swap(myInteriorPairs[2][k], myInteriorPairs[3][k]);
  }
}

//==================================================================================================

double GeomEval_GregorySurface::UReversedParameter(const double U) const
{
  return 1.0 - U;
}

//==================================================================================================

void GeomEval_GregorySurface::VReverse()
{
  // Swap rows: row 1 <-> row 4, row 2 <-> row 3
  for (int aCol = 1; aCol <= 4; ++aCol)
  {
    std::swap(myPoles.ChangeValue(1, aCol), myPoles.ChangeValue(4, aCol));
    std::swap(myPoles.ChangeValue(2, aCol), myPoles.ChangeValue(3, aCol));
  }

  // Swap interior pairs: corner 0 <-> corner 2, corner 1 <-> corner 3
  // Corner 0 is at (u=0,v=0), corner 2 at (u=0,v=1) -> after V-reverse they swap.
  // Corner 1 is at (u=1,v=0), corner 3 at (u=1,v=1) -> after V-reverse they swap.
  for (int k = 0; k < 2; ++k)
  {
    std::swap(myInteriorPairs[0][k], myInteriorPairs[2][k]);
    std::swap(myInteriorPairs[1][k], myInteriorPairs[3][k]);
  }
}

//==================================================================================================

double GeomEval_GregorySurface::VReversedParameter(const double V) const
{
  return 1.0 - V;
}

//==================================================================================================

void GeomEval_GregorySurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = 0.0;
  U2 = 1.0;
  V1 = 0.0;
  V2 = 1.0;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsUClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsVClosed() const
{
  return false;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsUPeriodic() const
{
  return false;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsVPeriodic() const
{
  return false;
}

//==================================================================================================

GeomAbs_Shape GeomEval_GregorySurface::Continuity() const
{
  return GeomAbs_C0;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsCNu(const int N) const
{
  return N <= 0;
}

//==================================================================================================

bool GeomEval_GregorySurface::IsCNv(const int N) const
{
  return N <= 0;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_GregorySurface::UIso(const double /*U*/) const
{
  return nullptr;
}

//==================================================================================================

occ::handle<Geom_Curve> GeomEval_GregorySurface::VIso(const double /*V*/) const
{
  return nullptr;
}

//==================================================================================================

double GeomEval_GregorySurface::bernstein3(int theI, double theT)
{
  const double aS = 1.0 - theT;
  switch (theI)
  {
    case 0: return aS * aS * aS;
    case 1: return 3.0 * aS * aS * theT;
    case 2: return 3.0 * aS * theT * theT;
    case 3: return theT * theT * theT;
    default: return 0.0;
  }
}

//==================================================================================================

void GeomEval_GregorySurface::computeEffectivePoles(
  double                     theU,
  double                     theV,
  NCollection_Array2<gp_Pnt>& theEffPoles) const
{
  // Copy all boundary poles directly.
  for (int aRow = 1; aRow <= 4; ++aRow)
  {
    for (int aCol = 1; aCol <= 4; ++aCol)
    {
      theEffPoles.SetValue(aRow, aCol, myPoles.Value(aRow, aCol));
    }
  }

  // Gregory blending at the 4 interior corners.
  // Each interior corner blends two control points rationally based on
  // the parametric distance to the corresponding patch corner.

  // Corner 0: grid position (2,2), parameter corner (u=0, v=0), blend params s=u, t=v
  {
    const double aS = theU;
    const double aT = theV;
    const double aDenom = aS + aT;
    if (aDenom > THE_GREGORY_EPS)
    {
      const double aWu = aS / aDenom;
      const double aWv = aT / aDenom;
      theEffPoles.SetValue(2, 2,
        gp_Pnt(aWu * myInteriorPairs[0][0].X() + aWv * myInteriorPairs[0][1].X(),
               aWu * myInteriorPairs[0][0].Y() + aWv * myInteriorPairs[0][1].Y(),
               aWu * myInteriorPairs[0][0].Z() + aWv * myInteriorPairs[0][1].Z()));
    }
    else
    {
      // Average when at the corner.
      theEffPoles.SetValue(2, 2,
        gp_Pnt(0.5 * (myInteriorPairs[0][0].X() + myInteriorPairs[0][1].X()),
               0.5 * (myInteriorPairs[0][0].Y() + myInteriorPairs[0][1].Y()),
               0.5 * (myInteriorPairs[0][0].Z() + myInteriorPairs[0][1].Z())));
    }
  }

  // Corner 1: grid position (3,2), parameter corner (u=1, v=0), blend params s=(1-u), t=v
  {
    const double aS = 1.0 - theU;
    const double aT = theV;
    const double aDenom = aS + aT;
    if (aDenom > THE_GREGORY_EPS)
    {
      const double aWu = aS / aDenom;
      const double aWv = aT / aDenom;
      theEffPoles.SetValue(3, 2,
        gp_Pnt(aWu * myInteriorPairs[1][0].X() + aWv * myInteriorPairs[1][1].X(),
               aWu * myInteriorPairs[1][0].Y() + aWv * myInteriorPairs[1][1].Y(),
               aWu * myInteriorPairs[1][0].Z() + aWv * myInteriorPairs[1][1].Z()));
    }
    else
    {
      theEffPoles.SetValue(3, 2,
        gp_Pnt(0.5 * (myInteriorPairs[1][0].X() + myInteriorPairs[1][1].X()),
               0.5 * (myInteriorPairs[1][0].Y() + myInteriorPairs[1][1].Y()),
               0.5 * (myInteriorPairs[1][0].Z() + myInteriorPairs[1][1].Z())));
    }
  }

  // Corner 2: grid position (2,3), parameter corner (u=0, v=1), blend params s=u, t=(1-v)
  {
    const double aS = theU;
    const double aT = 1.0 - theV;
    const double aDenom = aS + aT;
    if (aDenom > THE_GREGORY_EPS)
    {
      const double aWu = aS / aDenom;
      const double aWv = aT / aDenom;
      theEffPoles.SetValue(2, 3,
        gp_Pnt(aWu * myInteriorPairs[2][0].X() + aWv * myInteriorPairs[2][1].X(),
               aWu * myInteriorPairs[2][0].Y() + aWv * myInteriorPairs[2][1].Y(),
               aWu * myInteriorPairs[2][0].Z() + aWv * myInteriorPairs[2][1].Z()));
    }
    else
    {
      theEffPoles.SetValue(2, 3,
        gp_Pnt(0.5 * (myInteriorPairs[2][0].X() + myInteriorPairs[2][1].X()),
               0.5 * (myInteriorPairs[2][0].Y() + myInteriorPairs[2][1].Y()),
               0.5 * (myInteriorPairs[2][0].Z() + myInteriorPairs[2][1].Z())));
    }
  }

  // Corner 3: grid position (3,3), parameter corner (u=1, v=1), blend params s=(1-u), t=(1-v)
  {
    const double aS = 1.0 - theU;
    const double aT = 1.0 - theV;
    const double aDenom = aS + aT;
    if (aDenom > THE_GREGORY_EPS)
    {
      const double aWu = aS / aDenom;
      const double aWv = aT / aDenom;
      theEffPoles.SetValue(3, 3,
        gp_Pnt(aWu * myInteriorPairs[3][0].X() + aWv * myInteriorPairs[3][1].X(),
               aWu * myInteriorPairs[3][0].Y() + aWv * myInteriorPairs[3][1].Y(),
               aWu * myInteriorPairs[3][0].Z() + aWv * myInteriorPairs[3][1].Z()));
    }
    else
    {
      theEffPoles.SetValue(3, 3,
        gp_Pnt(0.5 * (myInteriorPairs[3][0].X() + myInteriorPairs[3][1].X()),
               0.5 * (myInteriorPairs[3][0].Y() + myInteriorPairs[3][1].Y()),
               0.5 * (myInteriorPairs[3][0].Z() + myInteriorPairs[3][1].Z())));
    }
  }
}

//==================================================================================================

gp_Pnt GeomEval_GregorySurface::EvalD0(const double U, const double V) const
{
  // Compute effective poles with Gregory blending.
  NCollection_Array2<gp_Pnt> anEffPoles(1, 4, 1, 4);
  computeEffectivePoles(U, V, anEffPoles);

  // Evaluate bi-cubic Bernstein surface.
  // S(u,v) = sum_{i=0}^{3} sum_{j=0}^{3} B_i(u) * B_j(v) * P(i+1,j+1)
  double aX = 0.0;
  double aY = 0.0;
  double aZ = 0.0;
  for (int i = 0; i < 4; ++i)
  {
    const double aBu = bernstein3(i, U);
    for (int j = 0; j < 4; ++j)
    {
      const double aBv  = bernstein3(j, V);
      const double aW   = aBu * aBv;
      const gp_Pnt& aPt = anEffPoles.Value(i + 1, j + 1);
      aX += aW * aPt.X();
      aY += aW * aPt.Y();
      aZ += aW * aPt.Z();
    }
  }
  return gp_Pnt(aX, aY, aZ);
}

//==================================================================================================

double GeomEval_GregorySurface::finiteDiffStep(double theParam, int theDerivOrder)
{
  // Optimal finite-difference step for central differences of order n:
  //   error = O(h^2 * f^(n+2)) + O(eps * f / h^n)
  //   optimal h ~ eps^(1/(n+2)) * max(1, |param|)
  // For double precision eps ~ 1e-16:
  //   n=1: h ~ eps^(1/3) ~ 6e-6
  //   n=2: h ~ eps^(1/4) ~ 1.2e-4
  //   n=3: h ~ eps^(1/5) ~ 2.5e-4
  const double aScale = std::max(1.0, std::abs(theParam));
  switch (theDerivOrder)
  {
    case 1:  return 6.0e-6 * aScale;
    case 2:  return 1.2e-4 * aScale;
    default: return 2.5e-4 * aScale; // n >= 3
  }
}

//==================================================================================================

Geom_Surface::ResD1 GeomEval_GregorySurface::EvalD1(const double U, const double V) const
{
  // Central finite differences for first derivatives.
  Geom_Surface::ResD1 aResult;
  aResult.Point = EvalD0(U, V);

  const double aHu = finiteDiffStep(U, 1);
  const double aHv = finiteDiffStep(V, 1);

  const gp_Pnt aPup = EvalD0(U + aHu, V);
  const gp_Pnt aPum = EvalD0(U - aHu, V);
  aResult.D1U = gp_Vec((aPup.X() - aPum.X()) / (2.0 * aHu),
                       (aPup.Y() - aPum.Y()) / (2.0 * aHu),
                       (aPup.Z() - aPum.Z()) / (2.0 * aHu));

  const gp_Pnt aPvp = EvalD0(U, V + aHv);
  const gp_Pnt aPvm = EvalD0(U, V - aHv);
  aResult.D1V = gp_Vec((aPvp.X() - aPvm.X()) / (2.0 * aHv),
                       (aPvp.Y() - aPvm.Y()) / (2.0 * aHv),
                       (aPvp.Z() - aPvm.Z()) / (2.0 * aHv));

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD2 GeomEval_GregorySurface::EvalD2(const double U, const double V) const
{
  // Central finite differences for second derivatives.
  Geom_Surface::ResD2 aResult;

  const gp_Pnt aP00 = EvalD0(U, V);
  aResult.Point = aP00;

  const double aH1u = finiteDiffStep(U, 1);
  const double aH1v = finiteDiffStep(V, 1);
  const double aH2u = finiteDiffStep(U, 2);
  const double aH2v = finiteDiffStep(V, 2);

  // First derivatives (using optimal D1 step).
  const gp_Pnt aPup1  = EvalD0(U + aH1u, V);
  const gp_Pnt aPum1  = EvalD0(U - aH1u, V);
  const gp_Pnt aPvp1  = EvalD0(U, V + aH1v);
  const gp_Pnt aPvm1  = EvalD0(U, V - aH1v);

  aResult.D1U = gp_Vec((aPup1.X() - aPum1.X()) / (2.0 * aH1u),
                       (aPup1.Y() - aPum1.Y()) / (2.0 * aH1u),
                       (aPup1.Z() - aPum1.Z()) / (2.0 * aH1u));

  aResult.D1V = gp_Vec((aPvp1.X() - aPvm1.X()) / (2.0 * aH1v),
                       (aPvp1.Y() - aPvm1.Y()) / (2.0 * aH1v),
                       (aPvp1.Z() - aPvm1.Z()) / (2.0 * aH1v));

  // Second derivatives (using optimal D2 step): d2P/du2 = (P(u+h) - 2*P(u) + P(u-h)) / h^2
  const gp_Pnt aPup  = EvalD0(U + aH2u, V);
  const gp_Pnt aPum  = EvalD0(U - aH2u, V);
  const gp_Pnt aPvp  = EvalD0(U, V + aH2v);
  const gp_Pnt aPvm  = EvalD0(U, V - aH2v);

  const double aInvHu2 = 1.0 / (aH2u * aH2u);
  aResult.D2U = gp_Vec((aPup.X() - 2.0 * aP00.X() + aPum.X()) * aInvHu2,
                       (aPup.Y() - 2.0 * aP00.Y() + aPum.Y()) * aInvHu2,
                       (aPup.Z() - 2.0 * aP00.Z() + aPum.Z()) * aInvHu2);

  const double aInvHv2 = 1.0 / (aH2v * aH2v);
  aResult.D2V = gp_Vec((aPvp.X() - 2.0 * aP00.X() + aPvm.X()) * aInvHv2,
                       (aPvp.Y() - 2.0 * aP00.Y() + aPvm.Y()) * aInvHv2,
                       (aPvp.Z() - 2.0 * aP00.Z() + aPvm.Z()) * aInvHv2);

  // Mixed derivative: d2P/dudv = (P(u+h,v+h) - P(u+h,v-h) - P(u-h,v+h) + P(u-h,v-h)) / (4*hu*hv)
  const gp_Pnt aPpp = EvalD0(U + aH2u, V + aH2v);
  const gp_Pnt aPpm = EvalD0(U + aH2u, V - aH2v);
  const gp_Pnt aPmp = EvalD0(U - aH2u, V + aH2v);
  const gp_Pnt aPmm = EvalD0(U - aH2u, V - aH2v);
  const double aInv4HuHv = 1.0 / (4.0 * aH2u * aH2v);
  aResult.D2UV = gp_Vec((aPpp.X() - aPpm.X() - aPmp.X() + aPmm.X()) * aInv4HuHv,
                        (aPpp.Y() - aPpm.Y() - aPmp.Y() + aPmm.Y()) * aInv4HuHv,
                        (aPpp.Z() - aPpm.Z() - aPmp.Z() + aPmm.Z()) * aInv4HuHv);

  return aResult;
}

//==================================================================================================

Geom_Surface::ResD3 GeomEval_GregorySurface::EvalD3(const double U, const double V) const
{
  // Use finite differences for all derivatives up to third order.
  Geom_Surface::ResD3 aResult;

  const double aH1u = finiteDiffStep(U, 1);
  const double aH1v = finiteDiffStep(V, 1);
  const double aH2u = finiteDiffStep(U, 2);
  const double aH2v = finiteDiffStep(V, 2);
  const double aH3u = finiteDiffStep(U, 3);
  const double aH3v = finiteDiffStep(V, 3);

  const gp_Pnt aP00 = EvalD0(U, V);
  aResult.Point = aP00;

  // D1: use optimal D1 step
  const gp_Pnt aPup1  = EvalD0(U + aH1u, V);
  const gp_Pnt aPum1  = EvalD0(U - aH1u, V);
  const gp_Pnt aPvp1  = EvalD0(U, V + aH1v);
  const gp_Pnt aPvm1  = EvalD0(U, V - aH1v);

  aResult.D1U = gp_Vec((aPup1.X() - aPum1.X()) / (2.0 * aH1u),
                       (aPup1.Y() - aPum1.Y()) / (2.0 * aH1u),
                       (aPup1.Z() - aPum1.Z()) / (2.0 * aH1u));

  aResult.D1V = gp_Vec((aPvp1.X() - aPvm1.X()) / (2.0 * aH1v),
                       (aPvp1.Y() - aPvm1.Y()) / (2.0 * aH1v),
                       (aPvp1.Z() - aPvm1.Z()) / (2.0 * aH1v));

  // D2: use optimal D2 step
  const gp_Pnt aPup2  = EvalD0(U + aH2u, V);
  const gp_Pnt aPum2  = EvalD0(U - aH2u, V);
  const gp_Pnt aPvp2  = EvalD0(U, V + aH2v);
  const gp_Pnt aPvm2  = EvalD0(U, V - aH2v);

  const double aInvHu2 = 1.0 / (aH2u * aH2u);
  aResult.D2U = gp_Vec((aPup2.X() - 2.0 * aP00.X() + aPum2.X()) * aInvHu2,
                       (aPup2.Y() - 2.0 * aP00.Y() + aPum2.Y()) * aInvHu2,
                       (aPup2.Z() - 2.0 * aP00.Z() + aPum2.Z()) * aInvHu2);

  const double aInvHv2 = 1.0 / (aH2v * aH2v);
  aResult.D2V = gp_Vec((aPvp2.X() - 2.0 * aP00.X() + aPvm2.X()) * aInvHv2,
                       (aPvp2.Y() - 2.0 * aP00.Y() + aPvm2.Y()) * aInvHv2,
                       (aPvp2.Z() - 2.0 * aP00.Z() + aPvm2.Z()) * aInvHv2);

  // Mixed second derivative (using D2 step).
  const gp_Pnt aPpp2 = EvalD0(U + aH2u, V + aH2v);
  const gp_Pnt aPpm2 = EvalD0(U + aH2u, V - aH2v);
  const gp_Pnt aPmp2 = EvalD0(U - aH2u, V + aH2v);
  const gp_Pnt aPmm2 = EvalD0(U - aH2u, V - aH2v);
  const double aInv4HuHv = 1.0 / (4.0 * aH2u * aH2v);
  aResult.D2UV = gp_Vec((aPpp2.X() - aPpm2.X() - aPmp2.X() + aPmm2.X()) * aInv4HuHv,
                        (aPpp2.Y() - aPpm2.Y() - aPmp2.Y() + aPmm2.Y()) * aInv4HuHv,
                        (aPpp2.Z() - aPpm2.Z() - aPmp2.Z() + aPmm2.Z()) * aInv4HuHv);

  // D3: use optimal D3 step
  // d3P/du3 = (P(u+2h) - 2*P(u+h) + 2*P(u-h) - P(u-2h)) / (2*h^3)
  const gp_Pnt aPup3  = EvalD0(U + aH3u, V);
  const gp_Pnt aPum3  = EvalD0(U - aH3u, V);
  const gp_Pnt aPu2p = EvalD0(U + 2.0 * aH3u, V);
  const gp_Pnt aPu2m = EvalD0(U - 2.0 * aH3u, V);
  const double aInv2Hu3 = 1.0 / (2.0 * aH3u * aH3u * aH3u);
  aResult.D3U = gp_Vec((aPu2p.X() - 2.0 * aPup3.X() + 2.0 * aPum3.X() - aPu2m.X()) * aInv2Hu3,
                       (aPu2p.Y() - 2.0 * aPup3.Y() + 2.0 * aPum3.Y() - aPu2m.Y()) * aInv2Hu3,
                       (aPu2p.Z() - 2.0 * aPup3.Z() + 2.0 * aPum3.Z() - aPu2m.Z()) * aInv2Hu3);

  const gp_Pnt aPvp3  = EvalD0(U, V + aH3v);
  const gp_Pnt aPvm3  = EvalD0(U, V - aH3v);
  const gp_Pnt aPv2p = EvalD0(U, V + 2.0 * aH3v);
  const gp_Pnt aPv2m = EvalD0(U, V - 2.0 * aH3v);
  const double aInv2Hv3 = 1.0 / (2.0 * aH3v * aH3v * aH3v);
  aResult.D3V = gp_Vec((aPv2p.X() - 2.0 * aPvp3.X() + 2.0 * aPvm3.X() - aPv2m.X()) * aInv2Hv3,
                       (aPv2p.Y() - 2.0 * aPvp3.Y() + 2.0 * aPvm3.Y() - aPv2m.Y()) * aInv2Hv3,
                       (aPv2p.Z() - 2.0 * aPvp3.Z() + 2.0 * aPvm3.Z() - aPv2m.Z()) * aInv2Hv3);

  // d3P/du2dv and d3P/dudv2 (using D3 step for the mixed terms)
  const gp_Pnt aPpp3 = EvalD0(U + aH3u, V + aH3v);
  const gp_Pnt aPpm3 = EvalD0(U + aH3u, V - aH3v);
  const gp_Pnt aPmp3 = EvalD0(U - aH3u, V + aH3v);
  const gp_Pnt aPmm3 = EvalD0(U - aH3u, V - aH3v);

  const double aInv2Hu2Hv = 1.0 / (2.0 * aH3u * aH3u * aH3v);
  aResult.D3UUV = gp_Vec(
    (aPpp3.X() - 2.0 * aPvp3.X() + aPmp3.X() - aPpm3.X() + 2.0 * aPvm3.X() - aPmm3.X()) * aInv2Hu2Hv,
    (aPpp3.Y() - 2.0 * aPvp3.Y() + aPmp3.Y() - aPpm3.Y() + 2.0 * aPvm3.Y() - aPmm3.Y()) * aInv2Hu2Hv,
    (aPpp3.Z() - 2.0 * aPvp3.Z() + aPmp3.Z() - aPpm3.Z() + 2.0 * aPvm3.Z() - aPmm3.Z()) * aInv2Hu2Hv);

  const double aInv2HuHv2 = 1.0 / (2.0 * aH3u * aH3v * aH3v);
  aResult.D3UVV = gp_Vec(
    (aPpp3.X() - 2.0 * aPup3.X() + aPpm3.X() - aPmp3.X() + 2.0 * aPum3.X() - aPmm3.X()) * aInv2HuHv2,
    (aPpp3.Y() - 2.0 * aPup3.Y() + aPpm3.Y() - aPmp3.Y() + 2.0 * aPum3.Y() - aPmm3.Y()) * aInv2HuHv2,
    (aPpp3.Z() - 2.0 * aPup3.Z() + aPpm3.Z() - aPmp3.Z() + 2.0 * aPum3.Z() - aPmm3.Z()) * aInv2HuHv2);

  return aResult;
}

//==================================================================================================

gp_Vec GeomEval_GregorySurface::EvalDN(const double U,
                                        const double V,
                                        const int    Nu,
                                        const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
  {
    throw Geom_UndefinedDerivative(
      "GeomEval_GregorySurface::EvalDN: invalid derivative order");
  }

  // Use finite differences recursively.
  // For first order, use central differences on EvalD0.
  // For higher orders, use central differences on lower-order derivatives.
  const int aTotalOrder = Nu + Nv;
  const double aHu = finiteDiffStep(U, aTotalOrder);
  const double aHv = finiteDiffStep(V, aTotalOrder);

  if (Nu >= 1)
  {
    // Reduce u-derivative order by central differencing.
    gp_Vec aVp, aVm;
    if (Nu == 1 && Nv == 0)
    {
      aVp = gp_Vec(EvalD0(U + aHu, V).XYZ());
      aVm = gp_Vec(EvalD0(U - aHu, V).XYZ());
    }
    else
    {
      aVp = EvalDN(U + aHu, V, Nu - 1, Nv);
      aVm = EvalDN(U - aHu, V, Nu - 1, Nv);
    }
    return gp_Vec((aVp.X() - aVm.X()) / (2.0 * aHu),
                  (aVp.Y() - aVm.Y()) / (2.0 * aHu),
                  (aVp.Z() - aVm.Z()) / (2.0 * aHu));
  }

  // Nu == 0, Nv >= 1: reduce v-derivative order.
  gp_Vec aVp, aVm;
  if (Nv == 1)
  {
    aVp = gp_Vec(EvalD0(U, V + aHv).XYZ());
    aVm = gp_Vec(EvalD0(U, V - aHv).XYZ());
  }
  else
  {
    aVp = EvalDN(U, V + aHv, 0, Nv - 1);
    aVm = EvalDN(U, V - aHv, 0, Nv - 1);
  }
  return gp_Vec((aVp.X() - aVm.X()) / (2.0 * aHv),
                (aVp.Y() - aVm.Y()) / (2.0 * aHv),
                (aVp.Z() - aVm.Z()) / (2.0 * aHv));
}

//==================================================================================================

void GeomEval_GregorySurface::Transform(const gp_Trsf& T)
{
  for (int aRow = 1; aRow <= 4; ++aRow)
  {
    for (int aCol = 1; aCol <= 4; ++aCol)
    {
      myPoles.ChangeValue(aRow, aCol).Transform(T);
    }
  }
  for (int aCorner = 0; aCorner < 4; ++aCorner)
  {
    myInteriorPairs[aCorner][0].Transform(T);
    myInteriorPairs[aCorner][1].Transform(T);
  }
}

//==================================================================================================

occ::handle<Geom_Geometry> GeomEval_GregorySurface::Copy() const
{
  return new GeomEval_GregorySurface(myPoles, myInteriorPairs);
}

//==================================================================================================

void GeomEval_GregorySurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  for (int aRow = 1; aRow <= 4; ++aRow)
  {
    for (int aCol = 1; aCol <= 4; ++aCol)
    {
      const gp_Pnt& aPt = myPoles.Value(aRow, aCol);
      OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aPt)
    }
  }
  for (int aCorner = 0; aCorner < 4; ++aCorner)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInteriorPairs[aCorner][0])
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInteriorPairs[aCorner][1])
  }
}
