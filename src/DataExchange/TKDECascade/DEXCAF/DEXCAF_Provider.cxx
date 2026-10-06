// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <DEXCAF_Provider.hxx>

#include <BinDrivers.hxx>
#include <BinLDrivers.hxx>
#include <BinTObjDrivers.hxx>
#include <BinXCAFDrivers.hxx>
#include <StdDrivers.hxx>
#include <StdLDrivers.hxx>
#include <XmlDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>
#include <XmlXCAFDrivers.hxx>

#include <BRep_Builder.hxx>
#include <DE_ValidationUtils.hxx>
#include <NCollection_Sequence.hxx>
#include <DEXCAF_ConfigurationNode.hxx>
#include <Message.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Owner.hxx>
#include <TDF_Data.hxx>
#include <OSD_FileSystem.hxx>
#include <Standard_Failure.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

namespace
{
// ==================================================================================================

occ::handle<TDocStd_Application> makeReadApplication()
{
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinDrivers::DefineFormat(anApp);
  BinLDrivers::DefineFormat(anApp);
  BinTObjDrivers::DefineFormat(anApp);
  BinXCAFDrivers::DefineFormat(anApp);
  StdDrivers::DefineFormat(anApp);
  StdLDrivers::DefineFormat(anApp);
  XmlDrivers::DefineFormat(anApp);
  XmlLDrivers::DefineFormat(anApp);
  XmlTObjDrivers::DefineFormat(anApp);
  XmlXCAFDrivers::DefineFormat(anApp);

  return anApp;
}

// ==================================================================================================

occ::handle<PCDM_ReaderFilter> makeReadFilter(const DEXCAF_ConfigurationNode& theNode)
{
  occ::handle<PCDM_ReaderFilter> aFilter =
    new PCDM_ReaderFilter(theNode.InternalParameters.ReadAppendMode);
  for (NCollection_List<TCollection_AsciiString>::Iterator anIt(
         theNode.InternalParameters.ReadSkipValues);
       anIt.More();
       anIt.Next())
  {
    aFilter->AddSkipped(anIt.Value());
  }
  for (NCollection_List<TCollection_AsciiString>::Iterator anIt(
         theNode.InternalParameters.ReadValues);
       anIt.More();
       anIt.Next())
  {
    if (anIt.Value().StartsWith("0"))
    {
      aFilter->AddPath(anIt.Value());
    }
    else
    {
      aFilter->AddRead(anIt.Value());
    }
  }
  return aFilter;
}

// ==================================================================================================

bool documentShape(const occ::handle<TDocStd_Document>& theDocument, TopoDS_Shape& theShape)
{
  NCollection_Sequence<TDF_Label>      aLabels; // Required XCAF output interface.
  const occ::handle<XCAFDoc_ShapeTool> aShapeTool =
    XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->GetFreeShapes(aLabels);
  if (aLabels.IsEmpty())
  {
    Message::SendFail("XCAF document has no free shapes");
    return false;
  }
  TopoDS_Shape aResult;
  if (aLabels.Size() == 1)
  {
    aResult = aShapeTool->GetShape(aLabels.First());
  }
  else
  {
    TopoDS_Compound aCompound;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound(aCompound);
    for (const TDF_Label& aLabel : aLabels)
    {
      const TopoDS_Shape aShape = aShapeTool->GetShape(aLabel);
      if (aShape.IsNull())
      {
        Message::SendFail("XCAF free-shape label has no shape");
        return false;
      }
      aBuilder.Add(aCompound, aShape);
    }
    aResult = aCompound;
  }
  if (aResult.IsNull())
  {
    Message::SendFail("XCAF free-shape label has no shape");
    return false;
  }
  theShape = aResult;
  return true;
}

// ==================================================================================================

void assignReadDocument(const occ::handle<TDocStd_Application>& theApplication,
                        const occ::handle<TDocStd_Document>&    theSource,
                        const occ::handle<TDocStd_Document>&    theTarget)
{
  // Close the temporary application document before assigning its data owner.
  const occ::handle<TDF_Data> aData = theSource->GetData();
  occ::handle<TDocStd_Owner>  anOwner;
  if (!aData->Root().FindAttribute(TDocStd_Owner::GetID(), anOwner))
  {
    throw Standard_Failure("Loaded XCAF document has no data owner");
  }
  theApplication->Close(theSource);
  const bool aModificationMode = theTarget->ModificationMode();
  // Old deltas and nested commands refer to the previous data framework.
  theTarget->BeforeClose();
  occ::handle<TDocStd_Owner> aPreviousOwner;
  if (theTarget->GetData()->Root().FindAttribute(TDocStd_Owner::GetID(), aPreviousOwner)
      && aPreviousOwner->GetDocument() == theTarget)
  {
    aPreviousOwner->SetDocument(occ::handle<TDocStd_Document>());
  }
  theTarget->SetData(aData);
  anOwner->SetDocument(theTarget);
  theTarget->SetModificationMode(aModificationMode);
  // Import changes the target, including when it already has a saved file.
  theTarget->SetSavedTime(-1);
}
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(DEXCAF_Provider, DE_Provider)

//=================================================================================================

DEXCAF_Provider::DEXCAF_Provider() = default;

//=================================================================================================

DEXCAF_Provider::DEXCAF_Provider(const occ::handle<DE_ConfigurationNode>& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&  theWS,
                            const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEXCAF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  occ::handle<DEXCAF_ConfigurationNode> aNode = occ::down_cast<DEXCAF_ConfigurationNode>(GetNode());
  if (aNode->InternalParameters.ReadAppendMode != PCDM_ReaderFilter::AppendMode_Forbid)
  {
    const std::shared_ptr<std::istream> aStream =
      OSD_FileSystem::DefaultFileSystem()->OpenIStream(thePath, std::ios::in | std::ios::binary);
    if (!aStream || !aStream->good())
    {
      Message::SendFail() << "Cannot open XCAF file for append: " << thePath;
      return false;
    }
    ReadStreamList aStreams;
    aStreams.Append(ReadStreamNode(thePath, *aStream));
    return Read(aStreams, theDocument, theProgress);
  }
  occ::handle<TDocStd_Document>         aDocument;
  const occ::handle<TDocStd_Application> anApp   = makeReadApplication();
  const occ::handle<PCDM_ReaderFilter>   aFilter = makeReadFilter(*aNode);

  if (anApp->Open(thePath, aDocument, aFilter, theProgress) != PCDM_RS_OK)
  {
    if (!aDocument.IsNull() && aDocument->IsOpened())
    {
      anApp->Close(aDocument);
    }
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file : " << thePath
                        << "\t: Cannot open XDE document";
    return false;
  }
  assignReadDocument(anApp, aDocument, theDocument);
  return true;
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);

  occ::handle<DEXCAF_ConfigurationNode> aNode = occ::down_cast<DEXCAF_ConfigurationNode>(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning()
      << "Warning in the DEXCAF_Provider during writing the file " << thePath
      << "\t: Target Units for writing were changed, but current format doesn't support scaling";
  }

  PCDM_StoreStatus aStatus = PCDM_SS_Doc_IsNull;
  if (!thePath.IsEmpty())
  {
    aStatus = anApp->SaveAs(theDocument, thePath, theProgress);
  }
  else if (!theDocument->IsSaved())
  {
    Message::SendFail() << "Storage error in the DEXCAF_Provider during writing the file "
                        << thePath << "\t: Storage error : this document has never been saved";
    return false;
  }
  else
  {
    aStatus = anApp->Save(theDocument, theProgress);
  }

  switch (aStatus)
  {
    case PCDM_SS_OK:
      return true;
    case PCDM_SS_DriverFailure:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : driver failure";
      break;
    case PCDM_SS_WriteFailure:
      Message::SendFail() << "Error in the DEXCAF_Provider during the writing the file : "
                          << thePath << "\t: Storage error : write failure";
      break;
    case PCDM_SS_Failure:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : general failure";
      break;
    case PCDM_SS_Doc_IsNull:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error :: document is NULL";
      break;
    case PCDM_SS_No_Obj:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : no object";
      break;
    case PCDM_SS_Info_Section_Error:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : section error";
      break;
    case PCDM_SS_UserBreak:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : user break";
      break;
    case PCDM_SS_UnrecognizedFormat:
      Message::SendFail() << "Error in the DEXCAF_Provider during writing the file : " << thePath
                          << "\t: Storage error : unrecognized document storage format : "
                          << theDocument->StorageFormat();
      break;
  }
  return false;
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString&      thePath,
                           TopoDS_Shape&                       theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString&      thePath,
                            const TopoDS_Shape&                 theShape,
                            occ::handle<XSControl_WorkSession>& theWS,
                            const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  theShape.Nullify();
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEXCAF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  if (!Read(thePath, aDocument, theProgress))
  {
    return false;
  }
  return documentShape(aDocument, theShape);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  occ::handle<TDocStd_Document>  aDoc    = new TDocStd_Document("BinXCAF");
  occ::handle<XCAFDoc_ShapeTool> aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEXCAF_Provider::GetFormat() const
{
  return TCollection_AsciiString("XCAF");
}

//=================================================================================================

TCollection_AsciiString DEXCAF_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

// ==================================================================================================

bool DEXCAF_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Read(theStreams, theDocument, theProgress);
}

// ==================================================================================================

bool DEXCAF_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&  theWS,
                            const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Write(theStreams, theDocument, theProgress);
}

// ==================================================================================================

bool DEXCAF_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  const TCollection_AsciiString aContext("reading an XCAF stream");
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext)
      || !DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                        STANDARD_TYPE(DEXCAF_ConfigurationNode),
                                                        aContext))
  {
    return false;
  }
  const occ::handle<DEXCAF_ConfigurationNode> aNode =
    occ::down_cast<DEXCAF_ConfigurationNode>(GetNode());
  const occ::handle<TDocStd_Application> anApp   = makeReadApplication();
  const occ::handle<PCDM_ReaderFilter>   aFilter = makeReadFilter(*aNode);
  occ::handle<TDocStd_Document> aDocument        = aFilter->IsAppendMode() ? theDocument : nullptr;
  const PCDM_ReaderStatus       aStatus =
    anApp->Open(theStreams.First().Stream, aDocument, aFilter, theProgress);
  if (aStatus != PCDM_RS_OK)
  {
    if (!aFilter->IsAppendMode() && !aDocument.IsNull() && aDocument->IsOpened())
    {
      anApp->Close(aDocument);
    }
    Message::SendFail() << "XCAF stream read failed, status " << static_cast<int>(aStatus);
    return false;
  }
  if (!aFilter->IsAppendMode())
  {
    assignReadDocument(anApp, aDocument, theDocument);
  }
  return true;
}

// ==================================================================================================

bool DEXCAF_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  const TCollection_AsciiString aContext("writing an XCAF stream");
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext)
      || !DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                        STANDARD_TYPE(DEXCAF_ConfigurationNode),
                                                        aContext))
  {
    return false;
  }
  const occ::handle<DEXCAF_ConfigurationNode> aNode =
    occ::down_cast<DEXCAF_ConfigurationNode>(GetNode());
  if (aNode->GlobalParameters.LengthUnit != 1.0)
  {
    Message::SendWarning(
      "XCAF stream export preserves document units; target units do not rescale shapes");
  }
  const occ::handle<TDocStd_Application> anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  const PCDM_StoreStatus aStatus =
    anApp->SaveAs(theDocument, theStreams.First().Stream, theProgress);
  if (aStatus != PCDM_SS_OK || !theStreams.First().Stream.good())
  {
    Message::SendFail() << "XCAF stream write failed, status " << static_cast<int>(aStatus);
    return false;
  }
  return true;
}

// ==================================================================================================

bool DEXCAF_Provider::Read(ReadStreamList&                     theStreams,
                           TopoDS_Shape&                       theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Read(theStreams, theShape, theProgress);
}

// ==================================================================================================

bool DEXCAF_Provider::Write(WriteStreamList&                    theStreams,
                            const TopoDS_Shape&                 theShape,
                            occ::handle<XSControl_WorkSession>& theWS,
                            const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Write(theStreams, theShape, theProgress);
}

// ==================================================================================================

bool DEXCAF_Provider::Read(ReadStreamList&              theStreams,
                           TopoDS_Shape&                theShape,
                           const Message_ProgressRange& theProgress)
{
  theShape.Nullify();
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  return Read(theStreams, aDocument, theProgress) && documentShape(aDocument, theShape);
}

// ==================================================================================================

bool DEXCAF_Provider::Write(WriteStreamList&             theStreams,
                            const TopoDS_Shape&          theShape,
                            const Message_ProgressRange& theProgress)
{
  if (theShape.IsNull())
  {
    Message::SendFail("Cannot write a null shape to an XCAF stream");
    return false;
  }
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->AddShape(theShape);
  return Write(theStreams, aDocument, theProgress);
}
