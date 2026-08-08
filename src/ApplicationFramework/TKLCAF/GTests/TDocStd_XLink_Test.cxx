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

#include <gtest/gtest.h>

#include <cmath>

#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Bnd_Box.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_XLink.hxx>
#include <TDocStd_XLinkTool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewDocument()
{
  static occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>         aDocument;
  anApplication->NewDocument("BinOcaf", aDocument);
  return aDocument;
}

static TDF_Label NewLabel(const occ::handle<TDocStd_Document>& theDocument, const int theTag)
{
  return theDocument->Main().FindChild(theTag, true);
}

static TopoDS_Shape MakeBox(const double theDx, const double theDy, const double theDz)
{
  return BRepPrimAPI_MakeBox(theDx, theDy, theDz).Shape();
}

static TopoDS_Shape MakeSphere(const double theRadius)
{
  return BRepPrimAPI_MakeSphere(theRadius).Shape();
}

static void SetShape(const TDF_Label& theLabel, const TopoDS_Shape& theShape)
{
  TNaming_Builder aBuilder(theLabel);
  aBuilder.Generated(theShape);
}

static occ::handle<TNaming_NamedShape> FindNamedShape(const TDF_Label& theLabel)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (!theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
  {
    return occ::handle<TNaming_NamedShape>();
  }
  return aNamedShape;
}

static TopoDS_Shape ShapeOf(const TDF_Label& theLabel)
{
  const occ::handle<TNaming_NamedShape> aNamedShape = FindNamedShape(theLabel);
  return aNamedShape.IsNull() ? TopoDS_Shape() : TNaming_Tool::GetShape(aNamedShape);
}

static bool SameBounds(const TopoDS_Shape& theFirst, const TopoDS_Shape& theSecond)
{
  if (theFirst.IsNull() || theSecond.IsNull())
  {
    return theFirst.IsNull() && theSecond.IsNull();
  }

  Bnd_Box aFirstBox;
  Bnd_Box aSecondBox;
  BRepBndLib::Add(theFirst, aFirstBox);
  BRepBndLib::Add(theSecond, aSecondBox);

  double aFirstXMin  = 0.0;
  double aFirstYMin  = 0.0;
  double aFirstZMin  = 0.0;
  double aFirstXMax  = 0.0;
  double aFirstYMax  = 0.0;
  double aFirstZMax  = 0.0;
  double aSecondXMin = 0.0;
  double aSecondYMin = 0.0;
  double aSecondZMin = 0.0;
  double aSecondXMax = 0.0;
  double aSecondYMax = 0.0;
  double aSecondZMax = 0.0;
  aFirstBox.Get(aFirstXMin,
                aFirstYMin,
                aFirstZMin,
                aFirstXMax,
                aFirstYMax,
                aFirstZMax);
  aSecondBox.Get(aSecondXMin,
                 aSecondYMin,
                 aSecondZMin,
                 aSecondXMax,
                 aSecondYMax,
                 aSecondZMax);

  return std::fabs(aFirstXMin - aSecondXMin) <= 1.0e-12
      && std::fabs(aFirstYMin - aSecondYMin) <= 1.0e-12
      && std::fabs(aFirstZMin - aSecondZMin) <= 1.0e-12
      && std::fabs(aFirstXMax - aSecondXMax) <= 1.0e-12
      && std::fabs(aFirstYMax - aSecondYMax) <= 1.0e-12
      && std::fabs(aFirstZMax - aSecondZMax) <= 1.0e-12;
}

static void EnableUndo(const occ::handle<TDocStd_Document>& theDocument)
{
  theDocument->SetUndoLimit(100);
}

static void CommitAndUndoRedo(const occ::handle<TDocStd_Document>& theDocument)
{
  theDocument->NewCommand();
  ASSERT_TRUE(theDocument->Undo());
  ASSERT_TRUE(theDocument->Redo());
}

static void ExpectLinked(const TDF_Label& theTarget, const TDF_Label& theSource)
{
  occ::handle<TDocStd_XLink> aXLink;
  ASSERT_TRUE(theTarget.FindAttribute(TDocStd_XLink::GetID(), aXLink));
  EXPECT_FALSE(aXLink->LabelEntry().IsEmpty());

  occ::handle<TDF_Reference> aReference;
  ASSERT_TRUE(theTarget.FindAttribute(TDF_Reference::GetID(), aReference));
  EXPECT_TRUE(aReference->Get() == theSource);
}
} // namespace

// caf/xlink/A1: copy a named shape without a link inside one document.
TEST(TDocStd_XLink_Test, CafXLink_A1_CopySameDocument)
{
  occ::handle<TDocStd_Document> aDocument = NewDocument();
  EnableUndo(aDocument);
  const TDF_Label aSource = NewLabel(aDocument, 2);
  const TDF_Label aTarget = NewLabel(aDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.Copy(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  CommitAndUndoRedo(aDocument);

  const TopoDS_Shape aCopiedBox = ShapeOf(aTarget);
  ASSERT_FALSE(aCopiedBox.IsNull());
  EXPECT_TRUE(SameBounds(aBox, aCopiedBox));
}

// caf/xlink/A2: copy a self-contained named shape between documents.
TEST(TDocStd_XLink_Test, CafXLink_A2_CopyOtherDocument)
{
  occ::handle<TDocStd_Document> aSourceDocument = NewDocument();
  occ::handle<TDocStd_Document> aTargetDocument = NewDocument();
  EnableUndo(aTargetDocument);
  const TDF_Label aSource = NewLabel(aSourceDocument, 2);
  const TDF_Label aTarget = NewLabel(aTargetDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aTargetDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.Copy(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  CommitAndUndoRedo(aTargetDocument);

  const TopoDS_Shape aCopiedBox = ShapeOf(aTarget);
  ASSERT_FALSE(aCopiedBox.IsNull());
  EXPECT_TRUE(SameBounds(aBox, aCopiedBox));
}

// caf/xlink/B1: create a link inside one document and preserve it through undo/redo.
TEST(TDocStd_XLink_Test, CafXLink_B1_CopyWithLinkSameDocument)
{
  occ::handle<TDocStd_Document> aDocument = NewDocument();
  EnableUndo(aDocument);
  const TDF_Label aSource = NewLabel(aDocument, 2);
  const TDF_Label aTarget = NewLabel(aDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  CommitAndUndoRedo(aDocument);

  ExpectLinked(aTarget, aSource);
  EXPECT_TRUE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/B2: a link follows source history through source undo/redo.
TEST(TDocStd_XLink_Test, CafXLink_B2_SourceUndoRedo)
{
  occ::handle<TDocStd_Document> aDocument = NewDocument();
  EnableUndo(aDocument);
  const TDF_Label aSource = NewLabel(aDocument, 2);
  const TDF_Label aTarget = NewLabel(aDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  aDocument->NewCommand();

  SetShape(aSource, MakeSphere(60.0));
  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Redo());

  aTool.UpdateLink(aTarget);
  ASSERT_TRUE(aTool.IsDone());

  ExpectLinked(aTarget, aSource);
  EXPECT_FALSE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/B4: create a link between two documents and preserve the copied shape.
TEST(TDocStd_XLink_Test, CafXLink_B4_CopyWithLinkOtherDocument)
{
  occ::handle<TDocStd_Document> aSourceDocument = NewDocument();
  occ::handle<TDocStd_Document> aTargetDocument = NewDocument();
  EnableUndo(aTargetDocument);
  const TDF_Label aSource = NewLabel(aSourceDocument, 2);
  const TDF_Label aTarget = NewLabel(aTargetDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aTargetDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  CommitAndUndoRedo(aTargetDocument);

  ExpectLinked(aTarget, aSource);
  EXPECT_TRUE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/B5: the target remains a valid external link after source history changes.
TEST(TDocStd_XLink_Test, CafXLink_B5_SourceHistoryOtherDocument)
{
  occ::handle<TDocStd_Document> aSourceDocument = NewDocument();
  occ::handle<TDocStd_Document> aTargetDocument = NewDocument();
  EnableUndo(aSourceDocument);
  EnableUndo(aTargetDocument);
  const TDF_Label aSource = NewLabel(aSourceDocument, 2);
  const TDF_Label aTarget = NewLabel(aTargetDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aTargetDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  aTargetDocument->NewCommand();

  aSourceDocument->NewCommand();
  SetShape(aSource, MakeSphere(70.0));
  aSourceDocument->NewCommand();
  ASSERT_TRUE(aSourceDocument->Undo());
  ASSERT_TRUE(aSourceDocument->Redo());

  aTool.UpdateLink(aTarget);
  ASSERT_TRUE(aTool.IsDone());

  ExpectLinked(aTarget, aSource);
  EXPECT_FALSE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/C1: updating references keeps the explicitly changed target shape.
TEST(TDocStd_XLink_Test, CafXLink_C1_UpdateReferences)
{
  occ::handle<TDocStd_Document> aDocument = NewDocument();
  EnableUndo(aDocument);
  const TDF_Label aSource = NewLabel(aDocument, 2);
  const TDF_Label aTarget = NewLabel(aDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  aDocument->NewCommand();

  aDocument->NewCommand();
  SetShape(aTarget, MakeSphere(70.0));
  occ::handle<TDocStd_XLink> aXLink;
  ASSERT_TRUE(aTarget.FindAttribute(TDocStd_XLink::GetID(), aXLink));
  aDocument->UpdateReferences(aXLink->DocumentEntry());
  aDocument->NewCommand();
  ASSERT_TRUE(aDocument->Undo());
  ASSERT_TRUE(aDocument->Redo());

  ExpectLinked(aTarget, aSource);
  EXPECT_FALSE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/D1: explicitly update a link after changing its source document.
TEST(TDocStd_XLink_Test, CafXLink_D1_UpdateOtherDocument)
{
  occ::handle<TDocStd_Document> aSourceDocument = NewDocument();
  occ::handle<TDocStd_Document> aTargetDocument = NewDocument();
  EnableUndo(aSourceDocument);
  EnableUndo(aTargetDocument);
  const TDF_Label aSource = NewLabel(aSourceDocument, 2);
  const TDF_Label aTarget = NewLabel(aTargetDocument, 3);
  const TopoDS_Shape aBox = MakeBox(100.0, 200.0, 300.0);
  SetShape(aSource, aBox);

  aTargetDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget, aSource);
  ASSERT_TRUE(aTool.IsDone());
  aTargetDocument->NewCommand();

  aSourceDocument->NewCommand();
  SetShape(aSource, MakeSphere(70.0));
  aSourceDocument->NewCommand();

  aTargetDocument->NewCommand();
  aTool.UpdateLink(aTarget);
  ASSERT_TRUE(aTool.IsDone());
  aTargetDocument->NewCommand();
  ASSERT_TRUE(aTargetDocument->Undo());
  ASSERT_TRUE(aTargetDocument->Redo());

  ExpectLinked(aTarget, aSource);
  EXPECT_FALSE(SameBounds(aBox, ShapeOf(aTarget)));
}

// caf/xlink/D2: update two cross-document links independently.
TEST(TDocStd_XLink_Test, CafXLink_D2_UpdateAllOtherDocumentLinks)
{
  occ::handle<TDocStd_Document> aSourceDocument = NewDocument();
  occ::handle<TDocStd_Document> aTargetDocument = NewDocument();
  EnableUndo(aSourceDocument);
  EnableUndo(aTargetDocument);
  const TDF_Label aSource1 = NewLabel(aSourceDocument, 2);
  const TDF_Label aSource2 = NewLabel(aSourceDocument, 22);
  const TDF_Label aTarget1 = NewLabel(aTargetDocument, 3);
  const TDF_Label aTarget2 = NewLabel(aTargetDocument, 32);
  const TopoDS_Shape aBox1 = MakeBox(100.0, 200.0, 300.0);
  const TopoDS_Shape aBox2 = MakeBox(2100.0, 2200.0, 2300.0);
  SetShape(aSource1, aBox1);
  SetShape(aSource2, aBox2);

  aTargetDocument->NewCommand();
  TDocStd_XLinkTool aTool;
  aTool.CopyWithLink(aTarget1, aSource1);
  ASSERT_TRUE(aTool.IsDone());
  aTool.CopyWithLink(aTarget2, aSource2);
  ASSERT_TRUE(aTool.IsDone());
  aTargetDocument->NewCommand();

  aSourceDocument->NewCommand();
  SetShape(aSource1, MakeSphere(70.0));
  SetShape(aSource2, MakeSphere(270.0));
  aSourceDocument->NewCommand();

  aTargetDocument->NewCommand();
  aTool.UpdateLink(aTarget1);
  ASSERT_TRUE(aTool.IsDone());
  aTool.UpdateLink(aTarget2);
  ASSERT_TRUE(aTool.IsDone());
  aTargetDocument->NewCommand();
  ASSERT_TRUE(aTargetDocument->Undo());
  ASSERT_TRUE(aTargetDocument->Redo());

  ExpectLinked(aTarget1, aSource1);
  ExpectLinked(aTarget2, aSource2);
  const TopoDS_Shape aUpdated1 = ShapeOf(aTarget1);
  const TopoDS_Shape aUpdated2 = ShapeOf(aTarget2);
  EXPECT_FALSE(SameBounds(aBox1, aUpdated1));
  EXPECT_FALSE(SameBounds(aBox2, aUpdated2));
  EXPECT_FALSE(SameBounds(aUpdated1, aUpdated2));
}
