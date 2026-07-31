// Created on: 1997-12-02
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

#ifndef _GeomFill_TrihedronLaw_HeaderFile
#define _GeomFill_TrihedronLaw_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_PipeError.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>

class gp_Vec;

//! To define Trihedron along one Curve
class GeomFill_TrihedronLaw : public Standard_Transient
{

public:
  //! initialize curve of trihedron law
  //! @return true
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C);

  Standard_EXPORT virtual occ::handle<GeomFill_TrihedronLaw> Copy() const = 0;

  //! Give a status to the Law
  //! Returns PipeOk (default implementation)
  Standard_EXPORT virtual GeomFill_PipeError ErrorStatus() const;

  //! compute Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual bool D0(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      BiNormal) = 0;

  //! compute Triedrhon and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal);

  //! compute Trihedron on curve
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT virtual bool D2(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      D2Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      D2Normal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal,
                                  gp_Vec&      D2BiNormal);

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
  Standard_EXPORT virtual void SetInterval(const double First, const double Last);

  //! Gets the bounds of the parametric interval on
  //! the function
  Standard_EXPORT void GetInterval(double& First, double& Last);

  //! Get average value of M(t) and V(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) = 0;

  //! Say if the law is Constant
  Standard_EXPORT virtual bool IsConstant() const;

  //! Say if the law is defined, only by the 3d Geometry of
  //! the set Curve
  //! Return False by Default.
  Standard_EXPORT virtual bool IsOnlyBy3dCurve() const;

  DEFINE_STANDARD_RTTIEXT(GeomFill_TrihedronLaw, Standard_Transient)

protected:
  occ::handle<Adaptor3d_Curve> myCurve;
  occ::handle<Adaptor3d_Curve> myTrimmed;
};

#endif // _GeomFill_TrihedronLaw_HeaderFile
