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

// Jean-Claude Vauthier 28 Novembre 1991
// Passage sur C1 Aout 1992

#include <BSplCLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert_BSplineSurfaceKnotSplitting.hxx>
#include <Standard_RangeError.hxx>

typedef NCollection_Array1<int>  Array1OfInteger;
typedef NCollection_HArray1<int> HArray1OfInteger;

GeomConvert_BSplineSurfaceKnotSplitting::GeomConvert_BSplineSurfaceKnotSplitting(

  const occ::handle<Geom_BSplineSurface>& BasisSurface,
  const int                               UContinuityRange,
  const int                               VContinuityRange

)
{

  if (UContinuityRange < 0 || VContinuityRange < 0)
  {
    throw Standard_RangeError();
  }

  int FirstUIndex = BasisSurface->FirstUKnotIndex();
  int LastUIndex  = BasisSurface->LastUKnotIndex();
  int FirstVIndex = BasisSurface->FirstVKnotIndex();
  int LastVIndex  = BasisSurface->LastVKnotIndex();
  int UDegree     = BasisSurface->UDegree();
  int VDegree     = BasisSurface->VDegree();
  int i;

  if (UContinuityRange == 0)
  {
    usplitIndexes = new HArray1OfInteger(1, 2);
    usplitIndexes->SetValue(1, FirstUIndex);
    usplitIndexes->SetValue(2, LastUIndex);
  }
  else
  {
    const Array1OfInteger& UMults = BasisSurface->UMultiplicities();
    int Mmax = BSplCLib::MaxKnotMult(UMults, FirstUIndex, LastUIndex);
    if (UDegree - Mmax >= UContinuityRange)
    {
      usplitIndexes = new HArray1OfInteger(1, 2);
      usplitIndexes->SetValue(1, FirstUIndex);
      usplitIndexes->SetValue(2, LastUIndex);
    }
    else
    {
      Array1OfInteger USplit(1, LastUIndex - FirstUIndex + 1);
      int             NbUSplit = 1;
      int             UIndex   = FirstUIndex;
      USplit(NbUSplit)         = UIndex;
      UIndex++;
      NbUSplit++;
      while (UIndex < LastUIndex)
      {
        if (UDegree - UMults(UIndex) < UContinuityRange)
        {
          USplit(NbUSplit) = UIndex;
          NbUSplit++;
        }
        UIndex++;
      }
      USplit(NbUSplit) = UIndex;
      usplitIndexes    = new HArray1OfInteger(1, NbUSplit);
      for (i = 1; i <= NbUSplit; i++)
      {
        usplitIndexes->SetValue(i, USplit(i));
      }
    }
  }

  if (VContinuityRange == 0)
  {
    vsplitIndexes = new HArray1OfInteger(1, 2);
    vsplitIndexes->SetValue(1, FirstVIndex);
    vsplitIndexes->SetValue(2, LastVIndex);
  }
  else
  {
    const Array1OfInteger& VMults = BasisSurface->VMultiplicities();
    int Mmax = BSplCLib::MaxKnotMult(VMults, FirstVIndex, LastVIndex);
    if (VDegree - Mmax >= VContinuityRange)
    {
      usplitIndexes = new HArray1OfInteger(1, 2);
      usplitIndexes->SetValue(1, FirstVIndex);
      usplitIndexes->SetValue(2, LastVIndex);
    }
    else
    {
      Array1OfInteger VSplit(1, LastVIndex - FirstVIndex + 1);
      int             NbVSplit = 1;
      int             VIndex   = FirstVIndex;
      VSplit(NbVSplit)         = VIndex;
      VIndex++;
      NbVSplit++;
      while (VIndex < LastVIndex)
      {
        if (VDegree - VMults(VIndex) < VContinuityRange)
        {
          VSplit(NbVSplit) = VIndex;
          NbVSplit++;
        }
        VIndex++;
      }
      VSplit(NbVSplit) = VIndex;
      vsplitIndexes    = new HArray1OfInteger(1, NbVSplit);
      for (i = 1; i <= NbVSplit; i++)
      {
        vsplitIndexes->SetValue(i, VSplit(i));
      }
    }
  }
}

int GeomConvert_BSplineSurfaceKnotSplitting::NbUSplits() const
{
  return usplitIndexes->Length();
}

int GeomConvert_BSplineSurfaceKnotSplitting::NbVSplits() const
{
  return vsplitIndexes->Length();
}

int GeomConvert_BSplineSurfaceKnotSplitting::USplitValue(

  const int UIndex

) const
{

  Standard_RangeError_Raise_if(UIndex < 1 || UIndex > usplitIndexes->Length(), " ");
  return usplitIndexes->Value(UIndex);
}

int GeomConvert_BSplineSurfaceKnotSplitting::VSplitValue(

  const int VIndex

) const
{

  Standard_RangeError_Raise_if(VIndex < 1 || VIndex > vsplitIndexes->Length(), " ");
  return vsplitIndexes->Value(VIndex);
}

void GeomConvert_BSplineSurfaceKnotSplitting::Splitting(

  Array1OfInteger& USplit,
  Array1OfInteger& VSplit

) const
{
  int i;
  for (i = 1; i <= usplitIndexes->Length(); i++)
  {
    USplit(i) = usplitIndexes->Value(i);
  }
  for (i = 1; i <= vsplitIndexes->Length(); i++)
  {
    VSplit(i) = vsplitIndexes->Value(i);
  }
}
