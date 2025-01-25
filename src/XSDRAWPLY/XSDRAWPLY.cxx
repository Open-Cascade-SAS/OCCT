// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <XSDRAWPLY.hxx>

#include <BRep_Builder.hxx>
#include <BRepLib_PointCloudShape.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <RWPly_CafWriter.hxx>
#include <RWPly_PlyWriterContext.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=======================================================================
// function : writeply
// purpose  : write PLY file
//=======================================================================
static Standard_Integer WritePly(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgVec)
{
  Handle(TDocStd_Document)    aDoc;
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  TCollection_AsciiString     aShapeName, aFileName;

  Standard_Real aDist     = 0.0;
  Standard_Real aDens     = Precision::Infinite();
  Standard_Real aTol      = Precision::Confusion();
  bool          hasColors = true, hasNormals = true, hasTexCoords = false, hasPartId = true,
       hasFaceId                                = false;
  bool                                 isPntSet = false, isDensityPoints = false;
  TColStd_IndexedDataMapOfStringString aFileInfo;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-normal")
    {
      hasNormals = Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArg == "-nonormal")
    {
      hasNormals = !Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArg == "-color" || anArg == "-nocolor" || anArg == "-colors" || anArg == "-nocolors")
    {
      hasColors = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArg == "-uv" || anArg == "-nouv")
    {
      hasTexCoords = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArg == "-partid")
    {
      hasPartId = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
      hasFaceId = hasFaceId && !hasPartId;
    }
    else if (anArg == "-surfid" || anArg == "-surfaceid" || anArg == "-faceid")
    {
      hasFaceId = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
      hasPartId = hasPartId && !hasFaceId;
    }
    else if (anArg == "-pntset" || anArg == "-pntcloud" || anArg == "-pointset"
             || anArg == "-pointcloud" || anArg == "-cloud" || anArg == "-points")
    {
      isPntSet = Draw::ParseOnOffIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if ((anArg == "-dist" || anArg == "-distance") && anArgIter + 1 < theNbArgs
             && Draw::ParseReal(theArgVec[anArgIter + 1], aDist))
    {
      ++anArgIter;
      isPntSet = true;
      if (aDist < 0.0)
      {
        theDI << "Syntax error: -distance value should be >= 0.0";
        return 1;
      }
      aDist = Max(aDist, Precision::Confusion());
    }
    else if ((anArg == "-dens" || anArg == "-density") && anArgIter + 1 < theNbArgs
             && Draw::ParseReal(theArgVec[anArgIter + 1], aDens))
    {
      ++anArgIter;
      isDensityPoints = Standard_True;
      isPntSet        = true;
      if (aDens <= 0.0)
      {
        theDI << "Syntax error: -density value should be > 0.0";
        return 1;
      }
    }
    else if ((anArg == "-tol" || anArg == "-tolerance") && anArgIter + 1 < theNbArgs
             && Draw::ParseReal(theArgVec[anArgIter + 1], aTol))
    {
      ++anArgIter;
      isPntSet = true;
      if (aTol < Precision::Confusion())
      {
        theDI << "Syntax error: -tol value should be >= " << Precision::Confusion();
        return 1;
      }
    }
    else if (anArg == "-comments" && anArgIter + 1 < theNbArgs)
    {
      aFileInfo.Add("Comments", theArgVec[++anArgIter]);
    }
    else if (anArg == "-author" && anArgIter + 1 < theNbArgs)
    {
      aFileInfo.Add("Author", theArgVec[++anArgIter]);
    }
    else if (aDoc.IsNull())
    {
      if (aShapeName.IsEmpty())
      {
        aShapeName = theArgVec[anArgIter];
      }

      Standard_CString aNameVar = theArgVec[anArgIter];
      DDocStd::GetDocument(aNameVar, aDoc, false);
      if (aDoc.IsNull())
      {
        TopoDS_Shape aShape = DBRep::Get(aNameVar);
        if (!aShape.IsNull())
        {
          anApp->NewDocument(TCollection_ExtendedString("BinXCAF"), aDoc);
          Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
          aShapeTool->AddShape(aShape);
        }
      }
    }
    else if (aFileName.IsEmpty())
    {
      aFileName = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aDoc.IsNull() && !aShapeName.IsEmpty())
  {
    theDI << "Syntax error: '" << aShapeName << "' is not a shape nor document";
    return 1;
  }
  else if (aDoc.IsNull() || aFileName.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  TDF_LabelSequence         aRootLabels;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  aShapeTool->GetFreeShapes(aRootLabels);
  if (aRootLabels.IsEmpty())
  {
    theDI << "Error: empty document";
    return 1;
  }

  if (isPntSet)
  {
    class PointCloudPlyWriter : public BRepLib_PointCloudShape, public RWPly_PlyWriterContext
    {
    public:
      PointCloudPlyWriter(Standard_Real theTol)
          : BRepLib_PointCloudShape(TopoDS_Shape(), theTol)
      {
      }

      void AddFaceColor(const TopoDS_Shape& theFace, const Graphic3d_Vec4ub& theColor)
      {
        myFaceColor.Bind(theFace, theColor);
      }

    protected:
      virtual void addPoint(const gp_Pnt&       thePoint,
                            const gp_Vec&       theNorm,
                            const gp_Pnt2d&     theUV,
                            const TopoDS_Shape& theFace)
      {
        Graphic3d_Vec4ub aColor;
        myFaceColor.Find(theFace, aColor);
        RWPly_PlyWriterContext::WriteVertex(
          thePoint,
          Graphic3d_Vec3((float)theNorm.X(), (float)theNorm.Y(), (float)theNorm.Z()),
          Graphic3d_Vec2((float)theUV.X(), (float)theUV.Y()),
          aColor);
      }

    private:
      NCollection_DataMap<TopoDS_Shape, Graphic3d_Vec4ub> myFaceColor;
    };

    PointCloudPlyWriter aPlyCtx(aTol);
    aPlyCtx.SetNormals(hasNormals);
    aPlyCtx.SetColors(hasColors);
    aPlyCtx.SetTexCoords(hasTexCoords);

    TopoDS_Compound aComp;
    BRep_Builder().MakeCompound(aComp);
    for (XCAFPrs_DocumentExplorer aDocExplorer(aDoc,
                                               aRootLabels,
                                               XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
         aDocExplorer.More();
         aDocExplorer.Next())
    {
      const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
      for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel,
                                         aDocNode.Location,
                                         true,
                                         aDocNode.Style);
           aFaceIter.More();
           aFaceIter.Next())
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

    Standard_Integer aNbPoints =
      isDensityPoints ? aPlyCtx.NbPointsByDensity(aDens) : aPlyCtx.NbPointsByTriangulation();
    if (aNbPoints <= 0)
    {
      theDI << "Error: unable to generate points";
      return 0;
    }

    if (!aPlyCtx.Open(aFileName)
        || !aPlyCtx.WriteHeader(aNbPoints, 0, TColStd_IndexedDataMapOfStringString()))
    {
      theDI << "Error: unable to create file '" << aFileName << "'";
      return 0;
    }

    Standard_Boolean isDone = isDensityPoints ? aPlyCtx.GeneratePointsByDensity(aDens)
                                              : aPlyCtx.GeneratePointsByTriangulation();
    if (!isDone)
    {
      theDI << "Error: Point cloud was not generated in file '" << aFileName << "'";
    }
    else if (!aPlyCtx.Close())
    {
      theDI << "Error: Point cloud file '" << aFileName << "' was not written";
    }
    else
    {
      theDI << aNbPoints;
    }
  }
  else
  {
    Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);
    RWPly_CafWriter                aPlyCtx(aFileName);
    aPlyCtx.SetNormals(hasNormals);
    aPlyCtx.SetColors(hasColors);
    aPlyCtx.SetTexCoords(hasTexCoords);
    aPlyCtx.SetPartId(hasPartId);
    aPlyCtx.SetFaceId(hasFaceId);
    aPlyCtx.Perform(aDoc, aFileInfo, aProgress->Start());
  }
  return 0;
}

//=================================================================================================

void XSDRAWPLY::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  const char* aGroup = "XSTEP-STL/VRML"; // Step transfer file commands
  // XSDRAW::LoadDraw(theCommands);
  theDI.Add("WritePly",
            R"(
WritePly Doc file [-normals {0|1}]=1 [-colors {0|1}]=1 [-uv {0|1}]=0 [-partId {0|1}]=1 [-faceId {0|1}]=0
                  [-pointCloud {0|1}]=0 [-distance Value]=0.0 [-density Value] [-tolerance Value]
Write document or triangulated shape into PLY file.
 -normals write per-vertex normals
 -colors  write per-vertex colors
 -uv      write per-vertex UV coordinates
 -partId  write per-element part index (alternative to -faceId)
 -faceId  write per-element face index (alternative to -partId)

Generate point cloud out of the shape and write it into PLY file.
 -pointCloud write point cloud instead without triangulation indices
 -distance   sets distance from shape into the range [0, Value];
 -density    sets density of points to generate randomly on surface;
 -tolerance  sets tolerance; default value is Precision::Confusion();
)",
            __FILE__,
            WritePly,
            aGroup);
  theDI.Add("writeply", "writeply shape file", __FILE__, WritePly, aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWPLY)
