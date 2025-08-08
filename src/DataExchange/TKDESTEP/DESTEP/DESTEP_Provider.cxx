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

#include <DESTEP_Provider.hxx>

#include <DE_ValidationUtils.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <DESTEP_Parameters.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <OSD_OpenFile.hxx>
#include <fstream>

IMPLEMENT_STANDARD_RTTIEXT(DESTEP_Provider, DE_Provider)

namespace
{
//! Helper function to validate configuration node
Standard_Boolean validateNode(const Handle(DE_ConfigurationNode)& theNode,
                              const TCollection_AsciiString&      theContext)
{
  return DE_ValidationUtils::ValidateConfigurationNode(theNode,
                                                       STANDARD_TYPE(DESTEP_ConfigurationNode),
                                                       theContext);
}

//! Configures STEPCAFControl_Reader with specified parameters and optional document setup.
//! @param[in,out] theReader STEP CAF reader to configure
//! @param[in] theParams Parameters containing read settings
//! @param[in] theWS Work session to initialize reader with (optional, if provided reader will
//! be initialized)
//! @param[in] theDocument Target document for length unit setup (optional)
//! @param[in] theLengthUnit Length unit for document setup (used only if theDocument is provided)
//! @param[in] theShapeFixParams Shape fix parameters (optional, uses default if not provided)
//! @note Sets up colors, names, layers, properties, metadata, and shape fix parameters
void configureSTEPCAFReader(STEPCAFControl_Reader&          theReader,
                            const DESTEP_Parameters&        theParams,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Handle(TDocStd_Document)& theDocument,
                            Standard_Real                   theLengthUnit,
                            const DE_ShapeFixParameters&    theShapeFixParams)
{
  theReader.Init(theWS);

  theReader.SetColorMode(theParams.ReadColor);
  theReader.SetNameMode(theParams.ReadName);
  theReader.SetLayerMode(theParams.ReadLayer);
  theReader.SetPropsMode(theParams.ReadProps);
  theReader.SetMetaMode(theParams.ReadMetadata);
  theReader.SetProductMetaMode(theParams.ReadProductMetadata);

  theReader.SetShapeFixParameters(theShapeFixParams);

  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      theLengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
}

//! Configures STEPCAFControl_Writer with full setup.
//! @param[in,out] theWriter STEP CAF writer to configure
//! @param[in] theParams Parameters containing write settings
//! @param[in,out] theWS Work session to initialize writer with
//! @param[in] theDocument Source document for length unit extraction
//! @param[in] theLengthUnit Length unit for document setup
//! @param[in] theShapeFixParams Shape fix parameters
//! @note Sets up all write parameters including colors, names, layers, props, materials
void configureSTEPCAFWriter(STEPCAFControl_Writer&          theWriter,
                            const DESTEP_Parameters&        theParams,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Handle(TDocStd_Document)& theDocument,
                            Standard_Real                   theLengthUnit,
                            const DE_ShapeFixParameters&    theShapeFixParams)
{
  theWriter.Init(theWS);

  theWriter.SetColorMode(theParams.WriteColor);
  theWriter.SetNameMode(theParams.WriteName);
  theWriter.SetLayerMode(theParams.WriteLayer);
  theWriter.SetPropsMode(theParams.WriteProps);
  theWriter.SetMaterialMode(theParams.WriteMaterial);
  theWriter.SetVisualMaterialMode(theParams.WriteVisMaterial);
  theWriter.SetCleanDuplicates(theParams.CleanDuplicates);

  theWriter.SetShapeFixParameters(theShapeFixParams);

  Handle(StepData_StepModel) aModel =
    Handle(StepData_StepModel)::DownCast(theWriter.Writer().WS()->Model());

  Standard_Real aScaleFactorMM = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter))
  {
    aModel->SetLocalLengthUnit(aScaleFactorMM);
  }
  else
  {
    aModel->SetLocalLengthUnit(theLengthUnit);
    Message::SendWarning()
      << "Warning in the DESTEP_Provider during writing"
      << "\t: The document has no information on Units. Using global parameter as initial Unit.";
  }
}

//! Checks if output stream is in writable state.
//! @param[in] theStream Output stream to check
//! @param[in] theKey Stream identifier for error reporting
//! @return Standard_True if stream is writable, Standard_False otherwise
bool checkStreamWritability(Standard_OStream& theStream, const TCollection_AsciiString& theKey)
{
  if (!theStream.good())
  {
    TCollection_AsciiString aKeyInfo = theKey.IsEmpty() ? "<empty key>" : theKey;
    Message::SendFail() << "Error: Output stream '" << aKeyInfo
                        << "' is not in good state for writing";
    return false;
  }

  return true;
}

} // namespace

//=================================================================================================

DESTEP_Provider::DESTEP_Provider() {}

//=================================================================================================

DESTEP_Provider::DESTEP_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DESTEP_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !validateNode(GetNode(), aContext))
  {
    return false;
  }

  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);

  STEPCAFControl_Reader aReader;
  configureSTEPCAFReader(aReader,
                         aNode->InternalParameters,
                         theWS,
                         theDocument,
                         aNode->GlobalParameters.LengthUnit,
                         aNode->ShapeFixParameters);

  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  DESTEP_Parameters     aParams   = aNode->InternalParameters;
  aReadStat                       = aReader.ReadFile(thePath.ToCString(), aParams);

  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DESTEP_Provider during reading the file " << thePath
                        << "\t: abandon";
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DESTEP_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the STEP file";
    return false;
  }
  return true;
}

//=================================================================================================

bool DESTEP_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !validateNode(GetNode(), aContext))
  {
    return false;
  }
  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);

  STEPCAFControl_Writer aWriter;
  configureSTEPCAFWriter(aWriter,
                         aNode->InternalParameters,
                         theWS,
                         theDocument,
                         aNode->GlobalParameters.SystemUnit,
                         aNode->ShapeFixParameters);

  Handle(StepData_StepModel) aModel =
    Handle(StepData_StepModel)::DownCast(aWriter.Writer().WS()->Model());
  STEPControl_StepModelType aMode =
    static_cast<STEPControl_StepModelType>(aNode->InternalParameters.WriteModelType);
  DESTEP_Parameters       aParams = aNode->InternalParameters;
  UnitsMethods_LengthUnit aTargetUnit =
    UnitsMethods::GetLengthUnitByFactorValue(aNode->GlobalParameters.LengthUnit,
                                             UnitsMethods_LengthUnit_Millimeter);
  aParams.WriteUnit = aTargetUnit;
  aModel->SetWriteLengthUnit(aNode->GlobalParameters.LengthUnit);
  TDF_Label aLabel;
  if (!aWriter.Transfer(theDocument, aParams, aMode, 0, theProgress))
  {
    Message::SendFail() << "Error in the DESTEP_Provider during writing the file " << thePath
                        << "\t: The document cannot be translated or gives no result";
    return false;
  }
  IFSelect_ReturnStatus aStatus = aWriter.Write(thePath.ToCString());
  switch (aStatus)
  {
    case IFSelect_RetVoid: {
      Message::SendFail() << "Error in the DESTEP_Provider during writing the file " << thePath
                          << "\t: No file written";
      return false;
      ;
    }
    case IFSelect_RetDone: {
      break;
    }
    default: {
      Message::SendFail() << "Error in the DESTEP_Provider during writing the file " << thePath
                          << "\t: Error on writing file";
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool DESTEP_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//=================================================================================================

bool DESTEP_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theDocument, aWS, theProgress);
}

//=================================================================================================

bool DESTEP_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!validateNode(GetNode(), aContext))
  {
    return false;
  }
  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  STEPControl_Reader aReader;
  aReader.SetWS(theWS);
  aReader.SetShapeFixParameters(aNode->ShapeFixParameters);
  IFSelect_ReturnStatus aReadstat   = IFSelect_RetVoid;
  DESTEP_Parameters     aParams     = aNode->InternalParameters;
  aReadstat                         = aReader.ReadFile(thePath.ToCString(), aParams);
  Handle(StepData_StepModel) aModel = aReader.StepModel();
  if (aReadstat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DESTEP_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    return false;
  }
  aModel->SetLocalLengthUnit(aNode->GlobalParameters.LengthUnit);
  if (aReader.TransferRoots(theProgress) <= 0)
  {
    Message::SendFail() << "Error in the DESTEP_Provider during reading the file " << thePath
                        << "\t:Cannot read any relevant data from the STEP file";
    return false;
  }
  theShape = aReader.OneShape();
  return true;
}

//=================================================================================================

bool DESTEP_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!validateNode(GetNode(), aContext))
  {
    return false;
  }
  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());

  personizeWS(theWS);
  STEPControl_Writer aWriter;
  aWriter.SetWS(theWS);
  IFSelect_ReturnStatus      aWritestat = IFSelect_RetVoid;
  Handle(StepData_StepModel) aModel     = aWriter.Model();
  ;
  DESTEP_Parameters aParams = aNode->InternalParameters;
  aModel->SetLocalLengthUnit(aNode->GlobalParameters.SystemUnit);
  UnitsMethods_LengthUnit aTargetUnit =
    UnitsMethods::GetLengthUnitByFactorValue(aNode->GlobalParameters.LengthUnit,
                                             UnitsMethods_LengthUnit_Millimeter);
  aParams.WriteUnit = aTargetUnit;
  if (aTargetUnit == UnitsMethods_LengthUnit_Undefined)
  {
    aModel->SetWriteLengthUnit(1.0);
    Message::SendWarning()
      << "Custom units are not supported by STEP format, but LengthUnit global parameter doesn't "
         "fit any predefined unit. Units will be scaled to Millimeters";
  }
  else
  {
    aModel->SetWriteLengthUnit(aNode->GlobalParameters.LengthUnit);
  }
  aWriter.SetShapeFixParameters(aNode->ShapeFixParameters);
  aWritestat = aWriter.Transfer(theShape,
                                aNode->InternalParameters.WriteModelType,
                                aParams,
                                true,
                                theProgress);
  if (aWritestat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DESTEP_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    return false;
  }
  if (aNode->InternalParameters.CleanDuplicates)
  {
    aWriter.CleanDuplicateEntities();
  }

  if (aWriter.Write(thePath.ToCString()) != IFSelect_RetDone)
  {
    Message::SendFail() << "DESTEP_Provider: Error on writing file";
    return false;
  }
  return true;
}

//=================================================================================================

bool DESTEP_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//=================================================================================================

bool DESTEP_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theShape, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Read(ReadStreamList&                 theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       Handle(XSControl_WorkSession)&  theWS,
                                       const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext)
      || !validateNode(GetNode(), aFullContext)
      || !DE_ValidationUtils::ValidateReadStreamList(theStreams, aFullContext))
  {
    return Standard_False;
  }

  Standard_IStream& aStream = theStreams.First().Stream;

  personizeWS(theWS);

  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());
  STEPCAFControl_Reader            aReader(theWS, Standard_False);
  configureSTEPCAFReader(aReader,
                         aNode->InternalParameters,
                         theWS,
                         theDocument,
                         aNode->GlobalParameters.LengthUnit,
                         aNode->ShapeFixParameters);

  Standard_Boolean isOk = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  if (!isOk)
  {
    Message::SendFail() << "Error: DESTEP_Provider failed to read stream " << aFirstKey;
    return Standard_False;
  }

  return aReader.Transfer(theDocument, theProgress);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Write(WriteStreamList&                theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)&  theWS,
                                        const Message_ProgressRange&    theProgress)
{
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext)
      || !validateNode(GetNode(), aFullContext))
  {
    return Standard_False;
  }

  Standard_OStream& aStream = theStreams.First().Stream;
  if (!checkStreamWritability(aStream, aFirstKey))
  {
    return Standard_False;
  }

  personizeWS(theWS);

  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());

  STEPCAFControl_Writer aWriter(theWS, Standard_False);
  configureSTEPCAFWriter(aWriter,
                         aNode->InternalParameters,
                         theWS,
                         theDocument,
                         aNode->GlobalParameters.LengthUnit,
                         aNode->ShapeFixParameters);

  Handle(StepData_StepModel) aModel =
    Handle(StepData_StepModel)::DownCast(aWriter.Writer().WS()->Model());
  DESTEP_Parameters       aParams = aNode->InternalParameters;
  UnitsMethods_LengthUnit aTargetUnit =
    UnitsMethods::GetLengthUnitByFactorValue(aNode->GlobalParameters.LengthUnit,
                                             UnitsMethods_LengthUnit_Millimeter);
  aParams.WriteUnit = aTargetUnit;
  aModel->SetWriteLengthUnit(aNode->GlobalParameters.LengthUnit);
  STEPControl_StepModelType aMode =
    static_cast<STEPControl_StepModelType>(aNode->InternalParameters.WriteModelType);
  Standard_Boolean isOk = aWriter.Transfer(theDocument, aParams, aMode, 0, theProgress);
  if (!isOk)
  {
    Message::SendFail() << "Error: DESTEP_Provider failed to transfer document for stream "
                        << aFirstKey;
    return Standard_False;
  }
  return aWriter.WriteStream(aStream);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Read(ReadStreamList&                 theStreams,
                                       const Handle(TDocStd_Document)& theDocument,
                                       const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(theStreams, theDocument, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Write(WriteStreamList&                theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(theStreams, theDocument, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Read(ReadStreamList&                theStreams,
                                       TopoDS_Shape&                  theShape,
                                       Handle(XSControl_WorkSession)& theWS,
                                       const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!validateNode(GetNode(), aFullContext))
  {
    return Standard_False;
  }

  Standard_IStream& aStream = theStreams.First().Stream;
  personizeWS(theWS);

  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());

  // Use STEPControl_Reader for shape operations from streams
  STEPControl_Reader aReader;
  aReader.SetWS(theWS);
  aReader.SetShapeFixParameters(aNode->ShapeFixParameters);

  // Read from stream using the reader's internal model
  IFSelect_ReturnStatus aReadStat = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error: DESTEP_Provider failed to read from stream " << aFirstKey;
    return Standard_False;
  }
  Handle(StepData_StepModel) aModel = aReader.StepModel();
  aModel->SetLocalLengthUnit(aNode->GlobalParameters.LengthUnit);

  // Transfer the first root to get the shape
  if (aReader.TransferRoots(theProgress) <= 0)
  {
    Message::SendFail() << "Error: DESTEP_Provider found no transferable roots in stream "
                        << aFirstKey;
    return Standard_False;
  }

  theShape = aReader.OneShape();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Write(WriteStreamList&               theStreams,
                                        const TopoDS_Shape&            theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange&   theProgress)
{
  TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return Standard_False;
  }

  TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!validateNode(GetNode(), aFullContext))
  {
    return Standard_False;
  }

  Standard_OStream& aStream = theStreams.First().Stream;
  personizeWS(theWS);

  Handle(DESTEP_ConfigurationNode) aNode = Handle(DESTEP_ConfigurationNode)::DownCast(GetNode());

  // Use STEPControl_Writer for shape operations to streams
  STEPControl_Writer aWriter;
  aWriter.SetWS(theWS);

  Handle(StepData_StepModel) aModel = aWriter.Model();
  aModel->SetLocalLengthUnit(aNode->GlobalParameters.SystemUnit);

  UnitsMethods_LengthUnit aTargetUnit =
    UnitsMethods::GetLengthUnitByFactorValue(aNode->GlobalParameters.LengthUnit,
                                             UnitsMethods_LengthUnit_Millimeter);
  DESTEP_Parameters aParams = aNode->InternalParameters;
  aParams.WriteUnit         = aTargetUnit;

  if (aTargetUnit == UnitsMethods_LengthUnit_Undefined)
  {
    aModel->SetWriteLengthUnit(1.0);
    Message::SendWarning()
      << "Custom units are not supported by STEP format, but LengthUnit global parameter doesn't "
         "fit any predefined unit. Units will be scaled to Millimeters";
  }
  else
  {
    aModel->SetWriteLengthUnit(aNode->GlobalParameters.LengthUnit);
  }

  aWriter.SetShapeFixParameters(aNode->ShapeFixParameters);

  IFSelect_ReturnStatus aWriteStat = aWriter.Transfer(theShape,
                                                      aNode->InternalParameters.WriteModelType,
                                                      aParams,
                                                      true,
                                                      theProgress);
  if (aWriteStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error: DESTEP_Provider failed to transfer shape for stream "
                        << aFirstKey;
    return Standard_False;
  }

  if (aNode->InternalParameters.CleanDuplicates)
  {
    aWriter.CleanDuplicateEntities();
  }

  // Write to stream
  if (!aWriter.WriteStream(aStream))
  {
    Message::SendFail() << "Error: DESTEP_Provider failed to write to stream " << aFirstKey;
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Read(ReadStreamList&              theStreams,
                                       TopoDS_Shape&                theShape,
                                       const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(theStreams, theShape, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DESTEP_Provider::Write(WriteStreamList&             theStreams,
                                        const TopoDS_Shape&          theShape,
                                        const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(theStreams, theShape, aWS, theProgress);
}

//=================================================================================================

TCollection_AsciiString DESTEP_Provider::GetFormat() const
{
  return TCollection_AsciiString("STEP");
}

//=================================================================================================

TCollection_AsciiString DESTEP_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

void DESTEP_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DESTEP_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  Handle(STEPCAFControl_Controller) aCntrl =
    Handle(STEPCAFControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    STEPCAFControl_Controller::Init();
    theWS->SelectNorm("STEP");
  }
}
