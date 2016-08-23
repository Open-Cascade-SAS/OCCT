// Created on: 2011-12-20 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
// 
//This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IVtkTools.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <vtkLookupTable.h>
#include <vtkMapper.h>

namespace IVtkTools
{
//============================================================================
// Method: InitLookupTable
// Purpose: Returns vtkLookupTable instance initialized by standrad OCCT colors.
//============================================================================
vtkSmartPointer<vtkLookupTable> InitLookupTable()
{
  vtkSmartPointer<vtkLookupTable> aColorTable = 
    vtkSmartPointer<vtkLookupTable>::New();
  // Set colors table for 3D shapes
  double aRange[2];
  aRange[0] = MT_Undefined;
  aRange[1] = MT_ShadedFace;
  aColorTable->Allocate (9);
  aColorTable->SetNumberOfTableValues (9);
  aColorTable->SetTableRange (aRange);
  aColorTable->SetValueRange (0, 1);
/*
  MT_Undefined     = -1   Undefined
  MT_IsoLine       =  0   IsoLine
  MT_FreeVertex    =  1   Free vertex
  MT_SharedVertex  =  2   Shared vertex
  MT_FreeEdge      =  3   Free edge
  MT_BoundaryEdge  =  4   Boundary edge (related to a single face)
  MT_SharedEdge    =  5   Shared edge (related to several faces)
  MT_WireFrameFace =  6   Wireframe face
  MT_ShadedFace    =  7   Shaded face
*/
  aColorTable->SetTableValue (0, 0, 0, 0); // Undefined
  aColorTable->SetTableValue (1, 0.5, 0.5, 0.5); // gray for IsoLine
  aColorTable->SetTableValue (2, 1, 0, 0); // red for Free vertex
  aColorTable->SetTableValue (3, 1, 1, 0); // yellow for Shared vertex
  aColorTable->SetTableValue (4, 1, 0, 0); // red for Free edge
  aColorTable->SetTableValue (5, 0, 1, 0); // green for Boundary edge (related to a single face)
  aColorTable->SetTableValue (6, 1, 1, 0); // yellow for Shared edge (related to several faces)
  aColorTable->SetTableValue (7, 1, 1, 0); // yellow for Wireframe face
  aColorTable->SetTableValue (8, 1, 1, 0); // yellow for Shaded face
  return aColorTable;
}

//============================================================================
//  Method: SetLookupTableColor
// Purpose: Set a color for given type of sub-shapes.
//============================================================================
void SetLookupTableColor (vtkLookupTable* theColorTable,
                          const IVtk_MeshType theColorRole,
                          const double theR, const double theG, const double theB,
                          const double /*theA*/)
{
  theColorTable->SetTableValue (theColorRole + 1, theR, theG, theB);
}

//============================================================================
//  Method: GetLookupTableColor
// Purpose: Get a color for given type of sub-shapes.
//============================================================================
void GetLookupTableColor (vtkLookupTable* theColorTable,
                          const IVtk_MeshType theColorRole,
                          double &theR, double &theG, double &theB)
{
  double aRgb[3];
  theColorTable->GetColor (theColorRole + 1, aRgb);
  theR = aRgb[0];
  theG = aRgb[1];
  theB = aRgb[2];
}

//============================================================================
//  Method: GetLookupTableColor
// Purpose: Get a color for given type of sub-shapes.
//============================================================================
void GetLookupTableColor (vtkLookupTable* theColorTable, 
                          const IVtk_MeshType theColorRole, 
                          double &theR, double &theG, double &theB, 
                          double &theA)
{
  theA = theColorTable->GetOpacity (theColorRole + 1);
  GetLookupTableColor (theColorTable, theColorRole, theR, theG, theB);
}

//============================================================================
//  Method: InitShapeMapper
// Purpose: Set up the initial shape mapper parameters with default OCC colors.
//============================================================================
void InitShapeMapper (vtkMapper* theMapper)
{
  InitShapeMapper (theMapper, InitLookupTable());
}

//============================================================================
//  Method: InitShapeMapper
// Purpose: Set up the initial shape mapper parameters with user colors.
//============================================================================
void InitShapeMapper (vtkMapper* theMapper, vtkLookupTable* theColorTable)
{
  theMapper->ScalarVisibilityOn();
  theMapper->SetScalarModeToUseCellFieldData();
  theMapper->SelectColorArray (IVtkVTK_ShapeData::ARRNAME_MESH_TYPES);
  theMapper->SetColorModeToMapScalars();
  theMapper->SetScalarRange (theColorTable->GetRange());
  theMapper->SetLookupTable (theColorTable);
  theMapper->Update();
}

} // namespace IVtkTools
