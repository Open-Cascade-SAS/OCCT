
// Copyright (c) 2020 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#ifndef _Extrema_GlobOptFuncConicS_HeaderFile
#define _Extrema_GlobOptFuncConicS_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_MultipleVarFunction.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>

//! This class implements function which calculate square Eucluidean distance
//! between point on surface and nearest point on Conic.
class Extrema_GlobOptFuncConicS : public math_MultipleVarFunction
{
public:
  //! Curve and surface should exist during all the lifetime of Extrema_GlobOptFuncConicS.
  Standard_EXPORT Extrema_GlobOptFuncConicS(const Adaptor3d_Curve* C, const Adaptor3d_Surface* S);

  Standard_EXPORT Extrema_GlobOptFuncConicS(const Adaptor3d_Surface* S);

  Standard_EXPORT Extrema_GlobOptFuncConicS(const Adaptor3d_Surface* S,
                                            const double             theUf,
                                            const double             theUl,
                                            const double             theVf,
                                            const double             theVl);

  Standard_EXPORT void LoadConic(const Adaptor3d_Curve* S, const double theTf, const double theTl);

  Standard_EXPORT int NbVariables() const override;

  Standard_EXPORT bool Value(const math_Vector& theX, double& theF) override;

  //! Parameter of conic for point on surface defined by theUV
  Standard_EXPORT double ConicParameter(const math_Vector& theUV) const;

private:
  bool checkInputData(const math_Vector& X, double& su, double& sv);

  void value(double su, double sv, double& F);

  const Adaptor3d_Curve*   myC;
  const Adaptor3d_Surface* myS;
  GeomAbs_CurveType        myCType;
  gp_Lin                   myLin;
  gp_Circ                  myCirc;
  gp_Elips                 myElips;
  gp_Hypr                  myHypr;
  gp_Parab                 myParab;
  gp_Pnt                   myCPf;
  gp_Pnt                   myCPl;
  // Boundaries
  double myTf;
  double myTl;
  double myUf;
  double myUl;
  double myVf;
  double myVl;
};

#endif
