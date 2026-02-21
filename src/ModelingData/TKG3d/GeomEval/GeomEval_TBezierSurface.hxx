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

#ifndef _GeomEval_TBezierSurface_HeaderFile
#define _GeomEval_TBezierSurface_HeaderFile

#include <Geom_BoundedSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>

class gp_Trsf;
class Geom_Geometry;
class Geom_Curve;

//! Tensor-product Trigonometric Bezier surface.
//! Uses trigonometric Bernstein-like bases in both U and V directions
//! over the space {1, sin(alpha*t), cos(alpha*t), ..., sin(n*alpha*t), cos(n*alpha*t)}.
//!
//! Parameter domain: U in [0, Pi/alphaU], V in [0, Pi/alphaV].
//! Number of control points: (2*nU + 1) x (2*nV + 1) for orders nU, nV.
//!
//! The surface is:
//! @code
//!   S(u,v) = sum_i sum_j P_ij * Bu_i(u) * Bv_j(v)
//! @endcode
//! where Bu_i and Bv_j are trigonometric basis functions in U and V respectively.
//!
//! For rational surfaces:
//! @code
//!   S(u,v) = sum_i sum_j (w_ij * P_ij * Bu_i(u) * Bv_j(v))
//!          / sum_i sum_j (w_ij * Bu_i(u) * Bv_j(v))
//! @endcode
class GeomEval_TBezierSurface : public Geom_BoundedSurface
{
public:
  //! Constructs a non-rational T-Bezier surface from poles and alpha parameters.
  //! @param[in] thePoles control points grid (row count and col count must be odd >= 3)
  //! @param[in] theAlphaU frequency parameter in U direction (must be > 0)
  //! @param[in] theAlphaV frequency parameter in V direction (must be > 0)
  //! @throw Standard_ConstructionError if validation fails
  Standard_EXPORT GeomEval_TBezierSurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                          double                            theAlphaU,
                                          double                            theAlphaV);

  //! Constructs a rational T-Bezier surface.
  //! @param[in] thePoles control points grid
  //! @param[in] theWeights weights grid (same dimensions as poles, all > 0)
  //! @param[in] theAlphaU frequency parameter in U direction (must be > 0)
  //! @param[in] theAlphaV frequency parameter in V direction (must be > 0)
  //! @throw Standard_ConstructionError if validation fails
  Standard_EXPORT GeomEval_TBezierSurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                          const NCollection_Array2<double>& theWeights,
                                          double                            theAlphaU,
                                          double                            theAlphaV);

  //! Returns the poles grid.
  Standard_EXPORT const NCollection_Array2<gp_Pnt>& Poles() const;

  //! Returns the weights grid (empty if non-rational).
  Standard_EXPORT const NCollection_Array2<double>& Weights() const;

  //! Returns the frequency parameter alpha in the U direction.
  Standard_EXPORT double AlphaU() const;

  //! Returns the frequency parameter alpha in the V direction.
  Standard_EXPORT double AlphaV() const;

  //! Returns the number of poles in the U direction.
  Standard_EXPORT int NbUPoles() const;

  //! Returns the number of poles in the V direction.
  Standard_EXPORT int NbVPoles() const;

  //! Returns the trigonometric order in U (NbUPoles = 2*nU + 1).
  Standard_EXPORT int OrderU() const;

  //! Returns the trigonometric order in V (NbVPoles = 2*nV + 1).
  Standard_EXPORT int OrderV() const;

  //! Returns true if the surface is rational.
  Standard_EXPORT bool IsRational() const;

  // -- Geom_Surface interface --

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void UReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void VReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (Pi/alphaU)
  //! @param[out] V1 lower V bound (0)
  //! @param[out] V2 upper V bound (Pi/alphaV)
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns true if the surface is closed in U.
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns true if the surface is closed in V.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns false. T-Bezier surfaces are not periodic in U.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns false. T-Bezier surfaces are not periodic in V.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Returns GeomAbs_CN. T-Bezier surfaces are infinitely differentiable.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true for all N. T-Bezier surfaces are infinitely differentiable in U.
  Standard_EXPORT bool IsCNu(int N) const final;

  //! Returns true for all N. T-Bezier surfaces are infinitely differentiable in V.
  Standard_EXPORT bool IsCNv(int N) const final;

  //! Isoparametric curve extraction is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Isoparametric curve extraction is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point S(U, V).
  Standard_EXPORT gp_Pnt EvalD0(const double U, const double V) const final;

  //! Computes the point and first partial derivatives at (U, V).
  Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in U and Nv in V.
  //! @param[in] U the u parameter
  //! @param[in] V the v parameter
  //! @param[in] Nu derivative order in U (must be >= 0)
  //! @param[in] Nv derivative order in V (must be >= 0)
  //! @return the derivative vector
  //! @throw Geom_UndefinedDerivative if Nu + Nv < 1 or Nu < 0 or Nv < 0
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                const double V,
                                const int    Nu,
                                const int    Nv) const final;

  //! Transformation is not supported for this eval geometry.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this T-Bezier surface.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_TBezierSurface, Geom_BoundedSurface)

private:
  //! Evaluate trigonometric basis functions in U at parameter theU.
  void evalBasisU(double theU, NCollection_Array1<double>& theBasis) const;

  //! Evaluate trigonometric basis functions in V at parameter theV.
  void evalBasisV(double theV, NCollection_Array1<double>& theBasis) const;

  //! Evaluate derivative of order theOrder of U-basis functions at theU.
  void evalBasisDerivU(double theU, int theOrder, NCollection_Array1<double>& theBasisDeriv) const;

  //! Evaluate derivative of order theOrder of V-basis functions at theV.
  void evalBasisDerivV(double theV, int theOrder, NCollection_Array1<double>& theBasisDeriv) const;

  //! Helper: evaluate a single trigonometric basis array for given alpha.
  static void evalTrigBasis(double                      theT,
                            double                      theAlpha,
                            int                         theOrder,
                            NCollection_Array1<double>& theBasis);

  //! Helper: evaluate derivative of trigonometric basis for given alpha.
  static void evalTrigBasisDeriv(double                      theT,
                                 double                      theAlpha,
                                 int                         theOrder,
                                 int                         theDerivOrder,
                                 NCollection_Array1<double>& theBasisDeriv);

  NCollection_Array2<gp_Pnt> myPoles;    //!< Control points grid
  NCollection_Array2<double> myWeights;  //!< Weights grid (empty if non-rational)
  double                     myAlphaU;   //!< Frequency parameter in U
  double                     myAlphaV;   //!< Frequency parameter in V
  bool                       myRational; //!< True if surface is rational
};

#endif // _GeomEval_TBezierSurface_HeaderFile
