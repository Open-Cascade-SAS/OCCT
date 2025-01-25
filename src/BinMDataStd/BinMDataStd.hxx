// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDataStd_HeaderFile
#define _BinMDataStd_HeaderFile

#include <Standard_GUID.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <TDocStd_FormatVersion.hxx>

class BinMDF_ADriverTable;
class Message_Messenger;

//! Storage and Retrieval drivers for modelling attributes.
class BinMDataStd
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds the attribute drivers to <theDriverTable>.
  Standard_EXPORT static void AddDrivers(const Handle(BinMDF_ADriverTable)& theDriverTable,
                                         const Handle(Message_Messenger)&   aMsgDrv);

  template <class T>
  static void SetAttributeID(const BinObjMgt_Persistent& theSource,
                             const Handle(T)&            anAtt,
                             const Standard_Integer      aDocFormatVersion)
  {
    Standard_Boolean ok = Standard_True;
    if (aDocFormatVersion >= TDocStd_FormatVersion_VERSION_10)
    { // process user defined guid
      const Standard_Integer& aPos = theSource.Position();
      Standard_GUID           aGuid;
      ok = theSource >> aGuid;
      if (!ok)
      {
        theSource.SetPosition(aPos);
        anAtt->SetID(T::GetID());
        ok = Standard_True;
      }
      else
      {
        anAtt->SetID(aGuid);
      }
    }
    else
      anAtt->SetID(T::GetID());
  }
};

#endif // _BinMDataStd_HeaderFile
