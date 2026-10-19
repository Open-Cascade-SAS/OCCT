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

#include <DEOBJ_Provider.hxx>

#include <BRep_Builder.hxx>
#include <DE_ValidationUtils.hxx>
#include <DEOBJ_ConfigurationNode.hxx>
#include <Message.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <RWObj_CafReader.hxx>
#include <RWObj_CafWriter.hxx>
#include <RWObj_TriangulationReader.hxx>
#include <RWMesh_CoordinateSystemConverter.hxx>
#include <Poly_Triangulation.hxx>
#include <TopoDS_Face.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DEOBJ_Provider, DE_Provider)

namespace
{
//==================================================================================================

void configureReader(RWObj_CafReader& theReader, const DEOBJ_ConfigurationNode& theNode)
{
  theReader.SetSinglePrecision(theNode.InternalParameters.ReadSinglePrecision);
  theReader.SetSystemLengthUnit(theNode.GlobalParameters.LengthUnit / 1000);
  theReader.SetSystemCoordinateSystem(theNode.InternalParameters.SystemCS);
  theReader.SetFileLengthUnit(theNode.InternalParameters.FileLengthUnit);
  theReader.SetFileCoordinateSystem(theNode.InternalParameters.FileCS);
  theReader.SetRootPrefix(theNode.InternalParameters.ReadRootPrefix);
  theReader.SetMemoryLimitMiB(theNode.InternalParameters.ReadMemoryLimitMiB);
}

//==================================================================================================

bool configureShapeReader(RWObj_TriangulationReader&     theReader,
                          const DEOBJ_ConfigurationNode& theNode)
{
  RWMesh_CoordinateSystemConverter aConverter;
  aConverter.SetOutputLengthUnit(theNode.GlobalParameters.LengthUnit / 1000);
  aConverter.SetOutputCoordinateSystem(theNode.InternalParameters.SystemCS);
  aConverter.SetInputLengthUnit(theNode.InternalParameters.FileLengthUnit);
  aConverter.SetInputCoordinateSystem(theNode.InternalParameters.FileCS);
  theReader.SetTransformation(aConverter);
  theReader.SetSinglePrecision(theNode.InternalParameters.ReadSinglePrecision);
  theReader.SetCreateShapes(theNode.InternalParameters.ReadCreateShapes);
  return theReader.SetMemoryLimitMiB(theNode.InternalParameters.ReadMemoryLimitMiB);
}

//==================================================================================================

bool readShapeResult(RWObj_TriangulationReader&     theReader,
                     const DEOBJ_ConfigurationNode& theNode,
                     TopoDS_Shape&                  theShape)
{
  if (theNode.InternalParameters.ReadCreateShapes)
  {
    const TopoDS_Shape aResult = theReader.ResultShape();
    if (aResult.IsNull())
    {
      Message::SendFail("Error: OBJ input contains no mesh shapes");
      return false;
    }
    theShape = aResult;
    return true;
  }
  const occ::handle<Poly_Triangulation> aTriangulation = theReader.GetTriangulation();
  if (aTriangulation.IsNull())
  {
    Message::SendFail("Error: OBJ input contains no triangulation");
    return false;
  }
  TopoDS_Face  aFace;
  BRep_Builder aBuilder;
  aBuilder.MakeFace(aFace, aTriangulation);
  theShape = aFace;
  return true;
}
} // namespace

//=================================================================================================

DEOBJ_Provider::DEOBJ_Provider() = default;

//=================================================================================================

DEOBJ_Provider::DEOBJ_Provider(const occ::handle<DE_ConfigurationNode>& theNode)
    : DE_Provider(theNode)
{
}

//=================================================================================================

bool DEOBJ_Provider::Read(const TCollection_AsciiString&       thePath,
                          const occ::handle<TDocStd_Document>& theDocument,
                          occ::handle<XSControl_WorkSession>&  theWS,
                          const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Read(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEOBJ_Provider::Write(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           occ::handle<XSControl_WorkSession>&  theWS,
                           const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=================================================================================================

bool DEOBJ_Provider::Read(const TCollection_AsciiString&       thePath,
                          const occ::handle<TDocStd_Document>& theDocument,
                          const Message_ProgressRange&         theProgress)
{
  TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateDocument(theDocument, aContext))
  {
    return false;
  }
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DEOBJ_ConfigurationNode),
                                                     aContext))
  {
    return false;
  }
  occ::handle<DEOBJ_ConfigurationNode> aNode = occ::down_cast<DEOBJ_ConfigurationNode>(GetNode());
  RWObj_CafReader                      aReader;
  configureReader(aReader, *aNode);
  aReader.SetDocument(theDocument);
  if (!aReader.Perform(thePath, theProgress))
  {
    Message::SendFail() << "Error in the DEOBJ_ConfigurationNode during reading the file "
                        << thePath;
    return false;
  }
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  return true;
}

//=================================================================================================

bool DEOBJ_Provider::Write(const TCollection_AsciiString&       thePath,
                           const occ::handle<TDocStd_Document>& theDocument,
                           const Message_ProgressRange&         theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(DEOBJ_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the DEOBJ_ConfigurationNode during writing the file "
                        << thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  occ::handle<DEOBJ_ConfigurationNode> aNode = occ::down_cast<DEOBJ_ConfigurationNode>(GetNode());

  NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString> aFileInfo;
  if (!aNode->InternalParameters.WriteAuthor.IsEmpty())
  {
    aFileInfo.Add("Author", aNode->InternalParameters.WriteAuthor);
  }
  if (!aNode->InternalParameters.WriteComment.IsEmpty())
  {
    aFileInfo.Add("Comments", aNode->InternalParameters.WriteComment);
  }

  RWMesh_CoordinateSystemConverter aConverter;
  double                           aScaleFactorMM = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument,
                                          aScaleFactorMM,
                                          UnitsMethods_LengthUnit_Millimeter))
  {
    aConverter.SetInputLengthUnit(aScaleFactorMM / 1000.);
  }
  else
  {
    aConverter.SetInputLengthUnit(aNode->GlobalParameters.SystemUnit / 1000.);
    Message::SendWarning()
      << "Warning in the DEOBJ_Provider during writing the file " << thePath
      << "\t: The document has no information on Units. Using global parameter as initial Unit.";
  }
  aConverter.SetInputCoordinateSystem(aNode->InternalParameters.SystemCS);
  aConverter.SetOutputLengthUnit(aNode->GlobalParameters.LengthUnit / 1000.);
  aConverter.SetOutputCoordinateSystem(aNode->InternalParameters.FileCS);

  RWObj_CafWriter aWriter(thePath);
  aWriter.SetCoordinateSystemConverter(aConverter);
  if (!aWriter.Perform(theDocument, aFileInfo, theProgress))
  {
    Message::SendFail() << "Error in the DEOBJ_ConfigurationNode during writing the file "
                        << thePath;
    return false;
  }
  return true;
}

//=================================================================================================

bool DEOBJ_Provider::Read(const TCollection_AsciiString&      thePath,
                          TopoDS_Shape&                       theShape,
                          occ::handle<XSControl_WorkSession>& theWS,
                          const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Read(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEOBJ_Provider::Write(const TCollection_AsciiString&      thePath,
                           const TopoDS_Shape&                 theShape,
                           occ::handle<XSControl_WorkSession>& theWS,
                           const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=================================================================================================

bool DEOBJ_Provider::Read(const TCollection_AsciiString& thePath,
                          TopoDS_Shape&                  theShape,
                          const Message_ProgressRange&   theProgress)
{
  const TCollection_AsciiString aContext = TCollection_AsciiString("reading the file ") + thePath;
  if (!DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                     STANDARD_TYPE(DEOBJ_ConfigurationNode),
                                                     aContext))
  {
    return false;
  }
  const occ::handle<DEOBJ_ConfigurationNode> aNode =
    occ::down_cast<DEOBJ_ConfigurationNode>(GetNode());
  RWObj_TriangulationReader aReader;
  if (!configureShapeReader(aReader, *aNode) || !aReader.Read(thePath, theProgress))
  {
    return false;
  }
  return readShapeResult(aReader, *aNode, theShape);
}

//=================================================================================================

bool DEOBJ_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape&            theShape,
                           const Message_ProgressRange&   theProgress)
{
  occ::handle<TDocStd_Document>  aDoc    = new TDocStd_Document("BinXCAF");
  occ::handle<XCAFDoc_ShapeTool> aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//==================================================================================================

bool DEOBJ_Provider::Read(ReadStreamList&                      theStreams,
                          const occ::handle<TDocStd_Document>& theDocument,
                          occ::handle<XSControl_WorkSession>&  theWS,
                          const Message_ProgressRange&         theProgress)
{
  (void)theWS;
  return Read(theStreams, theDocument, theProgress);
}

//==================================================================================================

bool DEOBJ_Provider::Read(ReadStreamList&                     theStreams,
                          TopoDS_Shape&                       theShape,
                          occ::handle<XSControl_WorkSession>& theWS,
                          const Message_ProgressRange&        theProgress)
{
  (void)theWS;
  return Read(theStreams, theShape, theProgress);
}

//==================================================================================================

bool DEOBJ_Provider::Read(ReadStreamList&                      theStreams,
                          const occ::handle<TDocStd_Document>& theDocument,
                          const Message_ProgressRange&         theProgress)
{
  const TCollection_AsciiString aContext("reading OBJ stream");
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext)
      || !DE_ValidationUtils::ValidateDocument(theDocument, aContext)
      || !DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                        STANDARD_TYPE(DEOBJ_ConfigurationNode),
                                                        aContext))
  {
    return false;
  }
  const occ::handle<DEOBJ_ConfigurationNode> aNode =
    occ::down_cast<DEOBJ_ConfigurationNode>(GetNode());
  RWObj_CafReader aReader;
  configureReader(aReader, *aNode);
  aReader.SetDocument(theDocument);
  if (!aReader.Perform(theStreams.First().Stream, theProgress, theStreams.First().Path))
  {
    return false;
  }
  XCAFDoc_DocumentTool::SetLengthUnit(theDocument,
                                      aNode->GlobalParameters.LengthUnit,
                                      UnitsMethods_LengthUnit_Millimeter);
  return true;
}

//==================================================================================================

bool DEOBJ_Provider::Read(ReadStreamList&              theStreams,
                          TopoDS_Shape&                theShape,
                          const Message_ProgressRange& theProgress)
{
  const TCollection_AsciiString aContext("reading OBJ stream");
  if (!DE_ValidationUtils::ValidateReadStreamList(theStreams, aContext)
      || !DE_ValidationUtils::ValidateConfigurationNode(GetNode(),
                                                        STANDARD_TYPE(DEOBJ_ConfigurationNode),
                                                        aContext))
  {
    return false;
  }
  const occ::handle<DEOBJ_ConfigurationNode> aNode =
    occ::down_cast<DEOBJ_ConfigurationNode>(GetNode());
  RWObj_TriangulationReader aReader;
  if (!configureShapeReader(aReader, *aNode)
      || !aReader.Read(theStreams.First().Stream, theStreams.First().Path, theProgress))
  {
    return false;
  }
  return readShapeResult(aReader, *aNode, theShape);
}

//==================================================================================================

TCollection_AsciiString DEOBJ_Provider::GetFormat() const
{
  return TCollection_AsciiString("OBJ");
}

//=================================================================================================

TCollection_AsciiString DEOBJ_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
