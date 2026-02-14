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

#ifndef _ExtremaCS_Line_HeaderFile
#define _ExtremaCS_Line_HeaderFile

#include <ExtremaCS.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Line curve evaluator wrapper for curve-surface extrema.
//!
//! Provides uniform interface for evaluating line geometry,
//! compatible with ExtremaCS_Numerical template.
class ExtremaCS_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line geometry (unbounded).
  explicit ExtremaCS_Line(const gp_Lin& theLine)
      : myLine(theLine),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line geometry and parameter domain.
  ExtremaCS_Line(const gp_Lin& theLine, const ExtremaCS::Domain1D& theDomain)
      : myLine(theLine),
        myDomain(theDomain)
  {
  }

  //! Evaluates point on line at parameter.
  gp_Pnt Value(double theT) const
  {
    return myLine.Location().Translated(theT * gp_Vec(myLine.Direction()));
  }

  //! Evaluates point and first derivative at parameter.
  void D1(double theT, gp_Pnt& thePt, gp_Vec& theD1) const
  {
    thePt = myLine.Location().Translated(theT * gp_Vec(myLine.Direction()));
    theD1 = gp_Vec(myLine.Direction());
  }

  //! Evaluates point and first two derivatives at parameter.
  void D2(double theT, gp_Pnt& thePt, gp_Vec& theD1, gp_Vec& theD2) const
  {
    thePt = myLine.Location().Translated(theT * gp_Vec(myLine.Direction()));
    theD1 = gp_Vec(myLine.Direction());
    theD2 = gp_Vec(0.0, 0.0, 0.0); // Line has zero curvature
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value() && myDomain->IsFinite(); }

  //! Returns the domain.
  const ExtremaCS::Domain1D& Domain() const
  {
    static const ExtremaCS::Domain1D anUnbounded{-1.0e100, 1.0e100};
    return myDomain.has_value() ? *myDomain : anUnbounded;
  }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  gp_Lin                             myLine;   //!< Line geometry
  std::optional<ExtremaCS::Domain1D> myDomain; //!< Parameter domain
};

#endif // _ExtremaCS_Line_HeaderFile
