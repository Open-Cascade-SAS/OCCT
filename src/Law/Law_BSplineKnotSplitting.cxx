// Created on: 1997-01-17
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <Law_BSplineKnotSplitting.ixx>


#include <Law_BSplineKnotSplitting.ixx>
#include <Standard_RangeError.hxx>

#include <BSplCLib.hxx>

typedef Handle(Law_BSpline)             Handle(BSplineCurve);
typedef TColStd_Array1OfInteger         Array1OfInteger;
typedef TColStd_HArray1OfInteger        HArray1OfInteger;




Law_BSplineKnotSplitting::
Law_BSplineKnotSplitting (

const Handle(BSplineCurve)& BasisCurve, 
const Standard_Integer               ContinuityRange

) {


  if (ContinuityRange < 0)  Standard_RangeError::Raise();

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
