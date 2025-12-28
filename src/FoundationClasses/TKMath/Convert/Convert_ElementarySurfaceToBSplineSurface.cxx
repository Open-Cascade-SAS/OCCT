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

//=================================================================================================

Convert_ElementarySurfaceToBSplineSurface::Convert_ElementarySurfaceToBSplineSurface(
  const int NbUPoles,
  const int NbVPoles,
  const int NbUKnots,
  const int NbVKnots,
  const int UDegree,
  const int VDegree)
    : poles(1, NbUPoles, 1, NbVPoles),
      weights(1, NbUPoles, 1, NbVPoles),
      uknots(1, NbUKnots),
      umults(1, NbUKnots),
      vknots(1, NbVKnots),
      vmults(1, NbVKnots),
      udegree(UDegree),
      vdegree(VDegree),
      nbUPoles(NbUPoles),
      nbVPoles(NbVPoles),
      nbUKnots(NbUKnots),
      nbVKnots(NbVKnots),
      isuperiodic(false),
      isvperiodic(false)

{
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UDegree() const
{
  return udegree;
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VDegree() const
{
  return vdegree;
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbUPoles() const
{
  return nbUPoles;
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbVPoles() const
{
  return nbVPoles;
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbUKnots() const
{
  return nbUKnots;
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::NbVKnots() const
{
  return nbVKnots;
}

//=================================================================================================

bool Convert_ElementarySurfaceToBSplineSurface::IsUPeriodic() const
{
  return isuperiodic;
}

//=================================================================================================

bool Convert_ElementarySurfaceToBSplineSurface::IsVPeriodic() const
{
  return isvperiodic;
}

//=================================================================================================

gp_Pnt Convert_ElementarySurfaceToBSplineSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUPoles || VIndex < 1 || VIndex > nbVPoles,
                               " ");
  return poles(UIndex, VIndex);
}

//=================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::Weight(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUPoles || VIndex < 1 || VIndex > nbVPoles,
                               " ");
  return weights(UIndex, VIndex);
}

//=================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUKnots, " ");
  return uknots(UIndex);
}

//=================================================================================================

double Convert_ElementarySurfaceToBSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > nbVKnots, " ");
  return vknots(VIndex);
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUKnots, " ");
  return umults(UIndex);
}

//=================================================================================================

int Convert_ElementarySurfaceToBSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > nbVKnots, " ");
  return vmults(VIndex);
}
