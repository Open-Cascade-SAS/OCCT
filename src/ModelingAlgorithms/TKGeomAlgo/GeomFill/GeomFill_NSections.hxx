// Created on: 1998-12-14
// Created by: Joelle CHAUVET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _GeomFill_NSections_HeaderFile
#define _GeomFill_NSections_HeaderFile

#include <Standard.hxx>

#include <Standard_Real.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Trsf.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Vec.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class Geom_BSplineSurface;
class gp_Pnt;
class Geom_Curve;

//! Define a Section Law by N Sections
class GeomFill_NSections : public GeomFill_SectionLaw
{

public:
  //! Make a SectionLaw with N Curves.
  Standard_EXPORT GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC);

  //! Make a SectionLaw with N Curves and N associated parameters.
  Standard_EXPORT GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                     const NCollection_Sequence<double>&                  NP);

  //! Make a SectionLaw with N Curves and N associated parameters.
  //! UF and UL are the parametric bounds of the NSections
  Standard_EXPORT GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                     const NCollection_Sequence<double>&                  NP,
                                     const double                                         UF,
                                     const double                                         UL);

  //! Make a SectionLaw with N Curves and N associated parameters.
  //! UF and UL are the parametric bounds of the NSections
  //! VF and VL are the parametric bounds of the path
  Standard_EXPORT GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                     const NCollection_Sequence<double>&                  NP,
                                     const double                                         UF,
                                     const double                                         UL,
                                     const double                                         VF,
                                     const double                                         VL);

  //! Make a SectionLaw with N Curves and N associated parameters.
  //! UF and UL are the parametric bounds of the NSections
  //! VF and VL are the parametric bounds of the path
  //! UF and UL are the parametric bounds of the NSections
  //! Surf is a reference surface used by BRepFill_NSections
  Standard_EXPORT GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                     const NCollection_Sequence<gp_Trsf>&                 Trsfs,
                                     const NCollection_Sequence<double>&                  NP,
                                     const double                                         UF,
                                     const double                                         UL,
                                     const double                                         VF,
                                     const double                                         VL,
                                     const occ::handle<Geom_BSplineSurface>&              Surf);

  //! compute the section for v = param
  Standard_EXPORT bool D0(const double                Param,
                          NCollection_Array1<gp_Pnt>& Poles,
                          NCollection_Array1<double>& Weigths) override;

  //! compute the first derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT bool D1(const double                Param,
                          NCollection_Array1<gp_Pnt>& Poles,
                          NCollection_Array1<gp_Vec>& DPoles,
                          NCollection_Array1<double>& Weigths,
                          NCollection_Array1<double>& DWeigths) override;

  //! compute the second derivative in v direction of the
  //! section for v = param
  //! Warning : It used only for C2 approximation
  Standard_EXPORT bool D2(const double                Param,
                          NCollection_Array1<gp_Pnt>& Poles,
                          NCollection_Array1<gp_Vec>& DPoles,
                          NCollection_Array1<gp_Vec>& D2Poles,
                          NCollection_Array1<double>& Weigths,
                          NCollection_Array1<double>& DWeigths,
                          NCollection_Array1<double>& D2Weigths) override;

  //! Sets the reference surface
  Standard_EXPORT void SetSurface(const occ::handle<Geom_BSplineSurface>& RefSurf);

  //! Computes the surface
  Standard_EXPORT void ComputeSurface();

  //! give if possible an bspline Surface, like iso-v are the
  //! section. If it is not possible this methode have to
  //! get an Null Surface. Is it the default implementation.
  Standard_EXPORT occ::handle<Geom_BSplineSurface> BSplineSurface() const override;

  //! get the format of an section
  Standard_EXPORT void SectionShape(int& NbPoles, int& NbKnots, int& Degree) const override;

  //! get the Knots of the section
  Standard_EXPORT void Knots(NCollection_Array1<double>& TKnots) const override;

  //! get the Multplicities of the section
  Standard_EXPORT void Mults(NCollection_Array1<int>& TMults) const override;

  //! Returns if the sections are rational or not
  Standard_EXPORT bool IsRational() const override;

  //! Returns if the sections are periodic or not
  Standard_EXPORT bool IsUPeriodic() const override;

  //! Returns if the law isperiodic or not
  Standard_EXPORT bool IsVPeriodic() const override;

  //! Returns the number of intervals for continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT void SetInterval(const double First, const double Last) override;

  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT void GetInterval(double& First, double& Last) const override;

  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is not modified by the
  //! SetValue method
  Standard_EXPORT void GetDomain(double& First, double& Last) const override;

  //! Returns the tolerances associated at each poles to
  //! reach in approximation, to satisfy: BoundTol error
  //! at the Boundary AngleTol tangent error at the
  //! Boundary (in radian) SurfTol error inside the
  //! surface.
  Standard_EXPORT void GetTolerance(const double                BoundTol,
                                    const double                SurfTol,
                                    const double                AngleTol,
                                    NCollection_Array1<double>& Tol3d) const override;

  //! Get the barycentre of Surface.
  //! An very poor estimation is sufficient.
  //! This information is useful to perform well
  //! conditioned rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT gp_Pnt BarycentreOfSurf() const override;

  //! Returns the length of the greater section. This
  //! information is useful to G1's control.
  //! Warning: With an little value, approximation can be slower.
  Standard_EXPORT double MaximalSection() const override;

  //! Compute the minimal value of weight for each poles
  //! in all sections.
  //! This information is useful to control error
  //! in rational approximation.
  //! Warning: Used only if <me> IsRational
  Standard_EXPORT void GetMinimalWeight(NCollection_Array1<double>& Weigths) const override;

  //! return True If the Law isConstant
  Standard_EXPORT bool IsConstant(double& Error) const override;

  //! Return the constant Section if <me> IsConstant.
  Standard_EXPORT occ::handle<Geom_Curve> ConstantSection() const override;

  //! Returns True if all section are circle, with same
  //! plane,same center and linear radius evolution
  //! Return False by Default.
  Standard_EXPORT bool IsConicalLaw(double& Error) const override;

  //! Return the circle section at parameter <Param>, if
  //! <me> a IsConicalLaw
  Standard_EXPORT occ::handle<Geom_Curve> CirclSection(const double Param) const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_NSections, GeomFill_SectionLaw)

private:
  double                                        UFirst;
  double                                        ULast;
  double                                        VFirst;
  double                                        VLast;
  NCollection_Sequence<occ::handle<Geom_Curve>> mySections;
  NCollection_Sequence<gp_Trsf>                 myTrsfs;
  NCollection_Sequence<double>                  myParams;
  occ::handle<Geom_BSplineSurface>              mySurface;
  occ::handle<Geom_BSplineSurface>              myRefSurf;
};

#endif // _GeomFill_NSections_HeaderFile
