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

#ifndef _IGESBasic_SubfigureDef_HeaderFile
#define _IGESBasic_SubfigureDef_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class TCollection_HAsciiString;
class Standard_Transient;

//! defines SubfigureDef, Type <308> Form <0>
//! in package IGESBasic
//! This Entity permits a single definition of a detail to
//! be utilized in multiple instances in the creation of
//! the whole picture
class IGESBasic_SubfigureDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESBasic_SubfigureDef();

  //! This method is used to set the fields of the class
  //! SubfigureDef
  //! - aDepth           : It indicates the amount of nesting
  //! - aName            : the subfigure name
  //! - allAssocEntities : the associated entities
  Standard_EXPORT void Init(
    const int                                                                 aDepth,
    const occ::handle<TCollection_HAsciiString>&                              aName,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allAssocEntities);

  //! returns depth of the Subfigure
  //! if theDepth = 0 - No reference to any subfigure instance.
  Standard_EXPORT int Depth() const;

  //! returns the name of Subfigure
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Name() const;

  //! returns number of entities. Is greater than or equal to zero.
  Standard_EXPORT int NbEntities() const;

  //! returns the specific entity as indicated by Index
  //! raises exception if Index <= 0 or Index > NbEntities()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> AssociatedEntity(const int Index) const;

  //! returns the specific entity as indicated by Index
  //! raises exception if Index <= 0 or Index > NbEntities()
  Standard_EXPORT occ::handle<Standard_Transient> Value(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESBasic_SubfigureDef, IGESData_IGESEntity)

private:
  int                                                                theDepth;
  occ::handle<TCollection_HAsciiString>                              theName;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> theAssocEntities;
};

#endif // _IGESBasic_SubfigureDef_HeaderFile
