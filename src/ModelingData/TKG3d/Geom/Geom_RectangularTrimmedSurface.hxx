// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Geom_RectangularTrimmedSurface_HeaderFile
#define _Geom_RectangularTrimmedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_BoundedSurface.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_Surface;
class Geom_Curve;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

//! Describes a portion of a surface (a patch) limited
//! by two values of the u parameter in the u
//! parametric direction, and two values of the v
//! parameter in the v parametric direction. The
//! domain of the trimmed surface must be within the
//! domain of the surface being trimmed.
//! The trimmed surface is defined by:
//! - the basis surface, and
//! - the values (umin, umax) and (vmin, vmax)
//! which limit it in the u and v parametric directions.
//! The trimmed surface is built from a copy of the basis
//! surface. Therefore, when the basis surface is
//! modified the trimmed surface is not changed.
//! Consequently, the trimmed surface does not
//! necessarily have the same orientation as the basis surface.
//! Warning: The case of surface being trimmed is periodic and
//! parametrics values are outside the domain is possible.
//! But, domain of the trimmed surface can be translated
//! by (n X) the period.
class Geom_RectangularTrimmedSurface : public Geom_BoundedSurface
{

public:
  //! The U parametric direction of the surface is oriented from U1
  //! to U2. The V parametric direction of the surface is oriented
  //! from V1 to V2.
  //! These two directions define the orientation of the surface
  //! (normal). If the surface is not periodic USense and VSense are
  //! not used for the construction. If the surface S is periodic in
  //! one direction USense and VSense give the available part of the
  //! surface. By default in this case the surface has the same
  //! orientation as the basis surface S.
  //! The returned surface is not closed and not periodic.
  //! ConstructionError Raised if
  //! S is not periodic in the UDirection and U1 or U2 are out of the
  //! bounds of S.
  //! S is not periodic in the VDirection and V1 or V2 are out of the
  //! bounds of S.
  //! U1 = U2 or V1 = V2
  Standard_EXPORT Geom_RectangularTrimmedSurface(const occ::handle<Geom_Surface>& S,
                                                 const double                     U1,
                                                 const double                     U2,
                                                 const double                     V1,
                                                 const double                     V2,
                                                 const bool                       USense = true,
                                                 const bool                       VSense = true);

  //! The basis surface S is only trim in one parametric direction.
  //! If UTrim = True the surface is trimmed in the U parametric
  //! direction else the surface is trimmed in the V parametric
  //! direction.
  //! In the considered parametric direction the resulting surface is
  //! oriented from Param1 to Param2. If S is periodic Sense gives the
  //! available part of the surface. By default the trimmed surface has
  //! the same orientation as the basis surface S in the considered
  //! parametric direction (Sense = True).
  //! If the basis surface S is closed or periodic in the parametric
  //! direction opposite to the trimming direction the trimmed surface
  //! has the same characteristics as the surface S in this direction.
  //! Warnings :
  //! In this package the entities are not shared.
  //! The RectangularTrimmedSurface is built with a copy of the
  //! surface S. So when S is modified the RectangularTrimmedSurface
  //! is not modified
  //! Raised if
  //! S is not periodic in the considered parametric direction and
  //! Param1 or Param2 are out of the bounds of S.
  //! Param1 = Param2
  Standard_EXPORT Geom_RectangularTrimmedSurface(const occ::handle<Geom_Surface>& S,
                                                 const double                     Param1,
                                                 const double                     Param2,
                                                 const bool                       UTrim,
                                                 const bool                       Sense = true);

  //! Modifies this patch by changing the trim values
  //! applied to the original surface
  //! The u parametric direction of
  //! this patch is oriented from U1 to U2. The v
  //! parametric direction of this patch is oriented
  //! from V1 to V2. USense and VSense are used
  //! for the construction only if the surface is periodic
  //! in the corresponding parametric direction, and
  //! define the available part of the surface; by default
  //! in this case, this patch has the same orientation
  //! as the basis surface.
  //! Raised if
  //! The BasisSurface is not periodic in the UDirection and U1 or U2
  //! are out of the bounds of the BasisSurface.
  //! The BasisSurface is not periodic in the VDirection and V1 or V2
  //! are out of the bounds of the BasisSurface.
  //! U1 = U2 or V1 = V2
  Standard_EXPORT void SetTrim(const double U1,
                               const double U2,
                               const double V1,
                               const double V2,
                               const bool   USense = true,
                               const bool   VSense = true);

  //! Modifies this patch by changing the trim values
  //! applied to the original surface
  //! The basis surface is trimmed only in one parametric direction: if UTrim
  //! is true, the surface is trimmed in the u parametric
  //! direction; if it is false, it is trimmed in the v
  //! parametric direction. In the "trimmed" direction,
  //! this patch is oriented from Param1 to Param2. If
  //! the basis surface is periodic in the "trimmed"
  //! direction, Sense defines its available part. By
  //! default in this case, this patch has the same
  //! orientation as the basis surface in this parametric
  //! direction. If the basis surface is closed or periodic
  //! in the other parametric direction (i.e. not the
  //! "trimmed" direction), this patch has the same
  //! characteristics as the basis surface in that parametric direction.
  //! Raised if
  //! The BasisSurface is not periodic in the considered direction and
  //! Param1 or Param2 are out of the bounds of the BasisSurface.
  //! Param1 = Param2
  Standard_EXPORT void SetTrim(const double Param1,
                               const double Param2,
                               const bool   UTrim,
                               const bool   Sense = true);

  //! Returns the Basis surface of <me>.
  Standard_EXPORT occ::handle<Geom_Surface> BasisSurface() const;

  //! Changes the orientation of this patch in the u
  //! parametric direction. The bounds of the surface are
  //! not changed, but the given parametric direction is
  //! reversed. Hence the orientation of the surface is reversed.
  Standard_EXPORT void UReverse() final;

  //! Computes the u parameter on the modified
  //! surface, produced by when reversing its u
  //! parametric direction, for any point of u parameter U on this patch.
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Changes the orientation of this patch in the v
  //! parametric direction. The bounds of the surface are
  //! not changed, but the given parametric direction is
  //! reversed. Hence the orientation of the surface is reversed.
  Standard_EXPORT void VReverse() final;

  //! Computes the v parameter on the modified
  //! surface, produced by when reversing its v
  //! parametric direction, for any point of v parameter V on this patch.
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds U1, U2, V1 and V2 of this patch.
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! Returns the continuity of the surface :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Surface,
  //! C2 : continuity of the second derivative all along the Surface,
  //! C3 : continuity of the third derivative all along the Surface,
  //! CN : the order of continuity is infinite.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! Returns true if this patch is closed in the given parametric direction.
  Standard_EXPORT bool IsUClosed() const final;

  //! Returns true if this patch is closed in the given parametric direction.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns true if the order of derivation in the U parametric
  //! direction is N.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! Returns true if the order of derivation in the V parametric
  //! direction is N.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! Returns true if this patch is periodic and not trimmed in the given
  //! parametric direction.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns the period of this patch in the u
  //! parametric direction.
  //! raises if the surface is not uperiodic.
  Standard_EXPORT double UPeriod() const final;

  //! Returns true if this patch is periodic and not trimmed in the given
  //! parametric direction.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Returns the period of this patch in the v
  //! parametric direction.
  //! raises if the surface is not vperiodic.
  //! value and derivatives
  Standard_EXPORT double VPeriod() const final;

  //! computes the U isoparametric curve.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! Computes the point of parameter (U, V) on the surface.
  //! Can be raised if the basis surface is an OffsetSurface.
  //! Returns std::nullopt on failure.
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U, const double V) const final;

  //! Computes the point and first partial derivatives at (U, V).
  //! The returned derivatives have the same orientation as the
  //! derivatives of the basis surface even if the trimmed surface
  //! has not the same parametric orientation.
  //! Returns std::nullopt if the surface continuity is not C1.
  Standard_EXPORT std::optional<Geom_Surface::ResD1> EvalD1(const double U,
                                                            const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  //! The returned derivatives have the same orientation as the
  //! derivatives of the basis surface even if the trimmed surface
  //! has not the same parametric orientation.
  //! Returns std::nullopt if the surface continuity is not C2.
  Standard_EXPORT std::optional<Geom_Surface::ResD2> EvalD2(const double U,
                                                            const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  //! The returned derivatives have the same orientation as the
  //! derivatives of the basis surface even if the trimmed surface
  //! has not the same parametric orientation.
  //! Returns std::nullopt if the surface continuity is not C3.
  Standard_EXPORT std::optional<Geom_Surface::ResD3> EvalD3(const double U,
                                                            const double V) const final;

  //! Computes the derivative of order Nu in U and Nv in V at (U, V).
  //! The returned derivative has the same orientation as the
  //! derivative of the basis surface even if the trimmed surface
  //! has not the same parametric orientation.
  //! Returns std::nullopt on failure.
  //! RangeError Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this patch.
  //! Warning
  //! As a consequence, the basis surface included in the
  //! data structure of this patch is also modified.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Computes the parameters on the transformed surface for
  //! the transform of the point of parameters U,V on <me>.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are the new values of U,V after calling
  //! @code
  //!   me->TransformParameters(U,V,T)
  //! @endcode
  //! This method calls the basis surface method.
  Standard_EXPORT void TransformParameters(double& U, double& V, const gp_Trsf& T) const final;

  //! Returns a 2d transformation used to find the new
  //! parameters of a point on the transformed surface.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are obtained by transforming U,V with
  //! the 2d transformation returned by
  //! @code
  //!   me->ParametricTransformation(T)
  //! @endcode
  //! This method calls the basis surface method.
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const final;

  //! Creates a new object which is a copy of this patch.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_RectangularTrimmedSurface, Geom_BoundedSurface)

private:
  //! General set trim, to implement constructors and
  //! others set trim.
  Standard_EXPORT void SetTrim(const double U1,
                               const double U2,
                               const double V1,
                               const double V2,
                               const bool   UTrim,
                               const bool   VTrim,
                               const bool   USense,
                               const bool   VSense);

  occ::handle<Geom_Surface> basisSurf;
  double                    utrim1;
  double                    vtrim1;
  double                    utrim2;
  double                    vtrim2;
  bool                      isutrimmed;
  bool                      isvtrimmed;
};

#endif // _Geom_RectangularTrimmedSurface_HeaderFile
