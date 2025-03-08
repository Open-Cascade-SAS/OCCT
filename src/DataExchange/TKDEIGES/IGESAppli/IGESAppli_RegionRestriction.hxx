// Created on: 1993-01-11
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

#ifndef _IGESAppli_RegionRestriction_HeaderFile
#define _IGESAppli_RegionRestriction_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>

class IGESAppli_RegionRestriction;
DEFINE_STANDARD_HANDLE(IGESAppli_RegionRestriction, IGESData_IGESEntity)

//! defines RegionRestriction, Type <406> Form <2>
//! in package IGESAppli
//! Defines regions to set an application's restriction
//! over a region.
class IGESAppli_RegionRestriction : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESAppli_RegionRestriction();

  //! This method is used to set the fields of the class
  //! RegionRestriction
  //! - nbPropVal  : Number of property values, always = 3
  //! - aViasRest  : Electrical Vias restriction
  //! - aCompoRest : Electrical components restriction
  //! - aCktRest   : Electrical circuitry restriction
  Standard_EXPORT void Init(const Standard_Integer nbPropVal,
                            const Standard_Integer aViasRest,
                            const Standard_Integer aCompoRest,
                            const Standard_Integer aCktRest);

  //! is always 3
  Standard_EXPORT Standard_Integer NbPropertyValues() const;

  //! returns the Electrical vias restriction
  //! is 0, 1 or 2
  Standard_EXPORT Standard_Integer ElectricalViasRestriction() const;

  //! returns the Electrical components restriction
  //! is 0, 1 or 2
  Standard_EXPORT Standard_Integer ElectricalComponentRestriction() const;

  //! returns the Electrical circuitry restriction
  //! is 0, 1 or 2
  Standard_EXPORT Standard_Integer ElectricalCktRestriction() const;

  DEFINE_STANDARD_RTTIEXT(IGESAppli_RegionRestriction, IGESData_IGESEntity)

protected:
private:
  Standard_Integer theNbPropertyValues;
  Standard_Integer theElectViasRestrict;
  Standard_Integer theElectCompRestrict;
  Standard_Integer theElectCktRestrict;
};

#endif // _IGESAppli_RegionRestriction_HeaderFile
