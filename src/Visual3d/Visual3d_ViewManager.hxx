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

#ifndef _Visual3d_ViewManager_HeaderFile
#define _Visual3d_ViewManager_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Visual3d_SequenceOfView.hxx>
#include <Aspect_GenId.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Visual3d_MapOfZLayerSettings.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Visual3d_HSequenceOfView.hxx>
#include <Standard_Integer.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_CView.hxx>
class Visual3d_Layer;
class Graphic3d_GraphicDriver;
class Visual3d_View;
class Graphic3d_Structure;
class Aspect_Window;
class Graphic3d_DataStructureManager;


class Visual3d_ViewManager;
DEFINE_STANDARD_HANDLE(Visual3d_ViewManager, Graphic3d_StructureManager)

//! This class allows the definition of a manager to
//! which the views are associated.
//! It allows them to be globally manipulated.
class Visual3d_ViewManager : public Graphic3d_StructureManager
{

public:

  
  //! Creates a 3D visualizer.
  //! Currently creating of more than 100 viewer instances
  //! is not supported and leads to InitializationError and
  //! initialisation failure.
  //! This limitation might be addressed in some future OCCT releases.
  //!
  //! Category: Methods to modify the class definition
  Standard_EXPORT Visual3d_ViewManager(const Handle(Graphic3d_GraphicDriver)& theDriver);
  
  //! Activates all the views of the manager <me>.
  Standard_EXPORT void Activate();
  
  //! Deactivates all the views of the manager <me>.
  Standard_EXPORT void Deactivate();
  
  //! Deletes and erases the 3D visualiser <me>.
  Standard_EXPORT virtual void Destroy() Standard_OVERRIDE;
~Visual3d_ViewManager()
{
  Destroy();
}
  
  //! Erases all of the structures displayed in the
  //! visualiser <me>.
  Standard_EXPORT void Erase();
  
  //! Redraws all the displayed structures.
  Standard_EXPORT void Redraw() const;
  
  //! Updates layer of immediate presentations.
  Standard_EXPORT void RedrawImmediate() const;
  
  //! Invalidates viewer content but does not redraw it.
  Standard_EXPORT void Invalidate() const;
  
  //! Deletes and erases the 3D visualiser <me>.
  Standard_EXPORT void Remove();
  
  //! Updates screen in function of modifications of the structures.
  //! Category: Methods to modify the class definition
  Standard_EXPORT void Update() const Standard_OVERRIDE;
  
  //! Returns the group of views activated in the visualiser <me>.
  Standard_EXPORT Handle(Visual3d_HSequenceOfView) ActivatedView() const;
  
  //! Returns the group of views defined in the visualiser <me>.
  Standard_EXPORT const Visual3d_SequenceOfView& DefinedViews() const;
  
  //! Returns the theoretical maximum number of
  //! definable views in the view manager <me>.
  //! Warning: It's not possible to accept an infinite
  //! number of definable views because each
  //! view must have an identification and we
  //! have different view managers.
  Standard_EXPORT Standard_Integer MaxNumOfViews() const;
  
  //! Returns :
  //! a new identification number for a new view
  //! in the visualiser.
  Standard_EXPORT Standard_Integer Identification (const Handle(Visual3d_View)& AView);
  
  //! Release a unique ID of the view reserved for the view on its creation.
  Standard_EXPORT void UnIdentification (const Standard_Integer aViewId);
  
  //! Returns the structure with the identification number <AId>.
  Standard_EXPORT Handle(Graphic3d_Structure) Identification (const Standard_Integer AId) const Standard_OVERRIDE;
  
  //! Returns the identification number of the visualiser.
  Standard_EXPORT Standard_Integer Identification() const Standard_OVERRIDE;
  
  //! Changes the display priority of the structure <AStructure>.
  Standard_EXPORT void ChangeDisplayPriority (const Handle(Graphic3d_Structure)& AStructure, const Standard_Integer OldPriority, const Standard_Integer NewPriority) Standard_OVERRIDE;
  
  //! Change Z layer for structure. The layer mechanism allows
  //! to display structures in higher layers in overlay of structures in
  //! lower layers.
  Standard_EXPORT void ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure, const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;
  
  //! Sets the settings for a single Z layer for all managed views.
  Standard_EXPORT void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings) Standard_OVERRIDE;
  
  //! Returns the settings of a single Z layer.
  Standard_EXPORT Graphic3d_ZLayerSettings ZLayerSettings (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;
  
  //! Add a new top-level Z layer and get its ID as
  //! <theLayerId> value. The method returns Standard_False if the layer
  //! can not be created. The layer mechanism allows to display
  //! structures in higher layers in overlay of structures in lower layers.
  Standard_EXPORT Standard_Boolean AddZLayer (Graphic3d_ZLayerId& theLayerId) Standard_OVERRIDE;
  
  //! Remove Z layer with ID <theLayerId>. Method returns
  //! Standard_False if the layer can not be removed or doesn't exists.
  //! By default, there are always default bottom-level layer that can't
  //! be removed.
  Standard_EXPORT Standard_Boolean RemoveZLayer (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;
  
  //! Return all Z layer ids in sequence ordered by overlay level
  //! from lowest layer to highest ( foreground ). The first layer ID
  //! in sequence is the default layer that can't be removed.
  Standard_EXPORT void GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const Standard_OVERRIDE;
  
  //! Clears the structure <AStructure>.
  Standard_EXPORT void Clear (const Handle(Graphic3d_Structure)& AStructure, const Standard_Boolean WithDestruction) Standard_OVERRIDE;
  
  //! Connects the structures <AMother> and <ADaughter>.
  Standard_EXPORT void Connect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) Standard_OVERRIDE;
  
  //! Disconnects the structures <AMother> and <ADaughter>.
  Standard_EXPORT void Disconnect (const Handle(Graphic3d_Structure)& AMother, const Handle(Graphic3d_Structure)& ADaughter) Standard_OVERRIDE;
  
  //! Display of the structure <AStructure>.
  Standard_EXPORT void Display (const Handle(Graphic3d_Structure)& AStructure) Standard_OVERRIDE;
  
  //! Erases the structure <AStructure>.
  Standard_EXPORT void Erase (const Handle(Graphic3d_Structure)& AStructure) Standard_OVERRIDE;
  
  //! Highlights the structure <AStructure>.
  Standard_EXPORT void Highlight (const Handle(Graphic3d_Structure)& AStructure, const Aspect_TypeOfHighlightMethod AMethod) Standard_OVERRIDE;
  
  Standard_EXPORT void SetTransform (const Handle(Graphic3d_Structure)& AStructure, const TColStd_Array2OfReal& ATrsf) Standard_OVERRIDE;
  
  //! Suppress the highlighting on all the structures.
  Standard_EXPORT void UnHighlight() Standard_OVERRIDE;
  
  //! Suppress the highlighting on the structure <AStructure>.
  Standard_EXPORT void UnHighlight (const Handle(Graphic3d_Structure)& AStructure) Standard_OVERRIDE;
  
  //! Returns Standard_True if the view associated to the
  //! window <AWindow> exists and is activated.
  //! <TheViewId> contains the internal identification of
  //! the associated view.
  Standard_EXPORT Standard_Boolean ViewExists (const Handle(Aspect_Window)& AWindow, Graphic3d_CView& TheCView) const;
  
  //! Forces a new construction of the structure <AStructure>
  //! if <AStructure> is displayed and TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_Structure)& AStructure) Standard_OVERRIDE;
  
  //! Forces a new construction of the structure <AStructure>
  //! if <AStructure> is displayed in <AProjector> and TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_Structure)& AStructure, const Handle(Graphic3d_DataStructureManager)& AProjector) Standard_OVERRIDE;
  
  //! Returns Standard_True if the zbuffer activity
  //! is managed automatically.
  //! Default Standard_False
  Standard_EXPORT Standard_Boolean ZBufferAuto() const;
  
  //! if <AFlag> is  Standard_True then the zbuffer activity
  //! is managed automatically.
  //! Default Standard_False
  Standard_EXPORT void SetZBufferAuto (const Standard_Boolean AFlag);

friend class Visual3d_View;
friend class Visual3d_Layer;


  DEFINE_STANDARD_RTTI(Visual3d_ViewManager,Graphic3d_StructureManager)

protected:




private:

  
  //! Install z layers managed by the view manager into the
  //! controlled view. This method used on the view initialization to
  //! make the layer lists consistent.
  Standard_EXPORT void InstallZLayers (const Handle(Visual3d_View)& theView) const;
  
  Visual3d_SequenceOfView MyDefinedView;
  Aspect_GenId MyViewGenId;
  Handle(Graphic3d_GraphicDriver) MyGraphicDriver;
  Standard_Boolean MyZBufferAuto;
  Aspect_GenId myZLayerGenId;
  TColStd_MapOfInteger myLayerIds;
  TColStd_SequenceOfInteger myLayerSeq;
  Visual3d_MapOfZLayerSettings myMapOfZLayerSettings;


};







#endif // _Visual3d_ViewManager_HeaderFile
