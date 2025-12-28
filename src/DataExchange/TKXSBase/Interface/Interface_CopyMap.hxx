// Created on: 1993-03-16
// Created by: Christian CAILLET
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

#ifndef _Interface_CopyMap_HeaderFile
#define _Interface_CopyMap_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <NCollection_Array1.hxx>
#include <Interface_CopyControl.hxx>
class Interface_InterfaceModel;
class Standard_Transient;

//! Manages a Map for the need of single Transfers, such as Copies
//! In such transfer, Starting Entities are read from a unique
//! Starting Model, and each transferred Entity is bound to one
//! and only one Result, which cannot be changed later.
class Interface_CopyMap : public Interface_CopyControl
{

public:
  //! Creates a CopyMap adapted to work from a Model
  Standard_EXPORT Interface_CopyMap(const occ::handle<Interface_InterfaceModel>& amodel);

  //! Clears Transfer List. Gets Ready to begin another Transfer
  Standard_EXPORT void Clear() override;

  //! Returns the InterfaceModel used at Creation time
  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  //! Binds a Starting Entity identified by its Number <num> in the
  //! Starting Model, to a Result of Transfer <res>
  Standard_EXPORT void Bind(const occ::handle<Standard_Transient>& ent,
                            const occ::handle<Standard_Transient>& res) override;

  //! Search for the result of a Starting Object (i.e. an Entity,
  //! identified by its Number <num> in the Starting Model)
  //! Returns True  if a  Result is Bound (and fills <res>)
  //! Returns False if no result is Bound (and nullifies <res>)
  Standard_EXPORT bool Search(const occ::handle<Standard_Transient>& ent,
                              occ::handle<Standard_Transient>&       res) const override;

  DEFINE_STANDARD_RTTIEXT(Interface_CopyMap, Interface_CopyControl)

private:
  occ::handle<Interface_InterfaceModel>               themod;
  NCollection_Array1<occ::handle<Standard_Transient>> theres;
};

#endif // _Interface_CopyMap_HeaderFile
