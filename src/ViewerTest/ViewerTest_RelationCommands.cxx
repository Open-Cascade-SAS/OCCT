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

#include <AIS_AngleDimension.hxx>
#include <AIS_Circle.hxx>
#include <AIS_ConcentricRelation.hxx>
#include <AIS_DiameterDimension.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_EqualDistanceRelation.hxx>
#include <AIS_EqualRadiusRelation.hxx>
#include <AIS_FixRelation.hxx>
#include <AIS_IdenticRelation.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_KindOfRelation.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_MapOfInteractive.hxx>
#include <AIS_OffsetDimension.hxx>
#include <AIS_ParallelRelation.hxx>
#include <AIS_PerpendicularRelation.hxx>
#include <AIS_Point.hxx>
#include <AIS_RadiusDimension.hxx>
#include <AIS_Relation.hxx>
#include <AIS_Shape.hxx>
#include <AIS_SymmetricRelation.hxx>
#include <AIS_TangentRelation.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_ExtCC.hxx>
#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepExtrema_ExtFF.hxx>
#include <BRepTools.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Window.hxx>
#include <DBRep.hxx>
#include <ElSLib.hxx>
#include <Font_FontMgr.hxx>
#include <GC_MakePlane.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakePln.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <Precision.hxx>
#include <StdSelect.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_EventManager.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Dimension.hxx>

extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern int ViewerMainLoop(Standard_Integer argc, const char** argv);
extern Handle(AIS_InteractiveContext)& TheAISContext ();

#define VertexMask 0x01
#define EdgeMask 0x02
#define FaceMask 0x04

//=======================================================================
//function : Get3DPointAtMousePosition
//purpose  : Calculates the 3D points corresponding to the mouse position
//           in the plane of the view
//=======================================================================
static gp_Pnt Get3DPointAtMousePosition()
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();

  Standard_Real xv,yv,zv;
  aView->Proj (xv,yv,zv);
  Standard_Real xat,yat,zat;
  aView->At(xat,yat,zat);
  gp_Pln aPlane (gp_Pnt(xat,yat,zat), gp_Dir(xv,yv,zv));
  
  Standard_Integer aPixX, aPixY;
  Standard_Real aX, aY, aZ, aDX, aDY, aDZ;

  ViewerTest::GetMousePosition (aPixX, aPixY);
  aView->ConvertWithProj (aPixX, aPixY, aX, aY, aZ, aDX, aDY, aDZ);
  gp_Lin aLine( gp_Pnt(aX, aY, aZ), gp_Dir(aDX, aDY, aDZ) );

  // Compute intersection
  Handle(Geom_Line) aGeomLine = new Geom_Line (aLine);
  Handle(Geom_Plane) aGeomPlane = new Geom_Plane (aPlane);
  GeomAPI_IntCS anIntersector (aGeomLine, aGeomPlane);
  if (!anIntersector.IsDone() || anIntersector.NbPoints() == 0)
  {
    return gp::Origin();
  }
  return anIntersector.Point (1);
}

//=======================================================================
//function : Get3DPointAtMousePosition
//purpose  : Calculates the 3D points corresponding to the mouse position
//           in the plane of the view
//=======================================================================
static Standard_Boolean Get3DPointAtMousePosition (const gp_Pnt& theFirstPoint,
                                                   const gp_Pnt& theSecondPoint,
                                                   gp_Pnt& theOutputPoint)
{
  theOutputPoint = gp::Origin();

  Handle(V3d_View) aView = ViewerTest::CurrentView();

  Standard_Integer aPixX, aPixY;
  Standard_Real aX, aY, aZ, aDx, aDy, aDz, aUx, aUy, aUz;

  // Get 3D point in view coordinates and projection vector from the pixel point.
  ViewerTest::GetMousePosition (aPixX, aPixY);
  aView->ConvertWithProj (aPixX, aPixY, aX, aY, aZ, aDx, aDy, aDz);
  gp_Lin aProjLin (gp_Pnt(aX, aY, aZ), gp_Dir(aDx, aDy, aDz));

  // Get plane
  gp_Vec aDimVec (theFirstPoint, theSecondPoint);
  aView->Up (aUx, aUy, aUz);
  gp_Vec aViewUp (aUx, aUy, aUz);

  if (aDimVec.IsParallel (aViewUp, Precision::Angular()))
  {
    theOutputPoint = Get3DPointAtMousePosition();
    return Standard_True;
  }

  gp_Vec aDimNormal = aDimVec ^ aViewUp;
  gp_Pln aViewPlane= gce_MakePln (theFirstPoint, aDimNormal);

  // Get intersection of view plane and projection line
  Handle(Geom_Plane) aPlane = new Geom_Plane (aViewPlane);
  Handle(Geom_Line) aProjLine = new Geom_Line (aProjLin);
  GeomAPI_IntCS anIntersector (aProjLine, aPlane);
  if (!anIntersector.IsDone() || anIntersector.NbPoints() == 0)
  {
    return Standard_False;
  }

  theOutputPoint = anIntersector.Point (1);
  return Standard_True;
}

//=======================================================================
//function : ParseDimensionParams
//purpose  : Auxilliary function: sets aspect parameters for
//           length, angle, radius and diameter dimension.
//
//draw args: -text [3d|2d] [wf|sh|wireframe|shading] [Size]
//           -label [left|right|hcenter|hfit] [top|bottom|vcenter|vfit]
//           -arrow [external|internal|fit] [Length(int)]
//           -arrowangle ArrowAngle(degrees)
//           -plane xoy|yoz|zox
//           -flyout FloatValue -extension FloatValue
//           -autovalue
//           -value CustomRealValue
//           -textvalue CustomTextValue
//           -dispunits DisplayUnitsString
//           -modelunits ModelUnitsString
//           -showunits
//           -hideunits
//
// Warning! flyout is not an aspect value, it is for dimension parameter
// likewise text position, but text position override other paramaters.
// For text position changing use 'vmovedim'.
//=======================================================================
static int ParseDimensionParams (Standard_Integer  theArgNum,
                                 const char**      theArgVec,
                                 Standard_Integer  theStartIndex,
                                 const Handle(Prs3d_DimensionAspect)& theAspect,
                                 Standard_Boolean& theIsCustomPlane, gp_Pln& thePlane,
                                 NCollection_DataMap<TCollection_AsciiString, Standard_Real>& theRealParams,
                                 NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theStringParams,
                                 NCollection_List<Handle(AIS_InteractiveObject)>* theShapeList = NULL)
{
  theRealParams.Clear();
  theStringParams.Clear();

  theIsCustomPlane  = Standard_False;

  // Begin from the second parameter: the first one is dimension name
  for (Standard_Integer anIt = theStartIndex; anIt < theArgNum; ++anIt)
  {
    TCollection_AsciiString aParam (theArgVec[anIt]);
    aParam.LowerCase();

    if (aParam.Search ("-") == -1)
    {
      continue;
    }

    // Boolean flags
    if (aParam.IsEqual ("-autovalue"))
    {
      theRealParams.Bind ("autovalue", 1);
      continue;
    }

    if (aParam.IsEqual ("-showunits"))
    {
      theAspect->MakeUnitsDisplayed (Standard_True);
      continue;
    }
    else if (aParam.IsEqual ("-hideunits"))
    {
      theAspect->MakeUnitsDisplayed (Standard_False);
      continue;
    }
    else if (aParam.IsEqual ("-selected"))
    {
      if (!theShapeList)
      {
        std::cerr << "Error: unknown parameter '" << aParam << "'\n";
        return 1;
      }

      for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected(); TheAISContext()->NextSelected())
      {
        TopoDS_Shape aShape = TheAISContext()->SelectedShape();
        if (!aShape.IsNull())
        {
          theShapeList->Append (new AIS_Shape (aShape));
        }
      }
      continue;
    }

    // Before all non-boolean flags parsing check if a flag have at least one value.
    if (anIt + 1 >= theArgNum)
    {
      std::cerr << "Error: "<< aParam <<" flag should have value.\n";
      return 1;
    }

    // Non-boolean flags
    if (aParam.IsEqual ("-shape")
     || aParam.IsEqual ("-shapes"))
    {
      if (!theShapeList)
      {
        std::cerr << "Error: unknown parameter '" << aParam << "'\n";
        return 1;
      }

      do
      {
        anIt++;
        TCollection_AsciiString anArgString = theArgVec[anIt];
        Handle(AIS_InteractiveObject) anAISObject;
        Standard_CString aStr   = anArgString.ToCString();
        TopoDS_Shape     aShape =  DBRep::Get (aStr);
        if (!aShape.IsNull())
        {
          anAISObject = new AIS_Shape (aShape);
        }
        else
        {
          if (!GetMapOfAIS().IsBound2 (anArgString))
          {
            std::cerr << "Error: shape with name '" << aStr << "' is not found.\n";
            return 1;
          }

          anAISObject = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (anArgString));
          if (anAISObject.IsNull())
          {
            std::cerr << "Error: " << aStr <<" is not a shape.\n";
            return 1;
          }
        }
        theShapeList->Append (anAISObject);
      }
      while (anIt + 1 < theArgNum && theArgVec[anIt + 1][0] != '-');
    }
    else if (aParam.IsEqual ("-text"))
    {
      do
      {
        anIt++;
        TCollection_AsciiString aValue (theArgVec[anIt]);
        aValue.LowerCase();
        if (aValue.IsEqual ("3d"))
        {
          theAspect->MakeText3d (Standard_True);
        }
        else if (aValue.IsEqual ("2d"))
        {
          theAspect->MakeText3d (Standard_False);
        }
        else if (aValue.IsEqual ("wf") || aValue.IsEqual ("wireframe"))
        {
           theAspect->MakeTextShaded (Standard_False);
        }
        else if ( aValue.IsEqual ("sh") || aValue.IsEqual ("shading"))
        {
          theAspect->MakeTextShaded (Standard_True);
        }
        else if (aValue.IsIntegerValue()) // text size
        {
          theAspect->TextAspect()->SetHeight (Draw::Atoi (aValue.ToCString()));
        }
      }
      while (anIt + 1 < theArgNum && theArgVec[anIt + 1][0] != '-');
    }
    else if (aParam.IsEqual ("-font"))
    {
      if (anIt + 1 >= theArgNum)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam << "'.\n";
        return 1;
      }

      theAspect->TextAspect()->SetFont (theArgVec[++anIt]);
    }
    else if (aParam.IsEqual ("-label"))
    {
      do
      {
        anIt++;
        TCollection_AsciiString aParamValue (theArgVec[anIt]);
        aParamValue.LowerCase();

        if (aParamValue == "left")         { theAspect->SetTextHorizontalPosition (Prs3d_DTHP_Left);  }
        else if (aParamValue == "right")   { theAspect->SetTextHorizontalPosition (Prs3d_DTHP_Right); }
        else if (aParamValue == "hcenter") { theAspect->SetTextHorizontalPosition (Prs3d_DTHP_Center);}
        else if (aParamValue == "hfit")    { theAspect->SetTextHorizontalPosition (Prs3d_DTHP_Fit);   }
        else if (aParamValue == "above")   { theAspect->SetTextVerticalPosition   (Prs3d_DTVP_Above); }
        else if (aParamValue == "below")   { theAspect->SetTextVerticalPosition   (Prs3d_DTVP_Below); }
        else if (aParamValue == "vcenter") { theAspect->SetTextVerticalPosition   (Prs3d_DTVP_Center);}
        else
        {
          std::cerr << "Error: invalid label position: '" << aParamValue << "'.\n";
          return 1;
        }
      }
      while (anIt + 1 < theArgNum && theArgVec[anIt+1][0] != '-');
    }
    else if (aParam.IsEqual ("-arrow"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);
      aLocalParam.LowerCase();

      if (aLocalParam == "external") { theAspect->SetArrowOrientation (Prs3d_DAO_External); }
      if (aLocalParam == "internal") { theAspect->SetArrowOrientation (Prs3d_DAO_Internal); }
      if (aLocalParam == "fit")      { theAspect->SetArrowOrientation (Prs3d_DAO_Fit); }
    }
    else if (aParam.IsEqual ("-arrowlength") || aParam.IsEqual ("-arlen"))
    {
      TCollection_AsciiString aValue (theArgVec[++anIt]);
      if (!aValue.IsRealValue())
      {
        std::cerr << "Error: arrow lenght should be float degree value.\n";
        return 1;
      }
      theAspect->ArrowAspect()->SetLength (Draw::Atof (aValue.ToCString()));
    }
    else if (aParam.IsEqual ("-arrowangle") || aParam.IsEqual ("-arangle"))
    {
      TCollection_AsciiString aValue (theArgVec[++anIt]);
      if (!aValue.IsRealValue())
      {
        std::cerr << "Error: arrow angle should be float degree value.\n";
        return 1;
      }
      theAspect->ArrowAspect()->SetAngle (Draw::Atof (aValue.ToCString()));
    }
    else if (aParam.IsEqual ("-color"))
    {
      theAspect->SetCommonColor (Quantity_Color (ViewerTest::GetColorFromName (theArgVec[++anIt])));
    }
    else if (aParam.IsEqual ("-extension"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);
      if (!aLocalParam.IsRealValue())
      {
        std::cerr << "Error: extension size for dimension should be real value.\n";
        return 1;
      }
      theAspect->SetExtensionSize (Draw::Atof (aLocalParam.ToCString()));
    }
    else if (aParam.IsEqual ("-plane"))
    {
      TCollection_AsciiString aValue (theArgVec[++anIt]);
      aValue.LowerCase();
      if (aValue == "xoy")
      {
        theIsCustomPlane = Standard_True;
        thePlane = gp_Pln (gp_Ax3 (gp::XOY()));
      }
      else if (aValue == "zox")
      {
        theIsCustomPlane = Standard_True;
        thePlane = gp_Pln (gp_Ax3 (gp::ZOX()));
      }
      else if (aValue == "yoz")
      {
        theIsCustomPlane = Standard_True;
        thePlane = gp_Pln (gp_Ax3 (gp::YOZ()));
      }
      else
      {
        std::cerr << "Error: wrong plane '" << aValue << "'.\n";
        return 1;
      }
    }
    else if (aParam.IsEqual ("-flyout"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);
      if (!aLocalParam.IsRealValue())
      {
        std::cerr << "Error: flyout for dimension should be real value.\n";
        return 1;
      }

      theRealParams.Bind ("flyout", Draw::Atof (aLocalParam.ToCString()));
    }
    else if (aParam.IsEqual ("-value"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);
      if (!aLocalParam.IsRealValue())
      {
        std::cerr << "Error: dimension value for dimension should be real value.\n";
        return 1;
      }

      theRealParams.Bind ("value", Draw::Atof (aLocalParam.ToCString()));
    }
    else if (aParam.IsEqual ("-textvalue"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);

      theStringParams.Bind ("textvalue", aLocalParam);
    }
    else if (aParam.IsEqual ("-modelunits"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);

      theStringParams.Bind ("modelunits", aLocalParam);
    }
    else if (aParam.IsEqual ("-dispunits"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);

      theStringParams.Bind ("dispunits", aLocalParam);
    }
    else
    {
      std::cerr << "Error: unknown parameter '" << aParam << "'.\n";
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : SetDimensionParams
//purpose  : Sets parameters for dimension
//=======================================================================
static void SetDimensionParams (const Handle(AIS_Dimension)& theDim,
                                const NCollection_DataMap<TCollection_AsciiString, Standard_Real>& theRealParams,
                                const NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theStringParams)
{
  if (theRealParams.IsBound ("flyout"))
  {
    theDim->SetFlyout (theRealParams.Find ("flyout"));
  }

  if (theRealParams.IsBound ("autovalue"))
  {
    theDim->SetComputedValue();
  }

  if (theRealParams.IsBound ("value"))
  {
    theDim->SetCustomValue (theRealParams.Find ("value"));
  }

  if (theStringParams.IsBound ("textvalue"))
  {
    theDim->SetCustomValue (theStringParams.Find ("textvalue"));
  }

  if (theStringParams.IsBound ("modelunits"))
  {
    theDim->SetModelUnits (theStringParams.Find ("modelunits"));
  }

  if (theStringParams.IsBound ("dispunits"))
  {
    theDim->SetDisplayUnits (theStringParams.Find ("dispunits"));
  }
}

//=======================================================================
//function : ParseAngleDimensionParams
//purpose  : Auxilliary function: sets custom parameters for angle dimension.
//
//draw args: -type [interior|exterior]
//           -showarrow [first|second|both|none]
//=======================================================================
static int ParseAngleDimensionParams (Standard_Integer  theArgNum,
                               const char**      theArgVec,
                               Standard_Integer  theStartIndex,
                               NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theStringParams)
{
  theStringParams.Clear();

  // Begin from the second parameter: the first one is dimension name
  for (Standard_Integer anIt = theStartIndex; anIt < theArgNum; ++anIt)
  {
    TCollection_AsciiString aParam (theArgVec[anIt]);
    aParam.LowerCase();

    if (aParam.Search ("-") == -1)
    {
      std::cerr << "Error: wrong parameter '" << aParam << "'.\n";
      return 1;
    }

    // Before all non-boolean flags parsing check if a flag have at least one value.
    if (anIt + 1 >= theArgNum)
    {
      std::cerr << "Error: "<< aParam <<" flag should have value.\n";
      return 1;
    }

    if (aParam.IsEqual ("-type"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);

      theStringParams.Bind ("type", aLocalParam);
    }
    else if (aParam.IsEqual ("-showarrow"))
    {
      TCollection_AsciiString aLocalParam(theArgVec[++anIt]);

      theStringParams.Bind ("showarrow", aLocalParam);
    }
    else
    {
      std::cerr << "Error: unknown parameter '" << aParam << "'.\n";
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : SetAngleDimensionParams
//purpose  : Sets parameters for angle dimension
//=======================================================================
static void SetAngleDimensionParams (const Handle(AIS_Dimension)& theDim,
                                     const NCollection_DataMap<TCollection_AsciiString,
                                     TCollection_AsciiString>& theStringParams)
{
  Handle(AIS_AngleDimension) anAngleDim = Handle(AIS_AngleDimension)::DownCast (theDim);
  if (anAngleDim.IsNull())
  {
    return;
  }

  if (theStringParams.IsBound ("type"))
  {
    AIS_TypeOfAngle anAngleType = AIS_TOA_Interior;
    TCollection_AsciiString anAngleTypeStr = theStringParams.Find ("type");
    if (anAngleTypeStr.IsEqual("interior"))
    {
      anAngleType = AIS_TOA_Interior;
    }
    else if (anAngleTypeStr.IsEqual("exterior"))
    {
      anAngleType = AIS_TOA_Exterior;
    }
    else
    {
      std::cerr << "Error: wrong angle type.\n";
    }
    anAngleDim->SetType(anAngleType);
  }

  if (theStringParams.IsBound ("showarrow"))
  {
    AIS_TypeOfAngleArrowVisibility anArrowType = AIS_TOAV_Both;
    TCollection_AsciiString anArrowTypeStr = theStringParams.Find ("showarrow");
    if (anArrowTypeStr.IsEqual("both"))
    {
      anArrowType = AIS_TOAV_Both;
    }
    else if (anArrowTypeStr.IsEqual("first"))
    {
      anArrowType = AIS_TOAV_First;
    }
    else if (anArrowTypeStr.IsEqual("second"))
    {
      anArrowType = AIS_TOAV_Second;
    }
    else if (anArrowTypeStr.IsEqual("none"))
    {
      anArrowType = AIS_TOAV_None;
    }
    else
    {
      std::cerr << "Error: wrong showarrow type.\n";
    }
    anAngleDim->SetArrowsVisibility(anArrowType);
  }
}

//=======================================================================
//function : VDimBuilder
//purpose  : Command for building dimension presentations: angle,
//           length, radius, diameter
//=======================================================================
static int VDimBuilder (Draw_Interpretor& /*theDi*/,
                        Standard_Integer  theArgsNb,
                        const char**      theArgs)
{
  if (theArgsNb < 2)
  {
    std::cerr << "Error: wrong number of arguments.\n";
    return 1;
  }

  // Parse parameters
  TCollection_AsciiString aName (theArgs[1]);

  NCollection_List<Handle(AIS_InteractiveObject)> aShapes;
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect;
  Standard_Boolean isPlaneCustom = Standard_False;
  gp_Pln aWorkingPlane;

  NCollection_DataMap<TCollection_AsciiString, Standard_Real> aRealParams;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> aStringParams;

  TCollection_AsciiString aDimType(theArgs[2]);
  aDimType.LowerCase();
  AIS_KindOfDimension aKindOfDimension;
  if (aDimType == "-length")
  {
    aKindOfDimension = AIS_KOD_LENGTH;
  }
  else if (aDimType == "-angle")
  {
    aKindOfDimension = AIS_KOD_PLANEANGLE;
  }
  else if (aDimType == "-radius")
  {
    aKindOfDimension = AIS_KOD_RADIUS;
  }
  else if (aDimType == "-diameter" || aDimType == "-diam")
  {
    aKindOfDimension = AIS_KOD_DIAMETER;
  }
  else
  {
    std::cerr << "Error: wrong type of dimension.\n";
    return 1;
  }


  if (ParseDimensionParams (theArgsNb, theArgs, 3,
                            anAspect,isPlaneCustom,aWorkingPlane,
                            aRealParams, aStringParams, &aShapes))
  {
    return 1;
  }

  // Build dimension
  Handle(AIS_Dimension) aDim;
  switch (aKindOfDimension)
  {
    case AIS_KOD_LENGTH:
    {
      if (aShapes.Extent() == 1)
      {
        if (aShapes.First()->Type() == AIS_KOI_Shape
          && (Handle(AIS_Shape)::DownCast(aShapes.First()))->Shape().ShapeType() != TopAbs_EDGE)
        {
          std::cerr << theArgs[0] << ": wrong shape type.\n";
          return 1;
        }
        if (!isPlaneCustom)
        {
          std::cerr << theArgs[0] << ": can not build dimension without working plane.\n";
          return 1;
        }

        // Adjust working plane
        TopoDS_Edge anEdge = TopoDS::Edge ((Handle(AIS_Shape)::DownCast(aShapes.First()))->Shape());
        TopoDS_Vertex aFirst, aSecond;
        TopExp::Vertices (anEdge, aFirst, aSecond);
        aDim = new AIS_LengthDimension (anEdge, aWorkingPlane);

        // Move standard plane (XOY, YOZ or ZOX) to the first point to make it working for dimension
        aWorkingPlane.SetLocation (Handle(AIS_LengthDimension)::DownCast (aDim)->FirstPoint());
      }
      else if (aShapes.Extent() == 2)
      {
        TopoDS_Shape aShape1, aShape2;

        // Getting shapes
        if (aShapes.First()->DynamicType() == STANDARD_TYPE (AIS_Point))
        {
          aShape1 = Handle(AIS_Point)::DownCast (aShapes.First ())->Vertex();
        }
        else if (aShapes.First()->Type() == AIS_KOI_Shape)
        {
          aShape1 = (Handle(AIS_Shape)::DownCast (aShapes.First()))->Shape();
        }

        if (aShapes.Last()->DynamicType() == STANDARD_TYPE (AIS_Point))
        {
          aShape2 = Handle(AIS_Point)::DownCast (aShapes.Last ())->Vertex();
        }
        else if (aShapes.Last()->Type() == AIS_KOI_Shape)
        {
          aShape2 = (Handle(AIS_Shape)::DownCast (aShapes.Last()))->Shape();
        }

        if (aShape1.IsNull() || aShape2.IsNull())
        {
          std::cerr << theArgs[0] << ": wrong shape type.\n";
          return 1;
        }

        // Face-Face case
        if (aShape1.ShapeType() == TopAbs_FACE && aShape2.ShapeType() == TopAbs_FACE)
        {
          aDim = new AIS_LengthDimension (TopoDS::Face (aShape1), TopoDS::Face (aShape2));
        }
        else if (aShape1.ShapeType() == TopAbs_FACE && aShape2.ShapeType() == TopAbs_EDGE)
        {
          aDim = new AIS_LengthDimension (TopoDS::Face (aShape1), TopoDS::Edge (aShape2));
        }
        else if (aShape1.ShapeType() == TopAbs_EDGE && aShape2.ShapeType() == TopAbs_FACE)
        {
          aDim = new AIS_LengthDimension (TopoDS::Face (aShape2), TopoDS::Edge (aShape1));
        }
        else
        {
          if (!isPlaneCustom)
          {
            std::cerr << theArgs[0] << ": can not build dimension without working plane.\n";
            return 1;
          }
          // Vertex-Vertex case
          if (aShape1.ShapeType() == TopAbs_VERTEX)
          {
            aWorkingPlane.SetLocation (BRep_Tool::Pnt (TopoDS::Vertex (aShape1)));
          }
          else if (aShape2.ShapeType() == TopAbs_VERTEX)
          {
            aWorkingPlane.SetLocation (BRep_Tool::Pnt (TopoDS::Vertex (aShape2)));
          }

          aDim = new AIS_LengthDimension (aShape1, aShape2, aWorkingPlane);
        }
      }
      else
      {
        std::cerr << theArgs[0] << ": wrong number of shapes to build dimension.\n";
        return 1;
      }

      break;
    }
    case AIS_KOD_PLANEANGLE:
    {
      if (aShapes.Extent() == 1 && aShapes.First()->Type()==AIS_KOI_Shape)
      {
        Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aShapes.First());
        if (aShape->Shape().ShapeType() == TopAbs_FACE)
          aDim = new AIS_AngleDimension (TopoDS::Face(aShape->Shape()));
      }
      if (aShapes.Extent() == 2)
      {
        Handle(AIS_Shape) aShape1 = Handle(AIS_Shape)::DownCast(aShapes.First());
        Handle(AIS_Shape) aShape2 = Handle(AIS_Shape)::DownCast(aShapes.Last());
        if (!aShape1.IsNull() && !aShape2.IsNull()
          && aShape1->Shape().ShapeType() == TopAbs_EDGE
          && aShape2->Shape().ShapeType() == TopAbs_EDGE)
          aDim = new AIS_AngleDimension (TopoDS::Edge(aShape1->Shape()),TopoDS::Edge(aShape2->Shape()));
        else
        {
          std::cerr << theArgs[0] << ": wrong shapes for angle dimension.\n";
          return 1;
        }
      }
      else if (aShapes.Extent() == 3)
      {
        gp_Pnt aP1, aP2, aP3;
        Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast (aShapes.First());
        if (aPoint.IsNull())
          return 1;
        aP1 = aPoint->Component()->Pnt();
        aShapes.RemoveFirst();
        aPoint = Handle(AIS_Point)::DownCast (aShapes.First());
        if (aPoint.IsNull())
          return 1;
        aP2 = aPoint->Component()->Pnt();
        aShapes.RemoveFirst();
        aPoint = Handle(AIS_Point)::DownCast (aShapes.First());
        if (aPoint.IsNull())
          return 1;
        aP3 = aPoint->Component()->Pnt();
        aDim = new AIS_AngleDimension (aP1, aP2, aP3);
      }
      else
      {
        std::cerr << theArgs[0] << ": wrong number of shapes to build dimension.\n";
        return 1;
      }

      break;
    }
    case AIS_KOD_RADIUS: // radius of the circle
    {
      gp_Pnt anAnchor;
      bool hasAnchor = false;
      for (NCollection_List<Handle(AIS_InteractiveObject)>::Iterator aShapeIter (aShapes); aShapeIter.More(); aShapeIter.Next())
      {
        if (Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(aShapeIter.Value()))
        {
          hasAnchor = true;
          anAnchor = aPoint->Component()->Pnt();
          aShapes.Remove (aShapeIter);
          break;
        }
      }
      if (aShapes.Extent() != 1)
      {
        std::cout << "Syntax error: wrong number of shapes to build dimension.\n";
        return 1;
      }

      if (Handle(AIS_Circle) aShapeCirc = Handle(AIS_Circle)::DownCast(aShapes.First()))
      {
        gp_Circ aCircle = aShapeCirc->Circle()->Circ();
        if (hasAnchor)
        {
          aDim = new AIS_RadiusDimension (aCircle, anAnchor);
        }
        else
        {
          aDim = new AIS_RadiusDimension (aCircle);
        }
      }
      else if (Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aShapes.First()))
      {
        Handle(AIS_RadiusDimension) aRadDim = new AIS_RadiusDimension (aShape->Shape());
        if (hasAnchor)
        {
          aRadDim->SetMeasuredGeometry (aShape->Shape(), anAnchor);
        }
        aDim = aRadDim;
      }
      else
      {
        std::cout << "Error: shape for radius has wrong type.\n";
        return 1;
      }
      break;
    }
    case AIS_KOD_DIAMETER:
    {
      if (aShapes.Extent() == 1)
      {
        if (aShapes.First()->DynamicType() == STANDARD_TYPE(AIS_Circle))
        {
          Handle(AIS_Circle) aShape = Handle(AIS_Circle)::DownCast (aShapes.First());
          gp_Circ aCircle = aShape->Circle()->Circ();
          aDim = new AIS_DiameterDimension (aCircle);
        }
        else
        {
          Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (aShapes.First());
          if (aShape.IsNull())
          {
            std::cerr << "Error: shape for radius is of wrong type.\n";
            return 1;
          }
          aDim = new AIS_DiameterDimension (aShape->Shape());
        }
      }
      else
      {
        std::cerr << theArgs[0] << ": wrong number of shapes to build dimension.\n";
        return 1;
      }

      break;
    }
    default:
    {
      std::cerr << theArgs[0] << ": wrong type of dimension. Type help for more information.\n";
      return 1;
    }
  }

  // Check dimension geometry
  if (!aDim->IsValid())
  {
    std::cerr << theArgs[0] << ":dimension geometry is invalid, " << aDimType.ToCString()
      << " dimension can't be built on input shapes.\n";
    return 1;
  }

  aDim->SetDimensionAspect (anAspect);

  SetDimensionParams (aDim, aRealParams, aStringParams);

  VDisplayAISObject (aName,aDim);

  return 0;
}

namespace
{
  //! If the given shapes are edges then check whether they are parallel else return true.
  Standard_Boolean IsParallel (const TopoDS_Shape& theShape1,
                               const TopoDS_Shape& theShape2)
  {
    if (theShape1.ShapeType() == TopAbs_EDGE
     && theShape2.ShapeType() == TopAbs_EDGE)
    {
      BRepExtrema_ExtCC aDelta (TopoDS::Edge (theShape1),
                                TopoDS::Edge (theShape2));
      return aDelta.IsParallel();
    }

    return Standard_True;
  }
}
//=======================================================================
//function : VRelationBuilder
//purpose  : Command for building realation presentation
//=======================================================================
static int VRelationBuilder (Draw_Interpretor& /*theDi*/,
                             Standard_Integer  theArgsNb,
                             const char**      theArgs)
{
  if (theArgsNb < 2)
  {
    std::cerr << "Error: wrong number of arguments.\n";
    return 1;
  }

  TCollection_AsciiString aName (theArgs[1]);
  TCollection_AsciiString aType (theArgs[2]);

  AIS_KindOfRelation aKindOfRelation = AIS_KOR_NONE;
  if (aType == "-concentric")
  {
    aKindOfRelation = AIS_KOR_CONCENTRIC;
  }
  else if (aType == "-equaldistance")
  {
    aKindOfRelation = AIS_KOR_EQUALDISTANCE;
  }
  else if (aType == "-equalradius")
  {
    aKindOfRelation = AIS_KOR_EQUALRADIUS;
  }
  else if (aType == "-fix")
  {
    aKindOfRelation = AIS_KOR_FIX;
  }
  else if (aType == "-identic")
  {
    aKindOfRelation = AIS_KOR_IDENTIC;
  }
  else if (aType == "-offset")
  {
    aKindOfRelation = AIS_KOR_OFFSET;
  }
  else if (aType == "-parallel")
  {
    aKindOfRelation = AIS_KOR_PARALLEL;
  }
  else if (aType == "-perpendicular")
  {
    aKindOfRelation = AIS_KOR_PERPENDICULAR;
  }
  else if (aType == "-tangent")
  {
    aKindOfRelation = AIS_KOR_TANGENT;
  }
  else if (aType == "-symmetric")
  {
    aKindOfRelation = AIS_KOR_SYMMETRIC;
  }

  TopTools_ListOfShape aShapes;
  ViewerTest::GetSelectedShapes (aShapes);

  // Build relation.
  Handle(AIS_Relation) aRelation;
  switch (aKindOfRelation)
  {
    case AIS_KOR_CONCENTRIC:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShape1 = aShapes.First();
      const TopoDS_Shape& aShape2 = aShapes.Last();

      if (!(aShape1.ShapeType() == TopAbs_EDGE
         && aShape2.ShapeType() == TopAbs_EDGE))
      {
        std::cerr << "Syntax error: selected shapes are not edges.\n";
        return 1;
      }

      BRepAdaptor_Curve aCurve1 (TopoDS::Edge (aShape1));
      gp_Circ           aCircle1 = aCurve1.Circle();
      gp_Pnt            aCenter1 = aCircle1.Location();
      gp_Pnt            B = aCurve1.Value (0.25);
      gp_Pnt            C = aCurve1.Value (0.75);
      GC_MakePlane      aMkPlane (aCenter1, B, C);

      aRelation = new AIS_ConcentricRelation (aShape1, aShape2, aMkPlane.Value());

      break;
    }

    case AIS_KOR_EQUALDISTANCE:
    {
      if (aShapes.Extent() != 4)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      TopoDS_Shape aSelectedShapes[4];

      Standard_Integer anIdx = 0;
      TopTools_ListOfShape::Iterator anIter (aShapes);
      for (; anIter.More(); anIter.Next(), ++anIdx)
      {
        aSelectedShapes[anIdx] = anIter.Value();
      }

      if (!IsParallel (aSelectedShapes[0], aSelectedShapes[1])
       || !IsParallel (aSelectedShapes[2], aSelectedShapes[3]))
      {
        std::cerr << "Syntax error: non parallel edges.\n";
        return 1;
      }

      gp_Pnt A, B, C;
      if (aSelectedShapes[0].ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Vertex Va, Vb;
        TopExp::Vertices (TopoDS::Edge (aSelectedShapes[0]), Va, Vb);
        A = BRep_Tool::Pnt (Va);
        B = BRep_Tool::Pnt (Vb);

        if (aSelectedShapes[1].ShapeType() == TopAbs_EDGE)
        {
          TopoDS_Vertex Vc, Vd;
          TopExp::Vertices (TopoDS::Edge (aSelectedShapes[1]), Vc, Vd);
          C = BRep_Tool::Pnt (Vc);
        }
        else
        {
          C = BRep_Tool::Pnt (TopoDS::Vertex (aSelectedShapes[1]));
        }
      }
      else
      {
        A = BRep_Tool::Pnt (TopoDS::Vertex (aSelectedShapes[0]));

        if (aSelectedShapes[1].ShapeType() == TopAbs_EDGE)
        {
          TopoDS_Vertex Vb, Vc;
          TopExp::Vertices (TopoDS::Edge (aSelectedShapes[1]), Vb, Vc);
          B = BRep_Tool::Pnt (Vb);
          C = BRep_Tool::Pnt (Vc);

        }
        else
        {
          B = BRep_Tool::Pnt (TopoDS::Vertex (aSelectedShapes[1]));
          C.SetX (B.X() + 5.0);
          C.SetY (B.Y() + 5.0);
          C.SetZ (B.Z() + 5.0);

        }
      }

      GC_MakePlane aMkPlane (A, B, C);
      aRelation = new AIS_EqualDistanceRelation (aSelectedShapes[0],
                                                 aSelectedShapes[1],
                                                 aSelectedShapes[2],
                                                 aSelectedShapes[3],
                                                 aMkPlane.Value());

      break;
    }

    case AIS_KOR_EQUALRADIUS:
    {
      if (aShapes.Extent() != 2 && aShapes.Extent() != 1)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShape1 = aShapes.First();
      const TopoDS_Shape& aShape2 = (aShapes.Extent() == 2) ? aShapes.Last() : aShape1;
      if (!(aShape1.ShapeType() == TopAbs_EDGE
         && aShape2.ShapeType() == TopAbs_EDGE))
      {
        std::cerr << "Syntax error: selected shapes are not edges.\n";
        return 1;
      }

      TopoDS_Edge       anEdge1 = TopoDS::Edge (aShape1);
      TopoDS_Edge       anEdge2 = TopoDS::Edge (aShape2);
      BRepAdaptor_Curve aCurve1 (anEdge1);
      gp_Pnt            A = aCurve1.Value (0.1);
      gp_Pnt            B = aCurve1.Value (0.5);
      gp_Pnt            C = aCurve1.Value (0.9);
      GC_MakePlane      aMkPlane (A, B, C);

      aRelation = new AIS_EqualRadiusRelation (anEdge1, anEdge2, aMkPlane.Value());
      break;
    }

    case AIS_KOR_FIX:
    {
      if (aShapes.Extent() != 1)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShape = aShapes.First();
      if (aShape.ShapeType() != TopAbs_EDGE)
      {
        std::cerr << "Syntax error: selected shapes are not edges.\n";
        return 1;
      }

      TopoDS_Edge anEdge = TopoDS::Edge (aShape);
      BRepAdaptor_Curve aCurve (anEdge);
      gp_Pnt A = aCurve.Value(0.1);
      gp_Pnt B = aCurve.Value(0.5);
      gp_Pnt D = aCurve.Value(0.9);
      gp_Pnt C (B.X() + 5.0, B.Y() + 5.0, B.Z() + 5.0);
      GC_MakePlane aMkPlane (A, D, C);

      aRelation = new AIS_FixRelation (anEdge, aMkPlane.Value());
      break;
    }

    case AIS_KOR_IDENTIC:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();

      gp_Pnt A,B,C;
      if (aShapeA.ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Edge anEdgeA = TopoDS::Edge (aShapeA);
        BRepAdaptor_Curve aCurveA (anEdgeA);

        A = aCurveA.Value (0.1);
        B = aCurveA.Value (0.9);
        C.SetX (B.X() + 5.0);
        C.SetY (B.Y() + 5.0);
        C.SetZ (B.Z() + 5.0);
      }
      else if (aShapeA.ShapeType() == TopAbs_VERTEX)
      {
        if (aShapeB.ShapeType() == TopAbs_EDGE)
        {
          TopoDS_Edge anEdgeB = TopoDS::Edge (aShapeB);
          BRepAdaptor_Curve aCurveB (anEdgeB);

          A = aCurveB.Value (0.1);
          B = aCurveB.Value (0.9);
          C.SetX (B.X() + 5.0);
          C.SetY (B.Y() + 5.0);
          C.SetZ (B.Z() + 5.0);
        }
        else if (aShapeB.ShapeType() == TopAbs_FACE)
        {
          TopoDS_Face aFaceB = TopoDS::Face (aShapeB);
          TopExp_Explorer aFaceExp (aFaceB, TopAbs_EDGE);
          TopoDS_Edge anEdgeFromB = TopoDS::Edge (aFaceExp.Current());
          BRepAdaptor_Curve aCurveB (anEdgeFromB);
          A = aCurveB.Value (0.1);
          B = aCurveB.Value (0.5);
          C = aCurveB.Value (0.9);
        }
        else
        {
          A = BRep_Tool::Pnt (TopoDS::Vertex (aShapeA));
          B = BRep_Tool::Pnt (TopoDS::Vertex (aShapeB));
          C.SetX (B.X() + 5.0);
          C.SetY (B.Y() + 5.0);
          C.SetZ (B.Z() + 5.0);
        }
      }
      else
      {
        TopoDS_Face aFaceA = TopoDS::Face (aShapeA);
        TopExp_Explorer aFaceExp (aFaceA, TopAbs_EDGE);
        TopoDS_Edge anEdgeFromA = TopoDS::Edge (aFaceExp.Current());
        BRepAdaptor_Curve aCurveA (anEdgeFromA);
        A = aCurveA.Value (0.1);
        B = aCurveA.Value (0.5);
        C = aCurveA.Value (0.9);
      }

      GC_MakePlane aMkPlane (A ,B ,C);
      aRelation = new AIS_IdenticRelation (aShapeA, aShapeB, aMkPlane.Value());
      break;
    }

    case AIS_KOR_OFFSET:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShape1 = aShapes.First();
      const TopoDS_Shape& aShape2 = aShapes.Last();
      if (!(aShape1.ShapeType() == TopAbs_FACE
         && aShape2.ShapeType() == TopAbs_FACE))
      {
        std::cerr << "Syntax error: selected shapes are not faces.\n";
        return 1;
      }

      TopoDS_Face aFace1 = TopoDS::Face (aShape1);
      TopoDS_Face aFace2 = TopoDS::Face (aShape2);

      BRepExtrema_ExtFF aDelta (aFace1, aFace2);
      if (!aDelta.IsParallel())
      {
        std::cerr << "Syntax error: the faces are not parallel.\n";
        return 1;
      }

      Standard_Real aDist = Round (sqrt (aDelta.SquareDistance (1)) * 10.0) / 10.0;
      TCollection_ExtendedString aMessage (TCollection_ExtendedString ("offset=") + TCollection_ExtendedString (aDist));

      aRelation = new AIS_OffsetDimension (aFace1, aFace2, aDist, aMessage);

      break;
    }

    case AIS_KOR_PARALLEL:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();
      if (aShapeA.ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Edge anEdgeA = TopoDS::Edge (aShapeA);
        TopoDS_Edge anEdgeB = TopoDS::Edge (aShapeB);
        BRepExtrema_ExtCC aDeltaEdge (anEdgeA, anEdgeB);

        if (!aDeltaEdge.IsParallel())
        {
          std::cerr << "Error: the edges are not parallel.\n";
          return 1;
        }

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);

        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A, B, C);

        aRelation = new AIS_ParallelRelation (anEdgeA, anEdgeB, aMkPlane.Value());
      }
      else
      {
        TopoDS_Face aFaceA = TopoDS::Face (aShapeA);
        TopoDS_Face aFaceB = TopoDS::Face (aShapeB);

        BRepExtrema_ExtFF aDeltaFace (aFaceA, aFaceB);
        if (!aDeltaFace.IsParallel())
        {
          std::cerr << "Error: the faces are not parallel.\n";
          return 1;
        }

        TopExp_Explorer aFaceExpA (aFaceA, TopAbs_EDGE);
        TopExp_Explorer aFaceExpB (aFaceB, TopAbs_EDGE);

        TopoDS_Edge anEdgeA = TopoDS::Edge (aFaceExpA.Current());
        TopoDS_Edge anEdgeB = TopoDS::Edge (aFaceExpB.Current());

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);
        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A, B, C);

        aRelation = new AIS_ParallelRelation (aFaceA, aFaceB, aMkPlane.Value());
      }
      break;
    }

    case AIS_KOR_PERPENDICULAR:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();

      if (aShapeA.ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Edge anEdgeA = TopoDS::Edge (aShapeA);
        TopoDS_Edge anEdgeB = TopoDS::Edge (aShapeB);

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);

        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A, B, C);

        aRelation = new AIS_PerpendicularRelation (anEdgeA, anEdgeB, aMkPlane.Value());
      }
      else
      {
        TopoDS_Face aFaceA = TopoDS::Face (aShapeA);
        TopoDS_Face aFaceB = TopoDS::Face (aShapeB);

        TopExp_Explorer aFaceExpA (aFaceA, TopAbs_EDGE);
        TopExp_Explorer aFaceExpB (aFaceB, TopAbs_EDGE);

        TopoDS_Edge anEdgeA = TopoDS::Edge (aFaceExpA.Current());
        TopoDS_Edge anEdgeB = TopoDS::Edge (aFaceExpB.Current());

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);

        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A, B, C);

        aRelation = new AIS_PerpendicularRelation (aFaceA, aFaceB);
      }

      break;
    }

    case AIS_KOR_TANGENT:
    {
      if (aShapes.Extent() != 2)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      const TopoDS_Shape& aShapeA = aShapes.First();
      const TopoDS_Shape& aShapeB = aShapes.Last();

      if (aShapeA.ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Edge anEdgeA = TopoDS::Edge (aShapeA);
        TopoDS_Edge anEdgeB = TopoDS::Edge (aShapeB);

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);
    
        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A,B,C);

        aRelation = new AIS_TangentRelation (anEdgeA, anEdgeB, aMkPlane.Value());
      }
      else
      {
        TopoDS_Face aFaceA = TopoDS::Face (aShapeA);
        TopoDS_Face aFaceB = TopoDS::Face (aShapeB);

        TopExp_Explorer aFaceExpA (aFaceA, TopAbs_EDGE);
        TopExp_Explorer aFaceExpB (aFaceB, TopAbs_EDGE);
    
        TopoDS_Edge anEdgeA = TopoDS::Edge (aFaceExpA.Current());
        TopoDS_Edge anEdgeB = TopoDS::Edge (aFaceExpB.Current());

        BRepAdaptor_Curve aCurveA (anEdgeA);
        BRepAdaptor_Curve aCurveB (anEdgeB);

        gp_Pnt A = aCurveA.Value (0.1);
        gp_Pnt B = aCurveA.Value (0.9);
        gp_Pnt C = aCurveB.Value (0.5);

        GC_MakePlane aMkPlane (A,B,C);

        aRelation = new AIS_TangentRelation (aFaceA, aFaceB, aMkPlane.Value());
      }
      break;
    }

    case AIS_KOR_SYMMETRIC:
    {
      if (aShapes.Extent() != 3)
      {
        std::cerr << "Error: Wrong number of selected shapes.\n";
        return 1;
      }

      TopoDS_Shape aSelectedShapes[3];
      Standard_Integer anIdx = 0;
      TopTools_ListOfShape::Iterator anIter (aShapes);
      for (; anIter.More(); anIter.Next(), ++anIdx)
      {
        aSelectedShapes[anIdx] = anIter.Value();
      }

      TopoDS_Edge anEdgeA = TopoDS::Edge (aSelectedShapes[0]);
      if (aSelectedShapes[1].ShapeType() == TopAbs_EDGE)
      {
        // 1 - edge,  2 - edge, 3 - edge.
        TopoDS_Edge anEdgeB = TopoDS::Edge (aSelectedShapes[1]);
        TopoDS_Edge anEdgeC = TopoDS::Edge (aSelectedShapes[2]);

        BRepExtrema_ExtCC aDeltaEdgeAB (anEdgeA, anEdgeB);
        BRepExtrema_ExtCC aDeltaEdgeAC (anEdgeA, anEdgeC);

        if (!aDeltaEdgeAB.IsParallel())
        {
          std::cerr << "Syntax error: the edges are not parallel.\n";
          return 1;
        }
        if (!aDeltaEdgeAC.IsParallel())
        {
          std::cerr << "Syntax error: the edges are not parallel.\n";
          return 1;
        }

        TopoDS_Vertex Va, Vb, Vc, Vd;
        TopExp::Vertices (anEdgeB, Va, Vb);
        TopExp::Vertices (anEdgeC, Vc, Vd);
        gp_Pnt A = BRep_Tool::Pnt (Va);
        gp_Pnt B = BRep_Tool::Pnt (Vc);
        gp_Pnt C = Get3DPointAtMousePosition();

        GC_MakePlane aMkPlane (A, B, C);

        aRelation = new AIS_SymmetricRelation (anEdgeA, anEdgeB, anEdgeC, aMkPlane.Value());
      }
      else
      {
        // 1 - edge, 2 - vertex, 3 - vertex
        TopoDS_Vertex aVertexB = TopoDS::Vertex (aSelectedShapes[1]);
        TopoDS_Vertex aVertexC = TopoDS::Vertex (aSelectedShapes[2]);

        gp_Pnt B = BRep_Tool::Pnt (aVertexB);
        gp_Pnt C = BRep_Tool::Pnt (aVertexC);

        TopoDS_Vertex Va, Vb;
        TopExp::Vertices (anEdgeA, Va, Vb);
        gp_Pnt A = BRep_Tool::Pnt (Va);

        GC_MakePlane aMkPlane(A, B, C);
        aRelation = new AIS_SymmetricRelation (anEdgeA, aVertexB, aVertexC, aMkPlane.Value());
      }

      break;
    }

    case AIS_KOR_NONE:
    {
      std::cerr << "Error: Unknown type of relation!\n";
      return 1;
    }
  }

  VDisplayAISObject (aName, aRelation);
  return 0;
}

//=======================================================================
//function : VDimParam
//purpose  : Sets aspect parameters to dimension.
//=======================================================================
static int VDimParam (Draw_Interpretor& theDi, Standard_Integer theArgNum, const char** theArgVec)
{
  if (theArgNum < 3)
  {
    theDi << theArgVec[0] << " error: the wrong number of input parameters.\n";
    return 1;
  }


  TCollection_AsciiString aName (theArgVec[1]);
  gp_Pln aWorkingPlane;
  Standard_Boolean isCustomPlane = Standard_False;
  Standard_Boolean toUpdate = Standard_True;

  NCollection_DataMap<TCollection_AsciiString, Standard_Real> aRealParams;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> aStringParams;

  if (!GetMapOfAIS().IsBound2 (aName))
  {
    theDi << theArgVec[0] << "error: no object with this name.\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2 (aName));
  if (anObject->Type() != AIS_KOI_Dimension)
  {
    theDi << theArgVec[0] << "error: no dimension with this name.\n";
    return 1;
  }

  Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast (anObject);
  Handle(Prs3d_DimensionAspect) anAspect = aDim->DimensionAspect();

  if (ParseDimensionParams (theArgNum, theArgVec, 2, anAspect,
                            isCustomPlane, aWorkingPlane,
                            aRealParams, aStringParams))
  {
    return 1;
  }

  if (isCustomPlane)
  {
    aDim->SetCustomPlane (aWorkingPlane);
  }

  SetDimensionParams (aDim, aRealParams, aStringParams);

  if (!aDim->IsValid())
  {
    std::cerr << "Error: Dimension geometry or plane is not valid.\n";
    return 1;
  }

  // Redisplay a dimension after parameter changing.
  if (ViewerTest::GetAISContext()->IsDisplayed (aDim))
  {
    ViewerTest::GetAISContext()->Redisplay (aDim, toUpdate);
  }

  return 0;
}

//=======================================================================
//function : VLengthParam
//purpose  : Sets parameters to length dimension.
//=======================================================================
static int VLengthParam (Draw_Interpretor&, Standard_Integer theArgNum, const char** theArgVec)
{
  if (theArgNum < 3)
  {
    std::cout << theArgVec[0] << " error: the wrong number of input parameters.\n";
    return 1;
  }

  TCollection_AsciiString aName (theArgVec[1]);
  if (!GetMapOfAIS().IsBound2 (aName))
  {
    std::cout << theArgVec[0] << "error: no object with this name.\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2 (aName));
  if (anObject->Type() != AIS_KOI_Dimension)
  {
    std::cout << theArgVec[0] << "error: no dimension with this name.\n";
    return 1;
  }
  
  Handle(AIS_LengthDimension) aLengthDim = Handle(AIS_LengthDimension)::DownCast (anObject);
  if (aLengthDim.IsNull())
  {
    std::cout << theArgVec[0] << "error: no length dimension with this name.\n";
    return 1;
  }

  // parse direction value
  gp_Dir aDirection;
  int anArgumentIt = 2;
  TCollection_AsciiString aParam (theArgVec[anArgumentIt]);
  aParam.LowerCase();

  bool isCustomDirection = false;
  if (aParam.IsEqual ("-direction"))
  {
    if (anArgumentIt + 1 >= theArgNum)
    {
      std::cout << "Error: "<< aParam <<" direction should have value.\n";
      return 1;
    }
    anArgumentIt++;
    isCustomDirection = Standard_True;
    TCollection_AsciiString aValue = theArgVec[anArgumentIt];
    aValue.LowerCase();
    if (aValue == "ox")
      aDirection = gp::DX();
    else if (aValue == "oy")
      aDirection = gp::DY();
    else if (aValue == "oz")
      aDirection = gp::DZ();
    else if (aValue == "autodirection")
      isCustomDirection = false;
    else
    {
      if (anArgumentIt + 2 >= theArgNum)
      {
        std::cout << "Error: wrong number of values for parameter '" << aParam << "'.\n";
        return 1;
      }
      // access coordinate arguments
      TColStd_SequenceOfReal aCoords;
      for (; anArgumentIt < theArgNum; ++anArgumentIt)
      {
        TCollection_AsciiString anArg (theArgVec[anArgumentIt]);
        if (!anArg.IsRealValue())
        {
          break;
        }
        aCoords.Append (anArg.RealValue());
      }
      // non-numeric argument too early
      if (aCoords.IsEmpty() || aCoords.Size() != 3)
      {
        std::cout << "Error: wrong number of direction arguments.\n";
        return 1;
      }
      aDirection = gp_Dir (aCoords.Value (1), aCoords.Value (2), aCoords.Value (3));
    }
  }

  aLengthDim->SetDirection (aDirection, isCustomDirection);
  if (!aLengthDim->IsValid())
  {
    std::cout << "Error: Dimension geometry or plane is not valid.\n";
    return 1;
  }

  // Redisplay a dimension after parameter changing.
  if (ViewerTest::GetAISContext()->IsDisplayed (aLengthDim))
  {
    ViewerTest::GetAISContext()->Redisplay (aLengthDim, true);
  }

  return 0;
}

//=======================================================================
//function : VAngleParam
//purpose  : Sets aspect parameters to angle dimension.
//=======================================================================
static int VAngleParam (Draw_Interpretor& theDi, Standard_Integer theArgNum, const char** theArgVec)
{
  if (theArgNum < 3)
  {
    theDi << theArgVec[0] << " error: the wrong number of input parameters.\n";
    return 1;
  }


  TCollection_AsciiString aName (theArgVec[1]);
  gp_Pln aWorkingPlane;
  Standard_Boolean toUpdate = Standard_True;

  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> aStringParams;

  if (!GetMapOfAIS().IsBound2 (aName))
  {
    theDi << theArgVec[0] << "error: no object with this name.\n";
    return 1;
  }

  Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast(GetMapOfAIS().Find2 (aName));
  if (anObject->Type() != AIS_KOI_Dimension)
  {
    theDi << theArgVec[0] << "error: no dimension with this name.\n";
    return 1;
  }

  Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast (anObject);
  Handle(Prs3d_DimensionAspect) anAspect = aDim->DimensionAspect();

  if (ParseAngleDimensionParams (theArgNum, theArgVec, 2, aStringParams))
  {
    return 1;
  }

  SetAngleDimensionParams (aDim, aStringParams);

  if (!aDim->IsValid())
  {
    std::cerr << "Error: Dimension geometry or plane is not valid.\n";
    return 1;
  }

  // Redisplay a dimension after parameter changing.
  if (ViewerTest::GetAISContext()->IsDisplayed (aDim))
  {
    ViewerTest::GetAISContext()->Redisplay (aDim, toUpdate);
  }
  return 0;
}

//=======================================================================
//function : VMoveDim
//purpose  : Moves dimension or relation text label to defined or picked
//           position and updates the object.
//draw args: vmovedim [name] [x y z]
//=======================================================================
static int VMoveDim (Draw_Interpretor& theDi, Standard_Integer theArgNum, const char** theArgVec) 
{
  if (theArgNum > 5)
  {
    theDi << theArgVec[0] << " error: the wrong number of parameters.\n";
    return 1;
  }

  // Parameters parsing
  Standard_Boolean isNameSet = (theArgNum ==2 || theArgNum == 5);
  Standard_Boolean isPointSet = (theArgNum == 4 || theArgNum == 5);

  Handle(AIS_InteractiveObject) aPickedObj;
  gp_Pnt aPoint (gp::Origin());
  Standard_Integer aMaxPickNum = 5;

  // Find object
  if (isNameSet)
  {
     TCollection_AsciiString aName (theArgVec[1]);
     if (!GetMapOfAIS().IsBound2 (aName))
     {
       theDi << theArgVec[0] << " error: no object with this name.\n";
       return 1;
     }

     aPickedObj = Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (aName));
     
     if (aPickedObj.IsNull())
     {
       theDi << theArgVec[0] << " error: the object with this name is not valid.\n";
       return 1;
     }

     if (aPickedObj->Type() != AIS_KOI_Dimension && aPickedObj->Type() != AIS_KOI_Relation)
     {
       theDi << theArgVec[0] << " error: no dimension or relation with this name.\n";
       return 1;
     }
  }
  else // Pick dimension or relation
  {
    // Loop that will be handle picking.
    Standard_Integer anArgNum = 5;
    const char *aBuffer[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
    const char **anArgVec = (const char **) aBuffer;

    Standard_Boolean isPicked = Standard_False;
    Standard_Integer aPickNum = 0;
    while (!isPicked && aPickNum < aMaxPickNum)
    {
      while (ViewerMainLoop (anArgNum, anArgVec)) { }

      for (TheAISContext()->InitSelected(); TheAISContext()->MoreSelected(); TheAISContext()->NextSelected())
      {
        aPickedObj = TheAISContext()->SelectedInteractive();
      }

      isPicked = (!aPickedObj.IsNull() && (aPickedObj->Type() == AIS_KOI_Dimension || aPickedObj->Type() == AIS_KOI_Relation));

      if (isPicked)
      {
        break;
      }
      aPickNum++;
    }
    if (!isPicked)
    {
      theDi << theArgVec[0] << ": no dimension or relation is selected.\n";
      return 1;
    }
  }

  // Find point
  if (isPointSet)
  {
    aPoint = theArgNum == 4 ? gp_Pnt (atoi (theArgVec[1]), atoi (theArgVec[2]), atoi (theArgVec[3]))
                            : gp_Pnt (atoi (theArgVec[2]), atoi (theArgVec[3]), atoi (theArgVec[4]));
  }
  else // Pick the point
  {
    Standard_Integer aPickArgNum = 5;
    const char *aPickBuff[] = {"VPick", "X", "VPickY", "VPickZ", "VPickShape"};
    const char **aPickArgVec = (const char **) aPickBuff;

    while (ViewerMainLoop (aPickArgNum, aPickArgVec)) { }

    // Set text position, update relation or dimension.
    if (aPickedObj->Type() == AIS_KOI_Relation)
    {
      Handle(AIS_Relation) aRelation = Handle(AIS_Relation)::DownCast (aPickedObj);
      aPoint = Get3DPointAtMousePosition();
      aRelation->SetPosition (aPoint);
      TheAISContext()->Redisplay (aRelation, Standard_True);
    }
    else
    {
      Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast (aPickedObj);
      gp_Pnt aFirstPoint, aSecondPoint;
      if (aDim->KindOfDimension() == AIS_KOD_PLANEANGLE)
      {
        Handle(AIS_AngleDimension) anAngleDim = Handle(AIS_AngleDimension)::DownCast (aDim);
        aFirstPoint = anAngleDim->FirstPoint();
        aSecondPoint = anAngleDim->SecondPoint();
      }
      else if (aDim->KindOfDimension() == AIS_KOD_LENGTH)
      {
        Handle(AIS_LengthDimension) aLengthDim = Handle(AIS_LengthDimension)::DownCast (aDim);
        aFirstPoint = aLengthDim->FirstPoint();
        aSecondPoint = aLengthDim->SecondPoint();
      }
      else if (aDim->KindOfDimension() == AIS_KOD_RADIUS)
      {
        Handle(AIS_RadiusDimension) aRadiusDim = Handle(AIS_RadiusDimension)::DownCast (aDim);
        aFirstPoint = aRadiusDim->AnchorPoint();
        aSecondPoint = aRadiusDim->Circle().Location();
      }
      else if (aDim->KindOfDimension() == AIS_KOD_DIAMETER)
      {
        Handle(AIS_DiameterDimension) aDiameterDim = Handle(AIS_DiameterDimension)::DownCast (aDim);
        aFirstPoint = aDiameterDim->AnchorPoint();
        aSecondPoint = aDiameterDim->Circle().Location();
      }

      if (!Get3DPointAtMousePosition (aFirstPoint, aSecondPoint, aPoint))
      {
        return 1;
      }

      aDim->SetTextPosition (aPoint);
      TheAISContext()->Redisplay (aDim, Standard_True);
    }

  }

  // Set text position, update relation or dimension.
  if (aPickedObj->Type() == AIS_KOI_Relation)
  {
    Handle(AIS_Relation) aRelation = Handle(AIS_Relation)::DownCast (aPickedObj);
    aRelation->SetPosition (aPoint);
    TheAISContext()->Redisplay (aRelation, Standard_True);
  }
  else
  {
    Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast (aPickedObj);
    aDim->SetTextPosition (aPoint);
    TheAISContext()->Redisplay (aDim, Standard_True);
  }

  return 0;
}

//=======================================================================
//function : RelationsCommands
//purpose  : 
//=======================================================================
      

void ViewerTest::RelationCommands(Draw_Interpretor& theCommands)
{
  const char *group = "AISRelations";

  theCommands.Add("vdimension",
      "vdimension name {-angle|-length|-radius|-diameter}"
      "[-shapes shape1 [shape2 [shape3]]\n"
      "[-selected]\n"
      "[-text 3d|2d wf|sh|wireframe|shading IntegerSize]\n"
      "[-font FontName]\n"
      "[-label left|right|hcenter|hfit top|bottom|vcenter|vfit]\n"
      "[-arrow external|internal|fit]\n"
      "[{-arrowlength|-arlen} RealArrowLength]\n"
      "[{-arrowangle|-arangle} ArrowAngle(degrees)]\n"
      "[-plane xoy|yoz|zox]\n"
      "[-flyout FloatValue -extension FloatValue]\n"
      "[-autovalue]\n"
      "[-value CustomRealValue]\n"
      "[-textvalue CustomTextValue]\n"
      "[-dispunits DisplayUnitsString]\n"
      "[-modelunits ModelUnitsString]\n"
      "[-showunits | -hideunits]\n"
      " -Builds angle, length, radius and diameter dimensions.\n"
      " -See also: vdimparam, vmovedim.\n",
      __FILE__,VDimBuilder,group);

  theCommands.Add ("vrelation",
      "vrelation name {-concentric|-equaldistance|-equalradius|-fix|-identic|-offset|-parallel|-perpendicular|-tangent|-symmetric}"
      "\n\t\t: concentric - 2 circled edges."
      "\n\t\t: equaldistance - 4 vertex/edges."
      "\n\t\t: equalradius - 1 or 2 circled edges."
      "\n\t\t: fix - 1 edge."
      "\n\t\t: identic - 2 faces, edges or vertices."
      "\n\t\t: offset - 2 faces."
      "\n\t\t: parallel - 2 faces or 2 edges."
      "\n\t\t: perpendicular - 2 faces or 2 edges."
      "\n\t\t: tangent - two coplanar edges (first the circular edge then the tangent edge) or two faces."
      "\n\t\t: symmetric - 3 edges or 1 edge and 2 vertices."
      "-Builds specific relation from selected objects.",
      __FILE__, VRelationBuilder, group);

  theCommands.Add("vdimparam",
    "vdimparam name"
    "[-text 3d|2d wf|sh|wireframe|shading IntegerSize]\n"
    "[-font FontName]\n"
    "[-label left|right|hcenter|hfit top|bottom|vcenter|vfit]\n"
    "[-arrow external|internal|fit]\n"
    "[{-arrowlength|-arlen} RealArrowLength]\n"
    "[{-arrowangle|-arangle} ArrowAngle(degrees)]\n"
    "[-plane xoy|yoz|zox]\n"
    "[-flyout FloatValue -extension FloatValue]\n"
    "[-value CustomNumberValue]\n"
    "[-textvalue CustomTextValue]\n"
    "[-dispunits DisplayUnitsString]\n"
    "[-modelunits ModelUnitsString]\n"
    "[-showunits | -hideunits]\n"
    " -Sets parameters for angle, length, radius and diameter dimensions.\n"
    " -See also: vmovedim, vdimension.\n",
    __FILE__,VDimParam,group);

  theCommands.Add("vlengthparam",
    "vlengthparam name"
    "[-direction {ox|oy|oz|x y z|autodirection}]\n"
    " -Sets parameters for length dimension.\n"
    " -See also: vdimparam, vdimension.\n",
    __FILE__,VLengthParam,group);

  theCommands.Add("vangleparam",
    "vangleparam name"
    "[-type interior|exterior]\n"
    "[-showarrow first|second|both|none]\n"
    " -Sets parameters for angle dimension.\n"
    " -See also: vdimparam, vdimension.\n",
    __FILE__,VAngleParam,group);

  theCommands.Add("vmovedim",
      "vmovedim : vmovedim [name] [x y z]"
      "\n\t\t: Moves picked or named (if name defined)"
      "\n\t\t: dimension to picked mouse position or input point."
      "\n\t\t: Text label of dimension 'name' is moved to position, another parts of dimensionare adjusted.",
		  __FILE__,VMoveDim,group);

}
