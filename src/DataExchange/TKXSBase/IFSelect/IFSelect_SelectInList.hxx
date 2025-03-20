// Created on: 1993-01-07
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

#ifndef _IFSelect_SelectInList_HeaderFile
#define _IFSelect_SelectInList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectAnyList.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_EntityIterator;

class IFSelect_SelectInList;
DEFINE_STANDARD_HANDLE(IFSelect_SelectInList, IFSelect_SelectAnyList)

//! A SelectInList kind Selection selects a List of an Entity,
//! which is composed of single Entities
//! To know the list on which to work, SelectInList has two
//! deferred methods : NbItems (inherited from SelectAnyList) and
//! ListedEntity (which gives an item as an Entity) which must be
//! defined to get a List in an Entity of the required Type (and
//! consider that list is empty if Entity has not required Type)
//!
//! As for SelectAnyList, if a type of Entity defines several
//! lists, a given sub-class of SelectInList is attached on one
class IFSelect_SelectInList : public IFSelect_SelectAnyList
{

public:
  //! Returns an Entity, given its rank in the list
  Standard_EXPORT virtual Handle(Standard_Transient) ListedEntity(
    const Standard_Integer            num,
    const Handle(Standard_Transient)& ent) const = 0;

  //! Puts into the result, the sub-entities of the list, from n1 to
  //! n2 included. Remark that adequation with Entity's type and
  //! length of list has already been made at this stage
  //! Called by RootResult; calls ListedEntity (see below)
  Standard_EXPORT void FillResult(const Standard_Integer            n1,
                                  const Standard_Integer            n2,
                                  const Handle(Standard_Transient)& ent,
                                  Interface_EntityIterator&         result) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectInList, IFSelect_SelectAnyList)

protected:
private:
};

#endif // _IFSelect_SelectInList_HeaderFile
