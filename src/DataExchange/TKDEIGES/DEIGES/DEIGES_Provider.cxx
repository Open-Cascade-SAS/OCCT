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

IMPLEMENT_STANDARD_RTTIEXT(DEIGES_Provider, DE_Provider)

namespace
{
  //! Helper function to validate configuration node
  Standard_Boolean validateConfigurationNode(const Handle(DE_ConfigurationNode)& theNode,
                                              const TCollection_AsciiString& thePath,
                                              const TCollection_AsciiString& theOperation)
  {
    if (!theNode->IsKind(STANDARD_TYPE(DEIGES_ConfigurationNode)))
    {
      Message::SendFail() << "Error in the DEIGES_Provider during " << theOperation 
                          << " the file " << thePath
                          << "\t: Incorrect or empty Configuration Node";
      return Standard_False;
    }
    return Standard_True;
  }

  //! Helper function to validate stream maps and warn about multiple streams
  template<typename StreamMapType>
  Standard_Boolean validateStreamMap(const StreamMapType& theStreams)
  {
    if (theStreams.IsEmpty())
    {
      Message::SendFail() << "Error: DEIGES_Provider stream map is empty";
      return Standard_False;
    }
    if (theStreams.Size() > 1)
    {
      const TCollection_AsciiString& aFirstKey = theStreams.FindKey(1);
      Message::SendWarning() << "Warning: DEIGES_Provider received " << theStreams.Size()
                             << " streams for " << aFirstKey << ", using only the first one";
    }
    return Standard_True;
  }

  //! Helper function to configure IGES reader parameters
  void configureIGESReader(IGESCAFControl_Reader& theReader,
                           const Handle(DEIGES_ConfigurationNode)& theNode)
  {
    theReader.SetReadVisible(theNode->InternalParameters.ReadOnlyVisible);
    theReader.SetColorMode(theNode->InternalParameters.ReadColor);
    theReader.SetNameMode(theNode->InternalParameters.ReadName);
    theReader.SetLayerMode(theNode->InternalParameters.ReadLayer);
    theReader.SetShapeFixParameters(theNode->ShapeFixParameters);
  }

  //! Helper function to configure IGES control reader parameters
  void configureIGESControlReader(IGESControl_Reader& theReader,
                                  const Handle(DEIGES_ConfigurationNode)& theNode)
  {
    theReader.SetReadVisible(theNode->InternalParameters.ReadOnlyVisible);
    theReader.SetShapeFixParameters(theNode->ShapeFixParameters);
  }

  //! Helper function to configure IGES CAF writer parameters  
  void configureIGESCAFWriter(IGESCAFControl_Writer& theWriter,
                               const Handle(DEIGES_ConfigurationNode)& theNode,
                               const Handle(TDocStd_Document)& theDocument,
                               const TCollection_AsciiString& thePath)
  {
    Standard_Integer aFlag = IGESData_BasicEditor::GetFlagByValue(theNode->GlobalParameters.LengthUnit);
    IGESData_GlobalSection aGS = theWriter.Model()->GlobalSection();
    
    Standard_Real aScaleFactorMM = 1.;
    Standard_Boolean aHasUnits =
      XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter);
    if (aHasUnits)
    {
      aGS.SetCascadeUnit(aScaleFactorMM);
    }
    else
    {
      aGS.SetCascadeUnit(theNode->GlobalParameters.SystemUnit);
      Message::SendWarning()
        << "Warning in the DEIGES_Provider during writing the file " << thePath
        << "\t: The document has no information on Units. Using global parameter as initial Unit.";
    }
    
    if (aFlag == 0)
    {
      aGS.SetScale(theNode->GlobalParameters.LengthUnit);
    }
    
    theWriter.Model()->SetGlobalSection(aGS);
    theWriter.SetColorMode(theNode->InternalParameters.WriteColor);
    theWriter.SetNameMode(theNode->InternalParameters.WriteName);
    theWriter.SetLayerMode(theNode->InternalParameters.WriteLayer);
    theWriter.SetShapeFixParameters(theNode->ShapeFixParameters);
  }

  //! Helper function to configure IGES control writer for shapes
  void configureIGESControlWriter(IGESControl_Writer& theWriter,
                                  const Handle(DEIGES_ConfigurationNode)& theNode)
  {
    Standard_Integer aFlag = IGESData_BasicEditor::GetFlagByValue(theNode->GlobalParameters.LengthUnit);
    IGESData_GlobalSection aGS = theWriter.Model()->GlobalSection();
    aGS.SetCascadeUnit(theNode->GlobalParameters.SystemUnit);
    
    if (aFlag == 0)
    {
      aGS.SetScale(theNode->GlobalParameters.LengthUnit);
    }
    
    theWriter.Model()->SetGlobalSection(aGS);
    theWriter.SetShapeFixParameters(theNode->ShapeFixParameters);
  }

  //! Helper function to setup IGES writer unit flags
  TCollection_AsciiString getIGESUnitString(const Handle(DEIGES_ConfigurationNode)& theNode)
  {
    Standard_Integer aFlag = IGESData_BasicEditor::GetFlagByValue(theNode->GlobalParameters.LengthUnit);
    return (aFlag > 0) ? IGESData_BasicEditor::UnitFlagName(aFlag) : "MM";
  }

  //! Helper function to process read file operation
  Standard_Boolean processReadFile(IGESControl_Reader& theReader,
                                   const TCollection_AsciiString& thePath)
  {
    IFSelect_ReturnStatus aReadStat = theReader.ReadFile(thePath.ToCString());
    if (aReadStat != IFSelect_RetDone)
    {
      Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                          << "\t: abandon, no model loaded";
      return Standard_False;
    }
    return Standard_True;
  }


}

//=================================================================================================

DEIGES_Provider::DEIGES_Provider() {}

//=================================================================================================

DEIGES_Provider::DEIGES_Provider(const Handle(DE_ConfigurationNode)& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

void DEIGES_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: DEIGES_Provider :"
                           << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  Handle(IGESControl_Controller) aCntrl =
    Handle(IGESControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    IGESControl_Controller::Init();
    theWS->SelectNorm("IGES");
  }
}

//=================================================================================================

void DEIGES_Provider::initStatic(const Handle(DE_ConfigurationNode)& theNode)
{
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(theNode);
  IGESData::Init();

  // Get previous values
  myOldValues.ReadBSplineContinuity =
    (DEIGES_Parameters::ReadMode_BSplineContinuity)Interface_Static::IVal(
      "read.iges.bspline.continuity");
  myOldValues.ReadPrecisionMode =
    (DEIGES_Parameters::ReadMode_Precision)Interface_Static::IVal("read.precision.mode");
  myOldValues.ReadPrecisionVal = Interface_Static::RVal("read.precision.val");
  myOldValues.ReadMaxPrecisionMode =
    (DEIGES_Parameters::ReadMode_MaxPrecision)Interface_Static::IVal("read.maxprecision.mode");
  myOldValues.ReadMaxPrecisionVal = Interface_Static::RVal("read.maxprecision.val");
  myOldValues.ReadSameParamMode   = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  myOldValues.ReadSurfaceCurveMode =
    (DEIGES_Parameters::ReadMode_SurfaceCurve)Interface_Static::IVal("read.surfacecurve.mode");
  myOldValues.EncodeRegAngle = Interface_Static::RVal("read.encoderegularity.angle") * 180.0 / M_PI;

  myOldValues.ReadApproxd1       = Interface_Static::IVal("read.iges.bspline.approxd1.mode") == 1;
  myOldValues.ReadFaultyEntities = Interface_Static::IVal("read.iges.faulty.entities") == 1;
  myOldValues.ReadOnlyVisible    = Interface_Static::IVal("read.iges.onlyvisible") == 1;

  myOldValues.WriteBRepMode =
    (DEIGES_Parameters::WriteMode_BRep)Interface_Static::IVal("write.iges.brep.mode");
  myOldValues.WriteConvertSurfaceMode =
    (DEIGES_Parameters::WriteMode_ConvertSurface)Interface_Static::IVal(
      "write.convertsurface.mode");
  myOldValues.WriteHeaderAuthor   = Interface_Static::CVal("write.iges.header.author");
  myOldValues.WriteHeaderCompany  = Interface_Static::CVal("write.iges.header.company");
  myOldValues.WriteHeaderProduct  = Interface_Static::CVal("write.iges.header.product");
  myOldValues.WriteHeaderReciever = Interface_Static::CVal("write.iges.header.receiver");
  myOldValues.WritePrecisionMode =
    (DEIGES_Parameters::WriteMode_PrecisionMode)Interface_Static::IVal("write.precision.mode");
  myOldValues.WritePrecisionVal = Interface_Static::RVal("write.precision.val");
  myOldValues.WritePlaneMode =
    (DEIGES_Parameters::WriteMode_PlaneMode)Interface_Static::IVal("write.iges.plane.mode");
  myOldValues.WriteOffsetMode = Interface_Static::IVal("write.iges.offset.mode") == 1;

  myOldLengthUnit = Interface_Static::IVal("xstep.cascade.unit");

  // Set new values
  UnitsMethods::SetCasCadeLengthUnit(aNode->GlobalParameters.LengthUnit,
                                     UnitsMethods_LengthUnit_Millimeter);
  TCollection_AsciiString aStrUnit(
    UnitsMethods::DumpLengthUnit(aNode->GlobalParameters.LengthUnit));
  aStrUnit.UpperCase();
  Interface_Static::SetCVal("xstep.cascade.unit", aStrUnit.ToCString());
  setStatic(aNode->InternalParameters);
}

//=================================================================================================

void DEIGES_Provider::setStatic(const DEIGES_Parameters& theParameter)
{
  Interface_Static::SetIVal("read.iges.bspline.continuity", theParameter.ReadBSplineContinuity);
  Interface_Static::SetIVal("read.precision.mode", theParameter.ReadPrecisionMode);
  Interface_Static::SetRVal("read.precision.val", theParameter.ReadPrecisionVal);
  Interface_Static::SetIVal("read.maxprecision.mode", theParameter.ReadMaxPrecisionMode);
  Interface_Static::SetRVal("read.maxprecision.val", theParameter.ReadMaxPrecisionVal);
  Interface_Static::SetIVal("read.stdsameparameter.mode", theParameter.ReadSameParamMode);
  Interface_Static::SetIVal("read.surfacecurve.mode", theParameter.ReadSurfaceCurveMode);
  Interface_Static::SetRVal("read.encoderegularity.angle",
                            theParameter.EncodeRegAngle * M_PI / 180.0);

  Interface_Static::SetIVal("read.iges.bspline.approxd1.mode", theParameter.ReadApproxd1);
  Interface_Static::SetIVal("read.iges.faulty.entities", theParameter.ReadFaultyEntities);
  Interface_Static::SetIVal("read.iges.onlyvisible", theParameter.ReadOnlyVisible);

  Interface_Static::SetIVal("write.iges.brep.mode", theParameter.WriteBRepMode);
  Interface_Static::SetIVal("write.convertsurface.mode", theParameter.WriteConvertSurfaceMode);
  Interface_Static::SetCVal("write.iges.header.author", theParameter.WriteHeaderAuthor.ToCString());
  Interface_Static::SetCVal("write.iges.header.company",
                            theParameter.WriteHeaderCompany.ToCString());
  Interface_Static::SetCVal("write.iges.header.product",
                            theParameter.WriteHeaderProduct.ToCString());
  Interface_Static::SetCVal("write.iges.header.receiver",
                            theParameter.WriteHeaderReciever.ToCString());
  Interface_Static::SetIVal("write.precision.mode", theParameter.WritePrecisionMode);
  Interface_Static::SetRVal("write.precision.val", theParameter.WritePrecisionVal);
  Interface_Static::SetIVal("write.iges.plane.mode", theParameter.WritePlaneMode);
  Interface_Static::SetIVal("write.iges.offset.mode", theParameter.WriteOffsetMode);
}

//=================================================================================================

void DEIGES_Provider::resetStatic()
{
  Interface_Static::SetIVal("xstep.cascade.unit", myOldLengthUnit);
  UnitsMethods::SetCasCadeLengthUnit(myOldLengthUnit);
  setStatic(myOldValues);
}

//=================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)&  theWS,
                           const Message_ProgressRange&    theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: theDocument shouldn't be null";
    return false;
  }
  
  if (!validateConfigurationNode(GetNode(), thePath, "reading"))
  {
    return false;
  }
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  
  IGESCAFControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESReader(aReader, aNode);

  IFSelect_ReturnStatus aReadStat = aReader.ReadFile(thePath.ToCString());
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: abandon, no model loaded";
    resetStatic();
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            Handle(XSControl_WorkSession)&  theWS,
                            const Message_ProgressRange&    theProgress)
{
  if (!validateConfigurationNode(GetNode(), thePath, "writing"))
  {
    return false;
  }
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  
  IGESCAFControl_Writer aWriter(theWS, Standard_False);
  configureIGESCAFWriter(aWriter, aNode, theDocument, thePath);
  
  if (!aWriter.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing the file " << thePath
                        << "\t: The document cannot be translated or gives no result";
    resetStatic();
    return false;
  }
  if (!aWriter.Write(thePath.ToCString()))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing the file " << thePath
                        << "\t: Write failed";
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString&  thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//=================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString&  thePath,
                            const Handle(TDocStd_Document)& theDocument,
                            const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theDocument, aWS, theProgress);
}

//=================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange&   theProgress)
{
  (void)theProgress;
  if (!validateConfigurationNode(GetNode(), thePath, "reading"))
  {
    return false;
  }
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  personizeWS(theWS);
  
  IGESControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESControlReader(aReader, aNode);

  if (!processReadFile(aReader, thePath))
  {
    resetStatic();
    return false;
  }
  
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading the file " << thePath
                        << "\t: Cannot read any relevant data from the IGES file";
    resetStatic();
    return false;
  }
  theShape = aReader.OneShape();
  resetStatic();
  return true;
}

//=================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            Handle(XSControl_WorkSession)& theWS,
                            const Message_ProgressRange&   theProgress)
{
  (void)theWS;
  (void)theProgress;
  if (!validateConfigurationNode(GetNode(), thePath, "writing"))
  {
    return false;
  }
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  
  IGESControl_Writer aWriter(getIGESUnitString(aNode).ToCString(), aNode->InternalParameters.WriteBRepMode);
  configureIGESControlWriter(aWriter, aNode);
  
  Standard_Boolean aIsOk = aWriter.AddShape(theShape);
  if (!aIsOk)
  {
    Message::SendFail() << "DEIGES_Provider: Shape not written";
    resetStatic();
    return false;
  }

  if (!aWriter.Write(thePath.ToCString()))
  {
    Message::SendFail() << "DEIGES_Provider: Error on writing file " << thePath;
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=================================================================================================

bool DEIGES_Provider::Read(const TCollection_AsciiString& thePath,
                           TopoDS_Shape&                  theShape,
                           const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//=================================================================================================

bool DEIGES_Provider::Write(const TCollection_AsciiString& thePath,
                            const TopoDS_Shape&            theShape,
                            const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theShape, aWS, theProgress);
}

//=================================================================================================

TCollection_AsciiString DEIGES_Provider::GetFormat() const
{
  return TCollection_AsciiString("IGES");
}

//=================================================================================================

TCollection_AsciiString DEIGES_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Read(ReadStreamMap&            theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        Handle(XSControl_WorkSession)&  theWS,
                                        const Message_ProgressRange&    theProgress)
{
  if (!validateStreamMap(theStreams))
  {
    return Standard_False;
  }
  
  const TCollection_AsciiString& aFirstKey = theStreams.FindKey(1);
  Standard_IStream& aStream = theStreams.ChangeFromIndex(1);
  
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << ". Document is null";
    return Standard_False;
  }
  personizeWS(theWS);
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  if (aNode.IsNull())
  {
    Message::SendFail() << "Error: DEIGES_Provider configuring failed in reading stream " << aFirstKey;
    return Standard_False;
  }
  
  initStatic(aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  
  IGESCAFControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESReader(aReader, aNode);

  IFSelect_ReturnStatus aReadStat = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: abandon, no model loaded";
    resetStatic();
    return Standard_False;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Cannot read any relevant data from the IGES stream";
    resetStatic();
    return Standard_False;
  }
  resetStatic();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Write(WriteStreamMap&                 theStreams,
                                         const Handle(TDocStd_Document)& theDocument,
                                         Handle(XSControl_WorkSession)&  theWS,
                                         const Message_ProgressRange&    theProgress)
{
  if (!validateStreamMap(theStreams))
  {
    return Standard_False;
  }
  
  const TCollection_AsciiString& aFirstKey = theStreams.FindKey(1);
  Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
  
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << ". Document is null";
    return Standard_False;
  }
  personizeWS(theWS);
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  if (aNode.IsNull())
  {
    Message::SendFail() << "Error: DEIGES_Provider configuring failed in writing stream " << aFirstKey;
    return Standard_False;
  }
  
  initStatic(aNode);
  IGESCAFControl_Writer aWriter(theWS, Standard_False);
  configureIGESCAFWriter(aWriter, aNode, theDocument, aFirstKey);
  
  if (!aWriter.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << "\t: The document cannot be translated or gives no result";
    resetStatic();
    return Standard_False;
  }
  
  if (!aWriter.Write(aStream))
  {
    Message::SendFail() << "Error in the DEIGES_Provider during writing stream " << aFirstKey
                        << "\t: Write failed";
    resetStatic();
    return Standard_False;
  }
  resetStatic();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Read(ReadStreamMap&           theStreams,
                                        TopoDS_Shape&                  theShape,
                                        Handle(XSControl_WorkSession)& theWS,
                                        const Message_ProgressRange&   /*theProgress*/)
{
  if (!validateStreamMap(theStreams))
  {
    return Standard_False;
  }
  
  const TCollection_AsciiString& aFirstKey = theStreams.FindKey(1);
  Standard_IStream& aStream = theStreams.ChangeFromIndex(1);
  
  personizeWS(theWS);
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  if (aNode.IsNull())
  {
    Message::SendFail() << "Error: DEIGES_Provider configuring failed in reading stream " << aFirstKey;
    return Standard_False;
  }
  
  initStatic(aNode);
  
  IGESControl_Reader aReader;
  aReader.SetWS(theWS);
  configureIGESControlReader(aReader, aNode);

  IFSelect_ReturnStatus aReadStat = aReader.ReadStream(aFirstKey.ToCString(), aStream);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Could not read stream, no model loaded";
    resetStatic();
    return Standard_False;
  }
  
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the DEIGES_Provider during reading stream " << aFirstKey
                        << "\t: Cannot read any relevant data from the IGES stream";
    resetStatic();
    return Standard_False;
  }
  theShape = aReader.OneShape();
  resetStatic();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Write(WriteStreamMap&                theStreams,
                                         const TopoDS_Shape&            theShape,
                                         Handle(XSControl_WorkSession)& theWS,
                                         const Message_ProgressRange&   /*theProgress*/)
{
  if (!validateStreamMap(theStreams))
  {
    return Standard_False;
  }
  
  const TCollection_AsciiString& aFirstKey = theStreams.FindKey(1);
  Standard_OStream& aStream = theStreams.ChangeFromIndex(1);
  
  personizeWS(theWS);
  
  Handle(DEIGES_ConfigurationNode) aNode = Handle(DEIGES_ConfigurationNode)::DownCast(GetNode());
  if (aNode.IsNull())
  {
    Message::SendFail() << "Error: DEIGES_Provider configuring failed in writing stream " << aFirstKey;
    return Standard_False;
  }
  
  initStatic(aNode);
  IGESControl_Writer aWriter(getIGESUnitString(aNode).ToCString(), aNode->InternalParameters.WriteBRepMode);
  configureIGESControlWriter(aWriter, aNode);
  
  Standard_Boolean isOk = aWriter.AddShape(theShape);
  if (!isOk)
  {
    Message::SendFail() << "Error: DEIGES_Provider failed to transfer shape for stream " << aFirstKey;
    resetStatic();
    return Standard_False;  
  }

  if (!aWriter.Write(aStream))
  {
    Message::SendFail() << "Error: DEIGES_Provider failed to write shape to stream " << aFirstKey;
    resetStatic();
    return Standard_False;
  }
  resetStatic();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Read(ReadStreamMap&            theStreams,
                                        const Handle(TDocStd_Document)& theDocument,
                                        const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(theStreams, theDocument, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Write(WriteStreamMap&                 theStreams,
                                         const Handle(TDocStd_Document)& theDocument,
                                         const Message_ProgressRange&    theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(theStreams, theDocument, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Read(ReadStreamMap&           theStreams,
                                        TopoDS_Shape&                  theShape,
                                        const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(theStreams, theShape, aWS, theProgress);
}

//=================================================================================================

Standard_Boolean DEIGES_Provider::Write(WriteStreamMap&                theStreams,
                                         const TopoDS_Shape&            theShape,
                                         const Message_ProgressRange&   theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(theStreams, theShape, aWS, theProgress);
}
