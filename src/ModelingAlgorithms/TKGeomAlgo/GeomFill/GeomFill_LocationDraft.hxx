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
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>

class GeomFill_DraftTrihedron;

DEFINE_STANDARD_HANDLE(GeomFill_LocationDraft, GeomFill_LocationLaw)

class GeomFill_LocationDraft : public GeomFill_LocationLaw
{

public:
  Standard_EXPORT GeomFill_LocationDraft(const gp_Dir& Direction, const Standard_Real Angle);

  Standard_EXPORT void SetStopSurf(const Handle(Adaptor3d_Surface)& Surf);

  Standard_EXPORT void SetAngle(const Standard_Real Angle);

  //! calculation of poles on locking surfaces (the intersection between the generatrixand the
  //! surface at the cross - section points myNbPts)
  //! @return Standard_True in case if execution end correctly
  Standard_EXPORT virtual Standard_Boolean SetCurve(const Handle(Adaptor3d_Curve)& C)
    Standard_OVERRIDE;

  Standard_EXPORT virtual const Handle(Adaptor3d_Curve)& GetCurve() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetTrsf(const gp_Mat& Transfo) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(GeomFill_LocationLaw) Copy() const Standard_OVERRIDE;

  //! compute Location
  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real Param,
                                              gp_Mat&             M,
                                              gp_Vec&             V) Standard_OVERRIDE;

  //! compute Location and 2d points
  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real   Param,
                                              gp_Mat&               M,
                                              gp_Vec&               V,
                                              TColgp_Array1OfPnt2d& Poles2d) Standard_OVERRIDE;

  //! compute location 2d  points and  associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real   Param,
                                              gp_Mat&               M,
                                              gp_Vec&               V,
                                              gp_Mat&               DM,
                                              gp_Vec&               DV,
                                              TColgp_Array1OfPnt2d& Poles2d,
                                              TColgp_Array1OfVec2d& DPoles2d) Standard_OVERRIDE;

  //! compute location 2d  points and associated
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2(const Standard_Real   Param,
                                              gp_Mat&               M,
                                              gp_Vec&               V,
                                              gp_Mat&               DM,
                                              gp_Vec&               DV,
                                              gp_Mat&               D2M,
                                              gp_Vec&               D2V,
                                              TColgp_Array1OfPnt2d& Poles2d,
                                              TColgp_Array1OfVec2d& DPoles2d,
                                              TColgp_Array1OfVec2d& D2Poles2d) Standard_OVERRIDE;

  //! Say if the first restriction is defined in this class.
  //! If it  is true the  first element  of poles array   in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns Standard_False (default implementation)
  Standard_EXPORT virtual Standard_Boolean HasFirstRestriction() const Standard_OVERRIDE;

  //! Say if the last restriction is defined in this class.
  //! If it is  true the  last element  of poles array in
  //! D0,D1,D2... Correspond to this restriction.
  //! Returns Standard_False (default implementation)
  Standard_EXPORT virtual Standard_Boolean HasLastRestriction() const Standard_OVERRIDE;

  //! Give the number of trace (Curves 2d which are not restriction)
  //! Returns 1 (default implementation)
  Standard_EXPORT virtual Standard_Integer TraceNumber() const Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals(const GeomAbs_Shape S) const
    Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(TColStd_Array1OfReal& T,
                                         const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval(const Standard_Real First,
                                           const Standard_Real Last) Standard_OVERRIDE;

  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval(Standard_Real& First,
                                           Standard_Real& Last) const Standard_OVERRIDE;

  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is  not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain(Standard_Real& First,
                                         Standard_Real& Last) const Standard_OVERRIDE;

  //! Returns the resolutions in the  sub-space 2d <Index>
  //! This information is usfull to find an good tolerance in
  //! 2d approximation.
  //! Warning: Used only if Nb2dCurve > 0
  Standard_EXPORT virtual void Resolution(const Standard_Integer Index,
                                          const Standard_Real    Tol,
                                          Standard_Real&         TolU,
                                          Standard_Real&         TolV) const Standard_OVERRIDE;

  //! Get the maximum Norm  of the matrix-location part.  It
  //! is usful to find an good Tolerance to approx M(t).
  Standard_EXPORT virtual Standard_Real GetMaximalNorm() Standard_OVERRIDE;

  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Mat& AM, gp_Vec& AV) Standard_OVERRIDE;

  //! Say if the Location  Law, is an translation of  Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsTranslation(Standard_Real& Error) const
    Standard_OVERRIDE;

  //! Say if the Location  Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsRotation(Standard_Real& Error) const Standard_OVERRIDE;

  Standard_EXPORT virtual void Rotation(gp_Pnt& Center) const Standard_OVERRIDE;

  //! Say if the generatrice interset the surface
  Standard_EXPORT Standard_Boolean IsIntersec() const;

  Standard_EXPORT gp_Dir Direction() const;

  DEFINE_STANDARD_RTTIEXT(GeomFill_LocationDraft, GeomFill_LocationLaw)

protected:
  Handle(TColgp_HArray1OfPnt2d) myPoles2d;

private:
  Standard_EXPORT void Prepare();

  gp_Mat                          Trans;
  Handle(GeomFill_DraftTrihedron) myLaw;
  Handle(Adaptor3d_Surface)       mySurf;
  Handle(Adaptor3d_Curve)         myCurve;
  Handle(Adaptor3d_Curve)         myTrimmed;
  gp_Dir                          myDir;
  Standard_Real                   myAngle;
  Standard_Integer                myNbPts;
  Standard_Boolean                Intersec;
  Standard_Boolean                WithTrans;
};

#endif // _GeomFill_LocationDraft_HeaderFile
