// Created on: 1996-04-09
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

#ifndef _AdvApp2Var_Iso_HeaderFile
#define _AdvApp2Var_Iso_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_IsoType.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <AdvApp2Var_EvaluatorFunc2Var.hxx>
class AdvApp2Var_Context;
class AdvApp2Var_Node;

//! used to store constraints on a line U = Ui or V = Vj
class AdvApp2Var_Iso : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AdvApp2Var_Iso, Standard_Transient)
public:
  Standard_EXPORT AdvApp2Var_Iso();

  Standard_EXPORT AdvApp2Var_Iso(const GeomAbs_IsoType type, const int iu, const int iv);

  Standard_EXPORT AdvApp2Var_Iso(const GeomAbs_IsoType type,
                                 const double          cte,
                                 const double          Ufirst,
                                 const double          Ulast,
                                 const double          Vfirst,
                                 const double          Vlast,
                                 const int             pos,
                                 const int             iu,
                                 const int             iv);

  Standard_EXPORT bool IsApproximated() const;

  Standard_EXPORT bool HasResult() const;

  Standard_EXPORT void MakeApprox(const AdvApp2Var_Context&           Conditions,
                                  const double                        a,
                                  const double                        b,
                                  const double                        c,
                                  const double                        d,
                                  const AdvApp2Var_EvaluatorFunc2Var& func,
                                  AdvApp2Var_Node&                    NodeBegin,
                                  AdvApp2Var_Node&                    NodeEnd);

  Standard_EXPORT void ChangeDomain(const double a, const double b);

  Standard_EXPORT void ChangeDomain(const double a, const double b, const double c, const double d);

  Standard_EXPORT void SetConstante(const double newcte);

  Standard_EXPORT void SetPosition(const int newpos);

  Standard_EXPORT void ResetApprox();

  Standard_EXPORT void OverwriteApprox();

  Standard_EXPORT GeomAbs_IsoType Type() const;

  Standard_EXPORT double Constante() const;

  Standard_EXPORT double T0() const;

  Standard_EXPORT double T1() const;

  Standard_EXPORT double U0() const;

  Standard_EXPORT double U1() const;

  Standard_EXPORT double V0() const;

  Standard_EXPORT double V1() const;

  Standard_EXPORT int UOrder() const;

  Standard_EXPORT int VOrder() const;

  Standard_EXPORT int Position() const;

  Standard_EXPORT int NbCoeff() const;

  Standard_EXPORT const occ::handle<NCollection_HArray1<double>>& Polynom() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> SomTab() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> DifTab() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<double>> MaxErrors() const;

  Standard_EXPORT occ::handle<NCollection_HArray2<double>> MoyErrors() const;

private:
  AdvApp2Var_Iso(const AdvApp2Var_Iso& Other);
  AdvApp2Var_Iso& operator=(const AdvApp2Var_Iso& theOther);

private:
  GeomAbs_IsoType                          myType;
  double                                   myConstPar;
  double                                   myU0;
  double                                   myU1;
  double                                   myV0;
  double                                   myV1;
  int                                      myPosition;
  int                                      myExtremOrder;
  int                                      myDerivOrder;
  int                                      myNbCoeff;
  bool                                     myApprIsDone;
  bool                                     myHasResult;
  occ::handle<NCollection_HArray1<double>> myEquation;
  occ::handle<NCollection_HArray2<double>> myMaxErrors;
  occ::handle<NCollection_HArray2<double>> myMoyErrors;
  occ::handle<NCollection_HArray1<double>> mySomTab;
  occ::handle<NCollection_HArray1<double>> myDifTab;
};

#endif // _AdvApp2Var_Iso_HeaderFile
