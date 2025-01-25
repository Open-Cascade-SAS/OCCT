// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Kiran)
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

#ifndef _IGESGeom_CompositeCurve_HeaderFile
#define _IGESGeom_CompositeCurve_HeaderFile

#include <Standard.hxx>

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>

class IGESGeom_CompositeCurve;
DEFINE_STANDARD_HANDLE(IGESGeom_CompositeCurve, IGESData_IGESEntity)

//! defines IGESCompositeCurve, Type <102> Form <0>
//! in package IGESGeom
//! A composite curve is defined as an ordered list of entities
//! consisting of a point, connect point and parametrised curve
//! entities (excluding the CompositeCurve entity).
class IGESGeom_CompositeCurve : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_CompositeCurve();

  //! This method is used to set the fields of the class
  //! CompositeCurve
  //! - allEntities : Constituent Entities of the composite curve
  Standard_EXPORT void Init(const Handle(IGESData_HArray1OfIGESEntity)& allEntities);

  //! returns the number of curves contained in the CompositeCurve
  Standard_EXPORT Standard_Integer NbCurves() const;

  //! returns Component of the CompositeCurve (a curve or a point)
  //! raises exception if Index <= 0 or Index > NbCurves()
  Standard_EXPORT Handle(IGESData_IGESEntity) Curve(const Standard_Integer Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_CompositeCurve, IGESData_IGESEntity)

protected:
private:
  Handle(IGESData_HArray1OfIGESEntity) theEntities;
};

#endif // _IGESGeom_CompositeCurve_HeaderFile
