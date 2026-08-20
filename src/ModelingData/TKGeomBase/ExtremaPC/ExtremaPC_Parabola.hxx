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

#ifndef _ExtremaPC_Parabola_HeaderFile
#define _ExtremaPC_Parabola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_Planar.hxx>
#include <ExtremaPC2d_Parabola.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathPoly_Cubic.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>
#include <ProjLib.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Parabola extrema computation.
//!
//! Computes the extrema between a 3D point and a parabola.
//! Uses cubic polynomial solving via MathPoly::Cubic.
//!
//! The algorithm:
//! 1. Projects point P onto the parabola plane -> Pp
//! 2. For parabola C(u) = ((u^2)/(4F), u) with focal length F,
//!    solves: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
//!    where (X, Y) are coordinates of Pp in parabola local frame.
//!
//! @note A parabola can have up to 3 extrema.
//!
//! The domain is fixed at construction time for optimal performance.
//! For infinite parabola, construct without domain or with nullopt.
class ExtremaPC_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with parabola geometry (infinite).
  //! @param[in] theParabola the parabola to compute extrema for
  explicit ExtremaPC_Parabola(const gp_Parab& theParabola)
      : myParabola(theParabola),
        myDomain(std::nullopt)
  {
    cacheGeometry();
  }

  //! Constructor with parabola geometry and parameter domain.
  //! @param[in] theParabola the parabola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC_Parabola(const gp_Parab& theParabola, const ExtremaPC::Domain1D& theDomain)
      : myParabola(theParabola),
        myDomain(theDomain)
  {
    cacheGeometry();
  }

  //! Copy constructor is deleted.
  ExtremaPC_Parabola(const ExtremaPC_Parabola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Parabola& operator=(const ExtremaPC_Parabola&) = delete;

  //! Move constructor.
  ExtremaPC_Parabola(ExtremaPC_Parabola&&) = default;

  //! Move assignment operator.
  ExtremaPC_Parabola& operator=(ExtremaPC_Parabola&&) = default;

  //! Evaluates point on parabola at parameter using cached geometry.
  //! @param theU parameter
  //! @return point on parabola
  gp_Pnt Value(double theU) const
  {
    if (myFocal <= gp::Resolution())
    {
      return myParabola.Location().Translated(theU * gp_Vec(myParabola.XAxis().Direction()));
    }

    // Parabola: P(u) = Vertex + (u^2/4F)*XDir + u*YDir
    const double aX = theU * theU * my1Over4F;
    return gp_Pnt(myVertexX + aX * myXDirX + theU * myYDirX,
                  myVertexY + aX * myXDirY + theU * myYDirY,
                  myVertexZ + aX * myXDirZ + theU * myYDirZ);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the parabola.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    const gp_Pln aPlane(gp_Ax3(myParabola.Position()));
    if (ExtremaPC::IsValidTolerance(theTol) && ExtremaPC::IsFinitePoint(theP)
        && (!myDomain.has_value() || myDomain->IsValid())
        && ExtremaPC::PerformPlanar<ExtremaPC2d_Parabola>(ProjLib::Project(aPlane, myParabola),
                                                          myDomain,
                                                          ProjLib::Project(aPlane, theP),
                                                          theP,
                                                          aPlane,
                                                          *this,
                                                          theTol,
                                                          theMode,
                                                          false,
                                                          myResult))
    {
      return myResult;
    }
    performCore(theP, myDomain, theTol, theMode);
    return myResult;
  }

  //! Compute extrema between point P and the parabola arc including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaPC::Result& PerformWithEndpoints(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    if (!ExtremaPC::IsValidTolerance(theTol) || !ExtremaPC::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Clear();
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return myResult;
    }

    if (myDomain.has_value() && myDomain->IsValid() && myDomain->Min == myDomain->Max)
    {
      myResult.Clear();
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      myResult.Status =
        myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
      return myResult;
    }

    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if ((myResult.Status == ExtremaPC::Status::OK
         || myResult.Status == ExtremaPC::Status::NoSolution)
        && myDomain.has_value())
    {
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      if (!myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaPC::Status::OK;
      }
    }

    return myResult;
  }

  //! Returns the parabola geometry.
  const gp_Parab& Parabola() const { return myParabola; }

private:
  //! Cache geometry components for fast computation.
  void cacheGeometry()
  {
    const gp_Pnt& aVertex = myParabola.Location();
    myVertexX             = aVertex.X();
    myVertexY             = aVertex.Y();
    myVertexZ             = aVertex.Z();

    const gp_Dir aXDir = myParabola.XAxis().Direction();
    myXDirX            = aXDir.X();
    myXDirY            = aXDir.Y();
    myXDirZ            = aXDir.Z();

    const gp_Dir aYDir = myParabola.YAxis().Direction();
    myYDirX            = aYDir.X();
    myYDirY            = aYDir.Y();
    myYDirZ            = aYDir.Z();

    const gp_Dir& aAxis = myParabola.Axis().Direction();
    myAxisX             = aAxis.X();
    myAxisY             = aAxis.Y();
    myAxisZ             = aAxis.Z();

    myFocal   = myParabola.Focal();
    my1Over4F = myFocal > gp::Resolution() ? 1.0 / (4.0 * myFocal) : 0.0;
    my2F      = 2.0 * myFocal;
  }

  //! Solve the cubic equation and return polynomial result using cached geometry.
  MathPoly::PolyResult solveCubic(const gp_Pnt& theP) const
  {
    // Vector from vertex to point
    const double aDx = theP.X() - myVertexX;
    const double aDy = theP.Y() - myVertexY;
    const double aDz = theP.Z() - myVertexZ;

    // Project point P onto the parabola plane
    // Height = (P - Vertex) . Axis
    const double aHeight = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Projected point Pp = P - Height * Axis
    const double aPpX = theP.X() - aHeight * myAxisX;
    const double aPpY = theP.Y() - aHeight * myAxisY;
    const double aPpZ = theP.Z() - aHeight * myAxisZ;

    // Vector from vertex to projected point
    const double aOPpX = aPpX - myVertexX;
    const double aOPpY = aPpY - myVertexY;
    const double aOPpZ = aPpZ - myVertexZ;

    // Local coordinates in parabola frame
    const double aX = aOPpX * myXDirX + aOPpY * myXDirY + aOPpZ * myXDirZ;
    const double aY = aOPpX * myYDirX + aOPpY * myYDirY + aOPpZ * myYDirZ;

    // Solve cubic equation: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
    return MathPoly::Cubic(my1Over4F, 0.0, my2F - aX, -my2F * aY);
  }

  //! Core algorithm - finds extrema with optional bounds checking.
  //! Stores results in myResult.
  //! @param theP query point
  //! @param theDomain optional parameter domain (nullopt for unbounded)
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode
  void performCore(const gp_Pnt&                             theP,
                   const std::optional<ExtremaPC::Domain1D>& theDomain,
                   double                                    theTol,
                   ExtremaPC::SearchMode                     theMode) const
  {
    myResult.Clear();

    if (!ExtremaPC::IsValidTolerance(theTol) || !ExtremaPC::IsFinitePoint(theP)
        || (theDomain.has_value() && !theDomain->IsValid()))
    {
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return;
    }

    if (myFocal <= gp::Resolution())
    {
      if (theMode == ExtremaPC::SearchMode::Max)
      {
        myResult.Status = ExtremaPC::Status::NoSolution;
        return;
      }

      const gp_Pnt& aVertex = myParabola.Location();
      const gp_Vec  aDirection(myParabola.XAxis().Direction());
      double        aU = gp_Vec(aVertex, theP).Dot(aDirection);
      if (theDomain.has_value() && !theDomain->Contains(aU, theTol))
      {
        myResult.Status = ExtremaPC::Status::NoSolution;
        return;
      }
      if (theDomain.has_value())
      {
        aU = theDomain->Clamp(aU);
      }

      const gp_Pnt              aCurvePt = Value(aU);
      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = theP.SquareDistance(aCurvePt);
      anExt.IsMinimum      = true;
      anExt.IsMaximum      = false;
      myResult.Extrema.Append(anExt);
      myResult.Status = ExtremaPC::Status::OK;
      return;
    }

    MathPoly::PolyResult aPolyRes = solveCubic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve               = Value(0.0);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
      }
      return;
    }

    // Process all roots
    for (size_t i = 0; i < aPolyRes.NbRoots; ++i)
    {
      double aU = aPolyRes.Roots[i];

      // Check bounds if domain is specified
      if (theDomain.has_value())
      {
        if (!theDomain->Contains(aU, ExtremaPC::THE_PARAM_TOLERANCE))
        {
          continue;
        }
        aU = theDomain->Clamp(aU);
      }

      gp_Pnt aCurvePt = Value(aU);

      // Check for duplicates
      bool aDuplicate = false;
      for (const ExtremaPC::ExtremumResult& anExtremum : myResult.Extrema)
      {
        if (std::abs(aU - anExtremum.Parameter) < ExtremaPC::THE_PARAM_TOLERANCE)
        {
          aDuplicate = true;
          break;
        }
      }
      if (aDuplicate)
        continue;

      const size_t aMultiplicity = aPolyRes.Multiplicities[i];
      if (aMultiplicity % 2 == 0)
      {
        continue;
      }

      // The stationary polynomial has a positive leading coefficient. Its sign to the right
      // of this root is determined by the odd-multiplicity roots that follow it.
      double aSqDist          = theP.SquareDistance(aCurvePt);
      size_t aNbOddRootsAfter = 0;
      for (size_t aNext = i + 1; aNext < aPolyRes.NbRoots; ++aNext)
      {
        aNbOddRootsAfter += aPolyRes.Multiplicities[aNext] % 2;
      }
      const bool aIsMin = aNbOddRootsAfter % 2 == 0;

      // Filter by search mode
      if (theMode == ExtremaPC::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaPC::SearchMode::Max && aIsMin)
        continue;

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      anExt.IsMaximum      = !aIsMin;

      myResult.Extrema.Append(anExt);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
  }

  gp_Parab                           myParabola; //!< Parabola geometry
  std::optional<ExtremaPC::Domain1D> myDomain;   //!< Parameter domain (nullopt for infinite)
  mutable ExtremaPC::Result          myResult;   //!< Reusable result storage

  // Cached geometry components for fast computation
  double myVertexX, myVertexY, myVertexZ; //!< Vertex location
  double myXDirX, myXDirY, myXDirZ;       //!< X-axis direction
  double myYDirX, myYDirY, myYDirZ;       //!< Y-axis direction
  double myAxisX, myAxisY, myAxisZ;       //!< Main axis direction
  double myFocal;                         //!< Focal length
  double my1Over4F;                       //!< Precomputed 1/(4*F)
  double my2F;                            //!< Precomputed 2*F
};

#endif // _ExtremaPC_Parabola_HeaderFile
