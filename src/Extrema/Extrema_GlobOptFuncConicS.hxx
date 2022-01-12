
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
  Standard_EXPORT  Extrema_GlobOptFuncConicS(const Adaptor3d_Curve   *C,
                                             const Adaptor3d_Surface *S);

  Standard_EXPORT  Extrema_GlobOptFuncConicS(const Adaptor3d_Surface *S);

  Standard_EXPORT  Extrema_GlobOptFuncConicS(const Adaptor3d_Surface *S,
                                             const Standard_Real     theUf, 
                                             const Standard_Real     theUl,
                                             const Standard_Real     theVf, 
                                             const Standard_Real     theVl);

  Standard_EXPORT void LoadConic(const Adaptor3d_Curve *S, const Standard_Real theTf, const Standard_Real theTl);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector &theX,
                                                 Standard_Real     &theF);

  //! Parameter of conic for point on surface defined by theUV
  Standard_EXPORT Standard_Real ConicParameter(const math_Vector& theUV) const;

private:

  Standard_Boolean checkInputData(const math_Vector   &X,
                                  Standard_Real       &su,
                                  Standard_Real       &sv);

  void value(Standard_Real su,
             Standard_Real sv,
             Standard_Real &F);


  const Adaptor3d_Curve   *myC;
  const Adaptor3d_Surface *myS;
  GeomAbs_CurveType myCType;
  gp_Lin myLin;
  gp_Circ myCirc;
  gp_Elips myElips;
  gp_Hypr myHypr;
  gp_Parab myParab;
  gp_Pnt myCPf;
  gp_Pnt myCPl;
  //Boundaries
  Standard_Real myTf;
  Standard_Real myTl;
  Standard_Real myUf;
  Standard_Real myUl;
  Standard_Real myVf;
  Standard_Real myVl;

};

#endif
