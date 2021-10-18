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
#include <Standard_Type.hxx>

#include <gp_Vec.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_SequenceOfVec.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomFill_Trihedron.hxx>
class GeomFill_Frenet;
class Law_Function;

class GeomFill_CorrectedFrenet;
DEFINE_STANDARD_HANDLE(GeomFill_CorrectedFrenet, GeomFill_TrihedronLaw)

//! Defined an Corrected Frenet  Trihedron  Law It is
//! like Frenet with an Torsion's minimization
class GeomFill_CorrectedFrenet : public GeomFill_TrihedronLaw
{

public:

  
  Standard_EXPORT GeomFill_CorrectedFrenet();
  
  Standard_EXPORT GeomFill_CorrectedFrenet(const Standard_Boolean ForEvaluation);
  
  Standard_EXPORT virtual Handle(GeomFill_TrihedronLaw) Copy() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetCurve (const Handle(Adaptor3d_Curve)& C) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetInterval (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE;
  
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
  
  //! Tries to define the best trihedron mode
  //! for the curve. It can be:
  //! - Frenet
  //! - CorrectedFrenet
  //! - DiscreteTrihedron
  //! Warning: the CorrectedFrenet must be constructed
  //! with option ForEvaluation = True,
  //! the curve must be set by method SetCurve.
  Standard_EXPORT GeomFill_Trihedron EvaluateBestMode();
  
  //! Get average value of Tangent(t) and Normal(t) it is usfull to
  //! make fast approximation of rational  surfaces.
  Standard_EXPORT virtual void GetAverageLaw (gp_Vec& ATangent, gp_Vec& ANormal, gp_Vec& ABiNormal) Standard_OVERRIDE;
  
  //! Say if the law is Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;
  
  //! Return True.
  Standard_EXPORT virtual Standard_Boolean IsOnlyBy3dCurve() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_CorrectedFrenet,GeomFill_TrihedronLaw)

protected:




private:

  
  Standard_EXPORT void Init();
  
  //! Computes BSpline representation of Normal evolution at one
  //! interval of continuity of Frenet. Returns True if FuncInt = 0
  Standard_EXPORT Standard_Boolean InitInterval (const Standard_Real First, const Standard_Real Last, const Standard_Real Step, Standard_Real& startAng, gp_Vec& prevTangent, gp_Vec& prevNormal, gp_Vec& aT, gp_Vec& aN, Handle(Law_Function)& FuncInt, TColStd_SequenceOfReal& SeqPoles, TColStd_SequenceOfReal& SeqAngle, TColgp_SequenceOfVec& SeqTangent, TColgp_SequenceOfVec& SeqNormal) const;
  
  //! Computes angle of Normal evolution of Frenet between any two points on the curve.
  Standard_EXPORT Standard_Real CalcAngleAT (const gp_Vec& Tangent, const gp_Vec& Normal, const gp_Vec& prevTangent, const gp_Vec& prevNormal) const;
  
  //! Get corrected value of angle of Normal evolution of Frenet
  Standard_EXPORT Standard_Real GetAngleAT (const Standard_Real P) const;

  Handle(GeomFill_Frenet) frenet;
  Handle(Law_Function) EvolAroundT;
  Handle(Law_Function) TLaw;
  gp_Vec AT;
  gp_Vec AN;
  Standard_Boolean isFrenet;
  Standard_Boolean myForEvaluation;
  Handle(TColStd_HArray1OfReal) HArrPoles;
  Handle(TColStd_HArray1OfReal) HArrAngle;
  Handle(TColgp_HArray1OfVec) HArrTangent;
  Handle(TColgp_HArray1OfVec) HArrNormal;


};







#endif // _GeomFill_CorrectedFrenet_HeaderFile
