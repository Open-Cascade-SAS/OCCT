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

#include <STEPCAFControl_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_ConfParameters.hxx>
#include <STEPCAFControl_ConfigurationNode.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <UnitsMethods.hxx>

IMPLEMENT_STANDARD_RTTIEXT(STEPCAFControl_Provider, DE_Provider)

//=======================================================================
// function : STEPCAFControl_Provider
// purpose  :
//=======================================================================
STEPCAFControl_Provider::STEPCAFControl_Provider()
{}

//=======================================================================
// function : STEPCAFControl_Provider
// purpose  :
//=======================================================================
STEPCAFControl_Provider::STEPCAFControl_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :DE_Provider(theNode)
{}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool STEPCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   const Handle(TDocStd_Document)& theDocument,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: theDocument shouldn't be null";
    return false;
  }
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(STEPCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(STEPCAFControl_ConfigurationNode) aNode = Handle(STEPCAFControl_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, aNode->GlobalParameters.LengthUnit, UnitsMethods_LengthUnit_Millimeter);
  STEPCAFControl_Reader aReader;
  aReader.Init(theWS);
  aReader.SetColorMode(aNode->InternalParameters.ReadColor);
  aReader.SetNameMode(aNode->InternalParameters.ReadName);
  aReader.SetLayerMode(aNode->InternalParameters.ReadLayer);
  aReader.SetPropsMode(aNode->InternalParameters.ReadProps);
  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;
  StepData_ConfParameters aParams;
  aReadStat = aReader.ReadFile(thePath.ToCString(), aParams);
  if (aReadStat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: abandon";
    return false;
  }

  if (!aReader.Transfer(theDocument, theProgress))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: Cannot read any relevant data from the STEP file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool STEPCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const Handle(TDocStd_Document)& theDocument,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(STEPCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during writing the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(STEPCAFControl_ConfigurationNode) aNode = Handle(STEPCAFControl_ConfigurationNode)::DownCast(GetNode());

  XCAFDoc_DocumentTool::SetLengthUnit(theDocument, 
    UnitsMethods::GetLengthUnitScale(aNode->InternalParameters.WriteUnit,
                                     UnitsMethods_LengthUnit_Millimeter), UnitsMethods_LengthUnit_Millimeter);
  personizeWS(theWS);
  STEPCAFControl_Writer aWriter;
  aWriter.Init(theWS);
  Handle(StepData_StepModel) aModel = Handle(StepData_StepModel)::DownCast(aWriter.Writer().WS()->Model());
  STEPControl_StepModelType aMode =
    static_cast<STEPControl_StepModelType>(aNode->InternalParameters.WriteModelType);
  aWriter.SetColorMode(aNode->InternalParameters.WriteColor);
  aWriter.SetNameMode(aNode->InternalParameters.WriteName);
  aWriter.SetLayerMode(aNode->InternalParameters.WriteLayer);
  aWriter.SetPropsMode(aNode->InternalParameters.WriteProps);

  TDF_Label aLabel;
  StepData_ConfParameters aParams;
  if (!aWriter.Transfer(theDocument, aParams, aMode, 0, theProgress))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during writing the file " <<
      thePath << "\t: The document cannot be translated or gives no result";
    return false;
  }
  IFSelect_ReturnStatus aStatus = aWriter.Write(thePath.ToCString());
  switch (aStatus)
  {
    case IFSelect_RetVoid:
    {
      Message::SendFail() << "Error in the STEPCAFControl_Provider during writing the file " <<
        thePath << "\t: No file written";
      return false;;
    }
    case IFSelect_RetDone:
    {
      break;
    }
    default:
    {
      Message::SendFail() << "Error in the STEPCAFControl_Provider during writing the file " <<
        thePath << "\t: Error on writing file";
      return false;
    }
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool STEPCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
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
bool STEPCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
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
bool STEPCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
                                   TopoDS_Shape& theShape,
                                   Handle(XSControl_WorkSession)& theWS,
                                   const Message_ProgressRange& theProgress)
{
  (void)theProgress;
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(STEPCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(STEPCAFControl_ConfigurationNode) aNode = Handle(STEPCAFControl_ConfigurationNode)::DownCast(GetNode());
  personizeWS(theWS);
  STEPControl_Reader aReader;
  aReader.SetWS(theWS);
  IFSelect_ReturnStatus aReadstat = IFSelect_RetVoid;
  StepData_ConfParameters aParams;
  aReadstat = aReader.ReadFile(thePath.ToCString(), aParams);
  Handle(StepData_StepModel) aModel = aReader.StepModel();
  if (aReadstat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: abandon, no model loaded";
    return false;
  }
  aModel->SetLocalLengthUnit(aNode->GlobalParameters.LengthUnit);
  if (aReader.TransferRoots() <= 0)
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t:Cannot read any relevant data from the STEP file";
    return false;
  }
  theShape = aReader.OneShape();
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool STEPCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
                                    const TopoDS_Shape& theShape,
                                    Handle(XSControl_WorkSession)& theWS,
                                    const Message_ProgressRange& theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(STEPCAFControl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(STEPCAFControl_ConfigurationNode) aNode = Handle(STEPCAFControl_ConfigurationNode)::DownCast(GetNode());

  personizeWS(theWS);
  STEPControl_Writer aWriter;
  aWriter.SetWS(theWS);
  IFSelect_ReturnStatus aWritestat = IFSelect_RetVoid;
  Handle(StepData_StepModel) aModel = aWriter.Model();
  aModel->SetWriteLengthUnit(UnitsMethods::GetLengthUnitScale(aNode->InternalParameters.WriteUnit, UnitsMethods_LengthUnit_Millimeter));
  StepData_ConfParameters aParams;
  aWritestat = aWriter.Transfer(theShape, aNode->InternalParameters.WriteModelType, aParams, true, theProgress);
  if (aWritestat != IFSelect_RetDone)
  {
    Message::SendFail() << "Error in the STEPCAFControl_Provider during reading the file " <<
      thePath << "\t: abandon, no model loaded";
    return false;
  }
  if (aWriter.Write(thePath.ToCString()) != IFSelect_RetDone)
  {
    Message::SendFail() << "STEPCAFControl_Provider: Error on writing file";
    return false;
  }
  return true;
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool STEPCAFControl_Provider::Read(const TCollection_AsciiString& thePath,
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
bool STEPCAFControl_Provider::Write(const TCollection_AsciiString& thePath,
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
TCollection_AsciiString STEPCAFControl_Provider::GetFormat() const
{
  return TCollection_AsciiString("STEP");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString STEPCAFControl_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}

//=======================================================================
// function : personizeWS
// purpose  :
//=======================================================================
void STEPCAFControl_Provider::personizeWS(Handle(XSControl_WorkSession)& theWS)
{
  if (theWS.IsNull())
  {
    Message::SendWarning() << "Warning: STEPCAFControl_Provider :"
      << " Null work session, use internal temporary session";
    theWS = new XSControl_WorkSession();
  }
  Handle(STEPCAFControl_Controller) aCntrl = Handle(STEPCAFControl_Controller)::DownCast(theWS->NormAdaptor());
  if (aCntrl.IsNull())
  {
    STEPCAFControl_Controller::Init();
    theWS->SelectNorm("STEP");
  }
}
