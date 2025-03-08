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

#ifndef _IGESDimen_BasicDimension_HeaderFile
#define _IGESDimen_BasicDimension_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt2d;

class IGESDimen_BasicDimension;
DEFINE_STANDARD_HANDLE(IGESDimen_BasicDimension, IGESData_IGESEntity)

//! Defines IGES Basic Dimension, Type 406, Form 31,
//! in package IGESDimen
//! The basic Dimension Property indicates that the referencing
//! dimension entity is to be displayed with a box around text.
class IGESDimen_BasicDimension : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_BasicDimension();

  Standard_EXPORT void Init(const Standard_Integer nbPropVal,
                            const gp_XY&           lowerLeft,
                            const gp_XY&           lowerRight,
                            const gp_XY&           upperRight,
                            const gp_XY&           upperLeft);

  //! returns the number of properties = 8
  Standard_EXPORT Standard_Integer NbPropertyValues() const;

  //! returns coordinates of lower left corner
  Standard_EXPORT gp_Pnt2d LowerLeft() const;

  //! returns coordinates of lower right corner
  Standard_EXPORT gp_Pnt2d LowerRight() const;

  //! returns coordinates of upper right corner
  Standard_EXPORT gp_Pnt2d UpperRight() const;

  //! returns coordinates of upper left corner
  Standard_EXPORT gp_Pnt2d UpperLeft() const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_BasicDimension, IGESData_IGESEntity)

protected:
private:
  Standard_Integer theNbPropertyValues;
  gp_XY            theLowerLeft;
  gp_XY            theLowerRight;
  gp_XY            theUpperRight;
  gp_XY            theUpperLeft;
};

#endif // _IGESDimen_BasicDimension_HeaderFile
