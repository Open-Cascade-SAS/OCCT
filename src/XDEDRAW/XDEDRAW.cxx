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


#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Trihedron.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <DBRep.hxx>
#include <DDF_Browser.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_PluginMacro.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <ViewerTest.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFPrs.hxx>
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
#include <XSDRAW.hxx>
#include <XSDRAWIGES.hxx>
#include <XSDRAWSTEP.hxx>

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

  PCDM_StoreStatus aStatus = PCDM_SS_Doc_IsNull;
  if (argc == 3)
  {
    TCollection_ExtendedString path (argv[2]);
    aStatus = A->SaveAs (D, path);
  }
  else if (!D->IsSaved())
  {
    std::cout << "Storage error: this document has never been saved\n";
    return 1;
  }
  else
  {
    aStatus = A->Save(D);
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

  if ( argc != 3 )
  {
    di << "invalid number of arguments. Usage:\t XOpen filename docname\n";
    return 1;
  }

  Standard_CString Filename = argv[1];
  Standard_CString DocName = argv[2];

  if ( DDocStd::GetDocument(DocName, D, Standard_False) )
  {
    di << "document with name " << DocName << " already exists\n";
    return 1;
  }

  if ( A->Open(Filename, D) != PCDM_RS_OK )
  {
    di << "cannot open XDE document\n";
    return 1;
  }

  DD = new DDocStd_DrawDocument(D);
  TDataStd_Name::Set(D->GetData()->Root(), DocName);
  Draw::Set(DocName, DD);

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
      TCollection_AsciiString AsciiStringName(Name->Get(),'?');
      di<<" "<<AsciiStringName.ToCString()<<"  has attributes: ";
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
  Quantity_ColorRGBA col;
  Standard_Boolean IsColor = Standard_False;
  if(CTool->GetColor(L,XCAFDoc_ColorGen,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorSurf,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorCurv,col))
    IsColor = Standard_True;
  if(IsColor) {
    TCollection_AsciiString Entry1;
    Entry1 = col.GetRGB().StringName(col.GetRGB().Name());
    if(PrintStructMode) di<<"Color("<<Entry1.ToCString()<<" "<<col.Alpha()<<") ";
    NbShapesWithColor++;
  }
  Handle(XCAFDoc_LayerTool) LTool = XCAFDoc_DocumentTool::LayerTool(aDoc->Main());
  Handle(TColStd_HSequenceOfExtendedString) aLayerS;
  LTool->GetLayers(L, aLayerS);
  if(!aLayerS.IsNull() && aLayerS->Length()>0) {
    if(PrintStructMode) {
      di<<"Layer(";
      for(Standard_Integer i=1; i<=aLayerS->Length(); i++) {
        TCollection_AsciiString Entry2(aLayerS->Value(i),'?');
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
      TCollection_AsciiString Entry(layerName,'?');
      di<<"\""<<Entry.ToCString() <<"\" ";
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
      prs->SetMaterial ( Graphic3d_NOM_PLASTIC );
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
    ViewerTest::ViewerInit (0, 0, 0, 0, aViewName.ToCString(), "");
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
      prs->SetMaterial ( Graphic3d_NOM_PLASTIC );
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
  if ( argc <4 ) { di << "ERROR: Too few args\n"; return 0; }
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(argv[1], Standard_True));
  if ( browser.IsNull() ) { di << "ERROR: Not a browser: " << argv[1] << "\n"; return 0; }

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),argv[2],lab);
  if ( lab.IsNull() ) { di << "ERROR: label is Null: " << argv[2] << "\n"; return 0; }

  Standard_Integer num = Draw::Atoi ( argv[3] );
  TDF_AttributeIterator itr(lab,Standard_False);
  for (Standard_Integer i=1; itr.More() && i < num; i++) itr.Next();

  if ( ! itr.More() ) { di << "ERROR: Attribute #" << num << " not found\n"; return 0; }

  const Handle(TDF_Attribute)& att = itr.Value();
  if ( att->IsKind(STANDARD_TYPE(TDataStd_TreeNode)) ) {
    Standard_CString type = "";
    if ( att->ID() == XCAFDoc::ShapeRefGUID() ) type = "Shape Instance Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorGen) ) type = "Generic Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorSurf) ) type = "Surface Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorCurv) ) type = "Curve Color Link";
    else if ( att->ID() == XCAFDoc::DimTolRefGUID() ) type = "DGT Link";
    else if ( att->ID() == XCAFDoc::DatumRefGUID() ) type = "Datum Link";
    else if ( att->ID() == XCAFDoc::MaterialRefGUID() ) type = "Material Link";
    Handle(TDataStd_TreeNode) TN = Handle(TDataStd_TreeNode)::DownCast(att);
    TCollection_AsciiString ref;
    if ( TN->HasFather() ) {
      TDF_Tool::Entry ( TN->Father()->Label(), ref );
      di << type << " ==> " << ref.ToCString();
    }
    else {
      di << type << " <== (" << ref.ToCString();
      Handle(TDataStd_TreeNode) child = TN->First();
      while ( ! child.IsNull() ) {
        TDF_Tool::Entry ( child->Label(), ref );
        if ( child != TN->First() ) di << ", ";
        di << ref.ToCString();
        child = child->Next();
      }
      di << ")";
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDF_Reference)) ) {
    Handle(TDF_Reference) val = Handle(TDF_Reference)::DownCast ( att );
    TCollection_AsciiString ref;
    TDF_Tool::Entry ( val->Get(), ref );
    di << "==> " << ref.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Integer)) ) {
    Handle(TDataStd_Integer) val = Handle(TDataStd_Integer)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Real)) ) {
    Handle(TDataStd_Real) val = Handle(TDataStd_Real)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Name)) ) {
    Handle(TDataStd_Name) val = Handle(TDataStd_Name)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Comment)) ) {
    Handle(TDataStd_Comment) val = Handle(TDataStd_Comment)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_AsciiString)) ) {
    Handle(TDataStd_AsciiString) val = Handle(TDataStd_AsciiString)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_IntegerArray)) ) {
    Handle(TDataStd_IntegerArray) val = Handle(TDataStd_IntegerArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_RealArray)) ) {
    Handle(TDataStd_RealArray) val = Handle(TDataStd_RealArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_ByteArray)) ) {
    Handle(TDataStd_ByteArray) val = Handle(TDataStd_ByteArray)::DownCast ( att );
    for ( Standard_Integer j=val->Lower(); j <= val->Upper(); j++ ) {
      if ( j > val->Lower() ) di << ", ";
      TCollection_AsciiString str ( val->Value(j) );
      di << str.ToCString();
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(TNaming_NamedShape)) ) {
    Handle(TNaming_NamedShape) val = Handle(TNaming_NamedShape)::DownCast ( att );
    TopoDS_Shape S = val->Get();
    di << S.TShape()->DynamicType()->Name();
    if ( ! S.Location().IsIdentity() ) di << "(located)";
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Volume)) ) {
    Handle(XCAFDoc_Volume) val = Handle(XCAFDoc_Volume)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Area)) ) {
    Handle(XCAFDoc_Area) val = Handle(XCAFDoc_Area)::DownCast ( att );
    TCollection_AsciiString str ( val->Get() );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Centroid)) ) {
    Handle(XCAFDoc_Centroid) val = Handle(XCAFDoc_Centroid)::DownCast ( att );
    gp_Pnt myCentroid = val->Get();
    di << "(" ;
    di << myCentroid.X();
    di <<" , ";
    di << myCentroid.Y();
    di <<" , ";
    di << myCentroid.Z();
    di << ")";
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_UAttribute)) ) {
    if ( att->ID() == XCAFDoc::AssemblyGUID() ) di << "is assembly";
    if ( att->ID() == XCAFDoc::InvisibleGUID() ) di << "invisible";
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Color)) ) {
    Handle(XCAFDoc_Color) val = Handle(XCAFDoc_Color)::DownCast ( att );
    Quantity_ColorRGBA C = val->GetColorRGBA();
    char string[260];
    Sprintf ( string, "%s (%g, %g, %g, %g)", C.GetRGB().StringName ( C.GetRGB().Name() ),
      C.GetRGB().Red(), C.GetRGB().Green(), C.GetRGB().Blue(), C.Alpha());
    di << string;
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_DimTol)) ) {
    Handle(XCAFDoc_DimTol) val = Handle(XCAFDoc_DimTol)::DownCast ( att );
    Standard_Integer kind = val->GetKind();
    Handle(TColStd_HArray1OfReal) HAR = val->GetVal();
    if(kind<20) { //dimension
      di<<"Diameter (ValueRange["<<HAR->Value(1)<<","<<HAR->Value(2)<<"])";
    }
    else {
      switch(kind) {
      case 21: di << "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_1 (Value="<<HAR->Value(1)<<")"; break;
      case 22: di << "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_2 (Value="<<HAR->Value(1)<<")"; break;
      case 23: di << "GeoTolAndGeoTolWthDatRefAndModGeoTolAndPosTol_3 (Value="<<HAR->Value(1)<<")"; break;
      case 24: di << "AngularityTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 25: di << "CircularRunoutTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 26: di << "CoaxialityTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 27: di << "ConcentricityTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 28: di << "ParallelismTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 29: di << "PerpendicularityTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 30: di << "SymmetryTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 31: di << "TotalRunoutTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 35: di << "ModifiedGeometricTolerance_1 (Value="<<HAR->Value(1)<<")"; break;
      case 36: di << "ModifiedGeometricTolerance_2 (Value="<<HAR->Value(1)<<")"; break;
      case 37: di << "ModifiedGeometricTolerance_3 (Value="<<HAR->Value(1)<<")"; break;
      case 38: di << "CylindricityTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 39: di << "FlatnessTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 40: di << "LineProfileTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 41: di << "PositionTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 42: di << "RoundnessTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 43: di << "StraightnessTolerance (Value="<<HAR->Value(1)<<")"; break;
      case 44: di << "SurfaceProfileTolerance (Value="<<HAR->Value(1)<<")"; break;
      }
    }
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Material)) ) {
    Handle(XCAFDoc_Material) val = Handle(XCAFDoc_Material)::DownCast ( att );
    Standard_Real dens = val->GetDensity();
    Standard_CString dimdens = "g/cu sm";
    if(dens==0) 
      di<<val->GetName()->ToCString();
    else
      di<<val->GetName()->ToCString()<<"(density="<<dens<<dimdens<<")";
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_GraphNode)) ) {
    Standard_CString type;
    if ( att->ID() == XCAFDoc::LayerRefGUID() ) {
      type = "Layer Instance Link";
    }
    else if ( att->ID() == XCAFDoc::SHUORefGUID() ) {
      type = "SHUO Instance Link";
    }
    else if ( att->ID() == XCAFDoc::DatumTolRefGUID() ) {
      type = "DatumToler Link";
    }
    else if ( att->ID() == XCAFDoc::DimensionRefFirstGUID() ) {
      type = "Dimension Link First";
    }
    else if ( att->ID() == XCAFDoc::DimensionRefSecondGUID() ) {
      type = "Dimension Link Second";
    }
    else if ( att->ID() == XCAFDoc::GeomToleranceRefGUID() ){
      type = "GeomTolerance Link";
    }
    else if ( att->ID() == XCAFDoc::DatumRefGUID() ){
      type = "Datum Link";
    }
    else if (att->ID() == XCAFDoc::ViewRefShapeGUID()){
      type = "View Shape Link";
    }
    else if (att->ID() == XCAFDoc::ViewRefGDTGUID()){
      type = "View GD&T Link";
    }
    else if (att->ID() == XCAFDoc::ViewRefPlaneGUID()) {
      type = "View Clipping Plane Link";
    }
    else return 0;

    Handle(XCAFDoc_GraphNode) DETGN = Handle(XCAFDoc_GraphNode)::DownCast(att);
    TCollection_AsciiString ref;
    Standard_Integer ii = 1;
    if (DETGN->NbFathers()!=0) {

      TDF_Tool::Entry ( DETGN->GetFather(ii)->Label(), ref );
      di << type<< " ==> (" << ref.ToCString();
      for (ii = 2; ii <= DETGN->NbFathers(); ii++) {
	TDF_Tool::Entry ( DETGN->GetFather(ii)->Label(), ref );
	  di << ", " << ref.ToCString();
	}
      di << ") ";
    }
    ii = 1;
    if (DETGN->NbChildren()!=0) {
      TDF_Tool::Entry ( DETGN->GetChild(ii)->Label(), ref );
      di << type<< " <== (" << ref.ToCString();
      for (ii = 2; ii <= DETGN->NbChildren(); ii++) {
	TDF_Tool::Entry ( DETGN->GetChild(ii)->Label(), ref );
	  di << ", " << ref.ToCString();
	}
      di << ")";
    }
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
//purpose  : auxilary
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
      prs->SetMaterial ( Graphic3d_NOM_PLASTIC );
    }
    prs->SetTransparency( aTransparency );
  }
  TPrsStd_AISViewer::Update(Doc->GetData()->Root());
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
//function : testDoc
//purpose  : Method to test destruction of document
//=======================================================================
static Standard_Integer testDoc (Draw_Interpretor&,
                                 Standard_Integer argc,
                                 const char ** argv)
{
  if( argc < 2 )
  {
    cout<<"Invalid numbers of arguments should be: XTestDoc shape"<<endl;
    return 1;
  }
  TopoDS_Shape shape = DBRep::Get(argv[1]);
  if( shape.IsNull())
    return 1;
 
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
 
  Handle(TDocStd_Document) aD1 = new TDocStd_Document("BinXCAF");
  aD1->Open(anApp);
  
  TCollection_AsciiString  aViewName ("Driver1/DummyDocument/View1");
  ViewerTest::ViewerInit (0, 0, 0, 0, aViewName.ToCString(), "");
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

  di.Add ("XOpen","Path Doc \t: Open XDE Document with name Doc from Path",
          __FILE__, openDoc, g);

  di.Add ("Xdump","Doc [int deep (0/1)] \t: Print information about tree's structure",
		   __FILE__, dump, g);

  di.Add ("XStat","Doc \t: Print statistics of document",
		   __FILE__, statdoc, g);

  di.Add ("XSetPrs","Doc [label1 lavbel2 ...] \t: Set presentation for given label(s) or whole doc",
		   __FILE__, setPrs, g);

  di.Add ("XShow","Doc [label1 lavbel2 ...] \t: Display document (or some labels) in a graphical window",
		   __FILE__, show, g);

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

  di.Add ("XShowFaceBoundary", 
          "Doc Label IsOn [R G B [LineWidth [LineStyle]]]:"
          "- turns on/off drawing of face boundaries and defines boundary line style",
          __FILE__, XShowFaceBoundary, g);
   di.Add ("XTestDoc", "XTestDoc shape", __FILE__, testDoc, g);

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
  XSDRAWIGES::InitSelect();
  XSDRAWIGES::InitToBRep(theDI);
  XSDRAWIGES::InitFromBRep(theDI);

  XSDRAWSTEP::InitCommands(theDI);

  XSDRAW::LoadDraw(theDI);

  XDEDRAW::Init(theDI);

#ifdef OCCT_DEBUG
      theDI << "Draw Plugin : All TKXDEDRAW commands are loaded\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XDEDRAW)
