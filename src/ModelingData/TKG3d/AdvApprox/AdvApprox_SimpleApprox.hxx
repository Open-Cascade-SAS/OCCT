// Created on: 1996-10-14
// Created by: Jeannine PANTIATICI
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

#ifndef _AdvApprox_SimpleApprox_HeaderFile
#define _AdvApprox_SimpleApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <GeomAbs_Shape.hxx>
#include <AdvApprox_EvaluatorFunction.hxx>
#include <Standard_OStream.hxx>
#include <PLib_JacobiPolynomial.hxx>

//! Approximate a function on an interval [First,Last]
//! The result is a simple polynomial whose degree is as low as
//! possible to satisfy the required tolerance and the
//! maximum degree. The maximum error and the average error
//! resulting from approximating the function by the polynomial are computed
class AdvApprox_SimpleApprox
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT AdvApprox_SimpleApprox(const int                          TotalDimension,
                                         const int                          TotalNumSS,
                                         const GeomAbs_Shape                Continuity,
                                         const int                          WorkDegree,
                                         const int                          NbGaussPoints,
                                         const PLib_JacobiPolynomial&       JacobiBase,
                                         const AdvApprox_EvaluatorFunction& Func);

  //! Constructs approximator tool.
  //!
  //! Warning:
  //! the Func should be valid reference to object of type
  //! inherited from class EvaluatorFunction from Approx
  //! with life time longer than that of the approximator tool;
  Standard_EXPORT void Perform(const NCollection_Array1<int>&    LocalDimension,
                               const NCollection_Array1<double>& LocalTolerancesArray,
                               const double                      First,
                               const double                      Last,
                               const int                         MaxDegree);

  Standard_EXPORT bool IsDone() const;

  Standard_EXPORT int Degree() const;

  //! returns the coefficients in the Jacobi Base
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> Coefficients() const;

  //! returns the constraints at First
  Standard_EXPORT occ::handle<NCollection_HArray2<double>> FirstConstr() const;

  //! returns the constraints at Last
  Standard_EXPORT occ::handle<NCollection_HArray2<double>> LastConstr() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> SomTab() const;

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> DifTab() const;

  Standard_EXPORT double MaxError(const int Index) const;

  Standard_EXPORT double AverageError(const int Index) const;

  //! display information on approximation
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  int                                      myTotalNumSS;
  int                                      myTotalDimension;
  int                                      myNbGaussPoints;
  int                                      myWorkDegree;
  int                                      myNivConstr;
  PLib_JacobiPolynomial                    myJacPol;
  occ::handle<NCollection_HArray1<double>> myTabPoints;
  occ::handle<NCollection_HArray2<double>> myTabWeights;
  void*                                    myEvaluator;
  int                                      myDegree;
  occ::handle<NCollection_HArray1<double>> myCoeff;
  occ::handle<NCollection_HArray2<double>> myFirstConstr;
  occ::handle<NCollection_HArray2<double>> myLastConstr;
  occ::handle<NCollection_HArray1<double>> mySomTab;
  occ::handle<NCollection_HArray1<double>> myDifTab;
  occ::handle<NCollection_HArray1<double>> myMaxError;
  occ::handle<NCollection_HArray1<double>> myAverageError;
  bool                                     done;
};

#endif // _AdvApprox_SimpleApprox_HeaderFile
