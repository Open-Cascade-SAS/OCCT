// Created on: 1997-12-19
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

#ifndef _GeomFill_Frenet_HeaderFile
#define _GeomFill_Frenet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class gp_Vec;


class GeomFill_Frenet;
DEFINE_STANDARD_HANDLE(GeomFill_Frenet, GeomFill_TrihedronLaw)

//! Defined Frenet Trihedron  Law
class GeomFill_Frenet : public GeomFill_TrihedronLaw
{

public:

  
  Standard_EXPORT GeomFill_Frenet();
  
  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT void Init();
  
  Standard_EXPORT virtual void SetCurve (const Handle(Adaptor3d_Curve)& C) Standard_OVERRIDE;
  
  //! compute Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual Standard_Boolean D0 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& Normal, gp_Vec& BiNormal) Standard_OVERRIDE;
  
  //! compute Triedrhon and  derivative Trihedron  on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& BiNormal, gp_Vec& DBiNormal) Standard_OVERRIDE;
  
  //! compute  Trihedron on curve
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2 (const Standard_Real Param, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& D2Tangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& D2Normal, gp_Vec& BiNormal, gp_Vec& DBiNormal, gp_Vec& D2BiNormal) Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Get average value of Tangent(t) and Normal(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal) Standard_OVERRIDE;
  
  //! Say if the law is Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;
  
  //! Return True.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_Frenet,GeomFill_TrihedronLaw)

protected:




private:

  
  Standard_EXPORT Standard_Boolean IsSingular (const Standard_Real U, Standard_Integer& Index) const;
  
  Standard_EXPORT Standard_Boolean DoSingular (const Standard_Real U, const Standard_Integer Index, gp_Vec& Tangent, gp_Vec& BiNormal, Standard_Integer& n, Standard_Integer& k, Standard_Integer& TFlag, Standard_Integer& BNFlag, Standard_Real& Delta);
  
  //! computes Triedrhon on curve at parameter <Param>
  Standard_EXPORT Standard_Boolean SingularD0 (const Standard_Real Param, const Standard_Integer Index, gp_Vec& Tangent, gp_Vec& Normal, gp_Vec& BiNormal, Standard_Real& Delta);
  
  //! computes Triedrhon and  derivative Trihedron  on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT Standard_Boolean SingularD1 (const Standard_Real Param, const Standard_Integer Index, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& BiNormal, gp_Vec& DBiNormal, Standard_Real& Delta);
  
  //! computes  Trihedron on curve
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT Standard_Boolean SingularD2 (const Standard_Real Param, const Standard_Integer Index, gp_Vec& Tangent, gp_Vec& DTangent, gp_Vec& D2Tangent, gp_Vec& Normal, gp_Vec& DNormal, gp_Vec& D2Normal, gp_Vec& BiNormal, gp_Vec& DBiNormal, gp_Vec& D2BiNormal, Standard_Real& Delta);
  
  //! revolves the trihedron (which is determined
  //! of given "Tangent", "Normal" and "BiNormal" vectors)
  //! to coincide "Tangent" and "NewTangent" axes.
  Standard_EXPORT Standard_Boolean RotateTrihedron (gp_Vec& Tangent, gp_Vec& Normal, gp_Vec& BiNormal, const gp_Vec& NewTangent) const;

  gp_Pnt P;
  Handle(TColStd_HArray1OfReal) mySngl;
  Handle(TColStd_HArray1OfReal) mySnglLen;
  Standard_Boolean isSngl;


};







#endif // _GeomFill_Frenet_HeaderFile
