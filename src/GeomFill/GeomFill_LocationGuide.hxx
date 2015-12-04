// Created on: 1998-07-08
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

#ifndef _GeomFill_LocationGuide_HeaderFile
#define _GeomFill_LocationGuide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray2OfPnt2d.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <gp_Mat.hxx>
#include <math_Vector.hxx>
#include <GeomFill_PipeError.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
class GeomFill_TrihedronWithGuide;
class GeomFill_SectionLaw;
class Adaptor3d_HCurve;
class Standard_NotImplemented;
class Standard_ConstructionError;
class Standard_OutOfRange;
class gp_Mat;
class GeomFill_LocationLaw;
class gp_Vec;
class gp_Pnt;
class Geom_Curve;


class GeomFill_LocationGuide;
DEFINE_STANDARD_HANDLE(GeomFill_LocationGuide, GeomFill_LocationLaw)


class GeomFill_LocationGuide : public GeomFill_LocationLaw
{

public:

  
  Standard_EXPORT GeomFill_LocationGuide(const Handle(GeomFill_TrihedronWithGuide)& Triedre);
  
  Standard_EXPORT void Set (const Handle(GeomFill_SectionLaw)& Section, const Standard_Boolean rotat, const Standard_Real SFirst, const Standard_Real SLast, const Standard_Real PrecAngle, Standard_Real& LastAngle);
  
  Standard_EXPORT void EraseRotation();
  
  Standard_EXPORT virtual void SetCurve (const Handle(Adaptor3d_HCurve)& C) Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Adaptor3d_HCurve)& GetCurve() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetTrsf (const gp_Mat& Transfo) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(GeomFill_LocationLaw) Copy() const Standard_OVERRIDE;
  
  //! compute Location
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V) Standard_OVERRIDE;
  
  //! compute Location and 2d points
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, TColgp_Array1OfPnt2d& Poles2d) Standard_OVERRIDE;
  
  //! compute location 2d  points and  associated
  //! first derivatives.
  //! Warning : It used only for C1 or C2 aproximation
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d) Standard_OVERRIDE;
  
  //! compute location 2d  points and associated
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 aproximation
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, gp_Mat& M, gp_Vec& V, gp_Mat& DM, gp_Vec& DV, gp_Mat& D2M, gp_Vec& D2V, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d) Standard_OVERRIDE;
  
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
  
  //! Give the number of trace (Curves 2d wich are not restriction)
  //! Returns 1 (default implementation)
  Standard_EXPORT virtual Standard_Integer TraceNumber() const Standard_OVERRIDE;
  
  //! Give a status to the Law
  //! Returns PipeOk (default implementation)
  Standard_EXPORT virtual GeomFill_PipeError ErrorStatus() const Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Sets the bounds of the parametric interval on
  //! the function
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT virtual void GetInterval (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  //! Gets the bounds of the function parametric domain.
  //! Warning: This domain it is  not modified by the
  //! SetValue method
  Standard_EXPORT virtual void GetDomain (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  //! Is usefull, if (me) have to run numerical
  //! algorithm to perform D0, D1 or D2
  //! The default implementation make nothing.
  Standard_EXPORT virtual void SetTolerance (const Standard_Real Tol3d, const Standard_Real Tol2d) Standard_OVERRIDE;
  
  //! Returns the resolutions in the  sub-space 2d <Index>
  //! This information is usfull to find an good tolerance in
  //! 2d approximation.
  //! Warning: Used only if Nb2dCurve > 0
  Standard_EXPORT virtual void Resolution (const Standard_Integer Index, const Standard_Real Tol, Standard_Real& TolU, Standard_Real& TolV) const Standard_OVERRIDE;
  
  //! Get the maximum Norm  of the matrix-location part.  It
  //! is usful to find an good Tolerance to approx M(t).
  Standard_EXPORT virtual Standard_Real GetMaximalNorm() Standard_OVERRIDE;
  
  //! Get average value of M(t) and V(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Mat& AM, gp_Vec& AV) Standard_OVERRIDE;
  
  //! Say if the Location  Law, is an translation of  Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsTranslation (Standard_Real& Error) const Standard_OVERRIDE;
  
  //! Say if the Location  Law, is a rotation of Location
  //! The default implementation is " returns False ".
  Standard_EXPORT virtual Standard_Boolean IsRotation (Standard_Real& Error) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Rotation (gp_Pnt& Center) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom_Curve) Section() const;
  
  Standard_EXPORT Handle(Adaptor3d_HCurve) Guide() const;
  
  Standard_EXPORT void SetOrigine (const Standard_Real Param1, const Standard_Real Param2);
  
  Standard_EXPORT GeomFill_PipeError ComputeAutomaticLaw (Handle(TColgp_HArray1OfPnt2d)& ParAndRad) const;




  DEFINE_STANDARD_RTTIEXT(GeomFill_LocationGuide,GeomFill_LocationLaw)

protected:


  Handle(TColgp_HArray2OfPnt2d) myPoles2d;


private:

  
  Standard_EXPORT void SetRotation (const Standard_Real PrecAngle, Standard_Real& LastAngle);
  
  Standard_EXPORT void InitX (const Standard_Real Param) const;

  Handle(GeomFill_TrihedronWithGuide) myLaw;
  Handle(GeomFill_SectionLaw) mySec;
  Handle(Adaptor3d_HCurve) myCurve;
  Handle(Adaptor3d_HCurve) myGuide;
  Handle(Adaptor3d_HCurve) myTrimmed;
  Standard_Integer myNbPts;
  Standard_Boolean rotation;
  Standard_Real OrigParam1;
  Standard_Real OrigParam2;
  Standard_Real Uf;
  Standard_Real Ul;
  Standard_Real myFirstS;
  Standard_Real myLastS;
  Standard_Real ratio;
  Standard_Boolean WithTrans;
  gp_Mat Trans;
  math_Vector TolRes;
  math_Vector Inf;
  math_Vector Sup;
  math_Vector X;
  math_Vector R;
  GeomFill_PipeError myStatus;


};







#endif // _GeomFill_LocationGuide_HeaderFile
