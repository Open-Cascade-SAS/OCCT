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

// JCV 16/10/91

#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <Standard_OutOfRange.hxx>

//==================================================================================================

void Convert_ElementarySurfaceToBSplineSurface::Finalize()
{
  // Trim oversized arrays down to actual sizes, preserving 2D element positions.
  if (myPoles.NbRows() != myNbUPoles || myPoles.NbColumns() != myNbVPoles)
  {
    myPoles.ResizeWithTrim(1, myNbUPoles, 1, myNbVPoles, true);
  }
  if (myWeights.NbRows() != myNbUPoles || myWeights.NbColumns() != myNbVPoles)
  {
    myWeights.ResizeWithTrim(1, myNbUPoles, 1, myNbVPoles, true);
  }
  if (myUKnots.Length() != myNbUKnots)
  {
    myUKnots.Resize(1, myNbUKnots, true);
  }
  if (myUMults.Length() != myNbUKnots)
  {
    myUMults.Resize(1, myNbUKnots, true);
  }
  if (myVKnots.Length() != myNbVKnots)
  {
    myVKnots.Resize(1, myNbVKnots, true);
  }
  if (myVMults.Length() != myNbVKnots)
  {
    myVMults.Resize(1, myNbVKnots, true);
  }
}

//==================================================================================================

Convert_ElementarySurfaceToBSplineSurface::Convert_ElementarySurfaceToBSplineSurface(
  const int theNbUPoles,
  const int theNbVPoles,
  const int theNbUKnots,
  const int theNbVKnots,
  const int theUDegree,
  const int theVDegree)
    : myPoles(1, theNbUPoles, 1, theNbVPoles),
      myWeights(1, theNbUPoles, 1, theNbVPoles),
      myUKnots(1, theNbUKnots),
      myVKnots(1, theNbVKnots),
      myUMults(1, theNbUKnots),
      myVMults(1, theNbVKnots),
      myUDegree(theUDegree),
      myVDegree(theVDegree),
      myNbUPoles(theNbUPoles),
      myNbVPoles(theNbVPoles),
      myNbUKnots(theNbUKnots),
      myNbVKnots(theNbVKnots)
{
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UDegree() const
{
  return myUDegree;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VDegree() const
{
  return myVDegree;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbUPoles() const
{
  return myNbUPoles;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbVPoles() const
{
  return myNbVPoles;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbUKnots() const
{
  return myNbUKnots;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbVKnots() const
{
  return myNbVKnots;
}

//==================================================================================================

bool Convert_ElementarySurfaceToBSplineSurface::IsUPeriodic() const
{
  return myIsUPeriodic;
}

//==================================================================================================

bool Convert_ElementarySurfaceToBSplineSurface::IsVPeriodic() const
{
  return myIsVPeriodic;
}

//==================================================================================================

Standard_DISABLE_DEPRECATION_WARNINGS gp_Pnt
  Convert_ElementarySurfaceToBSplineSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUPoles || VIndex < 1 || VIndex > myNbVPoles,
    "Convert_ElementarySurfaceToBSplineSurface::Pole: Index out of range");
  return myPoles(UIndex, VIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::Weight(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUPoles || VIndex < 1 || VIndex > myNbVPoles,
    "Convert_ElementarySurfaceToBSplineSurface::Weight: Index out of range");
  return myWeights(UIndex, VIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUKnots,
    "Convert_ElementarySurfaceToBSplineSurface::UKnot: Index out of range");
  return myUKnots(UIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    VIndex < 1 || VIndex > myNbVKnots,
    "Convert_ElementarySurfaceToBSplineSurface::VKnot: Index out of range");
  return myVKnots(VIndex);
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUKnots,
    "Convert_ElementarySurfaceToBSplineSurface::UMultiplicity: Index out of range");
  return myUMults(UIndex);
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    VIndex < 1 || VIndex > myNbVKnots,
    "Convert_ElementarySurfaceToBSplineSurface::VMultiplicity: Index out of range");
  return myVMults(VIndex);
}

Standard_ENABLE_DEPRECATION_WARNINGS

  //==================================================================================================

  const NCollection_Array2<gp_Pnt>&
        Convert_ElementarySurfaceToBSplineSurface::Poles() const
{
  return myPoles;
}

//==================================================================================================

const NCollection_Array2<double>& Convert_ElementarySurfaceToBSplineSurface::Weights() const
{
  return myWeights;
}

//==================================================================================================

const NCollection_Array1<double>& Convert_ElementarySurfaceToBSplineSurface::UKnots() const
{
  return myUKnots;
}

//==================================================================================================

const NCollection_Array1<double>& Convert_ElementarySurfaceToBSplineSurface::VKnots() const
{
  return myVKnots;
}

//==================================================================================================

const NCollection_Array1<int>& Convert_ElementarySurfaceToBSplineSurface::UMultiplicities() const
{
  return myUMults;
}

//==================================================================================================

const NCollection_Array1<int>& Convert_ElementarySurfaceToBSplineSurface::VMultiplicities() const
{
  return myVMults;
}
