// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESDimen_NewDimensionedGeometry.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_NewDimensionedGeometry,IGESData_IGESEntity)

IGESDimen_NewDimensionedGeometry::IGESDimen_NewDimensionedGeometry ()   { }


    void  IGESDimen_NewDimensionedGeometry::Init
  (const Standard_Integer nbDimens,
   const Handle(IGESData_IGESEntity)& aDimen,
   const Standard_Integer anOrientation, const Standard_Real anAngle,
   const Handle(IGESData_HArray1OfIGESEntity)& allEntities,
   const Handle(TColStd_HArray1OfInteger)& allLocations,
   const Handle(TColgp_HArray1OfXYZ)& allPoints)
{
  Standard_Integer num = allEntities->Length();
  if (allEntities->Lower()  != 1 ||
      allLocations->Lower() != 1 || allLocations->Length() != num ||
      allPoints->Lower()    != 1 || allPoints->Length()    != num )
    throw Standard_DimensionMismatch("IGESDimen_NewDimensionedGeometry: Init");
  theNbDimensions             = nbDimens;
  theDimensionEntity          = aDimen;
  theDimensionOrientationFlag = anOrientation;
  theAngleValue               = anAngle;
  theGeometryEntities         = allEntities;
  theDimensionLocationFlags   = allLocations;
  thePoints                   = allPoints;
  InitTypeAndForm(402,21);
}


    Standard_Integer  IGESDimen_NewDimensionedGeometry::NbDimensions () const
{
  return theNbDimensions;
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::NbGeometries () const
{
  return theGeometryEntities->Length();
}

    Handle(IGESData_IGESEntity)  IGESDimen_NewDimensionedGeometry::DimensionEntity
  ()const
{
  return theDimensionEntity;
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::DimensionOrientationFlag
  () const
{
  return theDimensionOrientationFlag;
}

    Standard_Real  IGESDimen_NewDimensionedGeometry::AngleValue () const
{
  return theAngleValue;
}

    Handle(IGESData_IGESEntity)  IGESDimen_NewDimensionedGeometry::GeometryEntity
  (const Standard_Integer Index) const
{
  return theGeometryEntities->Value(Index);
}

    Standard_Integer  IGESDimen_NewDimensionedGeometry::DimensionLocationFlag
  (const Standard_Integer Index) const
{
  return theDimensionLocationFlags->Value(Index);
}

    gp_Pnt  IGESDimen_NewDimensionedGeometry::Point
  (const Standard_Integer Index) const
{
  return gp_Pnt(thePoints->Value(Index));
}

    gp_Pnt  IGESDimen_NewDimensionedGeometry::TransformedPoint
  (const Standard_Integer Index) const
{
  gp_XYZ point = thePoints->Value(Index);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
