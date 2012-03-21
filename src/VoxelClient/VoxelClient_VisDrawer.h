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

#ifndef _VOXEL_VISDRAWER_H_
#define _VOXEL_VISDRAWER_H_

#include "Voxel_VisData.h"
#include <Graphic3d_CBounds.hxx>

class VoxelClient_VisDrawer
{
public:

  class VisElement;

public:

    Standard_EXPORT static void Init();

	Standard_EXPORT VoxelClient_VisDrawer(Voxel_VisData* theData);
	Standard_EXPORT virtual ~VoxelClient_VisDrawer();

	Standard_EXPORT void EvalMinMax(Graphic3d_CBounds& theMinMax) const;
	Standard_EXPORT void Display(const Standard_Boolean theHighlight);

private:
    
	Standard_EXPORT void DisplayVoxels(const Standard_Boolean theHighlight);
	Standard_EXPORT void DisplayPoints(const Standard_Boolean nearest);
	Standard_EXPORT void DisplayBoxes(const Standard_Boolean nearest);
    Standard_EXPORT void HighlightVoxel();
	
    Standard_EXPORT void DisplayTriangulation(const Standard_Boolean theHighlight);

	Voxel_VisData* myData;
};

#endif // _VOXEL_VISDRAWER_H_
