// Created on: 2002-10-29
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

#ifndef _BinDrivers_DocumentStorageDriver_HeaderFile
#define _BinDrivers_DocumentStorageDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_OStream.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>

class BinMDF_ADriverTable;
class Message_Messenger;
class BinLDrivers_DocumentSection;


class BinDrivers_DocumentStorageDriver;
DEFINE_STANDARD_HANDLE(BinDrivers_DocumentStorageDriver, BinLDrivers_DocumentStorageDriver)

//! persistent implementation of storage a document in a binary file
class BinDrivers_DocumentStorageDriver : public BinLDrivers_DocumentStorageDriver
{

public:

  
  //! Constructor
  Standard_EXPORT BinDrivers_DocumentStorageDriver();
  
  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
    (const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;
  
  //! implements the procedure of writing a shape section to file
  Standard_EXPORT virtual void WriteShapeSection
    (BinLDrivers_DocumentSection& theDocSection, 
     Standard_OStream& theOS, 
     const TDocStd_FormatVersion theDocVer,
     const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Return true if shape should be stored with triangles.
  Standard_EXPORT Standard_Boolean IsWithTriangles() const;
  //! Return true if shape should be stored with triangulation normals.
  Standard_EXPORT Standard_Boolean IsWithNormals() const;

  //! Set if triangulation should be stored or not.
  Standard_EXPORT void SetWithTriangles (const Handle(Message_Messenger)& theMessageDriver,
                                         const Standard_Boolean theWithTriangulation);
  //! Set if triangulation should be stored with normals or not.
  Standard_EXPORT void SetWithNormals(const Handle(Message_Messenger)& theMessageDriver,
                                         const Standard_Boolean theWithTriangulation);

  //! Enables writing in the quick part access mode.
  Standard_EXPORT void EnableQuickPartWriting(const Handle(Message_Messenger)& theMessageDriver,
                                              const Standard_Boolean theValue) Standard_OVERRIDE;

  //! Clears the NamedShape driver
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinDrivers_DocumentStorageDriver,BinLDrivers_DocumentStorageDriver)

};

#endif // _BinDrivers_DocumentStorageDriver_HeaderFile
