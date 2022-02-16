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

#include <RWStl_Provider.hxx>

#include <BinXCAFDrivers.hxx>
#include <BRep_Builder.hxx>
#include <Message.hxx>
#include <RWStl.hxx>
#include <RWStl_ConfigurationNode.hxx>
#include <StlAPI.hxx>
#include <StlAPI_Writer.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWStl_Provider, DE_Provider)

//=======================================================================
// function : RWStl_Provider
// purpose  :
//=======================================================================
RWStl_Provider::RWStl_Provider()
{}

//=======================================================================
// function : RWStl_Provider
// purpose  :
//=======================================================================
RWStl_Provider::RWStl_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :DE_Provider(theNode)
{}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWStl_Provider::Read(const TCollection_AsciiString& thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWStl_Provider::Write(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWStl_Provider::Read(const TCollection_AsciiString& thePath,
                          const Handle(TDocStd_Document)& theDocument,
                          const Message_ProgressRange& theProgress)
{
  if (theDocument.IsNull())
  {
    Message::SendFail() << "Error in the RWStl_Provider during reading the file " <<
      thePath << "\t: theDocument shouldn't be null";
    return false;
  }
  TopoDS_Shape aShape;
  if (!Read(thePath, aShape, theProgress))
  {
    return false;
  }
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->AddShape(aShape);
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWStl_Provider::Write(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange& theProgress)
{
  TopoDS_Shape aShape;
  TDF_LabelSequence aLabels;
  Handle(XCAFDoc_ShapeTool) aSTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aSTool->GetFreeShapes(aLabels);
  if (aLabels.Length() <= 0)
  {
    Message::SendFail() << "Error in the RWStl_Provider during writing the file " <<
      thePath << "\t: Document contain no shapes";
    return false;
  }

  if (aLabels.Length() == 1)
  {
    aShape = aSTool->GetShape(aLabels.Value(1));
  }
  else
  {
    TopoDS_Compound aComp;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound(aComp);
    for (Standard_Integer anIndex = 1; anIndex <= aLabels.Length(); anIndex++)
    {
      TopoDS_Shape aS = aSTool->GetShape(aLabels.Value(anIndex));
      aBuilder.Add(aComp, aS);
    }
    aShape = aComp;
  }
  return Write(thePath, aShape, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWStl_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape& theShape,
                          Handle(XSControl_WorkSession)& theWS,
                          const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWStl_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape& theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=======================================================================
// function : Read
// purpose  :
//=======================================================================
bool RWStl_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape& theShape,
                          const Message_ProgressRange& theProgress)
{
  Message::SendWarning() << "OCCT Stl reader does not support model scaling according to custom length unit";
  if (!GetNode()->IsKind(STANDARD_TYPE(RWStl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the RWStl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return true;
  }
  Handle(RWStl_ConfigurationNode) aNode = Handle(RWStl_ConfigurationNode)::DownCast(GetNode());
  double aMergeAngle = aNode->InternalParameters.ReadMergeAngle * M_PI / 180.0;
  if(aMergeAngle != M_PI_2)
  {
    if (aMergeAngle < 0.0 || aMergeAngle > M_PI_2)
    {
      Message::SendFail() << "Error in the RWStl_Provider during reading the file " <<
        thePath << "\t: The merge angle is out of the valid range";
      return false;
    }
  }
  if (!aNode->InternalParameters.ReadBRep)
  {
    Handle(Poly_Triangulation) aTriangulation = RWStl::ReadFile(thePath.ToCString(), aMergeAngle, theProgress);

    TopoDS_Face aFace;
    BRep_Builder aB;
    aB.MakeFace(aFace);
    aB.UpdateFace(aFace, aTriangulation);
    theShape = aFace;
  }
  else
  {
    Standard_DISABLE_DEPRECATION_WARNINGS
      if (!StlAPI::Read(theShape, thePath.ToCString()))
      {
        Message::SendFail() << "Error in the RWStl_Provider during reading the file " << thePath;
        return false;
      }
    Standard_ENABLE_DEPRECATION_WARNINGS
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWStl_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape& theShape,
                           const Message_ProgressRange& theProgress)
{
  Message::SendWarning() << "OCCT Stl writer does not support model scaling according to custom length unit";
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(RWStl_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the RWStl_Provider during reading the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(RWStl_ConfigurationNode) aNode = Handle(RWStl_ConfigurationNode)::DownCast(GetNode());

  StlAPI_Writer aWriter;
  aWriter.ASCIIMode() = aNode->InternalParameters.WriteAscii;
  if (!aWriter.Write(theShape, thePath.ToCString(), theProgress))
  {
    Message::SendFail() << "Error in the RWStl_Provider during reading the file " <<
      thePath << "\t: Mesh writing has been failed";
    return false;
  }
  return true;
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString RWStl_Provider::GetFormat() const
{
  return TCollection_AsciiString("STL");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString RWStl_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
