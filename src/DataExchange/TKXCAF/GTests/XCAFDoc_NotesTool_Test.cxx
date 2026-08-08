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

#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Integer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <XCAFDoc_AssemblyItemId.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_Note.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include <cstdint>

namespace
{
struct NotesContext
{
  occ::handle<TDocStd_Application> Application;
  occ::handle<TDocStd_Document>    Document;
  occ::handle<XCAFDoc_NotesTool>   Tool;
  TDF_Label                        Item1;
  TDF_Label                        Item2;
  Standard_GUID                    AttributeGuid;
};

static NotesContext NewContext()
{
  NotesContext aContext;
  aContext.Application = new TDocStd_Application();
  aContext.Application->NewDocument("BinXCAF", aContext.Document);
  occ::handle<XCAFDoc_ShapeTool> aShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(aContext.Document->Main());
  aContext.Item1 = aShapeTool->AddShape(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
  aContext.Item2 = aShapeTool->AddShape(BRepPrimAPI_MakeBox(2.0, 2.0, 2.0).Shape());
  aContext.Tool  = XCAFDoc_DocumentTool::NotesTool(aContext.Document->Main());
  aContext.AttributeGuid = Standard_GUID("27927843-72A0-41ef-901D-621FAB01C27A");
  TDataStd_Integer::Set(aContext.Item1, aContext.AttributeGuid, 1965);
  return aContext;
}

static occ::handle<XCAFDoc_Note> Comment(const NotesContext& theContext,
                                         const char*          theText)
{
  return theContext.Tool->CreateComment(TCollection_ExtendedString("The user"),
                                         TCollection_ExtendedString("2026-08-07T00:00:00"),
                                         TCollection_ExtendedString(theText));
}

static occ::handle<XCAFDoc_Note> Balloon(const NotesContext& theContext,
                                         const char*          theText)
{
  return theContext.Tool->CreateBalloon(TCollection_ExtendedString("The user"),
                                         TCollection_ExtendedString("2026-08-07T00:00:00"),
                                         TCollection_ExtendedString(theText));
}

static occ::handle<XCAFDoc_Note> BinaryNote(const NotesContext& theContext)
{
  occ::handle<NCollection_HArray1<uint8_t>> aData = new NCollection_HArray1<uint8_t>(1, 26);
  for (int anIndex = 1; anIndex <= 26; ++anIndex)
  {
    aData->SetValue(anIndex, static_cast<uint8_t>('a' + anIndex - 1));
  }
  return theContext.Tool->CreateBinData(TCollection_ExtendedString("The user"),
                                         TCollection_ExtendedString("2026-08-07T00:00:00"),
                                         TCollection_ExtendedString("Binary data"),
                                         TCollection_AsciiString("application/octet-stream"),
                                         aData);
}

static TDF_Label NoteLabel(const occ::handle<XCAFDoc_Note>& theNote)
{
  return theNote.IsNull() ? TDF_Label() : theNote->Label();
}

static XCAFDoc_AssemblyItemId ItemId(const TDF_Label& theItem)
{
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theItem, anEntry);
  return XCAFDoc_AssemblyItemId(anEntry);
}

static void AddBase(const NotesContext& theContext,
                    const occ::handle<XCAFDoc_Note>& theNote,
                    const TDF_Label& theItem)
{
  ASSERT_FALSE(theNote.IsNull());
  ASSERT_FALSE(theContext.Tool->AddNote(NoteLabel(theNote), theItem).IsNull());
}

static void AddAttribute(const NotesContext& theContext,
                         const occ::handle<XCAFDoc_Note>& theNote,
                         const TDF_Label& theItem)
{
  ASSERT_FALSE(theNote.IsNull());
  ASSERT_FALSE(
    theContext.Tool->AddNoteToAttr(NoteLabel(theNote), theItem, theContext.AttributeGuid).IsNull());
}

static void AddSubshape(const NotesContext& theContext,
                        const occ::handle<XCAFDoc_Note>& theNote,
                        const TDF_Label& theItem)
{
  ASSERT_FALSE(theNote.IsNull());
  ASSERT_FALSE(theContext.Tool->AddNoteToSubshape(NoteLabel(theNote), theItem, 1).IsNull());
}

static void ExpectCounts(const NotesContext& theContext,
                         const int           theAnnotations,
                         const int           theNotes,
                         const int           theOrphans)
{
  EXPECT_EQ(theContext.Tool->NbAnnotatedItems(), theAnnotations);
  EXPECT_EQ(theContext.Tool->NbNotes(), theNotes);
  EXPECT_EQ(theContext.Tool->NbOrphanNotes(), theOrphans);
}
} // namespace

// gdt/notes/A1: an empty notes tool has no annotations or notes.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_A1_Empty)
{
  const NotesContext aContext = NewContext();
  ExpectCounts(aContext, 0, 0, 0);
}

// gdt/notes/A2: comment and balloon notes are initially orphan notes.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_A2_CreateOrphans)
{
  const NotesContext aContext = NewContext();
  ASSERT_FALSE(Comment(aContext, "Hello, World!").IsNull());
  ASSERT_FALSE(Balloon(aContext, "Hello, Everyone!").IsNull());
  ExpectCounts(aContext, 0, 2, 2);
}

// gdt/notes/A3: binary notes are stored in the notes hive as orphan notes.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_A3_CreateBinary)
{
  const NotesContext aContext = NewContext();
  ASSERT_FALSE(BinaryNote(aContext).IsNull());
  ExpectCounts(aContext, 0, 1, 1);
}

// gdt/notes/A4: deleting one note leaves the other orphan note intact.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_A4_DeleteOne)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aComment = Comment(aContext, "Hello, World!");
  ASSERT_FALSE(aComment.IsNull());
  ASSERT_FALSE(Balloon(aContext, "Hello, Everyone!").IsNull());
  EXPECT_TRUE(aContext.Tool->DeleteNote(aComment->Label()));
  ExpectCounts(aContext, 0, 1, 1);
}

// gdt/notes/A5: deleting all notes clears the notes hive.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_A5_DeleteAll)
{
  const NotesContext aContext = NewContext();
  ASSERT_FALSE(Comment(aContext, "Hello, World!").IsNull());
  ASSERT_FALSE(Balloon(aContext, "Hello, Everyone!").IsNull());
  EXPECT_EQ(aContext.Tool->DeleteAllNotes(), 2);
  ExpectCounts(aContext, 0, 0, 0);
}

// gdt/notes/B1: one note may annotate two items.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_B1_TwoItemsOneNote)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote, aContext.Item1);
  AddBase(aContext, aNote, aContext.Item2);
  ExpectCounts(aContext, 2, 1, 0);
}

// gdt/notes/B2: two notes may annotate two different items.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_B2_TwoItemsTwoNotes)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item2);
  ExpectCounts(aContext, 2, 2, 0);
}

// gdt/notes/B3: deleting orphan notes removes only the unreferenced note.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_B3_DeleteOrphan)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote1, aContext.Item2);
  EXPECT_EQ(aContext.Tool->DeleteOrphanNotes(), 1);
  ExpectCounts(aContext, 2, 1, 0);
}

// gdt/notes/B4: two notes on one item share one annotated-item label.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_B4_TwoNotesOneItem)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item1);
  ExpectCounts(aContext, 1, 2, 0);
}

// gdt/notes/B5-B7: base, attribute, and subshape annotations are distinct references.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_B5_AttributeAnnotation)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddAttribute(aContext, aNote, aContext.Item1);
  ExpectCounts(aContext, 1, 1, 0);
  EXPECT_FALSE(aContext.Tool->FindAnnotatedItemAttr(aContext.Item1, aContext.AttributeGuid).IsNull());
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_B6_BaseAndAttribute)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote, aContext.Item1);
  AddAttribute(aContext, aNote, aContext.Item1);
  ExpectCounts(aContext, 2, 1, 0);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_B7_BaseAttributeAndSubshape)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote, aContext.Item1);
  AddAttribute(aContext, aNote, aContext.Item1);
  AddSubshape(aContext, aNote, aContext.Item1);
  ExpectCounts(aContext, 3, 1, 0);
}

// gdt/notes/C1-C2: deleting notes removes their annotations as well.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_C1_DeleteLinkedNote)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote, aContext.Item1);
  AddBase(aContext, aNote, aContext.Item2);
  EXPECT_TRUE(aContext.Tool->DeleteNote(aNote->Label()));
  ExpectCounts(aContext, 0, 0, 0);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_C2_DeleteOneLinkedNote)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item2);
  EXPECT_TRUE(aContext.Tool->DeleteNote(aNote1->Label()));
  ExpectCounts(aContext, 1, 1, 0);
}

// gdt/notes/C3-C4: removing selected links preserves unrelated links.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_C3_RemoveBaseAnnotation)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item2);
  EXPECT_TRUE(aContext.Tool->RemoveNote(aNote1->Label(), aContext.Item1));
  ExpectCounts(aContext, 1, 2, 1);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_C4_RemoveExtraAnnotations)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote = Comment(aContext, "Hello, World!");
  AddBase(aContext, aNote, aContext.Item1);
  AddAttribute(aContext, aNote, aContext.Item1);
  AddSubshape(aContext, aNote, aContext.Item1);
  EXPECT_TRUE(aContext.Tool->RemoveAttrNote(aNote->Label(), aContext.Item1, aContext.AttributeGuid));
  EXPECT_TRUE(aContext.Tool->RemoveSubshapeNote(aNote->Label(), aContext.Item1, 1));
  ExpectCounts(aContext, 1, 1, 0);
}

// gdt/notes/C5-C6: remove all notes of one extra-reference kind.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_C5_RemoveAllAttributeAnnotations)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote2, aContext.Item1);
  AddSubshape(aContext, aNote1, aContext.Item1);
  EXPECT_TRUE(
    aContext.Tool->RemoveAllAttrNotes(aContext.Item1, aContext.AttributeGuid));
  ExpectCounts(aContext, 2, 2, 1);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_C6_RemoveAllSubshapeAnnotations)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote1, aContext.Item1);
  AddSubshape(aContext, aNote1, aContext.Item1);
  AddSubshape(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aContext.Tool->RemoveAllSubshapeNotes(ItemId(aContext.Item1), 1));
  ExpectCounts(aContext, 2, 2, 1);
}

// gdt/notes/C7-C9: remove all annotations, optionally deleting orphans.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_C7_RemoveBaseAnnotations)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item1);
  AddAttribute(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote2, aContext.Item1);
  AddSubshape(aContext, aNote1, aContext.Item1);
  AddSubshape(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aContext.Tool->RemoveAllNotes(aContext.Item1));
  ExpectCounts(aContext, 2, 2, 0);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_C8_RemoveAllAnnotations)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item1);
  AddAttribute(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote2, aContext.Item1);
  AddSubshape(aContext, aNote1, aContext.Item1);
  AddSubshape(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aContext.Tool->RemoveAllNotes(aContext.Item1));
  EXPECT_TRUE(aContext.Tool->RemoveAllAttrNotes(aContext.Item1, aContext.AttributeGuid));
  EXPECT_TRUE(aContext.Tool->RemoveAllSubshapeNotes(ItemId(aContext.Item1), 1));
  ExpectCounts(aContext, 0, 2, 2);
}

TEST(XCAFDoc_NotesTool_Test, GdtNotes_C9_RemoveAllAndDeleteOrphans)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item1);
  AddAttribute(aContext, aNote1, aContext.Item1);
  AddAttribute(aContext, aNote2, aContext.Item1);
  AddSubshape(aContext, aNote1, aContext.Item1);
  AddSubshape(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aContext.Tool->RemoveAllNotes(aContext.Item1, true));
  EXPECT_TRUE(aContext.Tool->RemoveAllAttrNotes(aContext.Item1, aContext.AttributeGuid, true));
  EXPECT_TRUE(aContext.Tool->RemoveAllSubshapeNotes(ItemId(aContext.Item1), 1, true));
  ExpectCounts(aContext, 0, 0, 0);
}

// gdt/notes/D1: repeated annotations of the same kind reuse the same item label.
TEST(XCAFDoc_NotesTool_Test, GdtNotes_D1_ReuseAnnotatedItemLabels)
{
  const NotesContext aContext = NewContext();
  const occ::handle<XCAFDoc_Note> aNote1 = Comment(aContext, "Hello, World!");
  const occ::handle<XCAFDoc_Note> aNote2 = Comment(aContext, "Hello, Everyone!");
  AddBase(aContext, aNote1, aContext.Item1);
  const TDF_Label aBaseLabel = aContext.Tool->FindAnnotatedItem(aContext.Item1);
  AddBase(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aBaseLabel.IsEqual(aContext.Tool->FindAnnotatedItem(aContext.Item1)));

  AddAttribute(aContext, aNote1, aContext.Item1);
  const TDF_Label anAttributeLabel =
    aContext.Tool->FindAnnotatedItemAttr(aContext.Item1, aContext.AttributeGuid);
  AddAttribute(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(anAttributeLabel.IsEqual(
    aContext.Tool->FindAnnotatedItemAttr(aContext.Item1, aContext.AttributeGuid)));

  AddSubshape(aContext, aNote1, aContext.Item1);
  const TDF_Label aSubshapeLabel = aContext.Tool->FindAnnotatedItemSubshape(aContext.Item1, 1);
  AddSubshape(aContext, aNote2, aContext.Item1);
  EXPECT_TRUE(aSubshapeLabel.IsEqual(aContext.Tool->FindAnnotatedItemSubshape(aContext.Item1, 1)));
  ExpectCounts(aContext, 3, 2, 0);
}
