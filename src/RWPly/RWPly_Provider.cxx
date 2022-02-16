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

#include <RWPly_Provider.hxx>

#include <BRep_Builder.hxx>
#include <BRepLib_PointCloudShape.hxx>
#include <DE_Wrapper.hxx>
#include <Message.hxx>
#include <RWPly_ConfigurationNode.hxx>
#include <RWPly_CafWriter.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <RWPly_PlyWriterContext.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(RWPly_Provider, DE_Provider)

//=======================================================================
// function : RWPly_Provider
// purpose  :
//=======================================================================
RWPly_Provider::RWPly_Provider()
{}

//=======================================================================
// function : RWPly_Provider
// purpose  :
//=======================================================================
RWPly_Provider::RWPly_Provider(const Handle(DE_ConfigurationNode)& theNode)
  :DE_Provider(theNode)
{}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWPly_Provider::Write(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theDocument, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWPly_Provider::Write(const TCollection_AsciiString& thePath,
                           const Handle(TDocStd_Document)& theDocument,
                           const Message_ProgressRange& theProgress)
{
  if (GetNode().IsNull() || !GetNode()->IsKind(STANDARD_TYPE(RWPly_ConfigurationNode)))
  {
    Message::SendFail() << "Error in the RWPly_Provider during writing the file " <<
      thePath << "\t: Incorrect or empty Configuration Node";
    return false;
  }
  Handle(RWPly_ConfigurationNode) aNode = Handle(RWPly_ConfigurationNode)::DownCast(GetNode());

  TDF_LabelSequence aRootLabels;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->GetFreeShapes(aRootLabels);
  if (aRootLabels.IsEmpty())
  {
    return Standard_True;
  }

  if (aNode->InternalParameters.WritePntSet)
  {
    class PointCloudPlyWriter : public BRepLib_PointCloudShape, public RWPly_PlyWriterContext
    {
    public:
      PointCloudPlyWriter(Standard_Real theTol)
        : BRepLib_PointCloudShape(TopoDS_Shape(), theTol)
      {}

      void AddFaceColor(const TopoDS_Shape& theFace, const Graphic3d_Vec4ub& theColor)
      {
        myFaceColor.Bind(theFace, theColor);
      }

    protected:
      virtual void addPoint(const gp_Pnt& thePoint,
                            const gp_Vec& theNorm,
                            const gp_Pnt2d& theUV,
                            const TopoDS_Shape& theFace)
      {
        Graphic3d_Vec4ub aColor;
        myFaceColor.Find(theFace, aColor);
        RWPly_PlyWriterContext::WriteVertex(thePoint,
                                            Graphic3d_Vec3((float)theNorm.X(), (float)theNorm.Y(), (float)theNorm.Z()),
                                            Graphic3d_Vec2((float)theUV.X(), (float)theUV.Y()),
                                            aColor);
      }

    private:
      NCollection_DataMap<TopoDS_Shape, Graphic3d_Vec4ub> myFaceColor;
    };

    PointCloudPlyWriter aPlyCtx(aNode->InternalParameters.WriteTolerance);
    aPlyCtx.SetNormals(aNode->InternalParameters.WriteNormals);
    aPlyCtx.SetColors(aNode->InternalParameters.WriteColors);
    aPlyCtx.SetTexCoords(aNode->InternalParameters.WriteTexCoords);

    TopoDS_Compound aComp;
    BRep_Builder().MakeCompound(aComp);
    for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument, aRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
         aDocExplorer.More(); aDocExplorer.Next())
    {
      const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
      for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, aDocNode.Location, true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
      {
        BRep_Builder().Add(aComp, aFaceIter.Face());
        Graphic3d_Vec4ub aColorVec(255);
        if (aFaceIter.HasFaceColor())
        {
          Graphic3d_Vec4 aColorF = aFaceIter.FaceColor();
          aColorVec.SetValues((unsigned char)int(aColorF.r() * 255.0f),
                              (unsigned char)int(aColorF.g() * 255.0f),
                              (unsigned char)int(aColorF.b() * 255.0f),
                              (unsigned char)int(aColorF.a() * 255.0f));
        }
        aPlyCtx.AddFaceColor(aFaceIter.Face(), aColorVec);
      }
    }
    aPlyCtx.SetShape(aComp);
    Standard_Integer aNbPoints = aNode->InternalParameters.WriteDensity > 0
      ? aPlyCtx.NbPointsByDensity(aNode->InternalParameters.WriteDensity)
      : aPlyCtx.NbPointsByTriangulation();
    if (aNbPoints <= 0)
    {
      Message::SendFail() << "Error in the RWPly_Provider during writing the file " <<
        thePath << "\t: Incorrect number of points";
      return false;
    }

    if (!aPlyCtx.Open(thePath)
        || !aPlyCtx.WriteHeader(aNbPoints, 0, TColStd_IndexedDataMapOfStringString()))
    {
      Message::SendFail() << "Error in the RWPly_Provider during writing the file " << thePath;
      return false;
    }

    Standard_Boolean isDone = aNode->InternalParameters.WriteDensity > 0
      ? aPlyCtx.GeneratePointsByDensity(aNode->InternalParameters.WriteDensity)
      : aPlyCtx.GeneratePointsByTriangulation();
    if (!isDone)
    {
      Message::SendFail() << "Error in the RWPly_Provider during writing the file " 
        << thePath << "\t: Error during generating point process";
      return false;
    }
  }
  else
  {
    TColStd_IndexedDataMapOfStringString aFileInfo;
    if (!aNode->InternalParameters.WriteAuthor.IsEmpty())
    {
      aFileInfo.Add("Author", aNode->InternalParameters.WriteAuthor);
    }
    if (!aNode->InternalParameters.WriteComment.IsEmpty())
    {
      aFileInfo.Add("Comments", aNode->InternalParameters.WriteComment);
    }
    RWMesh_CoordinateSystemConverter aConverter;
    aConverter.SetInputLengthUnit(aNode->GlobalParameters.LengthUnit / 1000);
    aConverter.SetInputCoordinateSystem(aNode->InternalParameters.SystemCS);
    aConverter.SetOutputLengthUnit(aNode->InternalParameters.FileLengthUnit);
    aConverter.SetOutputCoordinateSystem(aNode->InternalParameters.FileCS);

    RWPly_CafWriter aPlyCtx(thePath);
    aPlyCtx.SetNormals(aNode->InternalParameters.WriteNormals);
    aPlyCtx.SetColors(aNode->InternalParameters.WriteColors);
    aPlyCtx.SetTexCoords(aNode->InternalParameters.WriteTexCoords);
    aPlyCtx.SetPartId(aNode->InternalParameters.WritePartId);
    aPlyCtx.SetFaceId(aNode->InternalParameters.WriteFaceId);
    if (!aPlyCtx.Perform(theDocument, aFileInfo, theProgress))
    {
      Message::SendFail() << "Error in the RWPly_Provider during writing the file " 
        << thePath << "\t: Cannot perform the document";
      return false;
    }
  }
  return true;
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWPly_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape& theShape,
                           Handle(XSControl_WorkSession)& theWS,
                           const Message_ProgressRange& theProgress)
{
  (void)theWS;
  return Write(thePath, theShape, theProgress);
}

//=======================================================================
// function : Write
// purpose  :
//=======================================================================
bool RWPly_Provider::Write(const TCollection_AsciiString& thePath,
                           const TopoDS_Shape& theShape,
                           const Message_ProgressRange& theProgress)
{
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinXCAF");
  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShTool->AddShape(theShape);
  return Write(thePath, aDoc, theProgress);
}

//=======================================================================
// function : GetFormat
// purpose  :
//=======================================================================
TCollection_AsciiString RWPly_Provider::GetFormat() const
{
  return TCollection_AsciiString("PLY");
}

//=======================================================================
// function : GetVendor
// purpose  :
//=======================================================================
TCollection_AsciiString RWPly_Provider::GetVendor() const
{
  return TCollection_AsciiString("OCC");
}
