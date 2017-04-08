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


#include <BRep_Builder.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <gp_Trsf.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_AttributeSequence.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GraphNode.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XDEDRAW_Shapes.hxx>

#include <stdio.h>
//=======================================================================
// Section: Work with shapes
//=======================================================================
static Standard_Integer addShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<" DocName Shape [int makeAssembly (1/0)]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean makeAssembly = Standard_True;
  if ( argc==4 && Draw::Atoi(argv[3]) == 0 ) makeAssembly = Standard_False;
  TDF_Label aLabel;
  aLabel = myAssembly->AddShape(aShape, makeAssembly);
  if (aLabel.IsNull()) di<<"Null Label\n";
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aLabel, Entry);
  di << Entry.ToCString();
  return 0;
}

static Standard_Integer newShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=2) {
    di<<"Use: "<<argv[0]<<" DocName \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  TDF_Label aLabel;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  aLabel=myAssembly->NewShape();
  //  di<<"New Shape at ChildTag"<<aLabel.Tag()<<"\n";
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aLabel, Entry);
  di << Entry.ToCString();
  return 0;
}

static Standard_Integer setShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=4) {
    di<<"Use: "<<argv[0]<<" DocName Label Shape \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  TopoDS_Shape aShape;
  //   if (aLabel.IsNull()) di<<"Null Label\n";
  aShape = DBRep::Get(argv[3]);
//  XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  myAssembly->SetShape(aLabel, aShape);
  return 0;
}

static Standard_Integer getShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=4) {
    di<<"Use: "<<argv[0]<<" Result DocName Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[2], Doc);
  if ( Doc.IsNull() ) { di << argv[2] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[3], aLabel);
  if (aLabel.IsNull()) {di<<"No such Label\n"; return 1;}
  TopoDS_Shape aShape;
//  XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  aShape = myAssembly->GetShape(aLabel);
  Standard_CString name1 = argv[1];
  DBRep::Set(name1, aShape);
  
  return 0;
}

static Standard_Integer removeShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3 && argc != 4)
  {
    di<<"Use: "<<argv[0]<<" DocName Label [int removeCompletely (1/0)]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull()) {di<<"No such Label\n"; return 1;}
  TopoDS_Shape aShape;
//  XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Standard_Boolean removeCompletely = Standard_True;
  if ( argc == 4 && Draw::Atoi(argv[3]) == 0 ) 
    removeCompletely = Standard_False;
  myAssembly->RemoveShape(aLabel, removeCompletely);
  
  return 0;
}

static Standard_Integer findShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" DocName Shape\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);
//  XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  aLabel = myAssembly->FindShape(aShape);
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aLabel, Entry);
  di << Entry.ToCString();
  //di<<"Label with Shape is "<<Entry<<"\n";
  return 0;
}

static Standard_Integer labelInfo (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" DocName Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
//  XCAFDoc_ShapeTool myAssembly;
//  myAssembly.Init(Doc);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TCollection_AsciiString Entry;

  if ( myAssembly->IsShape(aLabel) ) {
    //di<<"There are a TopLevelShape\n";
    Entry="There are a TopLevelShape";
    di << Entry.ToCString();
  }
  if ( myAssembly->IsComponent(aLabel) ) {
    //di<<"There are a Component\n";
    Entry="There are a Component";
    di << Entry.ToCString();
  }
  if ( myAssembly->IsAssembly(aLabel) ) {
    //di<<"There are an Assembly\n";
    Entry="There are an Assembly";
    di << Entry.ToCString();
  }
  if ( myAssembly->IsFree(aLabel) ) {
    //di<<"This Shape don't used\n";
    Entry="This Shape don't used";
    di << Entry.ToCString();
  }
  return 0;
}

static Standard_Integer getUsers (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<" Doc Label [withSubChilds(int)]\n";
    return 1;
  }
  Standard_Boolean getsubchilds = Standard_False;
  if ( (argc==4) && ( Draw::Atoi(argv[3])==1 ) ) getsubchilds = Standard_True;
  
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  TDF_LabelSequence labseq;
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TCollection_AsciiString Entry;
  Entry=myAssembly->GetUsers(aLabel, labseq, getsubchilds);
  di << Entry.ToCString();
  //di<<myAssembly->GetUsers(aLabel, labseq, getsubchilds)<<" assemblies use this component\n";
  return 0;
}

static Standard_Integer nbComponents (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<" Doc Label [withSubChilds(int)]\n";
    return 1;
  }
  Standard_Boolean getsubchilds = Standard_False;
  if ( (argc==4) && ( Draw::Atoi(argv[3])==1 ) ) getsubchilds = Standard_True;
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//  XCAFDoc_ShapeTool myAssembly->
//  myAssembly->Init(Doc);
  //di<<"This assembly has ";
  TCollection_AsciiString Entry;
  Entry=myAssembly->NbComponents( aLabel, getsubchilds);
  di << Entry.ToCString();
  //di<<" components\n";
  //di<<"This assembly has "<<myAssembly->NbComponents( aLabel, getsubchilds )<<" components\n";

  return 0;
}

static Standard_Integer addComponent (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=4) {
    di<<"Use: "<<argv[0]<<" DocName Label Shape \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[3]);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//  XCAFDoc_ShapeTool myAssembly->
//  myAssembly->Init(Doc);
  myAssembly->AddComponent(aLabel, aShape);
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aLabel, Entry);
  di << Entry.ToCString();
  
  return 0;
}

static Standard_Integer removeComponent (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" DocName Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//  XCAFDoc_ShapeTool myAssembly->
//  myAssembly->Init(Doc);
  myAssembly->RemoveComponent(aLabel);
  return 0;
}

static Standard_Integer getReferredShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" DocName Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel, RootLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//  XCAFDoc_ShapeTool myAssembly->
//  myAssembly->Init(Doc);
  myAssembly->GetReferredShape(aLabel, RootLabel);
  
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(RootLabel, Entry);
  //di<<"Label with Shape is ";
  di << Entry.ToCString();
  return 0;
}

static Standard_Integer getTopLevelShapes (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=2) {
    di<<"Use: "<<argv[0]<<" DocName \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_LabelSequence Labels;
  
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
//  XCAFDoc_ShapeTool myAssembly->
//  myAssembly->Init(Doc);
  myAssembly->GetShapes(Labels);
  TCollection_AsciiString Entry;
  if (Labels.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= Labels.Length(); i++) {
      aLabel = Labels.Value(i);
      TDF_Tool::Entry( aLabel, Entry);
      di << Entry.ToCString() << " ";
    }
  }
  return 0;
}

static Standard_Integer getFreeShapes (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc <2) {
    di<<"Use: "<<argv[0]<<" DocName [shape_prefix]\n";
    return 1;
  }
  
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_LabelSequence Labels;
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  STool->GetFreeShapes(Labels);
  if ( Labels.Length() <=0 ) {
    di << "Document " << argv[1] << " contain no shapes\n";
    return 0;
  }
  
  if ( argc ==2 ) {
    for ( Standard_Integer i = 1; i<= Labels.Length(); i++) {
      TCollection_AsciiString Entry;
      TDF_Tool::Entry( Labels.Value(i), Entry);
      di << Entry.ToCString() << " ";
    }
  }
  else if ( Labels.Length() ==1 ) {
    TopoDS_Shape S = STool->GetShape ( Labels.Value(1) );
    DBRep::Set ( argv[2], S );
    di << argv[2];
  }
  else {
    for ( Standard_Integer i = 1; i<= Labels.Length(); i++) {
      TopoDS_Shape S = STool->GetShape ( Labels.Value(i) );
      char string[260];
      Sprintf ( string, "%s_%d", argv[2], i );
      DBRep::Set ( string, S );
      di << string << " ";
    }
  }
  return 0;
}

static Standard_Integer getOneShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" shape DocName \n";
    return 1;
  }
  
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[2], Doc);
  if ( Doc.IsNull() ) { di << argv[2] << " is not a document\n"; return 1; }

  TDF_LabelSequence Labels;
  Handle(XCAFDoc_ShapeTool) STool = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  STool->GetFreeShapes(Labels);
  if ( Labels.Length() <=0 ) {
    di << "Document " << argv[2] << " contain no shapes\n";
    return 0;
  }
  
  if ( Labels.Length() ==1 ) {
    TopoDS_Shape S = STool->GetShape ( Labels.Value(1) );
    DBRep::Set ( argv[1], S );
  }
  else {
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound ( C );
    for ( Standard_Integer i = 1; i<= Labels.Length(); i++) {
      TopoDS_Shape S = STool->GetShape ( Labels.Value(i) );
      B.Add ( C, S );
    }
    DBRep::Set ( argv[1], C );
  }
  di << argv[1];
  return 0;
}

//=======================================================================
//function : XDumpLocation
//purpose  : Dump Transformation() of XCAFDoc_Location attribute
//=======================================================================
static Standard_Integer XDumpLocation (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Use: " << argv[0] << " Doc Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if (Doc.IsNull())
  {
    di << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);

  Handle(XCAFDoc_Location) aLoc;
  if (!aLabel.FindAttribute(XCAFDoc_Location::GetID(), aLoc))
  {
    di << "Label " << argv[2] << " doesn't contain XCAFDoc_Location attribute\n";
    return 1;
  }
  
  TopLoc_Location aTopLoc = aLoc->Get();
  gp_Trsf aTrsf = aTopLoc.Transformation();

  di << "Transformation (3 rows * 4 columns matrix):";
  for (int i = 1; i <= 3; i++) // row number
  {
    di << " (";
    for (int j = 1; j <= 4; j++) // column number
    {
      if (j > 1) di << ",";
      di << TCollection_AsciiString(aTrsf.Value(i,j)).ToCString();
    }
    di << ")";
  }

  return 0;
}

static Standard_Integer setSHUO (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use: " << argv[0] << " Doc UU_Label NU_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  
  TDF_LabelSequence aLabSeq;
  for (Standard_Integer i = 3; i <= argc; i++) {
    TDF_Label L;
    TDF_Tool::Label(Doc->GetData(), argv[i - 1], L);
    if (!L.IsNull())
      aLabSeq.Append( L );
    else
      di << argv[i - 1] << " is null label"  << "\n";
  }
  if (aLabSeq.Length() < 2) {
    di << "Error: couldnot set SHUO between on less then 2 labels\n";
  }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  Handle(XCAFDoc_GraphNode) aMainSHUO;
  myAssembly->SetSHUO( aLabSeq, aMainSHUO );
  if (aMainSHUO.IsNull()) {
    di << "Error: cannot set the SHUO\n";
    return 1;
  }
    
  return 0;
}

static Standard_Integer getSHUOUpperUsage (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc NU_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TDF_Label NL;
  TDF_Tool::Label(Doc->GetData(), argv[2], NL);
  if (NL.IsNull()) {
    di << argv[2] << " is null label"  << "\n";
    return 1;
  }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_LabelSequence labseq;
  myAssembly->GetSHUOUpperUsage( NL, labseq );
  TCollection_AsciiString Entry;
  if (labseq.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= labseq.Length(); i++) {
      TDF_Label aLabel = labseq.Value(i);
      TDF_Tool::Entry( aLabel, Entry);
      di << Entry.ToCString() << " ";
    }
  }
  return 0;
}

static Standard_Integer getSHUONextUsage (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc UU_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TDF_Label UL;
  TDF_Tool::Label(Doc->GetData(), argv[2], UL);
  if (UL.IsNull()) {
    di << argv[2] << " is null label"  << "\n";
    return 1;
  }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_LabelSequence labseq;
  myAssembly->GetSHUONextUsage( UL, labseq );
  TCollection_AsciiString Entry;
  if (labseq.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= labseq.Length(); i++) {
      TDF_Label aLabel = labseq.Value(i);
      TDF_Tool::Entry( aLabel, Entry);
      di << Entry.ToCString() << " ";
    }
  }
  return 0;
}

static Standard_Integer removeSHUO (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc SHUOComponent_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TDF_Label UL;
  TDF_Tool::Label(Doc->GetData(), argv[2], UL);
  if (UL.IsNull()) {
    di << argv[2] << " is null label"  << "\n";
    return 1;
  }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  myAssembly->RemoveSHUO( UL );
    
  return 0;
}

static Standard_Integer hasSHUO (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc SHUO_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TDF_Label UL;
  TDF_Tool::Label(Doc->GetData(), argv[2], UL);
  if (UL.IsNull()) {
    di << argv[2] << " is null label"  << "\n";
    return 1;
  }
  Handle(XCAFDoc_GraphNode) anAttrSHUO;
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  if (myAssembly->GetSHUO( UL, anAttrSHUO ))
    di << 1; 
  else
    di << 0;
  
  return 0;
}

static Standard_Integer getAllSHUO (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc SHUO_Label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_Label UL;
  TDF_Tool::Label(Doc->GetData(), argv[2], UL);
  if (UL.IsNull()) {
    di << argv[2] << " is null label"  << "\n";
    return 1;
  }
  TDF_AttributeSequence SHUOAttrs;
  myAssembly->GetAllComponentSHUO( UL, SHUOAttrs );
  TCollection_AsciiString Entry;
  if (SHUOAttrs.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= SHUOAttrs.Length(); i++) {
      TDF_Label aLabel = SHUOAttrs.Value(i)->Label();
      TDF_Tool::Entry( aLabel, Entry);
      di << Entry.ToCString() << " ";
    }
  }
  return 0;
}

static Standard_Integer findComponent (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc shape \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);
  TDF_LabelSequence labseq;
  myAssembly->FindComponent( aShape, labseq );
  TCollection_AsciiString Entry;
  if (labseq.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= labseq.Length(); i++) {
      TDF_Label aLabel = labseq.Value(i);
      TDF_Tool::Entry( aLabel, Entry);
      di << Entry.ToCString() << " ";
    }
  }
  return 0;
}

static Standard_Integer getStyledComponent (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use: " << argv[0] << " Doc res SHUO_label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TopoDS_Shape aShape;
  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[3], aLabel);
  Handle(XCAFDoc_GraphNode) SHUO;
  if (myAssembly->GetSHUO( aLabel, SHUO ))
    aShape = myAssembly->GetSHUOInstance( SHUO );
  
  if (aShape.IsNull()) {
    di << "cannot get component\n";
    return 1;
  }
  DBRep::Set ( argv[2], aShape );
  di << argv[2];
  return 0;
}

static Standard_Integer getAllStyledComponents (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use: " << argv[0] << " Doc res SHUO_label \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TopTools_SequenceOfShape aShapes;
  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[3], aLabel);
  Handle(XCAFDoc_GraphNode) SHUO;
  if (myAssembly->GetSHUO( aLabel, SHUO ))
    if (myAssembly->GetAllSHUOInstances(SHUO, aShapes)) {
      TopoDS_Compound aShape;
      BRep_Builder B;
      B.MakeCompound(aShape);
      for (Standard_Integer jj = 1; jj <= aShapes.Length(); jj++) {
        TopoDS_Shape aCurShape = aShapes.Value(jj);
        B.Add( aShape, aCurShape );
      }
      DBRep::Set ( argv[2], aShape );
      di << argv[2];
    }
      
  return 0;
}

static Standard_Integer findSHUO (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    di << "Use: " << argv[0] << " Doc labels \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TDF_LabelSequence aLabSeq;
  for (Standard_Integer i = 3; i <= argc; i++) {
    TDF_Label L;
    TDF_Tool::Label(Doc->GetData(), argv[i - 1], L);
    if (!L.IsNull())
      aLabSeq.Append( L );
    else
      di << argv[i - 1] << " is null label"  << "\n";
  }
  if (aLabSeq.Length() < 2) {
    di << "Error: couldnot find SHUO between on less then 2 labels\n";
  }
  Handle(XCAFDoc_GraphNode) SHUO;
  myAssembly->FindSHUO( aLabSeq, SHUO );
  if (SHUO.IsNull()) {
    di << "cannot find SHUO\n";
    return 1;
  }
  TCollection_AsciiString Entry;
  TDF_Tool::Entry( SHUO->Label(), Entry);
  di << Entry.ToCString() << " ";
  
  return 0;
}

static Standard_Integer setStyledComponent (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Use: " << argv[0] << " Doc shape \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ShapeTool) myAssembly = XCAFDoc_DocumentTool::ShapeTool(Doc->Main());
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);
  if (aShape.IsNull()) {
    di << "Shape " << argv[2] << " is null\n";
    return 1;
  }
  Handle(XCAFDoc_GraphNode) aSHUOAttr;
  aSHUOAttr = myAssembly->SetInstanceSHUO( aShape );
  if (aSHUOAttr.IsNull()) {
    di << "Error: cannot set a SHUO structure for indicated component"  << "\n";
    return 1;
  }
  TCollection_AsciiString Entry;
  TDF_Tool::Entry( aSHUOAttr->Label(), Entry);
  di << Entry.ToCString() << " ";
  
  return 0;
}

static Standard_Integer updateAssemblies(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Use: " << argv[0] << " Doc\n";
    return 1;
  }

  // Get XDE document
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if ( aDoc.IsNull() )
    return 1;

  // Get XDE shape tool
  Handle(XCAFDoc_ShapeTool)
    aShapeTool = XCAFDoc_DocumentTool::ShapeTool( aDoc->Main() );

  // Update assemblies
  aShapeTool->UpdateAssemblies();

  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_Shapes::InitCommands(Draw_Interpretor& di) 
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  //=====================================
  // Work with shapes
  //=====================================  
  
  Standard_CString g = "XDE shape's commands";

  di.Add ("XAddShape","Doc Shape [makeAssembly = 1]\t: Add shape (or assembly) to Document",
                   __FILE__, addShape, g);

  di.Add ("XNewShape","Doc \t: Create new empty top-level shape",
                   __FILE__, newShape, g);

  di.Add ("XSetShape","Doc Label Shape \t: Set shape at indicated label",
                   __FILE__, setShape, g);

  di.Add ("XGetShape","Result Doc Label \t: Put shape from tree to Result",
                   __FILE__, getShape, g);

  di.Add ("XRemoveShape","Doc Label \t: Remove shape from document",
                   __FILE__, removeShape, g);

  di.Add ("XFindShape","Doc Shape \t: Find and print label with indicated top-level shape",
                   __FILE__, findShape, g);

  di.Add ("XLabelInfo","Doc Label \t: Print information about object at following label",
                   __FILE__, labelInfo, g);

  di.Add ("XGetUsers","Doc Label [withSubChilds(int)] \t: Print number of assemblies that use shape at following label",
                   __FILE__, getUsers, g);

  di.Add ("XNbComponents","Doc Label [withSubChilds(int)] \t: Print number of component of assembly ",
                   __FILE__, nbComponents, g);

  di.Add ("XAddComponent","Doc Label Shape \t: Add component shape to assembly",
                   __FILE__, addComponent, g);

  di.Add ("XRemoveComponent","Doc Label \t: Remove component from components label",
                   __FILE__, removeComponent, g);

  di.Add ("XGetReferredShape","Doc Label \t: Print label, that contain a top-level shape, that corresponds shape at following label",
                   __FILE__, getReferredShape, g);

  di.Add ("XGetTopLevelShapes","Doc \t: Print labels, that contain a top-level shapes",
                   __FILE__, getTopLevelShapes, g);

  di.Add ("XGetFreeShapes","Doc [shape_prefix]\t: Print labels or create DRAW shapes for all free shapes in the Doc",
                   __FILE__, getFreeShapes, g);

  di.Add ("XGetOneShape","shape Doc \t: Put all free shapes of the Doc into signle DRAW shape",
                   __FILE__, getOneShape, g);

  di.Add ("XDumpLocation","Doc Label \t: Dump Transformation() of XCAFDoc_Location attribute",
                   __FILE__, XDumpLocation, g);

  di.Add ("XSetSHUO","Doc UU_Label [ multi-level labels ] NU_Label \t: sets the SHUO structure between UpperUsage and NextUsage",
                   __FILE__, setSHUO, g);

  di.Add ("XGetUU_SHUO","Doc NU_Label \t: prints the UpperUsages of indicated NextUsage",
                   __FILE__, getSHUOUpperUsage, g);

  di.Add ("XGetNU_SHUO","Doc UU_Label \t: prints the NextUsages of indicated UpperUsage",
                   __FILE__, getSHUONextUsage, g);

  di.Add ("XRemoveSHUO","Doc SHUO_Label \t: remove SHUO of indicated component",
                   __FILE__, removeSHUO, g);

  di.Add ("XIsHasSHUO","Doc SHUO_Label \t: remove SHUO of indicated component",
                   __FILE__, hasSHUO, g);

  di.Add ("XGetAllSHUO","Doc Comp_Label \t: remove SHUO of indicated component",
                   __FILE__, getAllSHUO, g);

  di.Add ("XFindComponent","Doc Shape \t: prints sequence of labels of assembly path",
                   __FILE__, findComponent, g);
  
  di.Add ("XGetSHUOInstance","Doc res SHUO_Label \t: returns SHUO_styled shape",
                   __FILE__, getStyledComponent, g);
  
  di.Add ("XGetAllSHUOInstances","Doc res SHUO_Label \t: returns SHUO_styled shapes as compound",
                   __FILE__, getAllStyledComponents, g);
  
  di.Add ("XFindSHUO","Doc labels of SHUO structure \t: prints label of SHUO that found by labels structure",
                   __FILE__, findSHUO, g);

  di.Add ("XSetInstanceSHUO","Doc shape \t: sets the SHUO structure for indicated component",
                   __FILE__, setStyledComponent, g);
  
  di.Add ("XUpdateAssemblies","Doc \t: updates assembly compounds",
                   __FILE__, updateAssemblies, g);
}
