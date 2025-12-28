// Created on: 1997-11-20
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomFill_SectionLaw_HeaderFile
#define _GeomFill_SectionLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_BSplineSurface;
class gp_Pnt;
class Geom_Curve;

//! To define section law in sweeping
class GeomFill_SectionLaw : public Standard_Transient
{

public:
  //! compute the section for v = param
  Standard_EXPORT virtual bool D0(const double                Param,
                                  NCollection_Array1<gp_Pnt>& Poles,
                                  NCollection_Array1<double>& Weigths) = 0;

  //! compute the first derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double                Param,
                                  NCollection_Array1<gp_Pnt>& Poles,
                                  NCollection_Array1<gp_Vec>& DPoles,
                                  NCollection_Array1<double>& Weigths,
                                  NCollection_Array1<double>& DWeigths);

  //! compute the second derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual bool D2(const double                Param,
                                  NCollection_Array1<gp_Pnt>& Poles,
                                  NCollection_Array1<gp_Vec>& DPoles,
                                  NCollection_Array1<gp_Vec>& D2Poles,
                                  NCollection_Array1<double>& Weigths,
                                  NCollection_Array1<double>& DWeigths,
                                  NCollection_Array1<double>& D2Weigths);

  //! give if possible an bspline Surface, like iso-v are the
  //! section. If it is not possible this method have to
  //! get an Null Surface. It is the default implementation.
  Standard_EXPORT virtual occ::handle<Geom_BSplineSurface> BSplineSurface() const;

  //! get the format of an section
  Standard_EXPORT virtual void SectionShape(int& NbPoles, int& NbKnots, int& Degree) const = 0;

  //! get the Knots of the section
  Standard_EXPORT virtual void Knots(NCollection_Array1<double>& TKnots) const = 0;

  //! get the Multplicities of the section
  Standard_EXPORT virtual void Mults(NCollection_Array1<int>& TMults) const = 0;

  //! Returns if the sections are rational or not
  Standard_EXPORT virtual bool IsRational() const = 0;

  //! Returns if the sections are periodic or not
  Standard_EXPORT virtual bool IsUPeriodic() const = 0;

  //! Returns if law is periodic or not
  Standard_EXPORT virtual bool IsVPeriodic() const = 0;

  //! Returns the number of intervals for continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const = 0;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape         S) const = 0;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const double First, const double Last) = 0;

  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval(double& First, double& Last) const = 0;

  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain(double& First, double& Last) const = 0;

  //! Returns the tolerances associated at each poles to
  //! reach in approximation, to satisfy: BoundTol error
  //! at the Boundary AngleTol tangent error at the
  //! Boundary (in radian) SurfTol error inside the
  //! surface.
  Standard_EXPORT virtual void GetTolerance(const double                BoundTol,
                                            const double                SurfTol,
                                            const double                AngleTol,
                                            NCollection_Array1<double>& Tol3d) const = 0;

  //! Is useful, if <me> has to run numerical
  //! algorithm to perform D0, D1 or D2
  //! The default implementation make nothing.
  Standard_EXPORT virtual void SetTolerance(const double Tol3d, const double Tol2d);

  //! Get the barycentre of Surface.
  //! A very poor estimation is sufficient.
  //! This information is useful to perform well
  //! conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual gp_Pnt BarycentreOfSurf() const;

  //! Returns the length of the greater section. This
  //! information is useful to G1's control.
  //! Warning: With an little value, approximation can be slower.
  Standard_EXPORT virtual double MaximalSection() const = 0;

  //! Compute the minimal value of weight for each poles
  //! in all sections.
  //! This information is useful to control error
  //! in rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT virtual void GetMinimalWeight(NCollection_Array1<double>& Weigths) const;

  //! Say if all sections are equals
  Standard_EXPORT virtual bool IsConstant(double& Error) const;

  //! Return a copy of the constant Section, if <me>
  //! IsConstant
  Standard_EXPORT virtual occ::handle<Geom_Curve> ConstantSection() const;

  //! Returns True if all section are circle, with same
  //! plane,same center and linear radius evolution
  //! Return False by Default.
  Standard_EXPORT virtual bool IsConicalLaw(double& Error) const;

  //! Return the circle section at parameter <Param>, if
  //! <me> a IsConicalLaw
  Standard_EXPORT virtual occ::handle<Geom_Curve> CirclSection(const double Param) const;

  DEFINE_STANDARD_RTTIEXT(GeomFill_SectionLaw, Standard_Transient)
};

#endif // _GeomFill_SectionLaw_HeaderFile
