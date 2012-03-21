// Copyright (c) 1995-1999 Matra Datavision
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

//JCV 16/10/91



#include <Convert_ElementarySurfaceToBSplineSurface.ixx>

#include <Standard_OutOfRange.hxx>


//=======================================================================
//function : Convert_ElementarySurfaceToBSplineSurface
//purpose  : 
//=======================================================================

Convert_ElementarySurfaceToBSplineSurface::
Convert_ElementarySurfaceToBSplineSurface 
  (const Standard_Integer NbUPoles,
   const Standard_Integer NbVPoles,
   const Standard_Integer NbUKnots,
   const Standard_Integer NbVKnots,
   const Standard_Integer UDegree,
   const Standard_Integer VDegree) : 
  poles  (1, NbUPoles, 1, NbVPoles), weights (1, NbUPoles, 1, NbVPoles),
  uknots (1, NbUKnots), umults (1, NbUKnots),
  vknots (1, NbVKnots), vmults (1, NbVKnots), 
  udegree  (UDegree),  vdegree  (VDegree),
  nbUPoles (NbUPoles), nbVPoles (NbVPoles),
  nbUKnots (NbUKnots), nbVKnots (NbVKnots)

{ }
   

//=======================================================================
//function : UDegree
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::UDegree () const 
{
  return udegree;
}


//=======================================================================
//function : VDegree
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::VDegree () const 
{
  return vdegree;
}


//=======================================================================
//function : NbUPoles
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbUPoles () const 
{
  return nbUPoles;
}


//=======================================================================
//function : NbVPoles
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbVPoles () const 
{
  return nbVPoles;
}


//=======================================================================
//function : NbUKnots
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbUKnots () const 
{
  return nbUKnots;
}


//=======================================================================
//function : NbVKnots
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::NbVKnots () const 
{
  return nbVKnots;
}


//=======================================================================
//function : IsUPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Convert_ElementarySurfaceToBSplineSurface::IsUPeriodic()
const 
{
  return isuperiodic;
}

//=======================================================================
//function : IsVPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Convert_ElementarySurfaceToBSplineSurface::IsVPeriodic()
const 
{
  return isvperiodic;
}


//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

gp_Pnt Convert_ElementarySurfaceToBSplineSurface::Pole 
  (const Standard_Integer UIndex, 
   const Standard_Integer VIndex ) const 
{
   Standard_OutOfRange_Raise_if (
                       UIndex < 1 || UIndex > nbUPoles ||
                       VIndex < 1 || VIndex > nbVPoles, " ");
   return poles (UIndex, VIndex);
}


//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::Weight 
  (const Standard_Integer UIndex,
   const Standard_Integer VIndex ) const 
{
   Standard_OutOfRange_Raise_if (
                       UIndex < 1 || UIndex > nbUPoles ||
                       VIndex < 1 || VIndex > nbVPoles," ");
   return weights (UIndex, VIndex);
}



//=======================================================================
//function : UKnot
//purpose  : 
//=======================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::UKnot 
  (const Standard_Integer UIndex) const 
{
  Standard_OutOfRange_Raise_if (UIndex < 1 || UIndex > nbUKnots, " ");
  return uknots (UIndex);
}


//=======================================================================
//function : VKnot
//purpose  : 
//=======================================================================

Standard_Real Convert_ElementarySurfaceToBSplineSurface::VKnot 
  (const Standard_Integer VIndex) const 
{
  Standard_OutOfRange_Raise_if (VIndex < 1 || VIndex > nbVKnots, " ");
  return vknots (VIndex);
}


//=======================================================================
//function : UMultiplicity
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::UMultiplicity 
  (const Standard_Integer UIndex) const 
{
  Standard_OutOfRange_Raise_if (UIndex < 1 || UIndex > nbUKnots, " ");
  return umults (UIndex);
}


//=======================================================================
//function : VMultiplicity
//purpose  : 
//=======================================================================

Standard_Integer Convert_ElementarySurfaceToBSplineSurface::VMultiplicity 
  (const Standard_Integer VIndex) const 
{
  Standard_OutOfRange_Raise_if (VIndex < 1 || VIndex > nbVKnots, " ");
  return vmults (VIndex);
}
