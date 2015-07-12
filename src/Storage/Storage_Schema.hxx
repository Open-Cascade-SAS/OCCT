// Created on: 1996-12-03
// Created by: Christophe LEYNADIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Storage_Schema_HeaderFile
#define _Storage_Schema_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Storage_MapOfCallBack.hxx>
#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <Storage_HArrayOfSchema.hxx>
#include <MMgt_TShared.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <Storage_SolveMode.hxx>
#include <Standard_CString.hxx>
class Storage_CallBack;
class Storage_StreamFormatError;
class TCollection_AsciiString;
class Storage_BaseDriver;
class Storage_Data;
class Storage_HeaderData;
class Storage_TypeData;
class Storage_RootData;
class Standard_Persistent;


class Storage_Schema;
DEFINE_STANDARD_HANDLE(Storage_Schema, MMgt_TShared)


//! Root class for basic storage/retrieval algorithms.
//! A Storage_Schema object processes:
//! -   writing of a set of persistent data into a
//! container (store mechanism),
//! -   reading of a container to extract all the
//! contained persistent data (retrieve mechanism).
//! A Storage_Schema object is based on the data
//! schema for the persistent data of the application, i.e.:
//! -   the list of all persistent objects which may be
//! known by the application,
//! -   the organization of their data; a data schema
//! knows how to browse each persistent object it contains.
//! During the store or retrieve operation, only
//! persistent objects known from the data schema
//! can be processed; they are then stored or
//! retrieved according to their description in the schema.
//! A data schema is specific to the object classes to
//! be read or written. Tools dedicated to the
//! environment in use allow a description of the
//! application persistent data structure.
//! Storage_Schema algorithms are called basic
//! because they do not support external references
//! between containers.
class Storage_Schema : public MMgt_TShared
{

public:

  
  //! Builds a storage/retrieval algorithm based on a
  //! given data schema.
  //! Example
  //! For example, if ShapeSchema is the class
  //! inheriting from Storage_Schema and containing
  //! the description of your application data schema,
  //! you create a storage/retrieval algorithm as follows:
  //! Handle(ShapeSchema) s = new
  //! ShapeSchema;
  //! -------- --
  //! USER API -- --------------------------------------------------------------
  //! -------- --
  Standard_EXPORT Storage_Schema();
  
  //! returns version of the schema
  Standard_EXPORT void SetVersion (const TCollection_AsciiString& aVersion);
  
  //! returns the version of the schema
  Standard_EXPORT TCollection_AsciiString Version() const;
  
  //! set the schema's name
  Standard_EXPORT void SetName (const TCollection_AsciiString& aSchemaName);
  
  //! returns the schema's name
  Standard_EXPORT TCollection_AsciiString Name() const;
  
  //! Writes the data aggregated in aData into the
  //! container defined by the driver s. The storage
  //! operation is performed according to the data
  //! schema with which this algorithm is working.
  //! Note: aData may aggregate several root objects
  //! to be stored together.
  Standard_EXPORT void Write (Storage_BaseDriver& s, const Handle(Storage_Data)& aData) const;
  
  //! Returns the data read from the container defined
  //! by the driver s. The retrieval operation is
  //! performed according to the data schema with
  //! which this algorithm is working.
  //! These data are aggregated in a Storage_Data
  //! object which may be browsed in order to extract
  //! the root objects from the container.
  Standard_EXPORT Handle(Storage_Data) Read (Storage_BaseDriver& s) const;
  
  //! read the header part of the stream
  //! Arguments:
  //! s: driver to read
  Standard_EXPORT Handle(Storage_HeaderData) ReadHeaderSection (Storage_BaseDriver& s) const;
  
  //! fill the TypeData with the  names of the type used
  //! in a stream
  //! Arguments:
  //! s: driver to read
  Standard_EXPORT Handle(Storage_TypeData) ReadTypeSection (Storage_BaseDriver& s) const;
  
  //! read root part of the file
  //! Arguments:
  //! s: driver to read
  Standard_EXPORT Handle(Storage_RootData) ReadRootSection (Storage_BaseDriver& s) const;
  
  //! returns the known types of a schema
  Standard_EXPORT virtual const TColStd_SequenceOfAsciiString& SchemaKnownTypes() const;
  
  //! indicates whether  the  are  types  in  the driver
  //! which are not known from  the schema and for which
  //! no callbacks have been set. The unknown types can
  //! be read in <theUnknownTypes>.
  Standard_EXPORT Standard_Boolean HasUnknownType (Storage_BaseDriver& aDriver, TColStd_SequenceOfAsciiString& theUnknownTypes) const;
  
  //! returns the all known types  of a schema and their
  //! nested schemes.
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) GetAllSchemaKnownTypes() const;
  
  Standard_EXPORT void SetNestedSchemas (const Handle(Storage_HArrayOfSchema)& theSchemas);
  
  Standard_EXPORT void ClearNestedSchemas();
  
  Standard_EXPORT Handle(Storage_HArrayOfSchema) NestedSchemas() const;
  
  //! return a current date string
  Standard_EXPORT static TCollection_AsciiString ICreationDate();
  
  //! returns True if theType migration is identified
  //! the callback support provides a way to read a file
  //! with a incomplete schema.
  //! ex. : A file contains 3 types a,b and c.
  //! The  application's  schema  contains  only 2
  //! type a and b. If you try to read the file in
  //! the application, you  will  have an error.To
  //! bypass this  problem  you  can  give to your
  //! application's schema  a  callback  used when
  //! the schema dosent  know  how  to handle this
  //! type.
  Standard_EXPORT static Standard_Boolean CheckTypeMigration (const TCollection_AsciiString& theTypeName, TCollection_AsciiString& theNewName);
  
  //! add two functions to the callback list
  Standard_EXPORT void AddReadUnknownTypeCallBack (const TCollection_AsciiString& aTypeName, const Handle(Storage_CallBack)& aCallBack);
  
  //! remove a callback for a type
  Standard_EXPORT void RemoveReadUnknownTypeCallBack (const TCollection_AsciiString& aTypeName);
  
  //! returns  a  list  of   type  name  with  installed
  //! callback.
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) InstalledCallBackList() const;
  
  //! clear all callback from schema instance.
  Standard_EXPORT void ClearCallBackList();
  
  //! install  a  callback  for  all  unknown  type. the
  //! objects with unknown types  will be skipped. (look
  //! SkipObject method in BaseDriver)
  Standard_EXPORT void UseDefaultCallBack();
  
  //! tells schema to uninstall the default callback.
  Standard_EXPORT void DontUseDefaultCallBack();
  
  //! ask if the schema is using the default callback.
  Standard_EXPORT Standard_Boolean IsUsingDefaultCallBack() const;
  
  //! overload the  default  function  for build.(use to
  //! set an  error  message  or  skip  an  object while
  //! reading an unknown type).
  Standard_EXPORT void SetDefaultCallBack (const Handle(Storage_CallBack)& f);
  
  //! reset  the  default  function  defined  by Storage
  //! package.
  Standard_EXPORT void ResetDefaultCallBack();
  
  //! returns   the   read   function   used   when  the
  //! UseDefaultCallBack() is set.
  Standard_EXPORT Handle(Storage_CallBack) DefaultCallBack() const;
  
  Standard_EXPORT virtual Handle(Storage_CallBack) CallBackSelection (const TCollection_AsciiString& tName) const;
  
  Standard_EXPORT virtual Handle(Storage_CallBack) AddTypeSelection (const Handle(Standard_Persistent)& sp) const;
  
    void WritePersistentObjectHeader (const Handle(Standard_Persistent)& sp, Storage_BaseDriver& s);
  
    void ReadPersistentObjectHeader (Storage_BaseDriver& s);
  
    void WritePersistentReference (const Handle(Standard_Persistent)& sp, Storage_BaseDriver& s);
  
  Standard_EXPORT void ReadPersistentReference (Handle(Standard_Persistent)& sp, Storage_BaseDriver& s);
  
  Standard_EXPORT Standard_Boolean AddPersistent (const Handle(Standard_Persistent)& sp, const Standard_CString tName) const;
  
  Standard_EXPORT Standard_Boolean PersistentToAdd (const Handle(Standard_Persistent)& sp) const;




  DEFINE_STANDARD_RTTI(Storage_Schema,MMgt_TShared)

protected:

  
  Standard_EXPORT Standard_Boolean IsNested() const;
  
  Standard_EXPORT Handle(Storage_CallBack) ResolveUnknownType (const TCollection_AsciiString& aTypeName, const Handle(Standard_Persistent)& aPers, const Storage_SolveMode aMode) const;
  
    Standard_Boolean HasTypeBinding (const TCollection_AsciiString& aTypeName) const;
  
  Standard_EXPORT void BindType (const TCollection_AsciiString& aTypeName, const Handle(Storage_CallBack)& aCallBack) const;
  
  Standard_EXPORT Handle(Storage_CallBack) TypeBinding (const TCollection_AsciiString& aTypeName) const;



private:

  
  Standard_EXPORT Standard_Boolean SetNested();
  
  Standard_EXPORT Standard_Boolean UnsetNested();
  
  Standard_EXPORT void Clear() const;
  
  Standard_EXPORT Standard_Boolean IReadHeaderSection (Storage_BaseDriver& s, const Handle(Storage_HeaderData)& iData) const;
  
  Standard_EXPORT Standard_Boolean IReadTypeSection (Storage_BaseDriver& s, const Handle(Storage_TypeData)& tData) const;
  
  Standard_EXPORT Standard_Boolean IReadRootSection (Storage_BaseDriver& s, const Handle(Storage_RootData)& rData) const;
  
  Standard_EXPORT static void ISetCurrentData (const Handle(Storage_Data)& dData);
  
  Standard_EXPORT static Handle(Storage_Data)& ICurrentData();

  Storage_MapOfCallBack myCallBack;
  Standard_Boolean myCallBackState;
  Handle(Storage_CallBack) myDefaultCallBack;
  TCollection_AsciiString myName;
  TCollection_AsciiString myVersion;
  Handle(Storage_HArrayOfSchema) myArrayOfSchema;
  Standard_Boolean myNestedState;


};


#include <Storage_Schema.lxx>





#endif // _Storage_Schema_HeaderFile
