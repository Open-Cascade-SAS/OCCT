// Created on: 1997-02-06
// Created by: Kernel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Storage_Data_HeaderFile
#define _Storage_Data_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_Error.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>
#include <Standard_Integer.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
class Storage_HeaderData;
class Storage_RootData;
class Storage_TypeData;
class Storage_InternalData;
class Storage_Schema;
class TCollection_AsciiString;
class TCollection_ExtendedString;
class Standard_Persistent;
class Storage_Root;


class Storage_Data;
DEFINE_STANDARD_HANDLE(Storage_Data, Standard_Transient)

//! A picture memorizing the data stored in a
//! container (for example, in a file).
//! A Storage_Data object represents either:
//! -   persistent data to be written into a container,
//! or
//! -   persistent data which are read from a container.
//! A Storage_Data object is used in both the
//! storage and retrieval operations:
//! -   Storage mechanism: create an empty
//! Storage_Data object, then add successively
//! persistent objects (roots) to be stored using
//! the function AddRoot. When the set of data is
//! complete, write it to a container using the
//! function Write in your Storage_Schema
//! storage/retrieval algorithm.
//! -   Retrieval mechanism: a Storage_Data
//! object is returned by the Read function from
//! your Storage_Schema storage/retrieval
//! algorithm. Use the functions NumberOfRoots
//! and Roots to find the roots which were stored
//! in the read container.
//! The roots of a Storage_Data object may share
//! references on objects. The shared internal
//! references of a Storage_Data object are
//! maintained by the storage/retrieval mechanism.
//! Note: References shared by objects which are
//! contained in two distinct Storage_Data objects
//! are not maintained by the storage/retrieval
//! mechanism: external references are not
//! supported by Storage_Schema algorithm
class Storage_Data : public Standard_Transient
{

public:

  

  //! Creates an empty set of data.
  //! You explicitly create a Storage_Data object
  //! when preparing the set of objects to be stored
  //! together in a container (for example, in a file).
  //! Then use the function AddRoot to add
  //! persistent objects to the set of data.
  //! A Storage_Data object is also returned by the
  //! Read function of a Storage_Schema
  //! storage/retrieval algorithm. Use the functions
  //! NumberOfRoots and Roots to find the roots
  //! which were stored in the read container.
  Standard_EXPORT Storage_Data();
  
  //! Returns Storage_VSOk if
  //! -   the last storage operation performed with the
  //! function Read, or
  //! -   the last retrieval operation performed with the function Write
  //! by a Storage_Schema algorithm, on this set of data was successful.
  //! If the storage or retrieval operation was not
  //! performed, the returned error status indicates the
  //! reason why the operation failed. The algorithm
  //! stops its analysis at the first detected error
  Standard_EXPORT Storage_Error ErrorStatus() const;
  

  //! Clears the error status positioned either by:
  //! -   the last storage operation performed with the
  //! Read function, or
  //! -   the last retrieval operation performed with the Write function
  //! by a Storage_Schema algorithm, on this set of data.
  //! This error status may be read by the function ErrorStatus.
  Standard_EXPORT void ClearErrorStatus();
  
  Standard_EXPORT TCollection_AsciiString ErrorStatusExtension() const;
  
  //! return the creation date
  Standard_EXPORT TCollection_AsciiString CreationDate() const;
  
  //! return the Storage package version
  Standard_EXPORT TCollection_AsciiString StorageVersion() const;
  
  //! get the version of the schema
  Standard_EXPORT TCollection_AsciiString SchemaVersion() const;
  
  //! get the schema's name
  Standard_EXPORT TCollection_AsciiString SchemaName() const;
  
  //! set the version of the application
  Standard_EXPORT void SetApplicationVersion (const TCollection_AsciiString& aVersion);
  
  //! get the version of the application
  Standard_EXPORT TCollection_AsciiString ApplicationVersion() const;
  
  //! set the name of the application
  Standard_EXPORT void SetApplicationName (const TCollection_ExtendedString& aName);
  
  //! get the name of the application
  Standard_EXPORT TCollection_ExtendedString ApplicationName() const;
  
  //! set the data type
  Standard_EXPORT void SetDataType (const TCollection_ExtendedString& aType);
  
  //! returns data type
  Standard_EXPORT TCollection_ExtendedString DataType() const;
  
  //! add <theUserInfo> to the user informations
  Standard_EXPORT void AddToUserInfo (const TCollection_AsciiString& anInfo);
  
  //! return the user informations
  Standard_EXPORT const TColStd_SequenceOfAsciiString& UserInfo() const;
  
  //! add <theUserInfo> to the user informations
  Standard_EXPORT void AddToComments (const TCollection_ExtendedString& aComment);
  
  //! return the user informations
  Standard_EXPORT const TColStd_SequenceOfExtendedString& Comments() const;
  
  //! the the number of persistent objects
  //! Return:
  //! the number of persistent objects readed
  Standard_EXPORT Standard_Integer NumberOfObjects() const;
  
  //! Returns the number of root objects in this set of data.
  //! -   When preparing a storage operation, the
  //! result is the number of roots inserted into this
  //! set of data with the function AddRoot.
  //! -   When retrieving an object, the result is the
  //! number of roots stored in the read container.
  //! Use the Roots function to get these roots in a sequence.
  Standard_EXPORT Standard_Integer NumberOfRoots() const;
  
  //! add a persistent root to write. the name of the root
  //! is a driver reference number.
  Standard_EXPORT void AddRoot (const Handle(Standard_Persistent)& anObject) const;
  
  //! Adds the root anObject to this set of data.
  //! The name of the root is aName if given; if not, it
  //! will be a reference number assigned by the driver
  //! when writing the set of data into the container.
  //! When naming the roots, it is easier to retrieve
  //! objects by significant references rather than by
  //! references without any semantic values.
  Standard_EXPORT void AddRoot (const TCollection_AsciiString& aName, const Handle(Standard_Persistent)& anObject) const;
  
  //! Removes from this set of data the root object named aName.
  //! Warning
  //! Nothing is done if there is no root object whose
  //! name is aName in this set of data.
  Standard_EXPORT void RemoveRoot (const TCollection_AsciiString& aName);
  
  //! Returns the roots of this set of data in a sequence.
  //! -   When preparing a storage operation, the
  //! sequence contains the roots inserted into this
  //! set of data with the function AddRoot.
  //! -   When retrieving an object, the sequence
  //! contains the roots stored in the container read.
  //! -   An empty sequence is returned if there is no root in this set of data.
  Standard_EXPORT Handle(Storage_HSeqOfRoot) Roots() const;
  
  //! Gives the root object whose name is aName in
  //! this set of data. The returned object is a
  //! Storage_Root object, from which the object it
  //! encapsulates may be extracted.
  //! Warning
  //! A null handle is returned if there is no root object
  //! whose name is aName in this set of data.
  Standard_EXPORT Handle(Storage_Root) Find (const TCollection_AsciiString& aName) const;
  
  //! returns Standard_True if <me> contains a root named <aName>
  Standard_EXPORT Standard_Boolean IsRoot (const TCollection_AsciiString& aName) const;
  
  //! Returns the number of types of objects used in this set of data.
  Standard_EXPORT Standard_Integer NumberOfTypes() const;
  
  //! Returns true if this set of data contains an object of type aName.
  //! Persistent objects from this set of data must
  //! have types which are recognized by the
  //! Storage_Schema algorithm used to store or retrieve them.
  Standard_EXPORT Standard_Boolean IsType (const TCollection_AsciiString& aName) const;
  

  //! Gives the list of types of objects used in this set of data in a sequence.
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) Types() const;


friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_Data,Standard_Transient)

 
  Standard_EXPORT Handle(Storage_HeaderData) HeaderData() const;
  
  Standard_EXPORT Handle(Storage_RootData) RootData() const;
  
  Standard_EXPORT Handle(Storage_TypeData) TypeData() const;
  
  Standard_EXPORT Handle(Storage_InternalData) InternalData() const;
  
  Standard_EXPORT void Clear() const;

private:  

  Standard_EXPORT void SetErrorStatus (const Storage_Error anError);
  
  Standard_EXPORT void SetErrorStatusExtension (const TCollection_AsciiString& anErrorExt);

  Handle(Storage_HeaderData) myHeaderData;
  Handle(Storage_RootData) myRootData;
  Handle(Storage_TypeData) myTypeData;
  Handle(Storage_InternalData) myInternal;
  Storage_Error myErrorStatus;
  TCollection_AsciiString myErrorStatusExt;


};







#endif // _Storage_Data_HeaderFile
