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

#include <GeomHash_PolygonOnTriHasher.hxx>

#include <Poly_PolygonOnTriangulation.hxx>
#include <Standard_HashUtils.hxx>

#include <cmath>
#include <cstdint>

namespace
{
std::size_t quantizedHash(const double theValue, const double theFactor) noexcept
{
  return opencascade::hash(static_cast<int64_t>(std::round(theValue * theFactor)));
}
} // namespace

GeomHash_PolygonOnTriHasher::GeomHash_PolygonOnTriHasher(const double theCompTolerance,
                                                         const double theHashTolerance)
    : CompTolerance(theCompTolerance),
      HashTolerance(theHashTolerance)
{
}

std::size_t GeomHash_PolygonOnTriHasher::operator()(
  const PolygonOnTriHashKey& theKey) const noexcept
{
  const occ::handle<Poly_PolygonOnTriangulation>& aPoly        = theKey.Poly;
  size_t                                          aCombined[9] = {};
  aCombined[0] = opencascade::hash(static_cast<int64_t>(theKey.TriRepId));
  if (aPoly.IsNull())
  {
    aCombined[1] = opencascade::hash(1);
    return opencascade::hashBytes(aCombined, sizeof(aCombined));
  }

  const double aFactor = 1.0 / HashTolerance;

  aCombined[2] = opencascade::hash(aPoly->NbNodes());
  aCombined[3] = opencascade::hash(aPoly->HasParameters() ? 1 : 0);
  aCombined[4] = quantizedHash(aPoly->Deflection(), aFactor);

  if (aPoly->NbNodes() > 0)
  {
    aCombined[5] = opencascade::hash(aPoly->Node(1));
    aCombined[6] = opencascade::hash(aPoly->Node(aPoly->NbNodes()));
    if (aPoly->HasParameters())
    {
      aCombined[7] = quantizedHash(aPoly->Parameter(1), aFactor);
      aCombined[8] = quantizedHash(aPoly->Parameter(aPoly->NbNodes()), aFactor);
    }
  }

  return opencascade::hashBytes(aCombined, sizeof(aCombined));
}

bool GeomHash_PolygonOnTriHasher::operator()(const PolygonOnTriHashKey& theKey1,
                                             const PolygonOnTriHashKey& theKey2) const noexcept
{
  const occ::handle<Poly_PolygonOnTriangulation>& aPoly1 = theKey1.Poly;
  const occ::handle<Poly_PolygonOnTriangulation>& aPoly2 = theKey2.Poly;

  if (theKey1.TriRepId != theKey2.TriRepId)
  {
    return false;
  }
  if (aPoly1.IsNull() || aPoly2.IsNull())
  {
    return aPoly1.IsNull() && aPoly2.IsNull();
  }

  if (aPoly1->NbNodes() != aPoly2->NbNodes())
  {
    return false;
  }
  if (aPoly1->HasParameters() != aPoly2->HasParameters())
  {
    return false;
  }
  if (std::abs(aPoly1->Deflection() - aPoly2->Deflection()) > CompTolerance)
  {
    return false;
  }

  for (int aIdx = 1; aIdx <= aPoly1->NbNodes(); ++aIdx)
  {
    if (aPoly1->Node(aIdx) != aPoly2->Node(aIdx))
    {
      return false;
    }
  }

  if (aPoly1->HasParameters())
  {
    for (int aIdx = 1; aIdx <= aPoly1->NbNodes(); ++aIdx)
    {
      if (std::abs(aPoly1->Parameter(aIdx) - aPoly2->Parameter(aIdx)) > CompTolerance)
      {
        return false;
      }
    }
  }

  return true;
}
