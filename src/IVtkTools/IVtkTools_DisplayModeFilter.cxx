// Created on: 2011-11-15 
// Created by: Roman KOZLOV
// Copyright (c) 2001-2012 OPEN CASCADE SAS 
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

#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(IVtkTools_DisplayModeFilter)

//============================================================================
// Method: Constructor
// Purpose:
//============================================================================
IVtkTools_DisplayModeFilter::IVtkTools_DisplayModeFilter()
  : myDisplayMode (DM_Wireframe),
    myDoDisplaySharedVertices (false)
{
  // Filter according to values in subshapes types array.
  myIdsArrayName = IVtkVTK_ShapeData::ARRNAME_MESH_TYPES;

  IVtk_IdTypeMap aTypes;

  aTypes.Add (MT_IsoLine);
  aTypes.Add (MT_FreeVertex);
  aTypes.Add (MT_FreeEdge);
  aTypes.Add (MT_BoundaryEdge);
  aTypes.Add (MT_SharedEdge);
  aTypes.Add (MT_WireFrameFace);

  myModesDefinition.Bind (DM_Wireframe, aTypes);

  aTypes.Clear();
  aTypes.Add (MT_FreeVertex);
  aTypes.Add (MT_ShadedFace);

  myModesDefinition.Bind (DM_Shading, aTypes);
}

//============================================================================
// Method: Destructor
// Purpose:
//============================================================================
IVtkTools_DisplayModeFilter::~IVtkTools_DisplayModeFilter() { }

//============================================================================
// Method: RequestData
// Purpose: Filters cells according to the selected display mode by mesh
//          parts types.
//============================================================================
int IVtkTools_DisplayModeFilter::RequestData (vtkInformation        *theRequest,
                                              vtkInformationVector **theInputVector,
                                              vtkInformationVector  *theOutputVector)
{
  SetData (myModesDefinition.Find (myDisplayMode));
  return Superclass::RequestData (theRequest, theInputVector, theOutputVector);
}

//============================================================================
// Method: PrintSelf
// Purpose:
//============================================================================
void IVtkTools_DisplayModeFilter::PrintSelf (std::ostream& theOs, vtkIndent theIndent)
{
  this->Superclass::PrintSelf (theOs, theIndent);
  theOs << theIndent << "IVtkTools_DisplayModeFilter: display mode = ";
  if (myDisplayMode == DM_Wireframe)
  {
    theOs << "Wireframe\n";
  }
  else
  {
    theOs << "Shading\n";
  }
}

//============================================================================
// Method: SetDisplaySharedVertices
// Purpose:
//============================================================================
void IVtkTools_DisplayModeFilter::SetDisplaySharedVertices (const bool theDoDisplay)
{
  if (myDoDisplaySharedVertices != theDoDisplay)
  {
    myDoDisplaySharedVertices = theDoDisplay;
    vtkIdType aVertexType = MT_SharedVertex;
    NCollection_DataMap<IVtk_DisplayMode, IVtk_IdTypeMap>::Iterator aModes (myModesDefinition);
    NCollection_DataMap<IVtk_DisplayMode, IVtk_IdTypeMap> aNewModes;
    IVtk_IdTypeMap aModeTypes;
    for (; aModes.More(); aModes.Next())
    {
      aModeTypes = aModes.Value();
      if (theDoDisplay && !aModeTypes.Contains(aVertexType))
      {
        aModeTypes.Add (aVertexType);
      }
      else if (!theDoDisplay && aModeTypes.Contains (aVertexType))
      {
        aModeTypes.Remove (aVertexType);
      }
      aNewModes.Bind (aModes.Key(), aModeTypes);
    }
    myModesDefinition = aNewModes;
    Modified();
  }
}

//============================================================================
// Method: SetDisplayMode
// Purpose:
//============================================================================
void IVtkTools_DisplayModeFilter::SetDisplayMode(const IVtk_DisplayMode theMode)
{
  if (myDisplayMode != theMode)
  {
    myDisplayMode = theMode;
    Modified();
  }
}

//============================================================================
// Method: GetDisplayMode
// Purpose:
//============================================================================
IVtk_DisplayMode IVtkTools_DisplayModeFilter::GetDisplayMode () const
{
  return myDisplayMode;
}

