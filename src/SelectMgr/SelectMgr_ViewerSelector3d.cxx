// Created on: 1995-03-15
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <SelectMgr_ViewerSelector3d.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_Structure.hxx>
#include <math_BullardGenerator.hxx>
#include <Quantity_ColorHasher.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <SelectMgr.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionImageFiller.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_ViewerSelector3d, SelectMgr_ViewerSelector)

//=======================================================================
// Function : Constructor
// Purpose  :
//=======================================================================
SelectMgr_ViewerSelector3d::SelectMgr_ViewerSelector3d()
{
  //
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector3d::Pick (const Standard_Integer theXPix,
                                       const Standard_Integer theYPix,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  if(myToUpdateTolerance)
  {
    mySelectingVolumeMgr.SetPixelTolerance (myTolerances.Tolerance());
    myToUpdateTolerance = Standard_False;
  }

  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Point);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  gp_Pnt2d aMousePos (static_cast<Standard_Real> (theXPix),
                      static_cast<Standard_Real> (theYPix));
  mySelectingVolumeMgr.BuildSelectingVolume (aMousePos);
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), Handle(Graphic3d_SequenceOfHClipPlane)(), NULL);

  TraverseSensitives();
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector3d::Pick (const Standard_Integer theXPMin,
                                       const Standard_Integer theYPMin,
                                       const Standard_Integer theXPMax,
                                       const Standard_Integer theYPMax,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Box);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  gp_Pnt2d aMinMousePos (static_cast<Standard_Real> (theXPMin),
                         static_cast<Standard_Real> (theYPMin));
  gp_Pnt2d aMaxMousePos (static_cast<Standard_Real> (theXPMax),
                         static_cast<Standard_Real> (theYPMax));
  mySelectingVolumeMgr.BuildSelectingVolume (aMinMousePos,
                                             aMaxMousePos);

  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), Handle(Graphic3d_SequenceOfHClipPlane)(), NULL);

  TraverseSensitives();
}

//=======================================================================
// Function: Pick
// Purpose : Selection using a polyline
//=======================================================================
void SelectMgr_ViewerSelector3d::Pick (const TColgp_Array1OfPnt2d& thePolyline,
                                       const Handle(V3d_View)& theView)
{
  updateZLayers (theView);
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  mySelectingVolumeMgr.SetActiveSelectionType (SelectMgr_SelectingVolumeManager::Polyline);
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.BuildSelectingVolume (thePolyline);

  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), Handle(Graphic3d_SequenceOfHClipPlane)(), NULL);

  TraverseSensitives();
}

//=======================================================================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=======================================================================
void SelectMgr_ViewerSelector3d::DisplaySensitive (const Handle(V3d_View)& theView)
{
  for (SelectMgr_SelectableObjectSet::Iterator aSelectableIt (mySelectableObjects); aSelectableIt.More(); aSelectableIt.Next())
  {
    Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());
    const Handle (SelectMgr_SelectableObject)& anObj = aSelectableIt.Value();
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      if (aSelIter.Value()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        SelectMgr::ComputeSensitivePrs (aStruct, aSelIter.Value(), anObj->Transformation(), anObj->TransformPersistence());
      }
    }

    myStructs.Append (aStruct);
  }

  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructs); aStructIter.More(); aStructIter.Next())
  {
    Handle(Graphic3d_Structure)& aStruct = aStructIter.ChangeValue();
    aStruct->SetDisplayPriority (10);
    aStruct->Display();
  }

  theView->Update();
}

//=======================================================================
// Function: ClearSensitive
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector3d::ClearSensitive (const Handle(V3d_View)& theView)
{
  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructs); aStructIter.More(); aStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aPrs = aStructIter.ChangeValue();
    aPrs->Erase();
    aPrs->Clear();
    aPrs->Remove();
  }
  myStructs.Clear();

  if (!theView.IsNull())
  {
    theView->Update();
  }
}

//=======================================================================
//function : DisplaySenstive
//purpose  :
//=======================================================================
void SelectMgr_ViewerSelector3d::DisplaySensitive (const Handle(SelectMgr_Selection)& theSel,
                                                   const gp_Trsf& theTrsf,
                                                   const Handle(V3d_View)& theView,
                                                   const Standard_Boolean theToClearOthers)
{
  if (theToClearOthers)
  {
    ClearSensitive (theView);
  }

  Handle(Graphic3d_Structure) aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

  SelectMgr::ComputeSensitivePrs (aStruct, theSel, theTrsf, Handle(Graphic3d_TransformPers)());

  myStructs.Append (aStruct);
  myStructs.Last()->SetDisplayPriority (10);
  myStructs.Last()->Display();

  theView->Update();
}

//=======================================================================
// Function: updateZLayers
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector3d::updateZLayers (const Handle(V3d_View)& theView)
{
  myZLayerOrderMap.Clear();
  TColStd_SequenceOfInteger aZLayers;
  theView->Viewer()->GetAllZLayers (aZLayers);
  Standard_Integer aPos = 0;
  Standard_Boolean isPrevDepthWrite = true;
  for (TColStd_SequenceOfInteger::Iterator aLayerIter (aZLayers); aLayerIter.More(); aLayerIter.Next())
  {
    Graphic3d_ZLayerSettings aSettings = theView->Viewer()->ZLayerSettings (aLayerIter.Value());
    if (aSettings.ToClearDepth()
     || isPrevDepthWrite != aSettings.ToEnableDepthWrite())
    {
      ++aPos;
    }
    isPrevDepthWrite = aSettings.ToEnableDepthWrite();
    myZLayerOrderMap.Bind (aLayerIter.Value(), aPos);
  }
}

//=======================================================================
//function : ToPixMap
//purpose  :
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector3d::ToPixMap (Image_PixMap&                        theImage,
                                                       const Handle(V3d_View)&              theView,
                                                       const StdSelect_TypeOfSelectionImage theType,
                                                       const Standard_Integer               thePickedIndex)
{
  if (theImage.IsEmpty())
  {
    throw Standard_ProgramError("SelectMgr_ViewerSelector3d::ToPixMap() has been called with empty image");
  }

  Handle(SelectMgr_SelectionImageFiller) aFiller = SelectMgr_SelectionImageFiller::CreateFiller (theImage, this, theType);
  if (aFiller.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aSizeX = static_cast<Standard_Integer> (theImage.SizeX());
  const Standard_Integer aSizeY = static_cast<Standard_Integer> (theImage.SizeY());
  for (Standard_Integer aRowIter = 0; aRowIter < aSizeY; ++aRowIter)
  {
    for (Standard_Integer aColIter = 0; aColIter < aSizeX; ++aColIter)
    {
      Pick (aColIter, aRowIter, theView);
      aFiller->Fill (aColIter, aRowIter, thePickedIndex);
    }
  }
  aFiller->Flush();
  return Standard_True;
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void SelectMgr_ViewerSelector3d::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myStructs.Length())
  for (Graphic3d_SequenceOfStructure::Iterator aStructsIt (myStructs); aStructsIt.More(); aStructsIt.Next())
  {
    const Handle(Graphic3d_Structure)& aStructure = aStructsIt.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, aStructure)
  }
}
