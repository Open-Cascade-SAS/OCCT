// File:	Law_BSplineKnotSplitting.cxx
// Created:	Fri Jan 17 15:23:19 1997
// Author:	Philippe MANGIN
//		<pmn@sgi29>


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
