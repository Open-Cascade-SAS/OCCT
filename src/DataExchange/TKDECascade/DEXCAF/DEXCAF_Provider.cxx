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
#include <DE_ValidationUtils.hxx>
#include <StdDrivers.hxx>
#include <StdLDrivers.hxx>
#include <XmlDrivers.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>
#include <XmlXCAFDrivers.hxx>

#include <BRep_Builder.hxx>
#include <DEXCAF_ConfigurationNode.hxx>
#include <Message.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEXCAF_Provider, DE_Provider)

namespace
{
Standard_Boolean ValidateConfigurationNode(const Handle(DE_ConfigurationNode)& theNode,
                                           const TCollection_AsciiString&      theContext,
                                           Handle(DEXCAF_ConfigurationNode)&   theDowncastNode)
{
  if (!DE_ValidationUtils::ValidateConfigurationNode(theNode,
                                                     STANDARD_TYPE(DEXCAF_ConfigurationNode),
                                                     theContext))
  {
    return Standard_False;
  }
  theDowncastNode = Handle(DEXCAF_ConfigurationNode)::DownCast(theNode);
  return Standard_True;
}

void SetupApplication(Handle(TDocStd_Application)& theApp,
                      Standard_Boolean             theFullSetup = Standard_True)
{
  theApp = new TDocStd_Application();
  if (theFullSetup)
  {
    BinDrivers::DefineFormat(theApp);
    BinLDrivers::DefineFormat(theApp);
    BinTObjDrivers::DefineFormat(theApp);
    BinXCAFDrivers::DefineFormat(theApp);
    StdDrivers::DefineFormat(theApp);
    StdLDrivers::DefineFormat(theApp);
    XmlDrivers::DefineFormat(theApp);
    XmlLDrivers::DefineFormat(theApp);
    XmlTObjDrivers::DefineFormat(theApp);
    XmlXCAFDrivers::DefineFormat(theApp);
  }
  else
  {
    BinXCAFDrivers::DefineFormat(theApp);
  }
}

void ConfigureReaderFilter(Handle(PCDM_ReaderFilter)&              theFilter,
                           const Handle(DEXCAF_ConfigurationNode)& theNode)
{
  theFilter = new PCDM_ReaderFilter(theNode->InternalParameters.ReadAppendMode);
  for (TColStd_ListOfAsciiString::Iterator anIt(theNode->InternalParameters.ReadSkipValues);
       anIt.More();
       anIt.Next())
  {
    theFilter->AddSkipped(anIt.Value());
  }
  for (TColStd_ListOfAsciiString::Iterator anIt(theNode->InternalParameters.ReadValues);
       anIt.More();
       anIt.Next())
  {
    if (anIt.Value().StartsWith("0"))
    {
      theFilter->AddPath(anIt.Value());
    }
    else
    {
      theFilter->AddRead(anIt.Value());
    }
  }
}

Standard_Boolean ExtractShapeFromDocument(const Handle(TDocStd_Document)& theDocument,
                                          const TCollection_AsciiString&  theContext,
                                          TopoDS_Shape&                   theShape)
{
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);

  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                        << ": Document contain no shapes";
    return Standard_False;
  }

  if (aLabels.Length() == 1)
  {
    theShape = aSTool->GetShape(aLabels.Value(1));
  }
  else
  {
    TopoDS_Compound aComp;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound(aComp);
    for (Standard_Integer anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
    {
      TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
      aBuilder.Add(aComp, aS);
    }
    theShape = aComp;
  }
  return Standard_True;
}

Standard_Boolean HandlePCDMStatus(PCDM_StoreStatus                theStatus,
                                  const Handle(TDocStd_Document)& theDocument,
                                  const TCollection_AsciiString&  theContext)
{
  switch (theStatus)
  {
    case PCDM_SS_OK:
      return Standard_True;
    case PCDM_SS_DriverFailure:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : driver failure";
      break;
    case PCDM_SS_WriteFailure:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : write failure";
      break;
    case PCDM_SS_Failure:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : general failure";
      break;
    case PCDM_SS_Doc_IsNull:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error :: document is NULL";
      break;
    case PCDM_SS_No_Obj:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : no object";
      break;
    case PCDM_SS_Info_Section_Error:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : section error";
      break;
    case PCDM_SS_UserBreak:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : user break";
      break;
    case PCDM_SS_UnrecognizedFormat:
      Message::SendFail() << "Error in the DEXCAF_Provider during " << theContext
                          << ": Storage error : unrecognized document storage format : "
                          << theDocument->StorageFormat();
      break;
  }
  return Standard_False;
}

void CheckLengthUnitWarning(const Handle(DEXCAF_ConfigurationNode)& theNode,
                            const TCollection_AsciiString&          theContext)
{
  DE_ValidationUtils::WarnLengthUnitNotSupported(theNode->GlobalParameters.LengthUnit, theContext);
}
} // namespace

//=================================================================================================

DEXCAF_Provider::DEXCAF_Provider() {}

//=================================================================================================

DEXCAF_Provider::DEXCAF_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  if (!DE_ValidationUtils::ValidateDocument(theDocument,
                                            TCollection_AsciiString("reading the file ") + thePath))
  {
    return false;
  }

  Handle(DEXCAF_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(),
                                 TCollection_AsciiString("reading the file ") + thePath,
                                 aNode))
  {
    return false;
  }

  Handle(TDocStd_Document)    aDocument;
  Handle(TDocStd_Application) anApp;
  SetupApplication(anApp, Standard_True);

  Handle(PCDM_ReaderFilter) aFilter;
  ConfigureReaderFilter(aFilter, aNode);

  if (anApp->Open(thePath, aDocument, aFilter, theProgress) != PCDM_RS_OK)
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file : " << thePath
                        << "\t: Cannot open XDE document";
    return false;
  }
  theDocument->SetData(aDocument->GetData());
  return true;
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  Handle(TDocStd_Application) anApp;
  SetupApplication(anApp, Standard_False);

  Handle(DEXCAF_ConfigurationNode) aNode = Handle(DEXCAF_ConfigurationNode)::DownCast(GetNode());
  CheckLengthUnitWarning(aNode, TCollection_AsciiString("writing the file ") + thePath);

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

  return HandlePCDMStatus(aStatus,
                          theDocument,
                          TCollection_AsciiString("writing the file ") + thePath);
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEXCAF_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  Handle(DEXCAF_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(),
                                 TCollection_AsciiString("reading the file ") + thePath,
                                 aNode))
  {
    return false;
  }

  Handle(TDocStd_Document)    aDocument;
  Handle(TDocStd_Application) anApp;
  SetupApplication(anApp, Standard_False);
  anApp->NewDocument("BinXCAF", aDocument);

  if (!Read(thePath, aDocument, theProgress))
  {
    return false;
  }

  return ExtractShapeFromDocument(aDocument,
                                  TCollection_AsciiString("reading the file ") + thePath,
                                  theShape);
}

//=================================================================================================

bool DEXCAF_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  Handle(TDocStd_Document)  aDoc    = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEXCAF_Provider::GetFormat() const
{
  return TCollection_AsciiString("XCAF");
}

//=================================================================================================

Standard_Boolean DEXCAF_Provider::Read(ReadStreamList&                  theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       Handle(XSControl_WorkSession)&  theWS,
                                       const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Read(theStreams, theDocument, theProgress);
}

Standard_Boolean DEXCAF_Provider::Write(WriteStreamList&                 theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)&  theWS,
                                        const Message_ProgressRange&    theProgress)
{
  (void)theWS;
  return Write(theStreams, theDocument, theProgress);
}

Standard_Boolean DEXCAF_Provider::Read(ReadStreamList&                 theStreams,
                                       TopoDS_Shape&                  theShape,
                                       Handle(XSControl_WorkSession)& theWS,
                                       const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Read(theStreams, theShape, theProgress);
}

Standard_Boolean DEXCAF_Provider::Write(WriteStreamList&                theStreams,
                                        const TopoDS_Shape&            theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  return Write(theStreams, theShape, theProgress);
}

Standard_Boolean DEXCAF_Provider::Read(ReadStreamList&                  theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext))
  {
    return Standard_False;
  }

  Handle(DEXCAF_ConfigurationNode) aNode;
  if (!ValidateConfigurationNode(GetNode(), aFullContext, aNode))
  {
    return Standard_False;
  }

  Handle(TDocStd_Document)    aDocument;
  Handle(TDocStd_Application) anApp;
  SetupApplication(anApp, Standard_True);

  Handle(PCDM_ReaderFilter) aFilter;
  ConfigureReaderFilter(aFilter, aNode);

  Standard_IStream& aStream = theStreams.First().Stream;

  if (anApp->Open(aStream, aDocument, aFilter, theProgress) != PCDM_RS_OK)
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading stream " << aFirstKey
                        << ": Cannot open XDE document";
    return Standard_False;
  }

  theDocument->SetData(aDocument->GetData());
  return Standard_True;
}

Standard_Boolean DEXCAF_Provider::Write(WriteStreamList&                 theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey = theStreams.First().Path;

  Handle(TDocStd_Application) anApp;
  SetupApplication(anApp, Standard_False);

  Handle(DEXCAF_ConfigurationNode) aNode = Handle(DEXCAF_ConfigurationNode)::DownCast(GetNode());
  TCollection_AsciiString          aFullContext = aContext + " " + aFirstKey;
  CheckLengthUnitWarning(aNode, aFullContext);

  Standard_OStream& aStream = theStreams.First().Stream;
  PCDM_StoreStatus  aStatus = anApp->SaveAs(theDocument, aStream, theProgress);

  return HandlePCDMStatus(aStatus, theDocument, aFullContext);
}

Standard_Boolean DEXCAF_Provider::Read(ReadStreamList&               theStreams,
                                       TopoDS_Shape&                theShape,
                                       const Message_ProgressRange& theProgress)
{
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey = theStreams.First().Path;

  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");
  if (!Read(theStreams, aDoc, theProgress))
  {
    return Standard_False;
  }

  return ExtractShapeFromDocument(aDoc, aContext + " " + aFirstKey, theShape);
}

Standard_Boolean DEXCAF_Provider::Write(WriteStreamList&              theStreams,
                                        const TopoDS_Shape&          theShape,
                                        const Message_ProgressRange& theProgress)
{
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  Handle(TDocStd_Document)  aDoc    = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(theStreams, aDoc, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEXCAF_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
