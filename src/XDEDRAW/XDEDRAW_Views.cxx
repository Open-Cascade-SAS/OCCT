// Created on: 2016-11-22
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <XDEDRAW_Views.hxx>

#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <Geom_Plane.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_View.hxx>
#include <XCAFDoc_ViewTool.hxx>
#include <XCAFView_Object.hxx>

//=======================================================================
//function : setView
//purpose  : 
//=======================================================================
static Standard_Integer setView(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XSetView Doc shape_label1 ... shape_labelN gdt_label1 ... gdt_labelN\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());
  Handle(XCAFDoc_DimTolTool) aDimTolTool = XCAFDoc_DocumentTool::DimTolTool(aDoc->Main());
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDoc->Main());

  TDF_LabelSequence aShapes, aGDTs;
  for (Standard_Integer i = 2; i < argc; i++) {
    TDF_Label aLabel;
    TDF_Tool::Label(aDoc->GetData(), argv[i], aLabel);
    if (aLabel.IsNull())
      continue;
    if (aShapeTool->IsShape(aLabel))
      aShapes.Append(aLabel);
    else if (aDimTolTool->IsDatum(aLabel) || aDimTolTool->IsDimension(aLabel) || aDimTolTool->IsGeomTolerance(aLabel))
      aGDTs.Append(aLabel);
  }

  if (aShapes.Length() == 0 && aGDTs.Length() == 0)
    return 1;

  TDF_Label aViewL = aViewTool->AddView();
  aViewTool->SetView(aShapes, aGDTs, aViewL);
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(aViewL, anEntry);
  di << anEntry << "\n";
  return 0;
}

//=======================================================================
//function : removeView
//purpose  : 
//=======================================================================
static Standard_Integer removeView(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XRemoveView Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  aViewTool->RemoveView(aLabel);
  return 0;
}


//=======================================================================
//function : setClippingPlanes
//purpose  : 
//=======================================================================
static Standard_Integer setClippingPlanes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XSetClippingPlanes Doc view_label plane_label1 ... plane_labelN";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());
  Handle(XCAFDoc_ClippingPlaneTool) aCPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(aDoc->Main());

  TDF_LabelSequence aCPlanes;
  for (Standard_Integer i = 3; i < argc; i++) {
    TDF_Label aLabel;
    TDF_Tool::Label(aDoc->GetData(), argv[i], aLabel);
    if (aLabel.IsNull())
      continue;
    if (aCPlaneTool->IsClippingPlane(aLabel))
      aCPlanes.Append(aLabel);
  }

  if (aCPlanes.Length() == 0)
    return 1;

  TDF_Label aViewL;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aViewL);
  aViewTool->SetClippingPlanes(aCPlanes, aViewL);
  return 0;
}

//=======================================================================
//function : isView
//purpose  : 
//=======================================================================
static Standard_Integer isView(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XIsView Doc Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "Invalid label\n";
    return 1;
  }

  if (aViewTool->IsView(aLabel))
    di << "1";
  else
    di << "0";

  return 0;
}

//=======================================================================
//function : getRefShapes
//purpose  : 
//=======================================================================
static Standard_Integer getRefShapes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewShapes Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull() || !aViewTool->IsView(aLabel))
  {
    di << "Invalid label\n";
    return 1;
  }

  TDF_LabelSequence aShapes;
  aViewTool->GetRefShapeLabel(aLabel, aShapes);
  for (Standard_Integer i = 1; i <= aShapes.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aShapes.Value(i), anEntry);
    di << anEntry << " ";
  }
  return 0;
}

//=======================================================================
//function : getRefGDTs
//purpose  : 
//=======================================================================
static Standard_Integer getRefGDTs(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewGDTs Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull() || !aViewTool->IsView(aLabel))
  {
    di << "Invalid label\n";
    return 1;
  }

  TDF_LabelSequence aGDTs;
  aViewTool->GetRefGDTLabel(aLabel, aGDTs);
  if (aGDTs.Length() == 0) {
    di << "No GDTs in the given View\n";
  }
  for (Standard_Integer i = 1; i <= aGDTs.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aGDTs.Value(i), anEntry);
    di << anEntry << " ";
  }
  return 0;
}

//=======================================================================
//function : getRefClippingPlanes
//purpose  : 
//=======================================================================
static Standard_Integer getRefClippingPlanes(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewClippingPlanes Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull() || !aViewTool->IsView(aLabel))
  {
    di << "Invalid label\n";
    return 1;
  }

  TDF_LabelSequence aCPlanes;
  aViewTool->GetRefClippingPlaneLabel(aLabel, aCPlanes);
  if (aCPlanes.Length() == 0) {
    di << "No Clipping Planes in the given View\n";
  }
  for (Standard_Integer i = 1; i <= aCPlanes.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aCPlanes.Value(i), anEntry);
    di << anEntry << " ";
  }
  return 0;
}

//=======================================================================
//function : setName
//purpose  : 
//=======================================================================
static Standard_Integer setName(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewName Doc ViewLabel name\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetName(new TCollection_HAsciiString(argv[3]));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getName
//purpose  : 
//=======================================================================
static Standard_Integer getName(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewName Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    di << aView->GetObject()->Name()->String();
  }
  return 0;
}

//=======================================================================
//function : setType
//purpose  : 
//=======================================================================
static Standard_Integer setType(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewType Doc ViewLabel type (central/parallel/no_camera)\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    XCAFView_ProjectionType aType = XCAFView_ProjectionType_NoCamera;
    if (argv[3][0] == 'c')
      aType = XCAFView_ProjectionType_Central;
    else if (argv[3][0] == 'p')
      aType = XCAFView_ProjectionType_Parallel;
    anObj->SetType(aType);
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getType
//purpose  : 
//=======================================================================
static Standard_Integer getType(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewType Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    XCAFView_ProjectionType aType = aView->GetObject()->Type();
    switch (aType) {
      case XCAFView_ProjectionType_NoCamera:
      di << "no_camera";
      break;
      case XCAFView_ProjectionType_Central:
      di << "central";
      break;
      case XCAFView_ProjectionType_Parallel:
      di << "parallel";
      break;
    }
  }
  return 0;
}

//=======================================================================
//function : setProjectionPont
//purpose  : 
//=======================================================================
static Standard_Integer setProjectionPoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di << "Use: XSetViewProjectionPoint Doc ViewLabel x y z\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetProjectionPoint(gp_Pnt(Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5])));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getProjectionPoint
//purpose  : 
//=======================================================================
static Standard_Integer getProjectionPoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewProjectionPoint Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    gp_Pnt aPnt = aView->GetObject()->ProjectionPoint();
    di << aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z();
  }
  return 0;
}

//=======================================================================
//function : setViewDir
//purpose  : 
//=======================================================================
static Standard_Integer setViewDir(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di << "Use: XSetViewDir Doc ViewLabel x y z\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetViewDirection(gp_Dir(Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5])));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getViewDir
//purpose  : 
//=======================================================================
static Standard_Integer getViewDir(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewDir Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    gp_Dir aDir = aView->GetObject()->ViewDirection();
    di << aDir.X() << " " << aDir.Y() << " " << aDir.Z();
  }
  return 0;
}

//=======================================================================
//function : setUpDir
//purpose  : 
//=======================================================================
static Standard_Integer setUpDir(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di << "Use: XSetViewUpDir Doc ViewLabel x y z\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetUpDirection(gp_Dir(Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5])));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getUpDir
//purpose  : 
//=======================================================================
static Standard_Integer getUpDir(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewUpDir Doc ViewLabel\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    gp_Dir aDir = aView->GetObject()->UpDirection();
    di << aDir.X() << " " << aDir.Y() << " " << aDir.Z();
  }
  return 0;
}

//=======================================================================
//function : setZoomFactor
//purpose  : 
//=======================================================================
static Standard_Integer setZoomFactor(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewZoom Doc View_Label value\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetZoomFactor(Draw::Atof(argv[3]));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getZoomFactor
//purpose  : 
//=======================================================================
static Standard_Integer getZoomFactor(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewZoom Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    di << aView->GetObject()->ZoomFactor();
  }
  return 0;
}

//=======================================================================
//function : setWindowSize
//purpose  : 
//=======================================================================
static Standard_Integer setWindowSize(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XSetViewWindowSize Doc ViewLabel width height\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetWindowHorizontalSize(Draw::Atof(argv[3]));
    anObj->SetWindowVerticalSize(Draw::Atof(argv[4]));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getWindowSize
//purpose  : 
//=======================================================================
static Standard_Integer getWindowSize(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XSetViewWindowSize Doc Dim_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    di << "width " << aView->GetObject()->WindowHorizontalSize();
    di << " height " << aView->GetObject()->WindowVerticalSize();
  }
  return 0;
}

//=======================================================================
//function : setFrontPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer setFrontPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewFrontPlaneDistance Doc View_Label value\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetFrontPlaneDistance(Draw::Atof(argv[3]));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : unsetFrontPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer unsetFrontPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XUnsetViewFrontPlaneDistance Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->UnsetFrontPlaneClipping();
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getFrontPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer getFrontPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewFrontPlaneDistance Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    if (aView->GetObject()->HasFrontPlaneClipping())
      di << aView->GetObject()->FrontPlaneDistance();
    else
      di << "View has not front plane clipping\n";
  }
  return 0;
}

//=======================================================================
//function : setBackPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer setBackPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewBackPlaneDistance Doc View_Label value\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetBackPlaneDistance(Draw::Atof(argv[3]));
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : unsetBackPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer unsetBackPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XUnsetViewBackPlaneDistance Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->UnsetBackPlaneClipping();
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getBackPlaneDistance
//purpose  : 
//=======================================================================
static Standard_Integer getBackPlaneDistance(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewFrontPlaneDistance Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    if (aView->GetObject()->HasBackPlaneClipping())
      di << aView->GetObject()->BackPlaneDistance();
    else
      di << "View has not back plane clipping\n";
  }
  return 0;
}

//=======================================================================
//function : setViewVolumeSidesClipping
//purpose  : 
//=======================================================================
static Standard_Integer setViewVolumeSidesClipping(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di << "Use: XSetViewVolumeSidesClipping Doc View_Label value\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    Handle(XCAFView_Object) anObj = aView->GetObject();
    anObj->SetViewVolumeSidesClipping((Draw::Atoi(argv[3])) == 1);
    aView->SetObject(anObj);
  }
  return 0;
}

//=======================================================================
//function : getViewVolumeSidesClipping
//purpose  : 
//=======================================================================
static Standard_Integer getViewVolumeSidesClipping(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetViewVolumeSidesClipping Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Handle(XCAFDoc_View) aView;
  if (aLabel.FindAttribute(XCAFDoc_View::GetID(), aView))
  {
    di << aView->GetObject()->HasViewVolumeSidesClipping();
  }
  return 0;
}

//=======================================================================
//function : dump
//purpose  : 
//=======================================================================
static Standard_Integer dump(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XDumpView Doc View_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ViewTool) aViewTool = XCAFDoc_DocumentTool::ViewTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_View) aView;
  if (aLabel.IsNull() || !(aLabel.FindAttribute(XCAFDoc_View::GetID(), aView)))
  {
    di << "View " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }

  TDF_LabelSequence aShapes;
  aViewTool->GetRefShapeLabel(aLabel, aShapes);
  di << "Reference shapes: ";
  for (Standard_Integer i = 1; i <= aShapes.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aShapes.Value(i), anEntry);
    di << anEntry << " ";
  }
  di << "\n";

  TDF_LabelSequence aGDTs;
  aViewTool->GetRefGDTLabel(aLabel, aGDTs);
  di << "Reference GD&Ts: ";
  for (Standard_Integer i = 1; i <= aGDTs.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aGDTs.Value(i), anEntry);
    di << anEntry << " ";
  }
  di << "\n";

  TDF_LabelSequence aCPlanes;
  aViewTool->GetRefClippingPlaneLabel(aLabel, aCPlanes);
  di << "Reference Clipping Planes: ";
  for (Standard_Integer i = 1; i <= aCPlanes.Length(); i++) {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aCPlanes.Value(i), anEntry);
    di << anEntry << " ";
  }
  di << "\n";

  di << "Name: " << aView->GetObject()->Name()->String() << "\n";

  XCAFView_ProjectionType aType = aView->GetObject()->Type();
  switch (aType) {
    case XCAFView_ProjectionType_NoCamera:
    di << "Type: no_camera\n";
    break;
    case XCAFView_ProjectionType_Central:
    di << "Type: central\n";
    break;
    case XCAFView_ProjectionType_Parallel:
    di << "Type: parallel\n";
    break;
  }

  gp_Pnt aPnt = aView->GetObject()->ProjectionPoint();
  di << "Projection point: "<< aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z() << "\n";

  gp_Dir aDir = aView->GetObject()->ViewDirection();
  di << "View Direction: " << aDir.X() << " " << aDir.Y() << " " << aDir.Z() << "\n";

  aDir = aView->GetObject()->UpDirection();
  di << "Up Direction: " << aDir.X() << " " << aDir.Y() << " " << aDir.Z() << "\n";

  di << "Zoom factor: " << aView->GetObject()->ZoomFactor() << "\n";

  di << "Window Size: width " << aView->GetObject()->WindowHorizontalSize() << ", " << " height " << aView->GetObject()->WindowVerticalSize() << "\n";

  if (aView->GetObject()->HasFrontPlaneClipping())
    di << "Front Plane Distance: " << aView->GetObject()->FrontPlaneDistance() << "\n";
  else
    di << "No Front Plane\n";

  if (aView->GetObject()->HasFrontPlaneClipping())
    di << "Front Back Distance: " << aView->GetObject()->BackPlaneDistance() << "\n";
  else
    di << "No Back Plane\n";

  di << "View VolumeSized Clipping: " << aView->GetObject()->HasViewVolumeSidesClipping() << "\n";

  return 0;
}

//=======================================================================
//function : addClippingPlane
//purpose  : 
//=======================================================================
static Standard_Integer addClippingPlane(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Use: XAddClippingPlane Doc plane name capping[0/1]";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ClippingPlaneTool) aCPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(aDoc->Main());
  gp_Pln aPlane;
  Handle(Geom_Plane) aSurf = Handle(Geom_Plane)::DownCast(DrawTrSurf::GetSurface(argv[2]));
  if (aSurf.IsNull()) {
    cout << argv[2] << " is not a plane" << endl;
    return 1;
  }
  aPlane = aSurf->Pln();
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString(argv[3]);
  Standard_Boolean aCapping = (argv[4][0] == '1');

  TDF_Label aCPlaneL = aCPlaneTool->AddClippingPlane(aPlane, aName, aCapping);
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(aCPlaneL, anEntry);
  di << anEntry << "\n";
  return 0;
}

//=======================================================================
//function : getClippingPlane
//purpose  : 
//=======================================================================
static Standard_Integer getClippingPlane(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetClippingPlane Doc ClippingPlane_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ClippingPlaneTool) aClippingPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "ClippingPlane " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  gp_Pln aPlane;
  Handle(TCollection_HAsciiString) aName;
  Standard_Boolean aCapping;
  aClippingPlaneTool->GetClippingPlane(aLabel, aPlane, aName, aCapping);
  Handle(Geom_Plane) aCPlane = new Geom_Plane(aPlane);
  DrawTrSurf::Set(aName->ToCString(), aCPlane);
  di << aName->ToCString();
  return 0;
}

//=======================================================================
//function : removeClippingPlane
//purpose  : 
//=======================================================================
static Standard_Integer removeClippingPlane(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XRemoveClippingPlane Doc ClippingPlane_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ClippingPlaneTool) aClippingPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "ClippingPlane " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  Standard_Boolean isRemoved = aClippingPlaneTool->RemoveClippingPlane(aLabel);
  if (isRemoved)
    di << "removed\n";
  else
    di << "clipping plane is not free, not removed\n";
  return 0;
}

//=======================================================================
//function : getClippingPlaneCapping
//purpose  : 
//=======================================================================
static Standard_Integer getClippingPlaneCapping(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di << "Use: XGetClippingPlaneCapping Doc ClippingPlane_Label\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument(argv[1], aDoc);
  if (aDoc.IsNull()) {
    di << argv[1] << " is not a document\n";
    return 1;
  }
  Handle(XCAFDoc_ClippingPlaneTool) aClippingPlaneTool = XCAFDoc_DocumentTool::ClippingPlaneTool(aDoc->Main());

  TDF_Label aLabel;
  TDF_Tool::Label(aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    di << "ClippingPlane " << argv[2] << " is absent in " << argv[1] << "\n";
    return 1;
  }
  di << aClippingPlaneTool->GetCapping(aLabel);
  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_Views::InitCommands(Draw_Interpretor& di) 
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;
  
  Standard_CString g = "XDE Views commands";

  di.Add("XSetView", "XSetView Doc shape_label1 ... shape_labelN gdt_label1 ... gdt_labelN",
    __FILE__, setView, g);

  di.Add("XRemoveView", "XRemoveView Doc ViewLabel",
    __FILE__, removeView, g);

  di.Add("XSetClippingPlanes", "XSetView Doc view_plane plane_label1 ... plane_labelN",
    __FILE__, setClippingPlanes, g);

  di.Add("XIsView", "XIsView Doc Label",
    __FILE__, isView, g);

  di.Add("XGetViewShapes", "XGetViewShapes Doc ViewLabel" "Return labels of reference shapes",
    __FILE__, getRefShapes, g);

  di.Add("XGetViewGDTs", "XGetViewGDTs Doc ViewLabel" "Return labels of reference GDTs",
    __FILE__, getRefGDTs, g);

  di.Add("XGetViewClippingPlanes", "XGetViewClippingPlanes Doc ViewLabel" "Return labels of reference Clipping Planes",
    __FILE__, getRefClippingPlanes, g);

  di.Add("XSetViewName", "XSetViewName Doc ViewLabel name",
    __FILE__, setName, g);

  di.Add("XGetViewName", "XGetViewName Doc ViewLabel",
    __FILE__, getName, g);

  di.Add("XSetViewType", "XSetViewType Doc ViewLabel type (central/parallel/no_camera)",
    __FILE__, setType, g);

  di.Add("XGetViewType", "XGetViewType Doc ViewLabel",
    __FILE__, getType, g);

  di.Add("XSetViewProjectionPoint", "XSetViewProjectionPoint Doc ViewLabel x y z",
    __FILE__, setProjectionPoint, g);

  di.Add("XGetViewProjectionPoint", "XGetViewProjectionPoint Doc ViewLabel",
    __FILE__, getProjectionPoint, g);

  di.Add("XSetViewDir", "XSetViewDir Doc ViewLabel x y z",
    __FILE__, setViewDir, g);

  di.Add("XGetViewDir", "XGetViewDir Doc ViewLabel",
    __FILE__, getViewDir, g);

  di.Add("XSetViewUpDir", "XSetViewUpDir Doc ViewLabel x y z",
    __FILE__, setUpDir, g);

  di.Add("XGetViewUpDir", "XGetViewUpDir Doc ViewLabel",
    __FILE__, getUpDir, g);

  di.Add("XSetViewZoom", "XSetViewZoom Doc ViewLabel zoom_factor",
    __FILE__, setZoomFactor, g);

  di.Add("XGetViewZoom", "XGetViewZoom Doc ViewLabel",
    __FILE__, getZoomFactor, g);

  di.Add("XSetViewWindowSize", "XSetViewWindowSize Doc ViewLabel width height",
    __FILE__, setWindowSize, g);

  di.Add("XGetViewWindowSize", "XGetViewWindowSize Doc ViewLabel",
    __FILE__, getWindowSize, g);

  di.Add("XSetViewFrontPlaneDistance", "XSetViewFrontPlaneDistance Doc ViewLabel distance",
    __FILE__, setFrontPlaneDistance, g);
    
  di.Add("XUnsetViewFrontPlaneDistance", "XSetViewFrontPlaneDistance Doc ViewLabel",
    __FILE__, unsetFrontPlaneDistance, g);

  di.Add("XGetViewFrontPlaneDistance", "XGetViewFrontPlaneDistance Doc ViewLabel",
    __FILE__, getFrontPlaneDistance, g);
    
  di.Add("XSetViewBackPlaneDistance", "XSetViewBackPlaneDistance Doc ViewLabel distance",
    __FILE__, setBackPlaneDistance, g);
    
  di.Add("XUnsetViewBackPlaneDistance", "XUnsetViewBackPlaneDistance Doc ViewLabel",
    __FILE__, unsetBackPlaneDistance, g);

  di.Add("XGetViewBackPlaneDistance", "XGetViewBackPlaneDistance Doc ViewLabel",
    __FILE__, getBackPlaneDistance, g);
    
  di.Add("XSetViewVolumeSidesClipping", "XSetViewVolumeSidesClipping Doc ViewLabel value(0 - unset, 1- set)",
    __FILE__, setViewVolumeSidesClipping, g);
    
  di.Add("XGetViewVolumeSidesClipping", "XGetViewVolumeSidesClipping Doc ViewLabel",
    __FILE__, getViewVolumeSidesClipping, g);

  di.Add("XDumpView", "XDumpView Doc ViewLabel",
    __FILE__, dump, g);

  di.Add("XAddClippingPlane", "XAddClippingPlane Doc plane name capping[0/1]",
    __FILE__, addClippingPlane, g);

  di.Add("XGetClippingPlaneCapping", "XGetClippingPlaneCapping Doc ClippingPlane_Label",
    __FILE__, getClippingPlaneCapping, g);

  di.Add("XGetClippingPlane", "XGetClippingPlane Doc ClippingPlane_Label",
    __FILE__, getClippingPlane, g);

  di.Add("XRemoveClippingPlane", "XRemoveClippingPlane Doc ClippingPlane_Label",
    __FILE__, removeClippingPlane, g);
}
