// Created on: 1998-11-12
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <ViewerTest.hxx>

#include <AIS_PlaneTrihedron.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>

#include <Font_BRepFont.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <Font_FontMgr.hxx>
#include <Message.hxx>
#include <NCollection_List.hxx>

#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d.hxx>

#include <AIS_Shape.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_PointCloud.hxx>
#include <BRepLib_PointCloudShape.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_EventManager.hxx>

#include <TopoDS_Solid.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <Draw_Window.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_Shape.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <GC_MakePlane.hxx>
#include <gp_Circ.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>
#include <TopLoc_Location.hxx>

#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <Aspect_Window.hxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfTriangleFans.hxx>
#include <Graphic3d_ArrayOfTriangleStrips.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <Graphic3d_ArrayOfQuadrangleStrips.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>
#include <Graphic3d_AttribBuffer.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Standard_Real.hxx>

#include <AIS_Circle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_Circle.hxx>
#include <GC_MakeCircle.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdFail_NotDone.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <TopoDS_Wire.hxx>

#include <AIS_MultipleConnectedInteractive.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <AIS_TextLabel.hxx>
#include <TopLoc_Location.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <StdPrs_Curve.hxx>

#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>

#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAttribute.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_VertexDrawMode.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ToolCylinder.hxx>
#include <Prs3d_ToolSphere.hxx>
#include <Prs3d_ToolTorus.hxx>

#include <Image_AlienPixMap.hxx>
#include <TColStd_HArray1OfAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern Handle(AIS_InteractiveContext)& TheAISContext();

namespace
{
  static bool convertToColor (const Handle(TColStd_HSequenceOfAsciiString)& theColorValues,
                              Quantity_Color& theColor)
  {
    const char* anArgs[3] =
    {
      theColorValues->Size() >= 1 ? theColorValues->Value (1).ToCString() : "",
      theColorValues->Size() >= 2 ? theColorValues->Value (2).ToCString() : "",
      theColorValues->Size() >= 3 ? theColorValues->Value (3).ToCString() : ""
    };
    return Draw::ParseColor (theColorValues->Size(), anArgs, theColor) != 0;
  }

  static bool convertToDatumPart (const TCollection_AsciiString& theValue,
                                  Prs3d_DatumParts& theDatumPart)
  {
    TCollection_AsciiString aValue = theValue;
    aValue.LowerCase();
    if      (aValue == "origin")  theDatumPart = Prs3d_DatumParts_Origin;
    else if (aValue == "xaxis")   theDatumPart = Prs3d_DatumParts_XAxis;
    else if (aValue == "yaxis")   theDatumPart = Prs3d_DatumParts_YAxis;
    else if (aValue == "zaxis")   theDatumPart = Prs3d_DatumParts_ZAxis;
    else if (aValue == "xarrow")  theDatumPart = Prs3d_DatumParts_XArrow;
    else if (aValue == "yarrow")  theDatumPart = Prs3d_DatumParts_YArrow;
    else if (aValue == "zarrow")  theDatumPart = Prs3d_DatumParts_ZArrow;
    else if (aValue == "xoyaxis") theDatumPart = Prs3d_DatumParts_XOYAxis;
    else if (aValue == "yozaxis") theDatumPart = Prs3d_DatumParts_YOZAxis;
    else if (aValue == "xozaxis") theDatumPart = Prs3d_DatumParts_XOZAxis;
    else if (aValue == "whole")   theDatumPart = Prs3d_DatumParts_None;
    else
    {
      return false;
    }
    return true;
  }

  static void convertToDatumParts (const TCollection_AsciiString& theValue,
                                   NCollection_List<Prs3d_DatumParts>& theParts)
  {
    TCollection_AsciiString aValue = theValue;
    const Standard_Integer aSplitPos = theValue.Search ("|");
    Prs3d_DatumParts aPart = Prs3d_DatumParts_None;
    if (aSplitPos > 0)
    {
      convertToDatumParts (theValue.SubString (aSplitPos + 1, theValue.Length()), theParts);
      if (aSplitPos == 1) // first symbol
      {
        return;
      }
      aValue = theValue.SubString (1, aSplitPos - 1);
    }
    if (convertToDatumPart (aValue, aPart))
    {
      theParts.Append (aPart);
    }
  }

  static bool convertToDatumAttribute (const TCollection_AsciiString& theValue,
                                       Prs3d_DatumAttribute& theAttribute)
  {
    TCollection_AsciiString aValue = theValue;
    aValue.LowerCase();
    if      (aValue == "xaxislength")       theAttribute = Prs3d_DatumAttribute_XAxisLength;
    else if (aValue == "yaxislength")       theAttribute = Prs3d_DatumAttribute_YAxisLength;
    else if (aValue == "zaxislength")       theAttribute = Prs3d_DatumAttribute_ZAxisLength;
    else if (aValue == "tuberadiuspercent") theAttribute = Prs3d_DatumAttribute_ShadingTubeRadiusPercent;
    else if (aValue == "coneradiuspercent") theAttribute = Prs3d_DatumAttribute_ShadingConeRadiusPercent;
    else if (aValue == "conelengthpercent") theAttribute = Prs3d_DatumAttribute_ShadingConeLengthPercent;
    else if (aValue == "originradiuspercent") theAttribute = Prs3d_DatumAttribute_ShadingOriginRadiusPercent;
    else if (aValue == "shadingnumberoffacettes") theAttribute = Prs3d_DatumAttribute_ShadingNumberOfFacettes;
    else
      return false;
    return true;
  }

  static void convertToDatumAttributes (const TCollection_AsciiString& theValue,
                                        NCollection_List<Prs3d_DatumAttribute>& theAttributes)
  {
    TCollection_AsciiString aValue = theValue;
    const Standard_Integer aSplitPos = theValue.Search ("|");
    Prs3d_DatumAttribute anAttribute = Prs3d_DatumAttribute_XAxisLength;
    if (aSplitPos > 0)
    {
      convertToDatumAttributes (theValue.SubString (aSplitPos + 1, theValue.Length()), theAttributes);
      if (aSplitPos == 1) // first symbol
      {
        return;
      }
      aValue = theValue.SubString (1, aSplitPos - 1);
    }
    if (convertToDatumAttribute (aValue, anAttribute))
    {
      theAttributes.Append (anAttribute);
    }
  }

  static bool convertToDatumAxes (const TCollection_AsciiString& theValue,
                                  Prs3d_DatumAxes& theDatumAxes)
  {
    TCollection_AsciiString aValue = theValue;
    aValue.LowerCase();
    if      (aValue == "x")   theDatumAxes = Prs3d_DatumAxes_XAxis;
    else if (aValue == "y")   theDatumAxes = Prs3d_DatumAxes_YAxis;
    else if (aValue == "z")   theDatumAxes = Prs3d_DatumAxes_ZAxis;
    else if (aValue == "xy")  theDatumAxes = Prs3d_DatumAxes_XYAxes;
    else if (aValue == "zy")  theDatumAxes = Prs3d_DatumAxes_YZAxes;
    else if (aValue == "xz")  theDatumAxes = Prs3d_DatumAxes_XZAxes;
    else if (aValue == "xyz") theDatumAxes = Prs3d_DatumAxes_XYZAxes;
    else
    {
      return false;
    }
    return true;
  }

  static Standard_Boolean setTrihedronParams (Standard_Integer  theArgsNb,
                                              const char** theArgVec,
                                              Handle(AIS_Trihedron) theTrihedron)
  {
    NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
    TCollection_AsciiString aParseKey;
    for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
    {
      TCollection_AsciiString anArg (theArgVec [anArgIt]);
      if (anArg.Value (1) == '-'
      && !anArg.IsRealValue (Standard_True))
      {
        aParseKey = anArg;
        aParseKey.Remove (1);
        aParseKey.LowerCase();
        std::string aKey = aParseKey.ToCString();
        aMapOfArgs.Bind (aParseKey, new TColStd_HSequenceOfAsciiString());
        continue;
      }

      if (aParseKey.IsEmpty())
      {
        continue;
      }

      aMapOfArgs (aParseKey)->Append (anArg);
    }

    // Check parameters
    if ((aMapOfArgs.IsBound ("xaxis") && !aMapOfArgs.IsBound ("zaxis"))
    || (!aMapOfArgs.IsBound ("xaxis") &&  aMapOfArgs.IsBound ("zaxis")))
    {
      Message::SendFail ("Syntax error: -xaxis and -zaxis parameters are to set together");
      return Standard_False;
    }

    Handle(TColStd_HSequenceOfAsciiString) aValues;
    Handle(Geom_Axis2Placement) aComponent = theTrihedron->Component();
    if (aMapOfArgs.Find ("origin", aValues))
    {
      aComponent->SetLocation (gp_Pnt (aValues->Value (1).RealValue(),
                                       aValues->Value (2).RealValue(),
                                       aValues->Value (3).RealValue()));
    }
    Handle(TColStd_HSequenceOfAsciiString) aXValues, aZValues;
    if (aMapOfArgs.Find ("xaxis", aXValues) && aMapOfArgs.Find ("zaxis", aZValues))
    {
      gp_Dir aXDir (aXValues->Value (1).RealValue(),
                    aXValues->Value (2).RealValue(),
                    aXValues->Value (3).RealValue());

      gp_Dir aZDir (aZValues->Value (1).RealValue(),
                    aZValues->Value (2).RealValue(),
                    aZValues->Value (3).RealValue());

      if (!aZDir.IsNormal (aXDir, M_PI / 180.0))
      {
        Message::SendFail ("Syntax error - parameters 'xaxis' and 'zaxis' are not applied as VectorX is not normal to VectorZ");
        return Standard_False;
      }

      aComponent->SetAx2 (gp_Ax2 (aComponent->Location(), aZDir, aXDir));
    }

    if (aMapOfArgs.Find ("dispmode", aValues))
    {
      TCollection_AsciiString aValue (aValues->Value (1));
      bool isWireframe = true;
      if (aValue.IsEqual ("sh") || aValue.IsEqual ("shading"))
        isWireframe = false;
      theTrihedron->SetDatumDisplayMode (isWireframe ? Prs3d_DM_WireFrame
                                                     : Prs3d_DM_Shaded);
    }

    if (aMapOfArgs.Find ("hidelabels", aValues))
    {
      Standard_Boolean toHideLabels = Standard_True;
      if (aValues->Size() == 1)
      {
        Draw::ParseOnOff (aValues->First().ToCString(), toHideLabels);
      }
      else if (aValues->Size() != 0)
      {
        Message::SendFail ("Syntax error: -hidelabels expects parameter 'on' or 'off' after");
        return Standard_False;
      }

      if (!theTrihedron->Attributes()->HasOwnDatumAspect())
      {
        theTrihedron->Attributes()->SetDatumAspect (new Prs3d_DatumAspect());
      }
      theTrihedron->Attributes()->DatumAspect()->SetDrawLabels (!toHideLabels);
    }

    if (aMapOfArgs.Find ("hidearrows", aValues))
    {
      Standard_Boolean toHideArrows = Standard_True;
      if (aValues->Size() == 1)
      {
        Draw::ParseOnOff (aValues->First().ToCString(), toHideArrows);
      }
      else if (aValues->Size() != 0)
      {
        Message::SendFail ("Syntax error: -hidearrows expects parameter 'on' or 'off' after");
        return Standard_False;
      }

      if (!theTrihedron->Attributes()->HasOwnDatumAspect())
      {
        theTrihedron->Attributes()->SetDatumAspect (new Prs3d_DatumAspect());
      }
      theTrihedron->Attributes()->DatumAspect()->SetDrawArrows (!toHideArrows);
    }

    if (aMapOfArgs.Find ("color", aValues))
    {
      NCollection_List<Prs3d_DatumParts> aParts;
      if (aValues->Size() < 2)
      {
        Message::SendFail ("Syntax error: -color wrong parameters");
        return Standard_False;
      }

      convertToDatumParts (aValues->Value(1), aParts);
      aValues->Remove (1); // datum part is processed
      Quantity_Color aColor;
      if (!convertToColor (aValues, aColor))
      {
        Message::SendFail ("Syntax error: -color wrong parameters");
        return Standard_False;
      }

      for (NCollection_List<Prs3d_DatumParts>::Iterator anIterator (aParts); anIterator.More(); anIterator.Next())
      {
        Prs3d_DatumParts aDatumPart = anIterator.Value();
        if (aDatumPart == Prs3d_DatumParts_None)
        {
          theTrihedron->SetColor (aColor);
        }
        else
        {
          theTrihedron->SetDatumPartColor (aDatumPart, aColor);
        }
      }
    }

    if (aMapOfArgs.Find ("textcolor", aValues))
    {
      Prs3d_DatumParts aDatumPart = Prs3d_DatumParts_None;
      if (!aValues->IsEmpty()
        && convertToDatumPart (aValues->First(), aDatumPart)
        && aDatumPart >= Prs3d_DatumParts_XAxis
        && aDatumPart <= Prs3d_DatumParts_ZAxis)
      {
        aValues->Remove (1); // datum part is processed
      }

      Quantity_Color aColor;
      if (!convertToColor (aValues, aColor))
      {
        Message::SendFail ("Syntax error: -textcolor wrong parameters");
        return Standard_False;
      }

      if (aDatumPart != Prs3d_DatumParts_None)
      {
        theTrihedron->SetTextColor (aDatumPart, aColor);
      }
      else
      {
        theTrihedron->SetTextColor (aColor);
      }
    }

    if (aMapOfArgs.Find ("arrowcolor", aValues))
    {
      Prs3d_DatumParts aDatumPart = Prs3d_DatumParts_None;
      if (!aValues->IsEmpty()
        && convertToDatumPart (aValues->First(), aDatumPart)
        && ((aDatumPart >= Prs3d_DatumParts_XArrow && aDatumPart <= Prs3d_DatumParts_ZArrow)
         || (aDatumPart >= Prs3d_DatumParts_XAxis  && aDatumPart <= Prs3d_DatumParts_ZAxis)))
      {
        aValues->Remove (1); // datum part is processed
      }

      Quantity_Color aColor;
      if (!convertToColor (aValues, aColor))
      {
        Message::SendFail ("Syntax error: -arrowcolor wrong parameters");
        return Standard_False;
      }

      if (aDatumPart != Prs3d_DatumParts_None)
      {
        Prs3d_DatumParts anArrowPart = Prs3d_DatumAspect::ArrowPartForAxis (aDatumPart);
        theTrihedron->SetArrowColor (anArrowPart, aColor);
      }
      else
      {
        theTrihedron->SetArrowColor (aColor);
      }
    }

    if (aMapOfArgs.Find ("attribute", aValues))
    {
      NCollection_List<Prs3d_DatumAttribute> anAttributes;
      if (aValues->Size() != 2)
      {
        Message::SendFail ("Syntax error: -attribute wrong parameters");
        return Standard_False;
      }

      convertToDatumAttributes (aValues->Value (1), anAttributes);
      if (!theTrihedron->Attributes()->HasOwnDatumAspect())
        theTrihedron->Attributes()->SetDatumAspect(new Prs3d_DatumAspect());
      for (NCollection_List<Prs3d_DatumAttribute>::Iterator anIterator (anAttributes); anIterator.More(); anIterator.Next())
      {
        theTrihedron->Attributes()->DatumAspect()->SetAttribute (anIterator.Value(), aValues->Value (2).RealValue());
      }
    }

    if (aMapOfArgs.Find ("priority", aValues))
    {
      Prs3d_DatumParts aDatumPart;
      if (aValues->Size() < 2
      || !convertToDatumPart (aValues->Value (1), aDatumPart))
      {
        Message::SendFail ("Syntax error: -priority wrong parameters");
        return Standard_False;
      }
      theTrihedron->SetSelectionPriority (aDatumPart, aValues->Value (2).IntegerValue());
    }

    if (aMapOfArgs.Find ("labels", aValues)
     || aMapOfArgs.Find ("label", aValues))
    {
      Prs3d_DatumParts aDatumPart = Prs3d_DatumParts_None;
      if (aValues->Size() >= 2
       && convertToDatumPart(aValues->Value(1), aDatumPart)
       && aDatumPart >= Prs3d_DatumParts_XAxis
       && aDatumPart <= Prs3d_DatumParts_ZAxis) // labels are set to axes only
      {
        theTrihedron->SetLabel (aDatumPart, aValues->Value (2));
      }
      else
      {
        Message::SendFail ("Syntax error: -labels wrong parameters");
        return Standard_False;
      }
    }

    if (aMapOfArgs.Find ("drawaxes", aValues))
    {
      Prs3d_DatumAxes aDatumAxes = Prs3d_DatumAxes_XAxis;
      if (aValues->Size() < 1
      || !convertToDatumAxes (aValues->Value (1), aDatumAxes))
      {
        Message::SendFail ("Syntax error: -drawaxes wrong parameters");
        return Standard_False;
      }
      if (!theTrihedron->Attributes()->HasOwnDatumAspect())
        theTrihedron->Attributes()->SetDatumAspect(new Prs3d_DatumAspect());
      theTrihedron->Attributes()->DatumAspect()->SetDrawDatumAxes (aDatumAxes);
    }
    return Standard_True;
  }

  //! Auxiliary function to parse font aspect style argument
  static Standard_Boolean parseFontStyle (const TCollection_AsciiString& theArg,
                                          Font_FontAspect&               theAspect)
  {
    if (theArg == "regular"
     || *theArg.ToCString() == 'r')
    {
      theAspect = Font_FA_Regular;
      return Standard_True;
    }
    else if (theArg == "bolditalic"
          || theArg == "bold-italic"
          || theArg == "italic-bold"
          || theArg == "italicbold")
    {
      theAspect = Font_FA_BoldItalic;
      return Standard_True;
    }
    else if (theArg == "bold"
          || *theArg.ToCString() == 'b')
    {
      theAspect = Font_FA_Bold;
      return Standard_True;
    }
    else if (theArg == "italic"
          || *theArg.ToCString() == 'i')
    {
      theAspect = Font_FA_Italic;
      return Standard_True;
    }
    return Standard_False;
  }

  //! Auxiliary function to parse font strict level argument
  static Standard_Integer parseFontStrictLevel (const Standard_Integer theArgNb,
                                                const char**           theArgVec,
                                                Font_StrictLevel&      theLevel)
  {
    if (theArgNb >= 1)
    {
      TCollection_AsciiString anArg (theArgVec[0]);
      anArg.LowerCase();
      if (anArg == "any")
      {
        theLevel = Font_StrictLevel_Any;
        return 1;
      }
      else if (anArg == "aliases")
      {
        theLevel = Font_StrictLevel_Aliases;
        return 1;
      }
      else if (anArg == "strict")
      {
        theLevel = Font_StrictLevel_Strict;
        return 1;
      }
    }
    theLevel = Font_StrictLevel_Strict;
    return 0;
  }
}

//==============================================================================
//function : Vtrihedron 2d
//purpose  : Create a plane with a 2D  trihedron from a faceselection
//Draw arg : vtri2d  name
//==============================================================================
static int VTrihedron2D (Draw_Interpretor& /*theDI*/,
                         Standard_Integer  theArgsNum,
                         const char**      theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  if (theArgsNum != 2)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.";
    return 1;
  }

  TopTools_ListOfShape aShapes;
  ViewerTest::GetSelectedShapes (aShapes);

  if (aShapes.Extent() != 1)
  {
    Message::SendFail ("Error: wrong number of selected shapes.");
    return 1;
  }

  const TopoDS_Shape& aShape = aShapes.First();

  TopoDS_Face     aFace = TopoDS::Face (aShape);
  TopExp_Explorer aFaceExp (aFace, TopAbs_EDGE);
  TopoDS_Edge     anEdge0 = TopoDS::Edge (aFaceExp.Current());

  gp_Pnt A,B,C;
  if (aFaceExp.More())
  {
    aFaceExp.Next();
    TopoDS_Edge anEdge1 = TopoDS::Edge (aFaceExp.Current() );
    BRepAdaptor_Curve aCurve0 (anEdge0);
    BRepAdaptor_Curve aCurve1 (anEdge1);
    A = aCurve1.Value (0.1);
    B = aCurve1.Value (0.9);
    C = aCurve0.Value (0.5);
  }
  else
  {
    BRepAdaptor_Curve aCurve0 (anEdge0);
    A = aCurve0.Value (0.1);
    B = aCurve0.Value (0.9);
    C = aCurve0.Value (0.5);
  }

  GC_MakePlane aMkPlane (A,B,C);

  Handle(AIS_PlaneTrihedron) anAISPlaneTri = new AIS_PlaneTrihedron (aMkPlane.Value());
  TCollection_AsciiString aName (theArgVec[1]);

  VDisplayAISObject (aName, anAISPlaneTri);

  return 0;
}

//=======================================================================
//function : VTrihedron
//purpose  :
//=======================================================================
static int VTrihedron (Draw_Interpretor& ,
                       Standard_Integer theArgsNb,
                       const char** theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  if (theArgsNb < 2)
  {
    Message::SendFail ("Syntax error: the wrong number of input parameters");
    return 1;
  }

  TCollection_AsciiString aName (theArgVec[1]);
  gp_Pln aWorkingPlane;
  Standard_Boolean toUpdate = Standard_True;

  NCollection_DataMap<TCollection_AsciiString, Standard_Real> aRealParams;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> aStringParams;

  Handle(AIS_Trihedron) aTrihedron;
  Handle(AIS_InteractiveObject) anObject;
  if (GetMapOfAIS().Find2 (aName, anObject))
  {
    aTrihedron = Handle(AIS_Trihedron)::DownCast (anObject);
    if (aTrihedron.IsNull())
    {
      Message::SendFail ("Syntax error: no trihedron with this name");
      return 1;
    }
  }
  else
  {
    Handle(Geom_Axis2Placement) aPlacement = new Geom_Axis2Placement (gp_Pnt (0.0, 0.0, 0.0),
                                                                      gp::DZ(), gp::DX());
    aTrihedron = new AIS_Trihedron (aPlacement);
  }

  if (!setTrihedronParams (theArgsNb, theArgVec, aTrihedron))
  {
    return 1;
  }

  // Redisplay a dimension after parameter changing.
  if (ViewerTest::GetAISContext()->IsDisplayed (aTrihedron))
  {
    ViewerTest::GetAISContext()->Redisplay (aTrihedron, toUpdate);
  }
  else
  {
    VDisplayAISObject (theArgVec[1], aTrihedron);
  }

  return 0;
}

//==============================================================================
//function : VSize
//author   : ege
//purpose  : Change the size of a named or selected trihedron
//           if no name : it affects the trihedrons witch are selected otherwise nothing is donne
//           if no value, the value is set at 100 by default
//Draw arg : vsize [name] [size]
//==============================================================================
static int VSize (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (TheAISContext().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aName;
  Standard_Real aSize = 0.0;
  switch (argc)
  {
    case 1:
    {
      aSize = 100;
      break;
    }
    case 2:
    {
      aSize = Draw::Atof (argv[1]);
      break;
    }
    case 3:
    {
      aName = argv[1];
      aSize = Draw::Atof (argv[2]);
      break;
    }
    default:
    {
      di << "Syntax error";
      return 1;
    }
  }

  NCollection_Sequence<Handle(AIS_Trihedron)> aTrihedrons;
  if (!aName.IsEmpty())
  {
    Handle(AIS_InteractiveObject) aShape;
    if (GetMapOfAIS().Find2 (aName, aShape))
    {
      if (Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast (aShape))
      {
        aTrihedrons.Append (aTrihedron);
      }
      else
      {
        di << "Syntax error: " << aName << " is not a trihedron";
        return 1;
      }
    }
  }
  else if (TheAISContext()->NbSelected() > 0)
  {
    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS()); it.More(); it.Next())
    {
      const Handle(AIS_InteractiveObject)& aShape = it.Key1();
      if (!aShape.IsNull()
       && TheAISContext()->IsSelected (aShape))
      {
        if (Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast (aShape))
        {
          aTrihedrons.Append (aTrihedron);
        }
      }
    }
  }

  for (NCollection_Sequence<Handle(AIS_Trihedron)>::Iterator anObjIter (aTrihedrons); anObjIter.More(); anObjIter.Next())
  {
    const Handle(AIS_Trihedron)& aTrihedron = anObjIter.Value();
    Quantity_Color aColor = Quantity_NOC_BLACK;
    const bool hasColor = aTrihedron->HasColor();
    if (hasColor)
    {
      aTrihedron->Color (aColor);
    }

    aTrihedron->SetSize (aSize);
    if (hasColor) { aTrihedron->SetColor (aColor); }
    else          { aTrihedron->UnsetColor(); }

    TheAISContext()->Redisplay (aTrihedron, Standard_False);
  }
  if (!aTrihedrons.IsEmpty())
  {
    TheAISContext()->UpdateCurrentViewer();
  }

  return 0;
}

//==============================================================================

//==============================================================================
//function : VPlaneTrihedron
//purpose  : Create a plane from a trihedron selection. If no arguments are set, the default
//Draw arg : vplanetri  name
//==============================================================================
#include <AIS_Plane.hxx>



static int VPlaneTrihedron (Draw_Interpretor& di, Standard_Integer argc, const char** argv)

{
  // Verification des arguments
  if ( argc!=2) {di<<argv[0]<<" error\n"; return 1;}

  if (TheAISContext().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (TheAISContext()->NbSelected() != 1)
  {
    Message::SendFail ("Error: Wrong number of selected shapes.");
    return 1;
  }

  TheAISContext()->InitSelected();
  Handle(AIS_InteractiveObject) aTest = TheAISContext()->SelectedInteractive();
  Handle(AIS_Plane) aPlane = Handle(AIS_Plane)::DownCast (aTest);
  if (aPlane.IsNull())
  {
    Message::SendFail ("Error: Selected shape is not a plane.");
    return 1;
  }

  VDisplayAISObject (argv[1], aPlane);

  return 0;
}



//==============================================================================
// Fonction        First click      2de click
//
// vaxis           vertex           vertex
//                 edge             None
// vaxispara       edge             vertex
// vaxisortho      edge             Vertex
// vaxisinter      Face             Face
//==============================================================================

//==============================================================================
//function : VAxisBuilder
//purpose  :
//Draw arg : vaxis AxisName Xa Ya Za Xb Yb Zb
//==============================================================================
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <Geom_Line.hxx>

static int VAxisBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Declarations
  Standard_Boolean HasArg;
  TCollection_AsciiString name;

  // Verification
  if (argc<2 || argc>8 ) {di<<" Syntaxe error\n";return 1;}
  if (argc==8) HasArg=Standard_True;
  else HasArg=Standard_False;

  name=argv[1];

  TopTools_ListOfShape aShapes;
  ViewerTest::GetSelectedShapes (aShapes);

  // Cas ou il y a des arguments
  // Purpose: Teste le constructeur AIS_Axis::AIS_Axis(x: Line from Geom)
  if (HasArg) {
    Standard_Real coord[6];
    for(Standard_Integer i=0;i<=5;i++){
      coord[i]=Draw::Atof(argv[2+i]);
    }
    gp_Pnt p1(coord[0],coord[1],coord[2]), p2(coord[3],coord[4],coord[5]) ;

    gp_Vec myVect (p1,p2);
    Handle(Geom_Line) myLine=new Geom_Line (p1 ,myVect );
    Handle(AIS_Axis) TheAxis=new AIS_Axis (myLine );
    GetMapOfAIS().Bind (TheAxis,name);
    TheAISContext()->Display(TheAxis, Standard_True);
  }

  // Pas d'arguments
  else {
    // fonction vaxis
    // Purpose: Teste le constructeur AIS_Axis::AIS_Axis (x:Axis1Placement from Geom)
    if ( !strcasecmp(argv[0], "vaxis")) {
      if (aShapes.Extent() != 2 && aShapes.Extent() != 1)
      {
        Message::SendFail ("Error: Wrong number of selected shapes.");
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      if (aShapeA.ShapeType() == TopAbs_VERTEX)
      {
        if (aShapes.Extent() != 2)
        {
          Message::SendFail ("Error: Wrong number of selected shapes.");
          return 1;
        }

        const TopoDS_Shape& aShapeB = aShapes.Last();
        if (aShapeB.ShapeType() != TopAbs_VERTEX)
        {
          Message::SendFail ("Syntax error: You should select two vertices or one edge.");
          return 1;
        }

        // Construction de l'axe
        gp_Pnt A = BRep_Tool::Pnt (TopoDS::Vertex (aShapeA));
        gp_Pnt B = BRep_Tool::Pnt (TopoDS::Vertex (aShapeB));
        gp_Vec V (A,B);
        gp_Dir D (V);
        Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (A,D);
        Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
        GetMapOfAIS().Bind (TheAxis,name);
        TheAISContext()->Display (TheAxis, Standard_True);
      }
      else
      {
        TopoDS_Edge    ed =TopoDS::Edge (aShapeA);
        TopoDS_Vertex  Va,Vb;
        TopExp::Vertices(ed,Va,Vb );
        gp_Pnt A=BRep_Tool::Pnt(Va);
        gp_Pnt B=BRep_Tool::Pnt(Vb);
        gp_Vec  V (A,B);
        gp_Dir   D (V);
        Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (A,D);
        Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
        GetMapOfAIS().Bind (TheAxis,name);
        TheAISContext()->Display (TheAxis, Standard_True);
      }

    }

    // Fonction axispara
    // Purpose: Teste le constructeur AIS_Axis::AIS_Axis(x: Axis2Placement from Geom, y: TypeOfAxis from AIS)
    else if ( !strcasecmp(argv[0], "vaxispara"))
    {
      if (aShapes.Extent() != 2)
      {
        Message::SendFail ("Error: Wrong number of selected shapes.");
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();
      if (!(aShapeA.ShapeType() == TopAbs_EDGE
         && aShapeB.ShapeType() == TopAbs_VERTEX))
      {
        Message::SendFail ("Syntax error: You should select face and then vertex.");
        return 1;
      }

      TopoDS_Edge    ed=TopoDS::Edge (aShapeA);
      gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(aShapeB));
      TopoDS_Vertex  Va,Vc;
      TopExp::Vertices(ed,Va,Vc );
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      gp_Vec  V (A,C);
      gp_Dir   D (V);
      Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (B,D);
      Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
      GetMapOfAIS().Bind (TheAxis,name);
      TheAISContext()->Display (TheAxis, Standard_True);

    }

    // Fonction axisortho
    else
    {
      if (aShapes.Extent() != 2)
      {
        Message::SendFail ("Error: Wrong number of selected shapes.");
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();
      if (!(aShapeA.ShapeType() == TopAbs_EDGE
         && aShapeB.ShapeType() == TopAbs_VERTEX))
      {
        Message::SendFail ("Syntax error: You should select face and then vertex.");
        return 1;
      }

      // Construction de l'axe
      TopoDS_Edge    ed=TopoDS::Edge(aShapeA) ;
      gp_Pnt B=BRep_Tool::Pnt(TopoDS::Vertex(aShapeB) );
      TopoDS_Vertex  Va,Vc;
      TopExp::Vertices(ed,Va,Vc );
      gp_Pnt A=BRep_Tool::Pnt(Va);
      gp_Pnt C=BRep_Tool::Pnt(Vc);
      gp_Pnt E(A.Y()+A.Z()-C.Y()-C.Z()  ,C.X()-A.X() ,C.X()-A.X() );
      gp_Vec  V (A,E);
      gp_Dir   D (V);
      Handle(Geom_Axis1Placement) OrigineAndVect=new Geom_Axis1Placement (B,D);
      Handle(AIS_Axis) TheAxis=new AIS_Axis (OrigineAndVect);
      GetMapOfAIS().Bind (TheAxis,name);
      TheAISContext()->Display (TheAxis, Standard_True);

    }

  }
  return 0;
}


//==============================================================================
// Fonction        First click      Result
//
// vpoint          vertex           AIS_Point=Vertex
//                 edge             AIS_Point=Middle of the edge
//==============================================================================

//==============================================================================
//function : VPointBuilder
//purpose  :
//==============================================================================
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>

static int VPointBuilder(Draw_Interpretor& ,
                         Standard_Integer theArgNb,
                         const char** theArgVec)
{
  TCollection_AsciiString aName;
  gp_Pnt aPnt (RealLast(), 0.0, 0.0);
  bool is2d = false, isNoSel = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-2d")
    {
      is2d = true;
    }
    else if (anArg == "-nosel"
          || anArg == "-noselection")
    {
      isNoSel = true;
    }
    else if (aName.IsEmpty())
    {
      aName = theArgVec[anArgIter];
    }
    else if (aPnt.X() == RealLast()
          && anArgIter + 1 < theArgNb
          && Draw::ParseReal (theArgVec[anArgIter + 0], aPnt.ChangeCoord().ChangeCoord (1))
          && Draw::ParseReal (theArgVec[anArgIter + 1], aPnt.ChangeCoord().ChangeCoord (2)))
    {
      if (anArgIter + 2 < theArgNb
       && TCollection_AsciiString (theArgVec[anArgIter + 2]) != "-2d"
       && Draw::ParseReal (theArgVec[anArgIter + 2], aPnt.ChangeCoord().ChangeCoord (3)))
      {
        anArgIter += 2;
      }
      else
      {
        anArgIter += 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at argument '" << anArg << "'\n";
      return 1;
    }
  }

  if (aPnt.X() == RealLast())
  {
    TopTools_ListOfShape aShapes;
    ViewerTest::GetSelectedShapes (aShapes);
    TopoDS_Shape aShapeA;
    if (aShapes.Extent() == 1)
    {
      aShapeA = aShapes.First();
    }
    switch (!aShapeA.IsNull() ? aShapeA.ShapeType() : TopAbs_SHAPE)
    {
      case TopAbs_VERTEX:
      {
        aPnt = BRep_Tool::Pnt (TopoDS::Vertex (aShapeA));
        break;
      }
      case TopAbs_EDGE: // edge middle point
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge (aShapeA);
        TopoDS_Vertex aVertPair[2];
        TopExp::Vertices (anEdge, aVertPair[0], aVertPair[1]);
        const gp_Pnt A = BRep_Tool::Pnt (aVertPair[0]);
        const gp_Pnt B = BRep_Tool::Pnt (aVertPair[1]);
        aPnt = (A.XYZ() + B.XYZ()) / 2;
        break;
      }
      default:
      {
        Message::SendFail() << "Error: Wrong number of selected shapes.\n"
                            << "\tYou should select one edge or vertex.";
        return 1;
      }
    }
  }

  if (is2d)
  {
    aPnt.SetY (-aPnt.Y());
  }
  Handle(Geom_CartesianPoint ) aGeomPoint = new Geom_CartesianPoint (aPnt);
  Handle(AIS_Point) aPointPrs = new AIS_Point (aGeomPoint);
  if (is2d)
  {
    aPointPrs->SetTransformPersistence (new Graphic3d_TransformPers (Graphic3d_TMF_2d, Aspect_TOTP_LEFT_UPPER));
    aPointPrs->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  }
  ViewerTest::Display (aName, aPointPrs);
  if (isNoSel)
  {
    ViewerTest::GetAISContext()->Deactivate (aPointPrs);
  }
  return 0;
}

//==============================================================================
// Function        1st click   2de click  3de click
// vplane          Vertex      Vertex     Vertex
//                 Vertex      Edge
//                 Edge        Vertex
//                 Face
// vplanepara      Face        Vertex
//                 Vertex      Face
// vplaneortho     Face        Edge
//                 Edge        Face
//==============================================================================

//==============================================================================
//function : VPlaneBuilder
//purpose  : Build an AIS_Plane from selected entities or Named AIS components
//Draw arg : vplane PlaneName [AxisName]  [PointName] [TypeOfSensitivity]
//                            [PointName] [PointName] [PointName] [TypeOfSensitivity]
//                            [PlaneName] [PointName] [TypeOfSensitivity]
//==============================================================================

static Standard_Integer VPlaneBuilder (Draw_Interpretor& /*di*/,
                                       Standard_Integer argc,
                                       const char** argv)
{
  // Declarations
  Standard_Boolean hasArg;
  TCollection_AsciiString aName;

  // Verification
  if (argc<2 || argc>6 )
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }
  if (argc == 6 || argc==5 || argc==4)
    hasArg=Standard_True;
  else 
    hasArg=Standard_False;

  aName=argv[1];

  // There are some arguments
  if (hasArg)
  {
    Handle(AIS_InteractiveObject) aShapeA;
    if (!GetMapOfAIS().Find2 (argv[2], aShapeA))
    {
      Message::SendFail ("Syntax error: 1st name is not displayed");
      return 1;
    }

    // The first argument is an AIS_Point
    if (!aShapeA.IsNull()
      && aShapeA->Type() == AIS_KindOfInteractive_Datum
      && aShapeA->Signature() == 1)
    {
      // The second argument must also be an AIS_Point
      Handle(AIS_InteractiveObject) aShapeB;
      if (argc<5 || !GetMapOfAIS().Find2 (argv[3], aShapeB))
      {
        Message::SendFail ("Syntax error: 2nd name is not displayed");
        return 1;
      }

      // If B is not an AIS_Point
      if (aShapeB.IsNull()
      || !(aShapeB->Type() == AIS_KindOfInteractive_Datum
        && aShapeB->Signature() == 1))
      {
        Message::SendFail ("Syntax error: 2nd object is expected to be an AIS_Point");
        return 1;
      }

      // The third object is an AIS_Point
      Handle(AIS_InteractiveObject) aShapeC;
      if (!GetMapOfAIS().Find2(argv[4], aShapeC))
      {
        Message::SendFail ("Syntax error: 3d name is not displayed");
        return 1;
      }

      // If C is not an AIS_Point
      if (aShapeC.IsNull()
      || !(aShapeC->Type() == AIS_KindOfInteractive_Datum
        && aShapeC->Signature() == 1))
      {
        Message::SendFail ("Syntax error: 3d object is expected to be an AIS_Point");
        return 1;
      }

      // Treatment of objects A, B, C
      // Downcast an AIS_IO to AIS_Point
      Handle(AIS_Point) anAISPointA = Handle(AIS_Point)::DownCast (aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast (aShapeB);
      Handle(AIS_Point) anAISPointC = Handle(AIS_Point)::DownCast (aShapeC);

      Handle(Geom_CartesianPoint ) aCartPointA = Handle(Geom_CartesianPoint)::DownCast (anAISPointA->Component());
      Handle(Geom_CartesianPoint ) aCartPointB = Handle(Geom_CartesianPoint)::DownCast (anAISPointB->Component());
      Handle(Geom_CartesianPoint ) aCartPointC = Handle(Geom_CartesianPoint)::DownCast (anAISPointC->Component());

      // Verification that the three points are different
      if (Abs(aCartPointB->X()-aCartPointA->X()) <= Precision::Confusion()
       && Abs(aCartPointB->Y()-aCartPointA->Y()) <= Precision::Confusion()
       && Abs(aCartPointB->Z()-aCartPointA->Z()) <= Precision::Confusion())
      {
        // B=A
        Message::SendFail ("Error: same points");
        return 1;
      }
      if (Abs(aCartPointC->X()-aCartPointA->X()) <= Precision::Confusion()
       && Abs(aCartPointC->Y()-aCartPointA->Y()) <= Precision::Confusion()
       && Abs(aCartPointC->Z()-aCartPointA->Z()) <= Precision::Confusion())
      {
        // C=A
        Message::SendFail ("Error: same points");
        return 1;
      }
      if (Abs(aCartPointC->X()-aCartPointB->X()) <= Precision::Confusion()
       && Abs(aCartPointC->Y()-aCartPointB->Y()) <= Precision::Confusion()
       && Abs(aCartPointC->Z()-aCartPointB->Z()) <= Precision::Confusion())
      {
        // C=B
        Message::SendFail ("Error: same points");
        return 1;
      }

      gp_Pnt A = aCartPointA->Pnt();
      gp_Pnt B = aCartPointB->Pnt();
      gp_Pnt C = aCartPointC->Pnt();

      // Construction of AIS_Plane
      GC_MakePlane MkPlane (A,B,C);
      const Handle(Geom_Plane)& aGeomPlane = MkPlane.Value();
      Handle(AIS_Plane)  anAISPlane = new AIS_Plane (aGeomPlane);
      GetMapOfAIS().Bind (anAISPlane, aName);
      if (argc == 6)
      {
        Standard_Integer aType = Draw::Atoi (argv[5]);
        if (aType != 0 && aType != 1)
        {
          Message::SendFail("Syntax error: wrong type of sensitivity.\n"
                            "Should be one of the following values:\n"
                            "0 - Interior\n"
                            "1 - Boundary");
          return 1;
        }
        else
        {
          anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
        }
      }
      TheAISContext()->Display (anAISPlane, Standard_True);
    }
    // The first argument is an AIS_Axis
    // Creation of a plane orthogonal to the axis through a point
    else if (aShapeA->Type() == AIS_KindOfInteractive_Datum
          && aShapeA->Signature() == 2)
    {
      // The second argument should be an AIS_Point
      Handle(AIS_InteractiveObject) aShapeB;
      if (argc!=4 || !GetMapOfAIS().Find2 (argv[3], aShapeB))
      {
        Message::SendFail ("Syntax error: 2d name is not displayed");
        return 1;
      }
      // If B is not an AIS_Point
      if (aShapeB.IsNull()
      || !(aShapeB->Type() == AIS_KindOfInteractive_Datum
        && aShapeB->Signature() == 1))
      {
        Message::SendFail ("Syntax error: 2d object is expected to be an AIS_Point");
        return 1;
      }

      // Treatment of objects A and B
      Handle(AIS_Axis) anAISAxisA = Handle(AIS_Axis)::DownCast(aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast(aShapeB);

      Handle(Geom_Line ) aGeomLineA = anAISAxisA->Component();
      Handle(Geom_Point) aGeomPointB = anAISPointB->Component();

      gp_Ax1 anAxis = aGeomLineA->Position();
      Handle(Geom_CartesianPoint) aCartPointB = Handle(Geom_CartesianPoint)::DownCast(aGeomPointB);

      gp_Dir D = anAxis.Direction();
      gp_Pnt B = aCartPointB->Pnt();

      // Construction of AIS_Plane
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane(B,D);
      Handle(AIS_Plane) anAISPlane = new AIS_Plane(aGeomPlane,B );
      GetMapOfAIS().Bind (anAISPlane,aName );
      if (argc == 5)
      {
        Standard_Integer aType = Draw::Atoi (argv[4]);
        if (aType != 0 && aType != 1)
        {
          Message::SendFail ("Syntax error: wrong type of sensitivity!\n"
                             "Should be one of the following values:\n"
                             "0 - Interior\n"
                             "1 - Boundary");
          return 1;
        }
        else
        {
          anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
        }
      }
      TheAISContext()->Display (anAISPlane, Standard_True);
    }
    // The first argument is an AIS_Plane
    // Creation of a plane parallel to the plane passing through the point
    else if (aShapeA->Type() == AIS_KindOfInteractive_Datum
          && aShapeA->Signature() == 7)
    {
      // The second argument should be an AIS_Point
      Handle(AIS_InteractiveObject) aShapeB;
      if (argc!=4 || !GetMapOfAIS().Find2 (argv[3], aShapeB))
      {
        Message::SendFail ("Syntax error: 2d name is not displayed");
        return 1;
      }
      // B should be an AIS_Point
      if (aShapeB.IsNull()
       || !(aShapeB->Type()==AIS_KindOfInteractive_Datum
         && aShapeB->Signature() == 1))
      {
        Message::SendFail ("Syntax error: 2d object is expected to be an AIS_Point");
        return 1;
      }

      // Treatment of objects A and B
      Handle(AIS_Plane) anAISPlaneA = Handle(AIS_Plane)::DownCast(aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast(aShapeB);

      Handle(Geom_Plane) aNewGeomPlane= anAISPlaneA->Component();
      Handle(Geom_Point) aGeomPointB = anAISPointB->Component();

      Handle(Geom_CartesianPoint) aCartPointB = Handle(Geom_CartesianPoint)::DownCast(aGeomPointB);
      gp_Pnt B= aCartPointB->Pnt();

      // Construction of an AIS_Plane
      Handle(AIS_Plane) anAISPlane = new AIS_Plane(aNewGeomPlane, B);
      GetMapOfAIS().Bind (anAISPlane, aName);
      if (argc == 5)
      {
        Standard_Integer aType = Draw::Atoi (argv[4]);
        if (aType != 0 && aType != 1)
        {
          Message::SendFail ("Syntax error: wrong type of sensitivity!\n"
                             "Should be one of the following values:\n"
                             "0 - Interior\n"
                             "1 - Boundary");
          return 1;
        }
        else
        {
          anAISPlane->SetTypeOfSensitivity (Select3D_TypeOfSensitivity (aType));
        }
      }
      TheAISContext()->Display (anAISPlane, Standard_True);
    }
    // Error
    else
    {
      Message::SendFail ("Syntax error: 1st object is not an AIS");
      return 1;
    }
  }
  // There are no arguments
  else 
  {
    TopTools_ListOfShape aShapes;
    ViewerTest::GetSelectedShapes (aShapes);

    // Function vplane
    // Test the constructor AIS_Plane::AIS_Plane(Geom_Plane, Standard_Boolean )
    if (!strcasecmp(argv[0], "vplane"))
    {
      if (aShapes.Extent() < 1 || aShapes.Extent() > 3)
      {
        Message::SendFail() << "Error: Wront number of selected shapes.\n"
                            << "\tYou should one of variant: face, edge and vertex or three vertices.";
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      if (aShapeA.ShapeType() == TopAbs_VERTEX)
      {
        if (aShapes.Extent() == 2)
        {
          const TopoDS_Shape& aShapeB = aShapes.Last();
          if (aShapeB.ShapeType() != TopAbs_EDGE)
          {
            Message::SendFail ("Syntax error: Together with vertex should be edge.");
            return 1;
          }

          // Verify that the vertex is not on the edge ShapeB
          TopoDS_Edge anEdgeB = TopoDS::Edge(aShapeB);
          TopoDS_Vertex aVertA = TopoDS::Vertex(aShapeA);

          BRepExtrema_ExtPC OrthoProj(aVertA, anEdgeB);
          if (OrthoProj.SquareDistance(1)<Precision::Approximation())
          {
            // The vertex is on the edge
            Message::SendFail ("Error: point is on the edge");
            return 1;
          }
          else
          {
            gp_Pnt A = BRep_Tool::Pnt(aVertA);
            TopoDS_Vertex aVBa, aVBb;
            TopExp::Vertices(anEdgeB ,aVBa ,aVBb);
            gp_Pnt aBa = BRep_Tool::Pnt(aVBa);
            gp_Pnt aBb = BRep_Tool::Pnt(aVBb);
            GC_MakePlane MkPlane (A, aBa, aBb);
            const Handle(Geom_Plane)& aGeomPlane = MkPlane.Value();
            Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
            GetMapOfAIS().Bind (anAISPlane, aName);
            TheAISContext()->Display (anAISPlane, Standard_True);
          }
        }
        else if (aShapes.Extent() == 3)
        {
          TopTools_ListOfShape::Iterator anIter (aShapes);

          anIter.Next();
          const TopoDS_Shape& aShapeB = anIter.Value();

          anIter.Next();
          const TopoDS_Shape& aShapeC = anIter.Value();

          if (!(aShapeB.ShapeType() == TopAbs_VERTEX
             && aShapeC.ShapeType() == TopAbs_VERTEX))
          {
            Message::SendFail ("Syntax error: You should one of variant: face, edge and vertex or three vertices.");
            return 1;
          }

          gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(aShapeA));
          gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(aShapeB));
          gp_Pnt C = BRep_Tool::Pnt(TopoDS::Vertex(aShapeC));
          GC_MakePlane MkPlane(A, B, C);
          const Handle(Geom_Plane)& aGeomPlane = MkPlane.Value();
          Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display (anAISPlane, Standard_True);
        }
        else
        {
          Message::SendFail ("Syntax error: You should one of variant: face, edge and vertex or three vertices.");
          return 1;
        }
      }
      else if (aShapeA.ShapeType() == TopAbs_EDGE)
      {
        if (aShapes.Extent() != 2)
        {
          Message::SendFail ("Error: wrong number of selected shapes.");
          return 1;
        }

        const TopoDS_Shape& aShapeB = aShapes.Last();
        if (aShapeB.ShapeType() != TopAbs_VERTEX)
        {
          Message::SendFail ("Syntax error: Together with edge should be vertex.");
          return 1;
        }

        // Check that the vertex aShapeB is not on the edge
        TopoDS_Edge anEdgeA = TopoDS::Edge(aShapeA);
        TopoDS_Vertex aVertB = TopoDS::Vertex(aShapeB);

        BRepExtrema_ExtPC OrthoProj (aVertB, anEdgeA);
        if (OrthoProj.SquareDistance(1)<Precision::Approximation())
        {
          // The vertex is on the edge
          Message::SendFail ("Error point is on the edge");
          return 1;
        }

        gp_Pnt B = BRep_Tool::Pnt(aVertB);
        TopoDS_Vertex aVAa, aVAb;
        TopExp::Vertices(anEdgeA, aVAa, aVAb);
        gp_Pnt Aa = BRep_Tool::Pnt(aVAa);
        gp_Pnt Ab = BRep_Tool::Pnt(aVAb);
        GC_MakePlane MkPlane (B,Aa,Ab);
        const Handle(Geom_Plane)& aGeomPlane = MkPlane.Value();
        Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane);
        GetMapOfAIS().Bind (anAISPlane ,aName);
        TheAISContext()->Display (anAISPlane, Standard_True);
      }
      else if (aShapeA.ShapeType() == TopAbs_FACE)
      {
        TopoDS_Face aFace = TopoDS::Face(aShapeA);
        BRepAdaptor_Surface aSurface (aFace, Standard_False);
        if (aSurface.GetType()==GeomAbs_Plane)
        {
          gp_Pln aPlane = aSurface.Plane();
          Handle(Geom_Plane) aGeomPlane = new Geom_Plane(aPlane);
          Handle(AIS_Plane) anAISPlane = new AIS_Plane(aGeomPlane);
          GetMapOfAIS().Bind (anAISPlane, aName);
          TheAISContext()->Display (anAISPlane, Standard_True);
        }
        else
        {
          Message::SendFail ("Error: surface is not Plane");
          return 1;
        }
      }
      else
      {
        Message::SendFail ("Syntax error: You should one of variant: face, edge and vertex or three vertices");
        return 1;
      }
    }

    // Function vPlanePara
    // ===================
    // test the constructor AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt)
    else if (!strcasecmp(argv[0], "vplanepara"))
    {
      if (aShapes.Extent() != 2)
      {
        Message::SendFail ("Error: Wrong number of selected shapes.");
        return 1;
      }

      const TopoDS_Shape* aShapeA = &aShapes.First();
      const TopoDS_Shape* aShapeB = &aShapes.Last();
      if (aShapeA->ShapeType() != TopAbs_VERTEX)
      {
        std::swap (aShapeA, aShapeB);
      }

      if (!(aShapeA->ShapeType() == TopAbs_VERTEX
         && aShapeB->ShapeType() == TopAbs_FACE))
      {
        Message::SendFail ("Syntax error: you should select face and vertex.");
        return 1;
      }

      gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(*aShapeA));

      TopoDS_Face aFace = TopoDS::Face(*aShapeB);
      BRepAdaptor_Surface aSurface (aFace, Standard_False);
      if (aSurface.GetType() == GeomAbs_Plane)
      {
        gp_Pln aPlane = aSurface.Plane();
        // Construct a plane parallel to aGeomPlane through A
        aPlane.SetLocation(A);
        Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
        Handle(AIS_Plane) aAISPlane = new AIS_Plane (aGeomPlane, A);
        GetMapOfAIS().Bind (aAISPlane ,aName);
        TheAISContext()->Display (aAISPlane, Standard_True);
      }
      else
      {
        Message::SendFail ("Error: Built surface is not a plane.");
        return 1;
      }
    }

    // Function vplaneortho
    // ====================
    // test the constructor AIS_Plane::AIS_Plane(Geom_Plane,gp_Pnt,gp_Pnt,gp_Pnt)
    else
    {
      if (aShapes.Extent() != 2)
      {
        Message::SendFail ("Error: wrong number of selected shapes.");
        return 1;
      }

      const TopoDS_Shape* aShapeA = &aShapes.First();
      const TopoDS_Shape* aShapeB = &aShapes.Last();

      if (aShapeA->ShapeType() != TopAbs_EDGE)
      {
        std::swap (aShapeA, aShapeB);
      }

      if (!(aShapeA->ShapeType() == TopAbs_EDGE
         && aShapeB->ShapeType() == TopAbs_FACE))
      {
        Message::SendFail ("Error: you should select edge and face.");
        return 1;
      }

      // Construction of plane
      TopoDS_Edge anEdgeA = TopoDS::Edge(*aShapeA);
      TopoDS_Vertex aVAa, aVAb;
      TopExp::Vertices(anEdgeA, aVAa, aVAb);
      gp_Pnt Aa = BRep_Tool::Pnt(aVAa);
      gp_Pnt Ab = BRep_Tool::Pnt(aVAb);
      gp_Vec ab (Aa,Ab);

      gp_Dir Dab (ab);
      // Creation of rotation axis
      gp_Ax1 aRotAxis (Aa,Dab);

      TopoDS_Face aFace = TopoDS::Face(*aShapeB);
      // The edge must be parallel to the face
      BRepExtrema_ExtPF aHeightA (aVAa, aFace);
      BRepExtrema_ExtPF aHeightB (aVAb, aFace);
      // Compare to heights
      if (fabs(sqrt(aHeightA.SquareDistance(1)) - sqrt(aHeightB.SquareDistance(1)))
          >Precision::Confusion())
      {
        // the edge is not parallel to the face
        Message::SendFail ("Error: the edge is not parallel to the face");
        return 1;
      }
      // the edge is OK
      BRepAdaptor_Surface aSurface (aFace, Standard_False);
      if (aSurface.GetType()==GeomAbs_Plane)
      {
        gp_Pln aPlane = aSurface.Plane();
        // It rotates a half turn round the axis of rotation
        aPlane.Rotate(aRotAxis , M_PI/2);

        Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
        // constructed aGeomPlane parallel to a plane containing the edge (center mid-edge)
        gp_Pnt aMiddle ((Aa.X()+Ab.X() )/2 ,(Aa.Y()+Ab.Y() )/2 ,(Aa.Z()+Ab.Z() )/2 );
        Handle(AIS_Plane) anAISPlane = new AIS_Plane (aGeomPlane, aMiddle);
        GetMapOfAIS().Bind (anAISPlane, aName);
        TheAISContext()->Display (anAISPlane, Standard_True);
      }
      else
      {
        Message::SendFail ("Error: surface is not Plane");
        return 1;
      }
    }
  }
  return 0;
}

//===============================================================================================
//function : VChangePlane
//purpose  :
//===============================================================================================
static int VChangePlane (Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail ("Error: no active viewer.");
    return 1;
  }

  if (theArgsNb < 3 || theArgsNb > 11)
  {
    Message::SendFail ("Syntax error: wrong number of arguments.");
    return 1;
  }

  TCollection_AsciiString aName (theArgVec[1]);

  Handle(AIS_Plane) aPlane = GetMapOfAIS().IsBound2(aName)
    ? Handle(AIS_Plane)::DownCast (GetMapOfAIS().Find2 (aName))
    : NULL;

  if ( aPlane.IsNull() )
  {
    Message::SendFail() << "Syntax error: there is no interactive plane with the given name '" << aName << "'.";
    return 1;
  }

  Standard_Real aCenterX = aPlane->Center().X();
  Standard_Real aCenterY = aPlane->Center().Y();
  Standard_Real aCenterZ = aPlane->Center().Z();

  Standard_Real aDirX = aPlane->Component()->Axis().Direction().X();
  Standard_Real aDirY = aPlane->Component()->Axis().Direction().Y();
  Standard_Real aDirZ = aPlane->Component()->Axis().Direction().Z();

  Standard_Real aSizeX = 0.0;
  Standard_Real aSizeY = 0.0;
  Standard_Boolean aHasMinSize = aPlane->HasMinimumSize();
  Standard_Real aMinSizeY = 0.0;
  aPlane->Size (aSizeX, aSizeY);
  Standard_Boolean isUpdate = Standard_True;

  TCollection_AsciiString aPName, aPValue;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.UpperCase();
    if (ViewerTest::SplitParameter (anArg, aPName, aPValue))
    {
      aPName.UpperCase();
      if (aPName.IsEqual ("X"))
      {
        aCenterX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("Y"))
      {
        aCenterY = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("Z"))
      {
        aCenterZ = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DX"))
      {
        aDirX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DY"))
      {
        aDirY = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("DZ"))
      {
        aDirZ = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("SX"))
      {
        aSizeX = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("SY"))
      {
        aSizeY = aPValue.RealValue();
      }
      else if (aPName.IsEqual ("MINSIZE"))
      {
        aHasMinSize = Standard_True;
        aMinSizeY = aPValue.RealValue();
      }
    }
    else if (anArg.IsEqual ("NOUPDATE"))
    {
      isUpdate = Standard_False;
    }
  }

  gp_Dir aDirection (aDirX, aDirY, aDirZ);
  gp_Pnt aCenterPnt (aCenterX, aCenterY, aCenterZ);
  aPlane->SetCenter (aCenterPnt);
  aPlane->SetComponent (new Geom_Plane (aCenterPnt, aDirection));
  aPlane->SetSize (aSizeX, aSizeY);

  if (aHasMinSize)
  {
    aPlane->SetMinimumSize (aMinSizeY);
  }
  else if (aPlane->HasMinimumSize())
  {
    aPlane->UnsetMinimumSize();
  }

  aContextAIS->Update (aPlane, isUpdate);

  return 0;
}

//==============================================================================
// Fonction  vline
// ---------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VLineBuilder
//purpose  : Build an AIS_Line
//Draw arg : vline LineName  [AIS_PointName] [AIS_PointName]
//                           [Xa] [Ya] [Za]   [Xb] [Yb] [Zb]
//==============================================================================
#include <Geom_CartesianPoint.hxx>
#include <AIS_Line.hxx>


static int VLineBuilder(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc == 4)   // parameters: AIS_Point AIS_Point
  {
    Handle(AIS_InteractiveObject) aShapeA, aShapeB;
    GetMapOfAIS().Find2 (argv[2], aShapeA);
    GetMapOfAIS().Find2 (argv[3], aShapeB);
    Handle(AIS_Point) anAISPointA = Handle(AIS_Point)::DownCast (aShapeA);
    Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast (aShapeB);
    if (anAISPointA.IsNull()
     || anAISPointB.IsNull())
    {
      di << "vline error: wrong type of arguments\n";
      return 1;
    }

    Handle(Geom_Point) aGeomPointBA = anAISPointA->Component();
    Handle(Geom_CartesianPoint) aCartPointA = Handle(Geom_CartesianPoint)::DownCast (aGeomPointBA);

    Handle(Geom_Point) aGeomPointB = anAISPointB->Component();
    Handle(Geom_CartesianPoint) aCartPointB = Handle(Geom_CartesianPoint)::DownCast (aGeomPointB);

    if (aCartPointB->X() == aCartPointA->X()
     && aCartPointB->Y() == aCartPointA->Y()
     && aCartPointB->Z() == aCartPointA->Z())
    {
      // B=A
      di << "vline error: same points\n";
      return 1;
    }

    Handle(AIS_Line) anAISLine = new AIS_Line (aCartPointA, aCartPointB);
    GetMapOfAIS().Bind (anAISLine, argv[1]);
    TheAISContext()->Display (anAISLine, Standard_True);
  }
  else if (argc == 8) // parametres 6 reals
  {
    Standard_Real aCoord[6] = {};
    for (Standard_Integer i = 0; i <= 2; ++i)
    {
      aCoord[i]     = Draw::Atof (argv[2 + i]);
      aCoord[i + 3] = Draw::Atof (argv[5 + i]);
    }

    Handle(Geom_CartesianPoint) aCartPointA = new Geom_CartesianPoint (aCoord[0], aCoord[1], aCoord[2]);
    Handle(Geom_CartesianPoint) aCartPointB = new Geom_CartesianPoint (aCoord[3], aCoord[4], aCoord[5]);

    Handle(AIS_Line) anAISLine = new AIS_Line (aCartPointA, aCartPointB);
    GetMapOfAIS().Bind (anAISLine, argv[1]);
    TheAISContext()->Display (anAISLine, Standard_True);

  }
  else if (argc == 2) // selection in 3D viewer
  {
    TopTools_ListOfShape aShapes;
    ViewerTest::GetSelectedShapes (aShapes);
    if (aShapes.Extent() != 2)
    {
      Message::SendFail ("Error: wrong number of selected shapes.");
      return 1;
    }

    const TopoDS_Shape& aShapeA = aShapes.First();
    const TopoDS_Shape& aShapeB = aShapes.Last();
    if (aShapeA.ShapeType() != TopAbs_VERTEX
     || aShapeB.ShapeType() != TopAbs_VERTEX)
    {
      Message::SendFail ("Error: you should select two different vertex.");
      return 1;
    }

    // Construction de la line
    gp_Pnt A = BRep_Tool::Pnt (TopoDS::Vertex (aShapeA));
    gp_Pnt B = BRep_Tool::Pnt (TopoDS::Vertex (aShapeB));

    Handle(Geom_CartesianPoint) aCartPointA = new Geom_CartesianPoint (A);
    Handle(Geom_CartesianPoint) aCartPointB = new Geom_CartesianPoint (B);

    Handle(AIS_Line) anAISLine = new AIS_Line (aCartPointA, aCartPointB);
    GetMapOfAIS().Bind (anAISLine, argv[1]);
    TheAISContext()->Display (anAISLine, Standard_True);
  }
  else
  {
    di << "Syntax error: wrong number of arguments";
    return 1;
  }

  return 0;
}

//==============================================================================
// class   : FilledCircle
// purpose : creates filled circle based on AIS_InteractiveObject 
//           and Geom_Circle.
//           This class is used to check method Matches() of class 
//           Select3D_SensitiveCircle with member myFillStatus = Standard_True, 
//           because none of AIS classes provides creation of 
//           Select3D_SensitiveCircle with member myFillStatus = Standard_True 
//           (look method ComputeSelection() )
//============================================================================== 

Handle(Geom_Circle) CreateCircle(gp_Pnt theCenter, Standard_Real theRadius) 
{
  gp_Ax2 anAxes(theCenter, gp_Dir(gp_Vec(0., 0., 1.))); 
  gp_Circ aCirc(anAxes, theRadius);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCirc);
  return aCircle;
}

class FilledCircle : public AIS_InteractiveObject 
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(FilledCircle, AIS_InteractiveObject);

  FilledCircle (const Handle(Geom_Circle)& theCircle,
                const Standard_Real theUStart,
                const Standard_Real theUEnd);

  FilledCircle (const gp_Pnt& theCenter,
                const Standard_Real theRadius,
                const Standard_Real theUStart,
                const Standard_Real theUEnd);

private:
  TopoDS_Face ComputeFace();

  // Virtual methods implementation
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE;

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) Standard_OVERRIDE;

protected:

  Handle(Geom_Circle) myCircle;
  Standard_Real       myUStart;
  Standard_Real       myUEnd;
  Standard_Boolean    myFilledStatus;

}; 

FilledCircle::FilledCircle (const Handle(Geom_Circle)& theCircle,
                            const Standard_Real theUStart,
                            const Standard_Real theUEnd)
: myCircle (theCircle),
  myUStart (theUStart),
  myUEnd (theUEnd),
  myFilledStatus (Standard_True)
{ }

FilledCircle::FilledCircle (const gp_Pnt& theCenter,
                            const Standard_Real theRadius,
                            const Standard_Real theUStart,
                            const Standard_Real theUEnd)
: FilledCircle (CreateCircle (theCenter, theRadius), theUStart, theUEnd)
{ }

TopoDS_Face FilledCircle::ComputeFace()
{
  // Create edge from myCircle
  BRepBuilderAPI_MakeEdge anEdgeMaker (myCircle->Circ(), myUStart, myUEnd);
  TopoDS_Edge anEdge = anEdgeMaker.Edge();

  // Create wire from anEdge
  BRepBuilderAPI_MakeWire aWireMaker;
  if (Abs (Abs (myUEnd - myUStart) - 2.0 * M_PI) > gp::Resolution())
  {
    TopoDS_Edge anEndCenterEdge = BRepBuilderAPI_MakeEdge (myCircle->Value (myUEnd), myCircle->Location()).Edge();
    TopoDS_Edge aStartCenterEdge = BRepBuilderAPI_MakeEdge (myCircle->Location(), myCircle->Value (myUStart)).Edge();
    aWireMaker = BRepBuilderAPI_MakeWire (anEdge, anEndCenterEdge, aStartCenterEdge);
  }
  else
  {
    aWireMaker = BRepBuilderAPI_MakeWire (anEdge);
  }
  TopoDS_Wire aWire = aWireMaker.Wire();

  // Create face from aWire
  BRepBuilderAPI_MakeFace aFaceMaker (aWire);
  TopoDS_Face aFace = aFaceMaker.Face();

  return aFace;
}

void FilledCircle::Compute (const Handle(PrsMgr_PresentationManager)& ,
                            const Handle(Prs3d_Presentation)& thePrs,
                            const Standard_Integer theMode)
{
  thePrs->Clear();

  TopoDS_Face aFace = ComputeFace();

  if (aFace.IsNull()) return;
  if (theMode != 0) return;

  StdPrs_ShadedShape::Add (thePrs, aFace, myDrawer);
}

void FilledCircle::ComputeSelection (const Handle(SelectMgr_Selection) &theSelection,
                                     const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(Select3D_SensitiveEntity) aSensitiveCircle;

  if (Abs (Abs (myUEnd - myUStart) - 2.0 * M_PI) > gp::Resolution())
  {
    aSensitiveCircle = new Select3D_SensitivePoly (anEntityOwner, myCircle->Circ(), myUStart, myUEnd, myFilledStatus);
  }
  else
  {
    aSensitiveCircle = new Select3D_SensitiveCircle (anEntityOwner, myCircle->Circ(), myFilledStatus);
  }

  theSelection->Add (aSensitiveCircle);
}

//==============================================================================
// Fonction  vcircle
// -----------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : VCircleBuilder
//purpose  : Build an AIS_Circle
//Draw arg : vcircle CircleName PlaneName PointName Radius IsFilled UStart UEnd
//                              PointName PointName PointName IsFilled UStart UEnd
//==============================================================================

void DisplayCircle (const Handle(Geom_Circle)& theGeomCircle,
                    const TCollection_AsciiString& theName,
                    const Standard_Boolean isFilled,
                    const Standard_Real theUStart,
                    const Standard_Real theUEnd)
{
  Handle(AIS_InteractiveObject) aCircle;
  if (isFilled)
  {
    aCircle = new FilledCircle (theGeomCircle, theUStart, theUEnd);
  }
  else
  {
    aCircle = new AIS_Circle (theGeomCircle, theUStart, theUEnd, Standard_False);
  }

  // Check if there is an object with given name
  // and remove it from context
  if (GetMapOfAIS().IsBound2(theName))
  {
    Handle(AIS_InteractiveObject) anInterObj = GetMapOfAIS().Find2 (theName);
    TheAISContext()->Remove (anInterObj, Standard_False);
    GetMapOfAIS().UnBind2 (theName);
   }

   // Bind the circle to its name
   GetMapOfAIS().Bind (aCircle, theName);

   // Display the circle
   TheAISContext()->Display (aCircle, Standard_True);
}

static int VCircleBuilder (Draw_Interpretor& /*di*/, Standard_Integer argc, const char** argv)
{
  if (argc > 8 || argc < 2)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  Standard_Real anUStart = 0, anUEnd = M_PI * 2.0;
  if (argc == 8)
  {
    anUStart = Draw::Atof (argv[6]) * M_PI / 180.0;
    anUEnd  = Draw::Atof (argv[7]) * M_PI / 180.0;
  }
  else if (argc == 4)
  {
    anUStart = Draw::Atof (argv[2]) * M_PI / 180.0;
    anUEnd  = Draw::Atof (argv[3]) * M_PI / 180.0;
  }

  if (argc == 6 || argc == 8)
  {
    TCollection_AsciiString aName (argv[1]);
    Standard_Boolean isFilled = Draw::Atoi(argv[5]) != 0;

    Handle(AIS_InteractiveObject) aShapeA, aShapeB;
    GetMapOfAIS().Find2 (argv[2], aShapeA);
    GetMapOfAIS().Find2 (argv[3], aShapeB);

    // Arguments: AIS_Point AIS_Point AIS_Point
    if (!aShapeA.IsNull()
     && !aShapeB.IsNull()
     &&  aShapeA->Type() == AIS_KindOfInteractive_Datum
     &&  aShapeA->Signature() == 1)
    {
      Handle(AIS_InteractiveObject) aShapeC;
      GetMapOfAIS().Find2 (argv[4], aShapeC);
      Handle(AIS_Point) anAISPointA = Handle(AIS_Point)::DownCast (aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast (aShapeB);
      Handle(AIS_Point) anAISPointC = Handle(AIS_Point)::DownCast (aShapeC);
      if (anAISPointA.IsNull()
       || anAISPointB.IsNull()
       || anAISPointC.IsNull())
      {
        Message::SendFail ("Error: arguments are expected to be points");
        return 1;
      }

      // Verify that the three points are different
      Handle(Geom_CartesianPoint) aCartPointA = Handle(Geom_CartesianPoint)::DownCast (anAISPointA->Component());
      Handle(Geom_CartesianPoint) aCartPointB = Handle(Geom_CartesianPoint)::DownCast (anAISPointB->Component());
      Handle(Geom_CartesianPoint) aCartPointC = Handle(Geom_CartesianPoint)::DownCast (anAISPointC->Component());
      // Test A=B
      if (Abs(aCartPointA->X() - aCartPointB->X()) <= Precision::Confusion()
       && Abs(aCartPointA->Y() - aCartPointB->Y()) <= Precision::Confusion()
       && Abs(aCartPointA->Z() - aCartPointB->Z()) <= Precision::Confusion())
      {
        Message::SendFail ("Error: Same points");
        return 1;
      }
      // Test A=C
      if (Abs(aCartPointA->X() - aCartPointC->X()) <= Precision::Confusion()
       && Abs(aCartPointA->Y() - aCartPointC->Y()) <= Precision::Confusion()
       && Abs(aCartPointA->Z() - aCartPointC->Z()) <= Precision::Confusion())
      {
        Message::SendFail ("Error: Same points");
        return 1;
      }
      // Test B=C
      if (Abs(aCartPointB->X() - aCartPointC->X()) <= Precision::Confusion()
       && Abs(aCartPointB->Y() - aCartPointC->Y()) <= Precision::Confusion()
       && Abs(aCartPointB->Z() - aCartPointC->Z()) <= Precision::Confusion())
      {
        Message::SendFail ("Error: Same points");
        return 1;
      }
      // Construction of the circle
      GC_MakeCircle aCir = GC_MakeCircle (aCartPointA->Pnt(), aCartPointB->Pnt(), aCartPointC->Pnt());
      Handle (Geom_Circle) aGeomCircle;
      try
      {
        aGeomCircle = aCir.Value();
      }
      catch (StdFail_NotDone const&)
      {
        Message::SendFail ("Error: can't create circle");
        return 1;
      }

      DisplayCircle (aGeomCircle, aName, isFilled, anUStart, anUEnd);
    }

    // Arguments: AIS_Plane AIS_Point Real
    else if (aShapeA->Type() == AIS_KindOfInteractive_Datum
          && aShapeA->Signature() == 7)
    {
      Handle(AIS_Plane) anAISPlane  = Handle(AIS_Plane)::DownCast (aShapeA);
      Handle(AIS_Point) anAISPointB = Handle(AIS_Point)::DownCast (aShapeB);
      if (anAISPointB.IsNull())
      {
        Message::SendFail ("Error: 2d element is a expected to be a point");
        return 1;
      }

      // Check that the radius is >= 0
      const Standard_Real anR = Draw::Atof (argv[4]);
      if (anR <= 0)
      {
        Message::SendFail ("Syntax error: the radius must be >=0");
        return 1;
      }

      // Recover the normal to the plane
      Handle(Geom_Plane) aGeomPlane  = anAISPlane->Component();
      Handle(Geom_Point) aGeomPointB = anAISPointB->Component();
      Handle(Geom_CartesianPoint) aCartPointB = Handle(Geom_CartesianPoint)::DownCast(aGeomPointB);

      gp_Pln aGpPlane = aGeomPlane->Pln();
      gp_Ax1 aGpAxe = aGpPlane.Axis();
      gp_Dir aDir = aGpAxe.Direction();
      gp_Pnt aCenter = aCartPointB->Pnt();
      GC_MakeCircle aCir = GC_MakeCircle (aCenter, aDir, anR);
      Handle(Geom_Circle) aGeomCircle;
      try
      {
        aGeomCircle = aCir.Value();
      }
      catch (StdFail_NotDone const&)
      {
        Message::SendFail ("Error: can't create circle");
        return 1;
      }

      DisplayCircle (aGeomCircle, aName, isFilled, anUStart, anUEnd);
    }
    else
    {
      Message::SendFail ("Error: 1st argument has an unexpected type");
      return 1;
    }
  }
  else // No arguments: selection in the viewer
  {
    // Get the name of the circle 
    TCollection_AsciiString aName (argv[1]);

    TopTools_ListOfShape aShapes;
    ViewerTest::GetSelectedShapes (aShapes);
    if (aShapes.Extent() != 3 && aShapes.Extent() != 2)
    {
      Message::SendFail ("Error: Wrong number of selected shapes.");
      return 1;
    }

    const TopoDS_Shape& aShapeA = aShapes.First();
    if (aShapeA.ShapeType() == TopAbs_VERTEX)
    {
      if (aShapes.Extent() != 3)
      {
        Message::SendFail ("Error: wrong number of selected shapes.");
        return 1;
      }

      TopTools_ListOfShape::Iterator anIter (aShapes);

      anIter.Next();
      const TopoDS_Shape& aShapeB = anIter.Value();

      anIter.Next();
      const TopoDS_Shape& aShapeC = anIter.Value();
      
      // Get isFilled
      Standard_Boolean isFilled;
      std::cout << "Enter filled status (0 or 1)\n";
      std::cin >> isFilled;

      // Construction of the circle
      gp_Pnt A = BRep_Tool::Pnt (TopoDS::Vertex (aShapeA));
      gp_Pnt B = BRep_Tool::Pnt (TopoDS::Vertex (aShapeB));
      gp_Pnt C = BRep_Tool::Pnt (TopoDS::Vertex (aShapeC));

      GC_MakeCircle aCir = GC_MakeCircle (A, B, C);
      Handle(Geom_Circle) aGeomCircle;
      try
      {
        aGeomCircle = aCir.Value();
      }
      catch (StdFail_NotDone const&)
      {
        Message::SendFail ("Error: can't create circle");
        return 1;
      }

      DisplayCircle (aGeomCircle, aName, isFilled, anUStart, anUEnd);
    }
    else if (aShapeA.ShapeType() == TopAbs_FACE)
    {
      const TopoDS_Shape& aShapeB = aShapes.Last();

      // Recover the radius 
      Standard_Real aRad = 0.0;
      do
      {
        std::cout << " Enter the value of the radius:\n";
        std::cin >> aRad;
      } while (aRad <= 0);

      // Get filled status
      Standard_Boolean isFilled;
      std::cout << "Enter filled status (0 or 1)\n";
      std::cin >> isFilled;

      // Recover the normal to the plane. tag
      TopoDS_Face aFace = TopoDS::Face (aShapeA);
      BRepAdaptor_Surface aSurface (aFace, Standard_False);
      gp_Pln aPlane = aSurface.Plane();
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
      gp_Pln aGpPlane = aGeomPlane->Pln();
      gp_Ax1 aGpAxe = aGpPlane.Axis();
      gp_Dir aDir = aGpAxe.Direction();

      // Recover the center
      gp_Pnt aCenter = BRep_Tool::Pnt (TopoDS::Vertex (aShapeB));

      // Construct the circle
      GC_MakeCircle aCir = GC_MakeCircle (aCenter, aDir, aRad);
      Handle(Geom_Circle) aGeomCircle;
      try
      {
        aGeomCircle = aCir.Value();
      }
      catch (StdFail_NotDone const&)
      {
        Message::SendFail ("Error: can't create circle");
        return 1;
      }

      DisplayCircle (aGeomCircle, aName, isFilled, anUStart, anUEnd);
    }
    else
    {
      Message::SendFail ("Error: You should select face and vertex or three vertices.");
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : VDrawText
//purpose  :
//=======================================================================
static int VDrawText (Draw_Interpretor& theDI,
                      Standard_Integer  theArgsNb,
                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (theArgsNb < 3)
  {
    Message::SendFail ("Syntax error: wrong number of arguments. See usage:");
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }
  else if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Integer           anArgIt = 1;
  TCollection_ExtendedString aName (theArgVec[anArgIt++], Standard_True);
  TCollection_ExtendedString aText (theArgVec[anArgIt++], Standard_True);
  Handle(AIS_TextLabel)      aTextPrs;
  ViewerTest_AutoUpdater     anAutoUpdater (aContext, ViewerTest::CurrentView());

  Standard_Boolean isNewPrs = Standard_False;
  if (GetMapOfAIS().IsBound2 (aName))
  {
    aTextPrs = Handle(AIS_TextLabel)::DownCast (GetMapOfAIS().Find2 (aName));
  }

  if (aTextPrs.IsNull())
  {
    isNewPrs = Standard_True;
    aTextPrs = new AIS_TextLabel();
  }

  aTextPrs->SetText (aText);

  Handle(Graphic3d_TransformPers) aTrsfPers;
  Aspect_TypeOfDisplayText aDisplayType = Aspect_TODT_NORMAL;

  Standard_Boolean aHasPlane = Standard_False;
  gp_Dir           aNormal;
  gp_Dir           aDirection;
  gp_Pnt           aPos;


  Handle(Font_TextFormatter) aTextFormatter;
  for (; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString aParam (theArgVec[anArgIt]);
    aParam.LowerCase();

    if (anAutoUpdater.parseRedrawMode (aParam))
    {
      continue;
    }
    else if ((aParam == "-pos"
           || aParam == "-position")
           && anArgIt + 3 < theArgsNb)
    {
      aPos.SetX (Draw::Atof (theArgVec[++anArgIt]));
      aPos.SetY (Draw::Atof (theArgVec[++anArgIt]));
      aPos.SetZ (Draw::Atof (theArgVec[++anArgIt]));
      aTextPrs->SetPosition (aPos);
    }
    else if (aParam == "-color")
    {
      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgsNb - anArgIt - 1,
                                                     theArgVec + anArgIt + 1,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << aParam << "'";
        return 1;
      }
      anArgIt += aNbParsed;
      aTextPrs->SetColor (aColor);
    }
    else if ((aParam == "-halign"
           || aParam == "-valign"
           || aParam == "-align")
          && anArgIt + 1 < theArgsNb)
    {
      TCollection_AsciiString aType (theArgVec[++anArgIt]);
      aType.LowerCase();
      if (aType == "left")
      {
        aTextPrs->SetHJustification (Graphic3d_HTA_LEFT);
        if (aParam == "-valign")
        {
          Message::SendFail() << "Syntax error at '" << aParam << "'";
          return 1;
        }
      }
      else if (aType == "center")
      {
        if (aParam == "-halign"
         || aParam == "-align")
        {
          aTextPrs->SetHJustification (Graphic3d_HTA_CENTER);
        }
        if (aParam == "-valign"
         || aParam == "-align")
        {
          aTextPrs->SetVJustification (Graphic3d_VTA_CENTER);
        }
      }
      else if (aType == "right")
      {
        aTextPrs->SetHJustification (Graphic3d_HTA_RIGHT);
        if (aParam == "-valign")
        {
          Message::SendFail() << "Syntax error at '" << aParam << "'";
          return 1;
        }
      }
      else if (aType == "top")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_TOP);
        if (aParam == "-halign")
        {
          Message::SendFail() << "Syntax error at '" << aParam << "'";
          return 1;
        }
      }
      else if (aType == "bottom")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_BOTTOM);
        if (aParam == "-halign")
        {
          Message::SendFail() << "Syntax error at '" << aParam << "'";
          return 1;
        }
      }
      else if (aType == "topfirstline")
      {
        aTextPrs->SetVJustification (Graphic3d_VTA_TOPFIRSTLINE);
        if (aParam == "-halign")
        {
          Message::SendFail() << "Syntax error at '" << aParam << "'";
          return 1;
        }
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << aParam << "'";
        return 1;
      }
    }
    else if (aParam == "-angle"
          && anArgIt + 1 < theArgsNb)
    {
      aTextPrs->SetAngle (Draw::Atof (theArgVec[++anArgIt]) * (M_PI / 180.0));
    }
    else if (aParam == "-zoom"
          || aParam == "-nozoom"
          || aParam == "-zoomable"
          || aParam == "-nonzoomable")
    {
      const bool isZoomable = Draw::ParseOnOffNoIterator (theArgsNb, theArgVec, anArgIt);
      aTextPrs->SetZoomable (isZoomable);
    }
    else if (aParam == "-height"
          && anArgIt + 1 < theArgsNb)
    {
      aTextPrs->SetHeight (Draw::Atof(theArgVec[++anArgIt]));
    }
    else if (aParam == "-wrapping"
          && anArgIt + 1 < theArgsNb)
    {
      if (aTextFormatter.IsNull())
      {
        aTextFormatter = new Font_TextFormatter();
      }
      aTextFormatter->SetWrapping ((Standard_ShortReal)Draw::Atof(theArgVec[++anArgIt]));
    }
    else if (aParam == "-wordwrapping")
    {
      const bool isWordWrapping = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIt);
      aTextFormatter->SetWordWrapping(isWordWrapping);
    }
    else if (aParam == "-aspect"
          && anArgIt + 1 < theArgsNb)
    {
      TCollection_AsciiString anOption (theArgVec[++anArgIt]);
      anOption.LowerCase();
      Font_FontAspect aFontAspect = Font_FA_Undefined;
      if (!parseFontStyle (anOption, aFontAspect))
      {
        Message::SendFail() << "Syntax error: unknown font aspect '" << anOption << "'";
        return 1;
      }
      aTextPrs->SetFontAspect (aFontAspect);
    }
    else if (aParam == "-font"
          && anArgIt + 1 < theArgsNb)
    {
      aTextPrs->SetFont (theArgVec[++anArgIt]);
    }
    else if (aParam == "-plane"
          && anArgIt + 6 < theArgsNb)
    {
      aNormal.SetCoord (Draw::Atof (theArgVec[anArgIt + 1]),
                        Draw::Atof (theArgVec[anArgIt + 2]),
                        Draw::Atof (theArgVec[anArgIt + 3]));
      aDirection.SetCoord (Draw::Atof (theArgVec[anArgIt + 4]),
                           Draw::Atof (theArgVec[anArgIt + 5]),
                           Draw::Atof (theArgVec[anArgIt + 6]));
      aHasPlane = Standard_True;
      anArgIt += 6;
    }
    else if (aParam == "-flipping"
          || aParam == "-noflipping"
          || aParam == "-flip"
          || aParam == "-noflip")
    {
      const bool toFlip = Draw::ParseOnOffNoIterator (theArgsNb, theArgVec, anArgIt);
      aTextPrs->SetFlipping (toFlip);
    }
    else if (aParam == "-ownanchor"
          || aParam == "-noownanchor")
    {
      const bool isOwnAnchor = Draw::ParseOnOffNoIterator (theArgsNb, theArgVec, anArgIt);
      aTextPrs->SetOwnAnchorPoint (isOwnAnchor);
    }
    else if ((aParam == "-disptype"
           || aParam == "-displaytype")
          && anArgIt + 1 < theArgsNb)
    {
      TCollection_AsciiString aType (theArgVec[++anArgIt]);
      aType.LowerCase();
      if      (aType == "subtitle")  { aDisplayType = Aspect_TODT_SUBTITLE; }
      else if (aType == "decal")     { aDisplayType = Aspect_TODT_DEKALE; }
      else if (aType == "blend")     { aDisplayType = Aspect_TODT_BLEND; }
      else if (aType == "dimension") { aDisplayType = Aspect_TODT_DIMENSION; }
      else if (aType == "normal")    { aDisplayType = Aspect_TODT_NORMAL; }
      else if (aType == "shadow")    { aDisplayType = Aspect_TODT_SHADOW; }
      else
      {
        Message::SendFail() << "Syntax error: wrong display type '" << aType << "'";
        return 1;
      }
    }
    else if (aParam == "-subcolor"
          || aParam == "-subtitlecolor")
    {
      Quantity_Color aColor;
      Standard_Integer aNbParsed = Draw::ParseColor (theArgsNb - anArgIt - 1,
                                                     theArgVec + anArgIt + 1,
                                                     aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << aParam << "'";
        return 1;
      }
      anArgIt += aNbParsed;
      aTextPrs->SetColorSubTitle (aColor);
    }
    else if (aParam == "-2d")
    {
      aTrsfPers = new Graphic3d_TransformPers (Graphic3d_TMF_2d);
    }
    else if (aParam == "-trsfperspos"
          || aParam == "-perspos")
    {
      if (anArgIt + 2 >= theArgsNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'.";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIt]);
      TCollection_AsciiString aY (theArgVec[++anArgIt]);
      TCollection_AsciiString aZ = "0";
      if (!aX.IsIntegerValue()
       || !aY.IsIntegerValue())
      {
        Message::SendFail() << "Error: wrong syntax at '" << aParam << "'.";
        return 1;
      }
      if (anArgIt + 1 < theArgsNb)
      {
        TCollection_AsciiString aTemp = theArgVec[anArgIt + 1];
        if (aTemp.IsIntegerValue())
        {
          aZ = aTemp;
          ++anArgIt;
        }
      }

      Standard_Integer aCorner = Aspect_TOTP_CENTER;
      if      (aX.IntegerValue() > 0.0) { aCorner |= Aspect_TOTP_RIGHT; }
      else if (aX.IntegerValue() < 0.0) { aCorner |= Aspect_TOTP_LEFT; }
      if      (aY.IntegerValue() > 0.0) { aCorner |= Aspect_TOTP_TOP; }
      else if (aY.IntegerValue() < 0.0) { aCorner |= Aspect_TOTP_BOTTOM; }
      aTrsfPers = new Graphic3d_TransformPers (aTrsfPers->Mode(), Aspect_TypeOfTriedronPosition (aCorner), Graphic3d_Vec2i (aZ.IntegerValue()));
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << aParam << "'";
      return 1;
    }
  }

  aTextPrs->SetTextFormatter (aTextFormatter);

  if (aHasPlane)
  {
    aTextPrs->SetOrientation3D (gp_Ax2 (aPos, aNormal, aDirection));
  }

  aTextPrs->SetDisplayType (aDisplayType);

  if (!aTrsfPers.IsNull())
  {
    aContext->SetTransformPersistence (aTextPrs, aTrsfPers);
    aTextPrs->SetZLayer(Graphic3d_ZLayerId_TopOSD);
    if (aTextPrs->Position().Z() != 0)
    {
      aTextPrs->SetPosition (gp_Pnt(aTextPrs->Position().X(), aTextPrs->Position().Y(), 0));
    }
  }
  else if (!aTextPrs->TransformPersistence().IsNull())
  {
    aContext->SetTransformPersistence (aTextPrs, Handle(Graphic3d_TransformPers)());
  }

  if (isNewPrs)
  {
    ViewerTest::Display (aName, aTextPrs, Standard_False);
  }
  else
  {
    aContext->Redisplay (aTextPrs, Standard_False, Standard_True);
  }
  return 0;
}

#include <math.h>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TShort_HArray1OfShortReal.hxx>

#include <AIS_Triangulation.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <AIS_InteractiveObject.hxx>


//===============================================================================================
//function : CalculationOfSphere
//author   : psn
//purpose  : Create a Sphere
//===============================================================================================

Handle( Poly_Triangulation ) CalculationOfSphere( double X , double Y , double Z ,
                                                  int res ,
                                                  double Radius ){
  double mRadius = Radius;
  double mCenter[3] = {X,Y,Z};
  int mThetaResolution;
  int mPhiResolution;
  double mStartTheta = 0;//StartTheta;
  double mEndTheta = 360;//EndTheta;
  double mStartPhi = 0;//StartPhi;
  double mEndPhi = 180;//EndPhi;
  res = res < 4 ? 4 : res;

  mThetaResolution = res;
  mPhiResolution = res;

  int i, j;
  int jStart, jEnd, numOffset;
  double x[3], n[3], deltaPhi, deltaTheta, phi, theta, radius;
  double startTheta, endTheta, startPhi, endPhi;
  int base, numPoles=0, thetaResolution, phiResolution;

  int pts[3];
  int piece = -1;
  int numPieces = 1;
  if ( numPieces > mThetaResolution ) {
    numPieces = mThetaResolution;
  }

  int localThetaResolution =  mThetaResolution;
  double localStartTheta =  mStartTheta;
  double localEndTheta =  mEndTheta;

  while ( localEndTheta < localStartTheta ) {
    localEndTheta += 360.0;
  }

  deltaTheta = (localEndTheta - localStartTheta) / localThetaResolution;

  // Change the ivars based on pieces.
  int start, end;
  start = piece * localThetaResolution / numPieces;
  end = (piece+1) * localThetaResolution / numPieces;
  localEndTheta = localStartTheta + (double)(end) * deltaTheta;
  localStartTheta = localStartTheta + (double)(start) * deltaTheta;
  localThetaResolution = end - start;

  // Create north pole if needed
  int number_point = 0;
  int number_pointArray = 0;

  if ( mStartPhi <= 0.0 ) {
    number_pointArray++;
    numPoles++;
  }
  if ( mEndPhi >= 180.0 ) {
    number_pointArray++;
    numPoles++;
  }

  // Check data, determine increments, and convert to radians
  startTheta = (localStartTheta < localEndTheta ? localStartTheta : localEndTheta);
  startTheta *= M_PI  / 180.0;
  endTheta = (localEndTheta > localStartTheta ? localEndTheta : localStartTheta);
  endTheta *= M_PI  / 180.0;


  startPhi = ( mStartPhi <  mEndPhi ?  mStartPhi :  mEndPhi);
  startPhi *= M_PI  / 180.0;
  endPhi = ( mEndPhi >  mStartPhi ?  mEndPhi :  mStartPhi);
  endPhi *= M_PI  / 180.0;

  phiResolution =  mPhiResolution - numPoles;
  deltaPhi = (endPhi - startPhi) / ( mPhiResolution - 1);
  thetaResolution = localThetaResolution;
  if ( fabs(localStartTheta - localEndTheta) < 360.0 ) {
    ++localThetaResolution;
  }
  deltaTheta = (endTheta - startTheta) / thetaResolution;

  jStart = ( mStartPhi <= 0.0 ? 1 : 0);
  jEnd = ( mEndPhi >= 180.0 ?  mPhiResolution - 1 :  mPhiResolution);

  // Create intermediate points
  for ( i = 0; i < localThetaResolution; i++ ) {
    for ( j = jStart; j < jEnd; j++ ) {
        number_pointArray++;
    }
  }

  //Generate mesh connectivity
  base = phiResolution * localThetaResolution;

  int number_triangle = 0 ;
  if ( mStartPhi <= 0.0 ) { // around north pole
    number_triangle += localThetaResolution;
  }

  if ( mEndPhi >= 180.0 ) { // around south pole
    number_triangle += localThetaResolution;
  }

  // bands in-between poles
  for ( i=0; i < localThetaResolution; i++){
    for ( j=0; j < (phiResolution-1); j++){
       number_triangle +=2;
    }
  }

  Handle(Poly_Triangulation) polyTriangulation = new Poly_Triangulation (number_pointArray, number_triangle, false, true);

  if (  mStartPhi <= 0.0 ){
      x[0] =  mCenter[0];
      x[1] =  mCenter[1];
      x[2] =  mCenter[2] +  mRadius;
      polyTriangulation->SetNode (1, gp_Pnt (x[0],x[1],x[2]));
  }

  // Create south pole if needed
  if (  mEndPhi >= 180.0 ){
      x[0] =  mCenter[0];
      x[1] =  mCenter[1];
      x[2] =  mCenter[2] -  mRadius;
      polyTriangulation->SetNode (2, gp_Pnt (x[0],x[1],x[2]));
  }

  number_point = 3;
  for ( i=0; i < localThetaResolution; i++){
    theta = localStartTheta * M_PI / 180.0 + i*deltaTheta;
    for ( j = jStart; j < jEnd; j++){
        phi = startPhi + j*deltaPhi;
        radius =  mRadius * sin((double)phi);
        n[0] = radius * cos((double)theta);
        n[1] = radius * sin((double)theta);
        n[2] =  mRadius * cos((double)phi);
        x[0] = n[0] +  mCenter[0];
        x[1] = n[1] +  mCenter[1];
        x[2] = n[2] +  mCenter[2];
        polyTriangulation->SetNode (number_point, gp_Pnt (x[0],x[1],x[2]));
        number_point++;
      }
    }

  numPoles = 3;
  number_triangle = 1;
  if ( mStartPhi <= 0.0 ){// around north pole
    for (i=0; i < localThetaResolution; i++){
        pts[0] = phiResolution*i + numPoles;
        pts[1] = (phiResolution*(i+1) % base) + numPoles;
        pts[2] = 1;
        polyTriangulation->SetTriangle (number_triangle, Poly_Triangle (pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  if (  mEndPhi >= 180.0 ){ // around south pole
    numOffset = phiResolution - 1 + numPoles;
    for (i=0; i < localThetaResolution; i++){
        pts[0] = phiResolution*i + numOffset;
        pts[2] = ((phiResolution*(i+1)) % base) + numOffset;
        pts[1] = numPoles - 1;
        polyTriangulation->SetTriangle (number_triangle, Poly_Triangle (pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  // bands in-between poles

  for (i=0; i < localThetaResolution; i++){
    for (j=0; j < (phiResolution-1); j++){
        pts[0] = phiResolution*i + j + numPoles;
        pts[1] = pts[0] + 1;
        pts[2] = ((phiResolution*(i+1)+j) % base) + numPoles + 1;
        polyTriangulation->SetTriangle (number_triangle, Poly_Triangle (pts[0],pts[1],pts[2]));
        number_triangle++;
        pts[1] = pts[2];
        pts[2] = pts[1] - 1;
        polyTriangulation->SetTriangle (number_triangle, Poly_Triangle (pts[0],pts[1],pts[2]));
        number_triangle++;
      }
    }

  Poly_Connect pc (polyTriangulation);

  Standard_Integer index[3];
  constexpr Standard_Real Tol = Precision::Confusion();

  gp_Dir Nor;
  for (i = 1; i <= polyTriangulation->NbNodes(); i++)
  {
    gp_XYZ eqPlan(0, 0, 0);
    for (pc.Initialize (i); pc.More(); pc.Next())
    {
      polyTriangulation->Triangle (pc.Value()).Get (index[0], index[1], index[2]);
      gp_XYZ v1 (polyTriangulation->Node (index[1]).Coord() - polyTriangulation->Node (index[0]).Coord());
      gp_XYZ v2 (polyTriangulation->Node (index[2]).Coord() - polyTriangulation->Node (index[1]).Coord());
      gp_XYZ vv = v1^v2;
      Standard_Real mod = vv.Modulus();
      if(mod < Tol) continue;
      eqPlan += vv/mod;
    }

    Standard_Real modmax = eqPlan.Modulus();
    if(modmax > Tol)
      Nor = gp_Dir(eqPlan);
    else
      Nor = gp_Dir(0., 0., 1.);

    polyTriangulation->SetNormal (i, Nor.XYZ());
  }

  return polyTriangulation;
}

//===============================================================================================
//function : VDrawSphere
//author   : psn
//purpose  : Create an AIS shape.
//===============================================================================================
static int VDrawSphere (Draw_Interpretor& /*di*/, Standard_Integer argc, const char** argv)
{
  // check for errors
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  else if (argc < 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\n"
                        << "Use: " << argv[0] << " shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToShowEdges=0]";
    return 1;
  }

  // read the arguments
  TCollection_AsciiString aShapeName (argv[1]);
  Standard_Integer aResolution = Draw::Atoi (argv[2]);
  Standard_Real aCenterX = (argc > 5) ? Draw::Atof (argv[3]) : 0.0;
  Standard_Real aCenterY = (argc > 5) ? Draw::Atof (argv[4]) : 0.0;
  Standard_Real aCenterZ = (argc > 5) ? Draw::Atof (argv[5]) : 0.0;
  Standard_Real aRadius =  (argc > 6) ? Draw::Atof (argv[6]) : 100.0;
  Standard_Boolean toShowEdges = (argc > 7) ? Draw::Atoi (argv[7]) == 1 : Standard_False;
  Standard_Boolean toPrintInfo = (argc > 8) ? Draw::Atoi (argv[8]) == 1 : Standard_True;

  // remove AIS object with given name from map
  VDisplayAISObject (aShapeName, Handle(AIS_InteractiveObject)());

  if (toPrintInfo)
    std::cout << "Compute Triangulation...\n";
  Handle(AIS_Triangulation) aShape
    = new AIS_Triangulation (CalculationOfSphere (aCenterX, aCenterY, aCenterZ,
                                                  aResolution,
                                                  aRadius));
  const Standard_Integer aNumberPoints    = aShape->GetTriangulation()->NbNodes();
  const Standard_Integer aNumberTriangles = aShape->GetTriangulation()->NbTriangles();

  // stupid initialization of Green color in RGBA space as integer
  // probably wrong for big-endian CPUs
  const Graphic3d_Vec4ub aColor (0, 255, 0, 0);

  // setup colors array per vertex
  Handle(TColStd_HArray1OfInteger) aColorArray = new TColStd_HArray1OfInteger (1, aNumberPoints);
  for (Standard_Integer aNodeId = 1; aNodeId <= aNumberPoints; ++aNodeId)
  {
    aColorArray->SetValue (aNodeId, *reinterpret_cast<const Standard_Integer*> (aColor.GetData()));
  }
  aShape->SetColors (aColorArray);

  // show statistics
  Standard_Integer aPointsSize      = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat
  Standard_Integer aNormalsSize     = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat
  Standard_Integer aColorsSize      = aNumberPoints * 3 * sizeof(float);  // 3x GLfloat without alpha
  Standard_Integer aTrianglesSize   = aNumberTriangles * 3 * sizeof(int); // 3x GLint
  Standard_Integer aPolyConnectSize = aNumberPoints * 4 + aNumberTriangles * 6 * 4;
  Standard_Integer aTotalSize       = aPointsSize + aNormalsSize + aColorsSize + aTrianglesSize;
  aTotalSize >>= 20; //MB
  aNormalsSize >>= 20;
  aColorsSize >>= 20;
  aTrianglesSize >>= 20;
  aPolyConnectSize >>= 20;
  if (toPrintInfo)
  {
    std::cout << "NumberOfPoints:    " << aNumberPoints << "\n"
      << "NumberOfTriangles: " << aNumberTriangles << "\n"
      << "Amount of memory required for PolyTriangulation without Normals: " << (aTotalSize - aNormalsSize) << " Mb\n"
      << "Amount of memory for colors: " << aColorsSize << " Mb\n"
      << "Amount of memory for PolyConnect: " << aPolyConnectSize << " Mb\n"
      << "Amount of graphic card memory required: " << aTotalSize << " Mb\n";
  }

  // Setting material properties, very important for desirable visual result!
  Graphic3d_MaterialAspect aMat (Graphic3d_NameOfMaterial_Plastified);
  aMat.SetAmbientColor (Quantity_Color (Graphic3d_Vec3 (0.04f)));
  aMat.SetSpecularColor(Quantity_Color (Graphic3d_Vec3 (0.50f)));
  Handle(Graphic3d_AspectFillArea3d) anAspect
    = new Graphic3d_AspectFillArea3d (Aspect_IS_SOLID,
                                      Quantity_NOC_WHITE,
                                      Quantity_NOC_YELLOW,
                                      Aspect_TOL_SOLID,
                                      1.0,
                                      aMat,
                                      aMat);
  Handle(Prs3d_ShadingAspect) aShAsp = new Prs3d_ShadingAspect();
  anAspect->SetDrawEdges (toShowEdges);
  aShAsp->SetAspect (anAspect);
  aShape->Attributes()->SetShadingAspect (aShAsp);

  VDisplayAISObject (aShapeName, aShape);
  return 0;
}

//=============================================================================
//function : VComputeHLR
//purpose  :
//=============================================================================

static int VComputeHLR (Draw_Interpretor& ,
                        Standard_Integer theArgNb,
                        const char** theArgVec)
{
  TCollection_AsciiString aShapeName, aHlrName;
  TopoDS_Shape aSh;
  gp_Pnt anEye;
  gp_Dir aDir;
  gp_Ax2 aProjAx;
  bool hasViewDirArg = false;
  Prs3d_TypeOfHLR anAlgoType = Prs3d_TOH_PolyAlgo;
  bool toShowCNEdges = false, toShowHiddenEdges = false;
  int aNbIsolines = 0;
  if (Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext())
  {
    gp_Dir aRight;
    Handle(V3d_View) aView = ViewerTest::CurrentView();
    Standard_Integer aWidth, aHeight;
    Standard_Real aCentX, aCentY, aCentZ, aDirX, aDirY, aDirZ;
    Standard_Real aRightX, aRightY, aRightZ;
    aView->Window()->Size (aWidth, aHeight);

    aView->ConvertWithProj (aWidth, aHeight/2, 
                            aRightX, aRightY, aRightZ,
                            aDirX, aDirY, aDirZ);
    aView->ConvertWithProj (aWidth/2, aHeight/2, 
                            aCentX, aCentY, aCentZ,
                            aDirX, aDirY, aDirZ);

    anEye.SetCoord (-aCentX, -aCentY, -aCentZ);
    aDir.SetCoord (-aDirX, -aDirY, -aDirZ);
    aRight.SetCoord (aRightX - aCentX, aRightY - aCentY, aRightZ - aCentZ);
    aProjAx.SetLocation (anEye);
    aProjAx.SetDirection (aDir);
    aProjAx.SetXDirection (aRight);
  }
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgIter + 1 < theArgNb
     && (anArgCase == "-algotype"
      || anArgCase == "-algo"
      || anArgCase == "-type"))
    {
      TCollection_AsciiString anArgNext (theArgVec[++anArgIter]);
      anArgNext.LowerCase();
      if (anArgNext == "polyalgo")
      {
        anAlgoType = Prs3d_TOH_PolyAlgo;
      }
      else if (anArgNext == "algo")
      {
        anAlgoType = Prs3d_TOH_Algo;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown algo type '" << anArgNext << "'";
        return 1;
      }
    }
    else if (anArgCase == "-showhiddenedges"
          || anArgCase == "-hiddenedges"
          || anArgCase == "-hidden")
    {
      toShowHiddenEdges = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toShowHiddenEdges))
      {
        ++anArgIter;
      }
    }
    else if (anArgCase == "-showtangentedges"
          || anArgCase == "-tangentedges"
          || anArgCase == "-tangent")
    {
      toShowCNEdges = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toShowCNEdges))
      {
        ++anArgIter;
      }
    }
    else if (anArgIter + 1 < theArgNb
          && (anArgCase == "-nbiso"
           || anArgCase == "-nbisolines"))
    {
      aNbIsolines = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else if (aSh.IsNull())
    {
      aSh = DBRep::Get (theArgVec[anArgIter]);
      aShapeName = theArgVec[anArgIter];
      if (aSh.IsNull())
      {
        BRep_Builder aBrepBuilder;
        BRepTools::Read (aSh, theArgVec[anArgIter], aBrepBuilder);
        if (aSh.IsNull())
        {
          Message::SendFail() << "Syntax error: no shape with name " << theArgVec[anArgIter] << " found";
          return 1;
        }
      }
    }
    else if (aHlrName.IsEmpty())
    {
      aHlrName = theArgVec[anArgIter];
    }
    else if (!hasViewDirArg
          && anArgIter + 8 < theArgNb)
    {
      hasViewDirArg = true;
      gp_Dir anUp;
      anEye.SetCoord (Draw::Atof (theArgVec[anArgIter + 0]), Draw::Atof (theArgVec[anArgIter + 1]), Draw::Atof (theArgVec[anArgIter + 2]));
      aDir .SetCoord (Draw::Atof (theArgVec[anArgIter + 3]), Draw::Atof (theArgVec[anArgIter + 4]), Draw::Atof (theArgVec[anArgIter + 5]));
      anUp .SetCoord (Draw::Atof (theArgVec[anArgIter + 6]), Draw::Atof (theArgVec[anArgIter + 7]), Draw::Atof (theArgVec[anArgIter + 8]));
      aProjAx.SetLocation (anEye);
      aProjAx.SetDirection (aDir);
      aProjAx.SetYDirection (anUp);
      anArgIter += 8;
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aHlrName.IsEmpty() || aSh.IsNull()
   || (ViewerTest::GetAISContext().IsNull() && hasViewDirArg))
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  HLRAlgo_Projector aProjector (aProjAx);
  TopoDS_Shape aVisible[6];
  TopoDS_Shape aHidden[6];
  if (anAlgoType == Prs3d_TOH_PolyAlgo)
  {
    Handle(HLRBRep_PolyAlgo) aPolyAlgo = new HLRBRep_PolyAlgo();
    aPolyAlgo->Projector (aProjector);
    aPolyAlgo->Load (aSh);
    aPolyAlgo->Update();

    HLRBRep_PolyHLRToShape aHLRToShape;
    aHLRToShape.Update (aPolyAlgo);

    aVisible[HLRBRep_Sharp]   = aHLRToShape.VCompound();
    aVisible[HLRBRep_OutLine] = aHLRToShape.OutLineVCompound(); // extract visible outlines
    aVisible[HLRBRep_Rg1Line] = aHLRToShape.Rg1LineVCompound();
    if (toShowCNEdges)
    {
      aVisible[HLRBRep_RgNLine] = aHLRToShape.RgNLineVCompound();
    }
    if (toShowHiddenEdges)
    {
      aHidden[HLRBRep_Sharp]   = aHLRToShape.HCompound();
      aHidden[HLRBRep_OutLine] = aHLRToShape.OutLineHCompound();
      aHidden[HLRBRep_Rg1Line] = aHLRToShape.Rg1LineHCompound();
      if (toShowCNEdges)
      {
        aHidden[HLRBRep_RgNLine] = aHLRToShape.RgNLineHCompound();
      }
    }
  }
  else
  {
    Handle(HLRBRep_Algo) aHlrAlgo = new HLRBRep_Algo();
    aHlrAlgo->Add (aSh, aNbIsolines);
    aHlrAlgo->Projector (aProjector);
    aHlrAlgo->Update();
    aHlrAlgo->Hide();

    HLRBRep_HLRToShape aHLRToShape (aHlrAlgo);
    aVisible[HLRBRep_Sharp]   = aHLRToShape.VCompound();
    aVisible[HLRBRep_OutLine] = aHLRToShape.OutLineVCompound();
    aVisible[HLRBRep_Rg1Line] = aHLRToShape.Rg1LineVCompound();
    if (toShowCNEdges)
    {
      aVisible[HLRBRep_RgNLine] = aHLRToShape.RgNLineVCompound();
    }
    aVisible[HLRBRep_IsoLine] = aHLRToShape.IsoLineVCompound();

    if (toShowHiddenEdges)
    {
      aHidden[HLRBRep_Sharp]   = aHLRToShape.HCompound();
      aHidden[HLRBRep_OutLine] = aHLRToShape.OutLineHCompound();
      aHidden[HLRBRep_Rg1Line] = aHLRToShape.Rg1LineHCompound();
      if (toShowCNEdges)
      {
        aHidden[HLRBRep_RgNLine] = aHLRToShape.RgNLineHCompound();
      }
      aHidden[HLRBRep_IsoLine] = aHLRToShape.IsoLineHCompound();
    }
    // extract 3d
    //aVisible[HLRBRep_Sharp]   = aHLRToShape.CompoundOfEdges (HLRBRep_Sharp, Standard_True, Standard_True);
    //aVisible[HLRBRep_OutLine] = aHLRToShape.OutLineVCompound3d();
  }

  TopoDS_Compound aCompRes, aCompVis, aCompHid;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aCompVis);
  aBuilder.MakeCompound (aCompHid);
  aBuilder.MakeCompound (aCompRes);
  for (int aTypeIter = 0; aTypeIter < 6; ++aTypeIter)
  {
    if (!aVisible[aTypeIter].IsNull())
    {
      aBuilder.Add (aCompVis, aVisible[aTypeIter]);
    }
    if (!aHidden[aTypeIter].IsNull())
    {
      aBuilder.Add (aCompHid, aHidden[aTypeIter]);
    }
  }
  aBuilder.Add (aCompRes, aCompVis);
  aBuilder.Add (aCompRes, aCompHid);

  // create an AIS shape and display it
  if (!ViewerTest::GetAISContext().IsNull())
  {
    Handle(AIS_ColoredShape) anObject = new AIS_ColoredShape (aCompRes);
    if (toShowHiddenEdges)
    {
      Handle(Prs3d_LineAspect) aLineAspect = new Prs3d_LineAspect (Quantity_Color (Quantity_NOC_RED), Aspect_TOL_DASH, 1.0f);
      for (int aTypeIter = 0; aTypeIter < 6; ++aTypeIter)
      {
        if (!aHidden[aTypeIter].IsNull())
        {
          Handle(AIS_ColoredDrawer) aDrawer = anObject->CustomAspects (aHidden[aTypeIter]);
          aDrawer->SetLineAspect (aLineAspect);
          aDrawer->SetWireAspect (aLineAspect);
          aDrawer->SetFreeBoundaryAspect (aLineAspect);
          aDrawer->SetUnFreeBoundaryAspect (aLineAspect);
        }
      }
    }
    ViewerTest::Display (aHlrName, anObject, true);
  }

  DBRep::Set (aHlrName.ToCString(), aCompRes);
  return 0;
}

// This class is a wrap for Graphic3d_ArrayOfPrimitives; it is used for
// manipulating and displaying such an array with AIS context

class MyPArrayObject : public AIS_InteractiveObject
{

public:

  MyPArrayObject (const Handle(Graphic3d_ArrayOfPrimitives)& thePArray) : myPArray (thePArray) {}

  MyPArrayObject (Graphic3d_TypeOfPrimitiveArray thePrimType,
                  const Handle(TColStd_HArray1OfAsciiString)& theDesc,
                  const Handle(Graphic3d_AspectMarker3d)& theMarkerAspect)
  {
    Init (thePrimType, theDesc, theMarkerAspect, Standard_False);
  }

  //! Initialize the array from specified description.
  Standard_Boolean Init (Graphic3d_TypeOfPrimitiveArray thePrimType,
                         const Handle(TColStd_HArray1OfAsciiString)& theDesc,
                         const Handle(Graphic3d_AspectMarker3d)& theMarkerAspect,
                         Standard_Boolean theToPatch);

  DEFINE_STANDARD_RTTI_INLINE(MyPArrayObject,AIS_InteractiveObject);

  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Sets color to this interactive object
  //! @param theColor the color to be set
  virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;

private:

  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                const Handle(Prs3d_Presentation)& thePrs,
                const Standard_Integer theMode) Standard_OVERRIDE;

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) Standard_OVERRIDE;

  bool CheckInputCommand (const TCollection_AsciiString& theCommand,
                          const Handle(TColStd_HArray1OfAsciiString)& theArgsArray,
                          Standard_Integer &theArgIndex,
                          Standard_Integer theArgCount,
                          Standard_Integer theMaxArgs);

  //! Sets color for the shading aspect of the drawer used in this interactive object
  //! @param theColor the color to be set
  void setColorForShadingAspect(const Quantity_Color& theColor);

  //! Replaces shading aspect from myDrawer->Link() with the own shading aspect of myDrawer for this interactive object
  void replaceShadingAspect();

protected:

  Handle(Graphic3d_AspectMarker3d) myMarkerAspect;
  Handle(Graphic3d_ArrayOfPrimitives) myPArray;

};

void MyPArrayObject::Compute (const Handle(PrsMgr_PresentationManager)& ,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer theMode)
{
  if (myPArray.IsNull() || theMode != 0)
  {
    return;
  }

  Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
  if (!myMarkerAspect.IsNull())
  {
    aGroup->SetGroupPrimitivesAspect (myMarkerAspect);
  }
  else
  {
    aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
  }
  aGroup->AddPrimitiveArray (myPArray);
}

Standard_Boolean MyPArrayObject::Init (Graphic3d_TypeOfPrimitiveArray thePrimType,
                                       const Handle(TColStd_HArray1OfAsciiString)& theDesc,
                                       const Handle(Graphic3d_AspectMarker3d)& theMarkerAspect,
                                       Standard_Boolean theToPatch)
{
  myMarkerAspect = theMarkerAspect;
  if (!theToPatch)
  {
    myPArray.Nullify();
  }

  // Parsing array description
  Standard_Integer aVertexNum = 0, aBoundNum = 0, aEdgeNum = 0;
  Graphic3d_ArrayFlags anArrayFlags = Graphic3d_ArrayFlags_None;

  const Standard_Integer anArgsCount = theDesc->Length();
  TCollection_AsciiString aCommand;
  for (Standard_Integer anArgIndex = theDesc->Lower(); anArgIndex <= theDesc->Upper(); )
  {
    aCommand = theDesc->Value (anArgIndex);
    aCommand.LowerCase();

    if (CheckInputCommand ("-deinterleaved", theDesc, anArgIndex, 0, anArgsCount))
    {
      anArrayFlags |= Graphic3d_ArrayFlags_AttribsDeinterleaved;
    }
    else if (CheckInputCommand ("-mutable", theDesc, anArgIndex, 0, anArgsCount))
    {
      anArrayFlags |= Graphic3d_ArrayFlags_AttribsMutable;
      anArrayFlags |= Graphic3d_ArrayFlags_IndexesMutable;
    }
    // vertex command
    else if (CheckInputCommand ("v", theDesc, anArgIndex, 3, anArgsCount))
    {
      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", theDesc, anArgIndex, 3, anArgsCount))
      {
        anArrayFlags = anArrayFlags | Graphic3d_ArrayFlags_VertexNormal;
      }

      // vertex has a color
      if (CheckInputCommand ("c", theDesc, anArgIndex, 3, anArgsCount))
      {
        anArrayFlags = anArrayFlags | Graphic3d_ArrayFlags_VertexColor;
      }

      // vertex has a texel
      if (CheckInputCommand ("t", theDesc, anArgIndex, 2, anArgsCount))
      {
        anArrayFlags = anArrayFlags | Graphic3d_ArrayFlags_VertexTexel;
      }

      aVertexNum++;
    }
    // bound command
    else if (CheckInputCommand ("b", theDesc, anArgIndex, 1, anArgsCount))
    {
      // bound has color
      if (CheckInputCommand ("c", theDesc, anArgIndex, 3, anArgsCount))
      {
        anArrayFlags = anArrayFlags | Graphic3d_ArrayFlags_BoundColor;
      }

      aBoundNum++;
    }
    // edge command
    else if (CheckInputCommand ("e", theDesc, anArgIndex, 1, anArgsCount))
    {
      aEdgeNum++;
    }
    // unknown command
    else
      anArgIndex++;
  }

  if (myPArray.IsNull())
  {
    myPArray = Graphic3d_ArrayOfPrimitives::CreateArray (thePrimType, aVertexNum, aBoundNum, aEdgeNum, anArrayFlags);
    if (myPArray->HasVertexColors())
    {
      myDrawer->SetupOwnShadingAspect();
      Graphic3d_MaterialAspect aMat (Graphic3d_NameOfMaterial_Plastified);
      aMat.SetSpecularColor (Quantity_NOC_BLACK);
      aMat.SetEmissiveColor (Quantity_NOC_BLACK);
      aMat.SetAmbientColor (Quantity_Color (Graphic3d_Vec3 (0.5f)));
      aMat.SetDiffuseColor (Quantity_Color (Graphic3d_Vec3 (0.5f)));
      myDrawer->ShadingAspect()->SetMaterial (aMat);
      myDrawer->ShadingAspect()->SetColor (Quantity_NOC_WHITE);
    }
  }
  else
  {
    if (myPArray->Type() != thePrimType
    ||  aVertexNum > myPArray->VertexNumberAllocated()
    ||  aEdgeNum   > myPArray->EdgeNumberAllocated()
    ||  aBoundNum  > myPArray->BoundNumberAllocated()
    || !myPArray->Attributes()->IsMutable()
    || (!myPArray->Indices().IsNull() && !myPArray->Indices()->IsMutable()))
    {
      Message::SendFail ("Syntax error: array cannot be patched");
      return Standard_False;
    }

    myPArray->Attributes()->NbElements = aVertexNum;
    if (Handle(Graphic3d_AttribBuffer) anAttribs = Handle(Graphic3d_AttribBuffer)::DownCast (myPArray->Attributes()))
    {
      anAttribs->Invalidate (0, aVertexNum - 1);
    }
    if (!myPArray->Indices().IsNull())
    {
      myPArray->Indices()->NbElements = aEdgeNum;
    }
    if (!myPArray->Bounds().IsNull())
    {
      myPArray->Bounds()->NbBounds = aBoundNum;
    }
  }

  Standard_Integer aVertIndex = 0;
  for (Standard_Integer anArgIndex = theDesc->Lower(); anArgIndex <= theDesc->Upper(); )
  {
    aCommand = theDesc->Value (anArgIndex);
    aCommand.LowerCase();
    if (!aCommand.IsAscii()
      || aCommand.IsEmpty())
    {
      break;
    }

    // skip beautifiers (syntax is not actually validated)
    if (aCommand == "-deinterleaved"
     || aCommand == "-mutable"
     || aCommand.Value (1) == '('
     || aCommand.Value (1) == ')'
     || aCommand.Value (1) == ',')
    {
      ++anArgIndex;
    }
    // vertex command
    else if (CheckInputCommand ("v", theDesc, anArgIndex, 3, anArgsCount))
    {
      const Graphic3d_Vec3 aVert ((float )theDesc->Value (anArgIndex - 3).RealValue(),
                                  (float )theDesc->Value (anArgIndex - 2).RealValue(),
                                  (float )theDesc->Value (anArgIndex - 1).RealValue());
      if ((anArrayFlags & Graphic3d_ArrayFlags_AttribsDeinterleaved) != 0
       || (anArrayFlags & Graphic3d_ArrayFlags_AttribsMutable) != 0)
      {
        ++aVertIndex;
        myPArray->SetVertice (aVertIndex, aVert.x(), aVert.y(), aVert.z());
      }
      else
      {
        aVertIndex = myPArray->AddVertex (aVert);
      }

      // vertex has a normal or normal with color or texel
      if (CheckInputCommand ("n", theDesc, anArgIndex, 3, anArgsCount))
      {
        const Graphic3d_Vec3 aNorm ((float )theDesc->Value (anArgIndex - 3).RealValue(),
                                    (float )theDesc->Value (anArgIndex - 2).RealValue(),
                                    (float )theDesc->Value (anArgIndex - 1).RealValue());
        myPArray->SetVertexNormal (aVertIndex, aNorm.x(), aNorm.y(), aNorm.z());
      }
      
      if (CheckInputCommand ("c", theDesc, anArgIndex, 3, anArgsCount))
      {
        const Graphic3d_Vec3d aCol (theDesc->Value (anArgIndex - 3).RealValue(),
                                    theDesc->Value (anArgIndex - 2).RealValue(),
                                    theDesc->Value (anArgIndex - 1).RealValue());
        myPArray->SetVertexColor (aVertIndex, aCol.r(), aCol.g(), aCol.b());
      }
      if (CheckInputCommand ("t", theDesc, anArgIndex, 2, anArgsCount))
      {
        const Graphic3d_Vec2 aTex ((float )theDesc->Value (anArgIndex - 2).RealValue(),
                                   (float )theDesc->Value (anArgIndex - 1).RealValue());
        myPArray->SetVertexTexel (aVertIndex, aTex.x(), aTex.y());
      }
    }
    // bounds command
    else if (CheckInputCommand ("b", theDesc, anArgIndex, 1, anArgsCount))
    {
      Standard_Integer aVertCount = theDesc->Value (anArgIndex - 1).IntegerValue();

      if (CheckInputCommand ("c", theDesc, anArgIndex, 3, anArgsCount))
        myPArray->AddBound (aVertCount,
                            theDesc->Value (anArgIndex - 3).RealValue(),
                            theDesc->Value (anArgIndex - 2).RealValue(),
                            theDesc->Value (anArgIndex - 1).RealValue());

      else
        myPArray->AddBound (aVertCount);
    }
    // edge command
    else if (CheckInputCommand ("e", theDesc, anArgIndex, 1, anArgsCount))
    {
      const Standard_Integer anEdge = theDesc->Value (anArgIndex - 1).IntegerValue();
      myPArray->AddEdge (anEdge);
    }
    // unknown command
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theDesc->Value(anArgIndex) << "'";
      return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
// function : SetColor
// purpose  :
//=======================================================================
void MyPArrayObject::SetColor (const Quantity_Color& theColor)
{
  AIS_InteractiveObject::SetColor (theColor);
  setColorForShadingAspect (theColor);
  if (myMarkerAspect)
  {
    myMarkerAspect->SetColor (theColor);
  }
  SynchronizeAspects();
}

void MyPArrayObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Integer theMode)
{
  if (theMode != 0
   || myPArray.IsNull())
  {
    return;
  }

  Handle(SelectMgr_EntityOwner) anOwner = new SelectMgr_EntityOwner (this);
  if (Handle(Graphic3d_ArrayOfTriangles) aTris = Handle(Graphic3d_ArrayOfTriangles)::DownCast (myPArray))
  {
    Handle(Select3D_SensitivePrimitiveArray) aSensitive = new Select3D_SensitivePrimitiveArray (anOwner);
    aSensitive->InitTriangulation (myPArray->Attributes(), myPArray->Indices(), TopLoc_Location(), true);
    theSelection->Add (aSensitive);
  }
  else if (Handle(Graphic3d_ArrayOfSegments) aSegs = Handle(Graphic3d_ArrayOfSegments)::DownCast (myPArray))
  {
    if (aSegs->EdgeNumber() > 0)
    {
      for (Standard_Integer aPntIter = 1; aPntIter <= aSegs->EdgeNumber(); aPntIter += 2)
      {
        Handle(Select3D_SensitiveSegment) aSeg = new Select3D_SensitiveSegment (anOwner, aSegs->Vertice (aSegs->Edge (aPntIter)), aSegs->Vertice (aSegs->Edge (aPntIter + 1)));
        aSeg->SetSensitivityFactor (4);
        theSelection->Add (aSeg);
      }
    }
    else
    {
      for (Standard_Integer aPntIter = 1; aPntIter <= aSegs->VertexNumber(); aPntIter += 2)
      {
        Handle(Select3D_SensitiveSegment) aSeg = new Select3D_SensitiveSegment (anOwner, aSegs->Vertice (aPntIter), aSegs->Vertice (aPntIter + 1));
        aSeg->SetSensitivityFactor (4);
        theSelection->Add (aSeg);
      }
    }
  }
  else
  {
    Handle(Select3D_SensitivePrimitiveArray) aSensitive = new Select3D_SensitivePrimitiveArray (anOwner);
    aSensitive->SetSensitivityFactor (8);
    aSensitive->InitPoints (myPArray->Attributes(), myPArray->Indices(), TopLoc_Location(), true);
    theSelection->Add (aSensitive);
  }
}

bool MyPArrayObject::CheckInputCommand (const TCollection_AsciiString& theCommand,
                                       const Handle(TColStd_HArray1OfAsciiString)& theArgsArray,
                                       Standard_Integer &theArgIndex,
                                       Standard_Integer theArgCount,
                                       Standard_Integer theMaxArgs)
{
  // check if there is more elements than expected
  if (theArgIndex >= theMaxArgs)
    return false;

  TCollection_AsciiString aStrCommand = theArgsArray->Value (theArgIndex);
  aStrCommand.LowerCase();
  if (aStrCommand.Search(theCommand) != 1 ||
      theArgIndex + (theArgCount - 1) >= theMaxArgs)
    return false;

  // go to the first data element
  theArgIndex++;

  // check data if it can be converted to numeric
  for (int aElement = 0; aElement < theArgCount; aElement++, theArgIndex++)
  {
    aStrCommand = theArgsArray->Value (theArgIndex);
    if (!aStrCommand.IsRealValue (Standard_True))
      return false;
  }

  return true;
}

//=======================================================================
// function : setColorForShadingAspect
// purpose  :
//=======================================================================
void MyPArrayObject::setColorForShadingAspect (const Quantity_Color& theColor)
{
  if (myDrawer->SetupOwnShadingAspect())
  {
    replaceShadingAspect();
  }
  myDrawer->ShadingAspect()->SetColor (theColor);
}

//=======================================================================
// function : replaceShadingAspect
// purpose  :
//=======================================================================
void MyPArrayObject::replaceShadingAspect()
{
  if (!myDrawer->Link())
  {
    return;
  }
  Graphic3d_MapOfAspectsToAspects anAspectReplacementMap;
  anAspectReplacementMap.Bind (myDrawer->Link()->ShadingAspect()->Aspect(), myDrawer->ShadingAspect()->Aspect());
  replaceAspects (anAspectReplacementMap);
}

//=============================================================================
//function : VDrawPArray
//purpose  : Draws primitives array from list of vertexes, bounds, edges
//=============================================================================

static int VDrawPArray (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail ("Error: no active Viewer");
    return 1;
  }
  else if (argc < 3)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  // read the arguments
  Standard_Integer anArgIndex = 1;
  TCollection_AsciiString aName (argv[anArgIndex++]);
  TCollection_AsciiString anArrayType (argv[anArgIndex++]);
  anArrayType.LowerCase();
  Handle(MyPArrayObject) aPObject;
  if (anArrayType == "-shape")
  {
    Standard_CString aShapeName = argv[anArgIndex++];
    TopoDS_Shape aShape = DBRep::Get (aShapeName);
    Handle(Graphic3d_ArrayOfPrimitives) aTris = StdPrs_ShadedShape::FillTriangles (aShape);
    if (aShape.IsNull())
    {
      Message::SendFail() << "Syntax error: shape '" << aShapeName << "' is not found";
      return 1;
    }
    else if (aTris.IsNull())
    {
      Message::SendFail() << "Syntax error: shape '" << aShapeName << "' is not triangulated";
      return 1;
    }

    aPObject = new MyPArrayObject (aTris);
    ViewerTest::Display (aName, aPObject);
    return 0;
  }
  else if (anArrayType == "-patch"
        || anArrayType == "-modify"
        || anArrayType == "-edit")
  {
    if (argc >= 3)
    {
      anArrayType = argv[anArgIndex++];
      anArrayType.LowerCase();
    }

    if (GetMapOfAIS().IsBound2 (aName))
    {
      aPObject = Handle(MyPArrayObject)::DownCast (GetMapOfAIS().Find2 (aName));
    }
    if (aPObject.IsNull())
    {
      Message::SendFail() << "Syntax error: object '" << aName << "' cannot be found";
      return 1;
    }
  }

  Standard_Boolean hasVertex = Standard_False;

  Graphic3d_TypeOfPrimitiveArray aPrimType = Graphic3d_TOPA_UNDEFINED;
  if (anArrayType == "points")
  {
    aPrimType = Graphic3d_TOPA_POINTS;
  }
  else if (anArrayType == "segments")
  {
    aPrimType = Graphic3d_TOPA_SEGMENTS;
  }
  else if (anArrayType == "polylines")
  {
    aPrimType = Graphic3d_TOPA_POLYLINES;
  }
  else if (anArrayType == "triangles")
  {
    aPrimType = Graphic3d_TOPA_TRIANGLES;
  }
  else if (anArrayType == "trianglefans")
  {
    aPrimType = Graphic3d_TOPA_TRIANGLEFANS;
  }
  else if (anArrayType == "trianglestrips")
  {
    aPrimType = Graphic3d_TOPA_TRIANGLESTRIPS;
  }
  else if (anArrayType == "quads")
  {
    aPrimType = Graphic3d_TOPA_QUADRANGLES;
  }
  else if (anArrayType == "quadstrips")
  {
    aPrimType = Graphic3d_TOPA_QUADRANGLESTRIPS;
  }
  else if (anArrayType == "polygons")
  {
    aPrimType = Graphic3d_TOPA_POLYGONS;
  }
  if (aPrimType == Graphic3d_TOPA_UNDEFINED)
  {
    Message::SendFail ("Syntax error: unexpected type of primitives array");
    return 1;
  }

  Standard_Integer aLowerArg = anArgIndex;
  Handle(TColStd_HArray1OfAsciiString) anArgsArray = new TColStd_HArray1OfAsciiString (0, argc - 3);
  for (; anArgIndex < argc; ++anArgIndex)
  {
    TCollection_AsciiString aCommand (argv[anArgIndex]);
    aCommand.LowerCase();
    if (!aCommand.IsAscii())
    {
      di << "Unexpected argument: #" << anArgIndex - 1 << " , "
         << "should be an array element: 'v', 'b', 'e' \n";
      break;
    }

    if (aCommand == "v")
    {
      hasVertex = Standard_True;
    }

    anArgsArray->SetValue (anArgIndex - aLowerArg, aCommand);
  }

  if (!hasVertex)
  {
    di << "You should pass any verticies in the list of array elements\n";
    return 1;
  }

  Handle(Graphic3d_AspectMarker3d)    anAspPoints;
  if (aPrimType == Graphic3d_TOPA_POINTS)
  {
    anAspPoints = new Graphic3d_AspectMarker3d (Aspect_TOM_POINT, Quantity_NOC_YELLOW, 1.0f);
  }

  // create primitives array object
  if (aPObject.IsNull())
  {
    // register the object in map
    aPObject = new MyPArrayObject (aPrimType, anArgsArray, anAspPoints);
    VDisplayAISObject (aName, aPObject);
  }
  else
  {
    aPObject->Init (aPrimType, anArgsArray, anAspPoints, Standard_True);
    ViewerTest::CurrentView()->Redraw();
  }
  return 0;
}

namespace
{
  //! Auxiliary function for parsing translation vector - either 2D or 3D.
  static Standard_Integer parseTranslationVec (Standard_Integer theArgNb,
                                               const char**     theArgVec,
                                               gp_Vec&          theVec)
  {
    if (theArgNb < 2)
    {
      return 0;
    }

    TCollection_AsciiString anX (theArgVec[0]);
    TCollection_AsciiString anY (theArgVec[1]);
    if (!anX.IsRealValue (Standard_True)
     || !anY.IsRealValue (Standard_True))
    {
      return 0;
    }

    theVec.SetX (anX.RealValue());
    theVec.SetY (anY.RealValue());
    if (theArgNb >= 3)
    {
      TCollection_AsciiString anZ (theArgVec[2]);
      if (anZ.IsRealValue (Standard_True))
      {
        theVec.SetZ (anZ.RealValue());
        return 3;
      }
    }
    return 2;
  }
}

//=======================================================================
//function : VSetLocation
//purpose  : Change location of AIS interactive object
//=======================================================================

static Standard_Integer VSetLocation (Draw_Interpretor& theDI,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toPrintInfo = Standard_True;
  Handle(AIS_InteractiveObject) anObj;
  TCollection_AsciiString aCmdName (theArgVec[0]);
  aCmdName.LowerCase();
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg = theArgVec[anArgIter];
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anObj.IsNull())
    {
      const TCollection_AsciiString aName (theArgVec[anArgIter]);
      GetMapOfAIS().Find2 (aName, anObj);
      if (anObj.IsNull())
      {
        Message::SendFail() << "Error: object '" << aName << "' is not displayed";
        return 1;
      }
    }
    else if (anArg == "-reset")
    {
      toPrintInfo = Standard_False;
      aContext->SetLocation (anObj, gp_Trsf());
    }
    else if (anArg == "-copyfrom"
          || anArg == "-copy")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aName2 (theArgVec[anArgIter + 1]);
      Handle(AIS_InteractiveObject) anObj2;
      GetMapOfAIS().Find2 (aName2, anObj2);
      if (anObj2.IsNull())
      {
        Message::SendFail() << "Error: object '" << aName2 << "' is not displayed";
        return 1;
      }

      ++anArgIter;
      aContext->SetLocation (anObj, anObj2->LocalTransformation());
    }
    else if (anArg == "-rotate"
          || anArg == "-prerotate")
    {
      toPrintInfo = Standard_False;
      if (anArgIter + 7 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      gp_Trsf aTrsf;
      aTrsf.SetRotation (gp_Ax1 (gp_Pnt (Draw::Atof (theArgVec[anArgIter + 1]),
                                         Draw::Atof (theArgVec[anArgIter + 2]),
                                         Draw::Atof (theArgVec[anArgIter + 3])),
                                 gp_Vec (Draw::Atof (theArgVec[anArgIter + 4]),
                                         Draw::Atof (theArgVec[anArgIter + 5]),
                                         Draw::Atof (theArgVec[anArgIter + 6]))),
                                         Draw::Atof (theArgVec[anArgIter + 7]) * (M_PI / 180.0));
      anArgIter += 7;

      if (anArg == "-prerotate")
      {
        aTrsf = anObj->LocalTransformation() * aTrsf;
      }
      else
      {
        aTrsf = aTrsf * anObj->LocalTransformation();
     }
      aContext->SetLocation (anObj, aTrsf);
    }
    else if (anArg == "-translate"
          || anArg == "-pretranslate")
    {
      toPrintInfo = Standard_False;
      gp_Vec aLocVec;
      Standard_Integer aNbParsed = parseTranslationVec (theArgNb - anArgIter - 1, theArgVec + anArgIter + 1, aLocVec);
      anArgIter += aNbParsed;
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      gp_Trsf aTrsf;
      aTrsf.SetTranslationPart (aLocVec);
      if (anArg == "-pretranslate")
      {
        aTrsf = anObj->LocalTransformation() * aTrsf;
      }
      else
      {
        aTrsf = aTrsf * anObj->LocalTransformation();
      }
      aContext->SetLocation (anObj, aTrsf);
    }
    else if (anArg == "-scale"
          || anArg == "-prescale"
          || anArg == "-setscale")
    {
      toPrintInfo = Standard_False;
      gp_XYZ aScaleLoc;
      Standard_Real aScale = 1.0;
      Standard_Boolean toPrintScale = Standard_True;
      Standard_Boolean hasScaleLoc  = Standard_False;
      if (anArgIter + 4 < theArgNb)
      {
        TCollection_AsciiString aScaleArgs[4] =
        {
          TCollection_AsciiString (theArgVec[anArgIter + 1]),
          TCollection_AsciiString (theArgVec[anArgIter + 2]),
          TCollection_AsciiString (theArgVec[anArgIter + 3]),
          TCollection_AsciiString (theArgVec[anArgIter + 4])
        };
        Standard_Integer aScaleArgIter = 0;
        for (; aScaleArgIter < 4; ++aScaleArgIter)
        {
          if (!aScaleArgs[aScaleArgIter].IsRealValue (Standard_True))
          {
            break;
          }
        }
        if (aScaleArgIter == 4)
        {
          aScaleLoc.SetCoord (aScaleArgs[0].RealValue(), aScaleArgs[1].RealValue(), aScaleArgs[2].RealValue());
          aScale = aScaleArgs[3].RealValue();
          anArgIter += 4;
          toPrintScale = Standard_False;
          hasScaleLoc  = Standard_True;
        }
        else if (aScaleArgIter >= 1)
        {
          aScale = aScaleArgs[0].RealValue();
          ++anArgIter;
          toPrintScale = Standard_False;
        }
      }
      else if (anArgIter + 1 < theArgNb)
      {
        TCollection_AsciiString aScaleArg (theArgVec[anArgIter + 1]);
        if (aScaleArg.IsRealValue (Standard_True))
        {
          aScale = aScaleArg.RealValue();
          ++anArgIter;
          toPrintScale = Standard_False;
        }
      }

      if (toPrintScale)
      {
        if (anArg == "-setscale"
         || anArg == "-prescale")
        {
          Message::SendFail() << "Syntax error at '" << anArg << "'";
          return 1;
        }

        char aText[1024];
        Sprintf (aText, "%g ", anObj->LocalTransformation().ScaleFactor());
        theDI << aText;
        continue;
      }

      if (anArg == "-setscale")
      {
        gp_Trsf aTrsf = anObj->LocalTransformation();
        if (hasScaleLoc)
        {
          aTrsf.SetScale (aScaleLoc, aScale);
        }
        else
        {
          aTrsf.SetScaleFactor (aScale);
        }
        aContext->SetLocation (anObj, aTrsf);
      }
      else
      {
        gp_Trsf aTrsf;
        if (hasScaleLoc)
        {
          aTrsf.SetScale (aScaleLoc, aScale);
        }
        else
        {
          aTrsf.SetScaleFactor (aScale);
        }

        if (anArg == "-prescale")
        {
          aTrsf = anObj->LocalTransformation() * aTrsf;
        }
        else
        {
          aTrsf = aTrsf * anObj->LocalTransformation();
        }
        aContext->SetLocation (anObj, aTrsf);
      }
    }
    else if (anArg == "-mirror"
          || anArg == "-premirror")
    {
      toPrintInfo = Standard_False;
      if (anArgIter + 6 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      gp_Trsf aTrsf;
      aTrsf.SetMirror (gp_Ax2 (gp_Pnt (Draw::Atof(theArgVec[theArgNb - 6]),
                                       Draw::Atof(theArgVec[theArgNb - 5]),
                                       Draw::Atof(theArgVec[theArgNb - 4])),
                               gp_Vec (Draw::Atof(theArgVec[theArgNb - 3]),
                                       Draw::Atof(theArgVec[theArgNb - 2]),
                                       Draw::Atof(theArgVec[theArgNb - 1]))));
      anArgIter += 6;
      if (anArg == "-premirror")
      {
        aTrsf = anObj->LocalTransformation() * aTrsf;
      }
      else
      {
        aTrsf = aTrsf * anObj->LocalTransformation();
      }
      aContext->SetLocation (anObj, aTrsf);
    }
    else if (anArg == "-setrotation"
          || anArg == "-rotation")
    {
      toPrintInfo = Standard_False;
      if (anArgIter + 4 < theArgNb)
      {
        TCollection_AsciiString aQuatArgs[4] =
        {
          TCollection_AsciiString (theArgVec[anArgIter + 1]),
          TCollection_AsciiString (theArgVec[anArgIter + 2]),
          TCollection_AsciiString (theArgVec[anArgIter + 3]),
          TCollection_AsciiString (theArgVec[anArgIter + 4])
        };
        Standard_Integer aQuatArgIter = 0;
        for (; aQuatArgIter < 4; ++aQuatArgIter)
        {
          if (!aQuatArgs[aQuatArgIter].IsRealValue (Standard_True))
          {
            break;
          }
        }

        if (aQuatArgIter == 4)
        {
          anArgIter += 4;
          const gp_Quaternion aQuat (aQuatArgs[0].RealValue(),
                                     aQuatArgs[1].RealValue(),
                                     aQuatArgs[2].RealValue(),
                                     aQuatArgs[3].RealValue());
          gp_Trsf aTrsf = anObj->LocalTransformation();
          aTrsf.SetRotationPart (aQuat);
          aContext->SetLocation (anObj, aTrsf);
          continue;
        }
        else if (anArg == "-setrotation")
        {
          Message::SendFail() << "Syntax error at '" << anArg << "'";
          return 1;
        }
      }

      char aText[1024];
      const gp_Quaternion aQuat = anObj->LocalTransformation().GetRotation();
      Sprintf (aText, "%g %g %g %g ", aQuat.X(), aQuat.Y(), aQuat.Z(), aQuat.W());
      theDI << aText;
    }
    else if (anArg == "-setlocation"
          || anArg == "-location")
    {
      toPrintInfo = Standard_False;
      gp_Vec aLocVec;
      Standard_Integer aNbParsed = parseTranslationVec (theArgNb - anArgIter - 1, theArgVec + anArgIter + 1, aLocVec);
      anArgIter += aNbParsed;
      if (aNbParsed != 0)
      {
        gp_Trsf aTrsf = anObj->LocalTransformation();
        aTrsf.SetTranslationPart (aLocVec);
        aContext->SetLocation (anObj, aTrsf);
      }
      else if (anArg == "-setlocation")
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      char aText[1024];
      const gp_XYZ aLoc = anObj->LocalTransformation().TranslationPart();
      Sprintf (aText, "%g %g %g ", aLoc.X(), aLoc.Y(), aLoc.Z());
      theDI << aText;
    }
    else if (aCmdName == "vsetlocation")
    {
      // compatibility with old syntax
      gp_Vec aLocVec;
      Standard_Integer aNbParsed = parseTranslationVec (theArgNb - anArgIter, theArgVec + anArgIter, aLocVec);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter = anArgIter + aNbParsed - 1;

      gp_Trsf aTrsf;
      aTrsf.SetTranslationPart (aLocVec);
      aContext->SetLocation (anObj, aTrsf);
      toPrintInfo = Standard_False;
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  if (anObj.IsNull())
  {
    Message::SendFail ("Syntax error - wrong number of arguments");
    return 1;
  }
  else if (!toPrintInfo)
  {
    return 0;
  }

  const gp_Trsf       aTrsf = anObj->LocalTransformation();
  const gp_XYZ        aLoc  = aTrsf.TranslationPart();
  const gp_Quaternion aRot  = aTrsf.GetRotation();
  char aText[4096];
  Sprintf (aText, "Location: %g %g %g\n"
                  "Rotation: %g %g %g %g\n"
                  "Scale:    %g\n",
                  aLoc.X(), aLoc.Y(), aLoc.Z(),
                  aRot.X(), aRot.Y(), aRot.Z(), aRot.W(),
                  aTrsf.ScaleFactor());
  theDI << aText;
  return 0;
}

//! Find displayed object.
static Handle(AIS_InteractiveObject) findConnectedObject (const TCollection_AsciiString& theName)
{
  Handle(AIS_InteractiveObject) aPrs;
  if (!GetMapOfAIS().Find2 (theName, aPrs))
  {
    return Handle(AIS_InteractiveObject)();
  }
  if (Handle(AIS_ConnectedInteractive) aConnected = Handle(AIS_ConnectedInteractive)::DownCast (aPrs))
  {
    return aConnected;
  }
  else if (Handle(AIS_MultipleConnectedInteractive) aMultiCon = Handle(AIS_MultipleConnectedInteractive)::DownCast (aPrs))
  {
    return aMultiCon;
  }

  // replace already displayed object with connected one
  TheAISContext()->Remove (aPrs, false);
  Handle(AIS_ConnectedInteractive) aConnected = new AIS_ConnectedInteractive();
  if (aPrs->HasDisplayMode())
  {
    aConnected->SetDisplayMode (aPrs->DisplayMode());
  }
  aConnected->Connect (aPrs, aPrs->LocalTransformationGeom());
  if (!aPrs->TransformPersistence().IsNull())
  {
    aConnected->SetTransformPersistence (aPrs->TransformPersistence());
  }
  ViewerTest::Display (theName, aConnected, false);
  return aConnected;
}

//===============================================================================================
//function : VConnect
//purpose  : Creates and displays AIS_ConnectedInteractive object from input object and location
//===============================================================================================
static Standard_Integer VConnect (Draw_Interpretor& /*di*/,
                                  Standard_Integer argc,
                                  const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  if (argc < 6)
  {
    Message::SendFail ("Syntax error: expect at least 5 arguments");
    return 1;
  }

  // Get values
  Standard_Integer anArgIter = 1;
  const TCollection_AsciiString aName (argv[anArgIter++]);
  Handle(AIS_MultipleConnectedInteractive) aMultiConObject;
  TCollection_AsciiString aColorString (argv[argc-1]);
  Quantity_Color aColor;
  Standard_Boolean hasColor = Standard_False;
  if (aColorString.Search ("color=") != -1)
  {
    hasColor = Standard_True;
    aColorString.Remove (1, 6);
    if (!Quantity_Color::ColorFromName (aColorString.ToCString(), aColor))
    {
      Message::SendFail() << "Syntax error at " << aColorString;
      return 1;
    }
  }

  const Standard_Integer aNbShapes = hasColor ? (argc - 1) : argc;
  for (Standard_Integer i = 5; i < aNbShapes; ++i)
  {
    TCollection_AsciiString anOriginObjectName (argv[i]);
    Handle(AIS_InteractiveObject) anObject;
    if (aName.IsEqual (anOriginObjectName))
    {
      Message::SendFail ("Syntax error: equal names for connected objects");
      continue;
    }

    anObject = findConnectedObject (anOriginObjectName);
    if (anObject.IsNull())
    {
      TopoDS_Shape aTDShape = DBRep::Get (anOriginObjectName);
      if (aTDShape.IsNull())
      {
        Message::SendFail() << "Syntax error: object " << anOriginObjectName << " doesn't exist";
        return 1;
      }
      Handle(AIS_Shape) aShapePrs = new AIS_Shape (aTDShape);
      Handle(AIS_ConnectedInteractive) aConnectedOrig = new AIS_ConnectedInteractive();
      aConnectedOrig->Connect (aShapePrs);
      anObject = aConnectedOrig;

      aContext->Load (anObject);
      anObject->SetColor (aColor);
    }

    if (aMultiConObject.IsNull())
    {
      aMultiConObject = new AIS_MultipleConnectedInteractive();
    }

    aMultiConObject->Connect (anObject);
  }
  if (aMultiConObject.IsNull())
  {
    Message::SendFail ("Syntax error: can't connect input objects");
    return 1;
  }

  // Create transformation
  gp_Trsf aTrsf; 
  aTrsf.SetTranslationPart (gp_Vec (Draw::Atof (argv[anArgIter + 0]),
                                    Draw::Atof (argv[anArgIter + 1]),
                                    Draw::Atof (argv[anArgIter + 2])));
  TopLoc_Location aLocation (aTrsf);
  anArgIter += 3;

  aMultiConObject->SetLocalTransformation (aTrsf);

  ViewerTest::Display (aName, aMultiConObject, true);
  return 0;
}

//===============================================================================================
//function : VConnectTo
//purpose  : Creates and displays AIS_ConnectedInteractive object from input object and location 
//===============================================================================================
static Standard_Integer VConnectTo (Draw_Interpretor& /*di*/,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  if (argc != 6 && argc != 7)
  {
    Message::SendFail ("Syntax error: expect at least 5 arguments");
    return 1;
  }

  Standard_Integer anArgIter = 1;
  TCollection_AsciiString aName (argv[anArgIter++]);
  Handle(AIS_InteractiveObject) anOriginObject;

  TCollection_AsciiString anOriginObjectName(argv[5]);
  if (aName.IsEqual (anOriginObjectName))
  {
    Message::SendFail ("Syntax error: equal names for connected objects");
    return 1;
  }
  anOriginObject = findConnectedObject (anOriginObjectName);
  if (anOriginObject.IsNull())
  {
    TopoDS_Shape aTDShape = DBRep::Get (anOriginObjectName);
    if (aTDShape.IsNull())
    {
      Message::SendFail() << "Syntax error: object " << anOriginObjectName << " doesn't exist";
      return 1;
    }

    Handle(AIS_Shape) aShapePrs = new AIS_Shape (aTDShape);
    Handle(AIS_ConnectedInteractive) aConnectedOrig = new AIS_ConnectedInteractive();
    aConnectedOrig->Connect (aShapePrs);

    anOriginObject = aConnectedOrig;
    GetMapOfAIS().Bind (aConnectedOrig, anOriginObjectName);
  }

  // Create transformation
  gp_Trsf aTrsf;
  aTrsf.SetTranslationPart (gp_Vec (Draw::Atof (argv[anArgIter + 0]),
                                    Draw::Atof (argv[anArgIter + 1]),
                                    Draw::Atof (argv[anArgIter + 2])));
  anArgIter += 3;

  Handle(AIS_ConnectedInteractive) aConnected = new AIS_ConnectedInteractive();
  aConnected->Connect (anOriginObject, aTrsf);
  if (argc == 7)
  {
    TCollection_AsciiString anArg = argv[6];
    anArg.LowerCase();
    if (anArg == "-nodisplay")
    {
      // bind connected object without displaying it
      Handle(AIS_InteractiveObject) anObj;
      if (GetMapOfAIS().Find2 (aName, anObj))
      {
        TheAISContext()->Remove (anObj, false);
        GetMapOfAIS().UnBind2 (aName);
      }
      GetMapOfAIS().Bind (aConnected, aName);
      return 0;
    }

    if (!anUpdateTool.parseRedrawMode (anArg))
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  ViewerTest::Display (aName, aConnected, false);
  return 0;
}

//=======================================================================
//function : VDisconnect
//purpose  :
//=======================================================================
static Standard_Integer VDisconnect (Draw_Interpretor& di,
                                     Standard_Integer argc,
                                     const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail( "Error: no active viewer");
    return 1;
  }
  
  if (argc != 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\nUsage: " << argv[0] << " name object";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  TCollection_AsciiString anObject (argv[2]);
  Standard_Integer anObjectNumber = Draw::Atoi (argv[2]);

  // find objects
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  if (!aMap.IsBound2 (aName) )
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aMap.Find2 (aName));
  if (anAssembly.IsNull())
  {
    di << "Not an assembly\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anIObj;
  if (!aMap.Find2 (anObject, anIObj))
  {
    // try to interpret second argument as child number
    if (anObjectNumber > 0 && anObjectNumber <= anAssembly->Children().Size())
    {
      Standard_Integer aCounter = 1;
      for (PrsMgr_ListOfPresentableObjectsIter anIter (anAssembly->Children()); anIter.More(); anIter.Next())
      {
        if (aCounter == anObjectNumber)
        {
          anIObj = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
          break;
        }
        ++aCounter;
      }
    }
    else
    {
      Message::SendFail ("Error: no active viewer");
      return 1;
    }    
  }

  aContext->Disconnect (anAssembly, anIObj);
  aContext->UpdateCurrentViewer();
  return 0;
}

//=======================================================================
//function : VAddConnected
//purpose  :
//=======================================================================
static Standard_Integer VAddConnected (Draw_Interpretor& ,
                                       Standard_Integer argc,
                                       const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (argc != 6)
  {
    Message::SendFail ("Syntax error: expect 5 arguments");
    return 1;
  }

  const TCollection_AsciiString aName (argv[1]);
  const Standard_Real aX = Draw::Atof (argv[2]);
  const Standard_Real aY = Draw::Atof (argv[3]);
  const Standard_Real aZ = Draw::Atof (argv[4]);
  const TCollection_AsciiString anObjectName (argv[5]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  {
    Handle(AIS_InteractiveObject) aPrs;
    aMap.Find2 (aName, aPrs);
    anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aPrs);
    if (anAssembly.IsNull())
    {
      Message::SendFail() << "Syntax error: '" << aName << "' is not an assembly";
      return 1;
    }
  }

  Handle(AIS_InteractiveObject) anIObj = findConnectedObject (anObjectName);
  if (anIObj.IsNull())
  {
    Message::SendFail() << "Syntax error: '" << anObjectName << "' is not displayed";
    return 1;
  }

  gp_Trsf aTrsf;
  aTrsf.SetTranslation (gp_Vec (aX, aY, aZ));
 
  anAssembly->Connect (anIObj, aTrsf);
  TheAISContext()->Display (anAssembly, Standard_False);
  TheAISContext()->RecomputeSelectionOnly (anAssembly);
  aContext->UpdateCurrentViewer();
  return 0;
}

//=======================================================================
//function : VListConnected
//purpose  :
//=======================================================================
static Standard_Integer VListConnected (Draw_Interpretor& /*di*/,
                                        Standard_Integer argc,
                                        const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  
  if (argc != 2)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\nUsage: " << argv[0] << " name";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_MultipleConnectedInteractive) anAssembly;
  if (!aMap.IsBound2 (aName) )
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  anAssembly = Handle(AIS_MultipleConnectedInteractive)::DownCast (aMap.Find2 (aName));
  if (anAssembly.IsNull())
  {
    Message::SendFail ("Syntax error: Not an assembly");
    return 1;
  }

  std::cout << "Children of " << aName << ":\n";

  Standard_Integer aCounter = 1;
  for (PrsMgr_ListOfPresentableObjectsIter anIter (anAssembly->Children()); anIter.More(); anIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anIter.Value());
    if (GetMapOfAIS().IsBound1 (anObj))
    {
      TCollection_AsciiString aCuurrentName = GetMapOfAIS().Find1 (anObj);
      std::cout << aCounter << ")  " << aCuurrentName << "    (" << anIter.Value()->DynamicType()->Name() << ")";
    }

    std::cout << aCounter << ")  " << anIter.Value()->DynamicType()->Name();

    Handle(AIS_ConnectedInteractive) aConnected = Handle(AIS_ConnectedInteractive)::DownCast (anIter.Value());
    if (!aConnected.IsNull() && !aConnected->ConnectedTo().IsNull() && aMap.IsBound1 (aConnected->ConnectedTo()))
    {
      std::cout << " connected to " << aMap.Find1 (aConnected->ConnectedTo());
    }
    std::cout << std::endl;
    
    ++aCounter;
  }

  return 0;
}

//=======================================================================
//function : VChild
//purpose  :
//=======================================================================
static Standard_Integer VChild (Draw_Interpretor& ,
                                Standard_Integer theNbArgs,
                                const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  int toAdd = -1;
  Handle(AIS_InteractiveObject) aParent;
  bool hasActions = false;
  int toInheritTrsf = -1;
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArg == "-add")
    {
      toAdd = 1;
      continue;
    }
    else if (anArg == "-remove")
    {
      toAdd = 0;
      continue;
    }
    else if (anArg == "-inheritparenttrsf"
      || anArg == "-inheritparentloc"
      || anArg == "-inheritparentlocation"
      || anArg == "-inheritparent"
      || anArg == "-noinheritparenttrsf"
      || anArg == "-noinheritparentloc"
      || anArg == "-noinheritparentlocation"
      || anArg == "-noinheritparent"
      || anArg == "-ignoreparenttrsf"
      || anArg == "-ignoreparentloc"
      || anArg == "-ignoreparentlocation"
      || anArg == "-ignoreparent")
    {
      bool aVal = true;
      if (anArgIter + 1 < theNbArgs
        && Draw::ParseOnOff(theArgVec[anArgIter + 1], aVal))
      {
        ++anArgIter;
      }
      if (anArg.StartsWith("-no")
        || anArg.StartsWith("-ignore"))
      {
        aVal = !aVal;
      }
      toInheritTrsf = aVal ? 1 : 0;
      continue;
    }

    Handle(AIS_InteractiveObject) aChild;
    if (!GetMapOfAIS().Find2 (theArgVec[anArgIter], aChild))
    {
      Message::SendFail() << "Syntax error: object '" << theArgVec[anArgIter] << "' is not found";
      return 1;
    }

    if (aParent.IsNull())
    {
      aParent = aChild;
    }
    else if (toAdd == -1)
    {
      Message::SendFail ("Syntax error: no action specified");
      return 1;
    }
    else
    {
      hasActions = true;
      if (toAdd == 1)
      {
        if(toInheritTrsf == 0)
          aParent->AddChildWithCurrentTransformation(aChild);
        else
          aParent->AddChild (aChild);
      }
      else
      {
        if (toInheritTrsf == 0)
          aParent->RemoveChildWithRestoreTransformation(aChild);
        else
          aParent->RemoveChild (aChild);
      }
    }
  }
  if (!hasActions)
  {
    Message::SendFail ("Syntax error: not enough arguments");
    return 1;
  }
  return 0;
}

//=======================================================================
//function : VParent
//purpose  :
//=======================================================================
static Standard_Integer VParent(Draw_Interpretor&,
  Standard_Integer theNbArgs,
  const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theNbArgs < 2 )
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  TCollection_AsciiString aName(theArgVec[1]);
  Handle(AIS_InteractiveObject) aParent;
  if (!GetMapOfAIS().Find2(theArgVec[1], aParent))
  {
    Message::SendFail() << "Syntax error: object '" << theArgVec[1] << "' is not found";
    return 1;
  }

  ViewerTest_AutoUpdater anUpdateTool(aContext, ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 2; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-ignorevisu")
      aParent->SetPropagateVisualState(Standard_False);
  }
  return 0;
}

//===============================================================================================
//function : VSetSelectionMode
//purpose  : vselmode
//===============================================================================================
static Standard_Integer VSetSelectionMode (Draw_Interpretor& /*di*/,
                                           Standard_Integer  theNbArgs,
                                           const char**      theArgv)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    Message::SendFail ("Error: no active Viewer");
    return 1;
  }

  NCollection_Sequence<TCollection_AsciiString> anObjNames;
  Standard_Integer aSelectionMode = -1;
  Standard_Boolean toTurnOn = Standard_True;
  AIS_SelectionModesConcurrency aSelModeConcurrency = AIS_SelectionModesConcurrency_GlobalOrLocal;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase (theArgv[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-set"
     || anArgCase == "-replace"
     || anArgCase == "-single"
     || anArgCase == "-exclusive")
    {
      aSelModeConcurrency = AIS_SelectionModesConcurrency_Single;
    }
    else if (anArgCase == "-add"
          || anArgCase == "-combine"
          || anArgCase == "-combination"
          || anArgCase == "-multiple")
    {
      aSelModeConcurrency = AIS_SelectionModesConcurrency_Multiple;
    }
    else if (anArgCase == "-globalorlocal"
          || anArgCase == "-localorglobal")
    {
      aSelModeConcurrency = AIS_SelectionModesConcurrency_GlobalOrLocal;
    }
    else
    {
      anObjNames.Append (theArgv[anArgIter]);
    }
  }
  if (anObjNames.Size() < 2
  || !Draw::ParseOnOff (anObjNames.Last().ToCString(), toTurnOn))
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }
  anObjNames.Remove (anObjNames.Upper());
  {
    const TCollection_AsciiString aSelModeString = anObjNames.Last();
    anObjNames.Remove (anObjNames.Upper());
    TopAbs_ShapeEnum aShapeType = TopAbs_SHAPE;
    if (aSelModeString.IsIntegerValue())
    {
      aSelectionMode = aSelModeString.IntegerValue();
    }
    else if (TopAbs::ShapeTypeFromString (aSelModeString.ToCString(), aShapeType))
    {
      aSelectionMode = AIS_Shape::SelectionMode (aShapeType);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown selection mode '" << aSelModeString  << "'";
      return 1;
    }
  }

  AIS_ListOfInteractive aTargetIOs;
  for (NCollection_Sequence<TCollection_AsciiString>::Iterator anObjIter (anObjNames); anObjIter.More(); anObjIter.Next())
  {
    const TCollection_AsciiString& aNameIO = anObjIter.Value();
    Handle(AIS_InteractiveObject) anIO;
    GetMapOfAIS().Find2 (aNameIO, anIO);
    if (anIO.IsNull())
    {
      Message::SendFail() << "Syntax error: undefined presentable object " << aNameIO;
      return 1;
    }
    aTargetIOs.Append (anIO);
  }
  if (aTargetIOs.IsEmpty())
  {
    anAISContext->DisplayedObjects (aTargetIOs);
  }

  for (AIS_ListIteratorOfListOfInteractive aTargetIt (aTargetIOs); aTargetIt.More(); aTargetIt.Next())
  {
    const Handle(AIS_InteractiveObject)& anIO = aTargetIt.Value();
    anAISContext->SetSelectionModeActive (anIO, aSelectionMode, toTurnOn, aSelModeConcurrency);
  }
  return 0;
}

//===============================================================================================
//function : VSelectionNext
//purpose  : 
//===============================================================================================
static Standard_Integer VSelectionNext(Draw_Interpretor& /*theDI*/,
                                 Standard_Integer /*theArgsNb*/,
                                 const char** /*theArgVec*/)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  if (anAISContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  anAISContext->HilightNextDetected (aView);
  return 0;
}

//===============================================================================================
//function : VSelectionPrevious
//purpose  : 
//===============================================================================================
static Standard_Integer VSelectionPrevious(Draw_Interpretor& /*theDI*/,
                                 Standard_Integer /*theArgsNb*/,
                                 const char** /*theArgVec*/)
{
  // Check errors
  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  if (anAISContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  anAISContext->HilightPreviousDetected (aView);
  return 0;
}

//===========================================================================
//function : VTriangle 
//Draw arg : vtriangle Name PointName PointName PointName
//purpose  : creates and displays Triangle
//===========================================================================
static Standard_Integer VTriangle (Draw_Interpretor& /*di*/,
                                   Standard_Integer argc,
                                   const char ** argv)
{
  const Standard_Boolean isTri = TCollection_AsciiString (argv[0]) == "vtriangle";
  Handle(Graphic3d_ArrayOfPrimitives) aPrims;
  if (isTri)
  {
    aPrims = new Graphic3d_ArrayOfTriangles (3);
  }
  else
  {
    aPrims = new Graphic3d_ArrayOfSegments (2);
  }

  if (argc != (2 + aPrims->VertexNumberAllocated()))
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  gp_Pnt aPnts[3];
  for (Standard_Integer aPntIter = 0; aPntIter < aPrims->VertexNumberAllocated(); ++aPntIter)
  {
    const TCollection_AsciiString aName (argv[2 + aPntIter]);
    if (Handle(AIS_Point) aPntPrs = Handle(AIS_Point)::DownCast (GetMapOfAIS().IsBound2 (aName) ? GetMapOfAIS().Find2 (aName) : NULL))
    {
      aPnts[aPntIter] = aPntPrs->Component()->Pnt();
    }
    else
    {
      TopoDS_Shape aShape = DBRep::Get (argv[2 + aPntIter]);
      if (aShape.IsNull()
       || aShape.ShapeType() != TopAbs_VERTEX)
      {
        Message::SendFail() << "Syntax error: argument " << aName << " must be a point";
        return 1;
      }
      aPnts[aPntIter] = BRep_Tool::Pnt (TopoDS::Vertex (aShape));
    }

    for (Standard_Integer aPnt2Iter = 0; aPnt2Iter < aPntIter; ++aPnt2Iter)
    {
      if (aPnts[aPnt2Iter].IsEqual (aPnts[aPntIter], Precision::Confusion()))
      {
        Message::SendFail ("Syntax error: points should not be equal");
        return 1;
      }
    }

    aPrims->AddVertex (aPnts[aPntIter]);
  }

  Handle(AIS_InteractiveObject) aPrs = new MyPArrayObject (aPrims);
  if (!isTri)
  {
    aPrs->Attributes()->SetupOwnShadingAspect();
    aPrs->Attributes()->ShadingAspect()->Aspect()->SetColor (Quantity_NOC_YELLOW);
  }
  ViewerTest::Display (argv[1], aPrs);
  return 0;
}

//===========================================================================
//function : VTorus
//purpose  : creates and displays a torus or torus segment
//===========================================================================
static Standard_Integer VTorus (Draw_Interpretor& /*di*/,
                                Standard_Integer theNbArgs,
                                const char** theArgVec)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aName;
  Standard_Integer aNbSlices = 100, aNbStacks = 100;
  Standard_Real aMajorRad = -1.0, aMinorRad = -1.0;
  Standard_Real aPipeAngle = 360.0, aSegAngle1 = 0.0, aSegAngle2 = 360.0;
  Standard_Real anAngles[3] = { -1.0, -1.0, -1.0 };
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbslices"
           || anArg == "-slices")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbSlices))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbstacks"
           || anArg == "-stacks")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbStacks))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-radius"
           || anArg == "-majorradius")
          && Draw::ParseReal (theArgVec[anArgIter + 1], aMajorRad))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-piperadius"
           || anArg == "-minoradius")
          && Draw::ParseReal (theArgVec[anArgIter + 1], aMinorRad))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-pipeangle"
           || anArg == "-angle")
          && Draw::ParseReal (theArgVec[anArgIter + 1], aPipeAngle))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-segmentanglefrom"
           || anArg == "-seganglefrom"
           || anArg == "-segmentangle1"
           || anArg == "-segangle1")
          && Draw::ParseReal (theArgVec[anArgIter + 1], aSegAngle1))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-segmentangleto"
           || anArg == "-segangleto"
           || anArg == "-segmentangle2"
           || anArg == "-segangle2")
          && Draw::ParseReal (theArgVec[anArgIter + 1], aSegAngle2))
    {
      ++anArgIter;
    }
    else if (aName.IsEmpty())
    {
      aName = theArgVec[anArgIter];
    }
    else if (aMajorRad < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aMajorRad))
    {
      //
    }
    else if (aMinorRad < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aMinorRad))
    {
      //
    }
    else if (anAngles[0] < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], anAngles[0]))
    {
      //
    }
    else if (anAngles[1] < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], anAngles[1]))
    {
      //
    }
    else if (anAngles[2] < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], anAngles[2]))
    {
      //
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (anAngles[2] > 0.0)
  {
    aSegAngle1 = anAngles[0];
    aSegAngle2 = anAngles[1];
    aPipeAngle = anAngles[2];
  }
  else if (anAngles[1] > 0.0)
  {
    aSegAngle1 = anAngles[0];
    aSegAngle2 = anAngles[1];
  }
  else if (anAngles[0] > 0.0)
  {
    aPipeAngle = anAngles[0];
  }

  aSegAngle1 = aSegAngle1 * (M_PI / 180.0);
  aSegAngle2 = aSegAngle2 * (M_PI / 180.0);
  aPipeAngle = aPipeAngle * (M_PI / 180.0);

  if (aMajorRad <= 0 || aMinorRad <= 0 || aNbSlices <= 0 || aNbStacks <= 0
   || Abs(aSegAngle2 - aSegAngle1) <= Precision::Angular()
   || Abs(aPipeAngle) <= Precision::Angular())
  {
    Message::SendFail ("Syntax error: wrong parameters");
    return 1;
  }

  Handle(Graphic3d_ArrayOfTriangles) aTriangles = Prs3d_ToolTorus::Create (aMajorRad, aMinorRad, aSegAngle1, aSegAngle2, aPipeAngle, aNbSlices, aNbStacks, gp_Trsf());
  Handle(AIS_InteractiveObject) anIO = new MyPArrayObject (aTriangles);
  ViewerTest::Display (aName, anIO, false);
  return 0;
}

//===========================================================================
//function : VCylinder
//purpose  : creates and displays a cylinder
//===========================================================================
static Standard_Integer VCylinder (Draw_Interpretor& /*di*/,
                                   Standard_Integer theNbArgs,
                                   const char** theArgVec)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aName;
  Standard_Integer aNbSlices = 100, aNbStacks = 1;
  Standard_Real aBotRad = -1.0, aTopRad = -1.0, aHeight = -1.0;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbslices"
           || anArg == "-slices")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbSlices))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbstacks"
           || anArg == "-stacks")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbStacks))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-radius"
          && Draw::ParseReal (theArgVec[anArgIter + 1], aBotRad))
    {
      aTopRad = aBotRad;
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-bottomradius"
          && Draw::ParseReal (theArgVec[anArgIter + 1], aBotRad))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-topradius"
          && Draw::ParseReal (theArgVec[anArgIter + 1], aTopRad))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-height"
          && Draw::ParseReal (theArgVec[anArgIter + 1], aHeight))
    {
      ++anArgIter;
    }
    else if (aName.IsEmpty())
    {
      aName = theArgVec[anArgIter];
    }
    else if (aBotRad < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aBotRad))
    {
      //
    }
    else if (aTopRad < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aTopRad))
    {
      //
    }
    else if (aHeight < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aHeight))
    {
      //
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aBotRad < 0 || aTopRad < 0 || aHeight <= 0 || aNbSlices < 3)
  {
    Message::SendFail ("Syntax error: wrong parameters");
    return 1;
  }

  Handle(Graphic3d_ArrayOfTriangles) aTriangles = Prs3d_ToolCylinder::Create (aBotRad, aTopRad, aHeight, aNbSlices, aNbStacks, gp_Trsf());
  Handle(AIS_InteractiveObject) anIO = new MyPArrayObject (aTriangles);
  ViewerTest::Display (aName, anIO, false);
  return 0;
}

//===========================================================================
//function : VSphere
//purpose  : creates and displays a sphere
//===========================================================================
static Standard_Integer VSphere (Draw_Interpretor& /*di*/,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aName;
  Standard_Integer aNbSlices = 100, aNbStacks = 100;
  Standard_Real aRad = -1.0;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbslices"
           || anArg == "-slices")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbSlices))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && (anArg == "-nbstacks"
           || anArg == "-stacks")
          && Draw::ParseInteger (theArgVec[anArgIter + 1], aNbStacks))
    {
      ++anArgIter;
    }
    else if (anArgIter + 1 < theNbArgs
          && anArg == "-radius"
          && Draw::ParseReal (theArgVec[anArgIter + 1], aRad))
    {
      ++anArgIter;
    }
    else if (aName.IsEmpty())
    {
      aName = theArgVec[anArgIter];
    }
    else if (aRad < 0.0
          && Draw::ParseReal (theArgVec[anArgIter], aRad))
    {
      //
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aRad <= 0 || aNbSlices <= 0 || aNbStacks <= 0)
  {
    Message::SendFail ("Syntax error: wrong parameters");
    return 1;
  }

  Handle(Graphic3d_ArrayOfTriangles) aTriangles = Prs3d_ToolSphere::Create (aRad, aNbSlices, aNbStacks, gp_Trsf());
  Handle(AIS_InteractiveObject) anIO = new MyPArrayObject (aTriangles);
  ViewerTest::Display (aName, anIO, false);
  return 0;
}

//=======================================================================
//function : VObjZLayer
//purpose  : Set or get z layer id for presentable object
//=======================================================================

static Standard_Integer VObjZLayer (Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  // get operation
  TCollection_AsciiString aOperation;
  if (argc >= 2)
    aOperation = TCollection_AsciiString (argv [1]);

  // check for correct arguments
  if (!(argc == 4 && aOperation.IsEqual ("set")) &&
      !(argc == 3 && aOperation.IsEqual ("get")))
  {
    di << "Usage : " << argv[0] << " set/get object [layerid]\n";
    di << " set - set layer id for interactive object, layerid - z layer id\n";
    di << " get - get layer id of interactive object\n";
    di << " argument layerid should be passed for set operation only\n";
    return 1;
  }

  // find object
  TCollection_AsciiString aName (argv[2]);
  Handle(AIS_InteractiveObject) anInterObj;
  GetMapOfAIS().Find2 (aName, anInterObj);
  if (anInterObj.IsNull())
  {
    Message::SendFail() << "Syntax error: object '" << aName << "' is not displayed";
    return 1;
  }

  // process operation
  if (aOperation.IsEqual ("set"))
  {
    Standard_Integer aLayerId = Draw::Atoi (argv [3]);
    aContext->SetZLayer (anInterObj, aLayerId);
  }
  else if (aOperation.IsEqual ("get"))
  {
    di << "Z layer id: " << aContext->GetZLayer (anInterObj);
  }
  
  return 0;
}

//=======================================================================
//function : VPolygonOffset
//purpose  : Set or get polygon offset parameters
//=======================================================================
static Standard_Integer VPolygonOffset(Draw_Interpretor& /*di*/,
                                       Standard_Integer argc,
                                       const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (argc > 2 && argc != 5)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\n"
                           "Usage: " << argv[0] << " [object [mode factor units]] - sets/gets polygon offset parameters for an object,"
                           "without arguments prints the default values";
    return 1;
  }

  // find object
  Handle(AIS_InteractiveObject) anInterObj;
  if (argc >= 2)
  {
    TCollection_AsciiString aName (argv[1]);
    if (!GetMapOfAIS().Find2 (aName, anInterObj)
      || anInterObj.IsNull())
    {
      Message::SendFail() << "Syntax error: object '" << aName << "' is not displayed";
      return 1;
    }
  }

  Standard_Integer aMode;
  Standard_ShortReal    aFactor, aUnits;
  if (argc == 5)
  {
    aMode   = Draw::Atoi(argv[2]);
    aFactor = (Standard_ShortReal) Draw::Atof(argv[3]);
    aUnits  = (Standard_ShortReal) Draw::Atof(argv[4]);

    anInterObj->SetPolygonOffsets(aMode, aFactor, aUnits);
    aContext->UpdateCurrentViewer();
    return 0;
  }
  else if (argc == 2)
  {
    if (anInterObj->HasPolygonOffsets())
    {
      anInterObj->PolygonOffsets(aMode, aFactor, aUnits);
      std::cout << "Current polygon offset parameters for " << argv[1] << ":" << std::endl;
      std::cout << "\tMode: "   << aMode   << std::endl;
      std::cout << "\tFactor: " << aFactor << std::endl;
      std::cout << "\tUnits: "  << aUnits  << std::endl;
      return 0;
    }
    else
    {
      std::cout << "Specific polygon offset parameters are not set for " << argv[1] << std::endl;
    }
  }

  std::cout << "Default polygon offset parameters:" << std::endl;
  aContext->DefaultDrawer()->ShadingAspect()->Aspect()->PolygonOffsets(aMode, aFactor, aUnits);
  std::cout << "\tMode: "   << aMode   << std::endl;
  std::cout << "\tFactor: " << aFactor << std::endl;
  std::cout << "\tUnits: "  << aUnits  << std::endl;

  return 0;
}

// This class is used for testing markers.
class ViewerTest_MarkersArrayObject : public AIS_InteractiveObject
{

public:

  ViewerTest_MarkersArrayObject (const gp_XYZ& theStartPoint,
                                 const Standard_Integer& thePointsOnSide,
                                 Handle(Graphic3d_AspectMarker3d) theMarkerAspect = NULL)
  {
    myStartPoint = theStartPoint;
    myPointsOnSide = thePointsOnSide;
    myMarkerAspect = theMarkerAspect;
  }

  DEFINE_STANDARD_RTTI_INLINE(ViewerTest_MarkersArrayObject,AIS_InteractiveObject);

private:

  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE;

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                 const Standard_Integer theMode) Standard_OVERRIDE;

protected:

  gp_XYZ myStartPoint;
  Standard_Integer myPointsOnSide;
  Handle(Graphic3d_AspectMarker3d) myMarkerAspect;
};


void ViewerTest_MarkersArrayObject::Compute (const Handle(PrsMgr_PresentationManager)& ,
                                             const Handle(Prs3d_Presentation)& aPresentation,
                                             const Standard_Integer )
{
  Handle(Graphic3d_ArrayOfPrimitives) anArray = new Graphic3d_ArrayOfPoints ((Standard_Integer )Pow (myPointsOnSide, 3), myPointsOnSide != 1);
  if (myPointsOnSide == 1)
  {
    anArray->AddVertex (myStartPoint);
  }
  else
  {
    for (Standard_Real i = 1; i <= myPointsOnSide; i++)
    {
      for (Standard_Real j = 1; j <= myPointsOnSide; j++)
      {
        for (Standard_Real k = 1; k <= myPointsOnSide; k++)
        {
          anArray->AddVertex (myStartPoint.X() + i, myStartPoint.Y() + j, myStartPoint.Z() + k);
          anArray->SetVertexColor (anArray->VertexNumber(),
                                   i / myPointsOnSide,
                                   j / myPointsOnSide,
                                   k / myPointsOnSide);
        }
      }
    }
  }

  aPresentation->Clear();
  if (!myMarkerAspect.IsNull())
  {
    aPresentation->CurrentGroup()->SetGroupPrimitivesAspect (myMarkerAspect);
  }
  aPresentation->CurrentGroup()->AddPrimitiveArray (anArray);
}

void ViewerTest_MarkersArrayObject::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                       const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner (this);

  if (myPointsOnSide == 1)
  {
    gp_Pnt aPoint (myStartPoint);
    Handle(Select3D_SensitivePoint) aSensetivePoint = new Select3D_SensitivePoint (anEntityOwner, aPoint);
    theSelection->Add (aSensetivePoint);
  }
  else
  {
    for (Standard_Real i = 1; i <= myPointsOnSide; i++)
    {
      for (Standard_Real j = 1; j <= myPointsOnSide; j++)
      {
        for (Standard_Real k = 1; k <= myPointsOnSide; k++)
        {
          gp_Pnt aPoint (myStartPoint.X() + i, myStartPoint.Y() + j, myStartPoint.Z() + k);
          Handle(Select3D_SensitivePoint) aSensetivePoint = new Select3D_SensitivePoint (anEntityOwner, aPoint);
          theSelection->Add (aSensetivePoint);
        }
      }
    }
  }
}
//=======================================================================
//function : VMarkersTest
//purpose  : Draws an array of markers for testing purposes.
//=======================================================================
static Standard_Integer VMarkersTest (Draw_Interpretor&,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  if (theArgNb < 5)
  {
    Message::SendFail ("Syntax error: wrong number of arguments");
    return 1;
  }

  Standard_Integer anArgIter = 1;

  TCollection_AsciiString aName (theArgVec[anArgIter++]);
  TCollection_AsciiString aFileName;
  gp_XYZ aPnt (Atof (theArgVec[anArgIter]),
               Atof (theArgVec[anArgIter + 1]),
               Atof (theArgVec[anArgIter + 2]));
  anArgIter += 3;

  Standard_Integer aPointsOnSide = 10;
  Standard_Integer aMarkerType   = -1;
  Standard_Real    aScale        = 1.0;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);
    if (anArg.Search ("PointsOnSide=") > -1)
    {
      aPointsOnSide = anArg.Token ("=", 2).IntegerValue();
    }
    else if (anArg.Search ("MarkerType=") > -1)
    {
      aMarkerType = anArg.Token ("=", 2).IntegerValue();
    }
    else if (anArg.Search ("Scale=") > -1)
    {
      aScale = anArg.Token ("=", 2).RealValue();
    }
    else if (anArg.Search ("FileName=") > -1)
    {
      aFileName = anArg.Token ("=", 2);
    }
    else
    {
      Message::SendFail() << "Syntax error: wrong argument '" << anArg << "'";
      return 1;
    }
  }

  Handle(Graphic3d_AspectMarker3d) anAspect;
  Handle(Image_AlienPixMap) anImage;
  Quantity_Color aColor (Quantity_NOC_GREEN1);
  if ((aMarkerType == Aspect_TOM_USERDEFINED || aMarkerType < 0)
   && !aFileName.IsEmpty())
  {
    anImage = new Image_AlienPixMap();
    if (!anImage->Load (aFileName))
    {
      Message::SendFail() << "Error: could not load image from file '" << aFileName << "'!";
      return 1;
    }
    if (anImage->Format() == Image_Format_Gray)
    {
      anImage->SetFormat (Image_Format_Alpha);
    }
    else if (anImage->Format() == Image_Format_GrayF)
    {
      anImage->SetFormat (Image_Format_AlphaF);
    }
    anAspect = new Graphic3d_AspectMarker3d (anImage);
  }
  else
  {
    anAspect = new Graphic3d_AspectMarker3d (aMarkerType >= 0 ? (Aspect_TypeOfMarker )aMarkerType : Aspect_TOM_POINT, aColor, aScale);
  }

  Handle(ViewerTest_MarkersArrayObject) aMarkersArray = new ViewerTest_MarkersArrayObject (aPnt, aPointsOnSide, anAspect);
  VDisplayAISObject (aName, aMarkersArray);

  return 0;
}

//=======================================================================
//function : TextToBrep
//purpose  : Tool for conversion text to occt-shapes
//=======================================================================
static int TextToBRep (Draw_Interpretor& /*theDI*/,
                       Standard_Integer  theArgNb,
                       const char**      theArgVec)
{
  // Check arguments
  if (theArgNb < 3)
  {
    Message::SendFail() << "Error: " << theArgVec[0] << " - invalid syntax";
    return 1;
  }

  Standard_Integer anArgIt = 1;
  Standard_CString aName   = theArgVec[anArgIt++];
  Standard_CString aText   = theArgVec[anArgIt++];

  Font_BRepFont           aFont;
  TCollection_AsciiString aFontName ("Courier");
  Standard_Real           aTextHeight        = 16.0;
  Font_FontAspect         aFontAspect        = Font_FA_Regular;
  Standard_Boolean        anIsCompositeCurve = Standard_False;
  gp_Ax3                  aPenAx3    (gp::XOY());
  gp_Dir                  aNormal    (0.0, 0.0, 1.0);
  gp_Dir                  aDirection (1.0, 0.0, 0.0);
  gp_Pnt                  aPenLoc;

  Graphic3d_HorizontalTextAlignment aHJustification = Graphic3d_HTA_LEFT;
  Graphic3d_VerticalTextAlignment   aVJustification = Graphic3d_VTA_BOTTOM;
  Font_StrictLevel aStrictLevel = Font_StrictLevel_Any;
  for (; anArgIt < theArgNb; ++anArgIt)
  {
    TCollection_AsciiString aParam (theArgVec[anArgIt]);
    aParam.LowerCase();

    if (aParam == "-pos"
     || aParam == "-position")
    {
      if (anArgIt + 3 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      aPenLoc.SetX (Draw::Atof(theArgVec[++anArgIt]));
      aPenLoc.SetY (Draw::Atof(theArgVec[++anArgIt]));
      aPenLoc.SetZ (Draw::Atof(theArgVec[++anArgIt]));
    }
    else if (aParam == "-halign")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "left")
      {
        aHJustification = Graphic3d_HTA_LEFT;
      }
      else if (aType == "center")
      {
        aHJustification = Graphic3d_HTA_CENTER;
      }
      else if (aType == "right")
      {
        aHJustification = Graphic3d_HTA_RIGHT;
      }
      else
      {
        Message::SendFail() << "Error: wrong syntax at '" << aParam << "'";
        return 1;
      }
    }
    else if (aParam == "-valign")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      TCollection_AsciiString aType (theArgVec[anArgIt]);
      aType.LowerCase();
      if (aType == "top")
      {
        aVJustification = Graphic3d_VTA_TOP;
      }
      else if (aType == "center")
      {
        aVJustification = Graphic3d_VTA_CENTER;
      }
      else if (aType == "bottom")
      {
        aVJustification = Graphic3d_VTA_BOTTOM;
      }
      else if (aType == "topfirstline")
      {
        aVJustification = Graphic3d_VTA_TOPFIRSTLINE;
      }
      else
      {
        Message::SendFail() << "Error: wrong syntax at '" << aParam << "'";
        return 1;
      }
    }
    else if (aParam == "-height")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      aTextHeight = Draw::Atof(theArgVec[anArgIt]);
    }
    else if (aParam == "-aspect")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      TCollection_AsciiString anOption (theArgVec[anArgIt]);
      anOption.LowerCase();
      if (!parseFontStyle (anOption, aFontAspect))
      {
        Message::SendFail() << "Error: unknown font aspect '" << anOption << "'";
        return 1;
      }
    }
    else if (aParam == "-font")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      aFontName = theArgVec[anArgIt];
    }
    else if (aParam == "-strict")
    {
      anArgIt += parseFontStrictLevel (theArgNb  - anArgIt - 1,
                                       theArgVec + anArgIt + 1,
                                       aStrictLevel);
    }
    else if (aParam == "-composite")
    {
      if (++anArgIt >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      Draw::ParseOnOff (theArgVec[anArgIt], anIsCompositeCurve);
    }
    else if (aParam == "-plane")
    {
      if (anArgIt + 6 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong number of values for parameter '" << aParam << "'";
        return 1;
      }

      Standard_Real aX = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aY = Draw::Atof (theArgVec[++anArgIt]);
      Standard_Real aZ = Draw::Atof (theArgVec[++anArgIt]);
      aNormal.SetCoord (aX, aY, aZ);

      aX = Draw::Atof (theArgVec[++anArgIt]);
      aY = Draw::Atof (theArgVec[++anArgIt]);
      aZ = Draw::Atof (theArgVec[++anArgIt]);
      aDirection.SetCoord (aX, aY, aZ);
    }
    else
    {
      Message::SendFail() << "Warning! Unknown argument '" << aParam << "'";
    }
  }

  aFont.SetCompositeCurveMode (anIsCompositeCurve);
  if (!aFont.FindAndInit (aFontName.ToCString(), aFontAspect, aTextHeight, aStrictLevel))
  {
    Message::SendFail ("Error: unable to load Font");
    return 1;
  }

  aPenAx3 = gp_Ax3 (aPenLoc, aNormal, aDirection);

  Font_BRepTextBuilder aBuilder;
  DBRep::Set (aName, aBuilder.Perform (aFont, aText, aPenAx3, aHJustification, aVJustification));
  return 0;
}

//=======================================================================
//function : VFont
//purpose  : Font management
//=======================================================================
struct FontComparator
{
  bool operator() (const Handle(Font_SystemFont)& theFontA,
                   const Handle(Font_SystemFont)& theFontB)
  {
    return theFontA->FontKey().IsLess (theFontB->FontKey());
  }
};

static int VFont (Draw_Interpretor& theDI,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  Handle(Font_FontMgr) aMgr = Font_FontMgr::GetInstance();
  bool toPrintList = theArgNb < 2, toPrintNames = false;
  Font_StrictLevel aStrictLevel = Font_StrictLevel_Any;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-strict")
    {
      anArgIter += parseFontStrictLevel (theArgNb  - anArgIter - 1,
                                         theArgVec + anArgIter + 1,
                                         aStrictLevel);
    }
    else if (anArgCase == "-clear")
    {
      aMgr->ClearFontDataBase();
    }
    else if (anArgCase == "-init")
    {
      aMgr->InitFontDataBase();
    }
    else if (anArgCase == "-list")
    {
      toPrintList = true;
    }
    else if (anArgCase == "-names")
    {
      toPrintList = true;
      toPrintNames = true;
    }
    else if (anArgIter + 1 < theArgNb
          && (anArgCase == "-find"
           || anArgCase == "-findinfo"
           || anArgCase == "-findall"
           || anArgCase == "find"))
    {
      const TCollection_AsciiString aFontName (theArgVec[++anArgIter]);
      Font_FontAspect aFontAspect = Font_FA_Undefined;
      if (++anArgIter < theArgNb)
      {
        anArgCase = theArgVec[anArgIter];
        anArgCase.LowerCase();
        if (!parseFontStyle (anArgCase, aFontAspect))
        {
          --anArgIter;
        }
      }

      const bool toFindAll   = (anArgCase == "-findall");
      const bool toPrintInfo = (anArgCase == "-findinfo");
      TCollection_AsciiString aResult;
      if (toFindAll
       || aFontName.Search ("*") != -1)
      {
        const Font_NListOfSystemFont aFonts = aMgr->GetAvailableFonts();
        std::vector<Handle(Font_SystemFont)> aFontsSorted;
        aFontsSorted.reserve (aFonts.Size());
        for (Font_NListOfSystemFont::Iterator aFontIter (aFonts); aFontIter.More(); aFontIter.Next())
        {
          aFontsSorted.push_back (aFontIter.Value());
        }
        std::stable_sort (aFontsSorted.begin(), aFontsSorted.end(), FontComparator());
        for (std::vector<Handle(Font_SystemFont)>::iterator aFontIter = aFontsSorted.begin(); aFontIter != aFontsSorted.end(); ++aFontIter)
        {
          const Handle(Font_SystemFont)& aFont = *aFontIter;
          const TCollection_AsciiString aCheck = TCollection_AsciiString ("string match -nocase \"") + aFontName + "\" \"" + aFont->FontName() + "\"";
          if (theDI.Eval (aCheck.ToCString()) == 0
          && *theDI.Result() != '1')
          {
            theDI.Reset();
            continue;
          }

          theDI.Reset();
          if (!aResult.IsEmpty())
          {
            aResult += "\n";
          }

          aResult += toPrintInfo ? aFont->ToString() : aFont->FontName();
          if (!toFindAll)
          {
            break;
          }
        }
      }
      else if (Handle(Font_SystemFont) aFont = aMgr->FindFont (aFontName, aStrictLevel, aFontAspect))
      {
        aResult = toPrintInfo ? aFont->ToString() : aFont->FontName();
      }

      if (!aResult.IsEmpty())
      {
        theDI << aResult;
      }
      else
      {
        Message::SendFail() << "Error: font '" << aFontName << "' is not found";
      }
    }
    else if (anArgIter + 1 < theArgNb
          && (anArgCase == "-add"
           || anArgCase == "add"
           || anArgCase == "-register"
           || anArgCase == "register"))
    {
      ++anArgIter;
      Standard_CString aFontPath = theArgVec[anArgIter++];
      TCollection_AsciiString aFontName;
      Font_FontAspect  aFontAspect = Font_FA_Undefined;
      Standard_Integer isSingelStroke = -1;
      for (; anArgIter < theArgNb; ++anArgIter)
      {
        anArgCase = theArgVec[anArgIter];
        anArgCase.LowerCase();
        if (aFontAspect == Font_FontAspect_UNDEFINED
         && parseFontStyle (anArgCase, aFontAspect))
        {
          continue;
        }
        else if (anArgCase == "singlestroke"
              || anArgCase == "singleline"
              || anArgCase == "oneline")
        {
          isSingelStroke = 1;
        }
        else if (aFontName.IsEmpty())
        {
          aFontName = theArgVec[anArgIter];
        }
        else
        {
          --anArgIter;
          break;
        }
      }

      Handle(Font_SystemFont) aFont = aMgr->CheckFont (aFontPath);
      if (aFont.IsNull())
      {
        Message::SendFail() << "Error: font '" << aFontPath << "' is not found!";
        continue;
      }

      if (aFontAspect != Font_FontAspect_UNDEFINED
      || !aFontName.IsEmpty())
      {
        TCollection_AsciiString aName = aFont->FontName();
        if (!aFontName.IsEmpty())
        {
          aName = aFontName;
        }
        Handle(Font_SystemFont) aFont2 = new Font_SystemFont (aName);
        if (aFontAspect != Font_FontAspect_UNDEFINED)
        {
          aFont2->SetFontPath (aFontAspect, aFontPath, 0);
        }
        else
        {
          for (int anAspectIter = 0; anAspectIter < Font_FontAspect_NB; ++anAspectIter)
          {
            aFont2->SetFontPath ((Font_FontAspect )anAspectIter,
                                 aFont->FontPath ((Font_FontAspect )anAspectIter),
                                 aFont->FontFaceId ((Font_FontAspect )anAspectIter));
          }
        }
        aFont = aFont2;
      }
      if (isSingelStroke != -1)
      {
        aFont->SetSingleStrokeFont (isSingelStroke == 1);
      }

      aMgr->RegisterFont (aFont, Standard_True);
      theDI << aFont->ToString();
    }
    else if (anArgCase == "-aliases")
    {
      TCollection_AsciiString anAliasName;
      TColStd_SequenceOfHAsciiString aNames;
      if (anArgIter + 1 < theArgNb
      && *theArgVec[anArgIter + 1] != '-')
      {
        anAliasName = theArgVec[++anArgIter];
      }
      if (!anAliasName.IsEmpty())
      {
        aMgr->GetFontAliases (aNames, anAliasName);
      }
      else
      {
        aMgr->GetAllAliases (aNames);
      }
      for (TColStd_SequenceOfHAsciiString::Iterator aNameIter (aNames); aNameIter.More(); aNameIter.Next())
      {
        theDI << "{" << aNameIter.Value()->String() << "} ";
      }
    }
    else if (anArgIter + 2 < theArgNb
          && anArgCase == "-addalias")
    {
      TCollection_AsciiString anAliasName(theArgVec[++anArgIter]);
      TCollection_AsciiString aFontName  (theArgVec[++anArgIter]);
      aMgr->AddFontAlias (anAliasName, aFontName);
    }
    else if (anArgIter + 2 < theArgNb
          && anArgCase == "-removealias")
    {
      TCollection_AsciiString anAliasName(theArgVec[++anArgIter]);
      TCollection_AsciiString aFontName  (theArgVec[++anArgIter]);
      aMgr->RemoveFontAlias (anAliasName, aFontName);
    }
    else if (anArgIter + 1 < theArgNb
          && anArgCase == "-clearalias")
    {
      TCollection_AsciiString anAliasName(theArgVec[++anArgIter]);
      aMgr->RemoveFontAlias (anAliasName, "");
    }
    else if (anArgCase == "-clearaliases")
    {
      aMgr->RemoveFontAlias ("", "");
    }
    else if (anArgCase == "-verbose"
          || anArgCase == "-trace")
    {
      bool toEnable = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aMgr->SetTraceAliases (toEnable);
    }
    else if (anArgCase == "-unicodefallback"
          || anArgCase == "-fallback"
          || anArgCase == "-touseunicodesubsetfallback")
    {
      bool toEnable = true;
      if (anArgIter + 1 < theArgNb
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      Font_FontMgr::ToUseUnicodeSubsetFallback() = toEnable;
    }
    else
    {
      Message::SendFail() << "Warning! Unknown argument '" << anArg << "'";
    }
  }

  if (toPrintList)
  {
    // just print the list of available fonts
    Standard_Boolean isFirst = Standard_True;
    const Font_NListOfSystemFont aFonts = aMgr->GetAvailableFonts();
    std::vector<Handle(Font_SystemFont)> aFontsSorted;
    aFontsSorted.reserve (aFonts.Size());
    for (Font_NListOfSystemFont::Iterator aFontIter (aFonts); aFontIter.More(); aFontIter.Next())
    {
      aFontsSorted.push_back (aFontIter.Value());
    }
    std::stable_sort (aFontsSorted.begin(), aFontsSorted.end(), FontComparator());
    for (std::vector<Handle(Font_SystemFont)>::iterator aFontIter = aFontsSorted.begin(); aFontIter != aFontsSorted.end(); ++aFontIter)
    {
      const Handle(Font_SystemFont)& aFont = *aFontIter;

      if (toPrintNames)
      {
        if (!isFirst)
        {
          theDI << "\n";
        }
        theDI << "\"" << aFont->FontName() << "\"";
      }
      else
      {
        if (!isFirst)
        {
          theDI << "\n";
        }
        theDI << aFont->ToString();
      }
      isFirst = Standard_False;
    }
    return 0;
  }

  return 0;
}

//=======================================================================
//function : VVertexMode
//purpose  : Switches vertex display mode for AIS_Shape or displays the current value
//=======================================================================

static int VVertexMode (Draw_Interpretor& theDI,
                         Standard_Integer  theArgNum,
                         const char**      theArgs)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  // No arguments --> print the current default vertex draw mode
  if (theArgNum == 1)
  {
    Prs3d_VertexDrawMode aCurrMode = aContext->DefaultDrawer()->VertexDrawMode();
    theDI <<  "Default vertex draw mode: " << (aCurrMode == Prs3d_VDM_Isolated ? "'isolated'" : "'all'") << "\n";
    return 0;
  }

  // -set argument --> change the default vertex draw mode and the mode for all displayed or given object(s)
  TCollection_AsciiString aParam (theArgs[1]);
  if (aParam == "-set")
  {
    if (theArgNum == 2)
    {
      Message::SendFail ("Error: '-set' option not followed by the mode and optional object name(s)\n"
                         "Type 'help vvertexmode' for usage hints");
      return 1;
    }

    TCollection_AsciiString aModeStr (theArgs[2]);
    Prs3d_VertexDrawMode aNewMode =
       aModeStr == "isolated" ? Prs3d_VDM_Isolated :
      (aModeStr == "all"      ? Prs3d_VDM_All :
                                Prs3d_VDM_Inherited);

    Standard_Boolean aRedrawNeeded = Standard_False;
    AIS_ListOfInteractive anObjs;

    // No object(s) specified -> use all displayed
    if (theArgNum == 3)
    {
      theDI << "Setting the default vertex draw mode and updating all displayed objects...\n";
      aContext->DisplayedObjects (anObjs);
      aContext->DefaultDrawer()->SetVertexDrawMode (aNewMode);
      aRedrawNeeded = Standard_True;
    }

    Handle(AIS_InteractiveObject) anObject;
    for (Standard_Integer aCount = 3; aCount < theArgNum; aCount++)
    {
      TCollection_AsciiString aName (theArgs[aCount]);
      if (!GetMapOfAIS().Find2 (aName, anObject))
      {
        theDI << "Warning: wrong object name ignored - " << theArgs[0] << "\n";
        continue;
      }
      anObjs.Append (anObject);
    }

    for (AIS_ListIteratorOfListOfInteractive anIt (anObjs); anIt.More(); anIt.Next())
    {
      anObject = anIt.Value();
      anObject->Attributes()->SetVertexDrawMode (aNewMode);
      aContext->Redisplay (anObject, Standard_False);
      aRedrawNeeded = Standard_True;
    }

    if (aRedrawNeeded)
      ViewerTest::CurrentView()->Redraw();

    return 0;
  }

  Handle(AIS_InteractiveObject) anObject;
  if (theArgNum > 2
  || !GetMapOfAIS().Find2 (aParam, anObject))
  {
    Message::SendFail ("Syntax error: invalid number of arguments");
    return 1;
  }

  // One argument (object name) --> print the current vertex draw mode for the object
  Prs3d_VertexDrawMode aCurrMode = anObject->Attributes()->VertexDrawMode();
  theDI <<  "Object's vertex draw mode: " << (aCurrMode == Prs3d_VDM_Isolated ? "'isolated'" : "'all'") << "\n";
  return 0;
}

//=======================================================================
//function : VPointCloud
//purpose  : Create interactive object for arbitrary set of points.
//=======================================================================
static Standard_Integer VPointCloud (Draw_Interpretor& theDI,
                                     Standard_Integer  theArgNum,
                                     const char**      theArgs)
{
  if (theArgNum < 2)
  {
     Message::SendFail ("Syntax error: wrong number of arguments");
     return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    Message::SendFail ("Error: no active view!");
    return 1;
  }

  TCollection_AsciiString aName;
  TopoDS_Shape aShape;

  TCollection_AsciiString aDistribution;
  gp_Pnt aDistCenter;
  Standard_Real aDistRadius = 0.0;
  Standard_Integer aDistNbPoints = 0;

  // parse options
  bool toRandColors = false;
  bool hasNormals = true, hasUV = false;
  bool isDensityPoints = false;
  Standard_Real aDensity = 0.0, aDist = 0.0;
  Standard_Real aTol = Precision::Confusion();
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNum; ++anArgIter)
  {
    TCollection_AsciiString aFlag (theArgs[anArgIter]);
    aFlag.LowerCase();
    if (aFlag == "-randcolors"
     || aFlag == "-randcolor")
    {
      toRandColors = Draw::ParseOnOffIterator (theArgNum, theArgs, anArgIter);
    }
    else if (aFlag == "-normals"
          || aFlag == "-normal")
    {
      hasNormals = Draw::ParseOnOffIterator (theArgNum, theArgs, anArgIter);
    }
    else if (aFlag == "-nonormals"
          || aFlag == "-nonormal")
    {
      hasNormals = !Draw::ParseOnOffIterator (theArgNum, theArgs, anArgIter);
    }
    else if (aFlag == "-uv"
          || aFlag == "-texels")
    {
      hasUV = Draw::ParseOnOffIterator (theArgNum, theArgs, anArgIter);
    }
    else if ((aFlag == "-dist"
           || aFlag == "-distance")
          && anArgIter + 1 < theArgNum
          && Draw::ParseReal (theArgs[anArgIter + 1], aDist))
    {
      ++anArgIter;
      if (aDist < 0.0)
      {
        theDI << "Syntax error: -distance value should be >= 0.0";
        return 1;
      }
      aDist = Max (aDist, Precision::Confusion());
    }
    else if ((aFlag == "-dens"
           || aFlag == "-density")
          && anArgIter + 1 < theArgNum
          && Draw::ParseReal (theArgs[anArgIter + 1], aDensity))
    {
      ++anArgIter;
      isDensityPoints = Standard_True;
      if (aDensity <= 0.0)
      {
        theDI << "Syntax error: -density value should be > 0.0";
        return 1;
      }
    }
    else if ((aFlag == "-tol"
           || aFlag == "-tolerance")
          && anArgIter + 1 < theArgNum
          && Draw::ParseReal (theArgs[anArgIter + 1], aTol))
    {
      ++anArgIter;
      if (aTol < Precision::Confusion())
      {
        theDI << "Syntax error: -tol value should be >= " << Precision::Confusion();
        return 1;
      }
    }
    else if ((aFlag == "-surface"
           || aFlag == "-volume")
            && anArgIter + 5 < theArgNum)
    {
      aDistribution = aFlag;
      aDistCenter.SetCoord (Draw::Atof (theArgs[anArgIter + 1]),
                            Draw::Atof (theArgs[anArgIter + 2]),
                            Draw::Atof (theArgs[anArgIter + 3]));
      aDistRadius   = Draw::Atof (theArgs[anArgIter + 4]);
      aDistNbPoints = Draw::Atoi (theArgs[anArgIter + 5]);
      anArgIter += 5;
    }
    else if (aName.IsEmpty())
    {
      aName = theArgs[anArgIter];
    }
    else if (aShape.IsNull())
    {
      aShape = DBRep::Get (theArgs[anArgIter]);
      if (aShape.IsNull())
      {
        theDI << "Syntax error: invalid shape '" << theArgs[anArgIter] << "'";
        return 1;
      }
    }
    else
    {
      theDI << "Syntax error at '" << theArgs[anArgIter] << "'";
      return 1;
    }
  }

  Graphic3d_ArrayFlags aFlags = Graphic3d_ArrayFlags_None;
  if (hasNormals)
  {
    aFlags |= Graphic3d_ArrayFlags_VertexNormal;
  }
  if (toRandColors)
  {
    aFlags |= Graphic3d_ArrayFlags_VertexColor;
  }
  if (hasUV)
  {
    aFlags |= Graphic3d_ArrayFlags_VertexTexel;
  }

  // generate arbitrary set of points
  Handle(Graphic3d_ArrayOfPoints) anArrayPoints;
  if (!aShape.IsNull())
  {
    class PointCloudPntFiller : public BRepLib_PointCloudShape
    {
    public:
      PointCloudPntFiller (Standard_Real theTol) : BRepLib_PointCloudShape (TopoDS_Shape(), theTol) {}
      void SetPointArray (const Handle(Graphic3d_ArrayOfPoints)& thePoints) { myPoints = thePoints; }

    protected:
      virtual void addPoint (const gp_Pnt& thePoint,
                             const gp_Vec& theNorm,
                             const gp_Pnt2d& theUV,
                             const TopoDS_Shape& ) Standard_OVERRIDE
      {
        const Standard_Integer aPntIndex = myPoints->AddVertex (thePoint, theUV);
        if (theNorm.SquareMagnitude() > gp::Resolution())
        {
          myPoints->SetVertexNormal (aPntIndex, theNorm);
        }
        if (myPoints->HasVertexColors())
        {
          Quantity_Color aColor (360.0 * Standard_Real(aPntIndex) / Standard_Real(myPoints->VertexNumberAllocated()),
                                 1.0, 0.5, Quantity_TOC_HLS);
          myPoints->SetVertexColor (aPntIndex, aColor);
        }
      }

    private:
      Handle(Graphic3d_ArrayOfPoints) myPoints;
    };

    PointCloudPntFiller aPoitCloudTool (aTol);
    aPoitCloudTool.SetShape (aShape);
    aPoitCloudTool.SetDistance (aDist);

    Standard_Integer aNbPoints = isDensityPoints
                               ? aPoitCloudTool.NbPointsByDensity (aDensity)
                               : aPoitCloudTool.NbPointsByTriangulation();
    theDI << "Number of the generated points : " << aNbPoints << "\n";
    anArrayPoints = new Graphic3d_ArrayOfPoints (aNbPoints, aFlags);
    aPoitCloudTool.SetPointArray (anArrayPoints);
    Standard_Boolean isDone = isDensityPoints
                            ? aPoitCloudTool.GeneratePointsByDensity (aDensity)
                            : aPoitCloudTool.GeneratePointsByTriangulation();
    if (!isDone)
    {
      Message::SendFail() << "Error: Point cloud was not generated";
      return 1;
    }
  }
  else if (!aDistribution.IsEmpty())
  {
    const bool isSurface = aDistribution == "-surface";

    anArrayPoints = new Graphic3d_ArrayOfPoints (aDistNbPoints, aFlags);
    std::mt19937 aRandomGenerator(0);
    std::uniform_real_distribution<> anAlphaDistrib(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<> aBetaDistrib  (0.0, 2.0 * M_PI);
    std::uniform_real_distribution<> aRadiusDistrib(0.0, aDistRadius);
    for (Standard_Integer aPntIt = 0; aPntIt < aDistNbPoints; ++aPntIt)
    {
      Standard_Real anAlpha   = anAlphaDistrib(aRandomGenerator);
      Standard_Real aBeta     = aBetaDistrib  (aRandomGenerator);
      Standard_Real aDistance = isSurface ? aDistRadius : aRadiusDistrib (aRandomGenerator);

      gp_Dir aDir (Cos (anAlpha) * Sin (aBeta),
                   Sin (anAlpha),
                   Cos (anAlpha) * Cos (aBeta));
      gp_Pnt aPoint = aDistCenter.Translated (aDir.XYZ() * aDistance);

      const Standard_Integer anIndexOfPoint = anArrayPoints->AddVertex (aPoint);
      if (toRandColors)
      {
        Quantity_Color aColor (360.0 * Standard_Real (anIndexOfPoint) / Standard_Real (aDistNbPoints),
                               1.0, 0.5, Quantity_TOC_HLS);
        anArrayPoints->SetVertexColor (anIndexOfPoint, aColor);
      }

      if (hasNormals)
      {
        anArrayPoints->SetVertexNormal (anIndexOfPoint, aDir);
      }
      if (hasUV)
      {
        anArrayPoints->SetVertexTexel (anIndexOfPoint, gp_Pnt2d (anAlpha / 2.0 * M_PI,
                                                                 aBeta   / 2.0 * M_PI));
      }
    }
  }
  else
  {
    Message::SendFail ("Error: wrong number of arguments");
    return 1;
  }

  // set array of points in point cloud object
  Handle(AIS_PointCloud) aPointCloud = new AIS_PointCloud();
  aPointCloud->SetPoints (anArrayPoints);
  ViewerTest::Display (aName, aPointCloud);
  return 0;
}

//=======================================================================
//function : VPriority
//purpose  : Prints or sets the display priority for an object
//=======================================================================

static int VPriority (Draw_Interpretor& theDI,
                      Standard_Integer  theArgNum,
                      const char**      theArgs)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aLastArg (theArgs[theArgNum - 1]);
  Standard_Integer aPriority = Graphic3d_DisplayPriority_INVALID;
  Standard_Integer aNbArgs   = theArgNum;
  if (aLastArg.IsIntegerValue())
  {
    aPriority = aLastArg.IntegerValue();
    --aNbArgs;
    if (aPriority < Graphic3d_DisplayPriority_Bottom || aPriority > Graphic3d_DisplayPriority_Topmost)
    {
      Message::SendFail() << "Syntax error: the specified display priority value '" << aLastArg
                          << "' is outside the valid range [" << Graphic3d_DisplayPriority_Bottom << ".." << Graphic3d_DisplayPriority_Topmost << "]";
      return 1;
    }
  }
  else
  {
    anUpdateTool.Invalidate();
  }

  if (aNbArgs < 2)
  {
    Message::SendFail ("Syntax error: wrong number of arguments! See usage:");
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < aNbArgs; ++anArgIter)
  {
    if (anUpdateTool.parseRedrawMode (theArgs[anArgIter]))
    {
      continue;
    }

    TCollection_AsciiString aName (theArgs[anArgIter]);
    Handle(AIS_InteractiveObject) anIObj;
    GetMapOfAIS().Find2 (aName, anIObj);
    if (anIObj.IsNull())
    {
      Message::SendFail() << "Error: the object '" << theArgs[1] << "' is not displayed";
      return 1;
    }

    if (aPriority == Graphic3d_DisplayPriority_INVALID)
    {
      theDI << aContext->DisplayPriority (anIObj) << " ";
    }
    else
    {
      aContext->SetDisplayPriority (anIObj, (Graphic3d_DisplayPriority )aPriority);
    }
  }
  return 0;
}

//! Auxiliary class for command vnormals.
class MyShapeWithNormals : public AIS_Shape
{
  DEFINE_STANDARD_RTTI_INLINE(MyShapeWithNormals, AIS_Shape);
public:

  Standard_Real    NormalLength;
  Standard_Integer NbAlongU;
  Standard_Integer NbAlongV;
  Standard_Boolean ToUseMesh;
  Standard_Boolean ToOrient;

public:

  //! Main constructor.
  MyShapeWithNormals (const TopoDS_Shape& theShape)
  : AIS_Shape   (theShape),
    NormalLength(10),
    NbAlongU  (1),
    NbAlongV  (1),
    ToUseMesh (Standard_False),
    ToOrient  (Standard_False) {}

protected:

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE
  {
    AIS_Shape::Compute (thePrsMgr, thePrs, theMode);

    NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > > aNormalMap;
    if (ToUseMesh)
    {
      DBRep_DrawableShape::addMeshNormals (aNormalMap, myshape, NormalLength);
    }
    else
    {
      DBRep_DrawableShape::addSurfaceNormals (aNormalMap, myshape, NormalLength, NbAlongU, NbAlongV);
    }

    Handle(Graphic3d_Group) aPrsGroup = thePrs->NewGroup();
    aPrsGroup->SetGroupPrimitivesAspect (myDrawer->ArrowAspect()->Aspect());

    const Standard_Real aArrowAngle  = myDrawer->ArrowAspect()->Angle();
    const Standard_Real aArrowLength = myDrawer->ArrowAspect()->Length();
    for (NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > >::Iterator aFaceIt (aNormalMap);
         aFaceIt.More(); aFaceIt.Next())
    {
      const Standard_Boolean toReverse = ToOrient && aFaceIt.Key().Orientation() == TopAbs_REVERSED;
      Handle(Graphic3d_ArrayOfSegments) aSegments = new Graphic3d_ArrayOfSegments (2 * aFaceIt.Value().Size());
      for (NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> >::Iterator aPntIt (aFaceIt.Value()); aPntIt.More(); aPntIt.Next())
      {
        std::pair<gp_Pnt, gp_Pnt> aPair = aPntIt.Value();
        if (toReverse)
        {
          const gp_Vec aDir = aPair.first.XYZ() - aPair.second.XYZ();
          aPair.second = aPair.first.XYZ() + aDir.XYZ();
        }

        aSegments->AddVertex (aPair.first);
        aSegments->AddVertex (aPair.second);
        Prs3d_Arrow::Draw (aPrsGroup, aPair.second, gp_Vec(aPair.first, aPair.second), aArrowAngle, aArrowLength);
      }

      aPrsGroup->AddPrimitiveArray (aSegments);
    }
  }

};

//=======================================================================
//function : VNormals
//purpose  : Displays/Hides normals calculated on shape geometry or retrieved from triangulation
//=======================================================================
static int VNormals (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNum,
                     const char**      theArgs)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail ("Error: no active viewer");
    return 1;
  }
  else if (theArgNum < 2)
  {
    Message::SendFail ("Syntax error: wrong number of arguments. See usage:");
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Standard_CString aShapeName = theArgs[anArgIter++];
  TopoDS_Shape     aShape     = DBRep::Get (aShapeName);
  Standard_Boolean isOn = Standard_True;
  if (aShape.IsNull())
  {
    Message::SendFail() << "Error: shape with name '" << aShapeName << "' is not found";
    return 1;
  }

  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(MyShapeWithNormals) aShapePrs;
  if (aMap.IsBound2 (aShapeName))
  {
    aShapePrs = Handle(MyShapeWithNormals)::DownCast (aMap.Find2 (aShapeName));
  }

  Standard_Boolean isUseMesh = Standard_False;
  Standard_Real    aLength = 10.0;
  Standard_Integer aNbAlongU = 1, aNbAlongV = 1;
  Standard_Boolean isOriented = Standard_False;
  for (; anArgIter < theArgNum; ++anArgIter)
  {
    TCollection_AsciiString aParam (theArgs[anArgIter]);
    aParam.LowerCase();
    if (anArgIter == 2
     && Draw::ParseOnOff (aParam.ToCString(), isOn))
    {
      continue;
    }
    else if (aParam == "-usemesh"
          || aParam == "-mesh")
    {
      isUseMesh = Standard_True;
    }
    else if (aParam == "-length"
          || aParam == "-len")
    {
      ++anArgIter;
      aLength = anArgIter < theArgNum ? Draw::Atof (theArgs[anArgIter]) : 0.0;
      if (Abs (aLength) <= gp::Resolution())
      {
        Message::SendFail ("Syntax error: length should not be zero");
        return 1;
      }
    }
    else if (aParam == "-orient"
          || aParam == "-oriented")
    {
      isOriented = Standard_True;
      if (anArgIter + 1 < theArgNum
        && Draw::ParseOnOff (theArgs[anArgIter + 1], isOriented))
      {
        ++anArgIter;
      }
    }
    else if (aParam == "-nbalongu"
          || aParam == "-nbu")
    {
      ++anArgIter;
      aNbAlongU = anArgIter < theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      if (aNbAlongU < 1)
      {
        Message::SendFail ("Syntax error: NbAlongU should be >=1");
        return 1;
      }
    }
    else if (aParam == "-nbalongv"
          || aParam == "-nbv")
    {
      ++anArgIter;
      aNbAlongV = anArgIter < theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      if (aNbAlongV < 1)
      {
        Message::SendFail ("Syntax error: NbAlongV should be >=1");
        return 1;
      }
    }
    else if (aParam == "-nbalong"
          || aParam == "-nbuv")
    {
      ++anArgIter;
      aNbAlongU = anArgIter < theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      aNbAlongV = aNbAlongU;
      if (aNbAlongU < 1)
      {
        Message::SendFail ("Syntax error: NbAlong should be >=1");
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << aParam << "'";
      return 1;
    }
  }

  if (isOn)
  {
    if (aShapePrs.IsNull())
    {
      aShapePrs = new MyShapeWithNormals (aShape);
    }
    aShapePrs->ToUseMesh    = isUseMesh;
    aShapePrs->ToOrient     = isOriented;
    aShapePrs->NormalLength = aLength;
    aShapePrs->NbAlongU     = aNbAlongU;
    aShapePrs->NbAlongV     = aNbAlongV;
    VDisplayAISObject (aShapeName, aShapePrs);
  }
  else if (!aShapePrs.IsNull())
  {
    VDisplayAISObject (aShapeName, new AIS_Shape (aShape));
  }

  return 0;
}

//=======================================================================
//function : ObjectsCommands
//purpose  :
//=======================================================================

void ViewerTest::ObjectCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup = "AIS Viewer";
  const char* aFileName = __FILE__;
  auto addCmd = [&](const char* theName, Draw_Interpretor::CommandFunction theFunc, const char* theHelp)
  {
    theCommands.Add (theName, theHelp, aFileName, theFunc, aGroup);
  };

  addCmd ("vtrihedron", VTrihedron, /* [vtrihedron] */ R"(
vtrihedron name
           [-dispMode {wireframe|shading} ]
           [-origin x y z ]
           [-zaxis u v w -xaxis u v w ]
           [-drawAxes {X|Y|Z|XY|YZ|XZ|XYZ}]
           [-hideLabels {on|off}]
           [-hideArrows {on|off}]
           [-label {XAxis|YAxis|ZAxis} value]
           [-attribute {XAxisLength|YAxisLength|ZAxisLength
                       |TubeRadiusPercent|ConeRadiusPercent
                       |ConeLengthPercent|OriginRadiusPercent
                       |ShadingNumberOfFacettes} value]
           [-color {Origin|XAxis|YAxis|ZAxis|XOYAxis|YOZAxis
                   |XOZAxis|Whole} {r g b | colorName}]
           [-textColor  [XAxis|YAxis|ZAxis] {r g b | colorName}]
           [-arrowColor [XAxis|YAxis|ZAxis] {r g b | colorName}]
           [-priority {Origin|XAxis|YAxis|ZAxis|XArrow
                      |YArrow|ZArrow|XOYAxis|YOZAxis
                      |XOZAxis|Whole} value]

Creates/changes *AIS_Trihedron* object.
 -dispMode   mode of visualization: wf - wireframe,
                                    sh - shading;
             default value is wireframe;
 -origin     allows to set trihedron location;
 -zaxis/-xaxis allows to set trihedron X and Z directions;
             the directions should be orthogonal;
             Y direction is calculated;
 -drawAxes   allows to set what axes are drawn in the
             trihedron, default state is XYZ;
 -hideLabels allows to show/hide trihedron labels;
 -hideArrows allows to show/hide trihedron arrows;
 -label      allows to change default X/Y/Z titles of axes;
 -attribute  sets parameters of trihedron;
 -color      sets color properties of parts of trihedron;
 -textColor  sets color properties of trihedron labels;
 -arrowColor sets color properties of trihedron arrows;
 -priority   allows to change default selection priority
             of trihedron components.
)" /* [vtrihedron] */);

  addCmd ("vtri2d", VTrihedron2D, /* [vtri2d] */ R"(
vtri2d Name : Creates a plane with a 2D trihedron from an interactively selected face.
)" /* [vtri2d] */);

  addCmd ("vplanetri", VPlaneTrihedron, /* [vplanetri] */ R"(
vplanetri name
Create a plane from a trihedron selection.
If no arguments are set, the default plane is created.
)" /* [vplanetri] */);

  addCmd ("vsize", VSize, /* [vsize] */ R"(
vsize [name(Default=Current)] [size(Default=100)]
Changes the size of a named or selected trihedron.
If the name is not defined: it affects the selected trihedrons otherwise nothing is done.
If the value is not defined: it is set to 100 by default.
)" /* [vsize] */);

  addCmd ("vaxis", VAxisBuilder, /* [vaxis] */ R"(
vaxis name [Xa] [Ya] [Za] [Xb] [Yb] [Zb]
Creates an axis. If  the values are not defined,
an axis is created by interactive selection of two vertices or one edge.
)" /* [vaxis] */);

  addCmd ("vaxispara", VAxisBuilder, /* [vaxispara] */ R"(
vaxispara name
Creates an axis by interactive selection of an edge and a vertex.
)" /* [vaxispara] */);

  addCmd ("vaxisortho", VAxisBuilder, /* [vaxisortho] */ R"(
vaxisortho name
Creates an axis by interactive selection of an edge and a vertex.
The axis will be orthogonal to the selected edge.
)" /* [vaxisortho] */);

  addCmd ("vpoint", VPointBuilder, /* [vpoint] */ R"(
vpoint name [X Y [Z]] [-2d] [-nosel]
Creates a point from coordinates.
If the values are not defined, a point is created from selected vertex or edge (center).
 -2d    defines on-screen 2D point from top-left window corner;
 -nosel creates non-selectable presentation.
)" /* [vpoint] */);

  addCmd ("vplane", VPlaneBuilder, /* [vplane] */ R"(
vplane PlaneName [AxisName/PlaneName/PointName]
       [PointName/PointName/PointName] [Nothing/Nothing/PointName] [TypeOfSensitivity {0|1}]
Creates a plane from named or interactively selected entities. TypeOfSensitivity:
  0 - Interior;
  1 - Boundary.
)" /* [vplane] */);

  addCmd ("vchangeplane", VChangePlane, /* [vchangeplane] */ R"(
vchangeplane plane_name
             [x=center_x y=center_y z=center_z]
             [dx=dir_x dy=dir_y dz=dir_z]
             [sx=size_x sy=size_y]
             [minsize=value]
             [noupdate]
Changes parameters of the plane:
 - x y z     - center
 - dx dy dz  - normal
 - sx sy     - plane sizes
 - noupdate  - do not update/redisplay the plane in context
Please enter coordinates in format "param=value" in arbitrary order.
)" /* [vchangeplane] */);

  addCmd ("vplanepara", VPlaneBuilder, /* [vplanepara] */ R"(
vplanepara  PlaneName
Creates a plane from interactively selected vertex and face.
)" /* [vplanepara] */);

  addCmd ("vplaneortho", VPlaneBuilder, /* [vplaneortho] */ R"(
vplaneortho  PlaneName
Creates a plane from interactive selected face and coplanar edge.
)" /* [vplaneortho] */);

  addCmd ("vline", VLineBuilder, /* [vline] */ R"(
vline LineName [Xa/PointName] [Ya/PointName] [Za] [Xb] [Yb] [Zb]
Creates a line from coordinates, named or interactively selected vertices.
)" /* [vline] */);

  addCmd ("vcircle", VCircleBuilder, /* [vcircle] */ R"(
vcircle CircleName [PointName PointName PointName IsFilled] [UStart UEnd]
                   [PlaneName PointName Radius IsFilled] [UStart UEnd]
Creates a circle from named or interactively selected entities.
Parameter IsFilled is defined as 0 or 1.
)" /* [vcircle] */);

  addCmd ("vdrawtext", VDrawText, /* [vdrawtext] */ R"(
vdrawtext name text
          [-pos X Y Z]={0 0 0}
          [-color {R G B|name}]=yellow
          [-halign {left|center|right}]=left
          [-valign {top|center|bottom|topfirstline}}]=bottom
          [-angle angle]=0
          [-zoom {0|1}]=0
          [-height height]=16
          [-wrapping width]=40
          [-wordwrapping {0|1}]=1
          [-aspect {regular|bold|italic|boldItalic}]=regular
          [-font font]=Times
          [-2d] [-perspos {X Y Z}]={0 0 0}
          [-disptype {blend|decal|shadow|subtitle|dimension|normal}}=normal
          [-subcolor {R G B|name}]=white
          [-noupdate]
          [-plane NormX NormY NormZ DirX DirY DirZ]
          [-flipping] [-ownanchor {0|1}]=1
Display text label at specified position.
Within -perspos, X and Y define the coordinate origin in 2d space relative to the view window.
Example: X=0 Y=0 is center, X=1 Y=1 is upper right corner etc...
Z coordinate defines the gap from border of view window (except center position).
)" /* [vdrawtext] */);

  addCmd ("vdrawsphere", VDrawSphere, /* [vdrawsphere] */ R"(
vdrawsphere shapeName Fineness [X=0.0 Y=0.0 Z=0.0] [Radius=100.0] [ToShowEdges=0] [ToPrintInfo=1]
)" /* [vdrawsphere] */);

  addCmd ("vlocation", VSetLocation, /* [vlocation] */ R"(
vlocation name
    [-reset] [-copyFrom otherName]
    [-translate    X Y [Z]] [-rotate    x y z dx dy dz angle] [-scale    [X Y Z] scale]
    [-pretranslate X Y [Z]] [-prerotate x y z dx dy dz angle] [-prescale [X Y Z] scale]
    [-mirror x y z dx dy dz] [-premirror x y z dx dy dz]
    [-setLocation X Y [Z]] [-setRotation QX QY QZ QW] [-setScale [X Y Z] scale]
Object local transformation management:
 -reset        resets transformation to identity
 -translate    applies translation vector
 -rotate       applies rotation around axis
 -scale        applies scale factor with optional anchor
 -mirror       applies mirror transformation
 -pretranslate pre-multiplies translation vector
 -prerotate    pre-multiplies rotation around axis
 -prescale     pre-multiplies scale  transformation
 -premirror    pre-multiplies mirror transformation
 -setLocation  overrides translation part
 -setRotation  overrides rotation part with specified quaternion
 -setScale     overrides scale factor
)" /* [vlocation] */);

  addCmd ("vsetlocation", VSetLocation, /* [vsetlocation] */ R"(
Alias for vlocation
)" /* [vsetlocation] */);

  addCmd ("vchild", VChild, /* [vchild] */ R"(
vchild parent [-add] [-remove] [-ignoreParentTrsf {0|1}] child1 [child2] [...]
Command for testing low-level presentation connections.
vconnect command should be used instead.
)" /* [vchild] */);

  addCmd ("vparent", VParent, /* [vparent] */ R"(
vparent parent [-ignoreVisu]
Command for testing object properties as parent in the hierarchy.
 -ignoreVisu do not propagate the visual state (display/erase/color) to children objects
)" /* [vparent] */);

  addCmd ("vcomputehlr", VComputeHLR, /* [vcomputehlr] */ R"(
vcomputehlr shapeInput hlrResult [-algoType {algo|polyAlgo}=polyAlgo]
    [eyeX eyeY eyeZ dirX dirY dirZ upX upY upZ]
    [-showTangentEdges {on|off}=off] [-nbIsolines N=0] [-showHiddenEdges {on|off}=off]
Arguments:
  shapeInput - name of the initial shape
  hlrResult  - result HLR object from initial shape
  eye, dir are eye position and look direction
  up is the look up direction vector
 -algoType HLR algorithm to use
 -showTangentEdges include tangent edges
 -nbIsolines include isolines
 -showHiddenEdges include hidden edges
Use vtop to see projected HLR shape.
)" /* [vcomputehlr] */);

  addCmd ("vdrawparray", VDrawPArray, /* [vdrawparray] */ R"(
vdrawparray name TypeOfArray={points|segments|polylines|triangles
                   |trianglefans|trianglestrips|quads|quadstrips|polygons}
            [-deinterleaved|-mutable]
            [vertex={'v' x y z [normal={'n' nx ny nz}] [color={'c' r g b}] [texel={'t' tx ty}]]
            [bound= {'b' nbVertices [bound_color={'c' r g b}]]
            [edge=  {'e' vertexId]
            [-shape shapeName] [-patch]
Commands create an Interactive Object for specified Primitive Array definition
with the main purpose is covering various combinations by tests.
)" /* [vdrawparray] */);

  addCmd ("vconnect", VConnect, /* [vconnect] */ R"(
vconnect name Xo Yo Zo object1 object2 ... [color=NAME]
Creates and displays AIS_ConnectedInteractive object from input object and location.
)" /* [vconnect] */);

  addCmd ("vconnectto", VConnectTo, /* [vconnectto] */ R"(
vconnectto instance_name Xo Yo Zo object [-nodisplay|-noupdate|-update]
Makes an instance 'instance_name' of 'object' with position (Xo Yo Zo).
 -nodisplay - only creates interactive object, but not displays it.
)" /* [vconnectto] */);

  addCmd ("vdisconnect", VDisconnect, /* [vdisconnect] */ R"(
vdisconnect assembly_name {object_name|object_number|'all'}
Disconnects all objects from assembly or disconnects object by name or number.
Use vlistconnected to enumerate assembly children.
)" /* [vdisconnect] */);

  addCmd ("vaddconnected", VAddConnected, /* [vaddconnected] */ R"(
vaddconnected assembly_name object_name
Adds object to assembly.
)" /* [vaddconnected] */);

  addCmd ("vlistconnected", VListConnected, /* [vlistconnected] */ R"(
vlistconnected assembly_name
Lists objects in assembly.
)" /* [vlistconnected] */);

  addCmd ("vselmode", VSetSelectionMode, /* [vselmode] */ R"(
vselmode [object] selectionMode {on|off}
         [{-add|-set|-globalOrLocal}=-globalOrLocal]
Switches selection mode for the specified object or for all objects in context.
Selection mode is either an integer number specific to Interactive Object,
or sub-shape type in case of AIS_Shape:
  Shape, Vertex, Edge, Wire, Face, Shell, Solid, CompSolid, Compound
The integer mode 0 (Shape in case of AIS_Shape) is reserved for selecting object as whole.
Additional options:
 -add           already activated selection modes will be left activated
 -set           already activated selection modes will be deactivated
 -globalOrLocal (default) if new mode is Global selection mode,
                then active local selection modes will be deactivated
                and the samthen active local selection modes will be deactivated
)" /* [vselmode] */);

  addCmd ("vselnext", VSelectionNext, /* [vselnext] */ R"(
vselnext : hilight next detected
)" /* [vselnext] */);

  addCmd ("vselprev", VSelectionPrevious, /* [vselprev] */ R"(
vselnext : hilight previous detected
)" /* [vselprev] */);

  addCmd ("vtriangle", VTriangle, /* [vtriangle] */ R"(
vtriangle Name PointName PointName PointName
Creates and displays a filled triangle from named points.
)" /* [vtriangle] */);

  addCmd ("vsegment", VTriangle, /* [vsegment] */ R"(
vsegment Name PointName PointName
Creates and displays a segment from named points.
)" /* [vsegment] */);

  addCmd ("vtorus", VTorus, /* [vtorus] */ R"(
vtorus name [R1 R2 [Angle1=0 Angle2=360] [Angle=360]]
       [-radius R1] [-pipeRadius R2]
       [-pipeAngle Angle=360] [-segmentAngle1 Angle1=0 -segmentAngle2 Angle2=360]
       [-nbSlices Number=100] [-nbStacks Number=100] [-noupdate]
Creates and displays a torus or torus segment.
Parameters of the torus:
 - R1     distance from the center of the pipe to the center of the torus
 - R2     radius of the pipe
 - Angle1 first angle to create a torus ring segment
 - Angle2 second angle to create a torus ring segment
 - Angle  angle to create a torus pipe segment
)" /* [vtorus] */);

  addCmd ("vcylinder", VCylinder, /* [vcylinder] */ R"(
vcylinder name [R1 R2 Height] [-height H] [-radius R] [-bottomRadius R1 -topRadius R2]
               [-nbSlices Number=100] [-noupdate]
Creates and displays a cylinder.
Parameters of the cylinder:
 - R1     cylinder bottom radius
 - R2     cylinder top radius
 - Height cylinder height
)" /* [vcylinder] */);

  addCmd ("vsphere", VSphere, /* [vsphere] */ R"(
vsphere name [-radius] R
             [-nbSlices Number=100] [-nbStacks Number=100] [-noupdate]
Creates and displays a sphere.
)" /* [vsphere] */);

  addCmd ("vobjzlayer", VObjZLayer, /* [vobjzlayer] */ R"(
vobjzlayer : set/get object [layerid] - set or get z layer id for the interactive object
)" /* [vobjzlayer] */);
  
  addCmd ("vpolygonoffset", VPolygonOffset, /* [vpolygonoffset] */ R"(
vpolygonoffset [object [mode factor units]]
Sets/gets polygon offset parameters for an object; without arguments prints the default values
)" /* [vpolygonoffset] */);

  addCmd ("vmarkerstest", VMarkersTest, /* [vmarkerstest] */ R"(
vmarkerstest: name X Y Z [PointsOnSide=10] [MarkerType=0] [Scale=1.0] [FileName=ImageFile]
)" /* [vmarkerstest] */);

  addCmd ("text2brep", TextToBRep, /* [text2brep] */ R"(
text2brep name text"
          [-pos X=0 Y=0 Z=0]"
          [-halign {left|center|right}=left]"
          [-valign {top|center|bottom|topfirstline}=bottom}]"
          [-height height=16]"
          [-aspect {regular|bold|italic|boldItalic}=regular]"
          [-font font=Courier] [-strict {strict|aliases|any}=any]"
          [-composite {on|off}=off]"
          [-plane NormX NormY NormZ DirX DirY DirZ]",
)" /* [text2brep] */);

  addCmd ("vfont", VFont, /* [vfont] */ R"(
vfont [-add pathToFont [fontName] [regular,bold,italic,boldItalic=undefined] [singleStroke]]
      [-strict {any|aliases|strict}] [-find fontName [regular,bold,italic,boldItalic=undefined]]
      [-verbose {on|off}]
      [-findAll fontNameMask] [-findInfo fontName]
      [-unicodeFallback {on|off}]
      [-clear] [-init] [-list] [-names]
      [-aliases [aliasName]] [-addAlias Alias FontName] [-removeAlias Alias FontName]
      [-clearAlias Alias] [-clearAliases]
Work with font registry - register font, list available fonts, find font.
 -findAll  is same as -find, but can print more than one font when mask is passed.
 -findInfo is same as -find, but prints complete font information instead of family name.
)" /* [vfont] */);

  addCmd ("vvertexmode", VVertexMode, /* [vvertexmode] */ R"(
vvertexmode [name | -set {isolated|all|inherited} [name1 name2 ...]]
Sets the vertex draw mode for the specified object(s)
or sets default vertex draw mode and updates the mode for all displayed objects.
Prints the default vertex draw mode without -set parameter.
)" /* [vvertexmode] */);

  addCmd ("vpointcloud", VPointCloud, /* [vpointcloud] */ R"(
vpointcloud name shape [-randColor {0|1}]=0 [-normals {0|1}]=1 [-uv {0|1}]=0
            [-distance Value]=0.0 [-density Value] [-tolerance Value]
Create an interactive object for arbitrary set of points from triangulated shape.

vpointcloud name {-surface|-volume} x y z r npts
            [-randColor] [-normals] [-uv]
Create arbitrary set of points (npts) randomly distributed
on spheric surface or within spheric volume (x y z r).

Additional options:
 -normals   generate or not normal per point
 -uv        generate UV (texel) coordinates per point
 -randColor generate random color per point
 -distance  distance from shape into the range [0, Value];
 -density   density of points to generate randomly on surface;
 -tolerance cloud generator's tolerance; default value is Precision::Confusion();

)" /* [vpointcloud] */);

  addCmd ("vpriority", VPriority, /* [vpriority] */ R"(
vpriority [-noupdate|-update] name [value]
Prints or sets the display priority for an object.
)" /* [vpriority] */);

  addCmd ("vnormals", VNormals, /* [vnormals] */ R"(
vnormals Shape [{on|off}=on] [-length {10}] [-nbAlongU {1}] [-nbAlongV {1}] [-nbAlong {1}]
               [-useMesh] [-oriented {0}1}=0]
Displays/Hides normals calculated on shape geometry or retrieved from triangulation
)" /* [vnormals] */);
}
