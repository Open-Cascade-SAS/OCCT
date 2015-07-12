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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_TypeOfUpdate.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapOfObject.hxx>
#include <Aspect_GenId.hxx>
#include <MMgt_TShared.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_ViewAffinity.hxx>
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
  //! TOU_ASAP	as soon as possible
  //! TOU_WAIT	on demand (with the Update function)
  //! Note : Dynamic Operations and Update Mode
  //! Use SetUpdateMode to control when changes to
  //! the display are made.   Use one of the   following
  //! functions to update one or more views:
  //! -   Update all views of the viewer:   Visual3d_ViewManager::Update ()
  //! -   Update one view of the viewer:   Visual3d_View::Update () Use one of
  //! the   following functions to update the entire display:
  //! -   Redraw all structures in all views:   Visual3d_ViewManager::Redraw ()
  //! -   Redraw all structures in one view:   Visual3d_View::Redraw ()  Update)
  Standard_EXPORT void SetUpdateMode (const Aspect_TypeOfUpdate AType);
  
  //! Updates screen in function of modifications of
  //! the structures.
  //! Warning: Not necessary if the update mode is TOU_ASAP.
  Standard_EXPORT virtual void Update() const = 0;
  
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
  
  //! Returns the screen update mode.
  //!
  //! TOU_ASAP	as soon as possible
  //! TOU_WAIT	on demand (Update)
  Standard_EXPORT Aspect_TypeOfUpdate UpdateMode() const;
  
  //! Changes the display priority of the structure <AStructure>.
  Standard_EXPORT virtual void ChangeDisplayPriority (const Handle(Graphic3d_Structure)& AStructure, const Standard_Integer OldPriority, const Standard_Integer NewPriority) = 0;
  
  //! Change Z layer for structure. The z layer mechanism allows
  //! to display structures in higher layers in overlay of structures in
  //! lower layers.
  Standard_EXPORT virtual void ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure, const Graphic3d_ZLayerId theLayerId) = 0;
  
  //! Sets the settings for a single Z layer for all managed views.
  Standard_EXPORT virtual void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings) = 0;
  
  //! Returns the settings of a single Z layer.
  Standard_EXPORT virtual Graphic3d_ZLayerSettings ZLayerSettings (const Graphic3d_ZLayerId theLayerId) = 0;
  
  //! Add a new top-level Z layer and get its ID as
  //! <theLayerId> value. The method returns Standard_False if the layer
  //! can not be created. The z layer mechanism allows to display
  //! structures in higher layers in overlay of structures in lower layers.
  Standard_EXPORT virtual Standard_Boolean AddZLayer (Graphic3d_ZLayerId& theLayerId) = 0;
  
  //! Remove Z layer with ID <theLayerId>. Method returns
  //! Standard_False if the layer can not be removed or doesn't exists.
  //! By default, there is always a default bottom-level layer that can't
  //! be removed.
  Standard_EXPORT virtual Standard_Boolean RemoveZLayer (const Graphic3d_ZLayerId theLayerId) = 0;
  
  //! Return all Z layer ids in sequence ordered by level
  //! from lowest layer to highest. The first layer ID in sequence is
  //! the default layer that can't be removed.
  Standard_EXPORT virtual void GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const = 0;
  
  //! Returns a current identifier available.
  Standard_EXPORT static Standard_Integer CurrentId();
  
  //! Forces a new construction of the structure <AStructure>
  //! if <AStructure> is displayed and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute (const Handle(Graphic3d_Structure)& AStructure) = 0;
  
  //! Forces a new construction of the structure <AStructure>
  //! if <AStructure> is displayed in <AProjector> and TOS_COMPUTED.
  Standard_EXPORT virtual void ReCompute (const Handle(Graphic3d_Structure)& AStructure, const Handle(Graphic3d_DataStructureManager)& AProjector) = 0;
  
  //! Clears the structure <AStructure>.
  Standard_EXPORT virtual void Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction) = 0;
  
  //! Connects the structures <AMother> and <ADaughter>.
  Standard_EXPORT virtual void Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) = 0;
  
  //! Disconnects the structures <AMother> and <ADaughter>.
  Standard_EXPORT virtual void Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) = 0;
  
  //! Display the structure <AStructure>.
  Standard_EXPORT virtual void Display (const Handle(Graphic3d_Structure)& AStructure) = 0;
  
  //! Erases the structure <AStructure>.
  Standard_EXPORT virtual void Erase (const Handle(Graphic3d_Structure)& AStructure) = 0;
  
  //! Highlights the structure <AStructure>.
  Standard_EXPORT virtual void Highlight (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfHighlightMethod AMethod) = 0;
  
  //! Transforms the structure <AStructure>.
  Standard_EXPORT virtual void SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf) = 0;
  
  //! Returns the graphic driver of <me>.
  Standard_EXPORT const Handle(Graphic3d_GraphicDriver)& GraphicDriver() const;
  
  //! Returns the identification number of the manager.
  Standard_EXPORT virtual Standard_Integer Identification() const;
  
  //! Returns the structure with the identification number <AId>.
  Standard_EXPORT virtual Handle(Graphic3d_Structure) Identification (const Standard_Integer AId) const;
  
  //! Returns a new identification number for a new structure in the manager.
  Standard_EXPORT Standard_Integer NewIdentification();
  
  //! Suppresses the highlighting on all the structures in <me>.
  Standard_EXPORT virtual void UnHighlight() = 0;
  
  //! Suppress the highlighting on the structure <AStructure>.
  Standard_EXPORT virtual void UnHighlight (const Handle(Graphic3d_Structure)& AStructure) = 0;
  
  Standard_EXPORT void RecomputeStructures();
  
  //! Recomputes all structures from theStructures.
  Standard_EXPORT void RecomputeStructures (const Graphic3d_MapOfStructure& theStructures);
  
  Standard_EXPORT Handle(Graphic3d_ViewAffinity) RegisterObject (const Handle(Standard_Transient)& theObject);
  
  Standard_EXPORT void UnregisterObject (const Handle(Standard_Transient)& theObject);
  
  Standard_EXPORT Handle(Graphic3d_ViewAffinity) ObjectAffinity (const Handle(Standard_Transient)& theObject) const;

friend class Graphic3d_Structure;


  DEFINE_STANDARD_RTTI(Graphic3d_StructureManager,MMgt_TShared)

protected:

  
  //! Initialises the ViewManager.
  //! Currently creating of more than 100 viewer instances
  //! is not supported and leads to InitializationError and
  //! initialisation failure.
  //! This limitation might be addressed in some future OCCT releases.
  //! Warning: Raises InitialisationError if the initialisation
  //! of the ViewManager failed.
  Standard_EXPORT Graphic3d_StructureManager(const Handle(Graphic3d_GraphicDriver)& theDriver);
  
  //! Returns the number of structures displayed in
  //! visualiser <me>.
  //! Returns the structure displayed in visualiser <me>.
  Standard_EXPORT Standard_Integer NumberOfDisplayedStructures() const;

  Standard_Integer MyId;
  Aspect_TypeOfUpdate MyUpdateMode;
  Handle(Graphic3d_AspectLine3d) MyAspectLine3d;
  Handle(Graphic3d_AspectText3d) MyAspectText3d;
  Handle(Graphic3d_AspectMarker3d) MyAspectMarker3d;
  Handle(Graphic3d_AspectFillArea3d) MyAspectFillArea3d;
  Graphic3d_MapOfStructure MyDisplayedStructure;
  Graphic3d_MapOfObject myRegisteredObjects;
  Graphic3d_MapOfStructure MyHighlightedStructure;
  Aspect_GenId MyStructGenId;
  Handle(Graphic3d_GraphicDriver) MyGraphicDriver;


private:

  
  //! Frees the identifieur <AnId>.
  Standard_EXPORT void Remove (const Standard_Integer AnId);



};







#endif // _Graphic3d_StructureManager_HeaderFile
