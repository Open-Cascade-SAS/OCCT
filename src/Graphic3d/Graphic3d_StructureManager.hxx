// Created on: 1991-09-05
// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Graphic3d_StructureManager_HeaderFile
#define _Graphic3d_StructureManager_HeaderFile

#include <Aspect_GenId.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>
#include <Aspect_TypeOfUpdate.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_MapOfObject.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_ViewAffinity.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <MMgt_TShared.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_SequenceOfInteger.hxx>

typedef NCollection_IndexedMap<Graphic3d_CView*> Graphic3d_IndexedMapOfView;

class Graphic3d_AspectLine3d;
class Graphic3d_AspectText3d;
class Graphic3d_AspectMarker3d;
class Graphic3d_AspectFillArea3d;
class Graphic3d_GraphicDriver;
class Graphic3d_InitialisationError;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;
class Standard_Transient;

class Graphic3d_StructureManager;
DEFINE_STANDARD_HANDLE(Graphic3d_StructureManager, MMgt_TShared)

//! This class allows the definition of a manager to
//! which the graphic objects are associated.
//! It allows them to be globally manipulated.
//! It defines the global attributes.
//! Keywords: Structure, Structure Manager, Update Mode,
//! Destroy, Highlight, Visible
class Graphic3d_StructureManager : public MMgt_TShared
{
public:

  //! Initializes the ViewManager.
  //! Currently creating of more than 100 viewer instances
  //! is not supported and leads to InitializationError and
  //! initialization failure.
  //! This limitation might be addressed in some future OCCT releases.
  //! Warning: Raises InitialisationError if the initialization
  //! of the ViewManager failed.
  Standard_EXPORT Graphic3d_StructureManager (const Handle(Graphic3d_GraphicDriver)& theDriver);

  //! Deletes the manager <me>.
  Standard_EXPORT virtual void Destroy();
~Graphic3d_StructureManager()
{
  Destroy();
}

  //! Modifies the default attributes for lines
  //! in the visualiser.
  Standard_EXPORT void SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& CTX);

  //! Modifies the default attributes for faces
  //! in the visualiser.
  Standard_EXPORT void SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& CTX);

  //! Modifies the default attributes for text
  //! in the visualiser.
  Standard_EXPORT void SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& CTX);

  //! Modifies the default attributes for markers
  //! in the visualiser.
  Standard_EXPORT void SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& CTX);

  //! Modifies the screen update mode.
  //!
  //! TOU_ASAP - as soon as possible
  //! TOU_WAIT - on demand (with the Update function)
  //! Note : Dynamic Operations and Update Mode
  //! Use SetUpdateMode to control when changes to
  //! the display are made.   Use one of the   following
  //! functions to update one or more views:
  //! - Update all views of the viewer: Graphic3d_StructureManager::Update()
  //! - Update one view of the viewer:  Graphic3d_View::Update()
  //! Use one of the following functions to update the entire display:
  //! - Redraw all structures in all views: Graphic3d_StructureManager::Redraw()
  //! - Redraw all structures in one view:  Graphic3d_View::Redraw()
  Standard_EXPORT void SetUpdateMode (const Aspect_TypeOfUpdate theType);

  //! Returns the screen update mode.
  //!
  //! TOU_ASAP	as soon as possible
  //! TOU_WAIT	on demand (Update)
  Standard_EXPORT Aspect_TypeOfUpdate UpdateMode() const;

  //! Updates screen in function of modifications of the structures.
  Standard_EXPORT virtual void Update (const Aspect_TypeOfUpdate theMode = Aspect_TOU_ASAP) const;

  //! Deletes and erases the 3D structure manager.
  Standard_EXPORT virtual void Remove();

  //! Erases all the structures.
  Standard_EXPORT virtual void Erase();

  //! Returns the set of structures displayed in
  //! visualiser <me>.
  Standard_EXPORT void DisplayedStructures (Graphic3d_MapOfStructure& SG) const;

  //! Returns the set of highlighted structures
  //! in a visualiser <me>.
  Standard_EXPORT void HighlightedStructures (Graphic3d_MapOfStructure& SG) const;

  //! Returns the values of the current default attributes.
  Standard_EXPORT Handle(Graphic3d_AspectFillArea3d) FillArea3dAspect() const;

  //! Returns maximum number of managers defineable.
  Standard_EXPORT static Standard_Integer Limit();

  //! Returns the values of the current default attributes.
  Standard_EXPORT Handle(Graphic3d_AspectLine3d) Line3dAspect() const;

  //! Returns the values of the current default attributes.
  Standard_EXPORT Handle(Graphic3d_AspectMarker3d) Marker3dAspect() const;

  //! Returns the values of the current default attributes.
  Standard_EXPORT void PrimitivesAspect (Handle(Graphic3d_AspectLine3d)& CTXL, Handle(Graphic3d_AspectText3d)& CTXT, Handle(Graphic3d_AspectMarker3d)& CTXM, Handle(Graphic3d_AspectFillArea3d)& CTXF) const;

  //! Returns the values of the current default attributes.
  Standard_EXPORT Handle(Graphic3d_AspectText3d) Text3dAspect() const;

  //! Returns a current identifier available.
  Standard_EXPORT static Standard_Integer CurrentId();

  //! Forces a new construction of the structure.
  //! if <theStructure> is displayed and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute (const Handle(Graphic3d_Structure)& theStructure);

  //! Forces a new construction of the structure.
  //! if <theStructure> is displayed in <theProjector> and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute (const Handle(Graphic3d_Structure)& theStructure, const Handle(Graphic3d_DataStructureManager)& theProjector);

  //! Clears the structure.
  Standard_EXPORT virtual void Clear (const Handle(Graphic3d_Structure)& theStructure, const Standard_Boolean theWithDestruction);

  //! Connects the structures.
  Standard_EXPORT virtual void Connect (const Handle(Graphic3d_Structure)& theMother, const Handle(Graphic3d_Structure)& theDaughter);

  //! Disconnects the structures.
  Standard_EXPORT virtual void Disconnect (const Handle(Graphic3d_Structure)& theMother, const Handle(Graphic3d_Structure)& theDaughter);

  //! Display the structure.
  Standard_EXPORT virtual void Display (const Handle(Graphic3d_Structure)& theStructure);

  //! Erases the structure.
  Standard_EXPORT virtual void Erase (const Handle(Graphic3d_Structure)& theStructure);

  //! Highlights the structure.
  Standard_EXPORT virtual void Highlight (const Handle(Graphic3d_Structure)& theStructure, const Aspect_TypeOfHighlightMethod theMethod);

  //! Transforms the structure.
  Standard_EXPORT virtual void SetTransform (const Handle(Graphic3d_Structure)& theStructure, const TColStd_Array2OfReal& theTrsf);

  //! Changes the display priority of the structure <AStructure>.
  Standard_EXPORT virtual void ChangeDisplayPriority (const Handle(Graphic3d_Structure)& theStructure, const Standard_Integer theOldPriority, const Standard_Integer theNewPriority);

  //! Change Z layer for structure. The Z layer mechanism allows to display structures in higher
  //! layers in overlay of structures in lower layers.
  Standard_EXPORT virtual void ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure, const Graphic3d_ZLayerId theLayerId);

  //! Returns the graphic driver of <me>.
  Standard_EXPORT const Handle(Graphic3d_GraphicDriver)& GraphicDriver() const;

  //! Attaches the view to this structure manager and sets its identification number within the manager.
  Standard_EXPORT Standard_Integer Identification (Graphic3d_CView* theView);

  //! Detach the view from this structure manager and release its identification.
  Standard_EXPORT void UnIdentification (Graphic3d_CView* theView);

  //! Returns the group of views defined in the structure manager.
  Standard_EXPORT const Graphic3d_IndexedMapOfView& DefinedViews() const;

  //! Returns the theoretical maximum number of definable views in the manager.
  //! Warning: It's not possible to accept an infinite number of definable views because each
  //! view must have an identification and we have different managers.
  Standard_EXPORT Standard_Integer MaxNumOfViews() const;

  //! Returns the identification number of the manager.
  Standard_EXPORT virtual Standard_Integer Identification() const;

  //! Returns the structure with the identification number <AId>.
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Identification (const Standard_Integer AId) const;

  //! Returns a new identification number for a new structure in the manager.
  Standard_EXPORT Standard_Integer NewIdentification();

  //! Suppress the highlighting on the structure <AStructure>.
  Standard_EXPORT virtual void UnHighlight (const Handle(Graphic3d_Structure)& AStructure);

  //! Suppresses the highlighting on all the structures in <me>.
  Standard_EXPORT virtual void UnHighlight();

  Standard_EXPORT void RecomputeStructures();

  //! Recomputes all structures from theStructures.
  Standard_EXPORT void RecomputeStructures (const Graphic3d_MapOfStructure& theStructures);

  Standard_EXPORT Handle(Graphic3d_ViewAffinity) RegisterObject (const Handle(Standard_Transient)& theObject);

  Standard_EXPORT void UnregisterObject (const Handle(Standard_Transient)& theObject);

  Standard_EXPORT Handle(Graphic3d_ViewAffinity) ObjectAffinity (const Handle(Standard_Transient)& theObject) const;

  friend class Graphic3d_Structure;

  DEFINE_STANDARD_RTTI(Graphic3d_StructureManager,MMgt_TShared)

protected:

  //! Returns the number of structures displayed in
  //! visualizer <me>.
  //! Returns the structure displayed in visualizer <me>.
  Standard_EXPORT Standard_Integer NumberOfDisplayedStructures() const;

private:

  //! Frees the identifier of a structure.
  void Remove (const Standard_Integer theId);

protected:

  Standard_Integer myId;
  Aspect_GenId myStructGenId;
  Aspect_GenId myViewGenId;
  Aspect_TypeOfUpdate myUpdateMode;
  Handle(Graphic3d_AspectLine3d) myAspectLine3d;
  Handle(Graphic3d_AspectText3d) myAspectText3d;
  Handle(Graphic3d_AspectMarker3d) myAspectMarker3d;
  Handle(Graphic3d_AspectFillArea3d) myAspectFillArea3d;
  Graphic3d_MapOfStructure myDisplayedStructure;
  Graphic3d_MapOfStructure myHighlightedStructure;
  Graphic3d_MapOfObject myRegisteredObjects;
  Handle(Graphic3d_GraphicDriver) myGraphicDriver;
  Graphic3d_IndexedMapOfView myDefinedViews;
};

#endif // _Graphic3d_StructureManager_HeaderFile
