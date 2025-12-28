// Created on: 2004-11-23
// Created by: Pavel DURANDIN
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_TNameContainer_HeaderFile
#define TObj_TNameContainer_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TObj_SequenceOfObject.hxx>
#include <TDF_Attribute.hxx>

/**
 * This class provides OCAF Attribute to storing the unique names of object in
 * model.
 */

class TObj_TNameContainer : public TDF_Attribute
{
public:
  //! Standard methods of OCAF attribute

  //! Empty constructor
  Standard_EXPORT TObj_TNameContainer();

  //! This method is used in implementation of ID()
  static Standard_EXPORT const Standard_GUID& GetID();

  //! Returns the ID of TObj_TNameContainer attribute.
  Standard_EXPORT const Standard_GUID& ID() const override;

public:
  //! Method for create TObj_TNameContainer object

  //! Creates NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label> attribute on
  //! given label if not exist
  static Standard_EXPORT occ::handle<TObj_TNameContainer> Set(const TDF_Label& theLabel);

public:
  //! Methods for adding and removing names

  //! Records name with label attached
  Standard_EXPORT void RecordName(const occ::handle<TCollection_HExtendedString>& theName,
                                  const TDF_Label&                                theLabel);

  //! Remove name from the map
  Standard_EXPORT void RemoveName(const occ::handle<TCollection_HExtendedString>& theName);

  //! Return True is theName is registered in the Map
  Standard_EXPORT bool IsRegistered(const occ::handle<TCollection_HExtendedString>& theName) const;

  //! Remove all names registered in container
  Standard_EXPORT void Clear();

public:
  //! Methods for setting and obtaining TObj_TNameContainer

  //! Sets the NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label> object
  Standard_EXPORT void Set(
    const NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label>& theElem);

  //! Returns the NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label> object
  Standard_EXPORT const NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label>&
                        Get() const;

public:
  //! Redefined OCAF abstract methods

  //! Returns an new empty TObj_TNameContainer attribute. It is used by the
  //! copy algorithm.
  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  //! Restores the backuped contents from <theWith> into this one. It is used
  //! when aborting a transaction.
  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& theWith) override;

  //! This method is used when copying an attribute from a source structure
  //! into a target structure.
  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       theInto,
                             const occ::handle<TDF_RelocationTable>& theRT) const override;

private:
  //! Fields
  NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label>
    myMap; //!< The map of the names

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_TNameContainer, TDF_Attribute)
};

//! Define handle class for TObj_TObject
#endif

#ifdef _MSC_VER
#pragma once
#endif
