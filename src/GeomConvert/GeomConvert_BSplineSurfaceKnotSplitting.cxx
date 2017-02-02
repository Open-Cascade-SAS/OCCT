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

//Jean-Claude Vauthier 28 Novembre 1991
//Passage sur C1 Aout 1992

#include <BSplCLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert_BSplineSurfaceKnotSplitting.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_RangeError.hxx>

typedef TColStd_Array1OfInteger      Array1OfInteger;
typedef TColStd_HArray1OfInteger HArray1OfInteger;

GeomConvert_BSplineSurfaceKnotSplitting::
GeomConvert_BSplineSurfaceKnotSplitting (

const Handle(Geom_BSplineSurface)& BasisSurface, 
const Standard_Integer        UContinuityRange,
const Standard_Integer        VContinuityRange

) {


  if (UContinuityRange < 0 || VContinuityRange < 0) { 
    throw Standard_RangeError();
  }

  Standard_Integer FirstUIndex = BasisSurface->FirstUKnotIndex ();
  Standard_Integer LastUIndex  = BasisSurface->LastUKnotIndex  ();
  Standard_Integer FirstVIndex = BasisSurface->FirstVKnotIndex ();
  Standard_Integer LastVIndex  = BasisSurface->LastVKnotIndex  ();
  Standard_Integer UDegree     = BasisSurface->UDegree ();
  Standard_Integer VDegree     = BasisSurface->VDegree ();
  Standard_Integer i;



  if (UContinuityRange == 0) {
    usplitIndexes = new HArray1OfInteger (1, 2);
    usplitIndexes->SetValue (1, FirstUIndex);
    usplitIndexes->SetValue (2, LastUIndex);
  }
  else {
    Standard_Integer NbUKnots = BasisSurface->NbUKnots();
    Array1OfInteger UMults (1, NbUKnots);
    BasisSurface->UMultiplicities (UMults);
    Standard_Integer Mmax = BSplCLib::MaxKnotMult (UMults, FirstUIndex, LastUIndex);
    if (UDegree - Mmax >= UContinuityRange) {
      usplitIndexes = new HArray1OfInteger (1, 2);
      usplitIndexes->SetValue (1, FirstUIndex);
      usplitIndexes->SetValue (2, LastUIndex);
    }
    else {
      Array1OfInteger USplit (1, LastUIndex - FirstUIndex + 1);
      Standard_Integer NbUSplit = 1;
      Standard_Integer UIndex = FirstUIndex;
      USplit (NbUSplit) = UIndex;
      UIndex++;
      NbUSplit++;
      while (UIndex < LastUIndex) {
        if (UDegree - UMults(UIndex) < UContinuityRange) {
          USplit (NbUSplit) = UIndex;
          NbUSplit++;
        }
        UIndex++;
      }
      USplit (NbUSplit) = UIndex;
      usplitIndexes = new HArray1OfInteger (1, NbUSplit);
      for (i = 1; i <= NbUSplit; i++) {
        usplitIndexes->SetValue (i, USplit (i));
      }
    }
  }



  if (VContinuityRange == 0) {
    vsplitIndexes = new HArray1OfInteger (1, 2);
    vsplitIndexes->SetValue (1, FirstVIndex);
    vsplitIndexes->SetValue (2, LastVIndex);
  }
  else {
    Standard_Integer NbVKnots = BasisSurface->NbVKnots();
    Array1OfInteger VMults (1, NbVKnots);
    BasisSurface->VMultiplicities (VMults);
    Standard_Integer Mmax = BSplCLib::MaxKnotMult (VMults, FirstVIndex, LastVIndex);
    if (VDegree - Mmax >= VContinuityRange) {
      usplitIndexes = new HArray1OfInteger (1, 2);
      usplitIndexes->SetValue (1, FirstVIndex);
      usplitIndexes->SetValue (2, LastVIndex);
    }
    else {
      Array1OfInteger VSplit (1, LastVIndex - FirstVIndex + 1);
      Standard_Integer NbVSplit  = 1;
      Standard_Integer VIndex    = FirstVIndex;
      VSplit (NbVSplit) = VIndex;
      VIndex++;
      NbVSplit++;
      while (VIndex < LastVIndex) {
        if (VDegree - VMults (VIndex) < VContinuityRange) {
          VSplit (NbVSplit) = VIndex;
          NbVSplit++;
        }
        VIndex++;
      }
      VSplit (NbVSplit) = VIndex;
      vsplitIndexes = new HArray1OfInteger (1, NbVSplit);
      for (i = 1; i <= NbVSplit; i++) {
        vsplitIndexes->SetValue (i, VSplit (i));
      }
    }
  }
}



Standard_Integer GeomConvert_BSplineSurfaceKnotSplitting::NbUSplits () const {
   return usplitIndexes->Length();
}


Standard_Integer GeomConvert_BSplineSurfaceKnotSplitting::NbVSplits () const {
   return vsplitIndexes->Length();
}


Standard_Integer GeomConvert_BSplineSurfaceKnotSplitting::USplitValue (

const Standard_Integer UIndex

) const {

  Standard_RangeError_Raise_if (
                      UIndex < 1 || UIndex > usplitIndexes->Length(), " ");
  return usplitIndexes->Value (UIndex);
}



Standard_Integer GeomConvert_BSplineSurfaceKnotSplitting::VSplitValue (

const Standard_Integer VIndex

) const {

  Standard_RangeError_Raise_if (
                      VIndex < 1 || VIndex > vsplitIndexes->Length(), " ");
  return vsplitIndexes->Value (VIndex);
}


void GeomConvert_BSplineSurfaceKnotSplitting::Splitting (

Array1OfInteger& USplit, 
Array1OfInteger& VSplit

) const {
  Standard_Integer i ;
  for ( i = 1; i <= usplitIndexes->Length(); i++){
    USplit (i) = usplitIndexes->Value (i);
  }
  for (i = 1; i <= vsplitIndexes->Length(); i++){
    VSplit (i) = vsplitIndexes->Value (i);
  }
}
