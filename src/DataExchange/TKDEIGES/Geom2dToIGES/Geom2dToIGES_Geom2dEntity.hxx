// Created on: 1995-09-13
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Geom2dToIGES_Geom2dEntity_HeaderFile
#define _Geom2dToIGES_Geom2dEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class IGESData_IGESModel;

//! provides methods to transfer Geom2d entity from CASCADE to IGES.
class Geom2dToIGES_Geom2dEntity
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a tool Geom2dEntity
  Standard_EXPORT Geom2dToIGES_Geom2dEntity();

  //! Creates a tool ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT Geom2dToIGES_Geom2dEntity(const Geom2dToIGES_Geom2dEntity& GE);

  //! Set the value of "TheModel"
  Standard_EXPORT void SetModel(const Handle(IGESData_IGESModel)& model);

  //! Returns the value of "TheModel"
  Standard_EXPORT Handle(IGESData_IGESModel) GetModel() const;

  //! Sets the value of the UnitFlag
  Standard_EXPORT void SetUnit(const Standard_Real unit);

  //! Returns the value of the UnitFlag of the header of the model
  //! in millimeters.
  Standard_EXPORT Standard_Real GetUnit() const;

protected:
private:
  Handle(IGESData_IGESModel) TheModel;
  Standard_Real              TheUnitFactor;
};

#endif // _Geom2dToIGES_Geom2dEntity_HeaderFile
