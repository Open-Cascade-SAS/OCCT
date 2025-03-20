// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2022 OPEN CASCADE SAS
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

#include <GeomConvert_FuncConeLSDist.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax3.hxx>
#include <math_Vector.hxx>
#include <ElSLib.hxx>

//=================================================================================================

GeomConvert_FuncConeLSDist::GeomConvert_FuncConeLSDist(const Handle(TColgp_HArray1OfXYZ)& thePoints,
                                                       const gp_Dir&                      theDir)
    : myPoints(thePoints),
      myDir(theDir)
{
}

//=================================================================================================

Standard_Integer GeomConvert_FuncConeLSDist::NbVariables() const
{
  return 5;
}

//=================================================================================================

Standard_Boolean GeomConvert_FuncConeLSDist::Value(const math_Vector& X, Standard_Real& F)
{
  gp_Pnt        aLoc(X(1), X(2), X(3));
  Standard_Real aSemiAngle = X(4), anR = X(5);
  gp_Ax3        aPos(aLoc, myDir);

  F = 0.;
  Standard_Integer i;
  for (i = myPoints->Lower(); i <= myPoints->Upper(); ++i)
  {
    Standard_Real u, v;
    gp_Pnt        aPi(myPoints->Value(i));
    ElSLib::ConeParameters(aPos, anR, aSemiAngle, aPi, u, v);
    gp_Pnt aPp;
    ElSLib::ConeD0(u, v, aPos, anR, aSemiAngle, aPp);
    F += aPi.SquareDistance(aPp);
  }

  return Standard_True;
}
