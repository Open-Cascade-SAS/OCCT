// Created on: 1996-04-10
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AdvApp2Var_Patch_HeaderFile
#define _AdvApp2Var_Patch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
#include <TColgp_HArray2OfPnt.hxx>
class AdvApp2Var_Context;
class AdvApp2Var_Framework;
class AdvApp2Var_Criterion;

//! used to store results on a domain [Ui,Ui+1]x[Vj,Vj+1]
class AdvApp2Var_Patch : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AdvApp2Var_Patch, Standard_Transient)
public:
  Standard_EXPORT AdvApp2Var_Patch();

  Standard_EXPORT AdvApp2Var_Patch(const Standard_Real    U0,
                                   const Standard_Real    U1,
                                   const Standard_Real    V0,
                                   const Standard_Real    V1,
                                   const Standard_Integer iu,
                                   const Standard_Integer iv);

  Standard_EXPORT Standard_Boolean IsDiscretised() const;

  Standard_EXPORT void Discretise(const AdvApp2Var_Context&           Conditions,
                                  const AdvApp2Var_Framework&         Constraints,
                                  const AdvApp2Var_EvaluatorFunc2Var& func);

  Standard_EXPORT Standard_Boolean IsApproximated() const;

  Standard_EXPORT Standard_Boolean HasResult() const;

  Standard_EXPORT void MakeApprox(const AdvApp2Var_Context&   Conditions,
                                  const AdvApp2Var_Framework& Constraints,
                                  const Standard_Integer      NumDec);

  Standard_EXPORT void AddConstraints(const AdvApp2Var_Context&   Conditions,
                                      const AdvApp2Var_Framework& Constraints);

  Standard_EXPORT void AddErrors(const AdvApp2Var_Framework& Constraints);

  Standard_EXPORT void ChangeDomain(const Standard_Real a,
                                    const Standard_Real b,
                                    const Standard_Real c,
                                    const Standard_Real d);

  Standard_EXPORT void ResetApprox();

  Standard_EXPORT void OverwriteApprox();

  Standard_EXPORT Standard_Real U0() const;

  Standard_EXPORT Standard_Real U1() const;

  Standard_EXPORT Standard_Real V0() const;

  Standard_EXPORT Standard_Real V1() const;

  Standard_EXPORT Standard_Integer UOrder() const;

  Standard_EXPORT Standard_Integer VOrder() const;

  Standard_EXPORT Standard_Integer CutSense() const;

  Standard_EXPORT Standard_Integer CutSense(const AdvApp2Var_Criterion& Crit,
                                            const Standard_Integer      NumDec) const;

  Standard_EXPORT Standard_Integer NbCoeffInU() const;

  Standard_EXPORT Standard_Integer NbCoeffInV() const;

  Standard_EXPORT void ChangeNbCoeff(const Standard_Integer NbCoeffU,
                                     const Standard_Integer NbCoeffV);

  Standard_EXPORT Handle(TColgp_HArray2OfPnt) Poles(const Standard_Integer    SSPIndex,
                                                    const AdvApp2Var_Context& Conditions) const;

  Standard_EXPORT Handle(TColStd_HArray1OfReal) Coefficients(
    const Standard_Integer    SSPIndex,
    const AdvApp2Var_Context& Conditions) const;

  Standard_EXPORT Handle(TColStd_HArray1OfReal) MaxErrors() const;

  Standard_EXPORT Handle(TColStd_HArray1OfReal) AverageErrors() const;

  Standard_EXPORT Handle(TColStd_HArray2OfReal) IsoErrors() const;

  Standard_EXPORT Standard_Real CritValue() const;

  Standard_EXPORT void SetCritValue(const Standard_Real dist);

private:
  AdvApp2Var_Patch(const AdvApp2Var_Patch& P);
  AdvApp2Var_Patch& operator=(const AdvApp2Var_Patch& theOther);

private:
  Standard_Real                 myU0;
  Standard_Real                 myU1;
  Standard_Real                 myV0;
  Standard_Real                 myV1;
  Standard_Integer              myOrdInU;
  Standard_Integer              myOrdInV;
  Standard_Integer              myNbCoeffInU;
  Standard_Integer              myNbCoeffInV;
  Standard_Boolean              myApprIsDone;
  Standard_Boolean              myHasResult;
  Handle(TColStd_HArray1OfReal) myEquation;
  Handle(TColStd_HArray1OfReal) myMaxErrors;
  Handle(TColStd_HArray1OfReal) myMoyErrors;
  Handle(TColStd_HArray2OfReal) myIsoErrors;
  Standard_Integer              myCutSense;
  Standard_Boolean              myDiscIsDone;
  Handle(TColStd_HArray1OfReal) mySosoTab;
  Handle(TColStd_HArray1OfReal) myDisoTab;
  Handle(TColStd_HArray1OfReal) mySodiTab;
  Handle(TColStd_HArray1OfReal) myDidiTab;
  Standard_Real                 myCritValue;
};

#endif // _AdvApp2Var_Patch_HeaderFile
