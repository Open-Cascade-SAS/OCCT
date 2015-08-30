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

#ifndef _StdSelect_ViewerSelector3d_HeaderFile
#define _StdSelect_ViewerSelector3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_DefineHandle.hxx>

#include <Handle_Graphic3d_Group.hxx>
#include <Handle_Graphic3d_Structure.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <SelectMgr_Selection.hxx>
#include <Handle_V3d_View.hxx>
#include <Handle_SelectMgr_EntityOwner.hxx>
#include <NCollection_Handle.hxx>

class Graphic3d_Group;
class Graphic3d_Structure;
class V3d_View;
class TColgp_Array1OfPnt2d;
class SelectMgr_EntityOwner;
class SelectMgr_SelectableObjectSet;


//! Selector Usable by Viewers from V3d
class StdSelect_ViewerSelector3d : public SelectMgr_ViewerSelector
{

public:

  //! Constructs an empty 3D selector object.
  Standard_EXPORT StdSelect_ViewerSelector3d();

  //! Sets the pixel tolerance <theTolerance>.
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTolerance);

  //! Returns the pixel tolerance.
  inline Standard_Integer PixelTolerance() const;

  //! Picks the sensitive entity at the pixel coordinates of
  //! the mouse <theXPix> and <theYPix>. The selector looks for touched areas and owners.
  Standard_EXPORT void Pick (const Standard_Integer theXPix,
                             const Standard_Integer theYPix,
                             const Handle(V3d_View)& theView);

  //! Picks the sensitive entity according to the minimum
  //! and maximum pixel values <theXPMin>, <theYPMin>, <theXPMax>
  //! and <theYPMax> defining a 2D area for selection in the 3D view aView.
  Standard_EXPORT void Pick (const Standard_Integer theXPMin,
                             const Standard_Integer theYPMin,
                             const Standard_Integer theXPMax,
                             const Standard_Integer theYPMax,
                             const Handle(V3d_View)& theView);

  //! pick action - input pixel values for polyline selection for selection.
  Standard_EXPORT void Pick (const TColgp_Array1OfPnt2d& thePolyline,
                             const Handle(V3d_View)& theView);

  //! Displays sensitives in view <theView>.
  Standard_EXPORT void DisplaySensitive (const Handle(V3d_View)& theView);
  
  Standard_EXPORT void ClearSensitive (const Handle(V3d_View)& theView);
  
  Standard_EXPORT void DisplaySensitive (const Handle(SelectMgr_Selection)& theSel,
                                         const gp_Trsf& theTrsf,
                                         const Handle(V3d_View)& theView,
                                         const Standard_Boolean theToClearOthers = Standard_True);

  //! Marks all sensitive entities, stored in viewer selector, as inactive for selection
  Standard_EXPORT void ResetSelectionActivationStatus();

  Standard_EXPORT virtual Standard_Boolean HasDepthClipping (const Handle(SelectMgr_EntityOwner)& theOwner) const Standard_OVERRIDE;

  //! Is used for rectangular selection only
  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the algorithm will
  //! mark both included and overlapped entities as matched
  Standard_EXPORT void AllowOverlapDetection (const Standard_Boolean theIsToAllow);

  DEFINE_STANDARD_RTTI(StdSelect_ViewerSelector3d)

protected:

  //! Set view clipping for the selector.
  //! @param thePlanes [in] the view planes.
  Standard_EXPORT void SetClipping (const Graphic3d_SequenceOfHClipPlane& thePlanes);

private:

  void ComputeSensitivePrs (const Handle(SelectMgr_Selection)& theSel, const gp_Trsf& theLoc);

  Handle_Graphic3d_Group myareagroup;
  Handle_Graphic3d_Group mysensgroup;
  Handle_Graphic3d_Structure mystruct;
  Graphic3d_SequenceOfHClipPlane myClipPlanes;
};

DEFINE_STANDARD_HANDLE(StdSelect_ViewerSelector3d, SelectMgr_ViewerSelector)

#include <StdSelect_ViewerSelector3d.lxx>

#endif
