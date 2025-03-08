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
#include <DEXCAF_ConfigurationNode.hxx>
#include <Message.hxx>
#include <TDocStd_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEXCAF_Provider, DE_Provider)

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
  Handle(DEXCAF_ConfigurationNode) aNode = Handle(DEXCAF_ConfigurationNode)::DownCast(GetNode());
  Handle(TDocStd_Document)         aDocument;
  Handle(TDocStd_Application)      anApp = new TDocStd_Application();
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
  Handle(PCDM_ReaderFilter) aFilter =
    new PCDM_ReaderFilter(aNode->InternalParameters.ReadAppendMode);
  for (TColStd_ListOfAsciiString::Iterator anIt(aNode->InternalParameters.ReadSkipValues);
       anIt.More();
       anIt.Next())
  {
    aFilter->AddSkipped(anIt.Value());
  }
  for (TColStd_ListOfAsciiString::Iterator anIt(aNode->InternalParameters.ReadValues); anIt.More();
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
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);

  Handle(DEXCAF_ConfigurationNode) aNode = Handle(DEXCAF_ConfigurationNode)::DownCast(GetNode());
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
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEXCAF_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file " << thePath
                        << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(TDocStd_Document)    aDocument;
  Handle(TDocStd_Application) anApp = new TDocStd_Application();
  BinXCAFDrivers::DefineFormat(anApp);
  anApp->NewDocument("BinXCAF", aDocument);
  Read(thePath, aDocument, theProgress);
  TDF_LabelSequence         aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the DEXCAF_Provider during reading the file : " << thePath
                        << "\t: Document contain no shapes";
    return false;
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
  return true;
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

TCollection_AsciiString DEXCAF_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
