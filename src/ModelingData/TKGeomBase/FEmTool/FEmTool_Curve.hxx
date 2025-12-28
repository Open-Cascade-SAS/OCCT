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
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Array2.hxx>
#include <PLib_HermitJacobi.hxx>

//! Curve defined by Polynomial Elements.
class FEmTool_Curve : public Standard_Transient
{

public:
  Standard_EXPORT FEmTool_Curve(const int   Dimension,
                                const int   NbElements,
                                const PLib_HermitJacobi& TheBase,
                                const double      Tolerance);

  Standard_EXPORT NCollection_Array1<double>& Knots() const;

  Standard_EXPORT void SetElement(const int      IndexOfElement,
                                  const NCollection_Array2<double>& Coeffs);

  Standard_EXPORT void D0(const double U, NCollection_Array1<double>& Pnt);

  Standard_EXPORT void D1(const double U, NCollection_Array1<double>& Vec);

  Standard_EXPORT void D2(const double U, NCollection_Array1<double>& Vec);

  Standard_EXPORT void Length(const double FirstU,
                              const double LastU,
                              double&      Length);

  Standard_EXPORT void GetElement(const int IndexOfElement,
                                  NCollection_Array2<double>&  Coeffs);

  //! returns coefficients of all elements in canonical base.
  Standard_EXPORT void GetPolynom(NCollection_Array1<double>& Coeffs);

  Standard_EXPORT int NbElements() const;

  Standard_EXPORT int Dimension() const;

  Standard_EXPORT const PLib_HermitJacobi& Base() const;

  Standard_EXPORT int Degree(const int IndexOfElement) const;

  Standard_EXPORT void SetDegree(const int IndexOfElement,
                                 const int Degree);

  Standard_EXPORT void ReduceDegree(const int IndexOfElement,
                                    const double    Tol,
                                    int&      NewDegree,
                                    double&         MaxError);

  DEFINE_STANDARD_RTTIEXT(FEmTool_Curve, Standard_Transient)

private:
  Standard_EXPORT void Update(const int Element, const int Order);

  int              myNbElements;
  int              myDimension;
  PLib_HermitJacobi             myBase;
  occ::handle<NCollection_HArray1<double>> myKnots;
  NCollection_Array1<int>       myDegree;
  NCollection_Array1<double>          myCoeff;
  NCollection_Array1<double>          myPoly;
  NCollection_Array1<double>          myDeri;
  NCollection_Array1<double>          myDsecn;
  NCollection_Array1<int>       HasPoly;
  NCollection_Array1<int>       HasDeri;
  NCollection_Array1<int>       HasSecn;
  NCollection_Array1<double>          myLength;
  double                 Uf;
  double                 Ul;
  double                 Denom;
  double                 USum;
  int              myIndex;
  int              myPtr;
};

#endif // _FEmTool_Curve_HeaderFile
