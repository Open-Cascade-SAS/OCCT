#ifndef _VOXEL_VISDRAWER_H_
#define _VOXEL_VISDRAWER_H_

#include "Voxel_VisData.h"
#include <Graphic3d_CBounds.hxx>

class VoxelClient_VisDrawer  
{
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
