// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Anand NATRAJAN)
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

#ifndef _IGESDimen_NewDimensionedGeometry_HeaderFile
#define _IGESDimen_NewDimensionedGeometry_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HArray1OfXYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESDimen_NewDimensionedGeometry;
DEFINE_STANDARD_HANDLE(IGESDimen_NewDimensionedGeometry, IGESData_IGESEntity)

//! defines New Dimensioned Geometry, Type <402>, Form <21>
//! in package IGESDimen
//! Links a dimension entity with the geometry entities it
//! is dimensioning, so that later, in the receiving
//! database, the dimension can be automatically recalculated
//! and redrawn should the geometry be changed.
class IGESDimen_NewDimensionedGeometry : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_NewDimensionedGeometry();
  
  //! This method is used to set the fields of the class
  //! NewDimensionedGeometry
  //! - nbDimen       : Number of Dimensions, default = 1
  //! - aDimen        : Dimension Entity
  //! - anOrientation : Dimension Orientation Flag
  //! - anAngle       : Angle Value
  //! - allEntities   : Geometric Entities
  //! - allLocations  : Dimension Location Flags
  //! - allPoints     : Points on the Geometry Entities
  //! exception raised if lengths of entities, locations, points
  //! are not the same
  Standard_EXPORT void Init (const Standard_Integer nbDimens, const Handle(IGESData_IGESEntity)& aDimen, const Standard_Integer anOrientation, const Standard_Real anAngle, const Handle(IGESData_HArray1OfIGESEntity)& allEntities, const Handle(TColStd_HArray1OfInteger)& allLocations, const Handle(TColgp_HArray1OfXYZ)& allPoints);
  
  //! returns the number of dimensions
  Standard_EXPORT Standard_Integer NbDimensions() const;
  
  //! returns the number of associated geometry entities
  Standard_EXPORT Standard_Integer NbGeometries() const;
  
  //! returns the dimension entity
  Standard_EXPORT Handle(IGESData_IGESEntity) DimensionEntity() const;
  
  //! returns the dimension orientation flag
  Standard_EXPORT Standard_Integer DimensionOrientationFlag() const;
  
  //! returns the angle value
  Standard_EXPORT Standard_Real AngleValue() const;
  
  //! returns the Index'th geometry entity
  //! raises exception if Index <= 0 or Index > NbGeometries()
  Standard_EXPORT Handle(IGESData_IGESEntity) GeometryEntity (const Standard_Integer Index) const;
  
  //! returns the Index'th geometry entity's dimension location flag
  //! raises exception if Index <= 0 or Index > NbGeometries()
  Standard_EXPORT Standard_Integer DimensionLocationFlag (const Standard_Integer Index) const;
  
  //! coordinate of point on Index'th geometry entity
  //! raises exception if Index <= 0 or Index > NbGeometries()
  Standard_EXPORT gp_Pnt Point (const Standard_Integer Index) const;
  
  //! coordinate of point on Index'th geometry entity after Transformation
  //! raises exception if Index <= 0 or Index > NbGeometries()
  Standard_EXPORT gp_Pnt TransformedPoint (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_NewDimensionedGeometry,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbDimensions;
  Handle(IGESData_IGESEntity) theDimensionEntity;
  Standard_Integer theDimensionOrientationFlag;
  Standard_Real theAngleValue;
  Handle(IGESData_HArray1OfIGESEntity) theGeometryEntities;
  Handle(TColStd_HArray1OfInteger) theDimensionLocationFlags;
  Handle(TColgp_HArray1OfXYZ) thePoints;


};







#endif // _IGESDimen_NewDimensionedGeometry_HeaderFile
