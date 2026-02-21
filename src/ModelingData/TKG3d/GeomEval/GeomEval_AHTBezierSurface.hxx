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

#ifndef _GeomEval_AHTBezierSurface_HeaderFile
#define _GeomEval_AHTBezierSurface_HeaderFile

#include <Geom_BoundedSurface.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>

class gp_Trsf;
class Geom_Geometry;
class Geom_Curve;

//! Tensor-product Algebraic-Hyperbolic-Trigonometric Bezier surface.
//! Uses a mixed basis in each parametric direction:
//! {1, t, ..., t^k, sinh(alpha*t), cosh(alpha*t), sin(beta*t), cos(beta*t)}.
//!
//! Separate AHT parameters per direction: (algDegreeU, alphaU, betaU)
//! and (algDegreeV, alphaV, betaV).
//!
//! The number of basis functions in each direction must equal the number
//! of poles in that direction.
//! Parameter range: U in [0, 1], V in [0, 1].
class GeomEval_AHTBezierSurface : public Geom_BoundedSurface
{
public:

  //! Non-rational constructor.
  //! @param[in] thePoles 2D array of control points
  //! @param[in] theAlgDegreeU algebraic polynomial degree in U (>= 0)
  //! @param[in] theAlgDegreeV algebraic polynomial degree in V (>= 0)
  //! @param[in] theAlphaU hyperbolic frequency in U (>= 0)
  //! @param[in] theAlphaV hyperbolic frequency in V (>= 0)
  //! @param[in] theBetaU trigonometric frequency in U (>= 0)
  //! @param[in] theBetaV trigonometric frequency in V (>= 0)
  Standard_EXPORT GeomEval_AHTBezierSurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                            int                               theAlgDegreeU,
                                            int                               theAlgDegreeV,
                                            double                            theAlphaU,
                                            double                            theAlphaV,
                                            double                            theBetaU,
                                            double                            theBetaV);

  //! Rational constructor.
  //! @param[in] thePoles 2D array of control points
  //! @param[in] theWeights 2D array of weights (must be > 0)
  //! @param[in] theAlgDegreeU algebraic polynomial degree in U (>= 0)
  //! @param[in] theAlgDegreeV algebraic polynomial degree in V (>= 0)
  //! @param[in] theAlphaU hyperbolic frequency in U (>= 0)
  //! @param[in] theAlphaV hyperbolic frequency in V (>= 0)
  //! @param[in] theBetaU trigonometric frequency in U (>= 0)
  //! @param[in] theBetaV trigonometric frequency in V (>= 0)
  Standard_EXPORT GeomEval_AHTBezierSurface(const NCollection_Array2<gp_Pnt>&  thePoles,
                                            const NCollection_Array2<double>& theWeights,
                                            int                               theAlgDegreeU,
                                            int                               theAlgDegreeV,
                                            double                            theAlphaU,
                                            double                            theAlphaV,
                                            double                            theBetaU,
                                            double                            theBetaV);

  //! Returns the 2D array of poles.
  Standard_EXPORT const NCollection_Array2<gp_Pnt>& Poles() const;

  //! Returns the 2D array of weights.
  Standard_EXPORT const NCollection_Array2<double>& Weights() const;

  //! Returns the algebraic polynomial degree in U.
  Standard_EXPORT int AlgDegreeU() const;

  //! Returns the algebraic polynomial degree in V.
  Standard_EXPORT int AlgDegreeV() const;

  //! Returns the hyperbolic frequency in U.
  Standard_EXPORT double AlphaU() const;

  //! Returns the hyperbolic frequency in V.
  Standard_EXPORT double AlphaV() const;

  //! Returns the trigonometric frequency in U.
  Standard_EXPORT double BetaU() const;

  //! Returns the trigonometric frequency in V.
  Standard_EXPORT double BetaV() const;

  //! Returns the number of poles in U direction.
  Standard_EXPORT int NbPolesU() const;

  //! Returns the number of poles in V direction.
  Standard_EXPORT int NbPolesV() const;

  //! Returns true if the surface is rational in U direction.
  Standard_EXPORT bool IsURational() const;

  //! Returns true if the surface is rational in V direction.
  Standard_EXPORT bool IsVRational() const;

  // Surface interface

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void UReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT void VReverse() final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Reversal is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (1)
  //! @param[out] V1 lower V bound (0)
  //! @param[out] V2 upper V bound (1)
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns false. The AHT-Bezier surface is not closed in U.
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns false. The AHT-Bezier surface is not closed in V.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns false. The AHT-Bezier surface is not periodic in U.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns false. The AHT-Bezier surface is not periodic in V.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Isoparametric curve extraction is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Isoparametric curve extraction is not supported for this eval surface.
  //! @throw Standard_NotImplemented
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Returns GeomAbs_CN.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true for any N.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! Returns true for any N.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! Computes the point at parameters (U, V).
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
  //! @param[in] Nu derivative order in u (must be >= 0)
  //! @param[in] Nv derivative order in v (must be >= 0)
  //! @return the derivative vector
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                const double V,
                                const int    Nu,
                                const int    Nv) const final;

  //! Applies the transformation T to this surface.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this surface.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_AHTBezierSurface, Geom_BoundedSurface)

private:

  //! Compute the number of basis functions for the given parameters.
  static int basisDimension(int theAlgDegree, double theAlpha, double theBeta);

  NCollection_Array2<gp_Pnt>  myPoles;
  NCollection_Array2<double>  myWeights;
  int                         myAlgDegreeU;
  int                         myAlgDegreeV;
  double                      myAlphaU;
  double                      myAlphaV;
  double                      myBetaU;
  double                      myBetaV;
  bool                        myURational;
  bool                        myVRational;
};

#endif // _GeomEval_AHTBezierSurface_HeaderFile
