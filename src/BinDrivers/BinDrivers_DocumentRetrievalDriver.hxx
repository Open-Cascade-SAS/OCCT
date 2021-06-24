// Created on: 2002-10-31
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

#ifndef _BinDrivers_DocumentRetrievalDriver_HeaderFile
#define _BinDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Boolean.hxx>
#include <Storage_Position.hxx>
#include <Standard_Integer.hxx>
class BinMDF_ADriverTable;
class Message_Messenger;
class BinLDrivers_DocumentSection;


class BinDrivers_DocumentRetrievalDriver;
DEFINE_STANDARD_HANDLE(BinDrivers_DocumentRetrievalDriver, BinLDrivers_DocumentRetrievalDriver)


class BinDrivers_DocumentRetrievalDriver : public BinLDrivers_DocumentRetrievalDriver
{

public:

  
  //! Constructor
  Standard_EXPORT BinDrivers_DocumentRetrievalDriver();
  
  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
    (const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadShapeSection
    (BinLDrivers_DocumentSection& theSection, 
     Standard_IStream& theIS,
     const Standard_Boolean isMess = Standard_False, 
     const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void CheckShapeSection
    (const Storage_Position& thePos, Standard_IStream& theIS) Standard_OVERRIDE;

  //! Clears the NamedShape driver
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Enables reading in the quick part access mode.
  Standard_EXPORT virtual void EnableQuickPartReading
    (const Handle(Message_Messenger)& theMessageDriver, Standard_Boolean theValue) Standard_OVERRIDE;


  DEFINE_STANDARD_RTTIEXT(BinDrivers_DocumentRetrievalDriver,BinLDrivers_DocumentRetrievalDriver)

};

#endif // _BinDrivers_DocumentRetrievalDriver_HeaderFile
