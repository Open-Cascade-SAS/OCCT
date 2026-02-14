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

#include <ExtremaSS_GenericPair.hxx>

#include <MathSys_Newton4D.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace
{

//! Number of samples per direction for surface grids.
constexpr int THE_NB_SAMPLES = 32;

//! Near-zero gradient tolerance factor.
constexpr double THE_GRADIENT_TOL_FACTOR = 1.0e-4;

//! Maximum parameter value for clamping infinite domains.
constexpr double THE_MAX_PARAM = 1.0e6;

//! Number of cells per dimension for spatial hash.
constexpr int THE_HASH_CELLS = 8;

//! Gradient data for a point pair.
struct GradientData
{
  double F1; //!< (S1-S2) · dS1/dU1
  double F2; //!< (S1-S2) · dS1/dV1
  double F3; //!< (S2-S1) · dS2/dU2
  double F4; //!< (S2-S1) · dS2/dV2
};

//! Entry for spatial hash table.
struct HashEntry
{
  int I, J; //!< Grid indices
};

//! Simple 3D spatial hash for point location acceleration.
class SpatialHash3D
{
public:
  //! Build spatial hash from grid points.
  template <typename GridType>
  void Build(const GridType& theGrid, int theNbU, int theNbV)
  {
    // Compute bounding box
    myMinX = myMinY = myMinZ = std::numeric_limits<double>::max();
    myMaxX = myMaxY = myMaxZ = -std::numeric_limits<double>::max();

    for (int i = 0; i < theNbU; ++i)
    {
      for (int j = 0; j < theNbV; ++j)
      {
        const gp_Pnt& aPt = theGrid(i, j).Point;
        myMinX = std::min(myMinX, aPt.X());
        myMaxX = std::max(myMaxX, aPt.X());
        myMinY = std::min(myMinY, aPt.Y());
        myMaxY = std::max(myMaxY, aPt.Y());
        myMinZ = std::min(myMinZ, aPt.Z());
        myMaxZ = std::max(myMaxZ, aPt.Z());
      }
    }

    // Add small margin to avoid boundary issues
    const double aMargin = 1.0e-10;
    myMinX -= aMargin;
    myMinY -= aMargin;
    myMinZ -= aMargin;
    myMaxX += aMargin;
    myMaxY += aMargin;
    myMaxZ += aMargin;

    // Compute cell sizes
    myCellSizeX = (myMaxX - myMinX) / THE_HASH_CELLS;
    myCellSizeY = (myMaxY - myMinY) / THE_HASH_CELLS;
    myCellSizeZ = (myMaxZ - myMinZ) / THE_HASH_CELLS;

    // Handle degenerate cases
    if (myCellSizeX < 1.0e-15)
      myCellSizeX = 1.0;
    if (myCellSizeY < 1.0e-15)
      myCellSizeY = 1.0;
    if (myCellSizeZ < 1.0e-15)
      myCellSizeZ = 1.0;

    // Clear and populate cells
    const int aTotalCells = THE_HASH_CELLS * THE_HASH_CELLS * THE_HASH_CELLS;
    myCells.resize(aTotalCells);
    for (auto& aCell : myCells)
    {
      aCell.clear();
    }

    for (int i = 0; i < theNbU; ++i)
    {
      for (int j = 0; j < theNbV; ++j)
      {
        const gp_Pnt& aPt      = theGrid(i, j).Point;
        int           aCellIdx = getCellIndex(aPt);
        myCells[aCellIdx].push_back({i, j});
      }
    }
  }

  //! Find closest point in grid to the query point.
  template <typename GridType>
  void FindClosest(const gp_Pnt&   theQuery,
                   const GridType& theGrid,
                   int&            theBestI,
                   int&            theBestJ,
                   double&         theBestSqDist) const
  {
    theBestSqDist = std::numeric_limits<double>::max();
    theBestI = theBestJ = 0;

    // Get cell indices for query point
    int aCellX = static_cast<int>((theQuery.X() - myMinX) / myCellSizeX);
    int aCellY = static_cast<int>((theQuery.Y() - myMinY) / myCellSizeY);
    int aCellZ = static_cast<int>((theQuery.Z() - myMinZ) / myCellSizeZ);

    // Clamp to valid range
    aCellX = std::max(0, std::min(aCellX, THE_HASH_CELLS - 1));
    aCellY = std::max(0, std::min(aCellY, THE_HASH_CELLS - 1));
    aCellZ = std::max(0, std::min(aCellZ, THE_HASH_CELLS - 1));

    // Search in progressively larger neighborhoods until we find a point
    for (int aRadius = 0; aRadius < THE_HASH_CELLS; ++aRadius)
    {
      bool aFoundInRadius = false;

      for (int dx = -aRadius; dx <= aRadius; ++dx)
      {
        int nx = aCellX + dx;
        if (nx < 0 || nx >= THE_HASH_CELLS)
          continue;

        for (int dy = -aRadius; dy <= aRadius; ++dy)
        {
          int ny = aCellY + dy;
          if (ny < 0 || ny >= THE_HASH_CELLS)
            continue;

          for (int dz = -aRadius; dz <= aRadius; ++dz)
          {
            // Only search cells on the boundary of the current radius
            if (aRadius > 0 && std::abs(dx) != aRadius && std::abs(dy) != aRadius
                && std::abs(dz) != aRadius)
              continue;

            int nz = aCellZ + dz;
            if (nz < 0 || nz >= THE_HASH_CELLS)
              continue;

            int aCellIdx = nx * THE_HASH_CELLS * THE_HASH_CELLS + ny * THE_HASH_CELLS + nz;

            for (const auto& aEntry : myCells[aCellIdx])
            {
              const gp_Pnt& aPt     = theGrid(aEntry.I, aEntry.J).Point;
              double        aSqDist = theQuery.SquareDistance(aPt);

              if (aSqDist < theBestSqDist)
              {
                theBestSqDist = aSqDist;
                theBestI      = aEntry.I;
                theBestJ      = aEntry.J;
                aFoundInRadius = true;
              }
            }
          }
        }
      }

      // If we found a point and searched at least one cell beyond,
      // we can guarantee we have the closest point
      if (aFoundInRadius && aRadius > 0)
      {
        // Check if the closest point could be in an outer shell
        double aMaxCellDiag = std::sqrt(myCellSizeX * myCellSizeX + myCellSizeY * myCellSizeY
                                        + myCellSizeZ * myCellSizeZ);
        if (theBestSqDist <= aMaxCellDiag * aMaxCellDiag * (aRadius - 1) * (aRadius - 1))
          break;
      }
      if (aFoundInRadius)
        break;
    }
  }

private:
  int getCellIndex(const gp_Pnt& thePt) const
  {
    int aCellX = static_cast<int>((thePt.X() - myMinX) / myCellSizeX);
    int aCellY = static_cast<int>((thePt.Y() - myMinY) / myCellSizeY);
    int aCellZ = static_cast<int>((thePt.Z() - myMinZ) / myCellSizeZ);

    aCellX = std::max(0, std::min(aCellX, THE_HASH_CELLS - 1));
    aCellY = std::max(0, std::min(aCellY, THE_HASH_CELLS - 1));
    aCellZ = std::max(0, std::min(aCellZ, THE_HASH_CELLS - 1));

    return aCellX * THE_HASH_CELLS * THE_HASH_CELLS + aCellY * THE_HASH_CELLS + aCellZ;
  }

private:
  double myMinX, myMaxX, myMinY, myMaxY, myMinZ, myMaxZ;
  double myCellSizeX, myCellSizeY, myCellSizeZ;
  std::vector<std::vector<HashEntry>> myCells;
};

//! @brief Compute gradient components for a point pair.
GradientData computeGradient(const gp_Pnt& theP1,
                             const gp_Vec& theDU1,
                             const gp_Vec& theDV1,
                             const gp_Pnt& theP2,
                             const gp_Vec& theDU2,
                             const gp_Vec& theDV2)
{
  const double aDx = theP1.X() - theP2.X();
  const double aDy = theP1.Y() - theP2.Y();
  const double aDz = theP1.Z() - theP2.Z();

  GradientData aGrad;
  aGrad.F1 = aDx * theDU1.X() + aDy * theDU1.Y() + aDz * theDU1.Z();
  aGrad.F2 = aDx * theDV1.X() + aDy * theDV1.Y() + aDz * theDV1.Z();
  aGrad.F3 = -(aDx * theDU2.X() + aDy * theDU2.Y() + aDz * theDU2.Z());
  aGrad.F4 = -(aDx * theDV2.X() + aDy * theDV2.Y() + aDz * theDV2.Z());

  return aGrad;
}

//! @brief Classify extremum as minimum or maximum using Hessian.
bool classifyExtremumAsMin(const Adaptor3d_Surface& theSurf1,
                           const Adaptor3d_Surface& theSurf2,
                           double                   theU1,
                           double                   theV1,
                           double                   theU2,
                           double                   theV2)
{
  gp_Pnt aPt1, aPt2;
  gp_Vec aD1U1, aD1V1, aD2UU1, aD2VV1, aD2UV1;
  gp_Vec aD1U2, aD1V2, aD2UU2, aD2VV2, aD2UV2;

  theSurf1.D2(theU1, theV1, aPt1, aD1U1, aD1V1, aD2UU1, aD2VV1, aD2UV1);
  theSurf2.D2(theU2, theV2, aPt2, aD1U2, aD1V2, aD2UU2, aD2VV2, aD2UV2);

  const gp_Vec aD(aPt2, aPt1);

  double aH00 = aD1U1.Dot(aD1U1) + aD.Dot(aD2UU1);
  double aH22 = aD1U2.Dot(aD1U2) - aD.Dot(aD2UU2);

  return (aH00 > 0.0 && aH22 > 0.0);
}

} // anonymous namespace

//==================================================================================================

ExtremaSS_GenericPair::ExtremaSS_GenericPair(const Adaptor3d_Surface& theSurface1,
                                             const Adaptor3d_Surface& theSurface2)
    : mySurface1(&theSurface1),
      mySurface2(&theSurface2),
      myDomain(std::nullopt),
      mySwapped(false)
{
}

//==================================================================================================

ExtremaSS_GenericPair::ExtremaSS_GenericPair(const Adaptor3d_Surface&   theSurface1,
                                             const Adaptor3d_Surface&   theSurface2,
                                             const ExtremaSS::Domain4D& theDomain)
    : mySurface1(&theSurface1),
      mySurface2(&theSurface2),
      myDomain(theDomain),
      mySwapped(false)
{
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_GenericPair::Perform(double                theTol,
                                                        ExtremaSS::SearchMode theMode) const
{
  myResult.Clear();

  buildGrids();
  scanGrids(theTol, theMode);
  refineCandidates(theTol, theMode);

  if (!myResult.Extrema.IsEmpty())
  {
    myResult.Status = ExtremaSS::Status::OK;
  }
  else
  {
    myResult.Status = ExtremaSS::Status::NoSolution;
  }

  return myResult;
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_GenericPair::PerformWithBoundary(
  double                theTol,
  ExtremaSS::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  const Bounds4D aBounds = extractBounds(false);

  const double aCorners1[4][2] = {{aBounds.U1Min, aBounds.V1Min},
                                  {aBounds.U1Max, aBounds.V1Min},
                                  {aBounds.U1Min, aBounds.V1Max},
                                  {aBounds.U1Max, aBounds.V1Max}};
  const double aCorners2[4][2] = {{aBounds.U2Min, aBounds.V2Min},
                                  {aBounds.U2Max, aBounds.V2Min},
                                  {aBounds.U2Min, aBounds.V2Max},
                                  {aBounds.U2Max, aBounds.V2Max}};

  for (int c1 = 0; c1 < 4; ++c1)
  {
    gp_Pnt aPt1 = mySurface1->Value(aCorners1[c1][0], aCorners1[c1][1]);

    for (int c2 = 0; c2 < 4; ++c2)
    {
      gp_Pnt aPt2    = mySurface2->Value(aCorners2[c2][0], aCorners2[c2][1]);
      double aSqDist = aPt1.SquareDistance(aPt2);

      bool aIsMin = true;
      if (theMode == ExtremaSS::SearchMode::Max)
      {
        aIsMin = false;
      }
      else if (theMode == ExtremaSS::SearchMode::MinMax)
      {
        aIsMin = (myResult.Extrema.IsEmpty() || aSqDist <= myResult.MinSquareDistance());
      }

      ExtremaSS::AddExtremum(myResult,
                             aCorners1[c1][0],
                             aCorners1[c1][1],
                             aCorners2[c2][0],
                             aCorners2[c2][1],
                             aPt1,
                             aPt2,
                             aSqDist,
                             aIsMin,
                             theTol);
    }
  }

  if (!myResult.Extrema.IsEmpty())
  {
    myResult.Status = ExtremaSS::Status::OK;
  }

  return myResult;
}

//==================================================================================================

ExtremaSS_GenericPair::Bounds4D ExtremaSS_GenericPair::extractBounds(bool theClamp) const
{
  Bounds4D aBounds;

  if (myDomain.has_value())
  {
    aBounds.U1Min = myDomain->Domain1.UMin;
    aBounds.U1Max = myDomain->Domain1.UMax;
    aBounds.V1Min = myDomain->Domain1.VMin;
    aBounds.V1Max = myDomain->Domain1.VMax;
    aBounds.U2Min = myDomain->Domain2.UMin;
    aBounds.U2Max = myDomain->Domain2.UMax;
    aBounds.V2Min = myDomain->Domain2.VMin;
    aBounds.V2Max = myDomain->Domain2.VMax;
  }
  else
  {
    aBounds.U1Min = mySurface1->FirstUParameter();
    aBounds.U1Max = mySurface1->LastUParameter();
    aBounds.V1Min = mySurface1->FirstVParameter();
    aBounds.V1Max = mySurface1->LastVParameter();
    aBounds.U2Min = mySurface2->FirstUParameter();
    aBounds.U2Max = mySurface2->LastUParameter();
    aBounds.V2Min = mySurface2->FirstVParameter();
    aBounds.V2Max = mySurface2->LastVParameter();
  }

  if (theClamp)
  {
    aBounds.U1Min = std::max(aBounds.U1Min, -THE_MAX_PARAM);
    aBounds.U1Max = std::min(aBounds.U1Max, THE_MAX_PARAM);
    aBounds.V1Min = std::max(aBounds.V1Min, -THE_MAX_PARAM);
    aBounds.V1Max = std::min(aBounds.V1Max, THE_MAX_PARAM);
    aBounds.U2Min = std::max(aBounds.U2Min, -THE_MAX_PARAM);
    aBounds.U2Max = std::min(aBounds.U2Max, THE_MAX_PARAM);
    aBounds.V2Min = std::max(aBounds.V2Min, -THE_MAX_PARAM);
    aBounds.V2Max = std::min(aBounds.V2Max, THE_MAX_PARAM);
  }

  return aBounds;
}

//==================================================================================================

gp_Pnt ExtremaSS_GenericPair::Value1(double theU, double theV) const
{
  return mySurface1->Value(theU, theV);
}

//==================================================================================================

gp_Pnt ExtremaSS_GenericPair::Value2(double theU, double theV) const
{
  return mySurface2->Value(theU, theV);
}

//==================================================================================================

void ExtremaSS_GenericPair::buildGrids() const
{
  const Bounds4D aBounds = extractBounds(true);

  myGrid1.Resize(0, THE_NB_SAMPLES - 1, 0, THE_NB_SAMPLES - 1, false);
  myGrid2.Resize(0, THE_NB_SAMPLES - 1, 0, THE_NB_SAMPLES - 1, false);

  const double aStepU1 = (aBounds.U1Max - aBounds.U1Min) / (THE_NB_SAMPLES - 1);
  const double aStepV1 = (aBounds.V1Max - aBounds.V1Min) / (THE_NB_SAMPLES - 1);
  const double aStepU2 = (aBounds.U2Max - aBounds.U2Min) / (THE_NB_SAMPLES - 1);
  const double aStepV2 = (aBounds.V2Max - aBounds.V2Min) / (THE_NB_SAMPLES - 1);

  for (int i = 0; i < THE_NB_SAMPLES; ++i)
  {
    double aU1 = aBounds.U1Min + i * aStepU1;
    if (i == THE_NB_SAMPLES - 1)
      aU1 = aBounds.U1Max;

    for (int j = 0; j < THE_NB_SAMPLES; ++j)
    {
      double aV1 = aBounds.V1Min + j * aStepV1;
      if (j == THE_NB_SAMPLES - 1)
        aV1 = aBounds.V1Max;

      gp_Pnt aPt;
      gp_Vec aDU, aDV;
      mySurface1->D1(aU1, aV1, aPt, aDU, aDV);

      SurfaceGridPoint& aGP = myGrid1(i, j);
      aGP.U                 = aU1;
      aGP.V                 = aV1;
      aGP.Point             = aPt;
      aGP.DU                = aDU;
      aGP.DV                = aDV;
    }
  }

  for (int i = 0; i < THE_NB_SAMPLES; ++i)
  {
    double aU2 = aBounds.U2Min + i * aStepU2;
    if (i == THE_NB_SAMPLES - 1)
      aU2 = aBounds.U2Max;

    for (int j = 0; j < THE_NB_SAMPLES; ++j)
    {
      double aV2 = aBounds.V2Min + j * aStepV2;
      if (j == THE_NB_SAMPLES - 1)
        aV2 = aBounds.V2Max;

      gp_Pnt aPt;
      gp_Vec aDU, aDV;
      mySurface2->D1(aU2, aV2, aPt, aDU, aDV);

      SurfaceGridPoint& aGP = myGrid2(i, j);
      aGP.U                 = aU2;
      aGP.V                 = aV2;
      aGP.Point             = aPt;
      aGP.DU                = aDU;
      aGP.DV                = aDV;
    }
  }
}

//==================================================================================================

void ExtremaSS_GenericPair::scanGrids(double theTol, ExtremaSS::SearchMode theMode) const
{
  myCandidates.Clear();

  const int aNb1U = myGrid1.UpperRow() - myGrid1.LowerRow() + 1;
  const int aNb1V = myGrid1.UpperCol() - myGrid1.LowerCol() + 1;
  const int aNb2U = myGrid2.UpperRow() - myGrid2.LowerRow() + 1;
  const int aNb2V = myGrid2.UpperCol() - myGrid2.LowerCol() + 1;

  const double aTolGrad   = theTol * THE_GRADIENT_TOL_FACTOR;
  const double aTolGradSq = aTolGrad * aTolGrad;

  // Build spatial hash for grid2 to accelerate closest point queries
  SpatialHash3D aSpatialHash;
  aSpatialHash.Build(myGrid2, aNb2U, aNb2V);

  double aMinSqDist = std::numeric_limits<double>::max();
  double aMaxSqDist = -std::numeric_limits<double>::max();
  int    aMinI1 = 0, aMinJ1 = 0, aMinI2 = 0, aMinJ2 = 0;
  int    aMaxI1 = 0, aMaxJ1 = 0, aMaxI2 = 0, aMaxJ2 = 0;

  for (int i1 = 0; i1 < aNb1U; ++i1)
  {
    for (int j1 = 0; j1 < aNb1V; ++j1)
    {
      const SurfaceGridPoint& aGP1 = myGrid1(i1, j1);

      // Use spatial hash to find closest point on grid2
      double aBestDist = 0.0;
      int    aBestI2 = 0, aBestJ2 = 0;
      aSpatialHash.FindClosest(aGP1.Point, myGrid2, aBestI2, aBestJ2, aBestDist);

      if (aBestDist < aMinSqDist)
      {
        aMinSqDist = aBestDist;
        aMinI1     = i1;
        aMinJ1     = j1;
        aMinI2     = aBestI2;
        aMinJ2     = aBestJ2;
      }
      if (aBestDist > aMaxSqDist)
      {
        aMaxSqDist = aBestDist;
        aMaxI1     = i1;
        aMaxJ1     = j1;
        aMaxI2     = aBestI2;
        aMaxJ2     = aBestJ2;
      }

      const SurfaceGridPoint& aGP2 = myGrid2(aBestI2, aBestJ2);
      GradientData aGrad = computeGradient(aGP1.Point, aGP1.DU, aGP1.DV,
                                           aGP2.Point, aGP2.DU, aGP2.DV);

      double aGradNormSq = aGrad.F1 * aGrad.F1 + aGrad.F2 * aGrad.F2
                         + aGrad.F3 * aGrad.F3 + aGrad.F4 * aGrad.F4;

      if (aGradNormSq < aTolGradSq || aGradNormSq < aBestDist * 1.0e-6)
      {
        Candidate4D aCand;
        aCand.I1        = i1;
        aCand.J1        = j1;
        aCand.I2        = aBestI2;
        aCand.J2        = aBestJ2;
        aCand.U1        = aGP1.U;
        aCand.V1        = aGP1.V;
        aCand.U2        = aGP2.U;
        aCand.V2        = aGP2.V;
        aCand.EstSqDist = aBestDist;
        myCandidates.Append(aCand);
      }
    }
  }

  if (theMode == ExtremaSS::SearchMode::Min || theMode == ExtremaSS::SearchMode::MinMax)
  {
    Candidate4D aCand;
    aCand.I1        = aMinI1;
    aCand.J1        = aMinJ1;
    aCand.I2        = aMinI2;
    aCand.J2        = aMinJ2;
    aCand.U1        = myGrid1(aMinI1, aMinJ1).U;
    aCand.V1        = myGrid1(aMinI1, aMinJ1).V;
    aCand.U2        = myGrid2(aMinI2, aMinJ2).U;
    aCand.V2        = myGrid2(aMinI2, aMinJ2).V;
    aCand.EstSqDist = aMinSqDist;
    myCandidates.Append(aCand);
  }

  if (theMode == ExtremaSS::SearchMode::Max || theMode == ExtremaSS::SearchMode::MinMax)
  {
    Candidate4D aCand;
    aCand.I1        = aMaxI1;
    aCand.J1        = aMaxJ1;
    aCand.I2        = aMaxI2;
    aCand.J2        = aMaxJ2;
    aCand.U1        = myGrid1(aMaxI1, aMaxJ1).U;
    aCand.V1        = myGrid1(aMaxI1, aMaxJ1).V;
    aCand.U2        = myGrid2(aMaxI2, aMaxJ2).U;
    aCand.V2        = myGrid2(aMaxI2, aMaxJ2).V;
    aCand.EstSqDist = aMaxSqDist;
    myCandidates.Append(aCand);
  }
}

//==================================================================================================

void ExtremaSS_GenericPair::refineCandidates(double                theTol,
                                             ExtremaSS::SearchMode theMode) const
{
  const Bounds4D aBounds = extractBounds(true);

  std::sort(myCandidates.begin(),
            myCandidates.end(),
            [theMode](const Candidate4D& a, const Candidate4D& b) {
              if (theMode == ExtremaSS::SearchMode::Max)
              {
                return a.EstSqDist > b.EstSqDist;
              }
              return a.EstSqDist < b.EstSqDist;
            });

  myFoundRoots.Clear();

  for (int c = 0; c < myCandidates.Length(); ++c)
  {
    const Candidate4D& aCand = myCandidates.Value(c);

    // Use MathSys Newton4D solver for surface-surface extrema
    MathSys::Newton4DResult aNewtonRes = MathSys::Newton4DSurfaceSurface(*mySurface1,
                                                                         *mySurface2,
                                                                         aCand.U1,
                                                                         aCand.V1,
                                                                         aCand.U2,
                                                                         aCand.V2,
                                                                         aBounds.U1Min,
                                                                         aBounds.U1Max,
                                                                         aBounds.V1Min,
                                                                         aBounds.V1Max,
                                                                         aBounds.U2Min,
                                                                         aBounds.U2Max,
                                                                         aBounds.V2Min,
                                                                         aBounds.V2Max,
                                                                         theTol * 1.0e-10);

    double aU1 = aNewtonRes.X1;
    double aV1 = aNewtonRes.X2;
    double aU2 = aNewtonRes.X3;
    double aV2 = aNewtonRes.X4;

    if (!aNewtonRes.IsDone())
    {
      aU1 = aCand.U1;
      aV1 = aCand.V1;
      aU2 = aCand.U2;
      aV2 = aCand.V2;
    }

    bool aIsDup = false;
    for (int r = 0; r < myFoundRoots.Length(); ++r)
    {
      const auto& aRoot = myFoundRoots.Value(r);
      if (std::abs(aU1 - std::get<0>(aRoot)) < theTol
          && std::abs(aV1 - std::get<1>(aRoot)) < theTol
          && std::abs(aU2 - std::get<2>(aRoot)) < theTol
          && std::abs(aV2 - std::get<3>(aRoot)) < theTol)
      {
        aIsDup = true;
        break;
      }
    }
    if (aIsDup)
      continue;

    gp_Pnt aPt1    = mySurface1->Value(aU1, aV1);
    gp_Pnt aPt2    = mySurface2->Value(aU2, aV2);
    double aSqDist = aPt1.SquareDistance(aPt2);

    bool aIsMin = classifyExtremumAsMin(*mySurface1, *mySurface2, aU1, aV1, aU2, aV2);

    if (theMode == ExtremaSS::SearchMode::Min && !aIsMin)
      continue;
    if (theMode == ExtremaSS::SearchMode::Max && aIsMin)
      continue;

    ExtremaSS::AddExtremum(myResult, aU1, aV1, aU2, aV2, aPt1, aPt2, aSqDist, aIsMin, theTol);
    myFoundRoots.Append(std::make_tuple(aU1, aV1, aU2, aV2));
  }

  if (myResult.Extrema.IsEmpty() && !myCandidates.IsEmpty())
  {
    const Candidate4D& aCand   = myCandidates.Value(0);
    gp_Pnt             aPt1    = mySurface1->Value(aCand.U1, aCand.V1);
    gp_Pnt             aPt2    = mySurface2->Value(aCand.U2, aCand.V2);
    double             aSqDist = aPt1.SquareDistance(aPt2);
    bool               aIsMin  = (theMode != ExtremaSS::SearchMode::Max);

    ExtremaSS::AddExtremum(myResult, aCand.U1, aCand.V1, aCand.U2, aCand.V2,
                           aPt1, aPt2, aSqDist, aIsMin, theTol);
  }
}
