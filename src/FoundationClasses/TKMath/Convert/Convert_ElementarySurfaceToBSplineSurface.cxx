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
  const Standard_Integer NbUPoles,
  const Standard_Integer NbVPoles,
  const Standard_Integer NbUKnots,
  const Standard_Integer NbVKnots,
  const Standard_Integer UDegree,
  const Standard_Integer VDegree)
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
      isuperiodic(Standard_False),
      isvperiodic(Standard_False)

{
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::UDegree() const
{
  return udegree;
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::VDegree() const
{
  return vdegree;
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbUPoles() const
{
  return nbUPoles;
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbVPoles() const
{
  return nbVPoles;
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbUKnots() const
{
  return nbUKnots;
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbVKnots() const
{
  return nbVKnots;
}

//=================================================================================================

Standard_Boolean Convert_ElementarySurfaceToBSplineSurface::IsUPeriodic() const
{
  return isuperiodic;
}

//=================================================================================================

Standard_Boolean Convert_ElementarySurfaceToBSplineSurface::IsVPeriodic() const
{
  return isvperiodic;
}

//=================================================================================================

gp_Pnt Convert_ElementarySurfaceToBSplineSurface::Pole(const Standard_Integer UIndex,
                                                       const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUPoles || VIndex < 1 || VIndex > nbVPoles,
                               " ");
  return poles(UIndex, VIndex);
}

//=================================================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::Weight(const Standard_Integer UIndex,
                                                                const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUPoles || VIndex < 1 || VIndex > nbVPoles,
                               " ");
  return weights(UIndex, VIndex);
}

//=================================================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::UKnot(const Standard_Integer UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUKnots, " ");
  return uknots(UIndex);
}

//=================================================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::VKnot(const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > nbVKnots, " ");
  return vknots(VIndex);
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::UMultiplicity(
  const Standard_Integer UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > nbUKnots, " ");
  return umults(UIndex);
}

//=================================================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::VMultiplicity(
  const Standard_Integer VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > nbVKnots, " ");
  return vmults(VIndex);
}
