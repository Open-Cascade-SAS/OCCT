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

#ifndef _GeomEval_GregorySurface_HeaderFile
#define _GeomEval_GregorySurface_HeaderFile

#include <Geom_BoundedSurface.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>

class gp_Trsf;
class Geom_Geometry;

//! Gregory patch surface.
//! A bi-cubic Gregory patch with 20 control points: 16 boundary/corner poles
//! (arranged as a 4x4 grid) plus 4 pairs of interior crossing points.
//! At each interior corner, the two control points are blended rationally
//! depending on the parameter direction of approach.
//!
//! The boundary poles form a standard 4x4 bi-cubic Bezier grid.
//! The 4 interior corners (at grid positions (1,1), (1,2), (2,1), (2,2))
//! are replaced by blended pairs:
//!   P(u,v) at corner k = (s * P_k^u + t * P_k^v) / (s + t)
//! where s and t are the parametric distances to the corresponding corner,
//! with averaging at s=t=0.
//!
//! The corner-to-parameter mapping is:
//! - Corner 0 at (u=0, v=0): s=u, t=v
//! - Corner 1 at (u=1, v=0): s=(1-u), t=v
//! - Corner 2 at (u=0, v=1): s=u, t=(1-v)
//! - Corner 3 at (u=1, v=1): s=(1-u), t=(1-v)
//!
//! Parameter domain: [0, 1] x [0, 1].
//! Continuity: C0 at extraordinary vertices, CN elsewhere.
class GeomEval_GregorySurface : public Geom_BoundedSurface
{
public:

  //! Constructs a Gregory surface from boundary poles and interior pairs.
  //! @param[in] thePoles 4x4 array of boundary/corner control points (1-based indexing)
  //! @param[in] theInteriorPairs 4 pairs of interior crossing points.
  //!   Pair 0: corner (1,1) - near (u=0, v=0)
  //!   Pair 1: corner (2,1) - near (u=1, v=0)
  //!   Pair 2: corner (1,2) - near (u=0, v=1)
  //!   Pair 3: corner (2,2) - near (u=1, v=1)
  //!   Each pair: [0] = u-direction point, [1] = v-direction point
  Standard_EXPORT GeomEval_GregorySurface(const NCollection_Array2<gp_Pnt>& thePoles,
                                          const gp_Pnt theInteriorPairs[4][2]);

  //! Returns the 4x4 boundary poles.
  Standard_EXPORT const NCollection_Array2<gp_Pnt>& Poles() const;

  //! Returns the interior pair point for the given corner.
  //! @param[in] theCorner corner index (0-3)
  //! @param[in] theIndex 0 for u-direction point, 1 for v-direction point
  Standard_EXPORT const gp_Pnt& InteriorPoint(int theCorner, int theIndex) const;

  //! Reverses the U direction of parametrization.
  Standard_EXPORT void UReverse() final;

  //! Computes the u parameter on the modified surface,
  //! when reversing its u parametric direction.
  //! @return 1 - U
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Reverses the V direction of parametrization.
  Standard_EXPORT void VReverse() final;

  //! Computes the v parameter on the modified surface,
  //! when reversing its v parametric direction.
  //! @return 1 - V
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this surface.
  //! @param[out] U1 lower U bound (0)
  //! @param[out] U2 upper U bound (1)
  //! @param[out] V1 lower V bound (0)
  //! @param[out] V2 upper V bound (1)
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns False. Gregory patches are not closed in U.
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns False. Gregory patches are not closed in V.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns False. Gregory patches are not periodic in U.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns False. Gregory patches are not periodic in V.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Returns GeomAbs_C0. Gregory patches are C0 at extraordinary vertices.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true if N <= 0.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! Returns true if N <= 0.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! Computes the U isoparametric curve.
  //! Not implemented for Gregory patches.
  //! @return nullptr
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  //! Not implemented for Gregory patches.
  //! @return nullptr
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point P(U, V) on the surface using bi-cubic Bernstein
  //! evaluation with Gregory blending at interior corners.
  Standard_EXPORT gp_Pnt EvalD0(const double U, const double V) const final;

  //! Computes the point and the first partial derivatives at (U, V).
  //! Uses central finite differences on EvalD0.
  Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  //! Uses central finite differences on EvalD0.
  Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  //! Uses central finite differences on EvalD0.
  Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in the direction u
  //! and Nv in the direction v.
  //! Uses central finite differences on EvalD0.
  //! @param[in] U the u parameter
  //! @param[in] V the v parameter
  //! @param[in] Nu derivative order in u (must be >= 0)
  //! @param[in] Nv derivative order in v (must be >= 0)
  //! @return the derivative vector
  //! @throw Geom_UndefinedDerivative if Nu + Nv < 1 or Nu < 0 or Nv < 0
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                const double V,
                                const int    Nu,
                                const int    Nv) const final;

  //! Applies the transformation T to this Gregory surface.
  //! Transforms all poles and interior pairs.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Creates a new object which is a copy of this Gregory surface.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream.
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(GeomEval_GregorySurface, Geom_BoundedSurface)

private:

  //! Compute the effective 4x4 control points for the given (u,v) parameter,
  //! with Gregory blending at the 4 interior corners.
  void computeEffectivePoles(double theU, double theV,
                             NCollection_Array2<gp_Pnt>& theEffPoles) const;

  //! Cubic Bernstein basis value.
  //! @param[in] theI basis index (0-3)
  //! @param[in] theT parameter value
  static double bernstein3(int theI, double theT);

  //! Compute the optimal finite difference step size for a given parameter
  //! value and derivative order.
  //! @param[in] theParam the parameter value
  //! @param[in] theDerivOrder the finite-difference derivative order (1, 2, or 3)
  //! @return optimal step size
  static double finiteDiffStep(double theParam, int theDerivOrder = 1);

  NCollection_Array2<gp_Pnt> myPoles;              //!< 4x4 boundary/corner poles
  gp_Pnt                     myInteriorPairs[4][2]; //!< 4 pairs of interior crossing points
};

#endif // _GeomEval_GregorySurface_HeaderFile
