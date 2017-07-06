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
IMPLEMENT_STANDARD_RTTIEXT(Storage_Schema,Standard_Transient)

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
//    AddTypeSelection(plist->Value(posto)->Object());
  }

  for (posfrom = plist->Length() + 1; posfrom <= iData->myPtoA.Length(); posfrom++) {
//    AddTypeSelection(iData->myPtoA.Value(posfrom));
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
        f.WriteRoot(plist->Value(i)->Name(),i,"PDocStd_Document");
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
      else
      {
        // hard-code migration table for known types
        aDMap.Bind("TDataStd_Shape",          "TDataXtd_Shape");
        aDMap.Bind("TDataStd_Constraint",     "TDataXtd_Constraint");
        aDMap.Bind("TDataStd_Geometry",       "TDataXtd_Geometry");
        aDMap.Bind("TDataStd_Axis",           "TDataXtd_Axis");
        aDMap.Bind("TDataStd_Point",          "TDataXtd_Point");
        aDMap.Bind("TDataStd_Plane",          "TDataXtd_Plane");
        aDMap.Bind("TDataStd_Position",       "TDataXtd_Position");
        aDMap.Bind("TDataStd_Placement",      "TDataXtd_Placement");
        aDMap.Bind("TDataStd_PatternStd",     "TDataXtd_PatternStd");
        aDMap.Bind("TPrsStd_AISPresentation", "TDataXtd_Presentation");
      }
#ifdef OCCT_DEBUG
      cout << "Storage_Sheme:: aDataMap.Size = " << aDMap.Extent() << endl;
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
