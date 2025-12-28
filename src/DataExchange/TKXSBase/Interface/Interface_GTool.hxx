// Created on: 1998-01-08
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Interface_GTool_HeaderFile
#define _Interface_GTool_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_GeneralLib.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
class Interface_Protocol;
class Interface_SignType;
class Interface_InterfaceModel;
class Interface_GeneralModule;

//! GTool - General Tool for a Model
//! Provides the functions performed by Protocol/GeneralModule for
//! entities of a Model, and recorded in a GeneralLib
//! Optimized : once an entity has been queried, the GeneralLib is
//! not longer queried
//! Shareable between several users : as a Handle
class Interface_GTool : public Standard_Transient
{

public:
  //! Creates an empty, not set, GTool
  Standard_EXPORT Interface_GTool();

  //! Creates a GTool from a Protocol
  //! Optional starting count of entities
  Standard_EXPORT Interface_GTool(const occ::handle<Interface_Protocol>& proto,
                                  const int                              nbent = 0);

  //! Sets a new SignType
  Standard_EXPORT void SetSignType(const occ::handle<Interface_SignType>& sign);

  //! Returns the SignType. Can be null
  Standard_EXPORT occ::handle<Interface_SignType> SignType() const;

  //! Returns the Signature for a Transient Object in a Model
  //! It calls SignType to do that
  //! If SignType is not defined, return ClassName of <ent>
  Standard_EXPORT const char* SignValue(const occ::handle<Standard_Transient>&       ent,
                                        const occ::handle<Interface_InterfaceModel>& model) const;

  //! Returns the Name of the SignType, or "Class Name"
  Standard_EXPORT const char* SignName() const;

  //! Sets a new Protocol
  //! if <enforce> is False and the new Protocol equates the old one
  //! then nothing is done
  Standard_EXPORT void SetProtocol(const occ::handle<Interface_Protocol>& proto,
                                   const bool                             enforce = false);

  //! Returns the Protocol. Warning: it can be Null
  Standard_EXPORT occ::handle<Interface_Protocol> Protocol() const;

  //! Returns the GeneralLib itself
  Standard_EXPORT Interface_GeneralLib& Lib();

  //! Reservates maps for a count of entities
  //! <enforce> False : minimum count
  //! <enforce> True  : clears former reservations
  //! Does not clear the maps
  Standard_EXPORT void Reservate(const int nb, const bool enforce = false);

  //! Clears the maps which record, for each already recorded entity
  //! its Module and Case Number
  Standard_EXPORT void ClearEntities();

  //! Selects for an entity, its Module and Case Number
  //! It is optimised : once done for each entity, the result is
  //! mapped and the GeneralLib is not longer queried
  //! <enforce> True overpasses this optimisation
  Standard_EXPORT bool Select(const occ::handle<Standard_Transient>& ent,
                              occ::handle<Interface_GeneralModule>&  gmod,
                              int&                                   CN,
                              const bool                             enforce = false);

  DEFINE_STANDARD_RTTIEXT(Interface_GTool, Standard_Transient)

private:
  occ::handle<Interface_Protocol>                           theproto;
  occ::handle<Interface_SignType>                           thesign;
  Interface_GeneralLib                                      thelib;
  NCollection_DataMap<occ::handle<Standard_Transient>, int> thentnum;
  NCollection_IndexedDataMap<occ::handle<Standard_Transient>, occ::handle<Standard_Transient>>
    thentmod;
};

#endif // _Interface_GTool_HeaderFile
