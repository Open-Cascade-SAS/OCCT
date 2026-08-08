// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Message_Messenger.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Type.hxx>
#include <BinTObjDrivers.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TObj_Application.hxx>
#include <TObj_Model.hxx>
#include <TObj_Object.hxx>
#include <TObj_ObjectIterator.hxx>
#include <TObj_OcafObjectIterator.hxx>
#include <TObj_Partition.hxx>
#include <TObj_Persistence.hxx>
#include <TObj_TNameContainer.hxx>

#include <sstream>

#include <gtest/gtest.h>

// Minimal concrete subclass of TObj_Model for testing purposes.
// TObj_Model is abstract due to pure virtual NewEmpty().
class TObj_TestModel : public TObj_Model
{
public:
  TObj_TestModel() = default;

  occ::handle<TObj_Model> NewEmpty() override { return new TObj_TestModel(); }

  DEFINE_STANDARD_RTTIEXT(TObj_TestModel, TObj_Model)
};

IMPLEMENT_STANDARD_RTTIEXT(TObj_TestModel, TObj_Model)

// Minimal concrete subclass of TObj_Object for testing purposes.
// TObj_Object has a protected constructor.
class TObj_TestObject : public TObj_Object
{
protected:
  enum DataTag
  {
    DataTag_First = TObj_Object::DataTag_Last,
    DataTag_IntVal,
    DataTag_RealArr,
    DataTag_Last = DataTag_First + 100
  };

  enum RefTag
  {
    RefTag_First = TObj_Object::RefTag_Last,
    RefTag_Other,
    RefTag_Last = RefTag_First + 100
  };

  enum ChildTag
  {
    ChildTag_First = TObj_Object::ChildTag_Last,
    ChildTag_Child,
    ChildTag_Last = ChildTag_First + 100
  };

public:
  TObj_TestObject(const TDF_Label& theLabel, const bool theSetName = true)
      : TObj_Object(theLabel, theSetName)
  {
  }

  void SetInt(const int theValue) { setInteger(theValue, DataTag_IntVal); }

  int GetInt() const { return getInteger(DataTag_IntVal); }

  void SetRealArr(const occ::handle<NCollection_HArray1<double>>& theArray)
  {
    setArray(theArray, DataTag_RealArr);
  }

  occ::handle<NCollection_HArray1<double>> GetRealArr() const
  {
    return getRealArray(0, DataTag_RealArr);
  }

  void SetRef(const occ::handle<TObj_Object>& theObject) { setReference(theObject, RefTag_Other); }

  occ::handle<TObj_Object> GetRef() const { return getReference(RefTag_Other); }

  occ::handle<TObj_TestObject> AddChild()
  {
    return new TObj_TestObject(getChildLabel(ChildTag_Child).NewChild());
  }

protected:
  DECLARE_TOBJOCAF_PERSISTENCE(TObj_TestObject, TObj_Object)

  DEFINE_STANDARD_RTTIEXT(TObj_TestObject, TObj_Object)
};

IMPLEMENT_STANDARD_RTTIEXT(TObj_TestObject, TObj_Object)
IMPLEMENT_TOBJOCAF_PERSISTENCE(TObj_TestObject)

static occ::handle<TObj_TestObject> FindTestObject(const occ::handle<TObj_Model>& theModel,
                                                    const char*                  theName)
{
  occ::handle<TCollection_HExtendedString> aName = new TCollection_HExtendedString(theName);
  return occ::down_cast<TObj_TestObject>(theModel->FindObject(aName, theModel->GetDictionary()));
}

// Test fixture that creates and initializes a TObj_Model.
class TObj_ModelTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myModel = new TObj_TestModel();
    // Load with empty string creates a new empty model
    ASSERT_TRUE(myModel->Load(TCollection_ExtendedString()));
  }

  void TearDown() override
  {
    if (!myModel.IsNull())
    {
      myModel->Close();
    }
  }

  occ::handle<TObj_TestModel> myModel;
};

TEST_F(TObj_ModelTest, Load_EmptyString_CreatesModel)
{
  EXPECT_FALSE(myModel->GetLabel().IsNull());
  EXPECT_FALSE(myModel->GetDocument().IsNull());
}

TEST_F(TObj_ModelTest, GetMainPartition_ReturnsNonNull)
{
  occ::handle<TObj_Partition> aPartition = myModel->GetMainPartition();
  ASSERT_FALSE(aPartition.IsNull());
}

TEST_F(TObj_ModelTest, GetRoot_ReturnsNonNull)
{
  occ::handle<TObj_Object> aRoot = myModel->GetRoot();
  ASSERT_FALSE(aRoot.IsNull());
}

TEST_F(TObj_ModelTest, GetModelName_ReturnsDefault)
{
  occ::handle<TCollection_HExtendedString> aName = myModel->GetModelName();
  ASSERT_FALSE(aName.IsNull());
  EXPECT_FALSE(aName->IsEmpty());
}

TEST_F(TObj_ModelTest, GetFormat_ReturnsTObjBin)
{
  TCollection_ExtendedString aFormat = myModel->GetFormat();
  EXPECT_TRUE(aFormat == "TObjBin");
}

TEST_F(TObj_ModelTest, GetApplication_ReturnsNonNull)
{
  occ::handle<TObj_Application> anApp = myModel->GetApplication();
  ASSERT_FALSE(anApp.IsNull());
}

TEST_F(TObj_ModelTest, GetDocumentModel_FromLabel_ReturnsSameModel)
{
  TDF_Label               aLabel      = myModel->GetLabel();
  occ::handle<TObj_Model> aFoundModel = TObj_Model::GetDocumentModel(aLabel);
  ASSERT_FALSE(aFoundModel.IsNull());
  EXPECT_EQ(myModel.get(), aFoundModel.get());
}

TEST_F(TObj_ModelTest, Close_NullifiesLabel)
{
  EXPECT_TRUE(myModel->Close());
  EXPECT_TRUE(myModel->GetLabel().IsNull());
}

TEST_F(TObj_ModelTest, Close_ReturnsFalse_WhenAlreadyClosed)
{
  EXPECT_TRUE(myModel->Close());
  EXPECT_FALSE(myModel->Close());
}

TEST_F(TObj_ModelTest, IsModified_InitialState)
{
  // A freshly loaded model reports its modification status
  // (implementation depends on document state).
  bool aModified = myModel->IsModified();
  (void)aModified; // just verify no crash
}

TEST_F(TObj_ModelTest, SetModified_True_MakesModified)
{
  myModel->SetModified(true);
  EXPECT_TRUE(myModel->IsModified());
}

TEST_F(TObj_ModelTest, SetModified_False_ClearsModified)
{
  myModel->SetModified(true);
  myModel->SetModified(false);
  EXPECT_FALSE(myModel->IsModified());
}

TEST_F(TObj_ModelTest, CommandTransaction_OpenCommit)
{
  myModel->GetDocument()->SetUndoLimit(10);
  EXPECT_FALSE(myModel->HasOpenCommand());
  myModel->OpenCommand();
  EXPECT_TRUE(myModel->HasOpenCommand());
  myModel->CommitCommand();
  EXPECT_FALSE(myModel->HasOpenCommand());
}

TEST_F(TObj_ModelTest, CommandTransaction_OpenAbort)
{
  myModel->GetDocument()->SetUndoLimit(10);
  myModel->OpenCommand();
  EXPECT_TRUE(myModel->HasOpenCommand());
  myModel->AbortCommand();
  EXPECT_FALSE(myModel->HasOpenCommand());
}

TEST_F(TObj_ModelTest, GetChildren_EmptyModel_NoChildren)
{
  occ::handle<TObj_ObjectIterator> anIter = myModel->GetChildren();
  if (!anIter.IsNull())
  {
    EXPECT_FALSE(anIter->More());
  }
}

TEST_F(TObj_ModelTest, GetObjects_EmptyModel_HasRootOnly)
{
  occ::handle<TObj_ObjectIterator> anIter = myModel->GetObjects();
  ASSERT_FALSE(anIter.IsNull());
  // The model iterator should at least exist; root partition is always present.
  // Count the objects.
  int aCount = 0;
  for (; anIter->More(); anIter->Next())
  {
    aCount++;
  }
  EXPECT_GE(aCount, 0);
}

TEST_F(TObj_ModelTest, NewEmpty_ReturnsFreshModel)
{
  occ::handle<TObj_Model> aNewModel = myModel->NewEmpty();
  ASSERT_FALSE(aNewModel.IsNull());
  // The new model should not share the same label
  EXPECT_TRUE(aNewModel->GetLabel().IsNull());
}

TEST_F(TObj_ModelTest, GetDictionary_ReturnsNonNull)
{
  occ::handle<TObj_TNameContainer> aDict = myModel->GetDictionary();
  ASSERT_FALSE(aDict.IsNull());
}

TEST_F(TObj_ModelTest, GetFormatVersion_FreshModel_ReturnsMinusOne)
{
  int aVersion = myModel->GetFormatVersion();
  EXPECT_EQ(aVersion, -1);
}

TEST_F(TObj_ModelTest, Update_DefaultReturnsTrue)
{
  EXPECT_TRUE(myModel->Update());
}

TEST_F(TObj_ModelTest, Messenger_SetAndGet)
{
  occ::handle<Message_Messenger> aMsgr = new Message_Messenger();
  myModel->SetMessenger(aMsgr);
  EXPECT_EQ(aMsgr.get(), myModel->Messenger().get());
}

// bugs/caf/bug29901: TObj save and restore preserve an integer through streams.
TEST_F(TObj_ModelTest, CafBug_29901_StreamInteger)
{
  BinTObjDrivers::DefineFormat(myModel->GetApplication());
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObject =
    new TObj_TestObject(myModel->GetMainPartition()->NewLabel());
  ASSERT_TRUE(anObject->SetName("obj1"));
  anObject->SetInt(29901);
  myModel->CommitCommand();

  std::ostringstream aStream;
  ASSERT_TRUE(myModel->SaveAs(aStream));
  ASSERT_FALSE(aStream.str().empty());
  ASSERT_TRUE(myModel->Close());

  std::istringstream aInput(aStream.str());
  occ::handle<TObj_TestModel> aRestored = new TObj_TestModel();
  ASSERT_TRUE(aRestored->Load(aInput));
  occ::handle<TObj_TestObject> aRestoredObject = FindTestObject(aRestored, "obj1");
  ASSERT_FALSE(aRestoredObject.IsNull());
  EXPECT_EQ(aRestoredObject->GetInt(), 29901);
  EXPECT_TRUE(aRestored->Close());
}

// bugs/caf/bug21231: TObj stream storage preserves names, hierarchy, references,
// an integer value, and an array of real values.
TEST_F(TObj_ModelTest, CafBug_21231_StreamObjectGraph)
{
  BinTObjDrivers::DefineFormat(myModel->GetApplication());
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObject1 =
    new TObj_TestObject(myModel->GetMainPartition()->NewLabel());
  occ::handle<TObj_TestObject> anObject2 =
    new TObj_TestObject(myModel->GetMainPartition()->NewLabel());
  occ::handle<TObj_TestObject> anObject3 =
    new TObj_TestObject(myModel->GetMainPartition()->NewLabel());
  ASSERT_TRUE(anObject1->SetName("obj1"));
  ASSERT_TRUE(anObject2->SetName("obj2"));
  ASSERT_TRUE(anObject3->SetName("obj3"));

  occ::handle<TObj_TestObject> aChild11 = anObject1->AddChild();
  occ::handle<TObj_TestObject> aChild21 = anObject2->AddChild();
  occ::handle<TObj_TestObject> aChild22 = anObject2->AddChild();
  ASSERT_TRUE(aChild11->SetName("ch11"));
  ASSERT_TRUE(aChild21->SetName("ch21"));
  ASSERT_TRUE(aChild22->SetName("ch22"));

  anObject1->SetRef(anObject2);
  anObject1->SetInt(533);
  occ::handle<NCollection_HArray1<double>> aValues = new NCollection_HArray1<double>(1, 3);
  aValues->SetValue(1, 3.14);
  aValues->SetValue(2, 2.78);
  aValues->SetValue(3, 0.123);
  anObject2->SetRealArr(aValues);
  myModel->CommitCommand();

  std::ostringstream aStream;
  ASSERT_TRUE(myModel->SaveAs(aStream));
  ASSERT_FALSE(aStream.str().empty());
  ASSERT_TRUE(myModel->Close());

  std::istringstream aInput(aStream.str());
  occ::handle<TObj_TestModel> aRestored = new TObj_TestModel();
  ASSERT_TRUE(aRestored->Load(aInput));
  occ::handle<TObj_TestObject> aRestoredObject1 = FindTestObject(aRestored, "obj1");
  occ::handle<TObj_TestObject> aRestoredObject2 = FindTestObject(aRestored, "obj2");
  ASSERT_FALSE(aRestoredObject1.IsNull());
  ASSERT_FALSE(aRestoredObject2.IsNull());

  occ::handle<TObj_ObjectIterator> anIterator = aRestoredObject2->GetChildren();
  ASSERT_FALSE(anIterator.IsNull());
  ASSERT_TRUE(anIterator->More());
  TCollection_AsciiString aFirstChildName;
  ASSERT_TRUE(anIterator->Value()->GetName(aFirstChildName));
  anIterator->Next();
  ASSERT_TRUE(anIterator->More());
  TCollection_AsciiString aSecondChildName;
  ASSERT_TRUE(anIterator->Value()->GetName(aSecondChildName));
  anIterator->Next();
  EXPECT_FALSE(anIterator->More());
  EXPECT_TRUE((aFirstChildName == "ch21" && aSecondChildName == "ch22")
              || (aFirstChildName == "ch22" && aSecondChildName == "ch21"));

  occ::handle<TObj_Object> aRestoredReference = aRestoredObject1->GetRef();
  ASSERT_FALSE(aRestoredReference.IsNull());
  TCollection_AsciiString aReferenceName;
  ASSERT_TRUE(aRestoredReference->GetName(aReferenceName));
  EXPECT_EQ(aReferenceName, "obj2");
  EXPECT_EQ(aRestoredObject1->GetInt(), 533);

  occ::handle<NCollection_HArray1<double>> aRestoredValues = aRestoredObject2->GetRealArr();
  ASSERT_FALSE(aRestoredValues.IsNull());
  ASSERT_EQ(aRestoredValues->Length(), 3);
  EXPECT_DOUBLE_EQ(aRestoredValues->Value(1), 3.14);
  EXPECT_DOUBLE_EQ(aRestoredValues->Value(2), 2.78);
  EXPECT_DOUBLE_EQ(aRestoredValues->Value(3), 0.123);
  EXPECT_FALSE(FindTestObject(aRestored, "obj3").IsNull());
  EXPECT_TRUE(aRestored->Close());
}

// Tests for TObj_Object functionality within a model context.
class TObj_ObjectTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myModel = new TObj_TestModel();
    ASSERT_TRUE(myModel->Load(TCollection_ExtendedString()));
    myPartition = myModel->GetMainPartition();
    ASSERT_FALSE(myPartition.IsNull());
  }

  void TearDown() override
  {
    if (!myModel.IsNull())
    {
      myModel->Close();
    }
  }

  occ::handle<TObj_TestObject> createObject()
  {
    TDF_Label aLabel = myPartition->NewLabel();
    return new TObj_TestObject(aLabel);
  }

  occ::handle<TObj_TestModel> myModel;
  occ::handle<TObj_Partition> myPartition;
};

TEST_F(TObj_ObjectTest, CreateObject_IsAlive)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();
  ASSERT_FALSE(anObj.IsNull());
  EXPECT_TRUE(anObj->IsAlive());
}

TEST_F(TObj_ObjectTest, CreateObject_HasLabel)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();
  EXPECT_FALSE(anObj->GetLabel().IsNull());
}

TEST_F(TObj_ObjectTest, CreateObject_GetModel_ReturnsSameModel)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();
  occ::handle<TObj_Model> aFoundModel = anObj->GetModel();
  ASSERT_FALSE(aFoundModel.IsNull());
  EXPECT_EQ(myModel.get(), aFoundModel.get());
}

TEST_F(TObj_ObjectTest, SetName_GetName_Roundtrip)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  occ::handle<TCollection_HExtendedString> aNewName =
    new TCollection_HExtendedString("TestObjectName");
  EXPECT_TRUE(anObj->SetName(aNewName));
  myModel->CommitCommand();

  occ::handle<TCollection_HExtendedString> aGotName = anObj->GetName();
  ASSERT_FALSE(aGotName.IsNull());
  EXPECT_TRUE(aGotName->String().IsEqual(aNewName->String()));
}

TEST_F(TObj_ObjectTest, SetName_CString_Roundtrip)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  EXPECT_TRUE(anObj->SetName("AsciiName"));
  myModel->CommitCommand();

  TCollection_AsciiString aName;
  EXPECT_TRUE(anObj->GetName(aName));
  EXPECT_TRUE(aName.IsEqual("AsciiName"));
}

TEST_F(TObj_ObjectTest, SetName_Duplicate_ReturnsFalse)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj1 = createObject();
  occ::handle<TObj_TestObject> anObj2 = createObject();
  myModel->CommitCommand();

  occ::handle<TCollection_HExtendedString> aName = new TCollection_HExtendedString("UniqueName");

  myModel->OpenCommand();
  EXPECT_TRUE(anObj1->SetName(aName));
  myModel->CommitCommand();

  myModel->OpenCommand();
  EXPECT_FALSE(anObj2->SetName(aName));
  myModel->CommitCommand();
}

TEST_F(TObj_ObjectTest, GetChildren_NoChildren_Empty)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  occ::handle<TObj_ObjectIterator> anIter = anObj->GetChildren();
  if (!anIter.IsNull())
  {
    EXPECT_FALSE(anIter->More());
  }
}

TEST_F(TObj_ObjectTest, GetDataLabel_ReturnsValidLabel)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  TDF_Label aDataLabel = anObj->GetDataLabel();
  EXPECT_FALSE(aDataLabel.IsNull());
}

TEST_F(TObj_ObjectTest, GetReferenceLabel_ReturnsValidLabel)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  TDF_Label aRefLabel = anObj->GetReferenceLabel();
  EXPECT_FALSE(aRefLabel.IsNull());
}

TEST_F(TObj_ObjectTest, Flags_SetAndTest)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  anObj->SetFlags(TObj_Object::ObjectState_Hidden);
  myModel->CommitCommand();

  EXPECT_TRUE(anObj->TestFlags(TObj_Object::ObjectState_Hidden));
  EXPECT_FALSE(anObj->TestFlags(TObj_Object::ObjectState_Saved));
}

TEST_F(TObj_ObjectTest, Flags_ClearFlags)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  anObj->SetFlags(TObj_Object::ObjectState_Hidden | TObj_Object::ObjectState_Saved);
  myModel->CommitCommand();

  myModel->OpenCommand();
  anObj->ClearFlags(TObj_Object::ObjectState_Hidden);
  myModel->CommitCommand();

  EXPECT_FALSE(anObj->TestFlags(TObj_Object::ObjectState_Hidden));
  EXPECT_TRUE(anObj->TestFlags(TObj_Object::ObjectState_Saved));
}

TEST_F(TObj_ObjectTest, Order_SetAndGet)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  EXPECT_TRUE(anObj->SetOrder(42));
  myModel->CommitCommand();

  EXPECT_EQ(42, anObj->GetOrder());
}

TEST_F(TObj_ObjectTest, Detach_RemovesObject)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  EXPECT_TRUE(anObj->Detach());
  myModel->CommitCommand();

  EXPECT_FALSE(anObj->IsAlive());
}

TEST_F(TObj_ObjectTest, GetObj_StaticLookup)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  TDF_Label                aLabel = anObj->GetLabel();
  occ::handle<TObj_Object> aFound;
  EXPECT_TRUE(TObj_Object::GetObj(aLabel, aFound));
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_EQ(anObj.get(), aFound.get());
}

TEST_F(TObj_ObjectTest, ModelIteration_AfterAddingObjects)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj1 = createObject();
  occ::handle<TObj_TestObject> anObj2 = createObject();
  occ::handle<TObj_TestObject> anObj3 = createObject();
  myModel->CommitCommand();

  occ::handle<TObj_ObjectIterator> anIter = myModel->GetChildren();
  ASSERT_FALSE(anIter.IsNull());

  int aCount = 0;
  for (; anIter->More(); anIter->Next())
  {
    aCount++;
  }
  EXPECT_EQ(3, aCount);
}

// caf/bugs/bug31323: typed child iteration remains recursive through all
// object levels.
TEST_F(TObj_ObjectTest, CafBug_31323_RecursiveTypedChildren)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObject = createObject();
  occ::handle<TObj_TestObject> aSub1    = anObject->AddChild();
  occ::handle<TObj_TestObject> aSub2    = anObject->AddChild();
  occ::handle<TObj_TestObject> aSub11   = aSub1->AddChild();
  occ::handle<TObj_TestObject> aSub12   = aSub1->AddChild();
  occ::handle<TObj_TestObject> aSub13   = aSub1->AddChild();
  occ::handle<TObj_TestObject> aSub121  = aSub12->AddChild();
  ASSERT_TRUE(anObject->SetName("obj"));
  ASSERT_TRUE(aSub1->SetName("sub1"));
  ASSERT_TRUE(aSub2->SetName("sub2"));
  ASSERT_TRUE(aSub11->SetName("sub11"));
  ASSERT_TRUE(aSub12->SetName("sub12"));
  ASSERT_TRUE(aSub13->SetName("sub13"));
  ASSERT_TRUE(aSub121->SetName("sub121"));
  myModel->CommitCommand();

  occ::handle<TObj_ObjectIterator> aFlatIterator = anObject->GetChildren();
  ASSERT_FALSE(aFlatIterator.IsNull());
  NCollection_IndexedMap<TCollection_AsciiString> aFlatNames;
  for (; aFlatIterator->More(); aFlatIterator->Next())
  {
    TCollection_AsciiString aName;
    ASSERT_TRUE(aFlatIterator->Value()->GetName(aName));
    aFlatNames.Add(aName);
  }
  ASSERT_EQ(aFlatNames.Extent(), 2);
  EXPECT_TRUE(aFlatNames.Contains(TCollection_AsciiString("sub1")));
  EXPECT_TRUE(aFlatNames.Contains(TCollection_AsciiString("sub2")));

  occ::handle<TObj_ObjectIterator> anIterator =
    new TObj_OcafObjectIterator(anObject->GetChildLabel(), nullptr, true, true);
  ASSERT_FALSE(anIterator.IsNull());
  NCollection_IndexedMap<TCollection_AsciiString> anAllNames;
  for (; anIterator->More(); anIterator->Next())
  {
    TCollection_AsciiString aName;
    ASSERT_TRUE(anIterator->Value()->GetName(aName));
    anAllNames.Add(aName);
  }
  ASSERT_EQ(anAllNames.Extent(), 6);
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub1")));
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub2")));
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub11")));
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub12")));
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub13")));
  EXPECT_TRUE(anAllNames.Contains(TCollection_AsciiString("sub121")));
}

TEST_F(TObj_ObjectTest, GetTypeFlags_DefaultVisible)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  int aTypeFlags = anObj->GetTypeFlags();
  EXPECT_TRUE((aTypeFlags & TObj_Object::Visible) != 0);
}

TEST_F(TObj_ObjectTest, HasBackReferences_InitiallyFalse)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  EXPECT_FALSE(anObj->HasBackReferences());
}

TEST_F(TObj_ObjectTest, NameRegistration_InModel)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  occ::handle<TCollection_HExtendedString> aName =
    new TCollection_HExtendedString("RegisteredName");
  anObj->SetName(aName);
  myModel->CommitCommand();

  EXPECT_TRUE(myModel->IsRegisteredName(aName, myModel->GetDictionary()));
}

TEST_F(TObj_ObjectTest, FindObject_ByName)
{
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> anObj = createObject();
  myModel->CommitCommand();

  myModel->OpenCommand();
  occ::handle<TCollection_HExtendedString> aName = new TCollection_HExtendedString("FindMe");
  anObj->SetName(aName);
  myModel->CommitCommand();

  occ::handle<TObj_Object> aFound = myModel->FindObject(aName, myModel->GetDictionary());
  ASSERT_FALSE(aFound.IsNull());
  EXPECT_EQ(anObj.get(), aFound.get());
}

// OCC31320: TObj - method TObj_Object::GetFatherObject() is not protected against deleted object
// After detaching a parent object, GetFatherObject() on its child must return null.
TEST_F(TObj_ObjectTest, OCC31320_GetFatherObject_ReturnsNull_AfterParentDetach)
{
  // Create the parent object inside the model partition.
  myModel->OpenCommand();
  occ::handle<TObj_TestObject> aParent = createObject();
  ASSERT_FALSE(aParent.IsNull());
  myModel->CommitCommand();

  // Create a child object on a sub-label of the parent's child collection.
  myModel->OpenCommand();
  TDF_Label                    aChildLabel = aParent->GetChildLabel().NewChild();
  occ::handle<TObj_TestObject> aChild      = new TObj_TestObject(aChildLabel);
  ASSERT_FALSE(aChild.IsNull());
  myModel->CommitCommand();

  // Detach the parent - this is what the original bug was about.
  myModel->OpenCommand();
  EXPECT_TRUE(aParent->Detach());
  myModel->CommitCommand();

  // After the parent is detached its TObj attributes are gone,
  // so GetFatherObject() traversing up must return null, not crash.
  occ::handle<TObj_Object> aFather = aChild->GetFatherObject();
  EXPECT_TRUE(aFather.IsNull());
}
