/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
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
