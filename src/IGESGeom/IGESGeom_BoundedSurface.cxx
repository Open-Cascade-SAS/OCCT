// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESGeom_BoundedSurface.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESGeom_BoundedSurface::IGESGeom_BoundedSurface ()    {  }


    void  IGESGeom_BoundedSurface::Init
  (const Standard_Integer aType,
   const Handle(IGESData_IGESEntity)& aSurface,
   const Handle(IGESGeom_HArray1OfBoundary)& aBoundary)
{
  if (aBoundary->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESGeom_BoundedSurface : Init");
  theType       = aType;
  theSurface    = aSurface;
  theBoundaries = aBoundary;
  InitTypeAndForm(143,0);
}

    Standard_Integer  IGESGeom_BoundedSurface::RepresentationType () const
{
  return theType;
}

    Handle(IGESData_IGESEntity)  IGESGeom_BoundedSurface::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESGeom_BoundedSurface::NbBoundaries () const
{
  
  return ( theBoundaries.IsNull() ? 0 : theBoundaries->Length());
}

    Handle(IGESGeom_Boundary)  IGESGeom_BoundedSurface::Boundary
  (const Standard_Integer Index) const
{
  return theBoundaries->Value(Index);
}
