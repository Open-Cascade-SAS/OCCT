// Created on: 1992-12-09
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFSelect_SelectAnyList_HeaderFile
#define _IFSelect_SelectAnyList_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
#include <Standard_Integer.hxx>
class IFSelect_IntParam;
class Interface_EntityIterator;
class Standard_Transient;
class Interface_Graph;
class TCollection_AsciiString;

class IFSelect_SelectAnyList;
DEFINE_STANDARD_HANDLE(IFSelect_SelectAnyList, IFSelect_SelectDeduct)

//! A SelectAnyList kind Selection selects a List of an Entity, as
//! well as this Entity contains some. A List contains sub-entities
//! as one per Item, or several (for instance if an Entity binds
//! couples of sub-entities, each item is one of these couples).
//! Remark that only Entities are taken into account (neither
//! Reals, nor Strings, etc...)
//!
//! To define the list on which to work, SelectAnyList has two
//! deferred methods : NbItems (which gives the length of the
//! list), FillResult (which fills an EntityIterator). They are
//! intended to get a List in an Entity of the required Type (and
//! consider that list is empty if Entity has not required Type)
//!
//! In addition, remark that some types of Entity define more than
//! one list in each instance : a given sub-class of SelectAnyList
//! must be attached to one list
//!
//! SelectAnyList keeps or rejects a sub-set of the list,
//! that is the Items of which rank in the list is in a given
//! range (for instance form 2nd to 6th, etc...)
//! Range is defined by two Integer values. In order to allow
//! external control of them, these values are not directly
//! defined as fields, but accessed through IntParams, that is,
//! referenced as Transient (Handle) objects
//!
//! Warning : the Input can be any kind of Selection, BUT its
//! RootResult must have zero (empty) or one Entity maximum
class IFSelect_SelectAnyList : public IFSelect_SelectDeduct
{

public:
  //! Keeps Input Entity, as having required type. It works by
  //! keeping in <iter>, only suitable Entities (SelectType can be
  //! used). Called by RootResult (which waits for ONE ENTITY MAX)
  Standard_EXPORT virtual void KeepInputEntity(Interface_EntityIterator& iter) const = 0;

  //! Returns count of Items in the list in the Entity <ent>
  //! If <ent> has not required type, returned value must be Zero
  Standard_EXPORT virtual Standard_Integer NbItems(const Handle(Standard_Transient)& ent) const = 0;

  //! Sets a Range for numbers, with a lower and a upper limits
  Standard_EXPORT void SetRange(const Handle(IFSelect_IntParam)& rankfrom,
                                const Handle(IFSelect_IntParam)& rankto);

  //! Sets a unique number (only one Entity will be sorted as True)
  Standard_EXPORT void SetOne(const Handle(IFSelect_IntParam)& rank);

  //! Sets a Lower limit but no upper limit
  Standard_EXPORT void SetFrom(const Handle(IFSelect_IntParam)& rankfrom);

  //! Sets an Upper limit but no lower limit (equivalent to lower 1)
  Standard_EXPORT void SetUntil(const Handle(IFSelect_IntParam)& rankto);

  //! Returns True if a Lower limit is defined
  Standard_EXPORT Standard_Boolean HasLower() const;

  //! Returns Lower limit (if there is; else, value is senseless)
  Standard_EXPORT Handle(IFSelect_IntParam) Lower() const;

  //! Returns Integer Value of Lower Limit (0 if none)
  Standard_EXPORT Standard_Integer LowerValue() const;

  //! Returns True if a Lower limit is defined
  Standard_EXPORT Standard_Boolean HasUpper() const;

  //! Returns Upper limit (if there is; else, value is senseless)
  Standard_EXPORT Handle(IFSelect_IntParam) Upper() const;

  //! Returns Integer Value of Upper Limit (0 if none)
  Standard_EXPORT Standard_Integer UpperValue() const;

  //! Returns the list of selected entities (list of entities
  //! complying with rank criterium)
  //! Error if the input list has more than one Item
  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  //! Puts into <res>, the sub-entities of the list, from n1 to
  //! n2 included. Remark that adequation with Entity's type and
  //! length of list has already been made at this stage
  //! Called by RootResult
  Standard_EXPORT virtual void FillResult(const Standard_Integer            n1,
                                          const Standard_Integer            n2,
                                          const Handle(Standard_Transient)& ent,
                                          Interface_EntityIterator&         res) const = 0;

  //! Returns a text defining the criterium : "Components of List "
  //! then Specific List Label, then, following cases :
  //! " From .. Until .." or "From .." or "Until .." or "Rank no .."
  //! Specific type is given by deferred method ListLabel
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;

  //! Returns the specific label for the list, which is included as
  //! a part of Label
  Standard_EXPORT virtual TCollection_AsciiString ListLabel() const = 0;

  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectAnyList, IFSelect_SelectDeduct)

protected:
private:
  Handle(IFSelect_IntParam) thelower;
  Handle(IFSelect_IntParam) theupper;
};

#endif // _IFSelect_SelectAnyList_HeaderFile
