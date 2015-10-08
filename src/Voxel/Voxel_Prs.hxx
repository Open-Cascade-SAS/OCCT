// Created on: 2008-05-06
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

#ifndef _Voxel_Prs_HeaderFile
#define _Voxel_Prs_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Address.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Voxel_VoxelDisplayMode.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Byte.hxx>
class Poly_Triangulation;
class Quantity_Color;
class Prs3d_Presentation;


class Voxel_Prs;
DEFINE_STANDARD_HANDLE(Voxel_Prs, AIS_InteractiveObject)

//! Interactive object for voxels.
class Voxel_Prs : public AIS_InteractiveObject
{

public:

  
  //! An empty constructor.
  Standard_EXPORT Voxel_Prs();
  
  //! <theVoxels> is a Voxel_BoolDS* object.
  Standard_EXPORT void SetBoolVoxels (const Standard_Address theVoxels);
  
  //! <theVoxels> is a Voxel_ColorDS* object.
  Standard_EXPORT void SetColorVoxels (const Standard_Address theVoxels);
  
  //! <theVoxels> is a Voxel_ROctBoolDS* object.
  Standard_EXPORT void SetROctBoolVoxels (const Standard_Address theVoxels);
  
  //! Sets a triangulation for visualization.
  Standard_EXPORT void SetTriangulation (const Handle(Poly_Triangulation)& theTriangulation);
  
  //! Sets a display mode for voxels.
  Standard_EXPORT void SetDisplayMode (const Voxel_VoxelDisplayMode theMode);
  
  //! Defines the color of points, quadrangles ... for BoolDS.
  Standard_EXPORT virtual void SetColor (const Quantity_Color& theColor) Standard_OVERRIDE;
  
  //! Defines the color of points, quadrangles... for ColorDS.
  //! For ColorDS the size of array is 0 .. 15.
  //! 0 - means no color, this voxel is not drawn.
  Standard_EXPORT void SetColors (const Handle(Quantity_HArray1OfColor)& theColors);
  
  //! Defines the size of points for all types of voxels.
  Standard_EXPORT void SetPointSize (const Standard_Real theSize);
  
  //! Defines the size of quadrangles in per cents (0 .. 100).
  Standard_EXPORT void SetQuadrangleSize (const Standard_Integer theSize);
  
  //! Defines the transparency value [0 .. 1] for quadrangular visualization.
  Standard_EXPORT virtual void SetTransparency (const Standard_Real theTransparency) Standard_OVERRIDE;
  
  //! Highlights a voxel.
  //! It doesn't re-computes the whole interactive object,
  //! but only marks a voxels as "highlighted".
  //! The voxel becomes highlighted on next swapping of buffers.
  //! In order to unhighlight a voxel, set ix = iy = iz = -1.
  Standard_EXPORT void Highlight (const Standard_Integer ix, const Standard_Integer iy, const Standard_Integer iz);
  
  //! A destructor of presentation data.
  Standard_EXPORT void Destroy();
~Voxel_Prs()
{
  Destroy();
}
  
  //! Simplifies visualization of voxels in case of view rotation, panning and zooming.
  Standard_EXPORT void SetDegenerateMode (const Standard_Boolean theDegenerate);
  
  //! GL lists accelerate view rotation, panning and zooming operations, but
  //! it takes additional memory...
  //! It is up to the user of this interactive object to decide whether
  //! he has enough memory and may use GL lists or
  //! he is lack of memory and usage of GL lists is not recommended.
  //! By default, usage of GL lists is on.
  //! Also, as I noticed, the view without GL lists looks more precisely.
  Standard_EXPORT void SetUsageOfGLlists (const Standard_Boolean theUsage);
  
  //! Switches visualization of points from smooth to rough.
  Standard_EXPORT void SetSmoothPoints (const Standard_Boolean theSmooth);
  
  //! Defines min-max values for visualization of voxels of ColorDS structure.
  //! By default, min value = 1, max value = 15 (all non-zero values).
  Standard_EXPORT void SetColorRange (const Standard_Byte theMinValue, const Standard_Byte theMaxValue);
  
  //! Defines the displayed area of voxels.
  //! By default, the range is equal to the box of voxels (all voxels are displayed).
  Standard_EXPORT void SetSizeRange (const Standard_Real theDisplayedXMin, const Standard_Real theDisplayedXMax, const Standard_Real theDisplayedYMin, const Standard_Real theDisplayedYMax, const Standard_Real theDisplayedZMin, const Standard_Real theDisplayedZMax);



  DEFINE_STANDARD_RTTI(Voxel_Prs,AIS_InteractiveObject)

protected:

  
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager, const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0) Standard_OVERRIDE;



protected:

  
  Standard_EXPORT void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection, const Standard_Integer theMode);
  
  //! Allocates the data structure of visualization.
  Standard_EXPORT void Allocate();

  Standard_Address myVisData;


};







#endif // _Voxel_Prs_HeaderFile
