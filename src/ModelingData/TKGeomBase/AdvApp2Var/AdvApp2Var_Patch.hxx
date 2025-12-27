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

  Standard_EXPORT AdvApp2Var_Patch(const double    U0,
                                   const double    U1,
                                   const double    V0,
                                   const double    V1,
                                   const int iu,
                                   const int iv);

  Standard_EXPORT bool IsDiscretised() const;

  Standard_EXPORT void Discretise(const AdvApp2Var_Context&           Conditions,
                                  const AdvApp2Var_Framework&         Constraints,
                                  const AdvApp2Var_EvaluatorFunc2Var& func);

  Standard_EXPORT bool IsApproximated() const;

  Standard_EXPORT bool HasResult() const;

  Standard_EXPORT void MakeApprox(const AdvApp2Var_Context&   Conditions,
                                  const AdvApp2Var_Framework& Constraints,
                                  const int      NumDec);

  Standard_EXPORT void AddConstraints(const AdvApp2Var_Context&   Conditions,
                                      const AdvApp2Var_Framework& Constraints);

  Standard_EXPORT void AddErrors(const AdvApp2Var_Framework& Constraints);

  Standard_EXPORT void ChangeDomain(const double a,
                                    const double b,
                                    const double c,
                                    const double d);

  Standard_EXPORT void ResetApprox();

  Standard_EXPORT void OverwriteApprox();

  Standard_EXPORT double U0() const;

  Standard_EXPORT double U1() const;

  Standard_EXPORT double V0() const;

  Standard_EXPORT double V1() const;

  Standard_EXPORT int UOrder() const;

  Standard_EXPORT int VOrder() const;

  Standard_EXPORT int CutSense() const;

  Standard_EXPORT int CutSense(const AdvApp2Var_Criterion& Crit,
                                            const int      NumDec) const;

  Standard_EXPORT int NbCoeffInU() const;

  Standard_EXPORT int NbCoeffInV() const;

  Standard_EXPORT void ChangeNbCoeff(const int NbCoeffU,
                                     const int NbCoeffV);

  Standard_EXPORT occ::handle<TColgp_HArray2OfPnt> Poles(const int    SSPIndex,
                                                    const AdvApp2Var_Context& Conditions) const;

  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> Coefficients(
    const int    SSPIndex,
    const AdvApp2Var_Context& Conditions) const;

  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> MaxErrors() const;

  Standard_EXPORT occ::handle<TColStd_HArray1OfReal> AverageErrors() const;

  Standard_EXPORT occ::handle<TColStd_HArray2OfReal> IsoErrors() const;

  Standard_EXPORT double CritValue() const;

  Standard_EXPORT void SetCritValue(const double dist);

private:
  AdvApp2Var_Patch(const AdvApp2Var_Patch& P);
  AdvApp2Var_Patch& operator=(const AdvApp2Var_Patch& theOther);

private:
  double                 myU0;
  double                 myU1;
  double                 myV0;
  double                 myV1;
  int              myOrdInU;
  int              myOrdInV;
  int              myNbCoeffInU;
  int              myNbCoeffInV;
  bool              myApprIsDone;
  bool              myHasResult;
  occ::handle<TColStd_HArray1OfReal> myEquation;
  occ::handle<TColStd_HArray1OfReal> myMaxErrors;
  occ::handle<TColStd_HArray1OfReal> myMoyErrors;
  occ::handle<TColStd_HArray2OfReal> myIsoErrors;
  int              myCutSense;
  bool              myDiscIsDone;
  occ::handle<TColStd_HArray1OfReal> mySosoTab;
  occ::handle<TColStd_HArray1OfReal> myDisoTab;
  occ::handle<TColStd_HArray1OfReal> mySodiTab;
  occ::handle<TColStd_HArray1OfReal> myDidiTab;
  double                 myCritValue;
};

#endif // _AdvApp2Var_Patch_HeaderFile
