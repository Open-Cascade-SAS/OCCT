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

#ifndef _GeomFill_CorrectedFrenet_HeaderFile
#define _GeomFill_CorrectedFrenet_HeaderFile

#include <Standard.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_Array1.hxx>
#include <GeomFill_Trihedron.hxx>
class GeomFill_Frenet;
class Law_Function;

//! Defined an Corrected Frenet Trihedron Law It is
//! like Frenet with an Torsion's minimization
class GeomFill_CorrectedFrenet : public GeomFill_TrihedronLaw
{

public:
  Standard_EXPORT GeomFill_CorrectedFrenet();

  Standard_EXPORT GeomFill_CorrectedFrenet(const bool ForEvaluation);

  Standard_EXPORT virtual occ::handle<GeomFill_TrihedronLaw> Copy() const override;

  //! initialize curve of frenet law
  //! @return true in case if execution end correctly
  Standard_EXPORT virtual bool SetCurve(const occ::handle<Adaptor3d_Curve>& C)
    override;

  Standard_EXPORT virtual void SetInterval(const double First,
                                           const double Last) override;

  //! compute Triedrhon on curve at parameter <Param>
  Standard_EXPORT virtual bool D0(const double Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             BiNormal) override;

  //! compute Triedrhon and derivative Trihedron on curve
  //! at parameter <Param>
  //! Warning : It used only for C1 or C2 approximation
  Standard_EXPORT virtual bool D1(const double Param,
                                              gp_Vec&             Tangent,
                                              gp_Vec&             DTangent,
                                              gp_Vec&             Normal,
                                              gp_Vec&             DNormal,
                                              gp_Vec&             BiNormal,
                                              gp_Vec&             DBiNormal) override;

  //! compute Trihedron on curve
  //! first and second derivatives.
  //! Warning : It used only for C2 approximation
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

  //! Returns the number of intervals for continuity
  //! <S>.
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

  //! Tries to define the best trihedron mode
  //! for the curve. It can be:
  //! - Frenet
  //! - CorrectedFrenet
  //! - DiscreteTrihedron
  //! Warning: the CorrectedFrenet must be constructed
  //! with option ForEvaluation = True,
  //! the curve must be set by method SetCurve.
  Standard_EXPORT GeomFill_Trihedron EvaluateBestMode();

  //! Get average value of Tangent(t) and Normal(t) it is useful to
  //! make fast approximation of rational surfaces.
  Standard_EXPORT virtual void GetAverageLaw(gp_Vec& ATangent,
                                             gp_Vec& ANormal,
                                             gp_Vec& ABiNormal) override;

  //! Say if the law is Constant.
  Standard_EXPORT virtual bool IsConstant() const override;

  //! Return True.
  Standard_EXPORT virtual bool IsOnlyBy3dCurve() const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_CorrectedFrenet, GeomFill_TrihedronLaw)

private:
  Standard_EXPORT void Init();

  //! Computes BSpline representation of Normal evolution at one
  //! interval of continuity of Frenet. Returns True if FuncInt = 0
  Standard_EXPORT bool InitInterval(const double     First,
                                                const double     Last,
                                                const double     Step,
                                                double&          startAng,
                                                gp_Vec&                 prevTangent,
                                                gp_Vec&                 prevNormal,
                                                gp_Vec&                 aT,
                                                gp_Vec&                 aN,
                                                occ::handle<Law_Function>&   FuncInt,
                                                NCollection_Sequence<double>& SeqPoles,
                                                NCollection_Sequence<double>& SeqAngle,
                                                NCollection_Sequence<gp_Vec>&   SeqTangent,
                                                NCollection_Sequence<gp_Vec>&   SeqNormal) const;

  //! Computes angle of Normal evolution of Frenet between any two points on the curve.
  Standard_EXPORT double CalcAngleAT(const gp_Vec& Tangent,
                                            const gp_Vec& Normal,
                                            const gp_Vec& prevTangent,
                                            const gp_Vec& prevNormal) const;

  //! Get corrected value of angle of Normal evolution of Frenet
  Standard_EXPORT double GetAngleAT(const double P) const;

  occ::handle<GeomFill_Frenet>       frenet;
  occ::handle<Law_Function>          EvolAroundT;
  occ::handle<Law_Function>          TLaw;
  gp_Vec                        AT;
  gp_Vec                        AN;
  bool              isFrenet;
  bool              myForEvaluation;
  occ::handle<NCollection_HArray1<double>> HArrPoles;
  occ::handle<NCollection_HArray1<double>> HArrAngle;
  occ::handle<NCollection_HArray1<gp_Vec>>   HArrTangent;
  occ::handle<NCollection_HArray1<gp_Vec>>   HArrNormal;
};

#endif // _GeomFill_CorrectedFrenet_HeaderFile
