// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <GeomHash_Polygon2DHasher.hxx>

#include <NCollection_Array1.hxx>
#include <Poly_Polygon2D.hxx>
#include <Standard_HashUtils.hxx>
#include <gp_Pnt2d.hxx>

#include <cmath>
#include <cstdint>

namespace
{
std::size_t quantizedHash(const double theValue, const double theFactor) noexcept
{
  return opencascade::hash(static_cast<int64_t>(std::round(theValue * theFactor)));
}
} // namespace

GeomHash_Polygon2DHasher::GeomHash_Polygon2DHasher(const double theCompTolerance,
                                                   const double theHashTolerance)
    : CompTolerance(theCompTolerance),
      HashTolerance(theHashTolerance)
{
}

std::size_t GeomHash_Polygon2DHasher::operator()(
  const occ::handle<Poly_Polygon2D>& thePoly) const noexcept
{
  if (thePoly.IsNull())
  {
    return 0;
  }

  const double aFactor = 1.0 / HashTolerance;

  size_t aCombined[6] = {};
  aCombined[0]        = opencascade::hash(thePoly->NbNodes());
  aCombined[1]        = quantizedHash(thePoly->Deflection(), aFactor);

  if (thePoly->NbNodes() > 0)
  {
    const NCollection_Array1<gp_Pnt2d>& aNodes = thePoly->Nodes();
    aCombined[2]                               = quantizedHash(aNodes.First().X(), aFactor);
    aCombined[3]                               = quantizedHash(aNodes.First().Y(), aFactor);
    aCombined[4]                               = quantizedHash(aNodes.Last().X(), aFactor);
    aCombined[5]                               = quantizedHash(aNodes.Last().Y(), aFactor);
  }

  return opencascade::hashBytes(aCombined, sizeof(aCombined));
}

bool GeomHash_Polygon2DHasher::operator()(
  const occ::handle<Poly_Polygon2D>& thePoly1,
  const occ::handle<Poly_Polygon2D>& thePoly2) const noexcept
{
  if (thePoly1.IsNull() || thePoly2.IsNull())
  {
    return thePoly1.IsNull() && thePoly2.IsNull();
  }

  if (thePoly1->NbNodes() != thePoly2->NbNodes())
  {
    return false;
  }
  if (std::abs(thePoly1->Deflection() - thePoly2->Deflection()) > CompTolerance)
  {
    return false;
  }

  const NCollection_Array1<gp_Pnt2d>& aNodes1 = thePoly1->Nodes();
  const NCollection_Array1<gp_Pnt2d>& aNodes2 = thePoly2->Nodes();
  for (int aIdx = aNodes1.Lower(); aIdx <= aNodes1.Upper(); ++aIdx)
  {
    const gp_Pnt2d& aP1 = aNodes1.Value(aIdx);
    const gp_Pnt2d& aP2 = aNodes2.Value(aIdx);
    if (std::abs(aP1.X() - aP2.X()) > CompTolerance || std::abs(aP1.Y() - aP2.Y()) > CompTolerance)
    {
      return false;
    }
  }

  return true;
}
