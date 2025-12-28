// Created on: 1998-04-21
// Created by: Stephanie HUMEAU
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

#ifndef _GeomFill_LocationDraft_HeaderFile
#define _GeomFill_LocationDraft_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Mat.hxx>
#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Boolean.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <gp_Vec2d.hxx>
#include <GeomAbs_Shape.hxx>

class GeomFill_DraftTrihedron;

class GeomFill_LocationDraft : public GeomFill_LocationLaw
{

public:
  Standard_EXPORT GeomFill_LocationDraft(const gp_Dir& Direction, const double Angle);

  Standard_EXPORT void SetStopSurf(const occ::handle<Adaptor3d_Surface>& Surf);

  Standard_EXPORT void SetAngle(const double Angle);

  //! calculation of poles on locking surfaces (the intersection between the generatrixand the
  //! surface at the cross - section points myNbPts)
  //! @return true in case if execution end correctly
  Standard_EXPORT bool SetCurve(const occ::handle<Adaptor3d_Curve>& C) override;

  Standard_EXPORT const occ::handle<Adaptor3d_Curve>& GetCurve() const override;

  Standard_EXPORT void SetTrsf(const gp_Mat& Transfo) override;

  Standard_EXPORT occ::handle<GeomFill_LocationLaw> Copy() const override;

  //! compute Location
  Standard_EXPORT bool D0(const double Param, gp_Mat& M, gp_Vec& V) override;

  //! compute Location and 2d points
  Standard_EXPORT bool D0(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d) override;

  //! compute location 2d points and associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT bool D1(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  gp_Mat&                       DM,
                                  gp_Vec&                       DV,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d,
                                  NCollection_Array1<gp_Vec2d>& DPoles2d) override;

  //! compute location 2d points and associated
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT bool D2(const double                  Param,
                                  gp_Mat&                       M,
                                  gp_Vec&                       V,
                                  gp_Mat&                       DM,
                                  gp_Vec&                       DV,
                                  gp_Mat&                       D2M,
                                  gp_Vec&                       D2V,
                                  NCollection_Array1<gp_Pnt2d>& Poles2d,
                                  NCollection_Array1<gp_Vec2d>& DPoles2d,
                                  NCollection_Array1<gp_Vec2d>& D2Poles2d) override;

  //! Say if the first restriction is defined in this class.
  //! If it is true the first element of poles array in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns false (default implementation)
  Standard_EXPORT bool HasFirstRestriction() const override;

  //! Say if the last restriction is defined in this class.
  //! If it is true the last element of poles array in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns false (default implementation)
  Standard_EXPORT bool HasLastRestriction() const override;

  //! Give the number of trace (Curves 2d which are not restriction)
  //! Returns 1 (default implementation)
  Standard_EXPORT int TraceNumber() const override;

  //! Returnsthe number of intervals for continuity <S>.
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

  //! Returns the resolutions in the sub-space 2d <Index>
  //! This information is useful to find a good tolerance in
  //! 2d approximation.
  //! Warning: Used only if Nb2dCurve > 0
  Standard_EXPORT void Resolution(const int    Index,
                                          const double Tol,
                                          double&      TolU,
                                          double&      TolV) const override;

  //! Get the maximum Norm of the matrix-location part. It
  //! is usful to find a good Tolerance to approx M(t).
  Standard_EXPORT double GetMaximalNorm() override;

  //! Get average value of M(t) and V(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT void GetAverageLaw(gp_Mat& AM, gp_Vec& AV) override;

  //! Say if the Location Law, is an translation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT bool IsTranslation(double& Error) const override;

  //! Say if the Location Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT bool IsRotation(double& Error) const override;

  Standard_EXPORT void Rotation(gp_Pnt& Center) const override;

  //! Say if the generatrice interset the surface
  Standard_EXPORT bool IsIntersec() const;

  Standard_EXPORT gp_Dir Direction() const;

  DEFINE_STANDARD_RTTIEXT(GeomFill_LocationDraft, GeomFill_LocationLaw)

protected:
  occ::handle<NCollection_HArray1<gp_Pnt2d>> myPoles2d;

private:
  Standard_EXPORT void Prepare();

  gp_Mat                               Trans;
  occ::handle<GeomFill_DraftTrihedron> myLaw;
  occ::handle<Adaptor3d_Surface>       mySurf;
  occ::handle<Adaptor3d_Curve>         myCurve;
  occ::handle<Adaptor3d_Curve>         myTrimmed;
  gp_Dir                               myDir;
  double                               myAngle;
  int                                  myNbPts;
  bool                                 Intersec;
  bool                                 WithTrans;
};

#endif // _GeomFill_LocationDraft_HeaderFile
