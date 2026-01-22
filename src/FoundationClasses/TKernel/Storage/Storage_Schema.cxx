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
#include <Standard_Type.hxx>
#include <Storage.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Data.hxx>
#include <Storage_DefaultCallBack.hxx>
#include <Storage_CallBack.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_Root.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Storage_Schema.hxx>
#include <Storage_StreamWriteError.hxx>
#include <Storage_TypeData.hxx>
#include <Storage_TypedCallBack.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(Storage_Schema, Standard_Transient)

#define DATATYPE_MIGRATION

#ifdef DATATYPE_MIGRATION
  #include <NCollection_DataMap.hxx>
  #include <OSD_File.hxx>
  #include <OSD_Path.hxx>
  #include <OSD_Protection.hxx>
  #include <OSD_Environment.hxx>

typedef NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>
  DataMapOfAStringAString;

#endif

// IMPLEMENTATION BucketOfPersistent
//
Storage_Bucket::~Storage_Bucket()
{
  Standard::Free(mySpace);
  mySpace     = nullptr;
  mySpaceSize = 0;
  Clear();
}

//=================================================================================================

void Storage_Bucket::Clear()
{
  myCurrentSpace = -1;
}

//=================================================================================================

void Storage_Bucket::Append(Standard_Persistent* sp)
{
  myCurrentSpace++;
  mySpace[myCurrentSpace] = sp;
}

//=================================================================================================

Standard_Persistent* Storage_Bucket::Value(const int theIndex) const
{
  return mySpace[theIndex];
}

//=================================================================================================

Storage_BucketOfPersistent::Storage_BucketOfPersistent(const int theBucketSize,
                                                       const int theBucketNumber)
    : myNumberOfBucket(1),
      myNumberOfBucketAllocated(theBucketNumber),
      myBucketSize(theBucketSize)
{
  myBuckets       = (Storage_Bucket**)Standard::Allocate(sizeof(Storage_Bucket*) * theBucketNumber);
  myBuckets[0]    = new Storage_Bucket(myBucketSize);
  myCurrentBucket = myBuckets[0];
  myLength        = 0;
  myCurrentBucketNumber = 0;
}

//=================================================================================================

void Storage_BucketOfPersistent::Clear()
{
  if (myBuckets)
  {
    int i;

    for (i = 1; i < myNumberOfBucket; i++)
      delete myBuckets[i];
    myNumberOfBucket = 1;
    myCurrentBucket  = myBuckets[0];
    myCurrentBucket->Clear();
    myCurrentBucketNumber = 0;
    myLength              = 0;
  }
}

Storage_BucketOfPersistent::~Storage_BucketOfPersistent()
{
  Clear();
  delete myBuckets[0];
  Standard::Free(myBuckets);
  myBuckets = nullptr;
}

//=================================================================================================

Standard_Persistent* Storage_BucketOfPersistent::Value(const int theIndex)
{
  int theInd, theCurrentBucketNumber, tecurrentind = theIndex - 1;
  theCurrentBucketNumber = tecurrentind / myBucketSize;
  theInd                 = tecurrentind - (myBucketSize * theCurrentBucketNumber);

  return myBuckets[theCurrentBucketNumber]->mySpace[theInd];
}

//=================================================================================================

void Storage_BucketOfPersistent::Append(const occ::handle<Standard_Persistent>& sp)
{
  myCurrentBucket->myCurrentSpace++;

  if (myCurrentBucket->myCurrentSpace != myBucketSize)
  {
    myLength++;
    myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.get();
    return;
  }

  myCurrentBucket->myCurrentSpace--;
  myNumberOfBucket++;
  myCurrentBucketNumber++;

  if (myNumberOfBucket > myNumberOfBucketAllocated)
  {
    size_t e  = sizeof(Storage_Bucket*) * myNumberOfBucketAllocated;
    myBuckets = (Storage_Bucket**)Standard::Reallocate(myBuckets, e * 2);
    myNumberOfBucketAllocated *= 2;
  }

  myBuckets[myCurrentBucketNumber] = new Storage_Bucket(myBucketSize);
  myCurrentBucket                  = myBuckets[myCurrentBucketNumber];
  myCurrentBucket->myCurrentSpace++;
  myLength++;
  myCurrentBucket->mySpace[myCurrentBucket->myCurrentSpace] = sp.get();
}

//=================================================================================================

Storage_BucketIterator::Storage_BucketIterator(Storage_BucketOfPersistent* aBucketManager)
    : myBucket(nullptr),
      myCurrentBucket(nullptr),
      myCurrentBucketIndex(0),
      myCurrentIndex(0),
      myBucketNumber(0),
      myMoreObject(false)
{
  if (aBucketManager)
  {
    myBucket             = aBucketManager;
    myCurrentBucket      = myBucket->myBuckets[0];
    myBucketNumber       = aBucketManager->myNumberOfBucket;
    myCurrentBucketIndex = 0;
    myCurrentIndex       = 0;
    myMoreObject         = true;
  }
}

//=================================================================================================

void Storage_BucketIterator::Reset()
{
  if (myBucket)
  {
    myCurrentBucket      = myBucket->myBuckets[0];
    myBucketNumber       = myBucket->myNumberOfBucket;
    myCurrentIndex       = 0;
    myCurrentBucketIndex = 0;
    myMoreObject         = true;
  }
  else
    myMoreObject = false;
}

//=================================================================================================

void Storage_BucketIterator::Init(Storage_BucketOfPersistent* aBucketManager)
{
  if (aBucketManager)
  {
    myBucket             = aBucketManager;
    myCurrentBucket      = myBucket->myBuckets[0];
    myBucketNumber       = aBucketManager->myNumberOfBucket;
    myCurrentIndex       = 0;
    myCurrentBucketIndex = 0;
    myMoreObject         = true;
  }
  else
    myMoreObject = false;
}

//=================================================================================================

void Storage_BucketIterator::Next()
{
  if (!myMoreObject)
    return;

  if (myCurrentIndex < myCurrentBucket->myCurrentSpace)
  {
    myCurrentIndex++;
  }
  else
  {
    myCurrentIndex = 0;
    myCurrentBucketIndex++;
    if (myCurrentBucketIndex < myBucketNumber)
    {
      myCurrentBucket = myBucket->myBuckets[myCurrentBucketIndex];
    }
    else
    {
      myMoreObject = false;
    }
  }
}

//=======================================================================
// function : Storage_Schema
// purpose  : USER API -- --------------------------------------------------------------
//           IMPLEMENTATION BucketOfPersistent
//=======================================================================

Storage_Schema::Storage_Schema()
{
  Clear();
  ResetDefaultCallBack();
  myCallBackState = false;
}

//=======================================================================
// function : SetVersion
// purpose  : returns version of the schema
//=======================================================================

void Storage_Schema::SetVersion(const TCollection_AsciiString& aVersion)
{
  myVersion = aVersion;
}

//=======================================================================
// function : Version
// purpose  : returns the version of the schema
//=======================================================================

TCollection_AsciiString Storage_Schema::Version() const
{
  return myVersion;
}

//=======================================================================
// function : SetName
// purpose  : set the schema's name
//=======================================================================

void Storage_Schema::SetName(const TCollection_AsciiString& aSchemaName)
{
  myName = aSchemaName;
}

//=======================================================================
// function : Name
// purpose  : returns the schema's name
//=======================================================================

TCollection_AsciiString Storage_Schema::Name() const
{
  return myName;
}

//=======================================================================
// function : Write
// purpose  : write
// Arguments:
//           s: driver to write
//           raises  if  the  stream  is  not  opened  in  VSWrite  or
//           VSReadWrite
//=======================================================================

void Storage_Schema::Write(const occ::handle<Storage_BaseDriver>& theDriver,
                           const occ::handle<Storage_Data>&       aData) const
{
  if (aData.IsNull())
    return;

  // add all the persistent to write...
  //
  int                                                           posfrom, posto;
  occ::handle<Standard_Persistent>                              p;
  occ::handle<NCollection_HSequence<occ::handle<Storage_Root>>> plist;
  TCollection_AsciiString                                       errorContext("AddPersistent");
  Storage_Schema::ISetCurrentData(aData);

  occ::handle<Storage_InternalData> iData = aData->InternalData();

  aData->Clear();
  aData->ClearErrorStatus();

  plist = aData->Roots();

  for (posto = 1; posto <= plist->Length(); posto++)
  {
    PersistentToAdd(plist->Value(posto)->Object());
  }

  for (posto = 1; posto <= plist->Length(); posto++)
  {
    //    AddTypeSelection(plist->Value(posto)->Object());
  }

  for (posfrom = plist->Length() + 1; posfrom <= iData->myPtoA.Length(); posfrom++)
  {
    //    AddTypeSelection(iData->myPtoA.Value(posfrom));
  }

  // ...and now we write
  //
  int i, len;

  aData->HeaderData()->SetCreationDate(ICreationDate());
  aData->HeaderData()->SetStorageVersion(Storage::Version());
  aData->HeaderData()->SetNumberOfObjects(iData->myPtoA.Length());
  aData->HeaderData()->SetSchemaName(myName);
  aData->HeaderData()->SetSchemaVersion(myVersion);

  if ((theDriver->OpenMode() == Storage_VSWrite) || (theDriver->OpenMode() == Storage_VSReadWrite))
  {
    try
    {
      OCC_CATCH_SIGNALS
      errorContext = "BeginWriteInfoSection";
      theDriver->BeginWriteInfoSection();
      errorContext = "WriteInfo";
      theDriver->WriteInfo(aData->NumberOfObjects(),
                           aData->StorageVersion(),
                           aData->CreationDate(),
                           aData->SchemaName(),
                           aData->SchemaVersion(),
                           aData->ApplicationName(),
                           aData->ApplicationVersion(),
                           aData->DataType(),
                           aData->UserInfo());
      errorContext = "EndWriteInfoSection";
      theDriver->EndWriteInfoSection();

      errorContext = "BeginWriteCommentSection";
      theDriver->BeginWriteCommentSection();
      errorContext = "WriteComment";
      theDriver->WriteComment(aData->Comments());
      errorContext = "EndWriteCommentSection";
      theDriver->EndWriteCommentSection();

      occ::handle<NCollection_HSequence<TCollection_AsciiString>> tlist;

      tlist = aData->Types();

      errorContext = "BeginWriteTypeSection";
      theDriver->BeginWriteTypeSection();
      len = aData->NumberOfTypes();

      occ::handle<NCollection_HArray1<occ::handle<Storage_CallBack>>> WFunc =
        new NCollection_HArray1<occ::handle<Storage_CallBack>>(1, len);

      theDriver->SetTypeSectionSize(len);

      NCollection_DataMap<TCollection_AsciiString, occ::handle<Storage_TypedCallBack>>::Iterator
                                         cbit(iData->myTypeBinding);
      occ::handle<Storage_TypedCallBack> atcallBack;

      for (; cbit.More(); cbit.Next())
      {
        atcallBack = cbit.Value();
        WFunc->SetValue(atcallBack->Index(), atcallBack->CallBack());
      }

      errorContext = "WriteTypeInformations";
      for (i = 1; i <= len; i++)
      {
        theDriver->WriteTypeInformations(i, tlist->Value(i).ToCString());
      }

      errorContext = "EndWriteTypeSection";
      theDriver->EndWriteTypeSection();

      errorContext = "BeginWriteRootSection";
      theDriver->BeginWriteRootSection();
      theDriver->SetRootSectionSize(plist->Length());

      errorContext = "WriteRoot";
      for (i = 1; i <= plist->Length(); i++)
      {
        theDriver->WriteRoot(plist->Value(i)->Name(), i, "PDocStd_Document");
      }

      errorContext = "EndWriteRootSection";
      theDriver->EndWriteRootSection();

      errorContext = "BeginWriteRefSection";
      theDriver->BeginWriteRefSection();
      theDriver->SetRefSectionSize(iData->myObjId - 1);
      errorContext = "WriteReferenceType";

      Storage_BucketIterator bit(&iData->myPtoA);

      while (bit.More())
      {
        p = bit.Value();
        if (!p.IsNull())
          theDriver->WriteReferenceType(p->_refnum, p->_typenum);
        bit.Next();
      }

      errorContext = "EndWriteRefSection";
      theDriver->EndWriteRefSection();

      errorContext = "BeginWriteDataSection";
      theDriver->BeginWriteDataSection();

      occ::handle<Storage_Schema> me = this;

      errorContext = "Write";

      bit.Reset();

      while (bit.More())
      {
        p = bit.Value();
        if (!p.IsNull())
        {
          WFunc->Value(p->_typenum)->Write(p, theDriver, me);
          p->_typenum = 0;
        }
        bit.Next();
      }

      errorContext = "EndWriteDataSection";
      theDriver->EndWriteDataSection();
    }
    catch (Storage_StreamWriteError const&)
    {
      aData->SetErrorStatus(Storage_VSWriteError);
      aData->SetErrorStatusExtension(errorContext);
    }
  }
  else
  {
    aData->SetErrorStatus(Storage_VSModeError);
    aData->SetErrorStatusExtension("OpenMode");
  }

  iData->Clear();
  Clear();
}

//=======================================================================
// function : AddReadUnknownTypeCallBack
// purpose  : add two functions to the callback list
//=======================================================================

void Storage_Schema::AddReadUnknownTypeCallBack(const TCollection_AsciiString&       aTypeName,
                                                const occ::handle<Storage_CallBack>& aCallBack)
{
  if (!aCallBack.IsNull())
  {
    occ::handle<Storage_TypedCallBack> aTCallBack = new Storage_TypedCallBack(aTypeName, aCallBack);

    myCallBack.Bind(aTypeName, aTCallBack);
  }
}

//=======================================================================
// function : RemoveReadUnknownTypeCallBack
// purpose  : remove a callback for a type
//=======================================================================

void Storage_Schema::RemoveReadUnknownTypeCallBack(const TCollection_AsciiString& aTypeName)
{
  if (myCallBack.IsBound(aTypeName))
  {
    myCallBack.UnBind(aTypeName);
  }
}

//=======================================================================
// function : InstalledCallBackList
// purpose  : returns  a  list  of   type  name  with  installed
//           callback.
//=======================================================================

occ::handle<NCollection_HSequence<TCollection_AsciiString>> Storage_Schema::InstalledCallBackList()
  const
{
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Storage_TypedCallBack>>::Iterator it(
    myCallBack);
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> result =
    new NCollection_HSequence<TCollection_AsciiString>;

  for (; it.More(); it.Next())
  {
    result->Append(it.Key());
  }

  return result;
}

//=======================================================================
// function : ClearCallBackList
// purpose  : clear all callback from schema instance.
//=======================================================================

void Storage_Schema::ClearCallBackList()
{
  myCallBack.Clear();
}

//=======================================================================
// function : UseDefaultCallBack
// purpose  : install  a  callback  for  all  unknown  type. the
//           objects with unknown types  will be skipped. (look
//           SkipObject method in BaseDriver)
//=======================================================================

void Storage_Schema::UseDefaultCallBack()
{
  myCallBackState = true;
}

//=======================================================================
// function : DontUseDefaultCallBack
// purpose  : tells schema to uninstall the default callback.
//=======================================================================

void Storage_Schema::DontUseDefaultCallBack()
{
  myCallBackState = false;
}

//=======================================================================
// function : IsUsingDefaultCallBack
// purpose  : ask if the schema is using the default callback.
//=======================================================================

bool Storage_Schema::IsUsingDefaultCallBack() const
{
  return myCallBackState;
}

//=======================================================================
// function : SetDefaultCallBack
// purpose  : overload the  default  function  for build.(use to
//           set an  error  message  or  skip  an  object while
//           reading an unknown type).
//=======================================================================

void Storage_Schema::SetDefaultCallBack(const occ::handle<Storage_CallBack>& f)
{
  myDefaultCallBack = f;
}

//=======================================================================
// function : ResetDefaultCallBack
// purpose  : reset  the  default  function  defined  by Storage
//           package.
//=======================================================================

void Storage_Schema::ResetDefaultCallBack()
{
  myDefaultCallBack = new Storage_DefaultCallBack;
}

//=======================================================================
// function : DefaultCallBack
// purpose  : returns   the   read   function   used   when  the
//           UseDefaultCallBack() is set.
//=======================================================================

occ::handle<Storage_CallBack> Storage_Schema::DefaultCallBack() const
{
  return myDefaultCallBack;
}

//=================================================================================================

void Storage_Schema::BindType(const TCollection_AsciiString&       aTypeName,
                              const occ::handle<Storage_CallBack>& aCallBack) const
{
  if (!HasTypeBinding(aTypeName))
  {
    occ::handle<Storage_InternalData>  iData = Storage_Schema::ICurrentData()->InternalData();
    occ::handle<Storage_TypeData>      tData = Storage_Schema::ICurrentData()->TypeData();
    occ::handle<Storage_TypedCallBack> c     = new Storage_TypedCallBack(aTypeName, aCallBack);

    tData->AddType(aTypeName, iData->myTypeId);
    c->SetIndex(iData->myTypeId++);
    iData->myTypeBinding.Bind(aTypeName, c);
  }
}

//=================================================================================================

occ::handle<Storage_CallBack> Storage_Schema::TypeBinding(
  const TCollection_AsciiString& aTypeName) const
{
  occ::handle<Storage_CallBack> result;

  if (HasTypeBinding(aTypeName))
  {
    occ::handle<Storage_InternalData> iData = Storage_Schema::ICurrentData()->InternalData();

    result = iData->myTypeBinding.Find(aTypeName)->CallBack();
  }

  return result;
}

//=================================================================================================

bool Storage_Schema::AddPersistent(const occ::handle<Standard_Persistent>& sp,
                                   const char* const                       tName) const
{
  bool result = false;

  if (!sp.IsNull())
  {
    occ::handle<Storage_InternalData> iData = Storage_Schema::ICurrentData()->InternalData();

    if (sp->_typenum == 0)
    {
      int                            aTypenum;
      static TCollection_AsciiString aTypeName;
      aTypeName                           = tName;
      occ::handle<Storage_TypeData> tData = Storage_Schema::ICurrentData()->TypeData();

      aTypenum = iData->myTypeBinding.Find(aTypeName)->Index();

      sp->_typenum = aTypenum;
      sp->_refnum  = iData->myObjId++;

      result = true;
    }
  }

  return result;
}

//=================================================================================================

bool Storage_Schema::PersistentToAdd(const occ::handle<Standard_Persistent>& sp) const
{
  bool result = false;

  if (!sp.IsNull())
  {
    occ::handle<Storage_InternalData> di = Storage_Schema::ICurrentData()->InternalData();

    if (sp->_typenum == 0 && sp->_refnum != -1)
    {
      result      = true;
      sp->_refnum = -1;
      di->myPtoA.Append(sp);
    }
  }

  return result;
}

//=================================================================================================

void Storage_Schema::Clear() const
{
  Storage_Schema::ICurrentData().Nullify();
}

#ifdef DATATYPE_MIGRATION
//=======================================================================
// environment variable CSF_MIGRATION_TYPES should define full path of a file
// containing migration types table: oldtype - newtype
//=======================================================================
bool Storage_Schema::CheckTypeMigration(const TCollection_AsciiString& oldName,
                                        TCollection_AsciiString&       newName)
{
  static bool                    isChecked(false);
  static DataMapOfAStringAString aDMap;
  bool                           aMigration(false);

  if (!isChecked)
  {
    isChecked = true;
    //    TCollection_AsciiString aFileName = getenv("CSF_MIGRATION_TYPES");
    OSD_Environment         csf(TCollection_AsciiString("CSF_MIGRATION_TYPES"));
    TCollection_AsciiString aFileName = csf.Value();
    OSD_File                aFile;
    OSD_Path                aPath(aFileName, OSD_Default);
    aFile.SetPath(aPath);
    if (aFile.Exists())
    {
      OSD_Protection aProt(OSD_R, OSD_R, OSD_R, OSD_R);
      aFile.Open(OSD_ReadOnly, aProt);
      if (aFile.IsOpen() && aFile.IsReadable())
      {
        TCollection_AsciiString aLine;
        int                     aNbReaded(0);
        for (;;)
        {
          aFile.ReadLine(aLine, 80, aNbReaded);
          if (aFile.IsAtEnd() || !aNbReaded)
          {
            aFile.Close();
            break;
          }
  #ifdef OCCT_DEBUG
          std::cout << "Storage_Sheme:: Line: = " << aLine << std::endl;
  #endif
          TCollection_AsciiString aKey, aValue;
          aKey   = aLine.Token();
          aValue = aLine.Token(" \t\n\r", 2);
          aDMap.Bind(aKey, aValue);
        }
      }
    }
    else
    {
      // hard-code migration table for known types
      aDMap.Bind("TDataStd_Shape", "TDataXtd_Shape");
      aDMap.Bind("TDataStd_Constraint", "TDataXtd_Constraint");
      aDMap.Bind("TDataStd_Geometry", "TDataXtd_Geometry");
      aDMap.Bind("TDataStd_Axis", "TDataXtd_Axis");
      aDMap.Bind("TDataStd_Point", "TDataXtd_Point");
      aDMap.Bind("TDataStd_Plane", "TDataXtd_Plane");
      aDMap.Bind("TDataStd_Position", "TDataXtd_Position");
      aDMap.Bind("TDataStd_Placement", "TDataXtd_Placement");
      aDMap.Bind("TDataStd_PatternStd", "TDataXtd_PatternStd");
      aDMap.Bind("TPrsStd_AISPresentation", "TDataXtd_Presentation");
      aDMap.Bind("PDataStd_Shape", "PDataXtd_Shape");
      aDMap.Bind("PDataStd_Constraint", "PDataXtd_Constraint");
      aDMap.Bind("PDataStd_Geometry", "PDataXtd_Geometry");
      aDMap.Bind("PDataStd_Axis", "PDataXtd_Axis");
      aDMap.Bind("PDataStd_Point", "PDataXtd_Point");
      aDMap.Bind("PDataStd_Plane", "PDataXtd_Plane");
      aDMap.Bind("PDataStd_Position", "PDataXtd_Position");
      aDMap.Bind("PDataStd_Placement", "PDataXtd_Placement");
      aDMap.Bind("PDataStd_PatternStd", "PDataXtd_PatternStd");
    }
  #ifdef OCCT_DEBUG
    std::cout << "Storage_Sheme:: aDataMap.Size = " << aDMap.Extent() << std::endl;
  #endif
  }

  if (aDMap.Extent())
  {
    const TCollection_AsciiString* pNewName = aDMap.Seek(oldName);
    if (pNewName)
    {
      newName    = *pNewName;
      aMigration = true;
  #ifdef OCCT_DEBUG
      std::cout << " newName = " << newName << std::endl;
  #endif
    }
  }
  return aMigration;
}
#endif

//=================================================================================================

void Storage_Schema::ISetCurrentData(const occ::handle<Storage_Data>& dData)
{
  Storage_Schema::ICurrentData() = dData;
}

//=================================================================================================

occ::handle<Storage_Data>& Storage_Schema::ICurrentData()
{
  static occ::handle<Storage_Data> _Storage_CData;
  return _Storage_CData;
}

#define SLENGTH 80

//=================================================================================================

TCollection_AsciiString Storage_Schema::ICreationDate()
{
  char       nowstr[SLENGTH];
  time_t     nowbin;
  struct tm* nowstruct;
  if (time(&nowbin) == (time_t)-1)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Storage ERROR : Could not get time of day from time()" << std::endl;
#endif
  }

  nowstruct = localtime(&nowbin);

  if (strftime(nowstr, SLENGTH, "%m/%d/%Y", nowstruct) == (size_t)0)
  {
#ifdef OCCT_DEBUG
    std::cerr << "Storage ERROR : Could not get string from strftime()" << std::endl;
#endif
  }

  TCollection_AsciiString t(nowstr);
  return t;
}
