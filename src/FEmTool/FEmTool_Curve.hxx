// Created on: 1997-09-12
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

#ifndef _FEmTool_Curve_HeaderFile
#define _FEmTool_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_Array2OfReal.hxx>
class PLib_Base;

class FEmTool_Curve;
DEFINE_STANDARD_HANDLE(FEmTool_Curve, Standard_Transient)

//! Curve defined by Polynomial Elements.
class FEmTool_Curve : public Standard_Transient
{

public:
  Standard_EXPORT FEmTool_Curve(const Standard_Integer   Dimension,
                                const Standard_Integer   NbElements,
                                const Handle(PLib_Base)& TheBase,
                                const Standard_Real      Tolerance);

  Standard_EXPORT TColStd_Array1OfReal& Knots() const;

  Standard_EXPORT void SetElement(const Standard_Integer      IndexOfElement,
                                  const TColStd_Array2OfReal& Coeffs);

  Standard_EXPORT void D0(const Standard_Real U, TColStd_Array1OfReal& Pnt);

  Standard_EXPORT void D1(const Standard_Real U, TColStd_Array1OfReal& Vec);

  Standard_EXPORT void D2(const Standard_Real U, TColStd_Array1OfReal& Vec);

  Standard_EXPORT void Length(const Standard_Real FirstU,
                              const Standard_Real LastU,
                              Standard_Real&      Length);

  Standard_EXPORT void GetElement(const Standard_Integer IndexOfElement,
                                  TColStd_Array2OfReal&  Coeffs);

  //! returns  coefficients  of  all  elements  in  canonical  base.
  Standard_EXPORT void GetPolynom(TColStd_Array1OfReal& Coeffs);

  Standard_EXPORT Standard_Integer NbElements() const;

  Standard_EXPORT Standard_Integer Dimension() const;

  Standard_EXPORT Handle(PLib_Base) Base() const;

  Standard_EXPORT Standard_Integer Degree(const Standard_Integer IndexOfElement) const;

  Standard_EXPORT void SetDegree(const Standard_Integer IndexOfElement,
                                 const Standard_Integer Degree);

  Standard_EXPORT void ReduceDegree(const Standard_Integer IndexOfElement,
                                    const Standard_Real    Tol,
                                    Standard_Integer&      NewDegree,
                                    Standard_Real&         MaxError);

  DEFINE_STANDARD_RTTIEXT(FEmTool_Curve, Standard_Transient)

protected:
private:
  Standard_EXPORT void Update(const Standard_Integer Element, const Standard_Integer Order);

  Standard_Integer              myNbElements;
  Standard_Integer              myDimension;
  Handle(PLib_Base)             myBase;
  Handle(TColStd_HArray1OfReal) myKnots;
  TColStd_Array1OfInteger       myDegree;
  TColStd_Array1OfReal          myCoeff;
  TColStd_Array1OfReal          myPoly;
  TColStd_Array1OfReal          myDeri;
  TColStd_Array1OfReal          myDsecn;
  TColStd_Array1OfInteger       HasPoly;
  TColStd_Array1OfInteger       HasDeri;
  TColStd_Array1OfInteger       HasSecn;
  TColStd_Array1OfReal          myLength;
  Standard_Real                 Uf;
  Standard_Real                 Ul;
  Standard_Real                 Denom;
  Standard_Real                 USum;
  Standard_Integer              myIndex;
  Standard_Integer              myPtr;
};

#endif // _FEmTool_Curve_HeaderFile
