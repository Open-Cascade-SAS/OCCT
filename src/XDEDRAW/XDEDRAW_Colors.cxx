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
#include <Message.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <OSD_File.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Shape.hxx>
#include <ViewerTest.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
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

//! Print triplet of values.
template<class S, class T> static S& operator<< (S& theStream, const NCollection_Vec3<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2];
  return theStream;
}

//! Print 4 values.
template<class S, class T> static S& operator<< (S& theStream, const NCollection_Vec4<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2] << " " << theVec[3];
  return theStream;
}

//! Convert alpha mode into string.
static const char* alphaModeToString (Graphic3d_AlphaMode theMode)
{
  switch (theMode)
  {
    case Graphic3d_AlphaMode_Opaque:    return "Opaque";
    case Graphic3d_AlphaMode_Mask:      return "Mask";
    case Graphic3d_AlphaMode_Blend:     return "Blend";
    case Graphic3d_AlphaMode_BlendAuto: return "BlendAuto";
  }
  return "";
}

//! Find existing visualization material in the document.
static TDF_Label findVisMaterial (const Handle(TDocStd_Document)& theDoc,
                                  const TCollection_AsciiString& theKey)
{
  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (theDoc->Main());
  TDF_Label aMatLab;
  TDF_Tool::Label (theDoc->GetData(), theKey, aMatLab);
  if (!aMatLab.IsNull())
  {
    return aMatTool->IsMaterial (aMatLab) ? aMatLab : TDF_Label();
  }

  TDF_LabelSequence aLabels;
  aMatTool->GetMaterials (aLabels);
  for (TDF_LabelSequence::Iterator aLabIter (aLabels); aLabIter.More(); aLabIter.Next())
  {
    Handle(TDataStd_Name) aNodeName;
    if (aLabIter.Value().FindAttribute (TDataStd_Name::GetID(), aNodeName)
     && aNodeName->Get().IsEqual (theKey))
    {
      return aLabIter.Value();
    }
  }
  return TDF_Label();
}

//! Check if image file exists.
static bool isImageFileExist (const TCollection_AsciiString& thePath)
{
  const OSD_Path aPath (thePath);
  if (!OSD_File (aPath).Exists())
  {
    std::cout << "Error: file '" << thePath << " not found\n";
    return false;
  }
  return true;
}

//! Parse RGB values coming after specified argument.
static bool parseRgbColor (Standard_Integer& theArgIter,
                           Quantity_Color&   theColor,
                           Standard_Integer  theNbArgs,
                           const char**      theArgVec)
{
  Standard_Integer aNbParsed = Draw::ParseColor (theNbArgs - theArgIter - 1,
                                                 theArgVec + theArgIter + 1,
                                                 theColor);
  if (aNbParsed == 0)
  {
    std::cout << "Syntax error at '" << theArgVec[theArgIter] << "'\n";
    return false;
  }
  theArgIter += aNbParsed;
  return true;
}

//! Parse normalized real value within 0..1 range.
static bool parseNormalizedReal (const char* theString,
                                 Standard_ShortReal& theValue)
{
  theValue = (Standard_ShortReal )Draw::Atof (theString);
  if (theValue < 0.0f || theValue > 1.0f)
  {
    std::cerr << "Syntax error at '" << theString << "'\n";
    return false;
  }
  return true;
}

//=======================================================================
// Section: Work with colors
//=======================================================================
static Standard_Integer setColor (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc < 4)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
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
      Message::SendFail() << "Syntax error: " << argv[2] << " is not a label nor shape";
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
      Standard_Integer aNbParsed = Draw::ParseColor (argc - anArgIter,
                                                     argv + anArgIter,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
        return 1;
      }
      anArgIter += aNbParsed - 1;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
      return 1;
    }
  }
  if (!isColorDefined)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (!aLabel.IsNull())
  {
    aColorTool->SetColor (aLabel, aColor, aColType);
  }
  else if (!aColorTool->SetColor (aShape, aColor, aColType))
  {
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a label nor shape";
    return 1;
  }
  return 0;
}

static Standard_Integer getColor (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }

  TDF_Label aLabel;
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    Message::SendFail() << "Syntax error: '" << argv[2] << "' label is not found in the document";
    return 1;
  }

  Handle(XCAFDoc_ColorTool) myColors = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  if (argc > 3 && !parseXDocColorType (argv[3], aColType))
  {
    Message::SendFail() << "Syntax error: unknown color type '" << argv[3] << "'";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }

  Quantity_ColorRGBA aColRGBA;
  Standard_Integer aNbParsed = Draw::ParseColor (argc - 2, argv + 2, aColRGBA);
  if (aNbParsed != argc - 2)
  {
    Message::SendFail() << "Syntax error at '" << argv[2] << "'";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  TDF_Label aLabel;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }
  TDF_Tool::Label (aDoc->GetData(), argv[2], aLabel);
  if (aLabel.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[2] << " label is not found in the document";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }

  Quantity_ColorRGBA aColRGBA;
  Standard_Integer aNbParsed = Draw::ParseColor (argc - 2, argv + 2, aColRGBA);
  if (aNbParsed != argc - 2)
  {
    Message::SendFail() << "Syntax error at '" << argv[2] << "'";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }

  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  if (!parseXDocColorType (argv[3], aColType))
  {
    Message::SendFail() << "Syntax error: unknown color type '" << argv[3] << "'";
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
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a label nor shape";
    return 1;
  }
  myColors->UnSetColor (aShape, aColType);
  return 0;
}

static Standard_Integer setVisibility (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 3 && argc != 4)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  TDF_Label aLabel;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
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
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a label not shape";
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
      Message::SendFail() << "Syntax error: unknown argument '" << argv[3] << "'";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
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
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a label not shape";
    return 1;
  }

  di << (aColorTool->IsVisible (aLabel) ? 1 : 0);
  return 0;
}

static Standard_Integer getStyledVisibility (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  TopoDS_Shape aShape = DBRep::Get(argv[2]);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }
  if (aShape.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a shape";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  XCAFDoc_ColorType aColType = XCAFDoc_ColorGen;
  DDocStd::GetDocument (argv[1], aDoc);
  TopoDS_Shape aShape = DBRep::Get (argv[2]);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }
  if (aShape.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a shape";
    return 1;
  }
  if (argc > 3 && !parseXDocColorType (argv[3], aColType))
  {
    Message::SendFail() << "Syntax error: unknown color type '" << argv[3] << "'";
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
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (argv[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[1] << " is not a document";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get (argv[2]);
  if (aShape.IsNull())
  {
    Message::SendFail() << "Syntax error: " << argv[2] << " is not a shape";
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
      Standard_Integer aNbParsed = Draw::ParseColor (argc - anArgIter,
                                                     argv + anArgIter,
                                                     aColRGBA);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
        return 1;
      }
      anArgIter += aNbParsed - 1;
    }
  }

  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (!aColorTool->SetInstanceColor (aShape, aColorType, aColRGBA))
  {
    Message::SendFail() << "Error: cannot set color for the indicated component";
    return 1;
  }
  return 0;
}

// ================================================================
// Function : XGetAllVisMaterials
// Purpose  :
// ================================================================
static Standard_Integer XGetAllVisMaterials (Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 2 && theNbArgs != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (theArgVec[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  bool toPrintNames = true;
  if (theNbArgs == 3)
  {
    TCollection_AsciiString anArgCase (theArgVec[2]);
    anArgCase.LowerCase();
    if (anArgCase == "-names")
    {
      toPrintNames = true;
    }
    else if (anArgCase == "-labels")
    {
      toPrintNames = false;
    }
  }

  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (aDoc->Main());
  TDF_LabelSequence aLabels;
  aMatTool->GetMaterials (aLabels);
  Standard_Integer aMatIndex = 1;
  for (TDF_LabelSequence::Iterator aLabIter (aLabels); aLabIter.More(); aLabIter.Next(), ++aMatIndex)
  {
    const TDF_Label& aMatLab = aLabIter.Value();
    if (!toPrintNames)
    {
      TCollection_AsciiString anEntryId;
      TDF_Tool::Entry (aMatLab, anEntryId);
      theDI << anEntryId << " ";
      continue;
    }

    Handle(TDataStd_Name) aNodeName;
    if (aMatLab.FindAttribute (TDataStd_Name::GetID(), aNodeName))
    {
      theDI << aNodeName->Get() << " ";
    }
    else
    {
      TCollection_AsciiString aName = TCollection_AsciiString("<UNNAMED") + aMatIndex + ">";
      theDI << aName << " ";
    }
  }
  return 0;
}

// ================================================================
// Function : XGetVisMaterial
// Purpose  :
// ================================================================
static Standard_Integer XGetVisMaterial (Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (theArgVec[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (aDoc->Main());
  Handle(XCAFDoc_VisMaterial) aMat;
  TDF_Label aMatLab = findVisMaterial (aDoc, theArgVec[2]);
  if (!aMatLab.IsNull())
  {
    aMat = aMatTool->GetMaterial (aMatLab);
  }
  else
  {
    TDF_Label aShapeLab;
    TDF_Tool::Label (aDoc->GetData(), theArgVec[2], aShapeLab);
    if (aShapeLab.IsNull())
    {
      TopoDS_Shape aShape = DBRep::Get (theArgVec[2]);
      if (!aShape.IsNull())
      {
        aShapeLab = aMatTool->ShapeTool()->FindShape (aShape);
      }
    }
    if (!aShapeLab.IsNull()
     && !aMatTool->ShapeTool()->IsShape (aShapeLab))
    {
      aShapeLab.Nullify();
    }
    if (aShapeLab.IsNull())
    {
      Message::SendFail() << "Syntax error: " << theArgVec[2] << " is not material nor shape";
      return 1;
    }

    aMat = aMatTool->GetShapeMaterial (aShapeLab);
  }

  if (aMat.IsNull())
  {
    theDI << "EMPTY\n";
    return 0;
  }

  TCollection_AsciiString anEntryId;
  TDF_Tool::Entry (aMat->Label(), anEntryId);
  theDI << "Label:                  " << anEntryId << "\n";

  Handle(TDataStd_Name) aNodeName;
  if (aMat->Label().FindAttribute (TDataStd_Name::GetID(), aNodeName))
  {
    theDI << "Name:                   " << aNodeName->Get() << "\n";
  }
  if (aMat->IsEmpty())
  {
    theDI << "EMPTY\n";
    return 0;
  }
  theDI << "AlphaMode:              " << alphaModeToString (aMat->AlphaMode()) << "\n";
  theDI << "AlphaCutOff:            " << aMat->AlphaCutOff() << "\n";
  theDI << "IsDoubleSided:          " << aMat->IsDoubleSided() << "\n";
  if (aMat->HasCommonMaterial())
  {
    const XCAFDoc_VisMaterialCommon& aMatCom = aMat->CommonMaterial();
    theDI << "Common.Ambient:         " << (Graphic3d_Vec3 )aMatCom.AmbientColor << "\n";
    theDI << "Common.Diffuse:         " << (Graphic3d_Vec3 )aMatCom.DiffuseColor << "\n";
    if (!aMatCom.DiffuseTexture.IsNull())
    {
      theDI << "Common.DiffuseTexture:  " << aMatCom.DiffuseTexture->TextureId() << "\n";
    }
    theDI << "Common.Specular:        " << (Graphic3d_Vec3 )aMatCom.SpecularColor << "\n";
    theDI << "Common.Emissive:        " << (Graphic3d_Vec3 )aMatCom.EmissiveColor << "\n";
    theDI << "Common.Shininess:       " << aMatCom.Shininess << "\n";
    theDI << "Common.Transparency:    " << aMatCom.Transparency << "\n";
  }
  if (aMat->HasPbrMaterial())
  {
    const XCAFDoc_VisMaterialPBR& aMatPbr = aMat->PbrMaterial();
    theDI << "PBR.BaseColor:          " << (Graphic3d_Vec3 )aMatPbr.BaseColor.GetRGB() << "\n";
    theDI << "PBR.Transparency:       " << (1.0 - aMatPbr.BaseColor.Alpha()) << "\n";
    theDI << "PBR.RefractionIndex:    " << aMatPbr.RefractionIndex << "\n";
    if (!aMatPbr.BaseColorTexture.IsNull())
    {
      theDI << "PBR.BaseColorTexture:   " << aMatPbr.BaseColorTexture->TextureId() << "\n";
    }
    theDI << "PBR.EmissiveFactor:     " << aMatPbr.EmissiveFactor << "\n";
    if (!aMatPbr.EmissiveTexture.IsNull())
    {
      theDI << "PBR.EmissiveTexture:    " << aMatPbr.EmissiveTexture->TextureId() << "\n";
    }
    theDI << "PBR.Metallic:           " << aMatPbr.Metallic << "\n";
    theDI << "PBR.Roughness:          " << aMatPbr.Roughness << "\n";
    if (!aMatPbr.MetallicRoughnessTexture.IsNull())
    {
      theDI << "PBR.MetallicRoughnessTexture: " << aMatPbr.MetallicRoughnessTexture->TextureId() << "\n";
    }
    if (!aMatPbr.OcclusionTexture.IsNull())
    {
      theDI << "PBR.OcclusionTexture:   " << aMatPbr.OcclusionTexture->TextureId() << "\n";
    }
    if (!aMatPbr.NormalTexture.IsNull())
    {
      theDI << "PBR.NormalTexture:      " << aMatPbr.NormalTexture->TextureId() << "\n";
    }
  }
  return 0;
}

// ================================================================
// Function : XAddVisMaterial
// Purpose  :
// ================================================================
static Standard_Integer XAddVisMaterial (Draw_Interpretor& , Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (theArgVec[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (aDoc->Main());
  TDF_Label aMatLab = findVisMaterial (aDoc, theArgVec[2]);
  if (aMatLab.IsNull())
  {
    aMatLab = aMatTool->AddMaterial (theArgVec[2]);
  }

  Handle(XCAFDoc_VisMaterial) aMat = aMatTool->GetMaterial (aMatLab);
  XCAFDoc_VisMaterialCommon aMatCom = aMat->CommonMaterial();
  XCAFDoc_VisMaterialPBR    aMatPbr = aMat->PbrMaterial();
  Standard_ShortReal aRealValue = 0.0f;
  for (Standard_Integer anArgIter = 3; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if ((anArg == "-transparency"
      || anArg == "-alpha")
     && anArgIter + 1 < theNbArgs
     && parseNormalizedReal (theArgVec[anArgIter + 1], aMatCom.Transparency))
    {
      ++anArgIter;
      if (anArg == "-alpha")
      {
        aMatCom.Transparency = 1.0f - aMatCom.Transparency;
      }
      aMatPbr.BaseColor.SetAlpha (1.0f - aMatCom.Transparency);
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-refractionindex" || anArg == "-ior"))
    {
      aMatPbr.RefractionIndex = (Standard_ShortReal )Draw::Atof (theArgVec[anArgIter + 1]);
      if (aMatPbr.RefractionIndex < 1.0f || aMatPbr.RefractionIndex > 3.0f)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      ++anArgIter;
      aMatPbr.IsDefined = true;
    }
    else if (anArg == "-alphaMode"
          && anArgIter + 2 < theNbArgs
          && parseNormalizedReal (theArgVec[anArgIter + 2], aRealValue))
    {
      TCollection_AsciiString aModeStr (theArgVec[anArgIter + 1]);
      aModeStr.LowerCase();
      Graphic3d_AlphaMode anAlphaMode = Graphic3d_AlphaMode_Opaque;
      if (aModeStr == "opaque")
      {
        anAlphaMode = Graphic3d_AlphaMode_Opaque;
      }
      else if (aModeStr == "mask")
      {
        anAlphaMode = Graphic3d_AlphaMode_Mask;
      }
      else if (aModeStr == "blend")
      {
        anAlphaMode = Graphic3d_AlphaMode_Blend;
      }
      else if (aModeStr == "blendauto")
      {
        anAlphaMode = Graphic3d_AlphaMode_BlendAuto;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      aMat->SetAlphaMode (anAlphaMode, aRealValue);
      anArgIter += 2;
    }
    else if (anArg == "-diffuse"
          || anArg == "-basecolor"
          || anArg == "-albedo")
    {
      Quantity_ColorRGBA aColorRGBA;
      Standard_Integer aNbParsed = Draw::ParseColor (theNbArgs - anArgIter - 1,
                                                     theArgVec + anArgIter + 1,
                                                     aColorRGBA);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      anArgIter += aNbParsed;

      if (anArg == "-diffuse")
      {
        aMatCom.IsDefined = true;
        aMatCom.DiffuseColor = aColorRGBA.GetRGB();
        if (aNbParsed == 2 || aNbParsed == 4)
        {
          aMatCom.Transparency = 1.0f - aColorRGBA.Alpha();
        }
      }
      else
      {
        aMatPbr.IsDefined = true;
        if (aNbParsed == 2 || aNbParsed == 4)
        {
          aMatPbr.BaseColor = aColorRGBA;
        }
        else
        {
          aMatPbr.BaseColor.SetRGB (aColorRGBA.GetRGB());
        }
      }
    }
    else if (anArg == "-specular"
          && parseRgbColor (anArgIter, aMatCom.SpecularColor,
                            theNbArgs, theArgVec))
    {
      aMatCom.IsDefined = true;
    }
    else if (anArg == "-ambient"
          && parseRgbColor (anArgIter, aMatCom.AmbientColor,
                            theNbArgs, theArgVec))
    {
      aMatCom.IsDefined = true;
    }
    else if (anArg == "-emissive"
          && parseRgbColor (anArgIter, aMatCom.EmissiveColor,
                            theNbArgs, theArgVec))
    {
      aMatCom.IsDefined = true;
    }
    else if (anArg == "-shininess"
          && anArgIter + 1 < theNbArgs)
    {
      aMatCom.IsDefined = true;
      aMatCom.Shininess = (float )Draw::Atof (theArgVec[++anArgIter]);
      if (aMatCom.Shininess < 0.0f || aMatCom.Shininess > 1.0f)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-diffusetexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatCom.IsDefined = true;
      aMatCom.DiffuseTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-basecolortexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatPbr.IsDefined = true;
      aMatPbr.BaseColorTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-emissivetexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatPbr.IsDefined = true;
      aMatPbr.EmissiveTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-metallicroughnesstexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatPbr.IsDefined = true;
      aMatPbr.MetallicRoughnessTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-normaltexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatPbr.IsDefined = true;
      aMatPbr.NormalTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-occlusiontexture"
          && isImageFileExist (theArgVec[anArgIter + 1]))
    {
      aMatPbr.IsDefined = true;
      aMatPbr.OcclusionTexture = new Image_Texture (theArgVec[++anArgIter]);
    }
    else if (anArg == "-emissivefactor"
          && anArgIter + 4 < theNbArgs)
    {
      aMatPbr.IsDefined = true;
      aMatPbr.EmissiveFactor.SetValues ((float )Draw::Atof (theArgVec[anArgIter + 1]),
                                        (float )Draw::Atof (theArgVec[anArgIter + 2]),
                                        (float )Draw::Atof (theArgVec[anArgIter + 3]));
      anArgIter += 3;
    }
    else if (anArg == "-doublesided")
    {
      aMatPbr.IsDefined = true;
      bool isDoubleSided = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], isDoubleSided))
      {
        ++anArgIter;
      }
      aMat->SetDoubleSided (isDoubleSided);
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-metallic"
          && parseNormalizedReal (theArgVec[anArgIter + 1], aMatPbr.Metallic))
    {
      ++anArgIter;
      aMatPbr.IsDefined = true;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-roughness"
          && parseNormalizedReal (theArgVec[anArgIter + 1], aMatPbr.Roughness))
    {
      ++anArgIter;
      aMatPbr.IsDefined = true;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  aMat->SetCommonMaterial (aMatCom);
  aMat->SetPbrMaterial (aMatPbr);
  return 0;
}

// ================================================================
// Function : XRemoveVisMaterial
// Purpose  :
// ================================================================
static Standard_Integer XRemoveVisMaterial (Draw_Interpretor& , Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  DDocStd::GetDocument (theArgVec[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  TDF_Label aMatLab = findVisMaterial (aDoc, theArgVec[2]);
  if (aMatLab.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[2] << " is not a material";
    return 1;
  }

  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (aDoc->Main());
  aMatTool->RemoveMaterial (aMatLab);
  return 0;
}

// ================================================================
// Function : XSetVisMaterial
// Purpose  :
// ================================================================
static Standard_Integer XSetVisMaterial (Draw_Interpretor& , Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 3 && theNbArgs != 4)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  Handle(TDocStd_Document) aDoc;
  TDF_Label aShapeLab;
  DDocStd::GetDocument (theArgVec[1], aDoc);
  if (aDoc.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[1] << " is not a document";
    return 1;
  }

  TDF_Tool::Label (aDoc->GetData(), theArgVec[2], aShapeLab);
  Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (aDoc->Main());
  if (aShapeLab.IsNull())
  {
    // get label by shape
    TopoDS_Shape aShape = DBRep::Get (theArgVec[2]);
    if (!aShape.IsNull())
    {
      aShapeLab = aColorTool->ShapeTool()->FindShape (aShape, Standard_True);
    }
  }
  if (aShapeLab.IsNull())
  {
    Message::SendFail() << "Syntax error: " << theArgVec[2] << " is not a label not shape";
    return 1;
  }

  TDF_Label aMatLab;
  if (theNbArgs == 4)
  {
    aMatLab = findVisMaterial (aDoc, theArgVec[3]);
    if (aMatLab.IsNull())
    {
      Message::SendFail() << "Syntax error: " << theArgVec[3] << " is not a material";
      return 1;
    }
  }

  Handle(XCAFDoc_VisMaterialTool) aMatTool = XCAFDoc_DocumentTool::VisMaterialTool (aDoc->Main());
  aMatTool->SetShapeMaterial (aShapeLab, aMatLab);
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

  di.Add ("XGetAllVisMaterials","Doc [{-names|-labels}=-names]"
          "\t: Print all visualization materials defined in document",
          __FILE__, XGetAllVisMaterials, g);
  di.Add ("XGetVisMaterial","Doc {Material|Shape}"
          "\t: Print visualization material properties",
          __FILE__, XGetVisMaterial, g);
  di.Add ("XAddVisMaterial",
          "Doc Material"
          "\n\t\t: [-transparency 0..1] [-alphaMode {Opaque|Mask|Blend|BlendAuto} CutOffValue] [-refractionIndex 1..3]"
          "\n\t\t: [-diffuse   RGB] [-diffuseTexture ImagePath]"
          "\n\t\t: [-specular  RGB] [-ambient RGB] [-emissive  RGB] [-shininess 0..1]"
          "\n\t\t: [-baseColor RGB] [-baseColorTexture ImagePath]"
          "\n\t\t: [-emissiveFactor RGB] [-emissiveTexture ImagePath]"
          "\n\t\t: [-metallic 0..1] [-roughness 0..1] [-metallicRoughnessTexture ImagePath]"
          "\n\t\t: [-occlusionTexture ImagePath] [-normalTexture ImagePath]"
          "\n\t\t: [-doubleSided {0|1}]"
          "\n\t\t: Add material into Document's material table.",
          __FILE__, XAddVisMaterial, g);
  di.Add ("XRemoveVisMaterial","Doc Material"
          "\t: Remove material in document from material table",
          __FILE__, XRemoveVisMaterial, g);
  di.Add ("XSetVisMaterial", "Doc Shape Material"
          "\t: Set material to shape",
          __FILE__, XSetVisMaterial, g);
  di.Add ("XUnsetVisMaterial", "Doc Shape"
          "\t: Unset material from shape",
          __FILE__, XSetVisMaterial, g);
}
