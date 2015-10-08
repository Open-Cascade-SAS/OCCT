// Created on: 2008-05-13
// Created by: Vladislav ROMASHKO
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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


#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <Voxel_Prs.hxx>

#include "Voxel_VisData.h"
Voxel_Prs::Voxel_Prs():AIS_InteractiveObject(PrsMgr_TOP_AllView),myVisData(0)
{

}

void Voxel_Prs::SetBoolVoxels(const Standard_Address theVoxels)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myBoolVoxels = (Voxel_BoolDS*) theVoxels;
}

void Voxel_Prs::SetColorVoxels(const Standard_Address theVoxels)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myColorVoxels = (Voxel_ColorDS*) theVoxels;
}

void Voxel_Prs::SetROctBoolVoxels(const Standard_Address theVoxels)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myROctBoolVoxels = (Voxel_ROctBoolDS*) theVoxels;
}

void Voxel_Prs::SetTriangulation(const Handle(Poly_Triangulation)& theTriangulation)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myTriangulation = theTriangulation;
}

void Voxel_Prs::SetColor(const Quantity_Color& theColor)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myColor = theColor;
}

void Voxel_Prs::SetPointSize(const Standard_Real theSize)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myPointSize = theSize;
}

void Voxel_Prs::SetQuadrangleSize(const Standard_Integer theSize)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myQuadrangleSize = theSize;
}

void Voxel_Prs::SetColors(const Handle(Quantity_HArray1OfColor)& theColors)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myColors = theColors;
}

void Voxel_Prs::SetDisplayMode(const Voxel_VoxelDisplayMode theMode)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayMode = theMode;
}

void Voxel_Prs::SetTransparency(const Standard_Real theTransparency)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myTransparency = theTransparency;
}

static void setMaterial(const Handle(Graphic3d_Group)& G,
			const Quantity_Color&          C,
			const Standard_Real            T)
{
  Graphic3d_MaterialAspect material(Graphic3d_NOM_PLASTIC);
  material.SetColor(C);
  material.SetTransparency(T);
  Handle(Graphic3d_AspectFillArea3d) aspect = 
    new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID, C, C, Aspect_TOL_SOLID, 1, material, material);
  aspect->SetDistinguishOff();
  aspect->SetEdgeOff();
  aspect->SetTextureMapOff();
  G->SetPrimitivesAspect(aspect);
}

void Voxel_Prs::Compute(const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                         const Handle(Prs3d_Presentation)& thePresentation, 
                         const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();
  if (!myVisData)
    return;

  if (((Voxel_VisData*)myVisData)->myBoolVoxels)
  {
    // Reset GL lists.
    // BoolDS
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolPointsFirst = 1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsFirst = 1;
  }

  if (((Voxel_VisData*)myVisData)->myColorVoxels)
  {
    // Reset GL lists.
    // ColorDS
    ((Voxel_VisData*)myVisData)->myDisplay.myColorPointsFirst = 1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsFirst = 1;
  }

  if (((Voxel_VisData*)myVisData)->myROctBoolVoxels)
  {
    // Reset GL lists.
    // ROctBoolDS
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolPointsFirst = 1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsFirst = 1;
  }

  // Set data to the user draw function.
  Handle(Graphic3d_Group) G = Prs3d_Root::CurrentGroup(thePresentation);
  if (((Voxel_VisData*)myVisData)->myDisplay.myDisplayMode == Voxel_VDM_BOXES || 
      ((Voxel_VisData*)myVisData)->myDisplay.myDisplayMode == Voxel_VDM_NEARESTBOXES ||
      !((Voxel_VisData*)myVisData)->myTriangulation.IsNull())
  {
    setMaterial(G, ((Voxel_VisData*)myVisData)->myDisplay.myColor, 
		((Voxel_VisData*)myVisData)->myDisplay.myTransparency);

    // Reset normals of triangulation
    if (!((Voxel_VisData*)myVisData)->myTriangulation.IsNull())
    {
      ((Voxel_VisData*)myVisData)->myNormalsOfNodes.Nullify();
      ((Voxel_VisData*)myVisData)->myDisplay.myTriangulationList = -1;
    }
  }

  ///Handle(OpenGl_Group) aGroup = Handle(OpenGl_Group)::DownCast (G);
  ///aGroup->AddElement (myVisData);
}

void Voxel_Prs::ComputeSelection(const Handle(SelectMgr_Selection)& /*theSelection*/,
				 const Standard_Integer /*theMode*/)
{

}

// Destructor
void Voxel_Prs::Destroy()
{
  if (myVisData)
  {
    delete (Voxel_VisData*) myVisData;
    myVisData = 0;
  }
}

void Voxel_Prs::Allocate()
{
  if (!myVisData)
  {
    myVisData = new Voxel_VisData;

    ((Voxel_VisData*)myVisData)->myBoolVoxels = 0;
    ((Voxel_VisData*)myVisData)->myColorVoxels = 0;
    ((Voxel_VisData*)myVisData)->myROctBoolVoxels = 0;

    // Points

    // BoolDS
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolPointsList = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[0] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[1] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[2] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[3] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[4] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[5] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsList[6] = -1;

    // ROctBoolDS
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolPointsList = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[0] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[1] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[2] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[3] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[4] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[5] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsList[6] = -1;

    // ColorDS:
    ((Voxel_VisData*)myVisData)->myDisplay.myColorPointsList = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[0] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[1] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[2] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[3] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[4] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[5] = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsList[6] = -1;
 
    ((Voxel_VisData*)myVisData)->myDisplay.myColorMinValue = 1;
    ((Voxel_VisData*)myVisData)->myDisplay.myColorMaxValue = 15;

    ((Voxel_VisData*)myVisData)->myDisplay.myPointSize      = 1.0;
    ((Voxel_VisData*)myVisData)->myDisplay.myQuadrangleSize = 100;
    ((Voxel_VisData*)myVisData)->myDisplay.myTransparency   = 0.0;
    ((Voxel_VisData*)myVisData)->myDisplay.myDegenerateMode = 0;
    ((Voxel_VisData*)myVisData)->myDisplay.myUsageOfGLlists = 1;
    ((Voxel_VisData*)myVisData)->myDisplay.mySmoothPoints   = 0;

    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedXMin = -DBL_MAX;
    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedXMax =  DBL_MAX;
    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedYMin = -DBL_MAX;
    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedYMax =  DBL_MAX;
    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedZMin = -DBL_MAX;
    ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedZMax =  DBL_MAX;

    ((Voxel_VisData*)myVisData)->myDisplay.myTriangulationList = -1;

    ((Voxel_VisData*)myVisData)->myDisplay.myHighlightx = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myHighlighty = -1;
    ((Voxel_VisData*)myVisData)->myDisplay.myHighlightz = -1;
  }
}

void Voxel_Prs::SetDegenerateMode(const Standard_Boolean theDegenerate)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myDegenerateMode = (theDegenerate == Standard_True);
}

void Voxel_Prs::SetUsageOfGLlists(const Standard_Boolean theUsage)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.myUsageOfGLlists = (theUsage == Standard_True);
}

void Voxel_Prs::SetSmoothPoints(const Standard_Boolean theSmooth)
{
  Allocate();
  ((Voxel_VisData*)myVisData)->myDisplay.mySmoothPoints = (theSmooth == Standard_True);
}

void Voxel_Prs::SetColorRange(const Standard_Byte theMinValue,
			      const Standard_Byte theMaxValue)
{
  Allocate();

  ((Voxel_VisData*)myVisData)->myDisplay.myColorMinValue = theMinValue;
  ((Voxel_VisData*)myVisData)->myDisplay.myColorMaxValue = theMaxValue;

  // Reset GL lists
  ((Voxel_VisData*)myVisData)->myDisplay.myColorPointsFirst = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsFirst = 1;
}

void Voxel_Prs::SetSizeRange(const Standard_Real theDisplayedXMin,
			     const Standard_Real theDisplayedXMax,
			     const Standard_Real theDisplayedYMin,
			     const Standard_Real theDisplayedYMax,
			     const Standard_Real theDisplayedZMin,
			     const Standard_Real theDisplayedZMax)
{
  Allocate();

  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedXMin = theDisplayedXMin;
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedXMax = theDisplayedXMax;
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedYMin = theDisplayedYMin;
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedYMax = theDisplayedYMax;
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedZMin = theDisplayedZMin;
  ((Voxel_VisData*)myVisData)->myDisplay.myDisplayedZMax = theDisplayedZMax;

  // Reset GL lists
  ((Voxel_VisData*)myVisData)->myDisplay.myBoolPointsFirst            = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myBoolNearestPointsFirst     = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myColorPointsFirst           = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myColorNearestPointsFirst    = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolPointsFirst        = 1;
  ((Voxel_VisData*)myVisData)->myDisplay.myROctBoolNearestPointsFirst = 1;
}

void Voxel_Prs::Highlight(const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz)
{
  Allocate();

  ((Voxel_VisData*)myVisData)->myDisplay.myHighlightx = ix;
  ((Voxel_VisData*)myVisData)->myDisplay.myHighlighty = iy;
  ((Voxel_VisData*)myVisData)->myDisplay.myHighlightz = iz;
}
