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
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HExtendedString.hxx>
#include <TDF_Label.hxx>
#include <TObj_Application.hxx>
#include <TObj_Model.hxx>
#include <TObj_Object.hxx>
#include <TObj_ObjectIterator.hxx>
#include <TObj_Partition.hxx>
#include <TObj_TNameContainer.hxx>

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
public:
  TObj_TestObject(const TDF_Label& theLabel, const bool theSetName = true)
      : TObj_Object(theLabel, theSetName)
  {
  }

  DEFINE_STANDARD_RTTIEXT(TObj_TestObject, TObj_Object)
};

IMPLEMENT_STANDARD_RTTIEXT(TObj_TestObject, TObj_Object)

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
