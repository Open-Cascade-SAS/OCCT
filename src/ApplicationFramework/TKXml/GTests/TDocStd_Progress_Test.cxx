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

#include <BinDrivers.hxx>
#include <XmlDrivers.hxx>

#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TCollection_AsciiString.hxx>
#include <TNaming_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>

#include <sstream>

#include <gtest/gtest.h>

namespace
{
class ProgressObserver : public Message_ProgressIndicator
{
public:
  const NCollection_Sequence<TCollection_AsciiString>& Messages() const { return myMessages; }

protected:
  void Show(const Message_ProgressScope& theScope, const bool) override
  {
    TCollection_AsciiString aMessage("Progress: ");
    aMessage += static_cast<int>(100.0 * GetPosition() + 0.5);
    aMessage += "%";
    const Message_ProgressScope* aScope = &theScope;
    while (aScope != nullptr)
    {
      if (aScope->Name() != nullptr)
      {
        aMessage += " ";
        aMessage += aScope->Name();
      }
      aScope = aScope->Parent();
    }
    myMessages.Append(aMessage);
  }

private:
  NCollection_Sequence<TCollection_AsciiString> myMessages;
};

// Keep the text formatting used by Draw_ProgressIndicator for tests which
// validate the complete nested-scope output.
class DrawTextProgressObserver : public Message_ProgressIndicator
{
public:
  const NCollection_Sequence<TCollection_AsciiString>& Messages() const { return myMessages; }

protected:
  void Show(const Message_ProgressScope& theScope, const bool) override
  {
    TCollection_AsciiString aMessage("Progress: ");
    aMessage += static_cast<int>(100.0 * GetPosition() + 0.5);
    aMessage += "%";

    NCollection_List<const Message_ProgressScope*> aScopes;
    for (const Message_ProgressScope* aScope = &theScope; aScope != nullptr;
         aScope                              = aScope->Parent())
    {
      aScopes.Prepend(aScope);
    }
    for (NCollection_List<const Message_ProgressScope*>::Iterator anIterator(aScopes);
         anIterator.More();
         anIterator.Next())
    {
      const Message_ProgressScope* aScope = anIterator.Value();
      if (aScope->Name() == nullptr)
      {
        continue;
      }

      aMessage += " ";
      aMessage += aScope->Name();
      aMessage += ": ";
      if (aScope->IsInfinite())
      {
        if (Precision::IsInfinite(aScope->Value()))
        {
          aMessage += "finished";
        }
        else
        {
          aMessage += static_cast<int>(aScope->Value() + 0.5);
        }
      }
      else
      {
        aMessage += static_cast<int>(aScope->Value() + 0.5);
        aMessage += " / ";
        aMessage += static_cast<int>(aScope->MaxValue() + 0.5);
      }
    }
    myMessages.Append(aMessage);
  }

private:
  NCollection_Sequence<TCollection_AsciiString> myMessages;
};

static occ::handle<TDocStd_Application> NewApplication()
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  XmlDrivers::DefineFormat(anApplication);
  BinDrivers::DefineFormat(anApplication);
  return anApplication;
}

static occ::handle<TDocStd_Document> NewDocument(
  const occ::handle<TDocStd_Application>& theApplication,
  const char*                             theFormat)
{
  occ::handle<TDocStd_Document> aDocument;
  theApplication->NewDocument(theFormat, aDocument);
  EXPECT_FALSE(aDocument.IsNull());
  return aDocument;
}

static void SetBox(const TDF_Label& theLabel)
{
  TNaming_Builder aBuilder(theLabel);
  aBuilder.Generated(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape());
}

static bool HasMessage(const ProgressObserver& theObserver, const char* theText)
{
  for (NCollection_Sequence<TCollection_AsciiString>::Iterator anIterator(theObserver.Messages());
       anIterator.More();
       anIterator.Next())
  {
    if (anIterator.Value().Search(theText) != -1)
    {
      return true;
    }
  }
  return false;
}

static void ExpectMessages(const ProgressObserver& theObserver,
                           const char* const       theExpectedMessages[],
                           const int               theNbExpectedMessages)
{
  ASSERT_GT(theObserver.Messages().Length(), 0);
  EXPECT_TRUE(HasMessage(theObserver, "0%"));
  EXPECT_TRUE(HasMessage(theObserver, "100%"));
  for (int anIndex = 0; anIndex < theNbExpectedMessages; ++anIndex)
  {
    EXPECT_TRUE(HasMessage(theObserver, theExpectedMessages[anIndex]))
      << theExpectedMessages[anIndex];
  }
}

static void RunSaveAsProgress(const char*       theFormat,
                              const char* const theExpectedMessages[],
                              const int         theNbExpectedMessages)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  SetBox(aDocument->Main().FindChild(1, true));

  std::stringstream             aStream;
  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  EXPECT_EQ(anApplication->SaveAs(aDocument, aStream, Message_ProgressIndicator::Start(aProgress)),
            PCDM_SS_OK);
  EXPECT_FALSE(aStream.str().empty());
  ExpectMessages(*aProgress, theExpectedMessages, theNbExpectedMessages);
  anApplication->Close(aDocument);
}

static void RunOpenProgress(const char*       theFormat,
                            const char* const theExpectedMessages[],
                            const int         theNbExpectedMessages)
{
  occ::handle<TDocStd_Application> anApplication = NewApplication();
  occ::handle<TDocStd_Document>    aDocument     = NewDocument(anApplication, theFormat);
  ASSERT_FALSE(aDocument.IsNull());
  SetBox(aDocument->Main().FindChild(1, true));

  std::stringstream aStream;
  EXPECT_EQ(anApplication->SaveAs(aDocument, aStream), PCDM_SS_OK);
  EXPECT_FALSE(aStream.str().empty());
  anApplication->Close(aDocument);

  aStream.seekg(0);
  occ::handle<ProgressObserver> aProgress = new ProgressObserver();
  occ::handle<TDocStd_Document> aRestored;
  EXPECT_EQ(anApplication->Open(aStream, aRestored, Message_ProgressIndicator::Start(aProgress)),
            PCDM_RS_OK);
  ASSERT_FALSE(aRestored.IsNull());
  ExpectMessages(*aProgress, theExpectedMessages, theNbExpectedMessages);
  anApplication->Close(aRestored);
}

static void RunNestedProgress(const bool        theIsInfinite,
                              const char* const theExpectedMessages[],
                              const int         theNbExpectedMessages)
{
  occ::handle<DrawTextProgressObserver> aProgress = new DrawTextProgressObserver();
  {
    Message_ProgressScope anOuter(aProgress->Start(), "Outer", 3);
    for (int anOuterIndex = 0; anOuterIndex < 3 && anOuter.More(); ++anOuterIndex)
    {
      Message_ProgressScope anInner(anOuter.Next(), "Inner", 2, theIsInfinite);
      for (int anInnerIndex = 0; anInnerIndex < (theIsInfinite ? 4 : 2) && anInner.More();
           ++anInnerIndex, anInner.Next())
      {
      }
    }
  }

  ASSERT_EQ(aProgress->Messages().Length(), theNbExpectedMessages);
  for (int anIndex = 1; anIndex <= theNbExpectedMessages; ++anIndex)
  {
    EXPECT_STREQ(theExpectedMessages[anIndex - 1], aProgress->Messages()(anIndex).ToCString())
      << "Unexpected progress message at index " << anIndex;
  }
}
} // namespace

// caf/progress/A1: binary SaveAs progress reports the document and subtree stages.
TEST(TDocStd_Progress_Test, CafProgress_A1_BinarySaveAs)
{
  const char* const anExpected[] = {"Writing document", "Writing sub tree"};
  RunSaveAsProgress("BinOcaf", anExpected, 2);
}

// caf/progress/A2: XML SaveAs progress reports the storage stages.
TEST(TDocStd_Progress_Test, CafProgress_A2_XmlSaveAs)
{
  const char* const anExpected[] = {"Writing sub-tree",
                                    "Writing shape section",
                                    "Writing",
                                    "Geometry",
                                    "2D Curves",
                                    "3D Curves",
                                    "Polygons On Triangulation",
                                    "Surfaces",
                                    "3D Polygons",
                                    "Triangulations",
                                    "Shapes"};
  RunSaveAsProgress("XmlOcaf", anExpected, 11);
}

// caf/progress/B1: binary Open progress reports the document and subtree stages.
TEST(TDocStd_Progress_Test, CafProgress_B1_BinaryOpen)
{
  const char* const anExpected[] = {"Reading data", "Reading sub tree"};
  RunOpenProgress("BinOcaf", anExpected, 2);
}

// caf/progress/B2: XML Open progress reports the retrieval stages.
TEST(TDocStd_Progress_Test, CafProgress_B2_XmlOpen)
{
  const char* const anExpected[] = {"Reading document",
                                    "Reading",
                                    "3D Curves",
                                    "2D Curves",
                                    "3D Polygons",
                                    "Polygons On Triangulation",
                                    "Shapes",
                                    "Triangulations",
                                    "Surfaces",
                                    "Reading sub-tree"};
  RunOpenProgress("XmlOcaf", anExpected, 10);
}

// fclasses/bug28478: nested finite scopes retain both scope names and values.
TEST(TDocStd_Progress_Test, FClassesBug_28478_NestedFiniteScopes)
{
  const char* const anExpected[] = {"Progress: 0%",
                                    "Progress: 17% Outer: 1 / 3 Inner: 1 / 2",
                                    "Progress: 33% Outer: 1 / 3 Inner: 2 / 2",
                                    "Progress: 50% Outer: 2 / 3 Inner: 1 / 2",
                                    "Progress: 67% Outer: 2 / 3 Inner: 2 / 2",
                                    "Progress: 83% Outer: 3 / 3 Inner: 1 / 2",
                                    "Progress: 100% Outer: 3 / 3 Inner: 2 / 2"};
  RunNestedProgress(false, anExpected, 7);
}

// fclasses/bug31092: an infinite nested scope reports its terminal value as finished.
TEST(TDocStd_Progress_Test, FClassesBug_31092_NestedInfiniteScopes)
{
  const char* const anExpected[] = {"Progress: 0%",
                                    "Progress: 11% Outer: 1 / 3 Inner: 1",
                                    "Progress: 17% Outer: 1 / 3 Inner: 2",
                                    "Progress: 20% Outer: 1 / 3 Inner: 3",
                                    "Progress: 22% Outer: 1 / 3 Inner: 4",
                                    "Progress: 33% Outer: 1 / 3 Inner: finished",
                                    "Progress: 44% Outer: 2 / 3 Inner: 1",
                                    "Progress: 50% Outer: 2 / 3 Inner: 2",
                                    "Progress: 53% Outer: 2 / 3 Inner: 3",
                                    "Progress: 56% Outer: 2 / 3 Inner: 4",
                                    "Progress: 67% Outer: 2 / 3 Inner: finished",
                                    "Progress: 78% Outer: 3 / 3 Inner: 1",
                                    "Progress: 83% Outer: 3 / 3 Inner: 2",
                                    "Progress: 87% Outer: 3 / 3 Inner: 3",
                                    "Progress: 89% Outer: 3 / 3 Inner: 4",
                                    "Progress: 100% Outer: 3 / 3 Inner: finished"};
  RunNestedProgress(true, anExpected, 16);
}
