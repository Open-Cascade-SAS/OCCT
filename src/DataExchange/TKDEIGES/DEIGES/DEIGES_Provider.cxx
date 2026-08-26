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

#include <DEIGES_Provider.hxx>

#include <DE_ValidationUtils.hxx>
#include <DEIGES_ConfigurationNode.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <UnitsMethods.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>

#include <ios>
#include <mutex>
#include <utility>

IMPLEMENT_STANDARD_RTTIEXT(DEIGES_Provider, DE_Provider)

namespace
{
//! Executes a callable when leaving the current scope.
//! @tparam Function callable type
template <typename Function>
class DEIGES_ScopeGuard
{
public:
  //! Stores the cleanup callable.
  //! @param[in] theFunction cleanup callable
  explicit DEIGES_ScopeGuard(Function&& theFunction)
      : myFunction(std::move(theFunction))
  {
  }

  //! Executes the cleanup callable.
  ~DEIGES_ScopeGuard() { myFunction(); }

  DEIGES_ScopeGuard(const DEIGES_ScopeGuard&)            = delete;
  DEIGES_ScopeGuard& operator=(const DEIGES_ScopeGuard&) = delete;

private:
  Function myFunction;
};

//! Creates a scope guard while preserving the callable type.
//! @param[in] theFunction cleanup callable
//! @return scope guard owning the callable
template <typename Function>
DEIGES_ScopeGuard<Function> makeScopeGuard(Function&& theFunction)
{
  return DEIGES_ScopeGuard<Function>(std::forward<Function>(theFunction));
}

//! Validates that a configuration node is suitable for this provider.
//! @param[in] theNode configuration node to validate
//! @param[in] theContext operation description used in diagnostics
//! @return true if the node is a valid IGES configuration node
bool validateConfigurationNode(const occ::handle<DE_ConfigurationNode>& theNode,
                               const TCollection_AsciiString&           theContext)
{
  return DE_ValidationUtils::ValidateConfigurationNode(theNode,
                                                       STANDARD_TYPE(DEIGES_ConfigurationNode),
                                                       theContext);
}

//! Applies configuration parameters to a CAF reader.
//! @param[in,out] theReader reader to configure
//! @param[in] theNode IGES configuration node
void configureIGESCAFReader(IGESCAFControl_Reader&                       theReader,
                            const occ::handle<DEIGES_ConfigurationNode>& theNode)
{
  theReader.SetReadVisible(theNode->InternalParameters.ReadOnlyVisible);
  theReader.SetColorMode(theNode->InternalParameters.ReadColor);
  theReader.SetNameMode(theNode->InternalParameters.ReadName);
  theReader.SetLayerMode(theNode->InternalParameters.ReadLayer);
  theReader.SetShapeFixParameters(theNode->ShapeFixParameters);
}

//! Applies configuration parameters to a shape reader.
//! @param[in,out] theReader reader to configure
//! @param[in] theNode IGES configuration node
void configureIGESControlReader(IGESControl_Reader&                          theReader,
                                const occ::handle<DEIGES_ConfigurationNode>& theNode)
{
  theReader.SetReadVisible(theNode->InternalParameters.ReadOnlyVisible);
  theReader.SetShapeFixParameters(theNode->ShapeFixParameters);
}

//! Configures IGES units in a global section.
//! @param[in,out] theGlobalSection global section to configure
//! @param[in] theNode IGES configuration node
//! @param[in] theDocument source document for unit information
//! @param[in] thePath output path or stream key used in diagnostics
//! @param[in] theUseDocumentUnits whether document units should take precedence
void setupIGESUnits(IGESData_GlobalSection&                      theGlobalSection,
                    const occ::handle<DEIGES_ConfigurationNode>& theNode,
                    const occ::handle<TDocStd_Document>&         theDocument,
                    const TCollection_AsciiString&               thePath,
                    bool                                         theUseDocumentUnits)
{
  const int aUnitFlag = IGESData_BasicEditor::GetFlagByValue(theNode->GlobalParameters.LengthUnit);

  if (theUseDocumentUnits && !theDocument.IsNull())
  {
    double     aScaleFactorMM = 1.;
    const bool aHasUnits      = XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                                               aScaleFactorMM,
                                                               UnitsMethods_LengthUnit_Millimeter);
    if (aHasUnits)
    {
      theGlobalSection.SetCascadeUnit(aScaleFactorMM);
    }
    else
    {
      theGlobalSection.SetCascadeUnit(theNode->GlobalParameters.SystemUnit);
      Message::SendWarning()
        << "Warning in the DEIGES_Provider during writing the file " << thePath
        << "\t: The document has no information on Units. Using global parameter as initial Unit.";
    }
  }
  else
  {
    theGlobalSection.SetCascadeUnit(theNode->GlobalParameters.SystemUnit);
  }

  if (aUnitFlag == 0)
  {
    theGlobalSection.SetScale(theNode->GlobalParameters.LengthUnit);
  }
}

//! Applies configuration parameters and units to a CAF writer.
//! @param[in,out] theWriter writer to configure
//! @param[in] theNode IGES configuration node
//! @param[in] theDocument source document for unit information
//! @param[in] thePath output path or stream key used in diagnostics
void configureIGESCAFWriter(IGESCAFControl_Writer&                       theWriter,
                            const occ::handle<DEIGES_ConfigurationNode>& theNode,
                            const occ::handle<TDocStd_Document>&         theDocument,
                            const TCollection_AsciiString&               thePath)
{
  IGESData_GlobalSection aGlobalSection = theWriter.Model()->GlobalSection();
  setupIGESUnits(aGlobalSection, theNode, theDocument, thePath, true);

  theWriter.Model()->SetGlobalSection(aGlobalSection);
  theWriter.SetColorMode(theNode->InternalParameters.WriteColor);
  theWriter.SetNameMode(theNode->InternalParameters.WriteName);
  theWriter.SetLayerMode(theNode->InternalParameters.WriteLayer);
  theWriter.SetShapeFixParameters(theNode->ShapeFixParameters);
}

//! Applies configuration parameters and units to a shape writer.
//! @param[in,out] theWriter writer to configure
//! @param[in] theNode IGES configuration node
void configureIGESControlWriter(IGESControl_Writer&                          theWriter,
                                const occ::handle<DEIGES_ConfigurationNode>& theNode)
{
  IGESData_GlobalSection              aGlobalSection = theWriter.Model()->GlobalSection();
  const occ::handle<TDocStd_Document> aNullDocument;
  setupIGESUnits(aGlobalSection, theNode, aNullDocument, "", false);

  theWriter.Model()->SetGlobalSection(aGlobalSection);
  theWriter.SetShapeFixParameters(theNode->ShapeFixParameters);
}

//! Resolves the configured IGES unit name.
//! @param[in] theNode IGES configuration node
//! @return IGES unit name, or MM when no standard flag matches
TCollection_AsciiString getIGESUnitString(const occ::handle<DEIGES_ConfigurationNode>& theNode)
{
  const int aUnitFlag = IGESData_BasicEditor::GetFlagByValue(theNode->GlobalParameters.LengthUnit);
  return (aUnitFlag > 0) ? IGESData_BasicEditor::UnitFlagName(aUnitFlag) : "MM";
}

//! Returns the mutex serializing access to process-wide IGES settings.
//! @return shared IGES operation mutex
std::mutex& globalIGESMutex()
{
  static std::mutex THE_GLOBAL_IGES_MUTEX;
  return THE_GLOBAL_IGES_MUTEX;
}

//! Checks whether an output stream can accept data.
//! @param[in] theStream stream to check
//! @param[in] theKey stream key used in diagnostics
//! @return true if the stream is ready for writing
bool checkStreamWritability(Standard_OStream& theStream, const TCollection_AsciiString& theKey)
{
  if (!theStream.good())
  {
    const TCollection_AsciiString aKeyInfo = theKey.IsEmpty() ? "<empty key>" : theKey;
    Message::SendFail() << "Error: Output stream '" << aKeyInfo
                        << "' is not in good state for writing";
    return false;
  }

  return true;
}

//! Reads an IGES file and reports a diagnostic on failure.
//! @param[in,out] theReader reader receiving the file model
//! @param[in] thePath input file path
//! @return true if the model was read successfully
bool processReadFile(IGESControl_Reader& theReader, const TCollection_AsciiString& thePath)
{
  const IFSelect_ReturnStatus aReadStatus = theReader.ReadFile(thePath.ToCString());
  if (aReadStatus != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    return false;
  }
  return true;
}

} // namespace

//==================================================================================================

DEIGES_Provider::DEIGES_Provider() = default;

//==================================================================================================

DEIGES_Provider::DEIGES_Provider(const occ::handle<DE_ConfigurationNode>& theNode)
    : DE_Provider(theNode)
{
}

//==================================================================================================

void DEIGES_Provider::personizeWS(occ::handle<XSControl_WorkSession>& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DEIGES_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  occ::handle<IGESControl_Controller> aCntrl =
    occ::down_cast<IGESControl_Controller>(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    IGESControl_Controller::Init();
    theWS->SelectNorm("IGES");
  }
}

//==================================================================================================

void DEIGES_Provider::initStatic(const occ::handle<DE_ConfigurationNode>& theNode)
{
  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(theNode);
  IGESData::Init();

  myOldValues.ReadBSplineContinuity = static_cast<DEIGES_Parameters::ReadMode_BSplineContinuity>(
    Interface_Static::IVal("read.iges.bspline.continuity"));
  myOldValues.ReadPrecisionMode = static_cast<DEIGES_Parameters::ReadMode_Precision>(
    Interface_Static::IVal("read.precision.mode"));
  myOldValues.ReadPrecisionVal     = Interface_Static::RVal("read.precision.val");
  myOldValues.ReadMaxPrecisionMode = static_cast<DEIGES_Parameters::ReadMode_MaxPrecision>(
    Interface_Static::IVal("read.maxprecision.mode"));
  myOldValues.ReadMaxPrecisionVal  = Interface_Static::RVal("read.maxprecision.val");
  myOldValues.ReadSameParamMode    = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  myOldValues.ReadSurfaceCurveMode = static_cast<DEIGES_Parameters::ReadMode_SurfaceCurve>(
    Interface_Static::IVal("read.surfacecurve.mode"));
  myOldValues.EncodeRegAngle = Interface_Static::RVal("read.encoderegularity.angle") * 180.0 / M_PI;

  myOldValues.ReadApproxd1       = Interface_Static::IVal("read.iges.bspline.approxd1.mode") == 1;
  myOldValues.ReadFaultyEntities = Interface_Static::IVal("read.iges.faulty.entities") == 1;
  myOldValues.ReadOnlyVisible    = Interface_Static::IVal("read.iges.onlyvisible") == 1;

  myOldValues.WriteBRepMode =
    static_cast<DEIGES_Parameters::WriteMode_BRep>(Interface_Static::IVal("write.iges.brep.mode"));
  myOldValues.WriteConvertSurfaceMode = static_cast<DEIGES_Parameters::WriteMode_ConvertSurface>(
    Interface_Static::IVal("write.convertsurface.mode"));
  myOldValues.WriteHeaderAuthor   = Interface_Static::CVal("write.iges.header.author");
  myOldValues.WriteHeaderCompany  = Interface_Static::CVal("write.iges.header.company");
  myOldValues.WriteHeaderProduct  = Interface_Static::CVal("write.iges.header.product");
  myOldValues.WriteHeaderReciever = Interface_Static::CVal("write.iges.header.receiver");
  myOldValues.WritePrecisionMode  = static_cast<DEIGES_Parameters::WriteMode_PrecisionMode>(
    Interface_Static::IVal("write.precision.mode"));
  myOldValues.WritePrecisionVal = Interface_Static::RVal("write.precision.val");
  myOldValues.WritePlaneMode    = static_cast<DEIGES_Parameters::WriteMode_PlaneMode>(
    Interface_Static::IVal("write.iges.plane.mode"));
  myOldValues.WriteOffsetMode = Interface_Static::IVal("write.iges.offset.mode") == 1;

  myOldLengthUnit = Interface_Static::IVal("xstep.cascade.unit");

  UnitsMethods::SetCasCadeLengthUnit(aNode->GlobalParameters.LengthUnit,
                                     UnitsMethods_LengthUnit_Millimeter);
  TCollection_AsciiString aUnitName(
    UnitsMethods::DumpLengthUnit(aNode->GlobalParameters.LengthUnit));
  aUnitName.UpperCase();
  Interface_Static::SetCVal("xstep.cascade.unit", aUnitName.ToCString());
  setStatic(aNode->InternalParameters);
}

//==================================================================================================

void DEIGES_Provider::setStatic(const DEIGES_Parameters& theParameters)
{
  Interface_Static::SetIVal("read.iges.bspline.continuity", theParameters.ReadBSplineContinuity);
  Interface_Static::SetIVal("read.precision.mode", theParameters.ReadPrecisionMode);
  Interface_Static::SetRVal("read.precision.val", theParameters.ReadPrecisionVal);
  Interface_Static::SetIVal("read.maxprecision.mode", theParameters.ReadMaxPrecisionMode);
  Interface_Static::SetRVal("read.maxprecision.val", theParameters.ReadMaxPrecisionVal);
  Interface_Static::SetIVal("read.stdsameparameter.mode", theParameters.ReadSameParamMode);
  Interface_Static::SetIVal("read.surfacecurve.mode", theParameters.ReadSurfaceCurveMode);
  Interface_Static::SetRVal("read.encoderegularity.angle",
                            theParameters.EncodeRegAngle * M_PI / 180.0);

  Interface_Static::SetIVal("read.iges.bspline.approxd1.mode", theParameters.ReadApproxd1);
  Interface_Static::SetIVal("read.iges.faulty.entities", theParameters.ReadFaultyEntities);
  Interface_Static::SetIVal("read.iges.onlyvisible", theParameters.ReadOnlyVisible);

  Interface_Static::SetIVal("write.iges.brep.mode", theParameters.WriteBRepMode);
  Interface_Static::SetIVal("write.convertsurface.mode", theParameters.WriteConvertSurfaceMode);
  Interface_Static::SetCVal("write.iges.header.author",
                            theParameters.WriteHeaderAuthor.ToCString());
  Interface_Static::SetCVal("write.iges.header.company",
                            theParameters.WriteHeaderCompany.ToCString());
  Interface_Static::SetCVal("write.iges.header.product",
                            theParameters.WriteHeaderProduct.ToCString());
  Interface_Static::SetCVal("write.iges.header.receiver",
                            theParameters.WriteHeaderReciever.ToCString());
  Interface_Static::SetIVal("write.precision.mode", theParameters.WritePrecisionMode);
  Interface_Static::SetRVal("write.precision.val", theParameters.WritePrecisionVal);
  Interface_Static::SetIVal("write.iges.plane.mode", theParameters.WritePlaneMode);
  Interface_Static::SetIVal("write.iges.offset.mode", theParameters.WriteOffsetMode);
}

//==================================================================================================

void DEIGES_Provider::resetStatic()
{
  Interface_Static::SetIVal("xstep.cascade.unit", myOldLengthUnit);
  UnitsMethods::SetCasCadeLengthUnit(myOldLengthUnit);
  setStatic(myOldValues);
}

//==================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !validateConfigurationNode(GetNode(), aContext))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);

  IGESCAFControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESCAFReader(aReader, aNode);

  const IFSelect_ReturnStatus aReadStatus = aReader.ReadFile(thePath.ToCString());
  if (aReadStatus != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&  theWS,
                            const Message_ProgressRange&         theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !validateConfigurationNode(GetNode(), aContext))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESCAFControl_Writer aWriter(theWS, false);
  configureIGESCAFWriter(aWriter, aNode, theDocument, thePath);

  if (!aWriter.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing the file " << thePath
                        << "\t: The document cannot be translated or gives no result";
    return false;
  }
  if (!aWriter.Write(thePath.ToCString()))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing the file " << thePath
                        << "\t: Write failed";
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString&       thePath,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Write(thePath, theDocument, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString&      thePath,
                           TopoDS_Shape&                       theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  std::lock_guard<std::mutex> aLock(globalIGESMutex());
  if (!validateConfigurationNode(GetNode(), TCollection_AsciiString("reading the file ") + thePath))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESControlReader(aReader, aNode);

  if (!processReadFile(aReader, thePath))
  {
    return false;
  }

  if (aReader.TransferRoots(theProgress) <= 0)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    return false;
  }
  theShape = aReader.OneShape();
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString&      thePath,
                            const TopoDS_Shape&                 theShape,
                            occ::handle<XSControl_WorkSession>& theWS,
                            const Message_ProgressRange&        theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = TCollection_AsciiString("writing the file ") + thePath;
  if (!validateConfigurationNode(GetNode(), aContext))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESControl_Writer aWriter(getIGESUnitString(aNode).ToCString(),
                             aNode->InternalParameters.WriteBRepMode);
  configureIGESControlWriter(aWriter, aNode);

  const bool aIsOk = aWriter.AddShape(theShape, theProgress);
  if (!aIsOk)
  {
    Message::SendFail() << "DEIGES_Provider: Shape not written";
    return false;
  }

  theWS->SetModel(aWriter.Model(), false);
  theWS->SetMapWriter(aWriter.TransferProcess());

  if (!aWriter.Write(thePath.ToCString()))
  {
    Message::SendFail() << "DEIGES_Provider: Error on writing file " << thePath;
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Write(thePath, theShape, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext)
      || !validateConfigurationNode(GetNode(), aFullContext))
  {
    return false;
  }

  Standard_IStream& aStream = theStreams.First().Stream;

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);

  IGESCAFControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESCAFReader(aReader, aNode);

  IFSelect_ReturnStatus aReadStatus = IFSelect_RetFail;
  try
  {
    aReadStatus = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  }
  catch (const std::ios_base::failure& theFailure)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: " << theFailure.what();
    return false;
  }
  if (aReadStatus != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: abandon, no model loaded";
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Cannot read any relevant data from the IGES stream";
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            occ::handle<XSControl_WorkSession>&  theWS,
                            const Message_ProgressRange&         theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aFullContext)
      || !validateConfigurationNode(GetNode(), aFullContext))
  {
    return false;
  }

  Standard_OStream& aStream = theStreams.First().Stream;
  if (!checkStreamWritability(aStream, aFirstKey))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESCAFControl_Writer aWriter(theWS, false);
  configureIGESCAFWriter(aWriter, aNode, theDocument, aFirstKey);

  if (!aWriter.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << "\t: The document cannot be translated or gives no result";
    return false;
  }

  bool isWritten = false;
  try
  {
    isWritten = aWriter.Write(aStream);
  }
  catch (const std::ios_base::failure& theFailure)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << "\t: " << theFailure.what();
    return false;
  }
  if (!isWritten)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << "\t: Write failed";
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Read(ReadStreamList&                      theStreams,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Read(theStreams, theDocument, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Write(WriteStreamList&                     theStreams,
                            const occ::handle<TDocStd_Document>& theDocument,
                            const Message_ProgressRange&         theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Write(theStreams, theDocument, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Read(ReadStreamList&                     theStreams,
                           TopoDS_Shape&                       theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = "reading stream";
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!validateConfigurationNode(GetNode(), aFullContext))
  {
    return false;
  }

  Standard_IStream& aStream = theStreams.First().Stream;

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESControlReader(aReader, aNode);

  IFSelect_ReturnStatus aReadStatus = IFSelect_RetFail;
  try
  {
    aReadStatus = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  }
  catch (const std::ios_base::failure& theFailure)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: " << theFailure.what();
    return false;
  }
  if (aReadStatus != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Could not read stream, no model loaded";
    return false;
  }

  if (aReader.TransferRoots(theProgress) <= 0)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Cannot read any relevant data from the IGES stream";
    return false;
  }
  theShape = aReader.OneShape();
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Write(WriteStreamList&                    theStreams,
                            const TopoDS_Shape&                 theShape,
                            occ::handle<XSControl_WorkSession>& theWS,
                            const Message_ProgressRange&        theProgress)
{
  std::lock_guard<std::mutex>   aLock(globalIGESMutex());
  const TCollection_AsciiString aContext = "writing stream";
  if (!DE_ValidationUtils::ValidateWriteStreamList(theStreams, aContext))
  {
    return false;
  }

  const TCollection_AsciiString aFirstKey    = theStreams.First().Path;
  const TCollection_AsciiString aFullContext = aContext + " " + aFirstKey;
  if (!validateConfigurationNode(GetNode(), aFullContext))
  {
    return false;
  }

  Standard_OStream& aStream = theStreams.First().Stream;
  if (!checkStreamWritability(aStream, aFirstKey))
  {
    return false;
  }

  const occ::handle<DEIGES_ConfigurationNode> aNode =
    occ::down_cast<DEIGES_ConfigurationNode>(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  const auto aResetGuard = makeScopeGuard([this]() { resetStatic(); });

  IGESControl_Writer aWriter(getIGESUnitString(aNode).ToCString(),
                             aNode->InternalParameters.WriteBRepMode);
  configureIGESControlWriter(aWriter, aNode);

  const bool aIsOk = aWriter.AddShape(theShape, theProgress);
  if (!aIsOk)
  {
    Message::SendFail() << "Error: DEIGES_Provider failed to transfer shape for stream "
                        << aFirstKey;
    return false;
  }

  theWS->SetModel(aWriter.Model(), false);
  theWS->SetMapWriter(aWriter.TransferProcess());

  bool isWritten = false;
  try
  {
    isWritten = aWriter.Write(aStream);
  }
  catch (const std::ios_base::failure& theFailure)
  {
    Message::SendFail() << "Error: DEIGES_Provider failed to write shape to stream " << aFirstKey
                        << "\t: " << theFailure.what();
    return false;
  }
  if (!isWritten)
  {
    Message::SendFail() << "Error: DEIGES_Provider failed to write shape to stream " << aFirstKey;
    return false;
  }
  return true;
}

//==================================================================================================

bool DEIGES_Provider::Read(ReadStreamList&              theStreams,
                           TopoDS_Shape&                theShape,
                           const Message_ProgressRange& theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Read(theStreams, theShape, aWS, theProgress);
}

//==================================================================================================

bool DEIGES_Provider::Write(WriteStreamList&             theStreams,
                            const TopoDS_Shape&          theShape,
                            const Message_ProgressRange& theProgress)
{
  occ::handle<XSControl_WorkSession> aWS = new XSControl_WorkSession();
  return Write(theStreams, theShape, aWS, theProgress);
}

//==================================================================================================

TCollection_AsciiString DEIGES_Provider::GetFormat() const
{
  return TCollection_AsciiString("IGES");
}

//==================================================================================================

TCollection_AsciiString DEIGES_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
