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

#ifndef _GeomProp_Line_HeaderFile
#define _GeomProp_Line_HeaderFile

#include <GeomAdaptor_Curve.hxx>
#include <GeomProp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Local differential properties for a 3D line.
//!
//! A line has constant tangent, zero curvature, undefined normal and centre.
//! No curvature extrema or inflection points exist.
//!
//! @warning The caller must ensure that the adaptor pointer remains valid
//! for the entire lifetime of this object. This class does not manage
//! the adaptor's lifetime.
class GeomProp_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with adaptor pointer (non-owning).
  //! @param theAdaptor the 3D curve adaptor (must wrap a line, must not be null)
  GeomProp_Line(const GeomAdaptor_Curve* theAdaptor)
      : myAdaptor(theAdaptor)
  {
  }

  //! Non-copyable and non-movable.
  GeomProp_Line(const GeomProp_Line&)            = delete;
  GeomProp_Line& operator=(const GeomProp_Line&) = delete;
  GeomProp_Line(GeomProp_Line&&)                 = delete;
  GeomProp_Line& operator=(GeomProp_Line&&)      = delete;

  //! Returns the adaptor pointer.
  const GeomAdaptor_Curve* Adaptor() const { return myAdaptor; }

  //! Compute tangent at given parameter.
  //! For a line, the tangent is always the line direction.
  //! @param[in] theParam curve parameter (unused)
  //! @param[in] theTol tolerance (unused)
  //! @return tangent result (always defined)
  GeomProp::TangentResult Tangent(double theParam, double theTol) const
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
  GeomProp::CurvatureResult Curvature(double theParam, double theTol) const
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
  GeomProp::NormalResult Normal(double theParam, double theTol) const
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
  GeomProp::CentreResult CentreOfCurvature(double theParam, double theTol) const
  {
    (void)theParam;
    (void)theTol;
    return {{}, false};
  }

  //! Find curvature extrema on the line.
  //! A line has no curvature extrema.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindCurvatureExtrema() const { return {{}, true}; }

  //! Find inflection points on the line.
  //! A line has no inflection points.
  //! @return empty analysis (always done)
  GeomProp::CurveAnalysis FindInflections() const { return {{}, true}; }

private:
  const GeomAdaptor_Curve* myAdaptor;
};

#endif // _GeomProp_Line_HeaderFile
