// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_Point_HeaderFile
#define _IGESGeom_Point_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class IGESBasic_SubfigureDef;
class gp_Pnt;

class IGESGeom_Point;
DEFINE_STANDARD_HANDLE(IGESGeom_Point, IGESData_IGESEntity)

//! defines IGESPoint, Type <116> Form <0>
//! in package IGESGeom
class IGESGeom_Point : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_Point();

  //! This method is used to set the fields of the class Point
  //! - aPoint  : Coordinates of point
  //! - aSymbol : SubfigureDefinition entity specifying the
  //! display symbol if there exists one, or zero
  Standard_EXPORT void Init(const gp_XYZ& aPoint, const Handle(IGESBasic_SubfigureDef)& aSymbol);

  //! returns coordinates of the point
  Standard_EXPORT gp_Pnt Value() const;

  //! returns coordinates of the point after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedValue() const;

  //! returns True if symbol exists
  Standard_EXPORT Standard_Boolean HasDisplaySymbol() const;

  //! returns display symbol entity if it exists
  Standard_EXPORT Handle(IGESBasic_SubfigureDef) DisplaySymbol() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_Point, IGESData_IGESEntity)

protected:
private:
  gp_XYZ                         thePoint;
  Handle(IGESBasic_SubfigureDef) theSymbol;
};

#endif // _IGESGeom_Point_HeaderFile
