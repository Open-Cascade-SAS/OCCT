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

#include <IGESCAFControl_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <IGESCAFControl_ConfigurationNode.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData.hxx>
#include <IGESData_IGESModel.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESCAFControl_Provider, DE_Provider)

//=======================================================================
// function : IGESCAFControl_Provider
// purpose  :
//=======================================================================
IGESCAFControl_Provider::IGESCAFControl_Provider()
{}

//=======================================================================
// function : IGESCAFControl_Provider
// purpose  :
//=======================================================================
IGESCAFControl_Provider::IGESCAFControl_Provider(const Handle(DE_ConfigurationNode)& theNode)
  : DE_Provider(theNode)
{}

//=======================================================================
// function : personizeWS
// purpose  :
//=======================================================================
void IGESCAFControl_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: IGESCAFControl_Provider :"
      << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  Handle(IGESControl_Controller) aCntrl = Handle(IGESControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    IGESControl_Controller::Init();
    theWS->SelectNorm("IGES");
  }
}

//=======================================================================
// function : initStatic
// purpose  :
//=======================================================================
void IGESCAFControl_Provider::initStatic(const Handle(DE_ConfigurationNode)& theNode)
{
  Handle(IGESCAFControl_ConfigurationNode) aNode = Handle(IGESCAFControl_ConfigurationNode)::DownCast(theNode);
  IGESData::Init();

  // Get previous values
  myOldValues.ReadBSplineContinuity = (IGESCAFControl_ConfigurationNode::ReadMode_BSplineContinuity)Interface_Static::IVal("read.iges.bspline.continuity");
  myOldValues.ReadPrecisionMode = (IGESCAFControl_ConfigurationNode::ReadMode_Precision)Interface_Static::IVal("read.precision.mode");
  myOldValues.ReadPrecisionVal = Interface_Static::RVal("read.precision.val");
  myOldValues.ReadMaxPrecisionMode = (IGESCAFControl_ConfigurationNode::ReadMode_MaxPrecision)Interface_Static::IVal("read.maxprecision.mode");
  myOldValues.ReadMaxPrecisionVal = Interface_Static::RVal("read.maxprecision.val");
  myOldValues.ReadSameParamMode = Interface_Static::IVal("read.stdsameparameter.mode") == 1;
  myOldValues.ReadSurfaceCurveMode = (IGESCAFControl_ConfigurationNode::ReadMode_SurfaceCurve)Interface_Static::IVal("read.surfacecurve.mode");
  myOldValues.EncodeRegAngle = Interface_Static::RVal("read.encoderegularity.angle") * 180.0 / M_PI;

  myOldValues.ReadApproxd1 = Interface_Static::IVal("read.iges.bspline.approxd1.mode") == 1;
  myOldValues.ReadResourceName = Interface_Static::CVal("read.iges.resource.name");
  myOldValues.ReadSequence = Interface_Static::CVal("read.iges.sequence");
  myOldValues.ReadFaultyEntities = Interface_Static::IVal("read.iges.faulty.entities") == 1;
  myOldValues.ReadOnlyVisible = Interface_Static::IVal("read.iges.onlyvisible") == 1;

  myOldValues.WriteBRepMode = (IGESCAFControl_ConfigurationNode::WriteMode_BRep)Interface_Static::IVal("write.iges.brep.mode");
  myOldValues.WriteConvertSurfaceMode = (IGESCAFControl_ConfigurationNode::WriteMode_ConvertSurface)Interface_Static::IVal("write.convertsurface.mode");
  myOldValues.WriteUnit = (UnitsMethods_LengthUnit)Interface_Static::IVal("write.iges.unit");
  myOldValues.WriteHeaderAuthor = Interface_Static::CVal("write.iges.header.author");
  myOldValues.WriteHeaderCompany = Interface_Static::CVal("write.iges.header.company");
  myOldValues.WriteHeaderProduct = Interface_Static::CVal("write.iges.header.product");
  myOldValues.WriteHeaderReciever = Interface_Static::CVal("write.iges.header.receiver");
  myOldValues.WriteResourceName = Interface_Static::CVal("write.iges.resource.name");
  myOldValues.WriteSequence = Interface_Static::CVal("write.iges.sequence");
  myOldValues.WritePrecisionMode = (IGESCAFControl_ConfigurationNode::WriteMode_PrecisionMode)Interface_Static::IVal("write.precision.mode");
  myOldValues.WritePrecisionVal = Interface_Static::RVal("write.precision.val");
  myOldValues.WritePlaneMode = (IGESCAFControl_ConfigurationNode::WriteMode_PlaneMode)Interface_Static::IVal("write.iges.plane.mode");
  myOldValues.WriteOffsetMode = Interface_Static::IVal("write.iges.offset.mode") == 1;

  myOldLengthUnit = Interface_Static::IVal("xstep.cascade.unit");

  // Set new values
  UnitsMethods::SetCasCadeLengthUnit(aNode->GlobalParameters.LengthUnit, UnitsMethods_LengthUnit_Millimeter);
  TCollection_AsciiString aStrUnit(UnitsMethods::DumpLengthUnit(aNode->GlobalParameters.LengthUnit));
  aStrUnit.UpperCase();
  Interface_Static::SetCVal("xstep.cascade.unit", aStrUnit.ToCString());
  setStatic(aNode->InternalParameters);
}

//=======================================================================
// function : setStatic
// purpose  :
//=======================================================================
void IGESCAFControl_Provider::setStatic(const IGESCAFControl_ConfigurationNode::IGESCAFControl_InternalSection& theParameter)
{
  Interface_Static::SetIVal("read.iges.bspline.continuity", theParameter.ReadBSplineContinuity);
  Interface_Static::SetIVal("read.precision.mode", theParameter.ReadPrecisionMode);
  Interface_Static::SetRVal("read.precision.val", theParameter.ReadPrecisionVal);
  Interface_Static::SetIVal("read.maxprecision.mode", theParameter.ReadMaxPrecisionMode);
  Interface_Static::SetRVal("read.maxprecision.val", theParameter.ReadMaxPrecisionVal);
  Interface_Static::SetIVal("read.stdsameparameter.mode", theParameter.ReadSameParamMode);
  Interface_Static::SetIVal("read.surfacecurve.mode", theParameter.ReadSurfaceCurveMode);
  Interface_Static::SetRVal("read.encoderegularity.angle", theParameter.EncodeRegAngle * M_PI / 180.0);

  Interface_Static::SetIVal("read.iges.bspline.approxd1.mode", theParameter.ReadApproxd1);
  Interface_Static::SetCVal("read.iges.resource.name", theParameter.ReadResourceName.ToCString());
  Interface_Static::SetCVal("read.iges.sequence", theParameter.ReadSequence.ToCString());
  Interface_Static::SetIVal("read.iges.faulty.entities", theParameter.ReadFaultyEntities);
  Interface_Static::SetIVal("read.iges.onlyvisible", theParameter.ReadOnlyVisible);

  Interface_Static::SetIVal("write.iges.brep.mode", theParameter.WriteBRepMode);
  Interface_Static::SetIVal("write.convertsurface.mode", theParameter.WriteConvertSurfaceMode);
  Interface_Static::SetIVal("write.iges.unit", theParameter.WriteUnit);
  Interface_Static::SetCVal("write.iges.header.author", theParameter.WriteHeaderAuthor.ToCString());
  Interface_Static::SetCVal("write.iges.header.company", theParameter.WriteHeaderCompany.ToCString());
  Interface_Static::SetCVal("write.iges.header.product", theParameter.WriteHeaderProduct.ToCString());
  Interface_Static::SetCVal("write.iges.header.receiver", theParameter.WriteHeaderReciever.ToCString());
  Interface_Static::SetCVal("write.iges.resource.name", theParameter.WriteResourceName.ToCString());
  Interface_Static::SetCVal("write.iges.sequence", theParameter.WriteSequence.ToCString());
  Interface_Static::SetIVal("write.precision.mode", theParameter.WritePrecisionMode);
  Interface_Static::SetRVal("write.precision.val", theParameter.WritePrecisionVal);
  Interface_Static::SetIVal("write.iges.plane.mode", theParameter.WritePlaneMode);
  Interface_Static::SetIVal("write.iges.offset.mode", theParameter.WriteOffsetMode);
}

//=======================================================================
// function : resetStatic
// purpose  :
//=======================================================================
void IGESCAFControl_Provider::resetStatic()
{
  Interface_Static::SetIVal("xstep.cascade.unit", myOldLengthUnit);
  UnitsMethods::SetCasCadeLengthUnit(myOldLengthUnit);
  setStatic(myOldValues);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: theDocument shouldn't be null";
    return false;
  }
  if (!GetNode()->IsKind(STANDARD_TYPE(IGESCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(IGESCAFControl_ConfigurationNode) aNode = Handle(IGESCAFControl_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->GlobalParameters.LengthUnit, UnitsMethods_LengthUnit_Millimeter);
  IGESCAFControl_Reader aReader;
  aReader.SetWS(theWS);

  aReader.SetReadVisible(aNode->InternalParameters.ReadOnlyVisible);

  aReader.SetColorMode(aNode->InternalParameters.ReadColor);
  aReader.SetNameMode(aNode->InternalParameters.ReadName);
  aReader.SetLayerMode(aNode->InternalParameters.ReadLayer);

  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  aReadStat = aReader.ReadFile(thePath.ToCString());
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: abandon, no model loaded";
    resetStatic();
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Cannot read any relevant data from the IGES file";
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  if (!GetNode()->IsKind(STANDARD_TYPE(IGESCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(IGESCAFControl_ConfigurationNode) aNode = Handle(IGESCAFControl_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  initStatic(aNode);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->InternalParameters.WriteUnit, UnitsMethods_LengthUnit_Millimeter);
  IGESCAFControl_Writer aWriter(theWS);
  aWriter.SetColorMode(aNode->InternalParameters.WriteColor);
  aWriter.SetNameMode(aNode->InternalParameters.WriteName);
  aWriter.SetLayerMode(aNode->InternalParameters.WriteLayer);

  if (!aWriter.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: The document cannot be translated or gives no result";
    resetStatic();
    return false;
  }
  if (!aWriter.Write(thePath.ToCString()))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Write failed";
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theDocument, aWS, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   TopoDS_Shape& theShape,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(IGESCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(IGESCAFControl_ConfigurationNode) aNode = Handle(IGESCAFControl_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  personizeWS(theWS);
  IGESControl_Reader aReader;
  aReader.SetWS(theWS);
  aReader.SetReadVisible(aNode->InternalParameters.ReadOnlyVisible);
  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  aReadStat = aReader.ReadFile(thePath.ToCString());
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Could not read file, no model loaded";
    resetStatic();
    return false;
  }
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Cannot read any relevant data from the IGES file";
    resetStatic();
    return false;
  }
  theShape = aReader.OneShape();
  resetStatic();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const TopoDS_Shape& theShape,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  (void)theWS;
  (void)theProgress;
  if (!GetNode()->IsKind(STANDARD_TYPE(IGESCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the IGESCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(IGESCAFControl_ConfigurationNode) aNode = Handle(IGESCAFControl_ConfigurationNode)::DownCast(GetNode());
  initStatic(aNode);
  TCollection_AsciiString aUnit(UnitsMethods::DumpLengthUnit(aNode->InternalParameters.WriteUnit));
  aUnit.UpperCase();
  IGESControl_Writer aWriter(aUnit.ToCString(),
                             aNode->InternalParameters.WriteBRepMode);
  Standard_Boolean aIsOk = aWriter.AddShape(theShape);
  if (!aIsOk)
  {
    Message::SendFail() << "IGESCAFControl_Provider: Shape not written";
    resetStatic();
    return false;
  }

  if (!(aWriter.Write(thePath.ToCString())))
  {
    Message::SendFail() << "IGESCAFControl_Provider: Error on writing file " << thePath;
    resetStatic();
    return false;
  }
  resetStatic();
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   TopoDS_Shape& theShape,
                                   const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Read(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool IGESCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const TopoDS_Shape& theShape,
                                    const Message_ProgressRange& theProgress)
{
  Handle(XSControl_WorkSession) aWS = new XSControl_WorkSession();
  return Write(thePath, theShape, aWS, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString IGESCAFControl_Provider::GetFormat() const
{
  return TCollection_AsciiString("IGES");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString IGESCAFControl_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
