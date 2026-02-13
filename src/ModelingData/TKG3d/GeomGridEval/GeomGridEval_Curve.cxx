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

#include <GeomGridEval_Curve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>

namespace
{

//! Extracts basis curve from potentially nested TrimmedCurve wrappers.
//! @param theCurve input curve (may be TrimmedCurve or any other)
//! @return the underlying basis curve, or theCurve if not a TrimmedCurve
occ::handle<Geom_Curve> ExtractBasisCurve(const occ::handle<Geom_Curve>& theCurve)
{
  occ::handle<Geom_Curve> aResult = theCurve;
  while (auto aTrimmed = occ::down_cast<Geom_TrimmedCurve>(aResult))
  {
    aResult = aTrimmed->BasisCurve();
  }
  return aResult;
}

//! Creates Geom_Curve from adaptor's elementary curve type.
//! @param theCurve the adaptor to extract geometry from
//! @return Geom_Curve handle, or null if type is not elementary
occ::handle<Geom_Curve> CreateGeomCurveFromAdaptor(const Adaptor3d_Curve& theCurve)
{
  switch (theCurve.GetType())
  {
    case GeomAbs_Line:
      return new Geom_Line(theCurve.Line());
    case GeomAbs_Circle:
      return new Geom_Circle(theCurve.Circle());
    case GeomAbs_Ellipse:
      return new Geom_Ellipse(theCurve.Ellipse());
    case GeomAbs_Hyperbola:
      return new Geom_Hyperbola(theCurve.Hyperbola());
    case GeomAbs_Parabola:
      return new Geom_Parabola(theCurve.Parabola());
    default:
      return {};
  }
}

} // namespace

//==================================================================================================

void GeomGridEval_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  myTrsf.reset();

  // Check for GeomAdaptor_TransformedCurve (includes BRepAdaptor_Curve)
  // to extract transformation and underlying geometry
  if (theCurve.IsKind(STANDARD_TYPE(GeomAdaptor_TransformedCurve)))
  {
    const auto&    aTransformed = static_cast<const GeomAdaptor_TransformedCurve&>(theCurve);
    const gp_Trsf& aTrsf        = aTransformed.Trsf();

    if (aTransformed.Is3DCurve())
    {
      // Extract inner Geom_Curve (in local coordinates) - needs transform.
      // Initialize(Geom_Curve) resets myTrsf, so set it after.
      Initialize(aTransformed.GeomCurve());
      if (aTrsf.Form() != gp_Identity)
      {
        myTrsf = aTrsf;
      }
      return;
    }

    // Curve-on-surface: try to create Geom_Curve from recognized elementary type
    const Adaptor3d_CurveOnSurface& aCOS     = aTransformed.CurveOnSurface();
    occ::handle<Geom_Curve>         aCOSGeom = CreateGeomCurveFromAdaptor(aCOS);
    if (!aCOSGeom.IsNull())
    {
      // Created curve is in COS local space - needs transform.
      // Initialize(Geom_Curve) resets myTrsf, so set it after.
      Initialize(aCOSGeom);
      if (aTrsf.Form() != gp_Identity)
      {
        myTrsf = aTrsf;
      }
      return;
    }

    // Use CurveOnSurface evaluator with direct chain rule composition.
    // COS evaluates in local surface coordinates; myTrsf handles the
    // transform to world space (same pattern as 3D curve extraction).
    myCurveType = aCOS.GetType();
    if (aTrsf.Form() != gp_Identity)
    {
      myTrsf = aTrsf;
    }
    myEvaluator.emplace<GeomGridEval_CurveOnSurface>(aCOS.GetCurve(), aCOS.GetSurface());
    return;
  }

  // Check for plain GeomAdaptor_Curve (without transformation)
  if (theCurve.IsKind(STANDARD_TYPE(GeomAdaptor_Curve)))
  {
    Initialize(static_cast<const GeomAdaptor_Curve&>(theCurve).Curve());
    return;
  }

  // For non-GeomAdaptor or when Geom_Curve is not available,
  // use reference for the evaluator
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<GeomGridEval_OtherCurve>(theCurve);
}

//==================================================================================================

void GeomGridEval_Curve::Initialize(const occ::handle<Geom_Curve>& theCurve)
{
  myTrsf.reset();

  if (theCurve.IsNull())
  {
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  // Extract basis curve from potentially nested TrimmedCurve wrappers
  occ::handle<Geom_Curve> aBasisCurve = ExtractBasisCurve(theCurve);

  if (auto aLine = occ::down_cast<Geom_Line>(aBasisCurve))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<GeomGridEval_Line>(aLine);
  }
  else if (auto aCircle = occ::down_cast<Geom_Circle>(aBasisCurve))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<GeomGridEval_Circle>(aCircle);
  }
  else if (auto anEllipse = occ::down_cast<Geom_Ellipse>(aBasisCurve))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<GeomGridEval_Ellipse>(anEllipse);
  }
  else if (auto aHyperbola = occ::down_cast<Geom_Hyperbola>(aBasisCurve))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<GeomGridEval_Hyperbola>(aHyperbola);
  }
  else if (auto aParabola = occ::down_cast<Geom_Parabola>(aBasisCurve))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<GeomGridEval_Parabola>(aParabola);
  }
  else if (auto aBezier = occ::down_cast<Geom_BezierCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<GeomGridEval_BezierCurve>(aBezier);
  }
  else if (auto aBSpline = occ::down_cast<Geom_BSplineCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<GeomGridEval_BSplineCurve>(aBSpline);
  }
  else if (auto anOffset = occ::down_cast<Geom_OffsetCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_OffsetCurve;
    myEvaluator.emplace<GeomGridEval_OffsetCurve>(anOffset);
  }
  else
  {
    // Unknown curve type - set uninitialized
    // All known Geom_Curve types are handled above
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
  }
}

//==================================================================================================

bool GeomGridEval_Curve::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_Curve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  NCollection_Array1<gp_Pnt> aResult = std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<gp_Pnt> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Pnt>();
      }
      else
      {
        return theEval.EvaluateGrid(theParams);
      }
    },
    myEvaluator);

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_Curve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  NCollection_Array1<GeomGridEval::CurveD1> aResult = std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD1> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD1>();
      }
      else
      {
        return theEval.EvaluateGridD1(theParams);
      }
    },
    myEvaluator);

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_Curve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  NCollection_Array1<GeomGridEval::CurveD2> aResult = std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD2> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD2>();
      }
      else
      {
        return theEval.EvaluateGridD2(theParams);
      }
    },
    myEvaluator);

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_Curve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  NCollection_Array1<GeomGridEval::CurveD3> aResult = std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD3> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD3>();
      }
      else
      {
        return theEval.EvaluateGridD3(theParams);
      }
    },
    myEvaluator);

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_Curve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  NCollection_Array1<gp_Vec> aResult = std::visit(
    [&theParams, theN](const auto& theEval) -> NCollection_Array1<gp_Vec> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Vec>();
      }
      else
      {
        return theEval.EvaluateGridDN(theParams, theN);
      }
    },
    myEvaluator);

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

void GeomGridEval_Curve::applyTransformation(NCollection_Array1<gp_Pnt>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int i = theGrid.Lower(); i <= theGrid.Upper(); ++i)
  {
    theGrid.ChangeValue(i).Transform(aTrsf);
  }
}

//==================================================================================================

void GeomGridEval_Curve::applyTransformation(
  NCollection_Array1<GeomGridEval::CurveD1>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int i = theGrid.Lower(); i <= theGrid.Upper(); ++i)
  {
    GeomGridEval::CurveD1& aVal = theGrid.ChangeValue(i);
    aVal.Point.Transform(aTrsf);
    aVal.D1.Transform(aTrsf);
  }
}

//==================================================================================================

void GeomGridEval_Curve::applyTransformation(
  NCollection_Array1<GeomGridEval::CurveD2>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int i = theGrid.Lower(); i <= theGrid.Upper(); ++i)
  {
    GeomGridEval::CurveD2& aVal = theGrid.ChangeValue(i);
    aVal.Point.Transform(aTrsf);
    aVal.D1.Transform(aTrsf);
    aVal.D2.Transform(aTrsf);
  }
}

//==================================================================================================

void GeomGridEval_Curve::applyTransformation(
  NCollection_Array1<GeomGridEval::CurveD3>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int i = theGrid.Lower(); i <= theGrid.Upper(); ++i)
  {
    GeomGridEval::CurveD3& aVal = theGrid.ChangeValue(i);
    aVal.Point.Transform(aTrsf);
    aVal.D1.Transform(aTrsf);
    aVal.D2.Transform(aTrsf);
    aVal.D3.Transform(aTrsf);
  }
}

//==================================================================================================

void GeomGridEval_Curve::applyTransformation(NCollection_Array1<gp_Vec>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int i = theGrid.Lower(); i <= theGrid.Upper(); ++i)
  {
    theGrid.ChangeValue(i).Transform(aTrsf);
  }
}
