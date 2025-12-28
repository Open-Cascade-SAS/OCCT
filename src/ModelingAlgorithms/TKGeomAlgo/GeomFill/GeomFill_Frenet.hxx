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

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
class gp_Vec;

//! Defined Frenet Trihedron Law
class GeomFill_Frenet : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_Frenet();

  Standard_EXPORT virtual occ::handle<GeomFill_TrihedronLaw> Copy() const override;

  Standard_EXPORT void Init();

  //! initialize curve of frenet law
  //! @return true
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C) override;

  //! compute Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual bool D0(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      BiNormal) override;

  //! compute Triedrhon and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning: It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double Param,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal) override;

  //! compute Trihedron on curve
  //! first and second derivatives.
  //! Warning: It used only for C2 approximation
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

  //! Returns the number of intervals for continuity <S>.
  //! May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(NCollection_Array1<double>& T,
                                         const GeomAbs_Shape         S) const override;

  //! Get average value of Tangent(t) and Normal(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) override;

  //! Say if the law is Constant.
  Standard_EXPORT virtual bool IsConstant() const override;

  //! Return True.
  Standard_EXPORT virtual bool IsOnlyBy3dCurve() const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_Frenet, GeomFill_TrihedronLaw)

private:
  Standard_EXPORT bool IsSingular(const double U, int& Index) const;

  Standard_EXPORT bool DoSingular(const double U,
                                  const int    Index,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      BiNormal,
                                  int&         n,
                                  int&         k,
                                  int&         TFlag,
                                  int&         BNFlag,
                                  double&      Delta);

  //! computes Triedrhon on curve at parameter <Param>
  Standard_EXPORT bool SingularD0(const double Param,
                                  const int    Index,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      BiNormal,
                                  double&      Delta);

  //! computes Triedrhon and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT bool SingularD1(const double Param,
                                  const int    Index,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal,
                                  double&      Delta);

  //! computes Trihedron on curve
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
  Standard_EXPORT bool SingularD2(const double Param,
                                  const int    Index,
                                  gp_Vec&      Tangent,
                                  gp_Vec&      DTangent,
                                  gp_Vec&      D2Tangent,
                                  gp_Vec&      Normal,
                                  gp_Vec&      DNormal,
                                  gp_Vec&      D2Normal,
                                  gp_Vec&      BiNormal,
                                  gp_Vec&      DBiNormal,
                                  gp_Vec&      D2BiNormal,
                                  double&      Delta);

  //! revolves the trihedron (which is determined
  //! of given "Tangent", "Normal" and "BiNormal" vectors)
  //! to coincide "Tangent" and "NewTangent" axes.
  Standard_EXPORT bool RotateTrihedron(gp_Vec&       Tangent,
                                       gp_Vec&       Normal,
                                       gp_Vec&       BiNormal,
                                       const gp_Vec& NewTangent) const;

  gp_Pnt                                   P;
  occ::handle<NCollection_HArray1<double>> mySngl;
  occ::handle<NCollection_HArray1<double>> mySnglLen;
  bool                                     isSngl;
};

#endif // _GeomFill_Frenet_HeaderFile
