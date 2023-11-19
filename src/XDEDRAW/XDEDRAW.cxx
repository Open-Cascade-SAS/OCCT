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
#include <DE_Wrapper.hxx>
#include <DEBRepCascade_ConfigurationNode.hxx>
#include <DEXCAFCascade_ConfigurationNode.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Geom_Axis2Placement.hxx>
#include <IGESCAFControl_ConfigurationNode.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <RWStl_ConfigurationNode.hxx>
#include <RWGltf_ConfigurationNode.hxx>
#include <RWObj_ConfigurationNode.hxx>
#include <RWPly_ConfigurationNode.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_ConfigurationNode.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
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
#include <TDF_LabelSequence.hxx>
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
#include <Vrml_ConfigurationNode.hxx>
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
#include <XSDRAWIGES.hxx>
#include <XSDRAWSTEP.hxx>
#include <UnitsMethods.hxx>

#include <BinXCAFDrivers.hxx>
#include <XmlXCAFDrivers.hxx>

#include <stdio.h>

//=======================================================================
// Section: General commands
//=======================================================================

//=======================================================================
//function : newDoc
//purpose  :
//=======================================================================
static Standard_Integer newDoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {di<<"Give document name\n";return 1;}

  Handle(TDocStd_Document) D;
  Handle(DDocStd_DrawDocument) DD;
  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  if (!DDocStd::GetDocument(argv[1],D,Standard_False)) {
    A->NewDocument(  "BinXCAF"  ,D);
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(),argv[1]);
    Draw::Set(argv[1],DD);
    di << "document " << argv[1] << " created\n";
    //DDocStd::ReturnLabel(di,D->Main());
  }
  else di << argv[1] << " is already a document\n";

  return 0;
}


//=======================================================================
//function : saveDoc
//purpose  :
//=======================================================================
static Standard_Integer saveDoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(TDocStd_Document) D;
  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  if (argc == 1) {
    if (A->NbDocuments() < 1) return 1;
    A->GetDocument(1, D);
  }
  else {
    if (!DDocStd::GetDocument(argv[1],D)) return 1;
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di);

  PCDM_StoreStatus aStatus = PCDM_SS_Doc_IsNull;
  if (argc == 3)
  {
    TCollection_ExtendedString path (argv[2]);
    aStatus = A->SaveAs (D, path, aProgress->Start());
  }
  else if (!D->IsSaved())
  {
    std::cout << "Storage error: this document has never been saved\n";
    return 1;
  }
  else
  {
    aStatus = A->Save (D, aProgress->Start());
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

//=======================================================================
//function : openDoc
//purpose  :
//=======================================================================
static Standard_Integer openDoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(TDocStd_Document) D;
  Handle(DDocStd_DrawDocument) DD;
  Handle(TDocStd_Application) A = DDocStd::GetApplication();

  if ( argc < 3 )
  {
    di << "invalid number of arguments. Usage:\t XOpen filename docname [-skipAttribute] [-readAttribute] [-readPath] [-append|-overwrite]\n";
    return 1;
  }

  TCollection_AsciiString Filename = argv[1];
  Standard_CString DocName = argv[2];

  Handle(PCDM_ReaderFilter) aFilter = new PCDM_ReaderFilter;
  for (Standard_Integer i = 3; i < argc; i++)
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

  if (aFilter->IsAppendMode() && !DDocStd::GetDocument (DocName, D, Standard_False))
  {
    di << "for append mode document " << DocName << " must be already created\n";
    return 1;
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator (di);
  if ( A->Open (Filename, D, aFilter, aProgress->Start()) != PCDM_RS_OK )
  {
    di << "cannot open XDE document\n";
    return 1;
  }

  if (!aFilter->IsAppendMode())
  {
    DD = new DDocStd_DrawDocument (D);
    TDataStd_Name::Set (D->GetData()->Root(), DocName);
    Draw::Set (DocName, DD);
  }

  di << "document " << DocName << " opened\n";

  return 0;
}

//=======================================================================
//function : dump
//purpose  :
//=======================================================================
static Standard_Integer dump (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<2) {
    di<<"Use: "<<argv[0]<<" Doc [int deep (0/1)]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean deep = Standard_False;
  if ( (argc==3) && (Draw::Atoi(argv[2])==1) ) deep = Standard_True;
  Standard_SStream aDumpLog;
  myAssembly->Dump(aDumpLog, deep);
  di<<aDumpLog;
  return 0;
}


//=======================================================================
//function : StatAssembly
//purpose  : recursive part of statistics
//=======================================================================

static void StatAssembly(const TDF_Label L,
			 const Standard_Integer level,
                         Handle(TColStd_HArray1OfInteger) &HAI,
                         Standard_Integer &NbCentroidProp,
                         Standard_Integer &NbVolumeProp,
                         Standard_Integer &NbAreaProp,
                         Standard_Integer &NbShapesWithName,
                         Standard_Integer &NbShapesWithColor,
                         Standard_Integer &NbShapesWithLayer,
                         Handle(TDocStd_Document) &aDoc,
                         Standard_Boolean &PrintStructMode,
			 Draw_Interpretor& di)
{
  if(PrintStructMode) {
    for(Standard_Integer j=0; j<=level; j++)
      di<<"  ";
  }
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(L, Entry);
  if(PrintStructMode) di<<Entry.ToCString();

  Handle(TDataStd_Name) Name;
  if(L.FindAttribute(TDataStd_Name::GetID(), Name)) {
    NbShapesWithName++;
    if(PrintStructMode) {
      di << " " << Name->Get() << "  has attributes: ";
    }
  }
  else {
    if(PrintStructMode) di<<" NoName  has attributes: ";
  }

  Handle(XCAFDoc_Centroid) aCentroid = new (XCAFDoc_Centroid);
  if(L.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid)) {
    if(PrintStructMode) di<<"Centroid ";
    NbCentroidProp++;
  }
  Standard_Real tmp;
  if(XCAFDoc_Volume::Get(L,tmp)) {
    if(PrintStructMode) di<<"Volume("<<tmp<<") ";
    NbVolumeProp++;
  }
  if(XCAFDoc_Area::Get(L,tmp)) {
    if(PrintStructMode) di<<"Area("<<tmp<<") ";
    NbAreaProp++;
  }
  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool(aDoc->Main());
  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool(aDoc->Main());
  Quantity_ColorRGBA col;
  Standard_Boolean IsColor = Standard_False;
  Standard_Boolean IsByLayer = Standard_False;
  if(CTool->GetColor(L,XCAFDoc_ColorGen,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorSurf,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorCurv,col))
    IsColor = Standard_True;
  else if(CTool->IsColorByLayer(L))
    IsByLayer = Standard_True;
  if(IsColor || IsByLayer) {
    if(IsByLayer)
    {
      Handle(TColStd_HSequenceOfExtendedString) aLayerS;
      LTool->GetLayers(L, aLayerS);
      // Currently for DXF pnly, thus
      // only 1 Layer should be.
      if(aLayerS->Length() == 1)
      {
        TDF_Label aLayer = LTool->FindLayer (aLayerS->First());
        Quantity_ColorRGBA aColor;
        if (CTool->GetColor (aLayer, XCAFDoc_ColorGen, aColor))
        {
          TCollection_AsciiString aColorName = aColor.GetRGB().StringName(aColor.GetRGB().Name());
          di<<"Color(" << aColorName.ToCString() << ") ";
        }
        else
        {
          di<<"Color(ByLayer) ";
        }
      }
      NbShapesWithColor++;
    }
    else
    {
      TCollection_AsciiString Entry1;
      Entry1 = col.GetRGB().StringName(col.GetRGB().Name());
      if(PrintStructMode) di<<"Color("<<Entry1.ToCString()<<" "<<col.Alpha()<<") ";
      NbShapesWithColor++;
    }
  }
  Handle(TColStd_HSequenceOfExtendedString) aLayerS;
  LTool->GetLayers(L, aLayerS);
  if(!aLayerS.IsNull() && aLayerS->Length()>0) {
    if(PrintStructMode) {
      di<<"Layer(";
      for(Standard_Integer i=1; i<=aLayerS->Length(); i++) {
        TCollection_AsciiString Entry2(aLayerS->Value(i));
        if(i==1)
          di<<"\""<<Entry2.ToCString()<<"\"";
        else
          di<<" \""<<Entry2.ToCString()<<"\"";
      }
      di<<") ";
    }
    NbShapesWithLayer++;
  }
  if(PrintStructMode) di<<"\n";
  
  HAI->SetValue(level, HAI->Value(level)+1 );
  if(L.HasChild()) {
    for(Standard_Integer i=1; i<=L.NbChildren(); i++) {
      StatAssembly(L.FindChild(i), level+1, HAI, NbCentroidProp, NbVolumeProp,
                   NbAreaProp, NbShapesWithName, NbShapesWithColor,
                   NbShapesWithLayer, aDoc, PrintStructMode, di);
    }
  }

}


//=======================================================================
//function : statdoc
//purpose  : 
//=======================================================================
static Standard_Integer statdoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<2) {
    di<<"Use: "<<argv[0]<<" Doc \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  Standard_Boolean PrintStructMode = (argc==3);
  Handle(XCAFDoc_ShapeTool) aTool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_LabelSequence SeqLabels;
  aTool->GetShapes(SeqLabels);
  if(SeqLabels.Length()<=0) return 0;
  if(PrintStructMode) di<<"\nStructure of shapes in the document:\n";
  Standard_Integer level=0;
  Standard_Integer NbCentroidProp=0, NbVolumeProp=0, NbAreaProp=0;
  Standard_Integer NbShapesWithName=0, NbShapesWithColor=0, NbShapesWithLayer=0;
  Handle(TColStd_HArray1OfInteger) HAI = new TColStd_HArray1OfInteger(0,20);
  Standard_Integer i=0;
  for(i=0; i<=20; i++) HAI->SetValue(i,0);
  for(i=1; i<=SeqLabels.Length(); i++) {
    StatAssembly(SeqLabels.Value(i), level, HAI, NbCentroidProp, NbVolumeProp,
                 NbAreaProp, NbShapesWithName, NbShapesWithColor,
                 NbShapesWithLayer, Doc, PrintStructMode, di);
  }
  Standard_Integer NbLabelsShape = 0;
  di<<"\nStatistis of shapes in the document:\n";
  for(i=0; i<=20; i++) {
    if(HAI->Value(i)==0) break;
    //di<<"level N "<<i<<" :  number of labels with shape = "<<HAI->Value(i)<<"\n";
    di<<"level N "<<i<<" : "<<HAI->Value(i)<<"\n";
    NbLabelsShape = NbLabelsShape + HAI->Value(i);
  }
  di<<"Total number of labels for shapes in the document = "<<NbLabelsShape<<"\n";
  di<<"Number of labels with name = "<<NbShapesWithName<<"\n";
  di<<"Number of labels with color link = "<<NbShapesWithColor<<"\n";
  di<<"Number of labels with layer link = "<<NbShapesWithLayer<<"\n";

  di<<"\nStatistis of Props in the document:\n";
  di<<"Number of Centroid Props = "<<NbCentroidProp<<"\n";
  di<<"Number of Volume Props = "<<NbVolumeProp<<"\n";
  di<<"Number of Area Props = "<<NbAreaProp<<"\n";

  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  TDF_LabelSequence CLabels;
  CTool->GetColors(CLabels);
  di<<"\nNumber of colors = "<<CLabels.Length()<<"\n";
  if(CLabels.Length()>0) {
    for(i=1; i<=CLabels.Length(); i++) {
      TDF_Label aLabel = CLabels.Value(i);
      Quantity_Color col;
      CTool->GetColor(aLabel, col);
      di<<col.StringName(col.Name())<<" ";
    }
    di<<"\n";
  }

  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool(Doc->Main());
  TDF_LabelSequence LLabels;
  LTool->GetLayerLabels(LLabels);
  di<<"\nNumber of layers = "<<LLabels.Length()<<"\n";
  if(LLabels.Length()>0) {
    for(i=1; i<=LLabels.Length(); i++) {
      TDF_Label aLabel = LLabels.Value(i);
      TCollection_ExtendedString layerName;
      LTool->GetLayer(aLabel, layerName);
      di << "\"" << layerName << "\" ";
    }
    di<<"\n";
  }

  di<<"\n";
  return 0;
}


//=======================================================================
//function : setPrs
//purpose  :
//=======================================================================
static Standard_Integer setPrs (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <2) {
    di<<"Use: "<<argv[0]<<" DocName [label1 label2 ...] \n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  // collect sequence of labels to set presentation
  Handle(XCAFDoc_ShapeTool) shapes = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_LabelSequence seq;
  if ( argc >2 ) {
    for ( Standard_Integer i=2; i < argc; i++ ) {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
      if ( aLabel.IsNull() || ! shapes->IsShape ( aLabel ) ) {
	di << argv[i] << " is not a valid shape label!";
	continue;
      }
      seq.Append ( aLabel );
    }
  }
  else {
    shapes->GetShapes ( seq );
  }

  // set presentations
  Handle(XCAFDoc_ColorTool) colors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  for ( Standard_Integer i=1; i <= seq.Length(); i++ ) {
    Handle(TPrsStd_AISPresentation) prs;
    if ( ! seq.Value(i).FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) ) {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i),XCAFPrs_Driver::GetID());
      prs->SetMaterial (Graphic3d_NameOfMaterial_Plastified);
    }
//    Quantity_Color Col;
//    if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorSurf, Col ) )
//      prs->SetColor ( Col.Name() );
//    else if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorCurv, Col ) )
//      prs->SetColor ( Col.Name() );
  }
  return 0;
}


//=======================================================================
//function : show
//purpose  :
//=======================================================================
static Standard_Integer show (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <2) {
    di<<"Use: "<<argv[0]<<" DocName [label1 label2 ...] \n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << argv[1] << " is not a document\n";
    return 1;
  }

  // init viewer
  TDF_Label aRoot = aDoc->GetData()->Root();
  Handle(TPrsStd_AISViewer) aDocViewer;
  TCollection_AsciiString   aViewName = TCollection_AsciiString ("Driver1/Document_") + argv[1] + "/View1";
  if (!TPrsStd_AISViewer::Find (aRoot, aDocViewer))
  {
    ViewerTest::ViewerInit (aViewName);
    aDocViewer = TPrsStd_AISViewer::New (aRoot, ViewerTest::GetAISContext());
  }

  // collect sequence of labels to display
  Handle(XCAFDoc_ShapeTool) shapes = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main());
  TDF_LabelSequence seq;
  if ( argc >2 ) {
    for ( Standard_Integer i=2; i < argc; i++ ) {
      TDF_Label aLabel;
      TDF_Tool::Label (aDoc->GetData(), argv[i], aLabel);
      if ( aLabel.IsNull() || ! shapes->IsShape ( aLabel ) ) {
	di << argv[i] << " is not a valid shape label!";
	continue;
      }
      seq.Append ( aLabel );
    }
  }
  else {
    shapes->GetFreeShapes ( seq );
  }

  // set presentations and show
  //Handle(XCAFDoc_ColorTool) colors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  for ( Standard_Integer i=1; i <= seq.Length(); i++ ) {
    Handle(TPrsStd_AISPresentation) prs;
    if ( ! seq.Value(i).FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) ) {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i),XCAFPrs_Driver::GetID());
      prs->SetMaterial (Graphic3d_NameOfMaterial_Plastified);
    }
//    Quantity_Color Col;
//    if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorSurf, Col ) )
//      prs->SetColor ( Col.Name() );
//    else if ( colors.GetColor ( seq.Value(i), XCAFDoc_ColorCurv, Col ) )
//      prs->SetColor ( Col.Name() );
    prs->Display(Standard_True);
  }
  TPrsStd_AISViewer::Update (aDoc->GetData()->Root());
  return 0;
}

//! XDisplay command implementation.
class XDEDRAW_XDisplayTool
{
public:
  //! XDisplay command interface.
  static Standard_Integer XDisplay (Draw_Interpretor& theDI,
                                    Standard_Integer theNbArgs,
                                    const char** theArgVec)
  {
    XDEDRAW_XDisplayTool aTool;
    return aTool.xdisplay (theDI, theNbArgs, theArgVec);
  }

private:
  //! Constructor.
  XDEDRAW_XDisplayTool()
  : myDispMode(-2),
    myHiMode  (-2),
    myIsAutoTriang (-1),
    myToPrefixDocName (Standard_True),
    myToGetNames (Standard_True),
    myToExplore  (Standard_False) {}

  //! Display single label.
  Standard_Integer displayLabel (Draw_Interpretor& theDI,
                                 const TDF_Label& theLabel,
                                 const TCollection_AsciiString& theNamePrefix,
                                 const TopLoc_Location& theLoc,
                                 TCollection_AsciiString& theOutDispList)
  {
    TCollection_AsciiString aName;
    if (myToGetNames)
    {
      Handle(TDataStd_Name) aNodeName;
      if (theLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
      {
        aName = aNodeName->Get();
      }
      if (aName.IsEmpty())
      {
        TDF_Label aRefLabel;
        if (XCAFDoc_ShapeTool::GetReferredShape (theLabel, aRefLabel)
         && aRefLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
        {
          aName = aNodeName->Get();
        }
      }

      if (aName.IsEmpty())
      {
        TDF_Tool::Entry (theLabel, aName);
      }
      for (Standard_Integer aNameIndex = 1;; ++aNameIndex)
      {
        if (myNameMap.Add (aName))
        {
          break;
        }
        aName = aNodeName->Get() + "_" + aNameIndex;
      }
    }
    else
    {
      TDF_Tool::Entry (theLabel, aName);
    }
    aName = theNamePrefix + aName;

    if (myToExplore)
    {
      TDF_Label aRefLabel = theLabel;
      XCAFDoc_ShapeTool::GetReferredShape (theLabel, aRefLabel);
      if (XCAFDoc_ShapeTool::IsAssembly (aRefLabel))
      {
        aName += "/";
        const TopLoc_Location aLoc = theLoc * XCAFDoc_ShapeTool::GetLocation (theLabel);
        for (TDF_ChildIterator aChildIter (aRefLabel); aChildIter.More(); aChildIter.Next())
        {
          if (displayLabel (theDI, aChildIter.Value(), aName, aLoc, theOutDispList) == 1)
          {
            return 1;
          }
        }
        return 0;
      }
    }

    Handle(XCAFPrs_AISObject) aPrs = new XCAFPrs_AISObject (theLabel);
    if (!theLoc.IsIdentity())
    {
      aPrs->SetLocalTransformation (theLoc);
    }
    if (myDispMode != -2)
    {
      if (myDispMode == -1)
      {
        aPrs->UnsetDisplayMode();
      }
      if (!aPrs->AcceptDisplayMode (myDispMode))
      {
        theDI << "Syntax error: " << aPrs->DynamicType()->Name() << " rejects " << myDispMode << " display mode";
        return 1;
      }
      else
      {
        aPrs->SetDisplayMode (myDispMode);
      }
    }
    if (myHiMode != -2)
    {
      if (myHiMode != -1
      && !aPrs->AcceptDisplayMode (myHiMode))
      {
        theDI << "Syntax error: " << aPrs->DynamicType()->Name() << " rejects " << myHiMode << " display mode";
        return 1;
      }
      aPrs->SetHilightMode (myHiMode);
    }
    if (myIsAutoTriang != -1)
    {
      aPrs->Attributes()->SetAutoTriangulation (myIsAutoTriang == 1);
    }

    ViewerTest::Display (aName, aPrs, false);
    theOutDispList += aName + " ";
    return 0;
  }

  //! XDisplay command implementation.
  Standard_Integer xdisplay (Draw_Interpretor& theDI,
                             Standard_Integer theNbArgs,
                             const char** theArgVec)
  {
    Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
    if (aContext.IsNull())
    {
      theDI << "Error: no active viewer";
      return 1;
    }

    ViewerTest_AutoUpdater anAutoUpdater (aContext, ViewerTest::CurrentView());
    for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
    {
      TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
      anArgCase.LowerCase();
      if (anAutoUpdater.parseRedrawMode (anArgCase))
      {
        continue;
      }
      else if (anArgIter + 1 < theNbArgs
            && myDispMode == -2
            && (anArgCase == "-dispmode"
             || anArgCase == "-displaymode")
            && TCollection_AsciiString (theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        myDispMode = TCollection_AsciiString (theArgVec[++anArgIter]).IntegerValue();
      }
      else if (anArgIter + 1 < theNbArgs
            && myHiMode == -2
            && (anArgCase == "-himode"
             || anArgCase == "-highmode"
             || anArgCase == "-highlightmode")
            && TCollection_AsciiString (theArgVec[anArgIter + 1]).IsIntegerValue())
      {
        myHiMode = TCollection_AsciiString (theArgVec[++anArgIter]).IntegerValue();
      }
      else if (anArgCase == "-autotr"
            || anArgCase == "-autotrian"
            || anArgCase == "-autotriang"
            || anArgCase == "-autotriangulation"
            || anArgCase == "-noautotr"
            || anArgCase == "-noautotrian"
            || anArgCase == "-noautotriang"
            || anArgCase == "-noautotriangulation")
      {
        myIsAutoTriang = Draw::ParseOnOffNoIterator (theNbArgs, theArgVec, anArgIter) ? 1 : 0;
      }
      else if (anArgCase == "-docprefix"
            || anArgCase == "-nodocprefix")
      {
        myToPrefixDocName = Standard_True;
        if (anArgIter + 1 < theNbArgs
         && Draw::ParseOnOff (theArgVec[anArgIter + 1], myToPrefixDocName))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith ("-no"))
        {
          myToPrefixDocName = !myToPrefixDocName;
        }
      }
      else if (anArgCase == "-names"
            || anArgCase == "-nonames")
      {
        myToGetNames = Standard_True;
        if (anArgIter + 1 < theNbArgs
         && Draw::ParseOnOff (theArgVec[anArgIter + 1], myToGetNames))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith ("-no"))
        {
          myToGetNames = !myToGetNames;
        }
      }
      else if (anArgCase == "-explore"
            || anArgCase == "-noexplore")
      {
        myToExplore = Standard_True;
        if (anArgIter + 1 < theNbArgs
         && Draw::ParseOnOff (theArgVec[anArgIter + 1], myToExplore))
        {
          ++anArgIter;
        }
        if (anArgCase.StartsWith ("-no"))
        {
          myToExplore = !myToExplore;
        }
      }
      else if (anArgCase == "-outdisplist"
            && anArgIter + 1 < theNbArgs)
      {
        myOutDispListVar = theArgVec[++anArgIter];
        myOutDispList.Clear();
      }
      else
      {
        if (myDoc.IsNull()
         && DDocStd::GetDocument (theArgVec[anArgIter], myDoc, Standard_False))
        {
          myDocName = theArgVec[anArgIter];
          continue;
        }

        TCollection_AsciiString aValue (theArgVec[anArgIter]);
        const Standard_Integer aFirstSplit = aValue.Search (":");
        if (!IsDigit (aValue.Value (1))
          && aFirstSplit >= 2
          && aFirstSplit < aValue.Length())
        {
          TCollection_AsciiString aDocName = aValue.SubString (1, aFirstSplit - 1);
          Standard_CString aDocNameStr = aDocName.ToCString();
          Handle(TDocStd_Document) aDoc;
          if (DDocStd::GetDocument (aDocNameStr, aDoc, Standard_False))
          {
            aValue = aValue.SubString (aFirstSplit + 1, aValue.Length());
            if (!myDoc.IsNull()
              && myDoc != aDoc)
            {
              theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
              return 1;
            }
            myDoc = aDoc;
            myDocName = aDocName;
          }
        }
        if (myDoc.IsNull())
        {
          theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        TDF_Label aLabel;
        TDF_Tool::Label (myDoc->GetData(), aValue.ToCString(), aLabel);
        if (aLabel.IsNull()
        || !XCAFDoc_ShapeTool::IsShape (aLabel))
        {
          theDI << "Syntax error: " << aValue << " is not a valid shape label";
          return 1;
        }
        myLabels.Append (aLabel);
      }
    }
    if (myDoc.IsNull())
    {
      theDI << "Syntax error: not enough arguments";
      return 1;
    }
    if (myLabels.IsEmpty())
    {
      XCAFDoc_DocumentTool::ShapeTool (myDoc->Main())->GetFreeShapes (myLabels);
    }

    for (TDF_LabelSequence::Iterator aLabIter (myLabels); aLabIter.More(); aLabIter.Next())
    {
      const TDF_Label& aLabel = aLabIter.Value();
      if (displayLabel (theDI, aLabel, myToPrefixDocName ? myDocName + ":" : "", TopLoc_Location(), myOutDispList) == 1)
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
      Draw::Set (myOutDispListVar.ToCString(), myOutDispList.ToCString());
    }
    return 0;
  }

private:
  NCollection_Map<TCollection_AsciiString>
                           myNameMap;         //!< names map to handle collisions
  Handle(TDocStd_Document) myDoc;             //!< document
  TCollection_AsciiString  myDocName;         //!< document name
  TCollection_AsciiString  myOutDispListVar;  //!< tcl variable to print the result objects
  TCollection_AsciiString  myOutDispList;     //!< string with list of all displayed object names
  TDF_LabelSequence        myLabels;          //!< labels to display
  Standard_Integer         myDispMode;        //!< shape display mode
  Standard_Integer         myHiMode;          //!< shape highlight mode
  Standard_Integer         myIsAutoTriang;    //!< auto-triangulation mode
  Standard_Boolean         myToPrefixDocName; //!< flag to prefix objects with document name
  Standard_Boolean         myToGetNames;      //!< flag to use label names or tags
  Standard_Boolean         myToExplore;       //!< flag to explore assembles
};

//=======================================================================
//function : xwd
//purpose  :
//=======================================================================
static Standard_Integer xwd (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <3) {
    di<<"Use: "<<argv[0]<<" DocName filename.{xwd|gif|bmp}\n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  Handle(AIS_InteractiveContext) IC;
  if ( ! TPrsStd_AISViewer::Find ( Doc->GetData()->Root(), IC ) ) {
    di << "Cannot find viewer for document " << argv[1] << "\n";
    return 1;
  }

  Handle(V3d_Viewer) aViewer = IC->CurrentViewer();
  V3d_ListOfViewIterator aViewIter = aViewer->ActiveViewIterator();
  if (aViewIter.More())
  {
    aViewIter.Value()->Dump ( argv[2] );
  }
  else {
    di << "Cannot find an active view in a viewer " << argv[1] << "\n";
    return 1;
  }

  return 0;
}


//=======================================================================
//function : XAttributeValue
//purpose  :
//=======================================================================
static Standard_Integer XAttributeValue (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc < 4 ) { std::cout << "Syntax error: Too few args\n"; return 1; }
  Handle(DDF_Browser) browser = Handle(DDF_Browser)::DownCast (Draw::GetExisting (argv[1]));
  if ( browser.IsNull() ) { std::cout << "Syntax error: Not a browser: " << argv[1] << "\n"; return 1; }

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),argv[2],lab);
  if ( lab.IsNull() ) { std::cout << "Syntax error: label is Null: " << argv[2] << "\n"; return 1; }

  Standard_Integer num = Draw::Atoi ( argv[3] );
  TDF_AttributeIterator itr(lab,Standard_False);
  for (Standard_Integer i=1; itr.More() && i < num; i++) itr.Next();

  if ( ! itr.More() ) { std::cout << "Syntax error: Attribute #" << num << " not found\n"; return 1; }

  TCollection_AsciiString anInfo = XCAFDoc::AttributeInfo ( itr.Value() );
  if (!anInfo.IsEmpty())
  {
    di << anInfo.ToCString();
  }
  return 0;
}


//=======================================================================
//function : setviewName
//purpose  :
//=======================================================================
static Standard_Integer setviewName (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <2) {
    di<<"Use: "<<argv[0]<<" (1/0)\n";
    return 1;
  }
  Standard_Boolean mode = Standard_False;
  if (Draw::Atoi(argv[1]) == 1) mode = Standard_True;
  XCAFPrs::SetViewNameMode(mode);
  return 0;
}


//=======================================================================
//function : getviewName
//purpose  : auxiliary
//=======================================================================

static Standard_Integer getviewName (Draw_Interpretor&  di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  if ( XCAFPrs::GetViewNameMode() ) di << "Display names ON\n";
  else di << "Display names OFF\n";
  return 0;
}


//=======================================================================
//function : XSetTransparency
//purpose  :
//=======================================================================
static Standard_Integer XSetTransparency (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: "<<argv[0]<<" Doc Transparency [label1 label2 ...] \n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  const Standard_Real aTransparency = Draw::Atof(argv[2]);

  // collect sequence of labels
  Handle(XCAFDoc_ShapeTool) shapes = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_LabelSequence seq;
  if ( argc > 3 ) {
    for ( Standard_Integer i=3; i < argc; i++ ) {
      TDF_Label aLabel;
      TDF_Tool::Label(Doc->GetData(), argv[i], aLabel);
      if ( aLabel.IsNull() || ! shapes->IsShape ( aLabel ) ) {
        di << argv[i] << " is not a valid shape label!";
        continue;
      }
      seq.Append ( aLabel );
    }
  }
  else {
    shapes->GetFreeShapes ( seq );
  }

  // find presentations and set transparency
  for ( Standard_Integer i=1; i <= seq.Length(); i++ ) {
    Handle(TPrsStd_AISPresentation) prs;
    if ( ! seq.Value(i).FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) ) {
      prs = TPrsStd_AISPresentation::Set(seq.Value(i),XCAFPrs_Driver::GetID());
      prs->SetMaterial (Graphic3d_NameOfMaterial_Plastified);
    }
    prs->SetTransparency( aTransparency );
  }
  TPrsStd_AISViewer::Update(Doc->GetData()->Root());
  return 0;
}

//=======================================================================
//function : setLengthUnit
//purpose  :
//=======================================================================
static Standard_Integer setLengthUnit(Draw_Interpretor& di,
                                      Standard_Integer argc,
                                      const char** argv)
{
  if (argc != 3)
  {
    di << "Use: " << argv[0] << " Doc {unitName|scaleFactor} \n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << "Error: " << argv[1] << " is not a document\n"; return 1;
  }

  TCollection_AsciiString anUnit(argv[2]);
  Standard_Real anUnitValue = 1.;
  TCollection_AsciiString anUnitName;
  if (!anUnit.IsRealValue(true))
  {
    UnitsMethods_LengthUnit aTypeUnit = UnitsMethods::LengthUnitFromString(anUnit.ToCString(), Standard_False);
    if (aTypeUnit == UnitsMethods_LengthUnit_Undefined)
    {
      di << "Error: " << anUnit
         << " is incorrect unit, use m, mm, km, cm, micron, mille, in, min, nin, ft, stat.mile\n";
      return 1;
    }
    anUnitName = anUnit;
    anUnitValue = UnitsMethods::GetLengthFactorValue(aTypeUnit) * 0.001;
  }
  else
  {
    anUnitValue = anUnit.RealValue();
    anUnitName = UnitsMethods::DumpLengthUnit(anUnitValue, UnitsMethods_LengthUnit_Meter);
  }
  XCAFDoc_LengthUnit::Set(aDoc->Main().Root(), anUnitName, anUnitValue);
  return 0;
}

//=======================================================================
//function : dumpLengthUnit
//purpose  :
//=======================================================================
static Standard_Integer dumpLengthUnit(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2 && argc != 3) {
    di << "Use: " << argv[0] << " Doc [-scale]\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) 
  { 
    di << "Error: " << argv[1] << " is not a document\n"; return 1;
  }
  Handle(XCAFDoc_LengthUnit) aUnits;
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
//function : XShowFaceBoundary
//purpose  : Set face boundaries on/off
//=======================================================================
static Standard_Integer XShowFaceBoundary (Draw_Interpretor& di,
                                           Standard_Integer argc,
                                           const char ** argv)
{
  if (( argc != 4 && argc < 7 ) || argc > 9)
  {
    di << "Usage :\n " << argv[0]
       << " Doc Label IsOn [R G B [LineWidth [LineStyle]]]\n"
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
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n"; 
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext;
  if (!TPrsStd_AISViewer::Find (aDoc->GetData()->Root(), aContext)) 
  {
    di << "Cannot find viewer for document " << argv[1] << "\n";
    return 1;
  }

  // get shape tool for shape verification
  Handle(XCAFDoc_ShapeTool) aShapes =
    XCAFDoc_DocumentTool::ShapeTool (aDoc->Main());

  // get label and validate that it is a shape label
  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull() || !aShapes->IsShape (aLabel))
  {
    di << argv[2] << " is not a valid shape label!";
    return 1;
  }

  // get presentation from label
  Handle(TPrsStd_AISPresentation) aPrs;
  if (!aLabel.FindAttribute (TPrsStd_AISPresentation::GetID (), aPrs))
  {
    aPrs = TPrsStd_AISPresentation::Set (aLabel,XCAFPrs_Driver::GetID ());
  }

  Handle(AIS_InteractiveObject) anInteractive = aPrs->GetAIS ();
  if (anInteractive.IsNull ())
  {
    di << "Can't set drawer attributes.\n"
          "Interactive object for shape label doesn't exists.";
    return 1;
  }

  // get drawer
  const Handle(Prs3d_Drawer)& aDrawer = anInteractive->Attributes ();

  // default attributes
  Standard_Real aRed   = 0.0;
  Standard_Real aGreen = 0.0;
  Standard_Real aBlue  = 0.0;
  Standard_Real aWidth = 1.0;
  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;
  
  // turn boundaries on/off
  Standard_Boolean isBoundaryDraw = (Draw::Atoi (argv[3]) == 1);
  aDrawer->SetFaceBoundaryDraw (isBoundaryDraw);
  
  // set boundary color
  if (argc >= 7)
  {
    // Text color
    aRed   = Draw::Atof (argv[4])/255.;
    aGreen = Draw::Atof (argv[5])/255.;
    aBlue  = Draw::Atof (argv[6])/255.;
  }

  // set line width
  if (argc >= 8)
  {
    aWidth = (Standard_Real)Draw::Atof (argv[7]);
  }

  // select appropriate line type
  if (argc == 9)
  {
    if (!ViewerTest::ParseLineType (argv[8], aLineType))
    {
      std::cout << "Syntax error: unknown line type '" << argv[8] << "'\n";
    }
  }

  Quantity_Color aColor (aRed, aGreen, aBlue, Quantity_TOC_RGB);

  Handle(Prs3d_LineAspect) aBoundaryAspect = 
    new Prs3d_LineAspect (aColor, aLineType, aWidth);

  aDrawer->SetFaceBoundaryAspect (aBoundaryAspect);

  aContext->Redisplay (anInteractive, Standard_True);
  
  return 0;
}

//=======================================================================
//function : XAssemblyTreeDump
//purpose  : Prints assembly tree structure up to the specified level
//=======================================================================

static Standard_Integer XDumpAssemblyTree(Draw_Interpretor& di,
                                          Standard_Integer argc,
                                          const char ** argv)
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
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  XCAFDoc_AssemblyItemId aRoot;
  Standard_Integer aLevel = INT_MAX;
  Standard_Boolean aPrintNames = Standard_False;
  for (Standard_Integer iarg = 2; iarg < argc; ++iarg)
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
      aPrintNames = Standard_True;
    }
  }

  Standard_SStream aSS;

  XCAFDoc_AssemblyIterator anIt = aRoot.IsNull() ? XCAFDoc_AssemblyIterator(aDoc, aLevel) 
                                                 : XCAFDoc_AssemblyIterator(aDoc, aRoot, aLevel);
  XCAFDoc_AssemblyTool::Traverse(anIt, [&](const XCAFDoc_AssemblyItemId& theItem) -> Standard_Boolean
  {
    if (aPrintNames)
    {
      Standard_Boolean aFirst = Standard_True;
      for (TColStd_ListOfAsciiString::Iterator anIt(theItem.GetPath()); anIt.More(); 
           anIt.Next(), aFirst = Standard_False)
      {
        if (!aFirst) aSS << "/";
        TDF_Label aL;
        TDF_Tool::Label(aDoc->GetData(), anIt.Value(), aL, Standard_False);
        if (!aL.IsNull())
        {
          TCollection_ExtendedString aName;
          Handle(TDataStd_Name) aNameAttr;
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
    return Standard_True;
  });

  di << aSS.str().c_str();
  return 0;
}

//=======================================================================
//function : graphNodeTypename
//purpose  : Returns node type name
//=======================================================================

static 
const char* graphNodeTypename(const XCAFDoc_AssemblyGraph::NodeType theNodeType)
{
  switch (theNodeType)
  {
  case XCAFDoc_AssemblyGraph::NodeType_AssemblyRoot: return "R";
  case XCAFDoc_AssemblyGraph::NodeType_Subassembly:  return "A";
  case XCAFDoc_AssemblyGraph::NodeType_Occurrence:   return "O";
  case XCAFDoc_AssemblyGraph::NodeType_Part:         return "P";
  case XCAFDoc_AssemblyGraph::NodeType_Subshape:     return "S";
  default:                                           return "?";
  }
}

//=======================================================================
//function : XAssemblyGraphDump
//purpose  : Prints assembly graph structure
//=======================================================================

static Standard_Integer XDumpAssemblyGraph(Draw_Interpretor& di,
                                           Standard_Integer argc,
                                           const char ** argv)
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
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  Standard_Boolean aPrintNames = Standard_False;
  TDF_Label aLabel = XCAFDoc_DocumentTool::ShapesLabel(aDoc->Main());
  for (Standard_Integer iarg = 2; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-root") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Root is expected!");
      TDF_Tool::Label(aDoc->GetData(), argv[++iarg], aLabel, Standard_False);
    }
    else if (strcmp(argv[iarg], "-names") == 0)
    {
      aPrintNames = Standard_True;
    }
  }

  Handle(XCAFDoc_AssemblyGraph) aG = new XCAFDoc_AssemblyGraph(aLabel);

  Standard_SStream aSS;

  XCAFDoc_AssemblyTool::Traverse(aG, 
    [](const Handle(XCAFDoc_AssemblyGraph)& /*theGraph*/,
       const Standard_Integer               /*theNode*/) -> Standard_Boolean
    {
      return Standard_True;
    },
    [&](const Handle(XCAFDoc_AssemblyGraph)& theGraph,
        const Standard_Integer               theNode) -> Standard_Boolean
    {
      const TDF_Label& aLabel = theGraph->GetNode(theNode);

      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);

      TCollection_AsciiString aNodeEntry;
      if (aPrintNames)
      {
        Handle(TDataStd_Name) aNameAttr;
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
      const TColStd_PackedMapOfInteger* aLinksPtr = anAdjacencyMap.Seek(theNode);
      if (aLinksPtr != NULL)
      {
        for (TColStd_MapIteratorOfPackedMapOfInteger anIt1(*aLinksPtr); anIt1.More(); anIt1.Next())
        {
          aSS << " " << anIt1.Key();
        }
      }
      aSS << std::endl;

      return Standard_True;
    }
  );

  di << aSS.str().c_str();
  return 0;
}

//=======================================================================
//function : XDumpNomenclature
//purpose  : Prints number of assembly instances
//=======================================================================

static Standard_Integer XDumpNomenclature(Draw_Interpretor& di,
                                          Standard_Integer argc,
                                          const char ** argv)
{
  if (argc < 2)
  {
    di << "Usage :\n " << argv[0] << " Doc [-names] \n"
      << "   Doc    - is the document name. \n"
      << "   -names - prints names instead of entries. \n";

    return 1;
  }

  // get specified document
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  Standard_Boolean aPrintNames = Standard_False;
  for (Standard_Integer iarg = 2; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-names") == 0)
    {
      aPrintNames = Standard_True;
    }
  }

  Handle(XCAFDoc_AssemblyGraph) aG = new XCAFDoc_AssemblyGraph(aDoc);

  Standard_SStream aSS;

  XCAFDoc_AssemblyTool::Traverse(aG, 
    [](const Handle(XCAFDoc_AssemblyGraph)& theGraph,
       const Standard_Integer               theNode) -> Standard_Boolean
    {
      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);
      return (aNodeType == XCAFDoc_AssemblyGraph::NodeType_AssemblyRoot) ||
             (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Subassembly) ||
             (aNodeType == XCAFDoc_AssemblyGraph::NodeType_Part);
    },
    [&](const Handle(XCAFDoc_AssemblyGraph)& theGraph,
        const Standard_Integer               theNode) -> Standard_Boolean
    {
      const TDF_Label& aLabel = theGraph->GetNode(theNode);

      const XCAFDoc_AssemblyGraph::NodeType aNodeType = theGraph->GetNodeType(theNode);

      TCollection_AsciiString aNodeEntry;
      if (aPrintNames)
      {
        Handle(TDataStd_Name) aNameAttr;
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

      return Standard_True;
    }
  );

  di << aSS.str().c_str();

  return 0;
}

//=======================================================================
//function : XRescaleGeometry
//purpose  : Applies geometrical scale to all assembly components
//=======================================================================

static Standard_Integer XRescaleGeometry(Draw_Interpretor& di,
                                         Standard_Integer argc,
                                         const char ** argv)
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
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  // get scale factor
  Standard_Real aScaleFactor = Draw::Atof(argv[2]);
  if (aScaleFactor <= 0)
  {
    di << "Scale factor must be positive\n";
    return 1;
  }

  Standard_Boolean aForce = Standard_False;
  TDF_Label aLabel = XCAFDoc_DocumentTool::ShapesLabel(aDoc->Main());
  for (Standard_Integer iarg = 3; iarg < argc; ++iarg)
  {
    if (strcmp(argv[iarg], "-root") == 0)
    {
      Standard_ProgramError_Raise_if(iarg + 1 >= argc, "Root is expected!");
      TDF_Tool::Label(aDoc->GetData(), argv[++iarg], aLabel, Standard_False);
    }
    else if (strcmp(argv[iarg], "-force") == 0)
    {
      aForce = Standard_True;
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
//function : testDoc
//purpose  : Method to test destruction of document
//=======================================================================
static Standard_Integer testDoc (Draw_Interpretor&,
                                 Standard_Integer argc,
                                 const char ** argv)
{
  if( argc < 2 )
  {
    std::cout<<"Invalid numbers of arguments should be: XTestDoc shape"<<std::endl;
    return 1;
  }
  TopoDS_Shape shape = DBRep::Get(argv[1]);
  if( shape.IsNull())
    return 1;
 
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
 
  Handle(TDocStd_Document) aD1 = new TDocStd_Document("BinXCAF");
  aD1->Open(anApp);
  
  TCollection_AsciiString  aViewName ("Driver1/DummyDocument/View1");
  ViewerTest::ViewerInit (aViewName);
  TPrsStd_AISViewer::New (aD1->GetData()->Root(), ViewerTest::GetAISContext());

  // get shape tool for shape verification
  Handle(XCAFDoc_ShapeTool) aShapes =
    XCAFDoc_DocumentTool::ShapeTool (aD1->Main());
  TDF_Label aLab = aShapes->AddShape(shape);

  Handle(Geom_Axis2Placement) aPlacement = 
    new Geom_Axis2Placement (gp::Origin(), gp::DZ(),gp::DX());
  Handle(AIS_Trihedron) aTriShape = new AIS_Trihedron (aPlacement);
  
  Handle(TNaming_NamedShape) NS;
  Handle(TPrsStd_AISPresentation) prs;
  if( aLab.FindAttribute( TNaming_NamedShape::GetID(), NS) ) {
    prs = TPrsStd_AISPresentation::Set( NS );
  }
   
  if( aLab.FindAttribute(TPrsStd_AISPresentation::GetID(), prs) ) 
    prs->Display();

  TPrsStd_AISViewer::Update(aLab);
  ViewerTest::GetAISContext()->Display (aTriShape, Standard_True);
  aD1->BeforeClose();
  aD1->Close();
  ViewerTest::RemoveView (aViewName);
  return 0;
}


//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void XDEDRAW::Init(Draw_Interpretor& di)
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  // Load static variables for STEPCAF (ssv; 16.08.2012)
  STEPCAFControl_Controller::Init();

  // Initialize XCAF formats
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  BinXCAFDrivers::DefineFormat(anApp);
  XmlXCAFDrivers::DefineFormat(anApp);

  // Register driver in global table for displaying XDE documents 
  // in 3d viewer using OCAF mechanics
  TPrsStd_DriverTable::Get()->AddDriver (XCAFPrs_Driver::GetID(), new XCAFPrs_Driver);

  //=====================================
  // General commands
  //=====================================

  Standard_CString g = "XDE general commands";

  di.Add ("XNewDoc","DocName \t: Create new DECAF document",
		   __FILE__, newDoc, g);

  di.Add ("XSave","[Doc Path] \t: Save Doc or first document in session",
		   __FILE__, saveDoc, g);

  di.Add ("XOpen","Path Doc [-skipAttribute] [-readAttribute] [-readPath] [-append|-overwrite]\t: Open XDE Document with name Doc from Path"
          "\n\t\t The options are:"
          "\n\t\t   -skipAttribute : class name of the attribute to skip during open, for example -skipTDF_Reference"
          "\n\t\t   -readAttribute : class name of the attribute to read only during open, for example -readTDataStd_Name loads only such attributes"
          "\n\t\t   -append : to read file into already existing document once again, append new attributes and don't touch existing"
          "\n\t\t   -overwrite : to read file into already existing document once again, overwriting existing attributes",
    __FILE__, openDoc, g);

  di.Add ("Xdump","Doc [int deep (0/1)] \t: Print information about tree's structure",
		   __FILE__, dump, g);

  di.Add ("XStat","Doc \t: Print statistics of document",
		   __FILE__, statdoc, g);

  di.Add ("XSetPrs","Doc [label1 lavbel2 ...] \t: Set presentation for given label(s) or whole doc",
		   __FILE__, setPrs, g);

  di.Add ("XShow","Doc [label1 lavbel2 ...] \t: Display document (or some labels) in a graphical window",
		   __FILE__, show, g);

  di.Add ("XDisplay",
          "XDisplay Doc [label1 [label2 [...]]] [-explore {on|off}] [-docPrefix {on|off}] [-names {on|off}]"
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
          __FILE__, XDEDRAW_XDisplayTool::XDisplay, g);

  di.Add ("XWdump","Doc filename.{gif|xwd|bmp} \t: Dump contents of viewer window to XWD, GIF or BMP file",
		   __FILE__, xwd, g);

  di.Add ("XAttributeValue", "Doc label #attribute: internal command for browser",
		   __FILE__, XAttributeValue, g);

  di.Add ("XSetViewNameMode", "(1/0) \t: Set/Unset mode of displaying names.",
		   __FILE__, setviewName, g);

  di.Add ("XGetViewNameMode", "\t: Print if  mode of displaying names is turn on.",
		   __FILE__, getviewName, g);

  di.Add ("XSetTransparency", "Doc Transparency [label1 label2 ...]\t: Set transparency for given label(s) or whole doc",
		   __FILE__, XSetTransparency, g);

  di.Add("XSetLengthUnit",
    "Doc {unit_name|scale_factor}\t: Set value of length unit"
    "\n\t\t: Possible unit_name: m, mm, km, cm, micron, mille, in(inch), min(microinch), nin(nano inch), ft, stat.mile"
    "\n\t\t: Possible scale factor: any real value more then 0. Factor to meter.",
    __FILE__, setLengthUnit, g);

  di.Add("XGetLengthUnit",
    "Doc [-scale]\t: Print name of length unit"
    "\n\t\t: -scale : print value of the scaling factor to meter of length unit",
    __FILE__, dumpLengthUnit, g);

  di.Add ("XShowFaceBoundary",
          "Doc Label IsOn [R G B [LineWidth [LineStyle]]]:"
          "- turns on/off drawing of face boundaries and defines boundary line style",
          __FILE__, XShowFaceBoundary, g);
   di.Add ("XTestDoc", "XTestDoc shape", __FILE__, testDoc, g);

  di.Add("XDumpAssemblyTree",
         "Doc [-root label] [-level l] [-names]: Iterates through the assembly tree in depth up to the specified level, if any",
         __FILE__, XDumpAssemblyTree, g);
  di.Add("XDumpAssemblyGraph",
         "Doc [-root label] [-names]: Prints assembly graph structure",
         __FILE__, XDumpAssemblyGraph, g);
  di.Add("XDumpNomenclature",
         "Doc [-names]: Prints number of assembly instances",
         __FILE__, XDumpNomenclature, g);
  di.Add("XRescaleGeometry",
         "Doc factor [-root label] [-force]: Applies geometrical scale to assembly",
         __FILE__, XRescaleGeometry, g);

  // Specialized commands
  XDEDRAW_Shapes::InitCommands ( di );
  XDEDRAW_Colors::InitCommands ( di );
  XDEDRAW_Layers::InitCommands ( di );
  XDEDRAW_Props::InitCommands ( di );
  XDEDRAW_GDTs::InitCommands ( di );
  XDEDRAW_Views::InitCommands(di);
  XDEDRAW_Notes::InitCommands(di);
  XDEDRAW_Common::InitCommands ( di );//moved from EXE
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
