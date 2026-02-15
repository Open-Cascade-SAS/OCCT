// Created on: 1996-11-21
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _GeomPlate_Surface_HeaderFile
#define _GeomPlate_Surface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Plate_Plate.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_XY.hxx>
#include <NCollection_Sequence.hxx>
class gp_Trsf;
class gp_GTrsf2d;
class Geom_Curve;
class Geom_Geometry;

//! Describes the characteristics of plate surface objects
//! returned by BuildPlateSurface::Surface. These can be
//! used to verify the quality of the resulting surface before
//! approximating it to a Geom_BSpline surface generated
//! by MakeApprox. This proves necessary in cases where
//! you want to use the resulting surface as the support for
//! a shape. The algorithmically generated surface cannot
//! fill this function as is, and as a result must be converted first.
class GeomPlate_Surface : public Geom_Surface
{

public:
  Standard_EXPORT GeomPlate_Surface(const occ::handle<Geom_Surface>& Surfinit,
                                    const Plate_Plate&               Surfinter);

  //! Reverses the U direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT void UReverse() override;

  //! Return the parameter on the Ureversed surface for
  //! the point of parameter U on <me>.
  //! @code
  //!   me->UReversed()->Value(me->UReversedParameter(U),V)
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V)
  //! @endcode
  Standard_EXPORT double UReversedParameter(const double U) const override;

  //! Reverses the V direction of parametrization of <me>.
  //! The bounds of the surface are not modified.
  Standard_EXPORT void VReverse() override;

  //! Return the parameter on the Vreversed surface for
  //! the point of parameter V on <me>.
  //! @code
  //!   me->VReversed()->Value(U,me->VReversedParameter(V))
  //! @endcode
  //! is the same point as
  //! @code
  //!   me->Value(U,V)
  //! @endcode
  Standard_EXPORT double VReversedParameter(const double V) const override;

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
  //! This methods does not change <U> and <V>
  //!
  //! It can be redefined. For example on the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT void TransformParameters(double& U, double& V, const gp_Trsf& T) const override;

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
  //! This method returns an identity transformation
  //!
  //! It can be redefined. For example on the Plane,
  //! Cylinder, Cone, Revolved and Extruded surfaces.
  Standard_EXPORT gp_GTrsf2d ParametricTransformation(const gp_Trsf& T) const override;

  Standard_EXPORT void Bounds(double& U1, double& U2, double& V1, double& V2) const override;

  //! Is the surface closed in the parametric direction U ?
  //! Returns True if for each parameter V the distance
  //! between the point P (UFirst, V) and P (ULast, V) is
  //! lower or equal to Resolution from gp. UFirst and ULast
  //! are the parametric bounds in the U direction.
  Standard_EXPORT bool IsUClosed() const override;

  //! Is the surface closed in the parametric direction V ?
  //! Returns True if for each parameter U the distance
  //! between the point P (U, VFirst) and P (U, VLast) is
  //! lower or equal to Resolution from gp. VFirst and VLast
  //! are the parametric bounds in the V direction.
  Standard_EXPORT bool IsVClosed() const override;

  //! Is the parametrization of a surface periodic in the
  //! direction U ?
  //! It is possible only if the surface is closed in this
  //! parametric direction and if the following relation is
  //! satisfied :
  //! for each parameter V the distance between the point
  //! P (U, V) and the point P (U + T, V) is lower or equal
  //! to Resolution from package gp. T is the parametric period
  //! and must be a constant.
  Standard_EXPORT bool IsUPeriodic() const override;

  //! returns the Uperiod.
  //! raises if the surface is not uperiodic.
  Standard_EXPORT double UPeriod() const override;

  //! Is the parametrization of a surface periodic in the
  //! direction U ?
  //! It is possible only if the surface is closed in this
  //! parametric direction and if the following relation is
  //! satisfied :
  //! for each parameter V the distance between the point
  //! P (U, V) and the point P (U + T, V) is lower or equal
  //! to Resolution from package gp. T is the parametric period
  //! and must be a constant.
  Standard_EXPORT bool IsVPeriodic() const override;

  //! returns the Vperiod.
  //! raises if the surface is not vperiodic.
  Standard_EXPORT double VPeriod() const override;

  //! Computes the U isoparametric curve.
  Standard_EXPORT occ::handle<Geom_Curve> UIso(const double U) const override;

  //! Computes the V isoparametric curve.
  Standard_EXPORT occ::handle<Geom_Curve> VIso(const double V) const override;

  //! Global Continuity of the surface in direction U and V :
  //! C0 : only geometric continuity,
  //! C1 : continuity of the first derivative all along the surface,
  //! C2 : continuity of the second derivative all along the surface,
  //! C3 : continuity of the third derivative all along the surface,
  //! G1 : tangency continuity all along the surface,
  //! G2 : curvature continuity all along the surface,
  //! CN : the order of continuity is infinite.
  //! Example :
  //! If the surface is C1 in the V parametric direction and C2
  //! in the U parametric direction Shape = C1.
  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the order of continuity of the surface in the
  //! U parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCNu(const int N) const override;

  //! Returns the order of continuity of the surface in the
  //! V parametric direction.
  //! Raised if N < 0.
  Standard_EXPORT bool IsCNv(const int N) const override;

  //! Computes the point of parameter U,V on the surface.
  //!
  //! Raised only for an "OffsetSurface" if it is not possible to
  //! compute the current point.
  Standard_EXPORT gp_Pnt EvalD0(const double U, const double V) const override;

  //! Computes the point P and the first derivatives in the
  //! directions U and V at this point.
  //! Raised if the continuity of the surface is not C1.
  Standard_EXPORT Geom_Surface::ResD1 EvalD1(const double U,
                                                            const double V) const override;

  //! Computes the point P, the first and the second derivatives in
  //! the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT Geom_Surface::ResD2 EvalD2(const double U,
                                                            const double V) const override;

  //! Computes the point P, the first,the second and the third
  //! derivatives in the directions U and V at this point.
  //! Raised if the continuity of the surface is not C2.
  Standard_EXPORT Geom_Surface::ResD3 EvalD3(const double U,
                                                            const double V) const override;

  //! ---Purpose ;
  //! Computes the derivative of order Nu in the direction U and Nv
  //! in the direction V at the point P(U, V).
  //!
  //! Raised if the continuity of the surface is not CNu in the U
  //! direction or not CNv in the V direction.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  Standard_EXPORT gp_Vec EvalDN(const double U,
                                               const double V,
                                               const int    Nu,
                                               const int    Nv) const override;

  Standard_EXPORT occ::handle<Geom_Geometry> Copy() const override;

  //! Transformation of a geometric object. This transformation
  //! can be a translation, a rotation, a symmetry, a scaling
  //! or a complex transformation obtained by combination of
  //! the previous elementaries transformations.
  //! (see class Transformation of the package Geom).
  Standard_EXPORT void Transform(const gp_Trsf& T) override;

  Standard_EXPORT occ::handle<Geom_Surface> CallSurfinit() const;

  Standard_EXPORT void SetBounds(const double Umin,
                                 const double Umax,
                                 const double Vmin,
                                 const double Vmax);

  Standard_EXPORT void RealBounds(double& U1, double& U2, double& V1, double& V2) const;

  Standard_EXPORT void Constraints(NCollection_Sequence<gp_XY>& Seq) const;

  DEFINE_STANDARD_RTTIEXT(GeomPlate_Surface, Geom_Surface)

private:
  Plate_Plate               mySurfinter;
  occ::handle<Geom_Surface> mySurfinit;
  double                    myUmin;
  double                    myUmax;
  double                    myVmin;
  double                    myVmax;
};

#endif // _GeomPlate_Surface_HeaderFile
