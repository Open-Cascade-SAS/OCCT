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
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>

//! Defines IGES Dimensioned Geometry, Type <402> Form <13>,
//! in package IGESDimen
//! This entity has been replaced by the new form of Dimensioned
//! Geometry Associativity Entity (Type 402, Form 21) and should no
//! longer be used by preprocessors.
class IGESDimen_DimensionedGeometry : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_DimensionedGeometry();

  Standard_EXPORT void Init(const int                      nbDims,
                            const occ::handle<IGESData_IGESEntity>&          aDimension,
                            const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& entities);

  //! returns the number of dimensions
  Standard_EXPORT int NbDimensions() const;

  //! returns the number of associated geometry entities
  Standard_EXPORT int NbGeometryEntities() const;

  //! returns the Dimension entity
  Standard_EXPORT occ::handle<IGESData_IGESEntity> DimensionEntity() const;

  //! returns the num'th Geometry entity
  //! raises exception if Index <= 0 or Index > NbGeometryEntities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> GeometryEntity(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_DimensionedGeometry, IGESData_IGESEntity)

private:
  int                     theNbDimensions;
  occ::handle<IGESData_IGESEntity>          theDimension;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> theGeometryEntities;
};

#endif // _IGESDimen_DimensionedGeometry_HeaderFile
