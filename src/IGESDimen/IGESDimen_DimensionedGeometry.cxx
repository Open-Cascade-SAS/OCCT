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

#include <IGESDimen_DimensionedGeometry.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESDimen_DimensionedGeometry::IGESDimen_DimensionedGeometry ()    {  }


    void  IGESDimen_DimensionedGeometry::Init
  (const Standard_Integer nbDims, 
   const Handle(IGESData_IGESEntity)& aDimension, 
   const Handle(IGESData_HArray1OfIGESEntity)& entities)
{
  if (entities->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_DimensionedGeometry : Init");
  theNbDimensions     = nbDims;
  theDimension        = aDimension;
  theGeometryEntities = entities;
  InitTypeAndForm(402,13);
}


    Standard_Integer  IGESDimen_DimensionedGeometry::NbDimensions () const 
{
  return theNbDimensions;
}

    Standard_Integer  IGESDimen_DimensionedGeometry::NbGeometryEntities () const 
{
  return theGeometryEntities->Length();
}

    Handle(IGESData_IGESEntity)  IGESDimen_DimensionedGeometry::DimensionEntity
  () const
{
  return theDimension;
}

    Handle(IGESData_IGESEntity)  IGESDimen_DimensionedGeometry::GeometryEntity
  (const Standard_Integer num) const 
{
  return theGeometryEntities->Value(num);
}

