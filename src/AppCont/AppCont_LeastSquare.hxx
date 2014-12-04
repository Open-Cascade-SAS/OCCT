// Created on: 1995-03-14
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef AppCont_LeastSquare_HeaderFile
#define AppCont_LeastSquare_HeaderFile

#include <AppCont_Function.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <NCollection_Array1.hxx>
#include <AppParCurves_Constraint.hxx>


struct PeriodicityInfo
{
  Standard_Boolean isPeriodic;
  Standard_Real    myPeriod;
};

class AppCont_LeastSquare
{
public:

  Standard_EXPORT AppCont_LeastSquare(const AppCont_Function&       SSP,
                                      const Standard_Real           U0,
                                      const Standard_Real           U1,
                                      const AppParCurves_Constraint FirstCons,
                                      const AppParCurves_Constraint LastCons,
                                      const Standard_Integer        Deg,
                                      const Standard_Integer        NbPoints);

  Standard_EXPORT const AppParCurves_MultiCurve& Value();

  Standard_EXPORT void Error(Standard_Real& F, 
                             Standard_Real& MaxE3d,
                             Standard_Real& MaxE2d) const;

  Standard_EXPORT Standard_Boolean IsDone() const;

private:

  //! Fix border point evaluation.
  void FixSingleBorderPoint(const AppCont_Function&   theSSP,
                            const Standard_Real       theU,
                            const Standard_Real       theU0,
                            const Standard_Real       theU1,
                            NCollection_Array1<gp_Pnt2d>& theFix2d,
                            NCollection_Array1<gp_Pnt>&   theFix);

  AppParCurves_MultiCurve mySCU;
  math_Matrix myPoints;
  math_Matrix myPoles;
  math_Vector myParam;
  math_Matrix myVB;
  NCollection_Array1<PeriodicityInfo> myPerInfo;
  Standard_Boolean myDone;
  Standard_Integer myDegre;
  Standard_Integer myNbdiscret, myNbP, myNbP2d;
};

#endif
