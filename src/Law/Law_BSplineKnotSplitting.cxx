// Created on: 1997-01-17
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


#include <BSplCLib.hxx>
#include <Law_BSpline.hxx>
#include <Law_BSplineKnotSplitting.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>

typedef TColStd_Array1OfInteger         Array1OfInteger;
typedef TColStd_HArray1OfInteger        HArray1OfInteger;

Law_BSplineKnotSplitting::
Law_BSplineKnotSplitting (const Handle(Law_BSpline)& BasisCurve, 
                          const Standard_Integer ContinuityRange)
{
  if (ContinuityRange < 0)  throw Standard_RangeError();

  Standard_Integer FirstIndex = BasisCurve->FirstUKnotIndex();
  Standard_Integer LastIndex  = BasisCurve->LastUKnotIndex();

  Standard_Integer Degree = BasisCurve->Degree();

  if (ContinuityRange == 0) {
    splitIndexes = new HArray1OfInteger (1, 2);
    splitIndexes->SetValue (1, FirstIndex);
    splitIndexes->SetValue (2, LastIndex);
  }
  else {
    Standard_Integer NbKnots = BasisCurve->NbKnots();
    Array1OfInteger Mults (1, NbKnots);
    BasisCurve->Multiplicities (Mults);
    Standard_Integer Mmax = BSplCLib::MaxKnotMult (Mults, FirstIndex, LastIndex);
    if (Degree - Mmax >= ContinuityRange) {
      splitIndexes = new HArray1OfInteger (1, 2);
      splitIndexes->SetValue (1, FirstIndex);
      splitIndexes->SetValue (2, LastIndex);
    }
    else {
      Array1OfInteger Split (1, LastIndex - FirstIndex + 1);
      Standard_Integer NbSplit = 1;
      Standard_Integer Index   = FirstIndex;
      Split (NbSplit) = Index;
      Index++;
      NbSplit++;
      while (Index < LastIndex) {
        if (Degree - Mults (Index) < ContinuityRange) {
          Split (NbSplit) = Index;
          NbSplit++;
        }
        Index++;
      }
      Split (NbSplit) = Index;
      splitIndexes = new HArray1OfInteger (1, NbSplit);
      for (Standard_Integer i = 1; i <= NbSplit; i++) {
         splitIndexes->SetValue (i, Split (i));
      }
    }
  }
}



Standard_Integer Law_BSplineKnotSplitting::NbSplits () const {

   return splitIndexes->Length();
}


Standard_Integer Law_BSplineKnotSplitting::SplitValue (

const Standard_Integer Index

) const {

   Standard_RangeError_Raise_if (
                       Index < 1 || Index > splitIndexes->Length(), " ");
   return splitIndexes->Value (Index);
}




void Law_BSplineKnotSplitting::Splitting (

Array1OfInteger& SplitValues

) const {

  for (Standard_Integer i = 1; i <= splitIndexes->Length(); i++){
    SplitValues (i) = splitIndexes->Value (i);
  }
}
