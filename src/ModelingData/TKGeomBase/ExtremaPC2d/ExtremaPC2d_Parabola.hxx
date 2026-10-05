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

#ifndef _ExtremaPC2d_Parabola_HeaderFile
#define _ExtremaPC2d_Parabola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <MathPoly_Cubic.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Parabola extrema computation.
//!
//! Computes the extrema between a 2D point and a parabola.
//! Uses cubic polynomial solving via MathPoly::Cubic.
//!
//! The algorithm:
//! For parabola C(u) = ((u^2)/(4F), u) with focal length F,
//!    solves: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
//!    where (X, Y) are coordinates of Pp in parabola local frame.
//!
//! @note A parabola can have up to 3 extrema.
//!
//! The domain is fixed at construction time for optimal performance.
//! For infinite parabola, construct without domain or with nullopt.
class ExtremaPC2d_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with parabola geometry (infinite).
  //! @param[in] theParabola the parabola to compute extrema for
  explicit ExtremaPC2d_Parabola(const gp_Parab2d& theParabola)
      : myParabola(theParabola),
        myDomain(std::nullopt)
  {
    cacheGeometry();
  }

  //! Constructor with parabola geometry and parameter domain.
  //! @param[in] theParabola the parabola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC2d_Parabola(const gp_Parab2d& theParabola, const ExtremaPC2d::Domain1D& theDomain)
      : myParabola(theParabola),
        myDomain(theDomain)
  {
    cacheGeometry();
  }

  //! Copy constructor is deleted.
  ExtremaPC2d_Parabola(const ExtremaPC2d_Parabola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC2d_Parabola& operator=(const ExtremaPC2d_Parabola&) = delete;

  //! Move constructor.
  ExtremaPC2d_Parabola(ExtremaPC2d_Parabola&&) = default;

  //! Move assignment operator.
  ExtremaPC2d_Parabola& operator=(ExtremaPC2d_Parabola&&) = default;

  //! Evaluates point on parabola at parameter using cached geometry.
  //! @param theU parameter
  //! @return point on parabola
  gp_Pnt2d Value(double theU) const
  {
    if (myFocal <= gp::Resolution())
    {
      return myParabola.Location().Translated(theU * gp_Vec2d(myParabola.MirrorAxis().Direction()));
    }

    // Parabola: P(u) = Vertex + (u^2/4F)*XDir + u*YDir
    const double aX = theU * theU * my1Over4F;
    return gp_Pnt2d(myVertexX + aX * myXDirX + theU * myYDirX,
                    myVertexY + aX * myXDirY + theU * myYDirY);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC2d::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the parabola.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] const ExtremaPC2d::Result& Perform(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const
  {
    performCore(theP, myDomain, theTol, theMode);
    return myResult;
  }

  //! Compute extrema between point P and the parabola arc including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaPC2d::Result& PerformWithEndpoints(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const
  {
    if (!ExtremaPC2d::IsValidTolerance(theTol) || !ExtremaPC2d::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Clear();
      myResult.Status = ExtremaPC2d::Status::InvalidInput;
      return myResult;
    }

    if (myDomain.has_value() && myDomain->IsValid() && myDomain->Min == myDomain->Max)
    {
      myResult.Clear();
      ExtremaPC2d::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      myResult.Status =
        myResult.Extrema.IsEmpty() ? ExtremaPC2d::Status::NoSolution : ExtremaPC2d::Status::OK;
      return myResult;
    }

    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if ((myResult.Status == ExtremaPC2d::Status::OK
         || myResult.Status == ExtremaPC2d::Status::NoSolution)
        && myDomain.has_value())
    {
      ExtremaPC2d::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      if (!myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaPC2d::Status::OK;
      }
    }

    return myResult;
  }

  //! Returns the parabola geometry.
  const gp_Parab2d& Parabola() const { return myParabola; }

private:
  //! Cache geometry components for fast computation.
  void cacheGeometry()
  {
    const gp_Pnt2d& aVertex = myParabola.Location();
    myVertexX               = aVertex.X();
    myVertexY               = aVertex.Y();
    const gp_Dir2d& aXDir   = myParabola.Axis().XDirection();
    myXDirX                 = aXDir.X();
    myXDirY                 = aXDir.Y();

    const gp_Dir2d& aYDir = myParabola.Axis().YDirection();
    myYDirX               = aYDir.X();
    myYDirY               = aYDir.Y();

    myFocal   = myParabola.Focal();
    my1Over4F = myFocal > gp::Resolution() ? 1.0 / (4.0 * myFocal) : 0.0;
    my2F      = 2.0 * myFocal;
  }

  //! Solve the cubic equation and return polynomial result using cached geometry.
  MathPoly::PolyResult solveCubic(const gp_Pnt2d& theP) const
  {
    // Vector from vertex to point
    const double aDx = theP.X() - myVertexX;
    const double aDy = theP.Y() - myVertexY;

    // Local coordinates in parabola frame
    const double aX = aDx * myXDirX + aDy * myXDirY;
    const double aY = aDx * myYDirX + aDy * myYDirY;

    // Solve cubic equation: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
    return MathPoly::Cubic(my1Over4F, 0.0, my2F - aX, -my2F * aY);
  }

  //! Core algorithm - finds extrema with optional bounds checking.
  //! Stores results in myResult.
  //! @param theP query point
  //! @param theDomain optional parameter domain (nullopt for unbounded)
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode
  void performCore(const gp_Pnt2d&                             theP,
                   const std::optional<ExtremaPC2d::Domain1D>& theDomain,
                   double                                      theTol,
                   ExtremaPC2d::SearchMode                     theMode) const
  {
    myResult.Clear();

    if (!ExtremaPC2d::IsValidTolerance(theTol) || !ExtremaPC2d::IsFinitePoint(theP)
        || (theDomain.has_value() && !theDomain->IsValid()))
    {
      myResult.Status = ExtremaPC2d::Status::InvalidInput;
      return;
    }

    if (myFocal <= gp::Resolution())
    {
      if (theMode == ExtremaPC2d::SearchMode::Max)
      {
        myResult.Status = ExtremaPC2d::Status::NoSolution;
        return;
      }

      const gp_Pnt2d& aVertex = myParabola.Location();
      const gp_Vec2d  aDirection(myParabola.MirrorAxis().Direction());
      double          aU = gp_Vec2d(aVertex, theP).Dot(aDirection);
      if (theDomain.has_value() && !theDomain->Contains(aU, theTol))
      {
        myResult.Status = ExtremaPC2d::Status::NoSolution;
        return;
      }
      if (theDomain.has_value())
      {
        aU = theDomain->Clamp(aU);
      }

      const gp_Pnt2d              aCurvePt = Value(aU);
      ExtremaPC2d::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = theP.SquareDistance(aCurvePt);
      anExt.IsMinimum      = true;
      anExt.IsMaximum      = false;
      myResult.Extrema.Append(anExt);
      myResult.Status = ExtremaPC2d::Status::OK;
      return;
    }

    MathPoly::PolyResult aPolyRes = solveCubic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC2d::Status::InfiniteSolutions;
        gp_Pnt2d aPtOnCurve             = Value(0.0);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC2d::Status::NumericalError;
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
        if (!theDomain->Contains(aU, ExtremaPC2d::THE_PARAM_TOLERANCE))
        {
          continue;
        }
        aU = theDomain->Clamp(aU);
      }

      gp_Pnt2d aCurvePt = Value(aU);

      // Check for duplicates
      bool aDuplicate = false;
      for (const ExtremaPC2d::ExtremumResult& anExtremum : myResult.Extrema)
      {
        if (std::abs(aU - anExtremum.Parameter) < ExtremaPC2d::THE_PARAM_TOLERANCE)
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
      if (theMode == ExtremaPC2d::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaPC2d::SearchMode::Max && aIsMin)
        continue;

      ExtremaPC2d::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      anExt.IsMaximum      = !aIsMin;

      myResult.Extrema.Append(anExt);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC2d::Status::NoSolution : ExtremaPC2d::Status::OK;
  }

  gp_Parab2d                           myParabola; //!< Parabola geometry
  std::optional<ExtremaPC2d::Domain1D> myDomain;   //!< Parameter domain (nullopt for infinite)
  mutable ExtremaPC2d::Result          myResult;   //!< Reusable result storage

  // Cached geometry components for fast computation
  double myVertexX, myVertexY; //!< Vertex location
  double myXDirX, myXDirY;     //!< X-axis direction
  double myYDirX, myYDirY;     //!< Y-axis direction
  double myFocal;              //!< Focal length
  double my1Over4F;            //!< Precomputed 1/(4*F)
  double my2F;                 //!< Precomputed 2*F
};

#endif // _ExtremaPC2d_Parabola_HeaderFile
