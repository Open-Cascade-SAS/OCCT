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

#ifndef _Geom_OffsetSurface_HeaderFile
#define _Geom_OffsetSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>

#include <memory>

class Geom_Curve;
class Geom_OsculatingSurface;
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Geometry;

//! Describes an offset surface in 3D space.
//! An offset surface is defined by:
//! - the basis surface to which it is parallel, and
//! - the distance between the offset surface and its basis surface.
//! A point on the offset surface is built by measuring the
//! offset value along the normal vector at a point on the
//! basis surface. This normal vector is given by the cross
//! product D1u^D1v, where D1u and D1v are the
//! vectors tangential to the basis surface in the u and v
//! parametric directions at this point. The side of the
//! basis surface on which the offset is measured
//! depends on the sign of the offset value.
//! A Geom_OffsetSurface surface can be
//! self-intersecting, even if the basis surface does not
//! self-intersect. The self-intersecting portions are not
//! deleted at the time of construction.
//! Warning
//! There must be only one normal vector defined at any
//! point on the basis surface. This must be verified by the
//! user as no check is made at the time of construction
//! to detect points with multiple possible normal
//! directions (for example, the top of a conical surface).
class Geom_OffsetSurface : public Geom_Surface
{

public:


  //! Constructs a surface offset from the basis surface
  //! S, where Offset is the distance between the offset
  //! surface and the basis surface at any point.
  //! A point on the offset surface is built by measuring
  //! the offset value along a normal vector at a point on
  //! S. This normal vector is given by the cross product
  //! D1u^D1v, where D1u and D1v are the vectors
  //! tangential to the basis surface in the u and v
  //! parametric directions at this point. The side of S on
  //! which the offset value is measured is indicated by
  //! this normal vector if Offset is positive, or is the
  //! inverse sense if Offset is negative.
  //! If isNotCheckC0 = TRUE checking if basis surface has C0-continuity
  //! is not made.
  //! Warnings :
  //! - The offset surface is built with a copy of the
  //! surface S. Therefore, when S is modified the
  //! offset surface is not modified.
  //! - No check is made at the time of construction to
  //! detect points on S with multiple possible normal directions.
  //! Raised if S is not at least C1.
  //! Warnings :
  //! No check is done to verify that a unique normal direction is
  //! defined at any point of the basis surface S.
  Standard_EXPORT Geom_OffsetSurface(const occ::handle<Geom_Surface>& S,
                                     const double                     Offset,
                                     const bool                       isNotCheckC0 = false);

  //! Raised if S is not at least C1.
  //! Warnings :
  //! No check is done to verify that a unique normal direction is
  //! defined at any point of the basis surface S.
  //! If isNotCheckC0 = TRUE checking if basis surface has C0-continuity
  //! is not made.
  //! Exceptions
  //! Standard_ConstructionError if the surface S is not
  //! at least "C1" continuous.
  Standard_EXPORT void SetBasisSurface(const occ::handle<Geom_Surface>& S,
                                       const bool                       isNotCheckC0 = false);

  //! Copy constructor for optimized copying without validation.
  //! @param[in] theOther the offset surface to copy from
  Standard_EXPORT Geom_OffsetSurface(const Geom_OffsetSurface& theOther);

  //! Destructor.
  Standard_EXPORT ~Geom_OffsetSurface() override;

  //! Changes this offset surface by assigning D as the offset value.
  Standard_EXPORT void SetOffsetValue(const double D);

  //! Returns the offset value of this offset surface.
  inline double Offset() const { return offsetValue; }

  //! Returns the basis surface of this offset surface.
  //! Note: The basis surface can be an offset surface.
  inline const occ::handle<Geom_Surface>& BasisSurface() const { return basisSurf; }

  //! Changes the orientation of this offset surface in the u
  //! parametric direction. The bounds of the surface
  //! are not changed but the given parametric direction is reversed.
  Standard_EXPORT void UReverse() final;

  //! Computes the u parameter on the modified
  //! surface, produced by reversing the u
  //! parametric direction of this offset surface, for any
  //! point of u parameter U on this offset surface.
  Standard_EXPORT double UReversedParameter(const double U) const final;

  //! Changes the orientation of this offset surface in the v parametric direction. The bounds of
  //! the surface are not changed but the given parametric direction is reversed.
  Standard_EXPORT void VReverse() final;

  //! Computes the v parameter on the modified
  //! surface, produced by reversing the or v
  //! parametric direction of this offset surface, for any
  //! point of v parameter V on this offset surface.
  Standard_EXPORT double VReversedParameter(const double V) const final;

  //! Returns the parametric bounds U1, U2, V1 and V2 of
  //! this offset surface.
  //! If the surface is infinite, this function can return:
  //! - double::RealFirst(), or
  //! - double::RealLast().
  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const final;

  //! This method returns the continuity of the basis surface - 1.
  //! Continuity of the Offset surface :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the Surface,
  //! C2 : continuity of the second derivative all along the Surface,
  //! C3 : continuity of the third derivative all along the Surface,
  //! CN : the order of continuity is infinite.
  //! Example :
  //! If the basis surface is C2 in the V direction and C3 in the U
  //! direction Shape = C1.
  //! Warnings :
  //! If the basis surface has a unique normal direction defined at
  //! any point this method gives the continuity of the offset
  //! surface otherwise the effective continuity can be lower than
  //! the continuity of the basis surface - 1.
  Standard_EXPORT GeomAbs_Shape Continuity() const final;

  //! This method answer True if the continuity of the basis surface
  //! is N + 1 in the U parametric direction. We suppose in this
  //! class that a unique normal is defined at any point on the basis
  //! surface.
  //! Raised if N <0.
  Standard_EXPORT bool IsCNu(const int N) const final;

  //! This method answer True if the continuity of the basis surface
  //! is N + 1 in the V parametric direction. We suppose in this
  //! class that a unique normal is defined at any point on the basis
  //! surface.
  //! Raised if N <0.
  Standard_EXPORT bool IsCNv(const int N) const final;

  //! Checks whether this offset surface is closed in the u
  //! parametric direction.
  //! Returns true if, taking uFirst and uLast as
  //! the parametric bounds in the u parametric direction,
  //! the distance between the points P(uFirst,v)
  //! and P(uLast,v) is less than or equal to
  //! gp::Resolution() for each value of the parameter v.
  Standard_EXPORT bool IsUClosed() const final;

  //! Checks whether this offset surface is closed in the u
  //! or v parametric direction. Returns true if taking vFirst and vLast as the
  //! parametric bounds in the v parametric direction, the
  //! distance between the points P(u,vFirst) and
  //! P(u,vLast) is less than or equal to
  //! gp::Resolution() for each value of the parameter u.
  Standard_EXPORT bool IsVClosed() const final;

  //! Returns true if this offset surface is periodic in the u
  //! parametric direction, i.e. if the basis
  //! surface of this offset surface is periodic in this direction.
  Standard_EXPORT bool IsUPeriodic() const final;

  //! Returns the period of this offset surface in the u
  //! parametric direction respectively, i.e. the period of the
  //! basis surface of this offset surface in this parametric direction.
  //! raises if the surface is not uperiodic.
  Standard_EXPORT double UPeriod() const final;

  //! Returns true if this offset surface is periodic in the v
  //! parametric direction, i.e. if the basis
  //! surface of this offset surface is periodic in this direction.
  Standard_EXPORT bool IsVPeriodic() const final;

  //! Returns the period of this offset surface in the v
  //! parametric direction respectively, i.e. the period of the
  //! basis surface of this offset surface in this parametric direction.
  //! raises if the surface is not vperiodic.
  Standard_EXPORT double VPeriod() const final;

  //! Computes the U isoparametric curve.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const final;

  //! Computes the V isoparametric curve.
  //!
  //! The following methods compute value and derivatives.
  //!
  //! Warnings
  //! An exception is raised if a unique normal vector is
  //! not defined on the basis surface for the parametric value (U,V).
  //! No check is done at the creation time and we suppose
  //! in this package that the offset surface can be defined at any point.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const final;

  //! @code
  //!   P (U, V) = Pbasis + Offset * Ndir
  //! @endcode
  //! where
  //! @code
  //!   Ndir = D1Ubasis ^ D1Vbasis / ||D1Ubasis ^ D1Vbasis||
  //! @endcode
  //! is the normal direction of the basis surface.
  //! Pbasis, D1Ubasis, D1Vbasis are the point and the first derivatives on the basis surface.
  //! If Ndir is undefined this method computes an approached normal
  //! direction using the following limited development:
  //! @code
  //! Ndir = N0 + DNdir/DU + DNdir/DV + Eps
  //! @endcode
  //! with Eps->0 which requires to compute the second derivatives on the basis surface.
  //! If the normal direction cannot be approximate for this order
  //! of derivation the exception UndefinedValue is raised.
  //!
  //! Raised if the continuity of the basis surface is not C1.
  //! Raised if the order of derivation required to compute the
  //! normal direction is greater than the second order.
  //! Returns std::nullopt on failure.
  Standard_EXPORT std::optional<gp_Pnt> EvalD0(const double U, const double V) const final;

  //! Computes the point and first partial derivatives at (U, V).
  //! Returns std::nullopt if the continuity of the basis surface is not C2.
  Standard_EXPORT std::optional<Geom_SurfD1> EvalD1(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 2nd order at (U, V).
  //! Returns std::nullopt if the continuity of the basis surface is not C3.
  Standard_EXPORT std::optional<Geom_SurfD2> EvalD2(const double U, const double V) const final;

  //! Computes the point and partial derivatives up to 3rd order at (U, V).
  //! Returns std::nullopt if the continuity of the basis surface is not C4.
  Standard_EXPORT std::optional<Geom_SurfD3> EvalD3(const double U, const double V) const final;

  //! Computes the derivative of order Nu in U and Nv in V at (U, V).
  //! Returns std::nullopt on failure.
  //!
  //! Raised if the continuity of the basis surface is not CNu + 1
  //! in the U direction and CNv + 1 in the V direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  //!
  //! Warnings:
  //! The exception UndefinedValue or UndefinedDerivative is
  //! raised if it is not possible to compute a unique offset direction.
  Standard_EXPORT std::optional<gp_Vec> EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const final;

  //! Applies the transformation T to this offset surface.
  //! Note: the basis surface is also modified.
  Standard_EXPORT void Transform(const gp_Trsf& T) final;

  //! Computes the parameters on the transformed surface for
  //! the transform of the point of parameters U,V on <me>.
  //! @code
  //!   me->Transformed(T)->Value(U',V')
  //! @endcode
  //! is the same point as
  //! @code
  //! me->Value(U,V).Transformed(T)
  //! @endcode
  //! Where U',V' are the new values of U,V after calling
  //! @code
  //! me->TransformParameters(U,V,T)
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
  //! Where U',V' are obtained by transforming U,V with the 2d transformation returned by
  //! @code
  //! me->ParametricTransformation(T)
  //! @endcode
  //! This method calls the basis surface method.
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const final;

  //! Creates a new object which is a copy of this offset surface.
  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const final;

  //! returns an equivalent surface of the offset surface
  //! when the basis surface is a canonic surface or a
  //! rectangular limited surface on canonic surface or if
  //! the offset is null.
  Standard_EXPORT occ::handle<Geom_Surface> Surface() const;

  //! if true, L is the local osculating surface
  //! along U at the point U,V. It means that DL/DU is
  //! collinear to DS/DU. If IsOpposite == true
  //! these vectors have opposite direction.
  Standard_EXPORT bool UOsculatingSurface(const double                      U,
                                          const double                      V,
                                          bool&                             IsOpposite,
                                          occ::handle<Geom_BSplineSurface>& UOsculSurf) const;

  //! if true, L is the local osculating surface
  //! along V at the point U,V.
  //! It means that DL/DV is collinear to DS/DV.
  //! If IsOpposite == true
  //! these vectors have opposite direction.
  Standard_EXPORT bool VOsculatingSurface(const double                      U,
                                          const double                      V,
                                          bool&                             IsOpposite,
                                          occ::handle<Geom_BSplineSurface>& VOsculSurf) const;

  //! Returns continuity of the basis surface.
  inline GeomAbs_Shape GetBasisSurfContinuity() const { return myBasisSurfContinuity; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const final;

  DEFINE_STANDARD_RTTIEXT(Geom_OffsetSurface, Geom_Surface)

private:
  occ::handle<Geom_Surface>               basisSurf;
  occ::handle<Geom_Surface>               equivSurf;
  double                                  offsetValue;
  std::unique_ptr<Geom_OsculatingSurface> myOscSurf;
  GeomAbs_Shape                           myBasisSurfContinuity;
};

#endif // _Geom_OffsetSurface_HeaderFile
