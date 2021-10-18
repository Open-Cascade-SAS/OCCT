// Copyright (c) 2014-2014 OPEN CASCADE SAS
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

#ifndef _GCPnts_DistFunction_HeaderFile
#define _GCPnts_DistFunction_HeaderFile

#include <gp_Lin.hxx>
#include <math_Function.hxx>
#include <math_MultipleVarFunction.hxx>
#include <Adaptor3d_Curve.hxx>


//! Class to define function, which calculates square distance between point on curve
//! C(u), U1 <= u <= U2 and line passing through points C(U1) and C(U2)
//! This function is used in any minimization algorithm to define maximal deviation between curve and line,
//! which required one variable function without derivative (for ex. math_BrentMinimum)
class GCPnts_DistFunction : public math_Function
{
public:
  Standard_EXPORT GCPnts_DistFunction(const Adaptor3d_Curve& theCurve,
                                      const Standard_Real U1, const Standard_Real U2);
  //
  Standard_EXPORT GCPnts_DistFunction(const GCPnts_DistFunction& theOther);

  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real X,
                                                        Standard_Real& F);
private:
  GCPnts_DistFunction & operator = (const GCPnts_DistFunction & theOther);

  const Adaptor3d_Curve& myCurve;
  gp_Lin myLin;
  Standard_Real myU1;
  Standard_Real myU2;
};
//
//! The same as class GCPnts_DistFunction, but it can be used in minimization algorithms that
//! requires multi variable function
class GCPnts_DistFunctionMV : public math_MultipleVarFunction
{
public:
  Standard_EXPORT GCPnts_DistFunctionMV(GCPnts_DistFunction& theCurvLinDist);

  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X,
                                                        Standard_Real& F);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

private:
  GCPnts_DistFunctionMV & operator = (const GCPnts_DistFunctionMV & theOther);
  GCPnts_DistFunction& myMaxCurvLinDist;
};

//


#endif // _GCPnts_DistFunction_HeaderFile
