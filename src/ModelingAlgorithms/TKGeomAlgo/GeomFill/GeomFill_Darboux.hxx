// Created on: 1998-03-04
// Created by: Roman BORISOV
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

#ifndef _GeomFill_Darboux_HeaderFile
#define _GeomFill_Darboux_HeaderFile

#include <Standard.hxx>

#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
class gp_Vec;

class GeomFill_Darboux;
DEFINE_STANDARD_HANDLE(GeomFill_Darboux, GeomFill_TrihedronLaw)

//! Defines Darboux case of Frenet Trihedron Law
class GeomFill_Darboux : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_Darboux();

  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;

  //! compute Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual Standard_Boolean D0(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             BiNormal) Standard_OVERRIDE;

  //! compute Triedrhon and  derivative Trihedron  on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal) Standard_OVERRIDE;

  //! compute  Trihedron on curve
  //! first and seconde  derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual Standard_Boolean D2(const Standard_Real Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             D2Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             D2Normal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal,
                                              gp_Vec&             D2BiNormal) Standard_OVERRIDE;

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

  //! Get average value of Tangent(t) and Normal(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) Standard_OVERRIDE;

  //! Say if the law is Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;

  //! Return False.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeomFill_Darboux, GeomFill_TrihedronLaw)

protected:
private:
};

#endif // _GeomFill_Darboux_HeaderFile
