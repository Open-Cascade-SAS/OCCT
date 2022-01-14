// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
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

#ifndef _IGESDimen_DimensionedGeometry_HeaderFile
#define _IGESDimen_DimensionedGeometry_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESDimen_DimensionedGeometry;
DEFINE_STANDARD_HANDLE(IGESDimen_DimensionedGeometry, IGESData_IGESEntity)

//! Defines IGES Dimensioned Geometry, Type <402> Form <13>,
//! in package IGESDimen
//! This entity has been replaced by the new form of  Dimensioned
//! Geometry Associativity Entity (Type 402, Form 21) and should no
//! longer be used by preprocessors.
class IGESDimen_DimensionedGeometry : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESDimen_DimensionedGeometry();
  
  Standard_EXPORT void Init (const Standard_Integer nbDims, const Handle(IGESData_IGESEntity)& aDimension, const Handle(IGESData_HArray1OfIGESEntity)& entities);
  
  //! returns the number of dimensions
  Standard_EXPORT Standard_Integer NbDimensions() const;
  
  //! returns the number of associated geometry entities
  Standard_EXPORT Standard_Integer NbGeometryEntities() const;
  
  //! returns the Dimension entity
  Standard_EXPORT Handle(IGESData_IGESEntity) DimensionEntity() const;
  
  //! returns the num'th Geometry entity
  //! raises exception if Index <= 0 or Index > NbGeometryEntities()
  Standard_EXPORT Handle(IGESData_IGESEntity) GeometryEntity (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESDimen_DimensionedGeometry,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theNbDimensions;
  Handle(IGESData_IGESEntity) theDimension;
  Handle(IGESData_HArray1OfIGESEntity) theGeometryEntities;


};







#endif // _IGESDimen_DimensionedGeometry_HeaderFile
