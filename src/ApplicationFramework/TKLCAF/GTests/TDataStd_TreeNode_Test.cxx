// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <NCollection_Array1.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TDocStd_Document.hxx>

namespace
{
struct TreeData
{
  occ::handle<TDF_Data>          myData;
  occ::handle<TDataStd_TreeNode> myRoot;
  occ::handle<TDataStd_TreeNode> myChild1;
  occ::handle<TDataStd_TreeNode> myChild2;
  occ::handle<TDataStd_TreeNode> myChild3;
  occ::handle<TDataStd_TreeNode> myChild4;
  occ::handle<TDataStd_TreeNode> myChild5;
  occ::handle<TDataStd_TreeNode> myChild6;
  occ::handle<TDataStd_TreeNode> myChild7;
  occ::handle<TDataStd_TreeNode> myChild8;
  occ::handle<TDataStd_TreeNode> myChild9;
  occ::handle<TDataStd_TreeNode> myChild10;
};

static TDF_Label MakeLabel(const occ::handle<TDF_Data>& theData, const int theTag)
{
  return theData->Root().FindChild(theTag, true);
}

static TreeData MakeFlatTree()
{
  TreeData aTree;
  aTree.myData    = new TDF_Data();
  aTree.myRoot    = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 2));
  aTree.myChild1  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 3));
  aTree.myChild2  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 4));
  aTree.myChild3  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 5));
  aTree.myChild4  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 6));
  aTree.myChild5  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 7));
  aTree.myChild6  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 8));
  aTree.myChild7  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 9));
  aTree.myChild8  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 10));
  aTree.myChild9  = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 11));
  aTree.myChild10 = TDataStd_TreeNode::Set(MakeLabel(aTree.myData, 12));
  return aTree;
}

static TreeData MakeNestedTree()
{
  TreeData aTree = MakeFlatTree();
  EXPECT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  EXPECT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  EXPECT_TRUE(aTree.myChild1->Append(aTree.myChild3));
  EXPECT_TRUE(aTree.myChild1->Append(aTree.myChild4));
  EXPECT_TRUE(aTree.myChild1->Append(aTree.myChild5));
  EXPECT_TRUE(aTree.myChild2->Append(aTree.myChild6));
  EXPECT_TRUE(aTree.myChild2->Append(aTree.myChild7));
  return aTree;
}

static void ExpectTreeOrder(const occ::handle<TDataStd_TreeNode>&                     theRoot,
                            const bool                                                theAllLevels,
                            const NCollection_Array1<occ::handle<TDataStd_TreeNode>>& theExpected)
{
  TDataStd_ChildNodeIterator anIterator(theRoot, theAllLevels);
  int                        anIndex = theExpected.Lower();
  for (; anIterator.More() && anIndex <= theExpected.Upper(); anIterator.Next(), ++anIndex)
  {
    EXPECT_EQ(anIterator.Value()->Label(), theExpected(anIndex)->Label());
  }
  EXPECT_EQ(anIndex, theExpected.Upper() + 1);
  EXPECT_FALSE(anIterator.More());
}

static NCollection_Array1<occ::handle<TDataStd_TreeNode>> ExpectedNodes(
  const occ::handle<TDataStd_TreeNode>& theNode1,
  const occ::handle<TDataStd_TreeNode>& theNode2)
{
  NCollection_Array1<occ::handle<TDataStd_TreeNode>> anExpected(1, 2);
  anExpected.SetValue(1, theNode1);
  anExpected.SetValue(2, theNode2);
  return anExpected;
}

static NCollection_Array1<occ::handle<TDataStd_TreeNode>> ExpectedNodes(
  const occ::handle<TDataStd_TreeNode>& theNode1,
  const occ::handle<TDataStd_TreeNode>& theNode2,
  const occ::handle<TDataStd_TreeNode>& theNode3)
{
  NCollection_Array1<occ::handle<TDataStd_TreeNode>> anExpected(1, 3);
  anExpected.SetValue(1, theNode1);
  anExpected.SetValue(2, theNode2);
  anExpected.SetValue(3, theNode3);
  return anExpected;
}

static NCollection_Array1<occ::handle<TDataStd_TreeNode>> ExpectedNodes(
  const occ::handle<TDataStd_TreeNode>& theNode1,
  const occ::handle<TDataStd_TreeNode>& theNode2,
  const occ::handle<TDataStd_TreeNode>& theNode3,
  const occ::handle<TDataStd_TreeNode>& theNode4,
  const occ::handle<TDataStd_TreeNode>& theNode5,
  const occ::handle<TDataStd_TreeNode>& theNode6,
  const occ::handle<TDataStd_TreeNode>& theNode7)
{
  NCollection_Array1<occ::handle<TDataStd_TreeNode>> anExpected(1, 7);
  anExpected.SetValue(1, theNode1);
  anExpected.SetValue(2, theNode2);
  anExpected.SetValue(3, theNode3);
  anExpected.SetValue(4, theNode4);
  anExpected.SetValue(5, theNode5);
  anExpected.SetValue(6, theNode6);
  anExpected.SetValue(7, theNode7);
  return anExpected;
}
} // namespace

// Test BUC60817: TDataStd_TreeNode descendant relationship
TEST(TDataStd_TreeNode_Test, BUC60817_DescendantRelationship)
{
  // Create TDF document
  occ::handle<TDF_Data> aDF = new TDF_Data();

  // Create two labels
  TDF_Label aLabel1 = aDF->Root().FindChild(2, true);
  TDF_Label aLabel2 = aDF->Root().FindChild(3, true);

  // Create TreeNodes on the labels
  occ::handle<TDataStd_TreeNode> aTN1 = TDataStd_TreeNode::Set(aLabel1);
  occ::handle<TDataStd_TreeNode> aTN2 = TDataStd_TreeNode::Set(aLabel2);

  // Append TN2 as a child of TN1
  aTN1->Append(aTN2);

  // Verify that TN2 is a descendant of TN1
  EXPECT_TRUE(aTN2->IsDescendant(aTN1)) << "TN2 should be a descendant of TN1 after Append";

  // Verify that TN1 is NOT a descendant of TN2
  EXPECT_FALSE(aTN1->IsDescendant(aTN2)) << "TN1 should not be a descendant of TN2";
}

// caf/bugs/bug24645: undoing the removal of intermediate children does not leave
// a stale last-child pointer on the tree node.
TEST(TDataStd_TreeNode_Test, CafBug_24645_UndoDetachedChildren)
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  aDocument->SetUndoLimit(1000);

  const TDF_Label                aRootLabel = aDocument->Main().FindChild(1, true);
  occ::handle<TDataStd_TreeNode> aRoot      = TDataStd_TreeNode::Set(aRootLabel);
  NCollection_Array1<occ::handle<TDataStd_TreeNode>> aChildren(1, 5);
  aDocument->NewCommand();
  for (int anIndex = 1; anIndex <= aChildren.Upper(); ++anIndex)
  {
    aChildren.SetValue(anIndex, TDataStd_TreeNode::Set(aRootLabel.FindChild(anIndex, true)));
    ASSERT_TRUE(aRoot->Append(aChildren(anIndex)));
    aDocument->NewCommand();
  }

  ASSERT_TRUE(aChildren(3)->Remove());
  ASSERT_TRUE(aChildren(4)->Remove());
  aDocument->NewCommand();

  ASSERT_TRUE(aDocument->Undo());
  EXPECT_FALSE(aRoot->HasLast());
}

// caf/tree/A1: TDF_TagSource::NewChild keeps the child label addressable.
TEST(TDataStd_TreeNode_Test, CafTree_A1_TagSourceNewChild)
{
  occ::handle<TDF_Data>      aData   = new TDF_Data();
  const TDF_Label            aParent = MakeLabel(aData, 2);
  const TDF_Label            aChild  = TDF_TagSource::NewChild(aParent);
  occ::handle<TDataStd_Name> aName =
    TDataStd_Name::Set(aChild, TCollection_ExtendedString("Label11"));

  ASSERT_FALSE(aName.IsNull());
  EXPECT_EQ(aChild.Father(), aParent);
  EXPECT_TRUE(aName->Get().IsEqual(TCollection_ExtendedString("Label11")));
}

// caf/tree/A2: explicit tag allocation and child creation use the same source.
TEST(TDataStd_TreeNode_Test, CafTree_A2_TagSourceNewTag)
{
  occ::handle<TDF_Data>      aData    = new TDF_Data();
  const TDF_Label            aParent1 = MakeLabel(aData, 2);
  const TDF_Label            aParent2 = MakeLabel(aData, 3);
  occ::handle<TDF_TagSource> aTagger1 = TDF_TagSource::Set(aParent1);
  occ::handle<TDF_TagSource> aTagger2 = TDF_TagSource::Set(aParent2);

  const TDF_Label aChild1 = TDF_TagSource::NewChild(aParent1);
  aTagger2->NewTag();
  aTagger2->NewTag();
  aTagger2->NewTag();
  const TDF_Label aChild2 = TDF_TagSource::NewChild(aParent2);

  EXPECT_EQ(aChild1.Tag(), 1);
  EXPECT_EQ(aChild2.Tag(), 4);
}

// caf/tree/B1: TDataStd_Directory::New creates a directory on the label.
TEST(TDataStd_TreeNode_Test, CafTree_B1_NewDirectory)
{
  occ::handle<TDF_Data>           aData      = new TDF_Data();
  const TDF_Label                 aLabel     = MakeLabel(aData, 2);
  occ::handle<TDataStd_Directory> aDirectory = TDataStd_Directory::New(aLabel);
  occ::handle<TDataStd_Name>      aName =
    TDataStd_Name::Set(aLabel, TCollection_ExtendedString("Label1"));

  ASSERT_FALSE(aDirectory.IsNull());
  ASSERT_FALSE(aName.IsNull());
  EXPECT_EQ(aDirectory->Label(), aLabel);
  EXPECT_TRUE(aName->Get().IsEqual(TCollection_ExtendedString("Label1")));
}

// caf/tree/B2: adding a directory allocates a child label and directory.
TEST(TDataStd_TreeNode_Test, CafTree_B2_AddDirectory)
{
  occ::handle<TDF_Data>           aData         = new TDF_Data();
  const TDF_Label                 aParent       = MakeLabel(aData, 2);
  occ::handle<TDataStd_Directory> aDirectory    = TDataStd_Directory::New(aParent);
  occ::handle<TDataStd_Directory> aSubDirectory = TDataStd_Directory::AddDirectory(aDirectory);
  occ::handle<TDataStd_Name>      aName =
    TDataStd_Name::Set(aSubDirectory->Label(), TCollection_ExtendedString("SubDirectory1"));

  ASSERT_FALSE(aSubDirectory.IsNull());
  ASSERT_FALSE(aName.IsNull());
  EXPECT_EQ(aSubDirectory->Label().Father(), aParent);
  EXPECT_TRUE(aName->Get().IsEqual(TCollection_ExtendedString("SubDirectory1")));
}

// caf/tree/C1: a node is its own root when it has no father.
TEST(TDataStd_TreeNode_Test, CafTree_C1_RootNode)
{
  TreeData aTree = MakeFlatTree();
  EXPECT_EQ(aTree.myRoot->Root()->Label(), aTree.myRoot->Label());
  EXPECT_TRUE(aTree.myRoot->IsRoot());
}

// caf/tree/D1-D2: AppendNode preserves insertion order.
TEST(TDataStd_TreeNode_Test, CafTree_D1_AppendNodeCount)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild3));

  EXPECT_EQ(aTree.myRoot->NbChildren(false), 3);
}

TEST(TDataStd_TreeNode_Test, CafTree_D2_AppendNodeOrder)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild3));

  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1, aTree.myChild2, aTree.myChild3));
}

// caf/tree/E1-E2: PrependNode inserts each new node before the old first child.
TEST(TDataStd_TreeNode_Test, CafTree_E1_PrependNodeCount)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Prepend(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Prepend(aTree.myChild3));

  EXPECT_EQ(aTree.myRoot->NbChildren(false), 3);
}

TEST(TDataStd_TreeNode_Test, CafTree_E2_PrependNodeOrder)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Prepend(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Prepend(aTree.myChild3));

  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild3, aTree.myChild2, aTree.myChild1));
}

// caf/tree/F1-F2: InsertNodeBefore places a node immediately before its target.
TEST(TDataStd_TreeNode_Test, CafTree_F1_InsertNodeBeforeCount)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myChild2->InsertBefore(aTree.myChild3));

  EXPECT_EQ(aTree.myRoot->NbChildren(false), 3);
}

TEST(TDataStd_TreeNode_Test, CafTree_F2_InsertNodeBeforeOrder)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myChild2->InsertBefore(aTree.myChild3));

  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1, aTree.myChild3, aTree.myChild2));
}

// caf/tree/G1-G2: InsertNodeAfter places a node immediately after its target.
TEST(TDataStd_TreeNode_Test, CafTree_G1_InsertNodeAfterCount)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myChild1->InsertAfter(aTree.myChild3));

  EXPECT_EQ(aTree.myRoot->NbChildren(false), 3);
}

TEST(TDataStd_TreeNode_Test, CafTree_G2_InsertNodeAfterOrder)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myChild1->InsertAfter(aTree.myChild3));

  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1, aTree.myChild3, aTree.myChild2));
}

// caf/tree/H1-H2: Remove detaches a node without changing its siblings.
TEST(TDataStd_TreeNode_Test, CafTree_H1_DetachNodeCount)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild3));
  ASSERT_TRUE(aTree.myChild2->Remove());

  EXPECT_EQ(aTree.myRoot->NbChildren(false), 2);
  EXPECT_TRUE(aTree.myChild2->Father().IsNull());
}

TEST(TDataStd_TreeNode_Test, CafTree_H2_DetachNodeOrder)
{
  TreeData aTree = MakeFlatTree();
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild1));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild2));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild3));
  ASSERT_TRUE(aTree.myChild2->Remove());

  ExpectTreeOrder(aTree.myRoot, true, ExpectedNodes(aTree.myChild1, aTree.myChild3));
}

// caf/tree/I1-I4: ChildNodeIterator supports both direct and recursive walks.
TEST(TDataStd_TreeNode_Test, CafTree_I1_ChildNodeIterateDirectCount)
{
  TreeData aTree = MakeNestedTree();
  EXPECT_EQ(aTree.myRoot->NbChildren(false), 2);
  ExpectTreeOrder(aTree.myRoot, false, ExpectedNodes(aTree.myChild1, aTree.myChild2));
}

TEST(TDataStd_TreeNode_Test, CafTree_I2_ChildNodeIterateDirectOrder)
{
  TreeData aTree = MakeNestedTree();
  ExpectTreeOrder(aTree.myRoot, false, ExpectedNodes(aTree.myChild1, aTree.myChild2));
}

TEST(TDataStd_TreeNode_Test, CafTree_I3_ChildNodeIterateAllLevelsCount)
{
  TreeData aTree = MakeNestedTree();
  EXPECT_EQ(aTree.myRoot->NbChildren(true), 7);
  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1,
                                aTree.myChild3,
                                aTree.myChild4,
                                aTree.myChild5,
                                aTree.myChild2,
                                aTree.myChild6,
                                aTree.myChild7));
}

TEST(TDataStd_TreeNode_Test, CafTree_I4_ChildNodeIterateAllLevelsOrder)
{
  TreeData aTree = MakeNestedTree();
  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1,
                                aTree.myChild3,
                                aTree.myChild4,
                                aTree.myChild5,
                                aTree.myChild2,
                                aTree.myChild6,
                                aTree.myChild7));
}

TEST(TDataStd_TreeNode_Test, CafTree_J1_ChildNodeIteratorMoreAllLevels)
{
  TreeData                   aTree = MakeNestedTree();
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, true);
  EXPECT_TRUE(anIterator.More());
}

TEST(TDataStd_TreeNode_Test, CafTree_J2_ChildNodeIteratorMoreDirect)
{
  TreeData                   aTree = MakeNestedTree();
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, false);
  EXPECT_TRUE(anIterator.More());
}

TEST(TDataStd_TreeNode_Test, CafTree_K1_ChildNodeIteratorNextAllLevels)
{
  TreeData                   aTree = MakeNestedTree();
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, true);
  int                        aChildCount = 0;
  while (anIterator.More())
  {
    anIterator.Next();
    ++aChildCount;
  }
  EXPECT_EQ(aChildCount, 7);
}

TEST(TDataStd_TreeNode_Test, CafTree_K2_ChildNodeIteratorNextDirect)
{
  TreeData                   aTree = MakeNestedTree();
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, false);
  int                        aChildCount = 0;
  while (anIterator.More())
  {
    anIterator.Next();
    ++aChildCount;
  }
  EXPECT_EQ(aChildCount, 2);
}

TEST(TDataStd_TreeNode_Test, CafTree_L1_ChildNodeIteratorValueDirect)
{
  TreeData aTree = MakeNestedTree();
  ExpectTreeOrder(aTree.myRoot, false, ExpectedNodes(aTree.myChild1, aTree.myChild2));
}

TEST(TDataStd_TreeNode_Test, CafTree_L2_ChildNodeIteratorValueAllLevels)
{
  TreeData aTree = MakeNestedTree();
  ExpectTreeOrder(aTree.myRoot,
                  true,
                  ExpectedNodes(aTree.myChild1,
                                aTree.myChild3,
                                aTree.myChild4,
                                aTree.myChild5,
                                aTree.myChild2,
                                aTree.myChild6,
                                aTree.myChild7));
}

TEST(TDataStd_TreeNode_Test, CafTree_M1_ChildNodeIteratorNextBrotherAllLevels)
{
  TreeData aTree = MakeNestedTree();
  ASSERT_TRUE(aTree.myChild4->Append(aTree.myChild8));
  ASSERT_TRUE(aTree.myChild4->Append(aTree.myChild9));
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, true);
  anIterator.Next();
  anIterator.Next();
  anIterator.Next();
  anIterator.NextBrother();
  EXPECT_EQ(anIterator.Value()->Label(), aTree.myChild9->Label());
  anIterator.NextBrother();
  EXPECT_EQ(anIterator.Value()->Label(), aTree.myChild5->Label());
  anIterator.NextBrother();
  EXPECT_EQ(anIterator.Value()->Label(), aTree.myChild2->Label());
}

TEST(TDataStd_TreeNode_Test, CafTree_M2_ChildNodeIteratorNextBrotherDirect)
{
  TreeData aTree = MakeNestedTree();
  ASSERT_TRUE(aTree.myChild4->Append(aTree.myChild8));
  ASSERT_TRUE(aTree.myChild4->Append(aTree.myChild9));
  ASSERT_TRUE(aTree.myRoot->Append(aTree.myChild10));
  TDataStd_ChildNodeIterator anIterator(aTree.myRoot, false);
  anIterator.NextBrother();
  EXPECT_EQ(anIterator.Value()->Label(), aTree.myChild2->Label());
  anIterator.NextBrother();
  EXPECT_EQ(anIterator.Value()->Label(), aTree.myChild10->Label());
}

// caf/bugs/buc60844: forgetting a tree node inside a command is reversible.
TEST(TDataStd_TreeNode_Test, Buc_CafBug_60844_ForgetAllUndo)
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  aDocument->SetUndoLimit(100);
  aDocument->NewCommand();

  const TDF_Label aLabel1   = aDocument->Main();
  const TDF_Label aLabel2   = aDocument->Main().Root().FindChild(2, true);
  const TDF_Label aLabel21  = aLabel2.FindChild(1, true);
  const TDF_Label aLabel211 = aLabel21.FindChild(1, true);
  const TDF_Label aLabel212 = aLabel21.FindChild(2, true);
  const TDF_Label aLabel3   = aDocument->Main().Root().FindChild(3, true);

  occ::handle<TDataStd_TreeNode> aNode1   = TDataStd_TreeNode::Set(aLabel1);
  occ::handle<TDataStd_TreeNode> aNode2   = TDataStd_TreeNode::Set(aLabel2);
  occ::handle<TDataStd_TreeNode> aNode21  = TDataStd_TreeNode::Set(aLabel21);
  occ::handle<TDataStd_TreeNode> aNode211 = TDataStd_TreeNode::Set(aLabel211);
  occ::handle<TDataStd_TreeNode> aNode212 = TDataStd_TreeNode::Set(aLabel212);
  occ::handle<TDataStd_TreeNode> aNode3   = TDataStd_TreeNode::Set(aLabel3);

  ASSERT_TRUE(aNode1->InsertAfter(aNode2));
  ASSERT_TRUE(aNode2->InsertAfter(aNode3));
  ASSERT_TRUE(aNode2->Append(aNode21));
  ASSERT_TRUE(aNode21->Append(aNode211));
  ASSERT_TRUE(aNode211->InsertAfter(aNode212));
  EXPECT_EQ(aNode2->NbChildren(), 1);
  EXPECT_EQ(aNode21->NbChildren(), 2);
  EXPECT_EQ(aNode21->Root()->Label(), aNode2->Label());

  aDocument->NewCommand();
  aLabel21.ForgetAllAttributes();
  EXPECT_FALSE(TDataStd_TreeNode::Find(aLabel21, aNode21));

  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());

  occ::handle<TDataStd_TreeNode> aRestored21;
  ASSERT_TRUE(TDataStd_TreeNode::Find(aLabel21, aRestored21));
  EXPECT_EQ(aRestored21->Father()->Label(), aLabel2);
  EXPECT_EQ(aRestored21->NbChildren(), 2);
  EXPECT_EQ(aRestored21->First()->Label(), aLabel211);
  EXPECT_EQ(aRestored21->Last()->Label(), aLabel212);
  EXPECT_EQ(aRestored21->Root()->Label(), aLabel2);
}
