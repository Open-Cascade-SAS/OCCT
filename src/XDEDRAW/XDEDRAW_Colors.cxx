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
#include <ViewerTest.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XDEDRAW_Colors.hxx>

//! Parse XCAFDoc_ColorType enumeration argument.
static bool parseXDocColorType (const TCollection_AsciiString& theArg,
                                XCAFDoc_ColorType& theType)
{
  TCollection_AsciiString anArgCase (theArg);
  anArgCase.LowerCase();
  if (anArgCase == "surf"
   || anArgCase == "surface"
   || anArgCase == "s")
  {
    theType = XCAFDoc_ColorSurf;
    return true;
  }
  else if (anArgCase == "curve"
        || anArgCase == "c")
  {
    theType = XCAFDoc_ColorCurv;
    return true;
  }
  else if (anArgCase == "gen"
        || anArgCase == "generic")
  {
    theType = XCAFDoc_ColorGen;
    return true;
  }
  return false;
}

//=======================================================================
// Section: Work with colors
//=======================================================================
static Standard_Integer setColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TopoDS_Shape aShape;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    aShape = DBRep::Get (argv[2]);
    if (aShape.IsNull())
    {
      std::cout << "Syntax error: " << argv[2] << " is not a label nor shape\n";
      return 1;
    }
  }

  Quantity_ColorRGBA aColor;
  bool isColorDefined = false;
  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  for (Standard_Integer anArgIter = 3; anArgIter < argc; ++anArgIter)
  {
    if (parseXDocColorType (argv[anArgIter], aColType))
    {
      //
    }
    else if (!isColorDefined)
    {
      isColorDefined = true;
      Standard_Integer aNbParsed = ViewerTest::ParseColor (argc - anArgIter,
                                                           argv + anArgIter,
                                                           aColor);
      if (aNbParsed == 0)
      {
        std::cout << "Syntax error at '" << argv[anArgIter] << "'\n";
        return 1;
      }
      anArgIter += aNbParsed - 1;
    }
    else
    {
      std::cout << "Syntax error at '" << argv[anArgIter] << "'\n";
      return 1;
    }
  }
  if (!isColorDefined)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (!aLabel.IsNull())
  {
    aColorTool->SetColor (aLabel, aColor, aColType);
  }
  else if (!aColorTool->SetColor (aShape, aColor, aColType))
  {
    std::cout << "Syntax error: " << argv[2] << " is not a label nor shape\n";
    return 1;
  }
  return 0;
}

static Standard_Integer getColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  Quantity_ColorRGBA aColor;
  if (!myColors->GetColor (aLabel, aColor))
  {
    return 0;
  }

  if ((1.0 - aColor.Alpha()) < Precision::Confusion())
  {
    di << aColor.GetRGB().StringName (aColor.GetRGB().Name());
  }
  else
  {
    di << aColor.GetRGB().StringName (aColor.GetRGB().Name()) << " (" << aColor.Alpha() << ")";
  }
  return 0;
}

static Standard_Integer getShapeColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3 && argc != 4)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    std::cout << "Syntax error: '" << argv[2] << "' label is not found in the document\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  if (argc > 3 && !parseXDocColorType (argv[3], aColType))
  {
    std::cout << "Syntax error: unknown color type '" << argv[3] << "'\n";
    return 1;
  }

  Quantity_ColorRGBA aColor;
  if (!myColors->GetColor (aLabel, aColType, aColor))
  {
    return 0;
  }

  if ((1.0 - aColor.Alpha()) < Precision::Confusion())
  {
    di << aColor.GetRGB().StringName(aColor.GetRGB().Name());
  }
  else
  {
    di << aColor.GetRGB().StringName(aColor.GetRGB().Name()) << " (" << aColor.Alpha() << ")";
  }

  return 0;
}

static Standard_Integer getAllColors (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  TDF_LabelSequence aLabels;
  aColorTool->GetColors (aLabels);
  if (aLabels.Length() >= 1)
  {
    for (TDF_LabelSequence::Iterator aLabIter (aLabels); aLabIter.More(); aLabIter.Next())
    {
      Quantity_ColorRGBA aColor;
      if (!aColorTool->GetColor (aLabIter.Value(), aColor))
      {
        continue;
      }
      if ((1.0 - aColor.Alpha()) < Precision::Confusion())
      {
        di << aColor.GetRGB().StringName (aColor.GetRGB().Name());
      }
      else
      {
        di << aColor.GetRGB().StringName (aColor.GetRGB().Name()) << " (" << aColor.Alpha() << ")";
      }
      di << " ";
    }
  }
  return 0;
}


static Standard_Integer addColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  Quantity_ColorRGBA aColRGBA;
  Standard_Integer aNbParsed = ViewerTest::ParseColor (argc - 2, argv + 2, aColRGBA);
  if (aNbParsed != argc - 2)
  {
    std::cout << "Syntax error at '" << argv[2] << "'\n";
    return 1;
  }

  TCollection_AsciiString anEntry;
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  TDF_Label aLabel = aColorTool->AddColor (aColRGBA);
  TDF_Tool::Entry (aLabel, anEntry);
  di << anEntry;
  return 0;
}

static Standard_Integer removeColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  TDF_Label aLabel;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " label is not found in the document\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  aColorTool->RemoveColor (aLabel);
  return 0;
}

static Standard_Integer findColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  Quantity_ColorRGBA aColRGBA;
  Standard_Integer aNbParsed = ViewerTest::ParseColor (argc - 2, argv + 2, aColRGBA);
  if (aNbParsed != argc - 2)
  {
    std::cout << "Syntax error at '" << argv[2] << "'\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry (aColorTool->FindColor (aColRGBA), anEntry);
  di << anEntry;
  return 0;
}

static Standard_Integer unsetColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 4)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  if (!parseXDocColorType (argv[3], aColType))
  {
    std::cout << "Syntax error: unknown color type '" << argv[3] << "'\n";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (!aLabel.IsNull())
  {
    myColors->UnSetColor (aLabel, aColType);
    return 0;
  }

  TopoDS_Shape aShape = DBRep::Get (argv[2]);
  if (aShape.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a label nor shape\n";
    return 1;
  }
  myColors->UnSetColor (aShape, aColType);
  return 0;
}

static Standard_Integer setVisibility (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 3 && argc != 4)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  TDF_Label aLabel;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (aLabel.IsNull())
  {
    // get label by shape
    TopoDS_Shape aShape = DBRep::Get (argv[2]);
    if (!aShape.IsNull())
    {
      aLabel = aColorTool->ShapeTool()->FindShape (aShape, Standard_True);
    }
  }
  if (aLabel.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a label not shape\n";
    return 1;
  }

  Standard_Boolean isVisible = Standard_False;
  if (argc == 4)
  {
    TCollection_AsciiString aVisArg (argv[3]);
    if (aVisArg == "1")
    {
      isVisible = Standard_True;
    }
    else if (aVisArg == "0")
    {
      isVisible = Standard_False;
    }
    else
    {
      std::cout << "Syntax error: unknown argument '" << argv[3] << "'\n";
      return 1;
    }
  }
  aColorTool->SetVisibility (aLabel, isVisible);
  return 0;
}

static Standard_Integer getVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    // get label by shape
    TopoDS_Shape aShape = DBRep::Get (argv[2]);
    if (!aShape.IsNull())
    {
      aLabel = aColorTool->ShapeTool()->FindShape (aShape, Standard_True);
    }
  }
  if (aLabel.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a label not shape\n";
    return 1;
  }

  di << (aColorTool->IsVisible (aLabel) ? 1 : 0);
  return 0;
}

static Standard_Integer getStyledVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  TopoDS_Shape aShape = DBRep::Get(argv[2]);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }
  if (aShape.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a shape\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  di << (aColorTool->IsInstanceVisible (aShape) ? 1 : 0);
  return 0;
}

static Standard_Integer getStyledcolor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3 && argc != 4)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  DDocStd::GetDocument (argv[1], aDoc);
  TopoDS_Shape aShape = DBRep::Get (argv[2]);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }
  if (aShape.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a shape\n";
    return 1;
  }
  if (argc > 3 && !parseXDocColorType (argv[3], aColType))
  {
    std::cout << "Syntax error: unknown color type '" << argv[3] << "'\n";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  Quantity_ColorRGBA aColor;
  if (aColorTool->GetInstanceColor (aShape, aColType, aColor))
  {
    if ((1.0 - aColor.Alpha()) < Precision::Confusion())
    {
      di << aColor.GetRGB().StringName (aColor.GetRGB().Name());
    }
    else
    {
      di << aColor.GetRGB().StringName (aColor.GetRGB().Name()) << " (" << aColor.Alpha() << ")";
    }
  }
  return 0;
}

static Standard_Integer setStyledcolor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc < 3)
  {
    std::cout << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    std::cout << "Syntax error: " << argv[1] << " is not a document\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get (argv[2]);
  if (aShape.IsNull())
  {
    std::cout << "Syntax error: " << argv[2] << " is not a shape\n";
    return 1;
  }

  XCAFDoc_ColorType aColorType = XCAFDoc_ColorGen;
  Quantity_ColorRGBA aColRGBA;
  for (Standard_Integer anArgIter = 3; anArgIter < argc; ++anArgIter)
  {
    if (parseXDocColorType (argv[anArgIter], aColorType))
    {
      //
    }
    else
    {
      Standard_Integer aNbParsed = ViewerTest::ParseColor (argc - anArgIter,
                                                           argv + anArgIter,
                                                           aColRGBA);
      if (aNbParsed == 0)
      {
        std::cout << "Syntax error at '" << argv[anArgIter] << "'\n";
        return 1;
      }
      anArgIter += aNbParsed - 1;
    }
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (!aColorTool->SetInstanceColor (aShape, aColorType, aColRGBA))
  {
    std::cout << "Error: cannot set color for the indicated component\n";
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

  di.Add ("XSetColor","Doc {Label|Shape} R G B [alpha] [{generic|surface|curve}=gen]"
                      "\t: Set color [R G B] to shape given by Label, "
                      "type of color 's' - for surface, 'c' - for curve (default generic)",
		   __FILE__, setColor, g);

  di.Add ("XGetColor","Doc label"
                      "\t: Return color defined on label in colortable",
 		   __FILE__, getColor, g);

  di.Add ("XGetShapeColor","Doc Label {generic|surface|curve}"
                           "\t: Returns color defined by label",
 		   __FILE__, getShapeColor, g);

  di.Add ("XGetAllColors","Doc"
                          "\t: Print all colors that defined in document",
 		   __FILE__, getAllColors, g);
  
  di.Add ("XAddColor","Doc R G B [alpha]"
                      "\t: Add color in document to color table",
 		   __FILE__, addColor, g);
  
  di.Add ("XRemoveColor","Doc Label"
                         "\t: Remove color in document from color table",
		   __FILE__, removeColor, g);

  di.Add ("XFindColor","Doc R G B [alpha]"
                       "\t: Find label where indicated color is situated",
 		   __FILE__, findColor, g);

  di.Add ("XUnsetColor","Doc {Label|Shape} {generic|surface|curve}"
                        "\t: Unset color",
		   __FILE__, unsetColor, g);
  
  di.Add ("XSetObjVisibility","Doc {Label|Shape} (0\1) \t: Set the visibility of shape  ",
		   __FILE__, setVisibility, g);
  
  di.Add ("XGetObjVisibility","Doc {Label|Shape} \t: Return the visibility of shape ",
		   __FILE__, getVisibility, g);

  di.Add ("XGetInstanceVisible","Doc Shape \t: Return the visibility of shape ",
		   __FILE__, getStyledVisibility, g);

  di.Add ("XGetInstanceColor","Doc Shape [{generic|surface|curve}=gen]"
                              "\t: Return the color of component shape",
		   __FILE__, getStyledcolor, g);

  di.Add ("XSetInstanceColor","Doc Shape R G B [alpha] [{generic|surface|curve}=gen]"
                              "\t: sets color for component of shape if SHUO structure exists already",
		   __FILE__, setStyledcolor, g);
}
