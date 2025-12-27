// Created on: 1997-12-09
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

#ifndef _GeomFill_ConstantBiNormal_HeaderFile
#define _GeomFill_ConstantBiNormal_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Vec.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
class GeomFill_Frenet;
class gp_Dir;

//! Defined a Trihedron Law where the BiNormal, is fixed
class GeomFill_ConstantBiNormal : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_ConstantBiNormal(const gp_Dir& BiNormal);

  Standard_EXPORT virtual occ::handle<GeomFill_TrihedronLaw> Copy() const override;

  //! initialize curve of trihedron law
  //! @return true in case if execution end correctly
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C)
    override;

  //! Computes Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual bool D0(const double Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             BiNormal) override;

  //! Computes Triedrhon and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning: It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal) override;

  //! compute Trihedron on curve
  //! first and second derivatives.
  //! Warning: It used only for C2 approximation
  Standard_EXPORT virtual bool D2(const double Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             D2Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             D2Normal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal,
                                              gp_Vec&             D2BiNormal) override;

  //! Returns the number of intervals for continuity <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const
    override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape   S) const override;

  //! Gets average value of Tangent(t) and Normal(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) override;

  //! Says if the law is Constant.
  Standard_EXPORT virtual bool IsConstant() const override;

  //! Return True.
  Standard_EXPORT virtual bool IsOnlyBy3dCurve() const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_ConstantBiNormal, GeomFill_TrihedronLaw)

private:
  gp_Vec                  BN;
  occ::handle<GeomFill_Frenet> frenet;
};

#endif // _GeomFill_ConstantBiNormal_HeaderFile
