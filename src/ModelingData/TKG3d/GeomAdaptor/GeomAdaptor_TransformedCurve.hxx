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

#ifndef _GeomAdaptor_TransformedCurve_HeaderFile
#define _GeomAdaptor_TransformedCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Trsf.hxx>

//! An adaptor for curves with an applied transformation.
//!
//! This class wraps a GeomAdaptor_Curve (or an Adaptor3d_CurveOnSurface) and
//! applies a gp_Trsf transformation to all point and derivative evaluations.
//! It serves as a base class for BRepAdaptor_Curve and allows batch evaluation
//! with transformations in GeomGridEval_Curve.
//!
//! The evaluation methods (Value, D0, D1, D2, D3, DN) are marked final
//! to enable optimizations in grid evaluation.
class GeomAdaptor_TransformedCurve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_TransformedCurve, Adaptor3d_Curve)
public:
  //! Creates an undefined curve with identity transformation.
  Standard_EXPORT GeomAdaptor_TransformedCurve();

  //! Creates a curve adaptor with transformation.
  //! @param theCurve underlying geometry
  //! @param theTrsf transformation to apply
  Standard_EXPORT GeomAdaptor_TransformedCurve(const occ::handle<Geom_Curve>& theCurve,
                                               const gp_Trsf&                theTrsf);

  //! Creates a curve adaptor with transformation and parameter bounds.
  //! @param theCurve underlying geometry
  //! @param theFirst minimum parameter
  //! @param theLast maximum parameter
  //! @param theTrsf transformation to apply
  Standard_EXPORT GeomAdaptor_TransformedCurve(const occ::handle<Geom_Curve>& theCurve,
                                               const double                  theFirst,
                                               const double                  theLast,
                                               const gp_Trsf&                theTrsf);

  //! Shallow copy of adaptor.
  Standard_EXPORT occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Loads the curve geometry.
  //! @param theCurve underlying geometry
  void Load(const occ::handle<Geom_Curve>& theCurve) { myCurve.Load(theCurve); }

  //! Loads the curve geometry with parameter bounds.
  //! @param theCurve underlying geometry
  //! @param theFirst minimum parameter
  //! @param theLast maximum parameter
  void Load(const occ::handle<Geom_Curve>& theCurve, const double theFirst, const double theLast)
  {
    myCurve.Load(theCurve, theFirst, theLast);
  }

  //! Sets the curve on surface adaptor.
  //! @param theConSurf curve on surface adaptor
  void LoadCurveOnSurface(const occ::handle<Adaptor3d_CurveOnSurface>& theConSurf)
  {
    myConSurf = theConSurf;
  }

  //! Sets the transformation.
  //! @param theTrsf transformation to apply
  void SetTrsf(const gp_Trsf& theTrsf) { myTrsf = theTrsf; }

  //! Returns the transformation.
  const gp_Trsf& Trsf() const { return myTrsf; }

  //! Returns true if the geometry is a 3D curve (not curve on surface).
  bool Is3DCurve() const { return myConSurf.IsNull(); }

  //! Returns true if the geometry is a curve on surface.
  bool IsCurveOnSurface() const { return !myConSurf.IsNull(); }

  //! Returns the underlying GeomAdaptor_Curve.
  const GeomAdaptor_Curve& Curve() const { return myCurve; }

  //! Returns the underlying GeomAdaptor_Curve for modification.
  GeomAdaptor_Curve& ChangeCurve() { return myCurve; }

  //! Returns the CurveOnSurface adaptor.
  const Adaptor3d_CurveOnSurface& CurveOnSurface() const { return *myConSurf; }

  //! Returns the underlying Geom_Curve.
  const occ::handle<Geom_Curve>& GeomCurve() const { return myCurve.Curve(); }

  // Parameter range methods - delegate to underlying curve or COS
  double FirstParameter() const override
  {
    return myConSurf.IsNull() ? myCurve.FirstParameter() : myConSurf->FirstParameter();
  }

  double LastParameter() const override
  {
    return myConSurf.IsNull() ? myCurve.LastParameter() : myConSurf->LastParameter();
  }

  GeomAbs_Shape Continuity() const override
  {
    return myConSurf.IsNull() ? myCurve.Continuity() : myConSurf->Continuity();
  }

  int NbIntervals(const GeomAbs_Shape theS) const override
  {
    return myConSurf.IsNull() ? myCurve.NbIntervals(theS) : myConSurf->NbIntervals(theS);
  }

  Standard_EXPORT void Intervals(NCollection_Array1<double>& theT,
                                 const GeomAbs_Shape         theS) const override;

  Standard_EXPORT occ::handle<Adaptor3d_Curve> Trim(const double theFirst,
                                                     const double theLast,
                                                     const double theTol) const override;

  bool IsClosed() const override
  {
    return myConSurf.IsNull() ? myCurve.IsClosed() : myConSurf->IsClosed();
  }

  bool IsPeriodic() const override
  {
    return myConSurf.IsNull() ? myCurve.IsPeriodic() : myConSurf->IsPeriodic();
  }

  double Period() const override
  {
    return myConSurf.IsNull() ? myCurve.Period() : myConSurf->Period();
  }

  //! Computes the point of parameter U on the curve.
  //! Applies transformation after evaluation.
  Standard_EXPORT gp_Pnt Value(const double theU) const final;

  //! Computes the point of parameter U.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D0(const double theU, gp_Pnt& theP) const final;

  //! Computes the point and first derivative.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D1(const double theU, gp_Pnt& theP, gp_Vec& theV) const final;

  //! Computes the point, first and second derivatives.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D2(const double theU,
                          gp_Pnt&      theP,
                          gp_Vec&      theV1,
                          gp_Vec&      theV2) const final;

  //! Computes the point, first, second and third derivatives.
  //! Applies transformation after evaluation.
  Standard_EXPORT void D3(const double theU,
                          gp_Pnt&      theP,
                          gp_Vec&      theV1,
                          gp_Vec&      theV2,
                          gp_Vec&      theV3) const final;

  //! Computes the derivative of order N.
  //! Applies transformation after evaluation.
  Standard_EXPORT gp_Vec DN(const double theU, const int theN) const final;

  double Resolution(const double theR3d) const override
  {
    return myConSurf.IsNull() ? myCurve.Resolution(theR3d) : myConSurf->Resolution(theR3d);
  }

  GeomAbs_CurveType GetType() const override
  {
    return myConSurf.IsNull() ? myCurve.GetType() : myConSurf->GetType();
  }

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  int Degree() const override
  {
    return myConSurf.IsNull() ? myCurve.Degree() : myConSurf->Degree();
  }

  bool IsRational() const override
  {
    return myConSurf.IsNull() ? myCurve.IsRational() : myConSurf->IsRational();
  }

  int NbPoles() const override
  {
    return myConSurf.IsNull() ? myCurve.NbPoles() : myConSurf->NbPoles();
  }

  int NbKnots() const override
  {
    return myConSurf.IsNull() ? myCurve.NbKnots() : myConSurf->NbKnots();
  }

  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

  Standard_EXPORT occ::handle<Geom_OffsetCurve> OffsetCurve() const override;

protected:
  GeomAdaptor_Curve                     myCurve;
  occ::handle<Adaptor3d_CurveOnSurface> myConSurf;
  gp_Trsf                               myTrsf;
};

#endif // _GeomAdaptor_TransformedCurve_HeaderFile
