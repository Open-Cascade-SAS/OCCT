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


#include <Standard_ErrorHandler.hxx>
#include <Standard_Persistent.hxx>
#include <Standard_Type.hxx>
#include <Storage.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_BucketOfPersistent.hxx>
#include <Storage_CallBack.hxx>
#include <Storage_Data.hxx>
#include <Storage_DataMapIteratorOfMapOfCallBack.hxx>
#include <Storage_DefaultCallBack.hxx>
#include <Storage_HArrayOfCallBack.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_HPArray.hxx>
#include <Storage_HSeqOfRoot.hxx>
#include <Storage_InternalData.hxx>
#include <Storage_Root.hxx>
#include <Storage_RootData.hxx>
#include <Storage_Schema.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamModeError.hxx>
#include <Storage_StreamReadError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamUnknownTypeError.hxx>
#include <Storage_StreamWriteError.hxx>
#include <Storage_TypeData.hxx>
#include <Storage_TypedCallBack.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_MapOfAsciiString.hxx>

#include <locale.h>
#include <stdio.h>
#define DATATYPE_MIGRATION

#ifdef DATATYPE_MIGRATION
#include <NCollection_DataMap.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_Environment.hxx>

typedef NCollection_DataMap <TCollection_AsciiString, 
  TCollection_AsciiString> DataMapOfAStringAString;

#endif

// IMPLEMENTATION BucketOfPersistent
//
Storage_Bucket::~Storage_Bucket()
{
  Standard::Free (mySpace);
  mySpace = 0L;
  mySpaceSize = 0;
  Clear();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Storage_Bucket::Clear()
{
  myCurrentSpace = -1;
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void Storage_Bucket::Append(Standard_Persistent *sp)
{
  myCurrentSpace++;
  mySpace[myCurrentSpace] = sp;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Persistent* Storage_Bucket::Value
                         (const Standard_Integer theIndex) const
{
  return mySpace[theIndex];
}

//=======================================================================
//function : Storage_BucketOfPersistent
//purpose  : 
//=======================================================================

Storage_BucketOfPersistent::Storage_BucketOfPersistent
                         (const Standard_Integer theBucketSize,
                          const Standard_Integer theBucketNumber)
: myNumberOfBucket(1),myNumberOfBucketAllocated(theBucketNumber),myBucketSize
                         (theBucketSize)
{
  myBuckets =  (Storage_Bucket**)Standard::Allocate
                         (sizeof(Storage_Bucket*) * theBucketNumber);
  myBuckets[0] = new Storage_Bucket(myBucketSize);
  myCurrentBucket = myBuckets[0];
  myLength = 0;
  myCurrentBucketNumber = 0;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Storage_BucketOfPersistent::Clear()
{
  if (myBuckets) {
    Standard_Integer i;

    for (i = 1; i < myNumberOfBucket; i++) delete myBuckets[i];
    myNumberOfBucket = 1;
    myCurrentBucket = myBuckets[0];
    myCurrentBucket->Clear();
    myCurrentBucketNumber = 0;
    myLength = 0;
  }
}

Storage_BucketOfPersistent::~Storage_BucketOfPersistent()
{
  Clear();
  delete myBuckets[0];
  Standard::Free (myBuckets);
  myBuckets = 0L;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Persistent* Storage_BucketOfPersistent::Value
                         (const Standard_Integer theIndex)
{
  Standard_Integer theInd,theCurrentBucketNumber,tecurrentind = theIndex - 1;
  theCurrentBucketNumber = tecurrentind / myBucketSize;
  theInd = tecurrentind - (myBucketSize * theCurrentBucketNumber);

  return myBuckets[theCurrentBucketNumber]->mySpace[theInd];

}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================

void Storage_BucketOfPersistent::Append(const Handle(Standard_Persistent)& sp)
{
  myCurrentBucket->myCurrentSpace++;

  if (myCurrentBucket->myCurrentSpace != myBucketSize) {
    myLength++;
    myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.operator->();
    return;
  }

  myCurrentBucket->myCurrentSpace--;
  myNumberOfBucket++;
  myCurrentBucketNumber++;

  if (myNumberOfBucket > myNumberOfBucketAllocated) {
    Standard_Size e = sizeof(Storage_Bucket*) * myNumberOfBucketAllocated;
    myBuckets =  (Storage_Bucket**)Standard::Reallocate(myBuckets, e * 2);
    myNumberOfBucketAllocated *= 2;
  }

  myBuckets[myCurrentBucketNumber] = new Storage_Bucket(myBucketSize);
  myCurrentBucket = myBuckets[myCurrentBucketNumber];
  myCurrentBucket->myCurrentSpace++;
  myLength++;
  myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.operator->();
}

//=======================================================================
//function : Storage_BucketIterator
//purpose  : 
//=======================================================================

Storage_BucketIterator::Storage_BucketIterator
                         (Storage_BucketOfPersistent* aBucketManager)
{
  if (aBucketManager) {
    myBucket             = aBucketManager;
    myCurrentBucket      = myBucket->myBuckets[0];
    myBucketNumber       = aBucketManager->myNumberOfBucket;
    myCurrentBucketIndex = 0;
    myCurrentIndex       = 0;
    myMoreObject         = Standard_True;
  }
  else myMoreObject         = Standard_False;
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void Storage_BucketIterator::Reset()
{
  if (myBucket) {
    myCurrentBucket = myBucket->myBuckets[0];
    myBucketNumber  = myBucket->myNumberOfBucket;
    myCurrentIndex  = 0;
    myCurrentBucketIndex = 0;
    myMoreObject = Standard_True;
  }
  else myMoreObject         = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Storage_BucketIterator::Init(Storage_BucketOfPersistent* aBucketManager)
{
  if (aBucketManager) {
    myBucket        = aBucketManager;
    myCurrentBucket = myBucket->myBuckets[0];
    myBucketNumber  = aBucketManager->myNumberOfBucket;
    myCurrentIndex  = 0;
    myCurrentBucketIndex = 0;
    myMoreObject = Standard_True;
  }
  else myMoreObject         = Standard_False;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void Storage_BucketIterator::Next()
{
  if (!myMoreObject) return;

  if (myCurrentIndex < myCurrentBucket->myCurrentSpace) {
    myCurrentIndex++;
  }
  else {
    myCurrentIndex = 0;
    myCurrentBucketIndex++;
    if (myCurrentBucketIndex < myBucketNumber) {
      myCurrentBucket = myBucket->myBuckets[myCurrentBucketIndex];
    }
    else {
      myMoreObject = Standard_False;
    }
  }
}

//=======================================================================
//function : Storage_Schema
//purpose  : USER API -- --------------------------------------------------------------
//           IMPLEMENTATION BucketOfPersistent
//=======================================================================

Storage_Schema::Storage_Schema()
{
  Clear();
  ResetDefaultCallBack();
  myCallBackState = Standard_False;
  myNestedState = Standard_False;
}

//=======================================================================
//function : SetVersion
//purpose  : returns version of the schema
//=======================================================================

void Storage_Schema::SetVersion(const TCollection_AsciiString& aVersion)
{
  myVersion = aVersion;
}

//=======================================================================
//function : Version
//purpose  : returns the version of the schema
//=======================================================================

TCollection_AsciiString Storage_Schema::Version() const
{
  return myVersion;
}

//=======================================================================
//function : SetName
//purpose  : set the schema's name
//=======================================================================

void Storage_Schema::SetName(const TCollection_AsciiString& aSchemaName)
{
  myName = aSchemaName;
}

//=======================================================================
//function : Name
//purpose  : returns the schema's name
//=======================================================================

TCollection_AsciiString Storage_Schema::Name() const
{
  return myName;
}

//=======================================================================
//function : Write
//purpose  : write 
//Arguments:
//           s: driver to write
//           raises  if  the  stream  is  not  opened  in  VSWrite  or
//           VSReadWrite
//=======================================================================

void Storage_Schema::Write
                         (Storage_BaseDriver& f,
                          const Handle(Storage_Data)& aData) const
{
 if (aData.IsNull()) return;

  // add all the persistent to write...
  //
  Standard_Integer                 posfrom,posto;
  Handle(Standard_Persistent)      p;
  Handle(Storage_HSeqOfRoot)       plist;
  TCollection_AsciiString          errorContext("AddPersistent");
  Storage_Schema::ISetCurrentData(aData);

  Handle(Storage_InternalData) iData = aData->InternalData();

  aData->Clear();
  aData->ClearErrorStatus();

  plist = aData->Roots();

  for (posto = 1; posto <= plist->Length(); posto++) {
    PersistentToAdd(plist->Value(posto)->Object());
  }

  for (posto = 1; posto <= plist->Length(); posto++) {
    AddTypeSelection(plist->Value(posto)->Object());
  }

  for (posfrom = plist->Length() + 1; posfrom <= iData->myPtoA.Length(); posfrom++) {
    AddTypeSelection(iData->myPtoA.Value(posfrom));
  }

  // ...and now we write
  //
  Standard_Integer            i,
                              len;

 aData->HeaderData()->SetCreationDate(ICreationDate());
 aData->HeaderData()->SetStorageVersion(Storage::Version());
 aData->HeaderData()->SetNumberOfObjects(iData->myPtoA.Length());
 aData->HeaderData()->SetSchemaName(myName);
 aData->HeaderData()->SetSchemaVersion(myVersion);

  if ((f.OpenMode() == Storage_VSWrite) || (f.OpenMode() == Storage_VSReadWrite)) {
    try {
      OCC_CATCH_SIGNALS
      errorContext = "BeginWriteInfoSection";
      f.BeginWriteInfoSection();
      errorContext = "WriteInfo";
      f.WriteInfo(aData->NumberOfObjects(),
                  aData->StorageVersion(),
                  aData->CreationDate(),
                  aData->SchemaName(),
                  aData->SchemaVersion(),
                  aData->ApplicationName(),
                  aData->ApplicationVersion(),
                  aData->DataType(),
                  aData->UserInfo());
      errorContext = "EndWriteInfoSection";
      f.EndWriteInfoSection();

      errorContext = "BeginWriteCommentSection";
      f.BeginWriteCommentSection();
      errorContext = "WriteComment";
      f.WriteComment(aData->Comments());
      errorContext = "EndWriteCommentSection";
      f.EndWriteCommentSection();

      Handle(TColStd_HSequenceOfAsciiString) tlist;

      tlist = aData->Types();

      errorContext = "BeginWriteTypeSection";
      f.BeginWriteTypeSection();
      len = aData->NumberOfTypes();

      Handle(Storage_HArrayOfCallBack) WFunc = new Storage_HArrayOfCallBack(1,len);

      f.SetTypeSectionSize(len);

      Storage_DataMapIteratorOfMapOfCallBack cbit(iData->myTypeBinding);
      Handle(Storage_TypedCallBack) atcallBack;

      for (; cbit.More(); cbit.Next()) {
        atcallBack = cbit.Value();
        WFunc->SetValue(atcallBack->Index(),atcallBack->CallBack());
      }

      errorContext = "WriteTypeInformations";
      for (i = 1; i <= len; i++) {
        f.WriteTypeInformations(i,tlist->Value(i).ToCString());
      }

      errorContext = "EndWriteTypeSection";
      f.EndWriteTypeSection();

      errorContext = "BeginWriteRootSection";
      f.BeginWriteRootSection();
      f.SetRootSectionSize(plist->Length());

      errorContext = "WriteRoot";
      for (i = 1; i <= plist->Length(); i++) {
        f.WriteRoot(plist->Value(i)->Name(),i,plist->Value(i)->Type());
      }

      errorContext = "EndWriteRootSection";
      f.EndWriteRootSection();

      errorContext = "BeginWriteRefSection";
      f.BeginWriteRefSection();
      f.SetRefSectionSize(iData->myObjId - 1);
      errorContext = "WriteReferenceType";

      Storage_BucketIterator bit(&iData->myPtoA);

      while(bit.More()) {
        p = bit.Value();
        if (!p.IsNull()) f.WriteReferenceType(p->_refnum,p->_typenum);
        bit.Next();
      }

      errorContext = "EndWriteRefSection";
      f.EndWriteRefSection();

      errorContext = "BeginWriteDataSection";
      f.BeginWriteDataSection();

      Handle(Storage_Schema) me = this;

      errorContext = "Write";

      bit.Reset();

      while(bit.More()) {
        p = bit.Value();
        if (!p.IsNull()) {
          WFunc->Value(p->_typenum)->Write(p,f,me);
          p->_typenum = 0;
        }
        bit.Next();
      }

      errorContext = "EndWriteDataSection";
      f.EndWriteDataSection();
    }
    catch(Storage_StreamWriteError) {
      aData->SetErrorStatus(Storage_VSWriteError);
      aData->SetErrorStatusExtension(errorContext);
    }
  }
  else {
    aData->SetErrorStatus(Storage_VSModeError);
    aData->SetErrorStatusExtension("OpenMode");
  }

  iData->Clear();
  Clear();
}

//=======================================================================
//function : Read
//purpose  : ...and read a Storage file
//Arguments:
//           s: driver to read
//=======================================================================

Handle(Storage_Data) Storage_Schema::Read(Storage_BaseDriver& f) const
{
  Handle(Storage_Data)          dData = new Storage_Data;
  Storage_Error                errorCode;
static Standard_Boolean             result;
static Standard_Integer             len;
static Standard_Integer             i;
  i = 0 ;
  Handle(Standard_Persistent)  per;
  Handle(Storage_HArrayOfCallBack) theCallBack;

  Handle(Storage_InternalData) iData = dData->InternalData();
  Handle(Storage_TypeData)     tData = dData->TypeData();
  Handle(Storage_RootData)     rData = dData->RootData();
  Handle(Storage_HeaderData)   hData = dData->HeaderData();

  if ((f.OpenMode() == Storage_VSRead) || (f.OpenMode() == Storage_VSReadWrite)) {

    Storage_Schema::ISetCurrentData(dData);

    // IReadHeaderSection can set an error status
    //
    result = IReadHeaderSection(f,hData);

    if (result) {
      Handle(Storage_CallBack) accallBack;
      Standard_Integer            p;
      TCollection_AsciiString     typeName;

      iData->myReadArray = new Storage_HPArray(1,dData->NumberOfObjects());

      // IReadTypeSection can set an error status
      //
      result = IReadTypeSection(f,tData);

      if (result) {
        len = dData->NumberOfTypes();
        theCallBack = new Storage_HArrayOfCallBack(1,len);
        {
          try {
            OCC_CATCH_SIGNALS
            for (i = 1; i <= len; i++) {
              typeName = tData->Type(i);
              p = tData->Type(typeName);
              theCallBack->SetValue(p,CallBackSelection(typeName));
            }
          }
          catch(Storage_StreamUnknownTypeError) {
            result = Standard_False;
            dData->SetErrorStatus(Storage_VSUnknownType);
            dData->SetErrorStatusExtension(typeName);
          }
        }
      }
      else {
        dData->SetErrorStatus(tData->ErrorStatus());
        dData->SetErrorStatusExtension(tData->ErrorStatusExtension());
      }
    }
    else {
      dData->SetErrorStatus(hData->ErrorStatus());
      dData->SetErrorStatusExtension(hData->ErrorStatusExtension());
    }

    if (result) {
      result = IReadRootSection(f,rData);
      dData->SetErrorStatus(rData->ErrorStatus());
      if (!result) dData->SetErrorStatusExtension(rData->ErrorStatusExtension());
    }

    if (result) {
      Standard_Integer otype, oref = 0;

      errorCode = f.BeginReadRefSection();

      if (errorCode == Storage_VSOk) {
        {
          try {
            OCC_CATCH_SIGNALS
            len = f.RefSectionSize();

            for (i = 1; i <= len; i++) {
              f.ReadReferenceType(oref,otype);
              iData->myReadArray->ChangeValue(oref) = theCallBack->Value(otype)->New();
              if (!iData->myReadArray->ChangeValue(oref).IsNull()) iData->myReadArray->ChangeValue(oref)->_typenum = otype;
            }
          }
          catch(Storage_StreamTypeMismatchError) {
            TCollection_AsciiString aOref = oref;
            result = Standard_False;
            dData->SetErrorStatus(Storage_VSTypeMismatch);
            dData->SetErrorStatusExtension(aOref);
          }
        }

        if (result) {
          errorCode = f.EndReadRefSection();
          result = (errorCode == Storage_VSOk);
          dData->SetErrorStatus(errorCode);
          if (!result) dData->SetErrorStatusExtension("EndReadRefSection");
        }
      }
      else {
        result = Standard_False;
        dData->SetErrorStatus(errorCode);
        dData->SetErrorStatusExtension("BeginReadRefSection");
      }
    }

    if (result) {
      errorCode = f.BeginReadDataSection();
      result = (errorCode == Storage_VSOk);
      dData->SetErrorStatus(errorCode);
      if (!result) dData->SetErrorStatusExtension("BeginReadDataSection");
    }

    if (result) {
      Handle(Storage_Schema) me = this;
      Handle(Storage_CallBack) rcback;

      {
        try {
          OCC_CATCH_SIGNALS
          for (i = 1; i <= dData->NumberOfObjects(); i++) {
            Handle(Standard_Persistent) pobj = iData->myReadArray->Value(i);
            if (!pobj.IsNull()) {
              rcback = theCallBack->Value(pobj->_typenum);
              rcback->Read(pobj,f,me);
              pobj->_typenum = 0;
            }
          }
        }
        catch(Storage_StreamTypeMismatchError) {
          result = Standard_False;
          dData->SetErrorStatus(Storage_VSTypeMismatch);
          dData->SetErrorStatusExtension(i-1);
        }
        catch(Storage_StreamFormatError) {
          result = Standard_False;
          dData->SetErrorStatus(Storage_VSFormatError);
          dData->SetErrorStatusExtension(i-1);
        }
        catch(Storage_StreamReadError) {
          result = Standard_False;
          dData->SetErrorStatus(Storage_VSFormatError);
          dData->SetErrorStatusExtension(i-1);
        }
      }

      if (result) {
        Handle(Storage_HSeqOfRoot) rlist = rData->Roots();
        Handle(Storage_Root)       rroot;

        for(i = 1; i <= dData->NumberOfRoots(); i++) {
          rroot = rlist->Value(i);
          rData->UpdateRoot(rroot->Name(),iData->myReadArray->Value(rroot->Reference()));
        }

        errorCode = f.EndReadDataSection();
        result = (errorCode == Storage_VSOk);
        dData->SetErrorStatus(errorCode);
        if (!result) dData->SetErrorStatusExtension("EndReadDataSection");
      }
    }
  }
  else {
    dData->SetErrorStatus(Storage_VSModeError);
    dData->SetErrorStatusExtension("OpenMode");
  }

  iData->Clear();
  Clear();

  return dData;
}

//=======================================================================
//function : ReadHeaderSection
//purpose  : read the header part of the stream
//Arguments:
//           s: driver to read
//=======================================================================

Handle(Storage_HeaderData) Storage_Schema::ReadHeaderSection
                         (Storage_BaseDriver& s) const
{
  Handle(Storage_HeaderData) result = new Storage_HeaderData;

  if ((s.OpenMode() == Storage_VSRead) || (s.OpenMode() == Storage_VSReadWrite)) {
    IReadHeaderSection(s,result);
  }
  else {
    result->SetErrorStatus(Storage_VSModeError);
    result->SetErrorStatusExtension("OpenMode");
  }

  return result;
}

//=======================================================================
//function : ReadTypeSection
//purpose  : fill the TypeData with the  names of the type used
//           in a stream
//Arguments:
//           s: driver to read
//=======================================================================

Handle(Storage_TypeData) Storage_Schema::ReadTypeSection
                         (Storage_BaseDriver& f) const
{
  Handle(Storage_TypeData) result = new Storage_TypeData;

  if ((f.OpenMode() == Storage_VSRead) || (f.OpenMode() == Storage_VSReadWrite)) {
    IReadTypeSection(f,result);
  }
  else {
    result->SetErrorStatus(Storage_VSModeError);
    result->SetErrorStatusExtension("OpenMode");
  }

  return result;
}

//=======================================================================
//function : ReadRootSection
//purpose  : read root part of the file
//Arguments:
//           s: driver to read
//=======================================================================

Handle(Storage_RootData) Storage_Schema::ReadRootSection
                         (Storage_BaseDriver& f) const
{
  Handle(Storage_RootData) result = new Storage_RootData;

  if ((f.OpenMode() == Storage_VSRead) || (f.OpenMode() == Storage_VSReadWrite)) {
    IReadRootSection(f,result);
  }
  else {
    result->SetErrorStatus(Storage_VSModeError);
    result->SetErrorStatusExtension("OpenMode");
  }

  return result;
}

//=======================================================================
//function : SchemaKnownTypes
//purpose  : returns the known types of a schema
//=======================================================================

const TColStd_SequenceOfAsciiString& Storage_Schema::SchemaKnownTypes() const
{
  static TColStd_SequenceOfAsciiString aSeq;
  return aSeq;
}

//=======================================================================
//function : GetAllSchemaKnownTypes
//purpose  : returns the all known types  of a schema and their
//           nested schemes.
//PTV      : add get of all known type for inheritance of schemas
//=======================================================================

Handle(TColStd_HSequenceOfAsciiString) Storage_Schema::
                          GetAllSchemaKnownTypes() const
{
  Handle(TColStd_HSequenceOfAsciiString) aSeqOfType = new TColStd_HSequenceOfAsciiString;
  const TColStd_SequenceOfAsciiString& alocalTypeList = SchemaKnownTypes();

  for (Standard_Integer k = 1; k <= alocalTypeList.Length(); k++)
    aSeqOfType->Append(alocalTypeList.Value(k));

  // get nested schemas
  Handle(Storage_HArrayOfSchema) aNestedSchemas = NestedSchemas();
  if (!aNestedSchemas.IsNull())
  {
    for (Standard_Integer i = aNestedSchemas->Lower(); i <= aNestedSchemas->Upper(); i++)
    {
      Handle(Storage_Schema) aSchema = aNestedSchemas->Value(i);
      if (aSchema.IsNull())
        continue;

      Handle(TColStd_HSequenceOfAsciiString) typeList = aSchema->GetAllSchemaKnownTypes();
      for (Standard_Integer j = 1; j <= typeList->Length(); j++)
        aSeqOfType->Append(typeList->Value(j));
    }
  }

  return aSeqOfType;
}

//=======================================================================
//function : HasUnknownType
//purpose  : indicates whether  the  are  types  in  the driver
//           which are not known from  the schema and for which
//           no callbacks have been set. The unknown types can
//           be read in <theUnknownTypes>.
//=======================================================================

Standard_Boolean Storage_Schema::HasUnknownType
                         (Storage_BaseDriver& f,
                          TColStd_SequenceOfAsciiString& theUnknownTypes) const
{
  Standard_Boolean result = Standard_False;
  Handle(TColStd_HSequenceOfAsciiString) typeList = GetAllSchemaKnownTypes();

  Handle(Storage_TypeData) tData;

  tData = ReadTypeSection(f);

  result = (tData->ErrorStatus() != Storage_VSOk);

  if (!result) {
    Standard_Integer i;
    TColStd_MapOfAsciiString names;

    for (i = 1; i <= typeList->Length(); i++) {
      names.Add(typeList->Value(i));
    }

    Handle(TColStd_HSequenceOfAsciiString) flist = tData->Types();

    for (i = 1; i <= flist->Length(); i++) {
      if (!names.Contains(flist->Value(i))) {
        theUnknownTypes.Append(flist->Value(i));
        result = Standard_True;
      }
    }
  }

  return result;
}

//=======================================================================
//function : SetNestedSchemas
//purpose  : 
//=======================================================================

void Storage_Schema::SetNestedSchemas
                         (const Handle(Storage_HArrayOfSchema)& theSchemas)
{
  myArrayOfSchema = theSchemas;
}

//=======================================================================
//function : ClearNestedSchemas
//purpose  : 
//=======================================================================

void Storage_Schema::ClearNestedSchemas()
{
  myArrayOfSchema.Nullify();
}

//=======================================================================
//function : NestedSchemas
//purpose  : 
//=======================================================================

Handle(Storage_HArrayOfSchema) Storage_Schema::NestedSchemas() const
{
  return myArrayOfSchema;
}

//=======================================================================
//function : AddReadUnknownTypeCallBack
//purpose  : add two functions to the callback list
//=======================================================================

void Storage_Schema::AddReadUnknownTypeCallBack
                         (const TCollection_AsciiString& aTypeName,
                          const Handle(Storage_CallBack)& aCallBack)
{
  if (!aCallBack.IsNull()) {
     Handle(Storage_TypedCallBack) aTCallBack = new Storage_TypedCallBack(aTypeName,aCallBack);

     myCallBack.Bind(aTypeName,aTCallBack);
  }
}

//=======================================================================
//function : RemoveReadUnknownTypeCallBack
//purpose  : remove a callback for a type
//=======================================================================

void Storage_Schema::RemoveReadUnknownTypeCallBack
                         (const TCollection_AsciiString& aTypeName)
{
  if (myCallBack.IsBound(aTypeName)) {
    myCallBack.UnBind(aTypeName);
  }
}

//=======================================================================
//function : InstalledCallBackList
//purpose  : returns  a  list  of   type  name  with  installed
//           callback.
//=======================================================================

Handle(TColStd_HSequenceOfAsciiString) Storage_Schema::
                          InstalledCallBackList() const
{
  Storage_DataMapIteratorOfMapOfCallBack it(myCallBack);
  Handle(TColStd_HSequenceOfAsciiString) result = new TColStd_HSequenceOfAsciiString;

  for (; it.More(); it.Next()) {
    result->Append(it.Key());
  }

  return result;
}

//=======================================================================
//function : ClearCallBackList
//purpose  : clear all callback from schema instance.
//=======================================================================

void Storage_Schema::ClearCallBackList()
{
  myCallBack.Clear();
}

//=======================================================================
//function : UseDefaultCallBack
//purpose  : install  a  callback  for  all  unknown  type. the
//           objects with unknown types  will be skipped. (look
//           SkipObject method in BaseDriver)
//=======================================================================

void Storage_Schema::UseDefaultCallBack()
{
  myCallBackState = Standard_True;
}

//=======================================================================
//function : DontUseDefaultCallBack
//purpose  : tells schema to uninstall the default callback.
//=======================================================================

void Storage_Schema::DontUseDefaultCallBack()
{
  myCallBackState = Standard_False;
}

//=======================================================================
//function : IsUsingDefaultCallBack
//purpose  : ask if the schema is using the default callback.
//=======================================================================

Standard_Boolean Storage_Schema::IsUsingDefaultCallBack() const
{
  return myCallBackState;
}

//=======================================================================
//function : SetDefaultCallBack
//purpose  : overload the  default  function  for build.(use to
//           set an  error  message  or  skip  an  object while
//           reading an unknown type).
//=======================================================================

void Storage_Schema::SetDefaultCallBack(const Handle(Storage_CallBack)& f)
{
  myDefaultCallBack = f;
}

//=======================================================================
//function : ResetDefaultCallBack
//purpose  : reset  the  default  function  defined  by Storage
//           package.
//=======================================================================

void Storage_Schema::ResetDefaultCallBack()
{
  myDefaultCallBack = new Storage_DefaultCallBack;
}

//=======================================================================
//function : DefaultCallBack
//purpose  : returns   the   read   function   used   when  the
//           UseDefaultCallBack() is set.
//=======================================================================

Handle(Storage_CallBack) Storage_Schema::DefaultCallBack() const
{
  return myDefaultCallBack;
}

//=======================================================================
//function : ResolveUnknownType
//purpose  : 
//=======================================================================

Handle(Storage_CallBack) Storage_Schema::ResolveUnknownType
                         (const TCollection_AsciiString&     aTypeName,
                          const Handle(Standard_Persistent)& p,
                          const Storage_SolveMode            aMode) const
{
  Handle(Storage_CallBack) theCallBack;

  if (!myArrayOfSchema.IsNull()) {
    Standard_Integer i;
    Standard_Boolean IsNotFound     = Standard_True;
    Standard_Boolean AlreadyMatched;

    for(i = myArrayOfSchema->Lower(); i <= myArrayOfSchema->Upper() && IsNotFound; i++) {
      Handle(Storage_Schema) aSchema = myArrayOfSchema->Value(i);

      if (!aSchema.IsNull()) {
        AlreadyMatched = aSchema->SetNested();
        if (!AlreadyMatched) {
          if (aMode == Storage_WriteSolve || aMode == Storage_ReadSolve) {
            theCallBack = aSchema->CallBackSelection(aTypeName);
          }
          else if (aMode == Storage_AddSolve) {
            theCallBack = aSchema->AddTypeSelection(p);
          }
          aSchema->UnsetNested();
          IsNotFound = theCallBack.IsNull();
        }
      }
    }
  }

  if (!myNestedState && theCallBack.IsNull()) {
    if (myCallBack.IsBound(aTypeName)) {
      theCallBack = myCallBack.Find(aTypeName)->CallBack();
    }
    else if (myCallBackState == Standard_True) {
      theCallBack = myDefaultCallBack;
    }
    else {
      Clear();
      Standard_SStream aMsg;

      aMsg << "Unknown type " << aTypeName << " in schema ";

      if (!myName.IsEmpty()) {
        aMsg << myName;
      }

      Storage_StreamUnknownTypeError::Raise(aMsg);
    }
  }

  return theCallBack;
}

//=======================================================================
//function : CallBackSelection
//purpose  : 
//=======================================================================

Handle(Storage_CallBack) Storage_Schema::CallBackSelection
                         (const TCollection_AsciiString&) const
{
  Handle(Storage_CallBack) theCallBack;

  return theCallBack;
}

//=======================================================================
//function : AddTypeSelection
//purpose  : 
//=======================================================================

Handle(Storage_CallBack)  Storage_Schema::AddTypeSelection
                         (const Handle(Standard_Persistent)&) const
{
  Handle(Storage_CallBack) theCallBack;

  return theCallBack;
}

//=======================================================================
//function : BindType
//purpose  : 
//=======================================================================

void Storage_Schema::BindType
                         (const TCollection_AsciiString& aTypeName,
                          const Handle(Storage_CallBack)& aCallBack) const
{
  if (!HasTypeBinding(aTypeName)) {
      Handle(Storage_InternalData) iData = Storage_Schema::ICurrentData()->InternalData();
      Handle(Storage_TypeData) tData = Storage_Schema::ICurrentData()->TypeData();
      Handle(Storage_TypedCallBack) c = new Storage_TypedCallBack(aTypeName,aCallBack);

      tData->AddType(aTypeName,iData->myTypeId);
      c->SetIndex(iData->myTypeId++);
      iData->myTypeBinding.Bind(aTypeName,c);
  }
}

//=======================================================================
//function : TypeBinding
//purpose  : 
//=======================================================================

Handle(Storage_CallBack) Storage_Schema::TypeBinding
                         (const TCollection_AsciiString& aTypeName) const
{
  Handle(Storage_CallBack) result;

  if (HasTypeBinding(aTypeName)) {
    Handle(Storage_InternalData) iData = Storage_Schema::ICurrentData()->InternalData();

    result =  iData->myTypeBinding.Find(aTypeName)->CallBack();
  }

  return result;
}

//=======================================================================
//function : ReadPersistentReference
//purpose  : 
//=======================================================================

void Storage_Schema::ReadPersistentReference
                         (Handle(Standard_Persistent)& sp,
                          Storage_BaseDriver&          f)
{
  Standard_Integer ref;

  f.GetReference(ref);

  if (ref != 0) {
    Handle(Storage_InternalData) iData = Storage_Schema::ICurrentData()->InternalData();

    sp = iData->myReadArray->Value(ref);
  }
  else {
    sp.Nullify();
  }
}

//=======================================================================
//function : AddPersistent
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::AddPersistent
                         (const Handle(Standard_Persistent)& sp,
                          const Standard_CString tName) const
{
  Standard_Boolean result = Standard_False;

  if (!sp.IsNull()) {
    Handle(Storage_InternalData)     iData = Storage_Schema::ICurrentData()->InternalData();

    if (sp->_typenum == 0) {
      Standard_Integer         aTypenum;
      static TCollection_AsciiString  aTypeName;
      aTypeName = tName;
      Handle(Storage_TypeData) tData = Storage_Schema::ICurrentData()->TypeData();

      aTypenum = iData->myTypeBinding.Find(aTypeName)->Index();

      sp->_typenum = aTypenum;
      sp->_refnum = iData->myObjId++;

      result = Standard_True;
    }
  }

  return result;
}

//=======================================================================
//function : PersistentToAdd
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::PersistentToAdd
                         (const Handle(Standard_Persistent)& sp) const
{
 Standard_Boolean result = Standard_False;

  if (!sp.IsNull()) {
    Handle(Storage_InternalData) di = Storage_Schema::ICurrentData()->InternalData();

    if (sp->_typenum == 0 && sp->_refnum != -1) {
      result = Standard_True;
      sp->_refnum = -1;
      di->myPtoA.Append(sp);
    }
  }

  return result;
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void Storage_Schema::Clear() const
{
  Storage_Schema::ICurrentData().Nullify();
}

//=======================================================================
//function : IReadHeaderSection
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::IReadHeaderSection
                         (Storage_BaseDriver& f,
                          const Handle(Storage_HeaderData)& iData) const
{
  Standard_Boolean                 result = Standard_False;
  Storage_Error                    errorCode;
  TCollection_AsciiString          uinfo,mStorageVersion,mDate,mSchemaName,mSchemaVersion,mApplicationVersion;
  TCollection_ExtendedString       mApplicationName,mDataType;
  TColStd_SequenceOfAsciiString    mUserInfo;
  TColStd_SequenceOfExtendedString mComment;
  Standard_Integer                 mNBObj;

  errorCode = f.BeginReadInfoSection();

  if (errorCode == Storage_VSOk) {
    {
      try {
        OCC_CATCH_SIGNALS
        f.ReadInfo(mNBObj,
                   mStorageVersion,
                   mDate,
                   mSchemaName,
                   mSchemaVersion,
                   mApplicationName,
                   mApplicationVersion,
                   mDataType,
                   mUserInfo);
      }
      catch(Storage_StreamTypeMismatchError) {
        iData->SetErrorStatus(Storage_VSTypeMismatch);
        iData->SetErrorStatusExtension("ReadInfo");
        return Standard_False;
      }
      catch(Storage_StreamExtCharParityError) {
        iData->SetErrorStatus(Storage_VSExtCharParityError);
        iData->SetErrorStatusExtension("ReadInfo");
        return Standard_False;
      }
    }


    errorCode = f.EndReadInfoSection();

    iData->SetErrorStatus(errorCode);

    result = (errorCode == Storage_VSOk);

    if (result) {
      Standard_Integer i;

      iData->SetNumberOfObjects(mNBObj);
      iData->SetStorageVersion(mStorageVersion);
      iData->SetCreationDate(mDate);
      iData->SetSchemaName(mSchemaName);
      iData->SetSchemaVersion(mSchemaVersion);
      iData->SetApplicationName(mApplicationName);
      iData->SetApplicationVersion(mApplicationVersion);
      iData->SetDataType(mDataType);

      for (i = 1; i <= mUserInfo.Length(); i++) {
        iData->AddToUserInfo(mUserInfo.Value(i));
      }

      errorCode = f.BeginReadCommentSection();

      if (errorCode == Storage_VSOk) {
        {
          {
            try {
              OCC_CATCH_SIGNALS
              f.ReadComment(mComment);
            }
            catch(Storage_StreamTypeMismatchError) {
              iData->SetErrorStatus(Storage_VSTypeMismatch);
              iData->SetErrorStatusExtension("ReadComment");
              return Standard_False;
            }
            catch(Storage_StreamExtCharParityError) {
              iData->SetErrorStatus(Storage_VSExtCharParityError);
              iData->SetErrorStatusExtension("ReadComment");
              return Standard_False;
            }
          }
        }

        errorCode = f.EndReadCommentSection();
        iData->SetErrorStatus(errorCode);
        iData->SetErrorStatusExtension("EndReadCommentSection");
        result = (errorCode == Storage_VSOk);

        if (result) {
          for (i = 1; i <= mComment.Length(); i++) {
            iData->AddToComments(mComment.Value(i));
          }
        }
      }
      else {
        result = Standard_False;
        iData->SetErrorStatus(errorCode);
        iData->SetErrorStatusExtension("BeginReadCommentSection");
     }
    }
    else {
      iData->SetErrorStatusExtension("EndReadInfoSection");
    }
  }
  else {
    iData->SetErrorStatus(errorCode);
    iData->SetErrorStatusExtension("BeginReadInfoSection");
  }

  return result;
}


#ifdef DATATYPE_MIGRATION
//=======================================================================
// environment variable CSF_MIGRATION_TYPES should define full path of a file
// containing migration types table: oldtype - newtype
//=======================================================================
Standard_Boolean Storage_Schema::CheckTypeMigration(
                                  const TCollection_AsciiString&  oldName,
				  TCollection_AsciiString&  newName)
{
  static Standard_Boolean isChecked(Standard_False);
  static DataMapOfAStringAString aDMap;
  Standard_Boolean aMigration(Standard_False);
  
  if(!isChecked) {
    isChecked = Standard_True;
//    TCollection_AsciiString aFileName = getenv("CSF_MIGRATION_TYPES");
    OSD_Environment csf(TCollection_AsciiString("CSF_MIGRATION_TYPES"));
    TCollection_AsciiString aFileName = csf.Value();
    if(aFileName.Length() > 0) {
      OSD_Path aPath(aFileName,OSD_Default);
      OSD_File aFile;
      aFile.SetPath(aPath);
      if(aFile.Exists()) {
	OSD_Protection aProt(OSD_R,OSD_R,OSD_R,OSD_R);
	aFile.Open(OSD_ReadOnly, aProt);
	if(aFile.IsOpen() && aFile.IsReadable()) {
	  TCollection_AsciiString aLine;
	  Standard_Integer aNbReaded(0);
	  for (;;) {
	    aFile.ReadLine(aLine, 80, aNbReaded);
	    if(aFile.IsAtEnd() || !aNbReaded) {
	      aFile.Close();
	      break;
	    }
#ifdef OCCT_DEBUG
	    cout << "Storage_Sheme:: Line: = " << aLine <<endl;
#endif
	    TCollection_AsciiString aKey, aValue;
	    aKey = aLine.Token();
	    aValue = aLine.Token(" \t\n\r", 2);
	    aDMap.Bind(aKey, aValue);
	  }
	}
      }
#ifdef OCCT_DEBUG
      cout << "Storage_Sheme:: aDataMap.Size = " << aDMap.Extent() <<endl;
#endif
    }
  }

  if(aDMap.Extent()) {
    if(aDMap.IsBound(oldName)) {
      newName.Clear();
      newName = aDMap.Find(oldName);
      aMigration = Standard_True;
#ifdef OCCT_DEBUG
      cout << " newName = " << newName << endl;
#endif
    }
  } 
  return aMigration;
}
#endif

//=======================================================================
//function : IReadTypeSection
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::IReadTypeSection
                         (Storage_BaseDriver& f,
                          const Handle(Storage_TypeData)& tData) const
{
static Standard_Boolean         result;
  TCollection_AsciiString  typeName;
  Standard_Integer         typeNum;

  Storage_Error      errorCode;
  Standard_Integer len,i;

  result = Standard_False;
  errorCode = f.BeginReadTypeSection();

  if (errorCode == Storage_VSOk) {
    try {
      OCC_CATCH_SIGNALS
      len = f.TypeSectionSize();

      for (i = 1; i <= len; i++) {
        f.ReadTypeInformations(typeNum,typeName);
#ifdef DATATYPE_MIGRATION
	TCollection_AsciiString  newName;	
	if(CheckTypeMigration(typeName, newName)) {
#ifdef OCCT_DEBUG
	  cout << "CheckTypeMigration:OldType = " <<typeName << " Len = "<<typeName.Length()<<endl;
	  cout << "CheckTypeMigration:NewType = " <<newName  << " Len = "<< newName.Length()<<endl;
#endif
	  typeName = newName;
	}
#endif
        tData->AddType(typeName,typeNum);
      }
      result = Standard_True;
    }
    catch(Storage_StreamTypeMismatchError) {
      tData->SetErrorStatus(Storage_VSTypeMismatch);
      tData->SetErrorStatusExtension("ReadTypeInformations");
      return Standard_False;
    }

    if (result) {
      errorCode = f.EndReadTypeSection();
      result = (errorCode == Storage_VSOk);

      tData->SetErrorStatus(errorCode);
      if (!result) tData->SetErrorStatusExtension("EndReadTypeSection");
    }
  }
  else {
    tData->SetErrorStatus(errorCode);
    tData->SetErrorStatusExtension("BeginReadTypeSection");
  }

  return result;
}

//=======================================================================
//function : IReadRootSection
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::IReadRootSection
                         (Storage_BaseDriver& f,
                          const Handle(Storage_RootData)& rData) const
{
static Standard_Boolean            result;
  Standard_Integer            len,i,ref;
  Storage_Error               errorCode;
  Handle(Standard_Persistent) p;
  Handle(Storage_Root)        aRoot;

  result = Standard_False;
  errorCode = f.BeginReadRootSection();

  if (errorCode == Storage_VSOk) {
    TCollection_AsciiString rootName,typeName;

    try {
      OCC_CATCH_SIGNALS
      len = f.RootSectionSize();

      for (i = 1; i <= len; i++) {
        f.ReadRoot(rootName,ref,typeName);
        aRoot = new Storage_Root(rootName,p);
        aRoot->SetReference(ref);
        aRoot->SetType(typeName);
        rData->AddRoot(aRoot);
      }
      result = Standard_True;
    }
    catch(Storage_StreamTypeMismatchError) {
      result = Standard_False;
      rData->SetErrorStatus(Storage_VSTypeMismatch);
      rData->SetErrorStatusExtension("ReadRoot");
    }

    if (result) {
      errorCode = f.EndReadRootSection();
      result = (errorCode == Storage_VSOk);

      rData->SetErrorStatus(errorCode);
      if (!result) rData->SetErrorStatusExtension("EndReadRootSection");
    }
  }
  else {
    rData->SetErrorStatus(errorCode);
    rData->SetErrorStatusExtension("BeginReadRootSection");
  }

  return result;
}

//=======================================================================
//function : ISetCurrentData
//purpose  : 
//=======================================================================

void Storage_Schema::ISetCurrentData(const Handle(Storage_Data)& dData)
{
  Storage_Schema::ICurrentData() = dData;
}

//=======================================================================
//function : ICurrentData
//purpose  : 
//=======================================================================

Handle(Storage_Data)& Storage_Schema::ICurrentData()
{
  static Handle(Storage_Data) _Storage_CData;
  return _Storage_CData;
}

#define SLENGTH 80

//=======================================================================
//function : ICreationDate
//purpose  : 
//=======================================================================

TCollection_AsciiString Storage_Schema::ICreationDate()
{
  char nowstr[SLENGTH];
  time_t nowbin;
  struct tm *nowstruct;
  if (time(&nowbin) == (time_t)-1)
  {
#ifdef OCCT_DEBUG
    cerr << "Storage ERROR : Could not get time of day from time()" << endl;
#endif
  }

  nowstruct = localtime(&nowbin);

  if (strftime(nowstr, SLENGTH, "%m/%d/%Y", nowstruct) == (size_t) 0)
  {
#ifdef OCCT_DEBUG
    cerr << "Storage ERROR : Could not get string from strftime()" << endl;
#endif
  }

  TCollection_AsciiString t(nowstr);
  return t;
}

//=======================================================================
//function : SetNested
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::SetNested()
{
  Standard_Boolean result = myNestedState;

  myNestedState = Standard_True;

  return result;
}

//=======================================================================
//function : IsNested
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::IsNested() const
{
  return myNestedState;
}

//=======================================================================
//function : UnsetNested
//purpose  : 
//=======================================================================

Standard_Boolean Storage_Schema::UnsetNested()
{
  Standard_Boolean result = myNestedState;

  myNestedState = Standard_False;

  return result;
}
