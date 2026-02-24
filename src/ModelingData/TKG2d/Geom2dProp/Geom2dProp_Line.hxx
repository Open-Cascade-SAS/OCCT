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

#ifndef _Geom2dProp_Line_HeaderFile
#define _Geom2dProp_Line_HeaderFile

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dProp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a 2D line.
//!
//! A line has constant tangent, zero curvature, undefined normal and centre.
//! No curvature extrema or inflection points exist.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object. This class does not manage
//! the adaptor's lifetime.
class Geom2dProp_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 2D curve adaptor (must wrap a line, must not be null)
  Geom2dProp_Line(const Geom2dAdaptor_Curve* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  Geom2dProp_Line(const Geom2dProp_Line&)            = delete;
  Geom2dProp_Line& operator=(const Geom2dProp_Line&) = delete;
  Geom2dProp_Line(Geom2dProp_Line&&)                 = delete;
  Geom2dProp_Line& operator=(Geom2dProp_Line&&)      = delete;

  //! Returns the adaptor pointer.
  const Geom2dAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Compute tangent at given parameter.
  //! For a line, the tangent is always the line direction.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return tangent result (always defined)
  Geom2dProp::TangentResult Tangent(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {myAdaptor->Line().Direction(), true};
  }

  //! Compute curvature at given parameter.
  //! For a line, curvature is always zero.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return curvature result (always zero)
  Geom2dProp::CurvatureResult Curvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {0.0, true, false};
  }

  //! Compute normal at given parameter.
  //! For a line, the normal is undefined (zero curvature).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return normal result (always undefined)
  Geom2dProp::NormalResult Normal(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {{}, false};
  }

  //! Compute centre of curvature at given parameter.
  //! For a line, the centre is undefined (zero curvature).
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return centre result (always undefined)
  Geom2dProp::CentreResult CentreOfCurvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {{}, false};
  }

  //! Find curvature extrema on the line.
  //! A line has no curvature extrema.
  //! @return empty analysis (always done)
  Geom2dProp::CurveAnalysis FindCurvatureExtrema() const { return {{}, true}; }

  //! Find inflection points on the line.
  //! A line has no inflection points.
  //! @return empty analysis (always done)
  Geom2dProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const Geom2dAdaptor_Curve* myAdaptor;
};

#endif // _Geom2dProp_Line_HeaderFile
