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


#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <Draw.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XDEDRAW_Colors.hxx>

//=======================================================================
// Section: Work with colors
//=======================================================================
static Standard_Integer setColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 6) {
    di<<"Use: "<<argv[0]<<" Doc {Label|Shape} R G B [alpha] [curve|surf]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  
  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Quantity_Color Col ( Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5]), Quantity_TOC_RGB );

  Quantity_ColorRGBA aColRGBA;
  aColRGBA.SetRGB(Col);
  if (argc > 6 && (argv[6][0] != 's' && argv[6][0] != 'c')) {
    aColRGBA.SetAlpha((Standard_ShortReal)(Draw::Atof(argv[6])));
  }
  
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  XCAFDoc_ColorType ctype = XCAFDoc_ColorGen;
  if (argc > 6) {
    if (argv[argc - 1][0] == 's')
      ctype = XCAFDoc_ColorSurf;
    else if (argv[argc - 1][0] == 'c')
      ctype = XCAFDoc_ColorCurv;
  }

  if ( !aLabel.IsNull() ) {
    myColors->SetColor(aLabel, aColRGBA, ctype);
  }
  else {
    TopoDS_Shape aShape= DBRep::Get(argv[2]);
    if ( !aShape.IsNull() ) {
      myColors->SetColor(aShape, aColRGBA, ctype);
    }
  }
  return 0;
}

static Standard_Integer getColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" Doc Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  Quantity_ColorRGBA col;
  if ( !myColors->GetColor(aLabel, col) ) return 0;
  
  if ((1 - col.Alpha()) < Precision::Confusion())
    di << col.GetRGB().StringName(col.GetRGB().Name());
  else
    di << col.GetRGB().StringName ( col.GetRGB().Name() ) << " (" << col.Alpha() << ")";
   
  return 0;
}

static Standard_Integer getShapeColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3) {
    di<<"Use: "<<argv[0]<<" Doc Label [curve|surf]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) {
    di << " no such label in document\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  const XCAFDoc_ColorType ctype = ( argc <= 3 ? XCAFDoc_ColorGen : ( argv[3][0] == 's' ? XCAFDoc_ColorSurf : XCAFDoc_ColorCurv ) );

  Quantity_ColorRGBA col;
  if ( !myColors->GetColor(aLabel, ctype, col) ) return 0;

  if ((1 - col.Alpha()) < Precision::Confusion())
    di << col.GetRGB().StringName(col.GetRGB().Name());
  else
    di << col.GetRGB().StringName(col.GetRGB().Name()) << " (" << col.Alpha() << ")";

  return 0;
}

static Standard_Integer getAllColors (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=2) {
    di<<"Use: "<<argv[0]<<" Doc \n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  Quantity_ColorRGBA col;
  TDF_LabelSequence Labels;
  myColors->GetColors(Labels);
  if (Labels.Length() >= 1) {
    for ( Standard_Integer i = 1; i<= Labels.Length(); i++) {
      aLabel = Labels.Value(i);
      if ( !myColors->GetColor(aLabel, col) ) continue;
      if ((1 - col.Alpha()) < Precision::Confusion())
        di << col.GetRGB().StringName(col.GetRGB().Name());
      else
        di << col.GetRGB().StringName(col.GetRGB().Name()) << " (" << col.Alpha() << ")";
      di << " ";
    }
  }
  return 0;
}


static Standard_Integer addColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"Use: "<<argv[0]<<" DocName R G B [alpha]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());

  Quantity_Color Col ( Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]), Quantity_TOC_RGB );
  if (argc == 6) {
    Quantity_ColorRGBA aColRGBA(Col);
    aColRGBA.SetAlpha((Standard_ShortReal)(Draw::Atof(argv[5])));
    aLabel = myColors->AddColor(aColRGBA);
  }
  else 
    aLabel = myColors->AddColor(Col);
  
  TCollection_AsciiString Entry;
  TDF_Tool::Entry(aLabel, Entry);
  di << Entry.ToCString();
  return 0;
}

static Standard_Integer removeColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=3) {
    di<<"Use: "<<argv[0]<<" DocName Label\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  myColors->RemoveColor(aLabel);
  
  return 0;
}

static Standard_Integer findColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"Use: "<<argv[0]<<" DocName R G B [alpha]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  TCollection_AsciiString Entry;
  Quantity_Color Col(Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]), Quantity_TOC_RGB);
  if (argc == 5) {
    TDF_Tool::Entry(myColors->FindColor(Col), Entry);
  }
  else {
    Quantity_ColorRGBA aColRGBA(Col);
    aColRGBA.SetAlpha((Standard_ShortReal)Draw::Atof(argv[5]));
    TDF_Tool::Entry(myColors->FindColor(aColRGBA), Entry);
  }
  di << Entry.ToCString();
  return 0;
}

static Standard_Integer unsetColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc!=4) {
    di<<"Use: "<<argv[0]<<" DocName {Label|Shape} ColorType\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }

  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  if ( !aLabel.IsNull() ) {
    myColors->UnSetColor(aLabel, argv[3][0] == 's' ? XCAFDoc_ColorSurf : XCAFDoc_ColorCurv);
  }
  TopoDS_Shape aShape= DBRep::Get(argv[2]);
  if ( !aShape.IsNull() ) {
    myColors->UnSetColor(aShape, argv[3][0] == 's' ? XCAFDoc_ColorSurf : XCAFDoc_ColorCurv);
  }
  return 0;
}

static Standard_Integer setVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<"DocName {Lable|Shape} [isvisible(1/0)]\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ColorTool) localTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  Standard_Boolean isvisible = Standard_False;
  if ( (argc==4) && (Draw::Atoi(argv[3])==1) ) isvisible = Standard_True;
  
  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) {
    // get label by shape
    TopoDS_Shape aShape= DBRep::Get(argv[2]);
    if ( !aShape.IsNull() ) {
      aLabel = localTool->ShapeTool()->FindShape( aShape, Standard_True );
    }
  }
  if ( aLabel.IsNull() ) {
    di << " cannot find indicated label in document\n";
    return 1;
  }
  localTool->SetVisibility( aLabel, isvisible );
  return 0;
}

static Standard_Integer getVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<"DocName {Lable|Shape}\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ColorTool) localTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  TDF_Label aLabel;
  TDF_Tool::Label(Doc->GetData(), argv[2], aLabel);
  if ( aLabel.IsNull() ) {
    // get label by shape
    TopoDS_Shape aShape= DBRep::Get(argv[2]);
    if ( !aShape.IsNull() ) {
      aLabel = localTool->ShapeTool()->FindShape( aShape, Standard_True );
    }
  }
  if ( aLabel.IsNull() ) {
    di << " cannot find indicated label in document\n";
    return 1;
  }
  if (localTool->IsVisible( aLabel) ) di << 1;
  else di << 0;
  return 0;
}

static Standard_Integer getStyledVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<"DocName Shape\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  Handle(XCAFDoc_ColorTool) localTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);
  if (localTool->IsInstanceVisible( aShape) ) di << 1;
  else di << 0;
  return 0;
}

static Standard_Integer getStyledcolor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di<<"Use: "<<argv[0]<<" Doc shape colortype(s/c)\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);

  Quantity_ColorRGBA col;
  XCAFDoc_ColorType type;
  if ( argv[3] && argv[3][0] == 's' )
    type = XCAFDoc_ColorSurf;
  else if ( argv[3] && argv[3][0] == 'c' )
    type = XCAFDoc_ColorCurv;
  else
    type = XCAFDoc_ColorGen;
  Handle(XCAFDoc_ColorTool) localTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  if (localTool->GetInstanceColor( aShape, type, col) ) 
  {
    if ((1 - col.Alpha()) < Precision::Confusion())
      di << col.GetRGB().StringName(col.GetRGB().Name());
    else
      di << col.GetRGB().StringName(col.GetRGB().Name()) << " (" << col.Alpha() << ")";
  }
  return 0;
}

static Standard_Integer setStyledcolor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<6) {
    di<<"Use: "<<argv[0]<<" Doc shape R G B [alpha] type(s/c)\n";
    return 1;
  }
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) { di << argv[1] << " is not a document\n"; return 1; }
  TopoDS_Shape aShape;
  aShape = DBRep::Get(argv[2]);

  Quantity_Color col ( Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5]), Quantity_TOC_RGB );
  Quantity_ColorRGBA aColRGBA;
  aColRGBA.SetRGB(col);
  if (argc > 6 && (argv[6][0] != 's' && argv[6][0] != 'c')) {
    aColRGBA.SetAlpha((Standard_ShortReal)(Draw::Atof(argv[6])));
  }

  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  XCAFDoc_ColorType ctype = XCAFDoc_ColorGen;
  if (argc > 6) {
    if (argv[argc - 1][0] == 's')
      ctype = XCAFDoc_ColorSurf;
    else if (argv[argc - 1][0] == 'c')
      ctype = XCAFDoc_ColorCurv;
  }
  Handle(XCAFDoc_ColorTool) localTool = XCAFDoc_DocumentTool::ColorTool(Doc->Main());
  if (!localTool->SetInstanceColor(aShape, ctype, aColRGBA))
  {
    di << "cannot set color for the indicated component\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void XDEDRAW_Colors::InitCommands(Draw_Interpretor& di) 
{
  static Standard_Boolean initactor = Standard_False;
  if (initactor)
  {
    return;
  }
  initactor = Standard_True;

  //=====================================
  // Work with colors
  //=====================================  
  
  Standard_CString g = "XDE color's commands";
  
  di.Add ("XSetColor","Doc {Label|Shape} R G B [alpha] [c|s]\t: Set color [R G B] to shape given by Label, "
                      "type of color 's' - for surface, 'c' - for curve (default generic)",
		   __FILE__, setColor, g);

  di.Add ("XGetColor","Doc label \t: Return color defined on label in colortable",
 		   __FILE__, getColor, g);

  di.Add ("XGetShapeColor","Doc Label ColorType \t: Returns color defined by label",
 		   __FILE__, getShapeColor, g);
  
  di.Add ("XGetAllColors","Doc \t: Print all colors that defined in document",
 		   __FILE__, getAllColors, g);
  
  di.Add ("XAddColor","Doc R G B [alpha]\t: Add color in document to color table",
 		   __FILE__, addColor, g);
  
  di.Add ("XRemoveColor","Doc Label \t: Remove color in document from color table",
		   __FILE__, removeColor, g);

  di.Add ("XFindColor","Doc R G B [alpha]\t: Find label where indicated color is situated",
 		   __FILE__, findColor, g);

  di.Add ("XUnsetColor","Doc {Label|Shape} ColorType \t: Unset color ",
		   __FILE__, unsetColor, g);
  
  di.Add ("XSetObjVisibility","Doc {Label|Shape} (0\1) \t: Set the visibility of shape  ",
		   __FILE__, setVisibility, g);
  
  di.Add ("XGetObjVisibility","Doc {Label|Shape} \t: Return the visibility of shape ",
		   __FILE__, getVisibility, g);

  di.Add ("XGetInstanceVisible","Doc Shape \t: Return the visibility of shape ",
		   __FILE__, getStyledVisibility, g);

  di.Add ("XGetInstanceColor","Doc Shape \t: Return the color of component shape ",
		   __FILE__, getStyledcolor, g);

  di.Add ("XSetInstanceColor","Doc Shape R G B [alpha] type \t: sets color for component of shape if SHUO structure exists already ",
		   __FILE__, setStyledcolor, g);

}
