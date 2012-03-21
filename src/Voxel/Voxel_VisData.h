/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef _VOXEL_VISDATA_H_
#define _VOXEL_VISDATA_H_

#include <Quantity_Color.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfDir.hxx>
#include <Poly_Triangulation.hxx>

#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_VoxelDisplayMode.hxx>

enum VoxelDirection
{
    None,
	Xminus,
	Xplus,
	Yminus,
	Yplus,
	Zminus,
	Zplus
};

typedef struct
{

	/* Display mode */
	Voxel_VoxelDisplayMode myDisplayMode;

    /* Range of displayed values */
    /* BoolDS */
    /* No range */
    /* ColorDS */
    Standard_Byte  myColorMinValue;
    Standard_Byte  myColorMaxValue;

    /* Range of displayed size */
    Standard_Real         myDisplayedXMin;
    Standard_Real         myDisplayedXMax;
    Standard_Real         myDisplayedYMin;
    Standard_Real         myDisplayedYMax;
    Standard_Real         myDisplayedZMin;
    Standard_Real         myDisplayedZMax;

	/* Colors */
	Quantity_Color myColor;
    Handle(Quantity_HArray1OfColor) myColors;

	/* Size, width... */
	Standard_Real         myPointSize;
    Standard_Integer          myQuadrangleSize; /* 0% .. 100% */
    Standard_Byte  mySmoothPoints;

    /* Transparency */
    Standard_Real         myTransparency;

    /* GL lists of each display mode */
    /* BoolDS */
    /* POINTS */
    Standard_Integer            myBoolPointsList;
    Standard_Byte  myBoolPointsFirst;
    /* NEAREST POINTS */
    Standard_Integer            myBoolNearestPointsList[7];
    Standard_Byte  myBoolNearestPointsFirst;
    /* ColorDS */
    /* POINTS */
    Standard_Integer            myColorPointsList;
    Standard_Byte  myColorPointsFirst;
    /* NEAREST POINTS */
    Standard_Integer            myColorNearestPointsList[7];
    Standard_Byte  myColorNearestPointsFirst;
    /* ROctBoolDS */
    /* POINTS */
    Standard_Integer            myROctBoolPointsList;
    Standard_Byte  myROctBoolPointsFirst;
    /* NEAREST POINTS */
    Standard_Integer            myROctBoolNearestPointsList[7];
    Standard_Byte  myROctBoolNearestPointsFirst;
    /* TRIANGULATION */
    Standard_Integer            myTriangulationList;
    /* Usage of GL lists */
    Standard_Byte  myUsageOfGLlists;

    /* Degenerate mode */
    Standard_Byte  myDegenerateMode;

    /* Highlighted voxel */
    Standard_Integer            myHighlightx;
    Standard_Integer            myHighlighty;
    Standard_Integer            myHighlightz;

} DisplayData;

typedef struct
{

	// Voxels
	Voxel_BoolDS*     myBoolVoxels;
	Voxel_ColorDS*    myColorVoxels;
	Voxel_ROctBoolDS* myROctBoolVoxels;

    // Triangulation
    Handle(Poly_Triangulation) myTriangulation;
    Handle(TColgp_HArray1OfDir) myNormalsOfNodes;

	// Display
	DisplayData    myDisplay;

} Voxel_VisData;

#endif // _VOXEL_VISDATA_H_
