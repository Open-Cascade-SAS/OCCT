// File:	XDEDRAW.cxx
// Created:	Fri Aug  4 14:38:55 2000
// Author:	Pavel TELKOV
//		<ptv@zamox.nnov.matra-dtv.fr>


#include <XDEDRAW.ixx>
#include <stdio.h>

#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Quantity_Color.hxx>

#include <TopoDS_Shape.hxx>

#include <Draw.hxx>
#include <DBRep.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest_Tool.hxx>

#include <DDF_Browser.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Data.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_RealArray.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_NamedShapeDriver.hxx>
#include <TPrsStd_AISViewer.hxx>

#include <XCAFDoc.hxx>
#include <XCAFDoc_Color.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <XCAFDoc_DimTol.hxx>
#include <XCAFDoc_Material.hxx>
#include <XCAFPrs_Driver.hxx>
#include <XCAFApp_Application.hxx>

#include <XDEDRAW_Shapes.hxx>
#include <XDEDRAW_Colors.hxx>
#include <XDEDRAW_Layers.hxx>
#include <XDEDRAW_Props.hxx>
#include <XDEDRAW_Common.hxx>
#include <XSDRAWIGES.hxx>
#include <XSDRAWSTEP.hxx>
#include <SWDRAW.hxx>
#include <XSDRAW.hxx>
#include <XCAFPrs.hxx>
#include <ViewerTest.hxx>
#include <Draw_PluginMacro.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#define ZVIEW_SIZE 1000000.0
// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

//=======================================================================
// Section: General commands
//=======================================================================

//=======================================================================
//function : newDoc
//purpose  :
//=======================================================================
static Standard_Integer newDoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {di<<"Give document name"<<"\n";return 1;}

  Handle(TDocStd_Document) D;
  Handle(DDocStd_DrawDocument) DD;
  Handle(TDocStd_Application) A;

  if (!DDocStd::Find(A)) return 1;

  if (!DDocStd::GetDocument(argv[1],D,Standard_False)) {
    A->NewDocument(  "MDTV-XCAF"  ,D);
    DD = new DDocStd_DrawDocument(D);
    TDataStd_Name::Set(D->GetData()->Root(),argv[1]);
    Draw::Set(argv[1],DD);
    di << "document " << argv[1] << " created" << "\n";
    //DDocStd::ReturnLabel(di,D->Main());
  }
  else di << argv[1] << " is already a document" << "\n";

  return 0;
}


//=======================================================================
//function : saveDoc
//purpose  :
//=======================================================================
static Standard_Integer saveDoc (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(TDocStd_Document) D;
  Handle(TDocStd_Application) A;
  if (!DDocStd::Find(A)) return 1;

  if (argc == 1) {
    if (A->NbDocuments() < 1) return 1;
    A->GetDocument(1, D);
  }
  else {
    if (!DDocStd::GetDocument(argv[1],D)) return 1;
  }

  if (argc == 3 ) {
    TCollection_ExtendedString path (argv[2]);
    A->SaveAs(D,path);
    return 0;
  }
  if (!D->IsSaved()) {
    di << "this document has never been saved" << "\n";
    return 1;
  }
  A->Save(D);
  return 0;
}


//=======================================================================
//function : dump
//purpose  :
//=======================================================================
static Standard_Integer dump (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<2) {
    di<<"Use: "<<argv[0]<<" Doc [int deep (0/1)]"<<"\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document" << "\n"; return 1; }

  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean deep = Standard_False;
  if ( (argc==3) && (atoi(argv[2])==1) ) deep = Standard_True;
  myAssembly->Dump(deep);
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
  Quantity_Color col;
  Standard_Boolean IsColor = Standard_False;
  if(CTool->GetColor(L,XCAFDoc_ColorGen,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorSurf,col))
    IsColor = Standard_True;
  else if(CTool->GetColor(L,XCAFDoc_ColorCurv,col))
    IsColor = Standard_True;
  if(IsColor) {
    TCollection_AsciiString Entry1;
    Entry1 = col.StringName(col.Name());
    if(PrintStructMode) di<<"Color("<<Entry1.ToCString()<<") ";
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
          di<<" "<<"\""<<Entry2.ToCString()<<"\"";
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
    di<<"Use: "<<argv[0]<<" Doc "<<"\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document" << "\n"; return 1; }

  Standard_Boolean PrintStructMode = (argc==3);
  Handle(XCAFDoc_ShapeTool) aTool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());

  TDF_LabelSequence SeqLabels;
  aTool->GetShapes(SeqLabels);
  if(SeqLabels.Length()<=0) return 0;
  if(PrintStructMode) di<<"\n"<<"Structure of shapes in the document:"<<"\n";
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
  di<<"\n"<<"Statistis of shapes in the document:"<<"\n";
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

  di<<"\n"<<"Statistis of Props in the document:"<<"\n";
  di<<"Number of Centroid Props = "<<NbCentroidProp<<"\n";
  di<<"Number of Volume Props = "<<NbVolumeProp<<"\n";
  di<<"Number of Area Props = "<<NbAreaProp<<"\n";

  Handle(XCAFDoc_ColorTool) CTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  TDF_LabelSequence CLabels;
  CTool->GetColors(CLabels);
  di<<"\n"<<"Number of colors = "<<CLabels.Length()<<"\n";
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
  di<<"\n"<<"Number of layers = "<<LLabels.Length()<<"\n";
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
    di<<"Use: "<<argv[0]<<" DocName [label1 label2 ...] "<<"\n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document" << "\n"; return 1; }

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
    di<<"Use: "<<argv[0]<<" DocName [label1 label2 ...] "<<"\n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document" << "\n"; return 1; }

  // init viewer
//  char string[260];
//  sprintf ( string, "AISInitViewer %s", argv[1] );
//  di.Eval ( string );
  TDF_Label acces = Doc->GetData()->Root();
  Handle(TPrsStd_AISViewer) viewer;
  if (!TPrsStd_AISViewer::Find (acces,viewer)) {
    TCollection_AsciiString title;
    title.Prepend(argv[1]);
    title.Prepend("_");
    title.Prepend("Document");
    Handle(V3d_Viewer) vw=ViewerTest_Tool::MakeViewer (title.ToCString());
    viewer = TPrsStd_AISViewer::New (acces,vw);
  }
  ViewerTest_Tool::InitViewerTest (viewer->GetInteractiveContext());

  //szv:CAX-TRJ7 c2-pe-214.stp was clipped
  viewer->GetInteractiveContext()->CurrentViewer()->ActiveView()->SetZSize(ZVIEW_SIZE);
  //DDF::ReturnLabel(di,viewer->Label());

  // collect sequence of labels to display
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
  TPrsStd_AISViewer::Update(Doc->GetData()->Root());
  return 0;
}


//=======================================================================
//function : xwd
//purpose  :
//=======================================================================
static Standard_Integer xwd (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <3) {
    di<<"Use: "<<argv[0]<<" DocName filename.{xwd|gif|bmp}"<<"\n";
    return 1;
  }

  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document" << "\n"; return 1; }

  Handle(AIS_InteractiveContext) IC;
  if ( ! TPrsStd_AISViewer::Find ( Doc->GetData()->Root(), IC ) ) {
    di << "Cannot find viewer for document " << argv[1] << "\n";
    return 1;
  }

  Handle(V3d_Viewer) viewer = IC->CurrentViewer();
  viewer->InitActiveViews();
  if ( viewer->MoreActiveViews() ) {
    viewer->ActiveView()->Dump ( argv[2] );
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
  if ( argc <4 ) { di << "ERROR: Too few args" << "\n"; return 0; }
  Handle(DDF_Browser) browser =
    Handle(DDF_Browser)::DownCast (Draw::Get(argv[1], Standard_True));
  if ( browser.IsNull() ) { di << "ERROR: Not a browser: " << argv[1] << "\n"; return 0; }

  TDF_Label lab;
  TDF_Tool::Label(browser->Data(),argv[2],lab);
  if ( lab.IsNull() ) { di << "ERROR: label is Null: " << argv[2] << "\n"; return 0; }

  Standard_Integer num = atoi ( argv[3] );
  TDF_AttributeIterator itr(lab,Standard_False);
  for (Standard_Integer i=1; itr.More() && i < num; i++) itr.Next();

  if ( ! itr.More() ) { di << "ERROR: Attribute #" << num << " not found" << "\n"; return 0; }

  const Handle(TDF_Attribute)& att = itr.Value();
  if ( att->IsKind(STANDARD_TYPE(TDataStd_TreeNode)) ) {
    Standard_CString type;
    if ( att->ID() == XCAFDoc::ShapeRefGUID() ) type = "Shape Instance Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorGen) ) type = "Generic Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorSurf) ) type = "Surface Color Link";
    else if ( att->ID() == XCAFDoc::ColorRefGUID(XCAFDoc_ColorCurv) ) type = "Curve Color Link";
    else if ( att->ID() == XCAFDoc::DimTolRefGUID() ) type = "DGT Link";
    else if ( att->ID() == XCAFDoc::DatumRefGUID() ) type = "Datum Link";
    else if ( att->ID() == XCAFDoc::MaterialRefGUID() ) type = "Material Link";
    else return 0;
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
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_Name)) ) {
    Handle(TDataStd_Name) val = Handle(TDataStd_Name)::DownCast ( att );
    TCollection_AsciiString str ( val->Get(), '?' );
    di << str.ToCString();
  }
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_RealArray)) ) {
    Handle(TDataStd_RealArray) val = Handle(TDataStd_RealArray)::DownCast ( att );
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
  else if ( att->IsKind(STANDARD_TYPE(TDataStd_UAttribute)) ) {
    if ( att->ID() == XCAFDoc::AssemblyGUID() ) di << "is assembly";
    if ( att->ID() == XCAFDoc::InvisibleGUID() ) di << "invisible";
  }
  else if ( att->IsKind(STANDARD_TYPE(XCAFDoc_Color)) ) {
    Handle(XCAFDoc_Color) val = Handle(XCAFDoc_Color)::DownCast ( att );
    Quantity_Color C = val->GetColor();
    char string[260];
    sprintf ( string, "%s (%g, %g, %g)", C.StringName ( C.Name() ),
	      C.Red(), C.Green(), C.Blue() );
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
    di<<"Use: "<<argv[0]<<" (1/0)"<<"\n";
    return 1;
  }
  Standard_Boolean mode = Standard_False;
  if (atoi(argv[1]) == 1) mode = Standard_True;
  XCAFPrs::SetViewNameMode(mode);
  return 0;
}


//=======================================================================
//function : getviewName
//purpose  : auxilary
//=======================================================================

static Standard_Integer getviewName (Draw_Interpretor&  di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  if ( XCAFPrs::GetViewNameMode() ) di << "Display names ON"<< "\n";
  else di << "Display names OFF"<< "\n";
  return 0;
}


//=======================================================================
//function : Init
//purpose  :
//=======================================================================

void XDEDRAW::Init(Draw_Interpretor& di)
{

  static Standard_Boolean initactor = Standard_False;
  if (initactor) return;  initactor = Standard_True;

  // OCAF *** szy: use <pload> command

//  DDF::AllCommands(di);
//  DNaming::AllCommands(di);
//  DDataStd::AllCommands(di);
//  DPrsStd::AllCommands(di);
  //DFunction::AllCommands(di);
//  DDocStd::AllCommands(di);

//  ViewerTest::Commands (di); *** szy: use <pload> command

  // init XCAF application (if not yet done)
  XCAFApp_Application::GetApplication();

  //=====================================
  // General commands
  //=====================================

  Standard_CString g = "XDE general commands";

  di.Add ("XNewDoc","DocName \t: Create new DECAF document",
		   __FILE__, newDoc, g);

  di.Add ("XSave","[Doc Path] \t: Save Doc or first document in session",
		   __FILE__, saveDoc, g);

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

  // Specialized commands
  XDEDRAW_Shapes::InitCommands ( di );
  XDEDRAW_Colors::InitCommands ( di );
  XDEDRAW_Layers::InitCommands ( di );
  XDEDRAW_Props::InitCommands ( di );
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

  SWDRAW::Init(theDI);
  XSDRAW::LoadDraw(theDI);

  XDEDRAW::Init(theDI);

#ifdef DEB
      theDI << "Draw Plugin : All TKXDEDRAW commands are loaded" << "\n";
#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XDEDRAW)
