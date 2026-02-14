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

#ifndef _ExtremaCS_LineCone_HeaderFile
#define _ExtremaCS_LineCone_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <ExtremaCS_GridEvaluator3D.hxx>
#include <ExtremaCS_Line.hxx>
#include <ExtremaCS_Cone.hxx>
#include <gp_Cone.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Line-Cone extrema computation.
//!
//! Computes extrema between a 3D line and a cone.
//!
//! This is a more complex problem than line-cylinder because the cone's
//! radius varies along its axis. Uses grid-based numerical approach with
//! Newton refinement for robustness.
//!
//! Special cases:
//! - Line through apex: infinite tangent directions at apex
//! - Line parallel to a generator: may have infinite solutions
class ExtremaCS_LineCone
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line and cone geometry.
  ExtremaCS_LineCone(const gp_Lin& theLine, const gp_Cone& theCone)
      : myLine(theLine),
        myCone(theCone),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line and cone geometry and domain.
  ExtremaCS_LineCone(const gp_Lin&                        theLine,
                     const gp_Cone&                       theCone,
                     const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myLine(theLine),
        myCone(theCone),
        myDomain(theDomain)
  {
  }

  //! @brief Perform extrema computation.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaCS::Result& Perform(
    double                theTol,
    ExtremaCS::SearchMode theMode = ExtremaCS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Build domain
    ExtremaCS::Domain3D aDomain;
    if (myDomain.has_value())
    {
      aDomain = *myDomain;
    }
    else
    {
      // Default domain for line: use reasonable range based on cone size
      const double aRefR = myCone.RefRadius();
      const double aRange = std::max(aRefR * 10.0, 100.0);
      aDomain.Curve.Min = -aRange;
      aDomain.Curve.Max = aRange;
      aDomain.Surface.UMin = 0.0;
      aDomain.Surface.UMax = ExtremaCS::THE_TWO_PI;
      aDomain.Surface.VMin = -aRange;
      aDomain.Surface.VMax = aRange;
    }

    // Use grid-based numerical approach
    ExtremaCS_Line aLineEval(myLine, aDomain.Curve);
    ExtremaCS_Cone aConeEval(myCone, aDomain.Surface);

    ExtremaCS_GridEvaluator3D<ExtremaCS_Line, ExtremaCS_Cone> aGridEval(aLineEval,
                                                                         aConeEval,
                                                                         aDomain);
    aGridEval.Perform(myResult, theTol, theMode);

    return myResult;
  }

private:
  gp_Lin                             myLine;   //!< Line geometry
  gp_Cone                            myCone;   //!< Cone geometry
  std::optional<ExtremaCS::Domain3D> myDomain; //!< Optional domain
  mutable ExtremaCS::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCS_LineCone_HeaderFile
