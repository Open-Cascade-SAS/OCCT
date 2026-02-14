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

#ifndef _ExtremaCS_CircleCylinder_HeaderFile
#define _ExtremaCS_CircleCylinder_HeaderFile

#include <ExtremaCS.hxx>
#include <ExtremaCS_Circle.hxx>
#include <ExtremaCS_Cylinder.hxx>
#include <ExtremaCS_GridEvaluator3D.hxx>
#include <gp_Circ.hxx>
#include <gp_Cylinder.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Circle-Cylinder extrema computation.
//!
//! Computes extrema between a 3D circle and a cylinder.
//!
//! This is a complex problem with multiple potential extrema depending on
//! the relative orientation and position of the circle and cylinder.
//! Uses grid-based numerical approach with Newton refinement for robustness.
class ExtremaCS_CircleCylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle and cylinder geometry.
  ExtremaCS_CircleCylinder(const gp_Circ& theCircle, const gp_Cylinder& theCylinder)
      : myCircle(theCircle),
        myCylinder(theCylinder),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle and cylinder geometry and domain.
  ExtremaCS_CircleCylinder(const gp_Circ&                       theCircle,
                           const gp_Cylinder&                   theCylinder,
                           const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myCircle(theCircle),
        myCylinder(theCylinder),
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
      // Default domains
      aDomain.Curve.Min = 0.0;
      aDomain.Curve.Max = ExtremaCS::THE_TWO_PI;
      aDomain.Surface.UMin = 0.0;
      aDomain.Surface.UMax = ExtremaCS::THE_TWO_PI;
      // V domain based on circle size and position
      const double aRange = myCircle.Radius() * 5.0 + 100.0;
      aDomain.Surface.VMin = -aRange;
      aDomain.Surface.VMax = aRange;
    }

    // Use grid-based numerical approach
    ExtremaCS_Circle aCircleEval(myCircle, aDomain.Curve);
    ExtremaCS_Cylinder aCylEval(myCylinder, aDomain.Surface);

    ExtremaCS_GridEvaluator3D<ExtremaCS_Circle, ExtremaCS_Cylinder> aGridEval(aCircleEval,
                                                                               aCylEval,
                                                                               aDomain);
    aGridEval.Perform(myResult, theTol, theMode);

    return myResult;
  }

private:
  gp_Circ                            myCircle;   //!< Circle geometry
  gp_Cylinder                        myCylinder; //!< Cylinder geometry
  std::optional<ExtremaCS::Domain3D> myDomain;   //!< Optional domain
  mutable ExtremaCS::Result          myResult;   //!< Reusable result storage
};

#endif // _ExtremaCS_CircleCylinder_HeaderFile
