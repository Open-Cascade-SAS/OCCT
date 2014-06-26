/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and / or modify it
 under the terms of the GNU Lesser General Public version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#ifndef _VOXEL_VISDRAWER_H_
#define _VOXEL_VISDRAWER_H_

#include "Voxel_VisData.h"
#include <Graphic3d_BndBox4f.hxx>

class Handle(OpenGl_GraphicDriver);

class VoxelClient_VisDrawer
{
public:

  class VisElement;

public:

  Standard_EXPORT static void Init (Handle(OpenGl_GraphicDriver)& theDriver);

	Standard_EXPORT VoxelClient_VisDrawer(Voxel_VisData* theData);
	Standard_EXPORT virtual ~VoxelClient_VisDrawer();

	Standard_EXPORT void EvalMinMax(Graphic3d_BndBox4f& theMinMax) const;
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
