// Created on: 2013-02-05
// Created by: Julia GERASIMOVA
// Copyright (c) 2001-2013 OPEN CASCADE SAS
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

#ifndef _GeomFill_DiscreteTrihedron_HeaderFile
#define _GeomFill_DiscreteTrihedron_HeaderFile

#include <Standard.hxx>

#include <gp_Ax2.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
class GeomFill_Frenet;
class gp_Vec;

//! Defined Discrete Trihedron Law.
//! The requirement for path curve is only G1.
//! The result is C0-continuous surface
//! that can be later approximated to C1.
class GeomFill_DiscreteTrihedron : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_DiscreteTrihedron();

  Standard_EXPORT virtual occ::handle<GeomFill_TrihedronLaw> Copy() const override;

  Standard_EXPORT void Init();

  //! initialize curve of trihedron law
  //! @return true in case if execution end correctly
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C) override;

  //! compute Trihedron on curve at parameter <Param>
  Standard_EXPORT virtual bool D0(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      BiNormal) override;

  //! compute Trihedron and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  //! For the moment it returns null values for DTangent, DNormal
  //! and DBiNormal.
  Standard_EXPORT virtual bool D1(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal) override;

  //! compute Trihedron on curve
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
  //! For the moment it returns null values for DTangent, DNormal
  //! DBiNormal, D2Tangent, D2Normal, D2BiNormal.
  Standard_EXPORT virtual bool D2(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      D2Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      D2Normal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal,
                                  gp_Vec&      D2BiNormal) override;

  //! Returns the number of intervals for continuity
  //! <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape         S) const override;

  //! Get average value of Tangent(t) and Normal(t) it is usful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) override;

  //! Say if the law is Constant.
  Standard_EXPORT virtual bool IsConstant() const override;

  //! Return True.
  Standard_EXPORT virtual bool IsOnlyBy3dCurve() const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_DiscreteTrihedron, GeomFill_TrihedronLaw)

private:
  gp_Pnt                                     myPoint;
  occ::handle<NCollection_HSequence<gp_Ax2>> myTrihedrons;
  occ::handle<NCollection_HSequence<double>> myKnots;
  occ::handle<GeomFill_Frenet>               myFrenet;
  bool                                       myUseFrenet;
};

#endif // _GeomFill_DiscreteTrihedron_HeaderFile
