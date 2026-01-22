// Created on: 2000-08-04
// Created by: Pavel TELKOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XSDRAW.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <DBRep.hxx>
#include <DDF_Browser.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_AssemblyIterator.hxx>
#include <XCAFDoc_AssemblyGraph.hxx>
#include <XCAFDoc_AssemblyTool.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Editor.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <XCAFPrs_Driver.hxx>
#include <XDEDRAW.hxx>
#include <XDEDRAW_Colors.hxx>
#include <XDEDRAW_Common.hxx>
#include <XDEDRAW_Layers.hxx>
#include <XDEDRAW_Props.hxx>
#include <XDEDRAW_Shapes.hxx>
#include <XDEDRAW_GDTs.hxx>
#include <XDEDRAW_Views.hxx>
#include <XDEDRAW_Notes.hxx>
#include <UnitsMethods.hxx>

#include <BinXCAFDrivers.hxx>
#include <XmlXCAFDrivers.hxx>

#include <cstdio>

//=======================================================================
// Section: General commands
//=======================================================================

//=================================================================================================

static int newDoc(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Give document name\n";
    return 1;
  }

  occ::handle<TDocStd_Document>     D;
  occ::handle<DDocStd_DrawDocument> DD;
  occ::handle<TDocStd_Application>  A = DDocStd::GetApplication();

  if (!DDocStd::GetDocument(argv[1], D, false))
  {
    A->NewDocument("BinXCAF", D);
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(), argv[1]);
    Draw::Set(argv[1], DD);
    di << "document " << argv[1] << " created\n";
    // DDocStd::ReturnLabel(di,D->Main());
  }
  else
    di << argv[1] << " is already a document\n";

  return 0;
}

//=================================================================================================

static int saveDoc(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<TDocStd_Document>    D;
  occ::handle<TDocStd_Application> A = DDocStd::GetApplication();

  if (argc == 1)
  {
    if (A->NbDocuments() < 1)
      return 1;
    A->GetDocument(1, D);
  }
  else
  {
    if (!DDocStd::GetDocument(argv[1], D))
      return 1;
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di);

  PCDM_StoreStatus aStatus = PCDM_SS_Doc_IsNull;
  if (argc == 3)
  {
    TCollection_ExtendedString path(argv[2]);
    aStatus = A->SaveAs(D, path, aProgress->Start());
  }
  else if (!D->IsSaved())
  {
    std::cout << "Storage error: this document has never been saved\n";
    return 1;
  }
  else
  {
    aStatus = A->Save(D, aProgress->Start());
  }

  switch (aStatus)
  {
    case PCDM_SS_OK:
      break;
    case PCDM_SS_DriverFailure:
      di << "Storage error: driver failure\n";
      break;
    case PCDM_SS_WriteFailure:
      di << "Storage error: write failure\n";
      break;
    case PCDM_SS_Failure:
      di << "Storage error: general failure\n";
      break;
    case PCDM_SS_Doc_IsNull:
      di << "Storage error: document is NULL\n";
      break;
    case PCDM_SS_No_Obj:
      di << "Storage error: no object\n";
      break;
    case PCDM_SS_Info_Section_Error:
      di << "Storage error: section error\n";
      break;
    case PCDM_SS_UserBreak:
      di << "Storage error: user break\n";
      break;
    case PCDM_SS_UnrecognizedFormat:
      di << "Storage error: unrecognized document storage format " << D->StorageFormat() << "\n";
      break;
  }

  return 0;
}

//=================================================================================================

static int openDoc(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<TDocStd_Document>     D;
  occ::handle<DDocStd_DrawDocument> DD;
  occ::handle<TDocStd_Application>  A = DDocStd::GetApplication();

  if (argc < 3)
  {
    di << "invalid number of arguments. Usage:\t XOpen filename docname [-skipAttribute] "
          "[-readAttribute] [-readPath] [-append|-overwrite]\n";
    return 1;
  }

  TCollection_AsciiString Filename = argv[1];
  const char*             DocName  = argv[2];

  occ::handle<PCDM_ReaderFilter> aFilter = new PCDM_ReaderFilter;
  for (int i = 3; i < argc; i++)
  {
    TCollection_AsciiString anArg(argv[i]);
    if (anArg == "-append")
    {
      aFilter->Mode() = PCDM_ReaderFilter::AppendMode_Protect;
    }
    else if (anArg == "-overwrite")
    {
      aFilter->Mode() = PCDM_ReaderFilter::AppendMode_Overwrite;
    }
    else if (anArg.StartsWith("-skip"))
    {
      TCollection_AsciiString anAttrType = anArg.SubString(6, anArg.Length());
      aFilter->AddSkipped(anAttrType);
    }
    else if (anArg.StartsWith("-read"))
    {
      TCollection_AsciiString aValue = anArg.SubString(6, anArg.Length());
      if (aValue.Value(1) == '0') // path
      {
        aFilter->AddPath(aValue);
      }
      else // attribute to read
      {
        aFilter->AddRead(aValue);
      }
    }
  }

  if (aFilter->IsAppendMode() && !DDocStd::GetDocument(DocName, D, false))
  {
    di << "for append mode document " << DocName << " must be already created\n";
    return 1;
  }

  occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di);
  if (A->Open(Filename, D, aFilter, aProgress->Start()) != PCDM_RS_OK)
  {
    di << "cannot open XDE document\n";
    return 1;
  }

  if (!aFilter->IsAppendMode())
  {
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(), DocName);
    Draw::Set(DocName, DD);
  }

  di << "document " << DocName << " opened\n";

  return 0;
}

//=================================================================================================

static int dump(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " Doc [int deep (0/1)]\n";
    return 1;
  }
  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  occ::handle<XCAFDoc_ShapeTool> myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  bool                           deep       = false;
  if ((argc == 3) && (Draw::Atoi(argv[2]) == 1))
    deep = true;
  Standard_SStream aDumpLog;
  myAssembly->Dump(aDumpLog, deep);
  di << aDumpLog;
  return 0;
}

//=======================================================================
// function : StatAssembly
// purpose  : recursive part of statistics
//=======================================================================

static void StatAssembly(const TDF_Label                        L,
                         const int                              level,
                         occ::handle<NCollection_HArray1<int>>& HAI,
                         int&                                   NbCentroidProp,
                         int&                                   NbVolumeProp,
                         int&                                   NbAreaProp,
                         int&                                   NbShapesWithName,
                         int&                                   NbShapesWithColor,
                         int&                                   NbShapesWithLayer,
                         int&                                   NbShapesWithVisMaterial,
                         occ::handle<TDocStd_Document>&         aDoc,
                         bool&                                  PrintStructMode,
                         Draw_Interpretor&                      di)
{
  if (PrintStructMode)
  {
    for (int j = 0; j <= level; j++)
      di << "  ";
  }
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(L, Entry);
  if (PrintStructMode)
    di << Entry.ToCString();

  occ::handle<TDataStd_Name> Name;
  if (L.FindAttribute(TDataStd_Name::GetID(), Name))
  {
    NbShapesWithName++;
    if (PrintStructMode)
    {
      di << " " << Name->Get() << "  has attributes: ";
    }
  }
  else
  {
    if (PrintStructMode)
      di << " NoName  has attributes: ";
  }

  occ::handle<XCAFDoc_Centroid> aCentroid = new (XCAFDoc_Centroid);
  if (L.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid))
  {
    if (PrintStructMode)
      di << "Centroid ";
    NbCentroidProp++;
  }
  double tmp;
  if (XCAFDoc_Volume::Get(L, tmp))
  {
    if (PrintStructMode)
      di << "Volume(" << tmp << ") ";
    NbVolumeProp++;
  }
  if (XCAFDoc_Area::Get(L, tmp))
  {
    if (PrintStructMode)
      di << "Area(" << tmp << ") ";
    NbAreaProp++;
  }
  occ::handle<XCAFDoc_ColorTool>       CTool  = XCAFDoc_DocumentTool::ColorTool(aDoc->Main());
  occ::handle<XCAFDoc_LayerTool>       LTool  = XCAFDoc_DocumentTool::LayerTool(aDoc->Main());
  occ::handle<XCAFDoc_VisMaterialTool> VMTool = XCAFDoc_DocumentTool::VisMaterialTool(aDoc->Main());
  Quantity_ColorRGBA                   col;
  bool                                 IsColor   = false;
  bool                                 IsByLayer = false;
  if (CTool->GetColor(L, XCAFDoc_ColorGen, col))
    IsColor = true;
  else if (CTool->GetColor(L, XCAFDoc_ColorSurf, col))
    IsColor = true;
  else if (CTool->GetColor(L, XCAFDoc_ColorCurv, col))
    IsColor = true;
  else if (CTool->IsColorByLayer(L))
    IsByLayer = true;
  if (IsColor || IsByLayer)
  {
    if (IsByLayer)
    {
      occ::handle<NCollection_HSequence<TCollection_ExtendedString>> aLayerS;
      LTool->GetLayers(L, aLayerS);
      // Currently for DXF only, thus
      // only 1 Layer should be.
      if (aLayerS->Length() == 1)
      {
        TDF_Label          aLayer = LTool->FindLayer(aLayerS->First());
        Quantity_ColorRGBA aColor;
        if (CTool->GetColor(aLayer, XCAFDoc_ColorGen, aColor))
        {
          TCollection_AsciiString aColorName = aColor.GetRGB().StringName(aColor.GetRGB().Name());
          di << "Color(" << aColorName.ToCString() << ") ";
        }
        else
        {
          di << "Color(ByLayer) ";
        }
      }
      NbShapesWithColor++;
    }
    else
    {
      TCollection_AsciiString Entry1;
      Entry1 = col.GetRGB().StringName(col.GetRGB().Name());
      if (PrintStructMode)
        di << "Color(" << Entry1.ToCString() << " " << col.Alpha() << ") ";
      NbShapesWithColor++;
    }
  }
  occ::handle<NCollection_HSequence<TCollection_ExtendedString>> aLayerS;
  LTool->GetLayers(L, aLayerS);
  if (!aLayerS.IsNull() && aLayerS->Length() > 0)
  {
    if (PrintStructMode)
    {
      di << "Layer(";
      for (int i = 1; i <= aLayerS->Length(); i++)
      {
        TCollection_AsciiString Entry2(aLayerS->Value(i));
        if (i == 1)
          di << "\"" << Entry2.ToCString() << "\"";
        else
          di << " \"" << Entry2.ToCString() << "\"";
      }
      di << ") ";
    }
    NbShapesWithLayer++;
  }

  TDF_Label aVMat;
  if (VMTool->GetShapeMaterial(L, aVMat))
  {
    if (PrintStructMode)
    {
      di << "VisMaterial(";
      occ::handle<TDataStd_Name> aNodeName;
      if (aVMat.FindAttribute(TDataStd_Name::GetID(), aNodeName))
      {
        di << "\"" << aNodeName->Get() << "\"";
      }
      di << ") ";
    }
    NbShapesWithVisMaterial++;
  }
  if (PrintStructMode)
    di << "\n";

  HAI->SetValue(level, HAI->Value(level) + 1);
  if (L.HasChild())
  {
    for (int i = 1; i <= L.NbChildren(); i++)
    {
      StatAssembly(L.FindChild(i),
                   level + 1,
                   HAI,
                   NbCentroidProp,
                   NbVolumeProp,
                   NbAreaProp,
                   NbShapesWithName,
                   NbShapesWithColor,
                   NbShapesWithLayer,
                   NbShapesWithVisMaterial,
                   aDoc,
                   PrintStructMode,
                   di);
    }
  }
}

//=================================================================================================

static int statdoc(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " Doc \n";
    return 1;
  }
  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  bool                           PrintStructMode = (argc == 3);
  occ::handle<XCAFDoc_ShapeTool> aTool           = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  NCollection_Sequence<TDF_Label> SeqLabels;
  aTool->GetShapes(SeqLabels);
  if (SeqLabels.Length() <= 0)
    return 0;
  if (PrintStructMode)
    di << "\nStructure of shapes in the document:\n";
  int level          = 0;
  int NbCentroidProp = 0, NbVolumeProp = 0, NbAreaProp = 0;
  int NbShapesWithName = 0, NbShapesWithColor = 0, NbShapesWithLayer = 0,
      NbShapesWithVisMaterial               = 0;
  occ::handle<NCollection_HArray1<int>> HAI = new NCollection_HArray1<int>(0, 20);
  int                                   i   = 0;
  for (i = 0; i <= 20; i++)
    HAI->SetValue(i, 0);
  for (i = 1; i <= SeqLabels.Length(); i++)
  {
    StatAssembly(SeqLabels.Value(i),
                 level,
                 HAI,
                 NbCentroidProp,
                 NbVolumeProp,
                 NbAreaProp,
                 NbShapesWithName,
                 NbShapesWithColor,
                 NbShapesWithLayer,
                 NbShapesWithVisMaterial,
                 Doc,
                 PrintStructMode,
                 di);
  }
  int NbLabelsShape = 0;
  di << "\nStatistis of shapes in the document:\n";
  for (i = 0; i <= 20; i++)
  {
    if (HAI->Value(i) == 0)
      break;
    // di<<"level N "<<i<<" :  number of labels with shape = "<<HAI->Value(i)<<"\n";
    di << "level N " << i << " : " << HAI->Value(i) << "\n";
    NbLabelsShape = NbLabelsShape + HAI->Value(i);
  }
  di << "Total number of labels for shapes in the document = " << NbLabelsShape << "\n";
  di << "Number of labels with name = " << NbShapesWithName << "\n";
  di << "Number of labels with color link = " << NbShapesWithColor << "\n";
  di << "Number of labels with layer link = " << NbShapesWithLayer << "\n";
  di << "Number of labels with vis material link = " << NbShapesWithVisMaterial << "\n";

  di << "\nStatistis of Props in the document:\n";
  di << "Number of Centroid Props = " << NbCentroidProp << "\n";
  di << "Number of Volume Props = " << NbVolumeProp << "\n";
  di << "Number of Area Props = " << NbAreaProp << "\n";

  occ::handle<XCAFDoc_ColorTool>  CTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  NCollection_Sequence<TDF_Label> CLabels;
  CTool->GetColors(CLabels);
  di << "\nNumber of colors = " << CLabels.Length() << "\n";
  if (CLabels.Length() > 0)
  {
    for (i = 1; i <= CLabels.Length(); i++)
    {
      TDF_Label      aLabel = CLabels.Value(i);
      Quantity_Color col;
      CTool->GetColor(aLabel, col);
      di << Quantity_Color::StringName(col.Name()) << " ";
    }
    di << "\n";
  }

  occ::handle<XCAFDoc_LayerTool>  LTool = XCAFDoc_DocumentTool::LayerTool(Doc->Main());
  NCollection_Sequence<TDF_Label> LLabels;
  LTool->GetLayerLabels(LLabels);
  di << "\nNumber of layers = " << LLabels.Length() << "\n";
  if (LLabels.Length() > 0)
  {
    for (i = 1; i <= LLabels.Length(); i++)
    {
      TDF_Label                  aLabel = LLabels.Value(i);
      TCollection_ExtendedString layerName;
      LTool->GetLayer(aLabel, layerName);
      di << "\"" << layerName << "\" ";
    }
    di << "\n";
  }

  occ::handle<XCAFDoc_VisMaterialTool> VMTool = XCAFDoc_DocumentTool::VisMaterialTool(Doc->Main());
  NCollection_Sequence<TDF_Label>      aVMats;
  VMTool->GetMaterials(aVMats);
  di << "\nNumber of vis materials = " << aVMats.Length() << "\n";
  if (!aVMats.IsEmpty())
  {
    for (NCollection_Sequence<TDF_Label>::Iterator aVMIter(aVMats); aVMIter.More(); aVMIter.Next())
    {
      occ::handle<TDataStd_Name> aNodeName;
      if (aVMIter.Value().FindAttribute(TDataStd_Name::GetID(), aNodeName))
      {
        di << "\"" << aNodeName->Get() << "\" ";
      }
    }
    di << "\n";
  }

  di << "\n";
  return 0;
}

//=================================================================================================

static int setPrs(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " DocName [label1 label2 ...] \n";
    return 1;
  }

  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  // collect sequence of labels to set presentation
  occ::handle<XCAFDoc_ShapeTool>  shapes = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  NCollection_Sequence<TDF_Label> seq;
  if (argc > 2)
  {
    for (int i = 2; i < argc; i++)
    {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
      if (aLabel.IsNull() || !shapes->IsShape(aLabel))
      {
        di << argv[i] << " is not a valid shape label!";
        continue;
      }
      seq.Append(aLabel);
    }
  }
  else
  {
    shapes->GetShapes(seq);
  }

  // set presentations
  occ::handle<XCAFDoc_ColorTool> colors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  for (int i = 1; i <= seq.Length(); i++)
  {
    occ::handle<TPrsStd_AISPresentation> prs;
    if (!seq.Value(i).FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
    {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i), XCAFPrs_Driver::GetID());
      prs->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
    }
    //    Quantity_Color Col;
    //    if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorSurf, Col ) )
    //      prs->SetColor ( Col.Name() );
    //    else if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorCurv, Col ) )
    //      prs->SetColor ( Col.Name() );
  }
  return 0;
}

//=================================================================================================

static int show(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " DocName [label1 label2 ...] \n";
    return 1;
  }

  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << argv[1] << " is not a document\n";
    return 1;
  }

  // init viewer
  TDF_Label                      aRoot = aDoc->GetData()->Root();
  occ::handle<TPrsStd_AISViewer> aDocViewer;
  TCollection_AsciiString        aViewName =
    TCollection_AsciiString("Driver1/Document_") + argv[1] + "/View1";
  if (!TPrsStd_AISViewer::Find(aRoot, aDocViewer))
  {
    ViewerTest::ViewerInit(aViewName);
    aDocViewer = TPrsStd_AISViewer::New(aRoot, ViewerTest::GetAISContext());
  }

  // collect sequence of labels to display
  occ::handle<XCAFDoc_ShapeTool>  shapes = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());
  NCollection_Sequence<TDF_Label> seq;
  if (argc > 2)
  {
    for (int i = 2; i < argc; i++)
    {
      TDF_Label aLabel;
      TDF_Tool::Label(aDoc->GetData(), argv[i], aLabel);
      if (aLabel.IsNull() || !shapes->IsShape(aLabel))
      {
        di << argv[i] << " is not a valid shape label!";
        continue;
      }
      seq.Append(aLabel);
    }
  }
  else
  {
    shapes->GetFreeShapes(seq);
  }

  // set presentations and show
  // occ::handle<XCAFDoc_ColorTool> colors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  for (int i = 1; i <= seq.Length(); i++)
  {
    occ::handle<TPrsStd_AISPresentation> prs;
    if (!seq.Value(i).FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
    {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i), XCAFPrs_Driver::GetID());
      prs->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
    }
    //    Quantity_Color Col;
    //    if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorSurf, Col ) )
    //      prs->SetColor ( Col.Name() );
    //    else if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorCurv, Col ) )
    //      prs->SetColor ( Col.Name() );
    prs->Display(true);
  }
  TPrsStd_AISViewer::Update(aDoc->GetData()->Root());
  return 0;
}

//! XDisplay command implementation.
class XDEDRAW_XDisplayTool
{
public:
  //! XDisplay command interface.
  static int XDisplay(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
  {
    XDEDRAW_XDisplayTool aTool;
    return aTool.xdisplay(theDI, theNbArgs, theArgVec);
  }

private:
  //! Constructor.
  XDEDRAW_XDisplayTool()
      : myDispMode(-2),
        myHiMode(-2),
        myIsAutoTriang(-1),
        myToPrefixDocName(true),
        myToGetNames(true),
        myToExplore(false)
  {
  }

  //! Display single label.
  int displayLabel(Draw_Interpretor&              theDI,
                   const TDF_Label&               theLabel,
                   const TCollection_AsciiString& theNamePrefix,
                   const TopLoc_Location&         theLoc,
                   TCollection_AsciiString&       theOutDispList)
  {
    TCollection_AsciiString aName;
    if (myToGetNames)
    {
      occ::handle<TDataStd_Name> aNodeName;
      if (theLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName))
      {
        aName = aNodeName->Get();
      }
      if (aName.IsEmpty())
      {
        TDF_Label aRefLabel;
        if (XCAFDoc_ShapeTool::GetReferredShape(theLabel, aRefLabel)
            && aRefLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName))
        {
          aName = aNodeName->Get();
        }
      }

      if (aName.IsEmpty())
      {
        TDF_Tool::Entry(theLabel, aName);
      }
      for (int aNameIndex = 1;; ++aNameIndex)
      {
        if (myNameMap.Add(aName))
        {
          break;
        }
        aName = aNodeName->Get() + "_" + aNameIndex;
      }
    }
    else
    {
      TDF_Tool::Entry(theLabel, aName);
    }
    aName = theNamePrefix + aName;

    if (myToExplore)
    {
      TDF_Label aRefLabel = theLabel;
      XCAFDoc_ShapeTool::GetReferredShape(theLabel, aRefLabel);
      if (XCAFDoc_ShapeTool::IsAssembly(aRefLabel))
      {
        aName += "/";
        const TopLoc_Location aLoc = theLoc * XCAFDoc_ShapeTool::GetLocation(theLabel);
        for (TDF_ChildIterator aChildIter(aRefLabel); aChildIter.More(); aChildIter.Next())
        {
          if (displayLabel(theDI, aChildIter.Value(), aName, aLoc, theOutDispList) == 1)
          {
            return 1;
          }
        }
        return 0;
      }
    }

    occ::handle<XCAFPrs_AISObject> aPrs = new XCAFPrs_AISObject(theLabel);
    if (!theLoc.IsIdentity())
    {
      aPrs->SetLocalTransformation(theLoc);
    }
    if (myDispMode != -2)
    {
      if (myDispMode == -1)
      {
        aPrs->UnsetDisplayMode();
      }
      if (!aPrs->AcceptDisplayMode(myDispMode))
      {
        theDI << "Syntax error: " << aPrs->DynamicType()->Name() << " rejects " << myDispMode
              << " display mode";
        return 1;
      }
      else
      {
        aPrs->SetDisplayMode(myDispMode);
      }
    }
    if (myHiMode != -2)
    {
      if (myHiMode != -1 && !aPrs->AcceptDisplayMode(myHiMode))
      {
        theDI << "Syntax error: " << aPrs->DynamicType()->Name() << " rejects " << myHiMode
              << " display mode";
        return 1;
      }
      aPrs->SetHilightMode(myHiMode);
    }
    if (myIsAutoTriang != -1)
    {
      aPrs->Attributes()->SetAutoTriangulation(myIsAutoTriang == 1);
    }

    ViewerTest::Display(aName, aPrs, false);
    theOutDispList += aName + " ";
    return 0;
  }

  //! XDisplay command implementation.
  int xdisplay(Draw_Interpretor& theDI, int theNbArgs, const char** theArgVec)
  {
    occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
    if (aContext.IsNull())
    {
      theDI << "Error: no active viewer";
      return 1;
    }

    ViewerTest_AutoUpdater anAutoUpdater(aContext, ViewerTest::CurrentView());
    for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
    {
      TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
      anArgCase.LowerCase();
      if (anAutoUpdater.parseRedrawMode(anArgCase))
      {
        continue;
      }
      else if (anArgIter + 1 < theNbArgs && myDispMode == -2
               && (anArgCase == "-dispmode" || anArgCase == "-displaymode")
               && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        myDispMode = TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      }
      else if (anArgIter + 1 < theNbArgs && myHiMode == -2
               && (anArgCase == "-himode" || anArgCase == "-highmode"
                   || anArgCase == "-highlightmode")
               && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        myHiMode = TCollection_AsciiString(theArgVec[++anArgIter]).IntegerValue();
      }
      else if (anArgCase == "-autotr" || anArgCase == "-autotrian" || anArgCase == "-autotriang"
               || anArgCase == "-autotriangulation" || anArgCase == "-noautotr"
               || anArgCase == "-noautotrian" || anArgCase == "-noautotriang"
               || anArgCase == "-noautotriangulation")
      {
        myIsAutoTriang = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter) ? 1 : 0;
      }
      else if (anArgCase == "-docprefix" || anArgCase == "-nodocprefix")
      {
        myToPrefixDocName = true;
        if (anArgIter + 1 < theNbArgs
            && Draw::ParseOnOff(theArgVec[anArgIter + 1], myToPrefixDocName))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith("-no"))
        {
          myToPrefixDocName = !myToPrefixDocName;
        }
      }
      else if (anArgCase == "-names" || anArgCase == "-nonames")
      {
        myToGetNames = true;
        if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], myToGetNames))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith("-no"))
        {
          myToGetNames = !myToGetNames;
        }
      }
      else if (anArgCase == "-explore" || anArgCase == "-noexplore")
      {
        myToExplore = true;
        if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], myToExplore))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith("-no"))
        {
          myToExplore = !myToExplore;
        }
      }
      else if (anArgCase == "-outdisplist" && anArgIter + 1 < theNbArgs)
      {
        myOutDispListVar = theArgVec[++anArgIter];
        myOutDispList.Clear();
      }
      else
      {
        if (myDoc.IsNull() && DDocStd::GetDocument(theArgVec[anArgIter], myDoc, false))
        {
          myDocName = theArgVec[anArgIter];
          continue;
        }

        TCollection_AsciiString aValue(theArgVec[anArgIter]);
        const int               aFirstSplit = aValue.Search(":");
        if (!IsDigit(aValue.Value(1)) && aFirstSplit >= 2 && aFirstSplit < aValue.Length())
        {
          TCollection_AsciiString       aDocName    = aValue.SubString(1, aFirstSplit - 1);
          const char*                   aDocNameStr = aDocName.ToCString();
          occ::handle<TDocStd_Document> aDoc;
          if (DDocStd::GetDocument(aDocNameStr, aDoc, false))
          {
            aValue = aValue.SubString(aFirstSplit + 1, aValue.Length());
            if (!myDoc.IsNull() && myDoc != aDoc)
            {
              theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
              return 1;
            }
            myDoc     = aDoc;
            myDocName = aDocName;
          }
        }
        if (myDoc.IsNull())
        {
          theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        TDF_Label aLabel;
        TDF_Tool::Label(myDoc->GetData(), aValue.ToCString(), aLabel);
        if (aLabel.IsNull() || !XCAFDoc_ShapeTool::IsShape(aLabel))
        {
          theDI << "Syntax error: " << aValue << " is not a valid shape label";
          return 1;
        }
        myLabels.Append(aLabel);
      }
    }
    if (myDoc.IsNull())
    {
      theDI << "Syntax error: not enough arguments";
      return 1;
    }
    if (myLabels.IsEmpty())
    {
      XCAFDoc_DocumentTool::ShapeTool(myDoc->Main())->GetFreeShapes(myLabels);
    }

    for (NCollection_Sequence<TDF_Label>::Iterator aLabIter(myLabels); aLabIter.More();
         aLabIter.Next())
    {
      const TDF_Label& aLabel = aLabIter.Value();
      if (displayLabel(theDI,
                       aLabel,
                       myToPrefixDocName ? myDocName + ":" : "",
                       TopLoc_Location(),
                       myOutDispList)
          == 1)
      {
        return 1;
      }
    }
    if (myOutDispListVar.IsEmpty())
    {
      theDI << myOutDispList;
    }
    else
    {
      Draw::Set(myOutDispListVar.ToCString(), myOutDispList.ToCString());
    }
    return 0;
  }

private:
  NCollection_Map<TCollection_AsciiString> myNameMap; //!< names map to handle collisions
  occ::handle<TDocStd_Document>            myDoc;     //!< document
  TCollection_AsciiString                  myDocName; //!< document name
  TCollection_AsciiString         myOutDispListVar;   //!< tcl variable to print the result objects
  TCollection_AsciiString         myOutDispList; //!< string with list of all displayed object names
  NCollection_Sequence<TDF_Label> myLabels;      //!< labels to display
  int                             myDispMode;    //!< shape display mode
  int                             myHiMode;      //!< shape highlight mode
  int                             myIsAutoTriang;    //!< auto-triangulation mode
  bool                            myToPrefixDocName; //!< flag to prefix objects with document name
  bool                            myToGetNames;      //!< flag to use label names or tags
  bool                            myToExplore;       //!< flag to explore assembles
};

//=================================================================================================

static int xwd(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " DocName filename.{xwd|gif|bmp}\n";
    return 1;
  }

  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> IC;
  if (!TPrsStd_AISViewer::Find(Doc->GetData()->Root(), IC))
  {
    di << "Cannot find viewer for document " << argv[1] << "\n";
    return 1;
  }

  occ::handle<V3d_Viewer>                           aViewer   = IC->CurrentViewer();
  NCollection_List<occ::handle<V3d_View>>::Iterator aViewIter = aViewer->ActiveViewIterator();
  if (aViewIter.More())
  {
    aViewIter.Value()->Dump(argv[2]);
  }
  else
  {
    di << "Cannot find an active view in a viewer " << argv[1] << "\n";
    return 1;
  }

  return 0;
}

//=================================================================================================

static int XAttributeValue(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 4)
  {
    std::cout << "Syntax error: Too few args\n";
    return 1;
  }
  occ::handle<DDF_Browser> browser = occ::down_cast<DDF_Browser>(Draw::GetExisting(argv[1]));
  if (browser.IsNull())
  {
    std::cout << "Syntax error: Not a browser: " << argv[1] << "\n";
    return 1;
  }

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(), argv[2], lab);
  if (lab.IsNull())
  {
    std::cout << "Syntax error: label is Null: " << argv[2] << "\n";
    return 1;
  }

  int                   num = Draw::Atoi(argv[3]);
  TDF_AttributeIterator itr(lab, false);
  for (int i = 1; itr.More() && i < num; i++)
    itr.Next();

  if (!itr.More())
  {
    std::cout << "Syntax error: Attribute #" << num << " not found\n";
    return 1;
  }

  TCollection_AsciiString anInfo = XCAFDoc::AttributeInfo(itr.Value());
  if (!anInfo.IsEmpty())
  {
    di << anInfo.ToCString();
  }
  return 0;
}

//=================================================================================================

static int setviewName(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " (1/0)\n";
    return 1;
  }
  bool mode = false;
  if (Draw::Atoi(argv[1]) == 1)
    mode = true;
  XCAFPrs::SetViewNameMode(mode);
  return 0;
}

//=================================================================================================

static int getviewName(Draw_Interpretor& di, int /*argc*/, const char** /*argv*/)
{
  if (XCAFPrs::GetViewNameMode())
    di << "Display names ON\n";
  else
    di << "Display names OFF\n";
  return 0;
}

//=================================================================================================

static int XSetTransparency(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc Transparency [label1 label2 ...] \n";
    return 1;
  }

  occ::handle<TDocStd_Document> Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  const double aTransparency = Draw::Atof(argv[2]);

  // collect sequence of labels
  occ::handle<XCAFDoc_ShapeTool>  shapes = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  NCollection_Sequence<TDF_Label> seq;
  if (argc > 3)
  {
    for (int i = 3; i < argc; i++)
    {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
      if (aLabel.IsNull() || !shapes->IsShape(aLabel))
      {
        di << argv[i] << " is not a valid shape label!";
        continue;
      }
      seq.Append(aLabel);
    }
  }
  else
  {
    shapes->GetFreeShapes(seq);
  }

  // find presentations and set transparency
  for (int i = 1; i <= seq.Length(); i++)
  {
    occ::handle<TPrsStd_AISPresentation> prs;
    if (!seq.Value(i).FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
    {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i), XCAFPrs_Driver::GetID());
      prs->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
    }
    prs->SetTransparency(aTransparency);
  }
  TPrsStd_AISViewer::Update(Doc->GetData()->Root());
  return 0;
}

//=================================================================================================

static int setLengthUnit(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Use: " << argv[0] << " Doc {unitName|scaleFactor} \n";
    return 1;
  }

  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << "Error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TCollection_AsciiString anUnit(argv[2]);
  double                  anUnitValue = 1.;
  TCollection_AsciiString anUnitName;
  if (!anUnit.IsRealValue(true))
  {
    UnitsMethods_LengthUnit aTypeUnit =
      UnitsMethods::LengthUnitFromString(anUnit.ToCString(), false);
    if (aTypeUnit == UnitsMethods_LengthUnit_Undefined)
    {
      di << "Error: " << anUnit
         << " is incorrect unit, use m, mm, km, cm, micron, mille, in, min, nin, ft, stat.mile\n";
      return 1;
    }
    anUnitName  = anUnit;
    anUnitValue = UnitsMethods::GetLengthFactorValue(aTypeUnit) * 0.001;
  }
  else
  {
    anUnitValue = anUnit.RealValue();
    anUnitName  = UnitsMethods::DumpLengthUnit(anUnitValue, UnitsMethods_LengthUnit_Meter);
  }
  XCAFDoc_LengthUnit::Set(aDoc->Main().Root(), anUnitName, anUnitValue);
  return 0;
}

//=================================================================================================

static int dumpLengthUnit(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2 && argc != 3)
  {
    di << "Use: " << argv[0] << " Doc [-scale]\n";
    return 1;
  }

  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << "Error: " << argv[1] << " is not a document\n";
    return 1;
  }
  occ::handle<XCAFDoc_LengthUnit> aUnits;
  if (!aDoc->Main().Root().FindAttribute(XCAFDoc_LengthUnit::GetID(), aUnits))
  {
    di << "Error: Document doesn't contain a Length Unit\n";
    return 1;
  }
  if (argc == 3)
  {
    TCollection_AsciiString anOption(argv[2]);
    anOption.LowerCase();
    if (anOption.IsEqual("-scale"))
    {
      di << aUnits->GetUnitValue();
      return 0;
    }
    else
    {
      di << "Error: Incorrect option, use -scale\n";
      return 1;
    }
  }
  di << aUnits->GetUnitName();
  return 0;
}

//=======================================================================
// function : XShowFaceBoundary
// purpose  : Set face boundaries on/off
//=======================================================================
static int XShowFaceBoundary(Draw_Interpretor& di, int argc, const char** argv)
{
  if ((argc != 4 && argc < 7) || argc > 9)
  {
    di << "Usage :\n " << argv[0] << " Doc Label IsOn [R G B [LineWidth [LineStyle]]]\n"
       << "   Doc       - is the document name. \n"
       << "   Label     - is the shape label. \n"
       << "   IsOn      - flag indicating whether the boundaries\n"
       << "                should be turned on or off (can be set\n"
       << "                to 0 (off) or 1 (on)).\n"
       << "   R, G, B   - red, green and blue components of boundary\n"
       << "                color in range (0 - 255).\n"
       << "                (default is (0, 0, 0)\n"
       << "   LineWidth - line width\n"
       << "                (default is 1)\n"
       << "   LineStyle - line fill style :\n"
       << "                 0 - solid  \n"
       << "                 1 - dashed \n"
       << "                 2 - dot    \n"
       << "                 3 - dashdot\n"
       << "                (default is solid)";

    return 1;
  }

  // get specified document
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContext;
  if (!TPrsStd_AISViewer::Find(aDoc->GetData()->Root(), aContext))
  {
    di << "Cannot find viewer for document " << argv[1] << "\n";
    return 1;
  }

  // get shape tool for shape verification
  occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());

  // get label and validate that it is a shape label
  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull() || !aShapes->IsShape(aLabel))
  {
    di << argv[2] << " is not a valid shape label!";
    return 1;
  }

  // get presentation from label
  occ::handle<TPrsStd_AISPresentation> aPrs;
  if (!aLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), aPrs))
  {
    aPrs = TPrsStd_AISPresentation::Set(aLabel, XCAFPrs_Driver::GetID());
  }

  occ::handle<AIS_InteractiveObject> anInteractive = aPrs->GetAIS();
  if (anInteractive.IsNull())
  {
    di << "Can't set drawer attributes.\n"
          "Interactive object for shape label doesn't exists.";
    return 1;
  }

  // get drawer
  const occ::handle<Prs3d_Drawer>& aDrawer = anInteractive->Attributes();

  // default attributes
  double            aRed      = 0.0;
  double            aGreen    = 0.0;
  double            aBlue     = 0.0;
  double            aWidth    = 1.0;
  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;

  // turn boundaries on/off
  bool isBoundaryDraw = (Draw::Atoi(argv[3]) == 1);
  aDrawer->SetFaceBoundaryDraw(isBoundaryDraw);

  // set boundary color
  if (argc >= 7)
  {
    // Text color
    aRed   = Draw::Atof(argv[4]) / 255.;
    aGreen = Draw::Atof(argv[5]) / 255.;
    aBlue  = Draw::Atof(argv[6]) / 255.;
  }

  // set line width
  if (argc >= 8)
  {
    aWidth = (double)Draw::Atof(argv[7]);
  }

  // select appropriate line type
  if (argc == 9)
  {
    if (!ViewerTest::ParseLineType(argv[8], aLineType))
    {
      std::cout << "Syntax error: unknown line type '" << argv[8] << "'\n";
    }
  }

  Quantity_Color aColor(aRed, aGreen, aBlue, Quantity_TOC_RGB);

  occ::handle<Prs3d_LineAspect> aBoundaryAspect = new Prs3d_LineAspect(aColor, aLineType, aWidth);

  aDrawer->SetFaceBoundaryAspect(aBoundaryAspect);

  aContext->Redisplay(anInteractive, true);

  return 0;
}

//=======================================================================
// function : XAssemblyTreeDump
// purpose  : Prints assembly tree structure up to the specified level
//=======================================================================

static int XDumpAssemblyTree(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage :\n " << argv[0] << " Doc [-root label] [-level l] [-names]\n"
       << "   Doc         - document name. \n"
       << "   -root label - starting root label. \n"
       << "   -level l    - depth level (infinite by default). \n"
       << "   -names      - prints names instead of entries. \n";

    return 1;
  }

  // get specified document
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  XCAFDoc_AssemblyItemId aRoot;
  int                    aLevel      = INT_MAX;
  bool                   aPrintNames = false;
  for (int iarg = 2; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-root") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Root is expected!");
      aRoot.Init(argv[++iarg]);
    }
    else if (strcmp(argv[iarg], "-level") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Level is expected!");
      TCollection_AsciiString anArg = argv[++iarg];
      Standard_ProgramError_Raise_if(!anArg.IsIntegerValue(), "Integer value is expected!");
      aLevel = anArg.IntegerValue();
    }
    else if (strcmp(argv[iarg], "-names") == 0)
    {
      aPrintNames = true;
    }
  }

  Standard_SStream aSS;

  XCAFDoc_AssemblyIterator anIt = aRoot.IsNull() ? XCAFDoc_AssemblyIterator(aDoc, aLevel)
                                                 : XCAFDoc_AssemblyIterator(aDoc, aRoot, aLevel);
  XCAFDoc_AssemblyTool::Traverse(anIt, [&](const XCAFDoc_AssemblyItemId& theItem) -> bool {
    if (aPrintNames)
    {
      bool aFirst = true;
      for (NCollection_List<TCollection_AsciiString>::Iterator anIt(theItem.GetPath()); anIt.More();
           anIt.Next(), aFirst = false)
      {
        if (!aFirst)
          aSS << "/";
        TDF_Label aL;
        TDF_Tool::Label(aDoc->GetData(), anIt.Value(), aL, false);
        if (!aL.IsNull())
        {
          TCollection_ExtendedString aName;
          occ::handle<TDataStd_Name> aNameAttr;
          if (aL.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
          {
            aName = aNameAttr->Get();
            aSS << aName;
            continue;
          }
        }
        aSS << anIt.Value();
      }
      aSS << std::endl;
    }
    else
    {
      aSS << theItem.ToString() << std::endl;
    }
    return true;
  });

  di << aSS.str().c_str();
  return 0;
}

//=======================================================================
// function : graphNodeTypename
// purpose  : Returns node type name
//=======================================================================

static const char* graphNodeTypename(const XCAFDoc_AssemblyGraph::NodeType theNodeType)
{
  switch (theNodeType)
  {
    case XCAFDoc_AssemblyGraph::NodeType_AssemblyRoot:
      return "R";
    case XCAFDoc_AssemblyGraph::NodeType_Subassembly:
      return "A";
    case XCAFDoc_AssemblyGraph::NodeType_Occurrence:
      return "O";
    case XCAFDoc_AssemblyGraph::NodeType_Part:
      return "P";
    case XCAFDoc_AssemblyGraph::NodeType_Subshape:
      return "S";
    default:
      return "?";
  }
}

//=======================================================================
// function : XAssemblyGraphDump
// purpose  : Prints assembly graph structure
//=======================================================================

static int XDumpAssemblyGraph(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage :\n " << argv[0] << " Doc [-root label] [-verbose] \n"
       << "   Doc         - is the document name. \n"
       << "   -root label - is the optional starting label. \n"
       << "   -names      - prints names instead of entries. \n";

    return 1;
  }

  // get specified document
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  bool      aPrintNames = false;
  TDF_Label aLabel      = XCAFDoc_DocumentTool::ShapesLabel(aDoc->Main());
  for (int iarg = 2; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-root") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Root is expected!");
      TDF_Tool::Label(aDoc->GetData(), argv[++iarg], aLabel, false);
    }
    else if (strcmp(argv[iarg], "-names") == 0)
    {
      aPrintNames = true;
    }
  }

  occ::handle<XCAFDoc_AssemblyGraph> aG = new XCAFDoc_AssemblyGraph(aLabel);

  Standard_SStream aSS;

  XCAFDoc_AssemblyTool::Traverse(
    aG,
    [](const occ::handle<XCAFDoc_AssemblyGraph>& /*theGraph*/, const int /*theNode*/) -> bool {
      return true;
    },
    [&](const occ::handle<XCAFDoc_AssemblyGraph>& theGraph, const int theNode) -> bool {
      const TDF_Label& aLabel = theGraph->GetNode(theNode);

      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);

      TCollection_AsciiString aNodeEntry;
      if (aPrintNames)
      {
        occ::handle<TDataStd_Name> aNameAttr;
        if (aLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
        {
          aNodeEntry.AssignCat("'");
          aNodeEntry.AssignCat(aNameAttr->Get());
          aNodeEntry.AssignCat("'");
        }
      }
      if (aNodeEntry.IsEmpty())
      {
        TDF_Tool::Entry(aLabel, aNodeEntry);
      }

      aSS << theNode << " " << graphNodeTypename(aNodeType) << " " << aNodeEntry;
      const XCAFDoc_AssemblyGraph::AdjacencyMap& anAdjacencyMap = theGraph->GetLinks();
      const TColStd_PackedMapOfInteger*          aLinksPtr      = anAdjacencyMap.Seek(theNode);
      if (aLinksPtr != nullptr)
      {
        for (TColStd_PackedMapOfInteger::Iterator anIt1(*aLinksPtr); anIt1.More(); anIt1.Next())
        {
          aSS << " " << anIt1.Key();
        }
      }
      aSS << std::endl;

      return true;
    });

  di << aSS.str().c_str();
  return 0;
}

//=======================================================================
// function : XDumpNomenclature
// purpose  : Prints number of assembly instances
//=======================================================================

static int XDumpNomenclature(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage :\n " << argv[0] << " Doc [-names] \n"
       << "   Doc    - is the document name. \n"
       << "   -names - prints names instead of entries. \n";

    return 1;
  }

  // get specified document
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  bool aPrintNames = false;
  for (int iarg = 2; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-names") == 0)
    {
      aPrintNames = true;
    }
  }

  occ::handle<XCAFDoc_AssemblyGraph> aG = new XCAFDoc_AssemblyGraph(aDoc);

  Standard_SStream aSS;

  XCAFDoc_AssemblyTool::Traverse(
    aG,
    [](const occ::handle<XCAFDoc_AssemblyGraph>& theGraph, const int theNode) -> bool {
      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);
      return (aNodeType == XCAFDoc_AssemblyGraph::NodeType_AssemblyRoot)
             || (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Subassembly)
             || (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Part);
    },
    [&](const occ::handle<XCAFDoc_AssemblyGraph>& theGraph, const int theNode) -> bool {
      const TDF_Label& aLabel = theGraph->GetNode(theNode);

      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);

      TCollection_AsciiString aNodeEntry;
      if (aPrintNames)
      {
        occ::handle<TDataStd_Name> aNameAttr;
        if (aLabel.FindAttribute(TDataStd_Name::GetID(), aNameAttr))
        {
          aNodeEntry.AssignCat("'");
          aNodeEntry.AssignCat(aNameAttr->Get());
          aNodeEntry.AssignCat("'");
        }
      }
      if (aNodeEntry.IsEmpty())
      {
        TDF_Tool::Entry(aLabel, aNodeEntry);
      }

      aSS << theNode << " " << graphNodeTypename(aNodeType) << " " << aNodeEntry << " "
          << theGraph->NbOccurrences(theNode) << std::endl;

      return true;
    });

  di << aSS.str().c_str();

  return 0;
}

//=======================================================================
// function : XRescaleGeometry
// purpose  : Applies geometrical scale to all assembly components
//=======================================================================

static int XRescaleGeometry(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Usage :\n " << argv[0] << " Doc factor [-root label] [-force]\n"
       << "   Doc         - is the document name. \n"
       << "   factor      - is the scale factor. \n"
       << "   -root label - is the starting label to apply rescaling. \n"
       << "   -force      - forces rescaling even if the starting label\n"
       << "                 is not a root. \n";

    return 1;
  }

  // get specified document
  occ::handle<TDocStd_Document> aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  // get scale factor
  double aScaleFactor = Draw::Atof(argv[2]);
  if (aScaleFactor <= 0)
  {
    di << "Scale factor must be positive\n";
    return 1;
  }

  bool      aForce = false;
  TDF_Label aLabel = XCAFDoc_DocumentTool::ShapesLabel(aDoc->Main());
  for (int iarg = 3; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-root") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Root is expected!");
      TDF_Tool::Label(aDoc->GetData(), argv[++iarg], aLabel, false);
    }
    else if (strcmp(argv[iarg], "-force") == 0)
    {
      aForce = true;
    }
  }

  if (!XCAFDoc_Editor::RescaleGeometry(aLabel, aScaleFactor, aForce))
  {
    di << "Geometry rescale failed\n";
    return 1;
  }

  return 0;
}

//=======================================================================
// function : testDoc
// purpose  : Method to test destruction of document
//=======================================================================
static int testDoc(Draw_Interpretor&, int argc, const char** argv)
{
  if (argc < 2)
  {
    std::cout << "Invalid numbers of arguments should be: XTestDoc shape" << std::endl;
    return 1;
  }
  TopoDS_Shape shape = DBRep::Get(argv[1]);
  if (shape.IsNull())
    return 1;

  occ::handle<TDocStd_Application> anApp = DDocStd::GetApplication();

  occ::handle<TDocStd_Document> aD1 = new TDocStd_Document("BinXCAF");
  aD1->Open(anApp);

  TCollection_AsciiString aViewName("Driver1/DummyDocument/View1");
  ViewerTest::ViewerInit(aViewName);
  TPrsStd_AISViewer::New(aD1->GetData()->Root(), ViewerTest::GetAISContext());

  // get shape tool for shape verification
  occ::handle<XCAFDoc_ShapeTool> aShapes = XCAFDoc_DocumentTool::ShapeTool(aD1->Main());
  TDF_Label                      aLab    = aShapes->AddShape(shape);

  occ::handle<Geom_Axis2Placement> aPlacement =
    new Geom_Axis2Placement(gp::Origin(), gp::DZ(), gp::DX());
  occ::handle<AIS_Trihedron> aTriShape = new AIS_Trihedron(aPlacement);

  occ::handle<TNaming_NamedShape>      NS;
  occ::handle<TPrsStd_AISPresentation> prs;
  if (aLab.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    prs = TPrsStd_AISPresentation::Set(NS);
  }

  if (aLab.FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
    prs->Display();

  TPrsStd_AISViewer::Update(aLab);
  ViewerTest::GetAISContext()->Display(aTriShape, true);
  aD1->BeforeClose();
  aD1->Close();
  ViewerTest::RemoveView(aViewName);
  return 0;
}

//=================================================================================================

void XDEDRAW::Init(Draw_Interpretor& di)
{
  static bool initactor = false;
  if (initactor)
  {
    return;
  }
  initactor = true;

  // Initialize XCAF formats
  occ::handle<TDocStd_Application> anApp = DDocStd::GetApplication();
  BinXCAFDrivers::DefineFormat(anApp);
  XmlXCAFDrivers::DefineFormat(anApp);

  // Register driver in global table for displaying XDE documents
  // in 3d viewer using OCAF mechanics
  TPrsStd_DriverTable::Get()->AddDriver(XCAFPrs_Driver::GetID(), new XCAFPrs_Driver);

  //=====================================
  // General commands
  //=====================================

  const char* g = "XDE general commands";

  di.Add("XNewDoc", "DocName \t: Create new DECAF document", __FILE__, newDoc, g);

  di.Add("XSave", "[Doc Path] \t: Save Doc or first document in session", __FILE__, saveDoc, g);

  di.Add("XOpen",
         "Path Doc [-skipAttribute] [-readAttribute] [-readPath] [-append|-overwrite]\t: Open XDE "
         "Document with name Doc from Path"
         "\n\t\t The options are:"
         "\n\t\t   -skipAttribute : class name of the attribute to skip during open, for example "
         "-skipTDF_Reference"
         "\n\t\t   -readAttribute : class name of the attribute to read only during open, for "
         "example -readTDataStd_Name loads only such attributes"
         "\n\t\t   -append : to read file into already existing document once again, append new "
         "attributes and don't touch existing"
         "\n\t\t   -overwrite : to read file into already existing document once again, "
         "overwriting existing attributes",
         __FILE__,
         openDoc,
         g);

  di.Add("Xdump",
         "Doc [int deep (0/1)] \t: Print information about tree's structure",
         __FILE__,
         dump,
         g);

  di.Add("XStat", "Doc \t: Print statistics of document", __FILE__, statdoc, g);

  di.Add("XSetPrs",
         "Doc [label1 lavbel2 ...] \t: Set presentation for given label(s) or whole doc",
         __FILE__,
         setPrs,
         g);

  di.Add("XShow",
         "Doc [label1 lavbel2 ...] \t: Display document (or some labels) in a graphical window",
         __FILE__,
         show,
         g);

  di.Add("XDisplay",
         "XDisplay Doc [label1 [label2 [...]]] [-explore {on|off}] [-docPrefix {on|off}] [-names "
         "{on|off}]"
         "\n\t\t:      [-noupdate] [-dispMode Mode] [-highMode Mode] [-autoTriangulation {0|1}]"
         "\n\t\t: Displays document (parts) in 3D Viewer."
         "\n\t\t:  -dispMode    Presentation display mode."
         "\n\t\t:  -highMode    Presentation highlight mode."
         "\n\t\t:  -docPrefix   Prepend document name to object names; TRUE by default."
         "\n\t\t:  -names       Use object names instead of label tag; TRUE by default."
         "\n\t\t:  -explore     Explode labels to leaves; FALSE by default."
         "\n\t\t:  -outDispList Set the TCL variable to the list of displayed object names."
         "\n\t\t:               (instead of printing them to draw interpreter)"
         "\n\t\t:  -autoTriang  Enable/disable auto-triangulation for displayed shapes.",
         __FILE__,
         XDEDRAW_XDisplayTool::XDisplay,
         g);

  di.Add("XWdump",
         "Doc filename.{gif|xwd|bmp} \t: Dump contents of viewer window to XWD, GIF or BMP file",
         __FILE__,
         xwd,
         g);

  di.Add("XAttributeValue",
         "Doc label #attribute: internal command for browser",
         __FILE__,
         XAttributeValue,
         g);

  di.Add("XSetViewNameMode",
         "(1/0) \t: Set/Unset mode of displaying names.",
         __FILE__,
         setviewName,
         g);

  di.Add("XGetViewNameMode",
         "\t: Print if  mode of displaying names is turn on.",
         __FILE__,
         getviewName,
         g);

  di.Add("XSetTransparency",
         "Doc Transparency [label1 label2 ...]\t: Set transparency for given label(s) or whole doc",
         __FILE__,
         XSetTransparency,
         g);

  di.Add("XSetLengthUnit",
         "Doc {unit_name|scale_factor}\t: Set value of length unit"
         "\n\t\t: Possible unit_name: m, mm, km, cm, micron, mille, in(inch), min(microinch), "
         "nin(nano inch), ft, stat.mile"
         "\n\t\t: Possible scale factor: any real value more then 0. Factor to meter.",
         __FILE__,
         setLengthUnit,
         g);

  di.Add("XGetLengthUnit",
         "Doc [-scale]\t: Print name of length unit"
         "\n\t\t: -scale : print value of the scaling factor to meter of length unit",
         __FILE__,
         dumpLengthUnit,
         g);

  di.Add("XShowFaceBoundary",
         "Doc Label IsOn [R G B [LineWidth [LineStyle]]]:"
         "- turns on/off drawing of face boundaries and defines boundary line style",
         __FILE__,
         XShowFaceBoundary,
         g);
  di.Add("XTestDoc", "XTestDoc shape", __FILE__, testDoc, g);

  di.Add("XDumpAssemblyTree",
         "Doc [-root label] [-level l] [-names]: Iterates through the assembly tree in depth up to "
         "the specified level, if any",
         __FILE__,
         XDumpAssemblyTree,
         g);
  di.Add("XDumpAssemblyGraph",
         "Doc [-root label] [-names]: Prints assembly graph structure",
         __FILE__,
         XDumpAssemblyGraph,
         g);
  di.Add("XDumpNomenclature",
         "Doc [-names]: Prints number of assembly instances",
         __FILE__,
         XDumpNomenclature,
         g);
  di.Add("XRescaleGeometry",
         "Doc factor [-root label] [-force]: Applies geometrical scale to assembly",
         __FILE__,
         XRescaleGeometry,
         g);

  // Specialized commands
  XDEDRAW_Shapes::InitCommands(di);
  XDEDRAW_Colors::InitCommands(di);
  XDEDRAW_Layers::InitCommands(di);
  XDEDRAW_Props::InitCommands(di);
  XDEDRAW_GDTs::InitCommands(di);
  XDEDRAW_Views::InitCommands(di);
  XDEDRAW_Notes::InitCommands(di);
  XDEDRAW_Common::InitCommands(di); // moved from EXE
}

//==============================================================================
// XDEDRAW::Factory
//==============================================================================
void XDEDRAW::Factory(Draw_Interpretor& theDI)
{
  XDEDRAW::Init(theDI);

#ifdef OCCT_DEBUG
  theDI << "Draw Plugin : All TKXDEDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XDEDRAW)
