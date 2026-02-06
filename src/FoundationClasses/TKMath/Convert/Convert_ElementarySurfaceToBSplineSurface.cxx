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
  // Resize poles array if dimensions don't match actual sizes.
  if (myData.Poles.NbRows() != myNbUPoles || myData.Poles.NbColumns() != myNbVPoles)
  {
    NCollection_Array2<gp_Pnt> aNewPoles(1, myNbUPoles, 1, myNbVPoles);
    for (int i = 1; i <= myNbUPoles; i++)
    {
      for (int j = 1; j <= myNbVPoles; j++)
      {
        aNewPoles(i, j) = myData.Poles(i, j);
      }
    }
    myData.Poles = std::move(aNewPoles);
  }

  // Resize weights array if dimensions don't match actual sizes.
  if (myData.Weights.NbRows() != myNbUPoles || myData.Weights.NbColumns() != myNbVPoles)
  {
    NCollection_Array2<double> aNewWeights(1, myNbUPoles, 1, myNbVPoles);
    for (int i = 1; i <= myNbUPoles; i++)
    {
      for (int j = 1; j <= myNbVPoles; j++)
      {
        aNewWeights(i, j) = myData.Weights(i, j);
      }
    }
    myData.Weights = std::move(aNewWeights);
  }

  // Resize U-knots array if length doesn't match actual size.
  if (myData.UKnots.Length() != myNbUKnots)
  {
    NCollection_Array1<double> aNewUKnots(1, myNbUKnots);
    for (int i = 1; i <= myNbUKnots; i++)
    {
      aNewUKnots(i) = myData.UKnots(i);
    }
    myData.UKnots = std::move(aNewUKnots);
  }

  // Resize U-multiplicities array if length doesn't match actual size.
  if (myData.UMults.Length() != myNbUKnots)
  {
    NCollection_Array1<int> aNewUMults(1, myNbUKnots);
    for (int i = 1; i <= myNbUKnots; i++)
    {
      aNewUMults(i) = myData.UMults(i);
    }
    myData.UMults = std::move(aNewUMults);
  }

  // Resize V-knots array if length doesn't match actual size.
  if (myData.VKnots.Length() != myNbVKnots)
  {
    NCollection_Array1<double> aNewVKnots(1, myNbVKnots);
    for (int i = 1; i <= myNbVKnots; i++)
    {
      aNewVKnots(i) = myData.VKnots(i);
    }
    myData.VKnots = std::move(aNewVKnots);
  }

  // Resize V-multiplicities array if length doesn't match actual size.
  if (myData.VMults.Length() != myNbVKnots)
  {
    NCollection_Array1<int> aNewVMults(1, myNbVKnots);
    for (int i = 1; i <= myNbVKnots; i++)
    {
      aNewVMults(i) = myData.VMults(i);
    }
    myData.VMults = std::move(aNewVMults);
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
    : myNbUPoles(theNbUPoles),
      myNbVPoles(theNbVPoles),
      myNbUKnots(theNbUKnots),
      myNbVKnots(theNbVKnots)
{
  myData.Poles   = NCollection_Array2<gp_Pnt>(1, theNbUPoles, 1, theNbVPoles);
  myData.Weights = NCollection_Array2<double>(1, theNbUPoles, 1, theNbVPoles);
  myData.UKnots  = NCollection_Array1<double>(1, theNbUKnots);
  myData.UMults  = NCollection_Array1<int>(1, theNbUKnots);
  myData.VKnots  = NCollection_Array1<double>(1, theNbVKnots);
  myData.VMults  = NCollection_Array1<int>(1, theNbVKnots);
  myData.UDegree     = theUDegree;
  myData.VDegree     = theVDegree;
  myData.IsUPeriodic = false;
  myData.IsVPeriodic = false;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UDegree() const
{
  return myData.UDegree;
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VDegree() const
{
  return myData.VDegree;
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
  return myData.IsUPeriodic;
}

//==================================================================================================

bool Convert_ElementarySurfaceToBSplineSurface::IsVPeriodic() const
{
  return myData.IsVPeriodic;
}

//==================================================================================================

Standard_DISABLE_DEPRECATION_WARNINGS
gp_Pnt Convert_ElementarySurfaceToBSplineSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUPoles || VIndex < 1 || VIndex > myNbVPoles,
    "Convert_ElementarySurfaceToBSplineSurface::Pole: Index out of range");
  return myData.Poles(UIndex, VIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::Weight(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUPoles || VIndex < 1 || VIndex > myNbVPoles,
    "Convert_ElementarySurfaceToBSplineSurface::Weight: Index out of range");
  return myData.Weights(UIndex, VIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUKnots,
    "Convert_ElementarySurfaceToBSplineSurface::UKnot: Index out of range");
  return myData.UKnots(UIndex);
}

//==================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    VIndex < 1 || VIndex > myNbVKnots,
    "Convert_ElementarySurfaceToBSplineSurface::VKnot: Index out of range");
  return myData.VKnots(VIndex);
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(
    UIndex < 1 || UIndex > myNbUKnots,
    "Convert_ElementarySurfaceToBSplineSurface::UMultiplicity: Index out of range");
  return myData.UMults(UIndex);
}

//==================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(
    VIndex < 1 || VIndex > myNbVKnots,
    "Convert_ElementarySurfaceToBSplineSurface::VMultiplicity: Index out of range");
  return myData.VMults(VIndex);
}
Standard_ENABLE_DEPRECATION_WARNINGS

//==================================================================================================

const NCollection_Array2<gp_Pnt>& Convert_ElementarySurfaceToBSplineSurface::Poles() const
{
  return myData.Poles;
}

//==================================================================================================

const NCollection_Array2<double>& Convert_ElementarySurfaceToBSplineSurface::Weights() const
{
  return myData.Weights;
}

//==================================================================================================

const NCollection_Array1<double>& Convert_ElementarySurfaceToBSplineSurface::UKnots() const
{
  return myData.UKnots;
}

//==================================================================================================

const NCollection_Array1<double>& Convert_ElementarySurfaceToBSplineSurface::VKnots() const
{
  return myData.VKnots;
}

//==================================================================================================

const NCollection_Array1<int>& Convert_ElementarySurfaceToBSplineSurface::UMultiplicities() const
{
  return myData.UMults;
}

//==================================================================================================

const NCollection_Array1<int>& Convert_ElementarySurfaceToBSplineSurface::VMultiplicities() const
{
  return myData.VMults;
}
