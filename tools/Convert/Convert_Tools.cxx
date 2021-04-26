// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <inspector/Convert_Tools.hxx>
#include <inspector/Convert_TransientShape.hxx>

#include <AIS_Line.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPreviewAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <gp_XY.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Standard_Dump.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Compound.hxx>

// =======================================================================
// function : ReadShape
// purpose :
// =======================================================================
TopoDS_Shape Convert_Tools::ReadShape (const TCollection_AsciiString& theFileName)
{
  TopoDS_Shape aShape;

  BRep_Builder aBuilder;
  BRepTools::Read (aShape, theFileName.ToCString(), aBuilder);
  return aShape;
}

//=======================================================================
//function : ConvertStreamToPresentations
//purpose  :
//=======================================================================
void Convert_Tools::ConvertStreamToPresentations (const Standard_SStream& theSStream,
                                                  const Standard_Integer theStartPos,
                                                  const Standard_Integer /*theLastPos*/,
                                                  NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  int aStartPos = theStartPos;

  gp_XYZ aPoint;
  if (aPoint.InitFromJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPoint)));
    return;
  }

  gp_Pnt aPnt;
  if (aPnt.InitFromJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeVertex (aPnt)));
    return;
  }

  gp_Dir aDir;
  if (aDir.InitFromJson (theSStream, aStartPos))
  {
    gp_Lin aLin (gp::Origin(), aDir);
    Handle(Geom_Line) aGeomLine = new Geom_Line (aLin);
    CreatePresentation (aGeomLine, thePresentations);
    return;
  }

  gp_Ax2 anAx2;
  if (anAx2.InitFromJson (theSStream, aStartPos))
  {
    Handle(Geom_Plane) aGeomPlane = new Geom_Plane (gp_Ax3 (anAx2));
    CreatePresentation (aGeomPlane, thePresentations);
    return;
  }

  gp_Ax3 anAx3; // should be after gp_Ax2
  if (anAx3.InitFromJson (theSStream, aStartPos))
  {
    Handle(Geom_Plane) aGeomPlane = new Geom_Plane (anAx3);
    CreatePresentation (aGeomPlane, thePresentations);
    return;
  }

  // should be after gp_Ax3
  gp_Ax1 anAxis;
  if (anAxis.InitFromJson (theSStream, aStartPos))
  {
    thePresentations.Append (new Convert_TransientShape (BRepBuilderAPI_MakeEdge (anAxis.Location(), anAxis.Location().Coord() + anAxis.Direction().XYZ())));
    return;
  }

  gp_Trsf aTrsf;
  if (aTrsf.InitFromJson (theSStream, aStartPos))
  {
    CreatePresentation (aTrsf, thePresentations);
    return;
  }

  Bnd_Box aBox;
  if (aBox.InitFromJson (theSStream, aStartPos))
  {
    TopoDS_Shape aShape;
    if (Convert_Tools::CreateShape (aBox, aShape))
      thePresentations.Append (new Convert_TransientShape (aShape));
    return;
  }

  Select3D_BndBox3d aSelectBndBox;
  if (aSelectBndBox.InitFromJson (theSStream, aStartPos))
  {
    TopoDS_Shape aShape;

    gp_Pnt aPntMin = gp_Pnt (aSelectBndBox.CornerMin().x(), aSelectBndBox.CornerMin().y(), aSelectBndBox.CornerMin().z());
    gp_Pnt aPntMax = gp_Pnt (aSelectBndBox.CornerMax().x(), aSelectBndBox.CornerMax().y(), aSelectBndBox.CornerMax().z());
    if (CreateBoxShape (aPntMin, aPntMax, aShape))
      thePresentations.Append (new Convert_TransientShape (aShape));
    return;
  }
}

//=======================================================================
//function : ConvertStreamToColor
//purpose  :
//=======================================================================
Standard_Boolean Convert_Tools::ConvertStreamToColor (const Standard_SStream& theSStream,
                                                      Quantity_Color& theColor)
{
  Standard_Integer aStartPos = 1;
  Quantity_ColorRGBA aColorRGBA;
  if (aColorRGBA.InitFromJson (theSStream, aStartPos))
  {
    theColor = aColorRGBA.GetRGB();
    return Standard_True;
  }

  Quantity_Color aColor;
  if (aColor.InitFromJson (theSStream, aStartPos))
  {
    theColor = aColor;
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
Standard_Boolean Convert_Tools::CreateShape (const Bnd_Box& theBoundingBox, TopoDS_Shape& theShape)
{
  if (theBoundingBox.IsVoid() || theBoundingBox.IsWhole())
    return Standard_False;

  Standard_Real aXmin, anYmin, aZmin, aXmax, anYmax, aZmax;
  theBoundingBox.Get (aXmin, anYmin, aZmin, aXmax, anYmax, aZmax);

  gp_Pnt aPntMin = gp_Pnt (aXmin, anYmin, aZmin);
  gp_Pnt aPntMax = gp_Pnt (aXmax, anYmax, aZmax);

  return CreateBoxShape (aPntMin, aPntMax, theShape);
}

//=======================================================================
//function : CreateShape
//purpose  :
//=======================================================================
Standard_Boolean Convert_Tools::CreateShape (const Bnd_OBB& theBoundingBox, TopoDS_Shape& theShape)
{
  if (theBoundingBox.IsVoid())
    return Standard_False;

  TColgp_Array1OfPnt anArrPnts(0, 8);
  theBoundingBox.GetVertex(&anArrPnts(0));

  BRep_Builder aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound (aCompound);

  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(1))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(0)), gp_Pnt (anArrPnts.Value(2))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(1)), gp_Pnt (anArrPnts.Value(3))));
  aBuilder.Add (aCompound, BRepBuilderAPI_MakeEdge (gp_Pnt (anArrPnts.Value(2)), gp_Pnt (anArrPnts.Value(3))));

  theShape = aCompound;
  return Standard_True;
}

//=======================================================================
//function : CreateBoxShape
//purpose  :
//=======================================================================
Standard_Boolean Convert_Tools::CreateBoxShape (const gp_Pnt& thePntMin, const gp_Pnt& thePntMax, TopoDS_Shape& theShape)
{
  BRepPreviewAPI_MakeBox aMakeBox;
  aMakeBox.Init (thePntMin, thePntMax);
  theShape = aMakeBox.Shape();

  return Standard_True;
}

//=======================================================================
//function : CreatePresentation
//purpose  :
//=======================================================================
void Convert_Tools::CreatePresentation (const Handle(Geom_Line)& theLine,
                                        NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  Handle(AIS_Line) aLinePrs = new AIS_Line (theLine);
  aLinePrs->SetColor (Quantity_NOC_TOMATO);
  thePresentations.Append (aLinePrs);
}

//=======================================================================
//function : CreatePresentation
//purpose  :
//=======================================================================
void Convert_Tools::CreatePresentation (const Handle(Geom_Plane)& thePlane,
                                        NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  Handle(AIS_Plane) aPlanePrs = new AIS_Plane (thePlane);

  aPlanePrs->Attributes()->SetPlaneAspect (new Prs3d_PlaneAspect());
  Handle (Prs3d_PlaneAspect) aPlaneAspect = aPlanePrs->Attributes()->PlaneAspect();
  aPlaneAspect->SetPlaneLength (100, 100);
  aPlaneAspect->SetDisplayCenterArrow (Standard_True);
  aPlaneAspect->SetDisplayEdgesArrows (Standard_True);
  aPlaneAspect->SetArrowsSize (100);
  aPlaneAspect->SetArrowsLength (100);
  aPlaneAspect->SetDisplayCenterArrow (Standard_True);
  aPlaneAspect->SetDisplayEdges (Standard_True);

  aPlanePrs->SetColor (Quantity_NOC_WHITE);
  aPlanePrs->SetTransparency (0);

  thePresentations.Append (aPlanePrs);
}

//=======================================================================
//function : CreatePresentation
//purpose  :
//=======================================================================
void Convert_Tools::CreatePresentation (const gp_Trsf& theTrsf,
                                        NCollection_List<Handle(Standard_Transient)>& thePresentations)
{
  Bnd_Box aBox (gp_Pnt(), gp_Pnt(10., 10., 10));

  TopoDS_Shape aBoxShape;
  if (!Convert_Tools::CreateShape (aBox, aBoxShape))
    return;

  Handle(AIS_Shape) aSourcePrs = new AIS_Shape (aBoxShape);
  aSourcePrs->Attributes()->SetAutoTriangulation (Standard_False);
  aSourcePrs->SetColor (Quantity_NOC_WHITE);
  aSourcePrs->SetTransparency (0.5);
  thePresentations.Append (aSourcePrs);

  Handle(AIS_Shape) aTransformedPrs = new AIS_Shape (aBoxShape);
  aTransformedPrs->Attributes()->SetAutoTriangulation (Standard_False);
  aTransformedPrs->SetColor (Quantity_NOC_TOMATO);
  aTransformedPrs->SetTransparency (0.5);
  aTransformedPrs->SetLocalTransformation (theTrsf);
  thePresentations.Append (aTransformedPrs);
}
