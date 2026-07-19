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

#include <GeomHash_TriangulationHasher.hxx>

#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_HashUtils.hxx>
#include <gp_Pnt.hxx>
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

GeomHash_TriangulationHasher::GeomHash_TriangulationHasher(const double theCompTolerance,
                                                           const double theHashTolerance)
    : CompTolerance(theCompTolerance),
      HashTolerance(theHashTolerance)
{
}

std::size_t GeomHash_TriangulationHasher::operator()(
  const occ::handle<Poly_Triangulation>& theTri) const noexcept
{
  if (theTri.IsNull())
  {
    return 0;
  }

  const double aFactor = 1.0 / HashTolerance;

  size_t aCombined[20] = {};
  aCombined[0]         = opencascade::hash(theTri->NbNodes());
  aCombined[1]         = opencascade::hash(theTri->NbTriangles());
  aCombined[2]         = opencascade::hash(theTri->HasUVNodes() ? 1 : 0);
  aCombined[3]         = quantizedHash(theTri->Deflection(), aFactor);

  if (theTri->NbNodes() > 0)
  {
    const gp_Pnt& aFirstNode = theTri->Node(1);
    const gp_Pnt& aLastNode  = theTri->Node(theTri->NbNodes());
    aCombined[4]             = quantizedHash(aFirstNode.X(), aFactor);
    aCombined[5]             = quantizedHash(aFirstNode.Y(), aFactor);
    aCombined[6]             = quantizedHash(aFirstNode.Z(), aFactor);
    aCombined[7]             = quantizedHash(aLastNode.X(), aFactor);
    aCombined[8]             = quantizedHash(aLastNode.Y(), aFactor);
    aCombined[9]             = quantizedHash(aLastNode.Z(), aFactor);
  }

  if (theTri->NbTriangles() > 0)
  {
    int aN1 = 0, aN2 = 0, aN3 = 0;
    theTri->Triangle(1).Get(aN1, aN2, aN3);
    aCombined[10] = opencascade::hash(aN1);
    aCombined[11] = opencascade::hash(aN2);
    aCombined[12] = opencascade::hash(aN3);
    theTri->Triangle(theTri->NbTriangles()).Get(aN1, aN2, aN3);
    aCombined[13] = opencascade::hash(aN1);
    aCombined[14] = opencascade::hash(aN2);
    aCombined[15] = opencascade::hash(aN3);
  }

  if (theTri->HasUVNodes() && theTri->NbNodes() > 0)
  {
    const gp_Pnt2d& aFirstUV = theTri->UVNode(1);
    const gp_Pnt2d& aLastUV  = theTri->UVNode(theTri->NbNodes());
    aCombined[16]            = quantizedHash(aFirstUV.X(), aFactor);
    aCombined[17]            = quantizedHash(aFirstUV.Y(), aFactor);
    aCombined[18]            = quantizedHash(aLastUV.X(), aFactor);
    aCombined[19]            = quantizedHash(aLastUV.Y(), aFactor);
  }

  return opencascade::hashBytes(aCombined, sizeof(aCombined));
}

bool GeomHash_TriangulationHasher::operator()(
  const occ::handle<Poly_Triangulation>& theTri1,
  const occ::handle<Poly_Triangulation>& theTri2) const noexcept
{
  if (theTri1.IsNull() || theTri2.IsNull())
  {
    return theTri1.IsNull() && theTri2.IsNull();
  }

  if (theTri1->NbNodes() != theTri2->NbNodes())
  {
    return false;
  }
  if (theTri1->NbTriangles() != theTri2->NbTriangles())
  {
    return false;
  }
  if (theTri1->HasUVNodes() != theTri2->HasUVNodes())
  {
    return false;
  }
  if (std::abs(theTri1->Deflection() - theTri2->Deflection()) > CompTolerance)
  {
    return false;
  }

  for (int aIdx = 1; aIdx <= theTri1->NbNodes(); ++aIdx)
  {
    const gp_Pnt& aP1 = theTri1->Node(aIdx);
    const gp_Pnt& aP2 = theTri2->Node(aIdx);
    if (std::abs(aP1.X() - aP2.X()) > CompTolerance || std::abs(aP1.Y() - aP2.Y()) > CompTolerance
        || std::abs(aP1.Z() - aP2.Z()) > CompTolerance)
    {
      return false;
    }
  }

  for (int aIdx = 1; aIdx <= theTri1->NbTriangles(); ++aIdx)
  {
    int aN11 = 0, aN12 = 0, aN13 = 0;
    int aN21 = 0, aN22 = 0, aN23 = 0;
    theTri1->Triangle(aIdx).Get(aN11, aN12, aN13);
    theTri2->Triangle(aIdx).Get(aN21, aN22, aN23);
    if (aN11 != aN21 || aN12 != aN22 || aN13 != aN23)
    {
      return false;
    }
  }

  if (theTri1->HasUVNodes())
  {
    for (int aIdx = 1; aIdx <= theTri1->NbNodes(); ++aIdx)
    {
      const gp_Pnt2d& aU1 = theTri1->UVNode(aIdx);
      const gp_Pnt2d& aU2 = theTri2->UVNode(aIdx);
      if (std::abs(aU1.X() - aU2.X()) > CompTolerance
          || std::abs(aU1.Y() - aU2.Y()) > CompTolerance)
      {
        return false;
      }
    }
  }

  return true;
}
