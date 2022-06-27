// Created on: 1992-01-17
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _V3d_Viewer_HeaderFile
#define _V3d_Viewer_HeaderFile

#include <Aspect_GridDrawMode.hxx>
#include <Aspect_GridType.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>
#include <V3d_ListOfLight.hxx>
#include <V3d_ListOfView.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfView.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <Quantity_Color.hxx>

class Aspect_Grid;
class Graphic3d_AspectMarker3d;
class Graphic3d_GraphicDriver;
class Graphic3d_Group;
class Graphic3d_Structure;
class V3d_CircularGrid;
class V3d_RectangularGrid;
class V3d_View;

//! Defines services on Viewer type objects.
//! The methods of this class allow editing and
//! interrogation of the parameters linked to the viewer
//! its friend classes (View,light,plane).
class V3d_Viewer : public Standard_Transient
{
  friend class V3d_View;
  DEFINE_STANDARD_RTTIEXT(V3d_Viewer, Standard_Transient)
public:

  //! Create a Viewer with the given graphic driver and with default parameters:
  //! - View orientation: V3d_XposYnegZpos
  //! - View background: Quantity_NOC_GRAY30
  //! - Shading model: V3d_GOURAUD
  Standard_EXPORT V3d_Viewer (const Handle(Graphic3d_GraphicDriver)& theDriver);

  //! Returns True if One View more can be defined in this Viewer.
  Standard_EXPORT Standard_Boolean IfMoreViews() const;

  //! Creates a view in the viewer according to its default parameters.
  Standard_EXPORT Handle(V3d_View) CreateView();
  
  //! Activates all of the views of a viewer attached to a window.
  Standard_EXPORT void SetViewOn();
  
  //! Activates a particular view in the Viewer.
  //! Must be call if the Window attached to the view has been Deiconified.
  Standard_EXPORT void SetViewOn (const Handle(V3d_View)& theView);

  //! Deactivates all the views of a Viewer
  //! attached to a window.
  Standard_EXPORT void SetViewOff();
  
  //! Deactivates a particular view in the Viewer.
  //! Must be call if the Window attached to the view
  //! has been Iconified .
  Standard_EXPORT void SetViewOff (const Handle(V3d_View)& theView);
  
  //! Deprecated, Redraw() should be used instead.
  void Update() { Redraw(); }

  //! Redraws all the views of the Viewer even if no
  //! modification has taken place. Must be called if
  //! all the views of the Viewer are exposed, as for
  //! example in a global DeIconification.
  Standard_EXPORT void Redraw() const;
  
  //! Updates layer of immediate presentations.
  Standard_EXPORT void RedrawImmediate() const;
  
  //! Invalidates viewer content but does not redraw it.
  Standard_EXPORT void Invalidate() const;
  
  //! Suppresses the Viewer.
  Standard_EXPORT void Remove();

  //! Return Graphic Driver instance.
  const Handle(Graphic3d_GraphicDriver)& Driver() const { return myDriver; }

  //! Returns the structure manager associated to this viewer.
  Handle(Graphic3d_StructureManager) StructureManager() const { return myStructureManager; }

  //! Return default Rendering Parameters.
  //! By default these parameters are set in a new V3d_View.
  const Graphic3d_RenderingParams& DefaultRenderingParams() const { return myDefaultRenderingParams; }

  //! Set default Rendering Parameters.
  void SetDefaultRenderingParams (const Graphic3d_RenderingParams& theParams) { myDefaultRenderingParams = theParams; }

  //! Defines the default background colour of views
  //! attached to the viewer by supplying the color object
  void SetDefaultBackgroundColor (const Quantity_Color& theColor) { myBackground.SetColor (theColor); }

  //! Returns the gradient background of the view.
  const Aspect_GradientBackground& GetGradientBackground() const { return myGradientBackground; }
  
  //! Defines the default gradient background colours of views
  //! attached to the viewer by supplying the colour objects
  void SetDefaultBgGradientColors (const Quantity_Color& theColor1,
                                   const Quantity_Color& theColor2,
                                   const Aspect_GradientFillMethod theFillStyle = Aspect_GradientFillMethod_Horizontal)
  {
    myGradientBackground.SetColors (theColor1, theColor2, theFillStyle);
  }

  //! Returns the default size of the view.
  Standard_Real DefaultViewSize() const { return myViewSize; }

  //! Gives a default size for the creation of views of the viewer.
  Standard_EXPORT void SetDefaultViewSize (const Standard_Real theSize);

  //! Returns the default Projection.
  V3d_TypeOfOrientation DefaultViewProj() const { return myViewProj; }

  //! Sets the default projection for creating views in the viewer.
  void SetDefaultViewProj (const V3d_TypeOfOrientation theOrientation) { myViewProj = theOrientation; }

  //! Returns the default type of Visualization.
  V3d_TypeOfVisualization DefaultVisualization() const { return myVisualization; }

  //! Gives the default visualization mode.
  void SetDefaultVisualization (const V3d_TypeOfVisualization theType) { myVisualization = theType; }

  //! Returns the default type of Shading; Graphic3d_TypeOfShadingModel_Phong by default.
  Graphic3d_TypeOfShadingModel DefaultShadingModel() const { return myDefaultRenderingParams.ShadingModel; }

  //! Gives the default type of SHADING.
  void SetDefaultShadingModel (const Graphic3d_TypeOfShadingModel theType) { myDefaultRenderingParams.ShadingModel = theType; }

  //! Returns the default type of View (orthographic or perspective projection) to be returned by CreateView() method.
  V3d_TypeOfView DefaultTypeOfView() const { return myDefaultTypeOfView; }

  //! Set the default type of View (orthographic or perspective projection) to be returned by CreateView() method.
  void SetDefaultTypeOfView (const V3d_TypeOfView theType) { myDefaultTypeOfView = theType; }

  //! Returns the default background colour object.
  Quantity_Color DefaultBackgroundColor() const { return myBackground.Color(); }

  //! Returns the gradient background colour objects of the view.
  void DefaultBgGradientColors (Quantity_Color& theColor1, Quantity_Color& theColor2) const { myGradientBackground.Colors (theColor1, theColor2); }

  //! Return all Z layer ids in sequence ordered by overlay level from lowest layer to highest ( foreground ).
  //! The first layer ID in sequence is the default layer that can't be removed.
  Standard_EXPORT void GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const;

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a particular view.
  //! Custom layers will be inserted before Graphic3d_ZLayerId_Top (e.g. between Graphic3d_ZLayerId_Default and before Graphic3d_ZLayerId_Top).
  //! @param theLayerId [out] id of created layer
  //! @param theSettings [in] new layer settings
  //! @return FALSE if the layer can not be created
  Standard_Boolean AddZLayer (Graphic3d_ZLayerId& theLayerId,
                              const Graphic3d_ZLayerSettings& theSettings = Graphic3d_ZLayerSettings())
  {
    return InsertLayerBefore (theLayerId, theSettings, Graphic3d_ZLayerId_Top);
  }

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a particular view.
  //! Layer rendering order is defined by its position in list (altered by theLayerAfter)
  //! and IsImmediate() flag (all layers with IsImmediate() flag are drawn afterwards);
  //! @param theNewLayerId [out] id of created layer; layer id is arbitrary and does not depend on layer position in the list
  //! @param theSettings    [in] new layer settings
  //! @param theLayerAfter  [in] id of layer to append new layer before
  //! @return FALSE if the layer can not be created
  Standard_EXPORT Standard_Boolean InsertLayerBefore (Graphic3d_ZLayerId& theNewLayerId,
                                                      const Graphic3d_ZLayerSettings& theSettings,
                                                      const Graphic3d_ZLayerId theLayerAfter);

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a particular view.
  //! Layer rendering order is defined by its position in list (altered by theLayerAfter)
  //! and IsImmediate() flag (all layers with IsImmediate() flag are drawn afterwards);
  //! @param theNewLayerId [out] id of created layer; layer id is arbitrary and does not depend on layer position in the list
  //! @param theSettings    [in] new layer settings
  //! @param theLayerBefore [in] id of layer to append new layer after
  //! @return FALSE if the layer can not be created
  Standard_EXPORT Standard_Boolean InsertLayerAfter (Graphic3d_ZLayerId& theNewLayerId,
                                                     const Graphic3d_ZLayerSettings& theSettings,
                                                     const Graphic3d_ZLayerId theLayerBefore);

  //! Remove Z layer with ID <theLayerId>.
  //! Method returns Standard_False if the layer can not be removed or doesn't exists.
  //! By default, there are always default bottom-level layer that can't be removed.
  Standard_EXPORT Standard_Boolean RemoveZLayer (const Graphic3d_ZLayerId theLayerId);

  //! Returns the settings of a single Z layer.
  Standard_EXPORT const Graphic3d_ZLayerSettings& ZLayerSettings (const Graphic3d_ZLayerId theLayerId) const;

  //! Sets the settings for a single Z layer.
  Standard_EXPORT void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings);

public:

  //! Return a list of active views.
  const V3d_ListOfView& ActiveViews() const { return myActiveViews; }

  //! Return an iterator for active views.
  V3d_ListOfViewIterator ActiveViewIterator() const { return V3d_ListOfViewIterator (myActiveViews); }

  //! returns true if there is only one active view.
  Standard_Boolean LastActiveView() const { return myActiveViews.Extent() == 1; }

public:

  //! Return a list of defined views.
  const V3d_ListOfView& DefinedViews() const { return myDefinedViews; }

  //! Return an iterator for defined views.
  V3d_ListOfViewIterator DefinedViewIterator() const { return V3d_ListOfViewIterator (myDefinedViews); }

public: //! @name lights management

  //! Defines default lights:
  //!  positional-light 0.3 0. 0.
  //!  directional-light V3d_XnegYposZpos
  //!  directional-light V3d_XnegYneg
  //!  ambient-light
  Standard_EXPORT void SetDefaultLights();

  //! Activates MyLight in the viewer.
  Standard_EXPORT void SetLightOn (const Handle(V3d_Light)& theLight);
  
  //! Activates all the lights defined in this viewer.
  Standard_EXPORT void SetLightOn();
  
  //! Deactivates MyLight in this viewer.
  Standard_EXPORT void SetLightOff (const Handle(V3d_Light)& theLight);
  
  //! Deactivate all the Lights defined in this viewer.
  Standard_EXPORT void SetLightOff();

  //! Adds Light in Sequence Of Lights.
  Standard_EXPORT void AddLight (const Handle(V3d_Light)& theLight);

  //! Delete Light in Sequence Of Lights.
  Standard_EXPORT void DelLight (const Handle(V3d_Light)& theLight);
  
  //! Updates the lights of all the views of a viewer.
  Standard_EXPORT void UpdateLights();

  Standard_EXPORT Standard_Boolean IsGlobalLight (const Handle(V3d_Light)& TheLight) const;

  //! Return a list of active lights.
  const V3d_ListOfLight& ActiveLights() const { return myActiveLights; }

  //! Return an iterator for defined lights.
  V3d_ListOfLightIterator ActiveLightIterator() const { return V3d_ListOfLightIterator (myActiveLights); }

public:

  //! Return a list of defined lights.
  const V3d_ListOfLight& DefinedLights() const { return myDefinedLights; }

  //! Return an iterator for defined lights.
  V3d_ListOfLightIterator DefinedLightIterator() const { return V3d_ListOfLightIterator (myDefinedLights); }

public: //! @name objects management

  //! Erase all Objects in All the views.
  Standard_EXPORT void Erase() const;

  //! UnHighlight all Objects in All the views.
  Standard_EXPORT void UnHighlight() const;

public:

  //! returns true if the computed mode can be used.
  Standard_Boolean ComputedMode() const { return myComputedMode; }

  //! Set if the computed mode can be used.
  void SetComputedMode (const Standard_Boolean theMode) { myComputedMode = theMode; }

  //! returns true if by default the computed mode must be used.
  Standard_Boolean DefaultComputedMode() const { return myDefaultComputedMode; }

  //! Set if by default the computed mode must be used.
  void SetDefaultComputedMode (const Standard_Boolean theMode) { myDefaultComputedMode = theMode; }

public: //! @name privileged plane management

  const gp_Ax3& PrivilegedPlane() const { return myPrivilegedPlane; }

  Standard_EXPORT void SetPrivilegedPlane (const gp_Ax3& thePlane);

  Standard_EXPORT void DisplayPrivilegedPlane (const Standard_Boolean theOnOff, const Standard_Real theSize = 1);

public: //! @name grid management

  //! Activates the grid in all views of <me>.
  Standard_EXPORT void ActivateGrid (const Aspect_GridType aGridType, const Aspect_GridDrawMode aGridDrawMode);
  
  //! Deactivates the grid in all views of <me>.
  Standard_EXPORT void DeactivateGrid();
  
  //! Show/Don't show grid echo to the hit point.
  //! If TRUE,the grid echo will be shown at ConvertToGrid() time.
  Standard_EXPORT void SetGridEcho (const Standard_Boolean showGrid = Standard_True);
  
  //! Show grid echo <aMarker> to the hit point.
  //! Warning: When the grid echo marker is not set,
  //! a default marker is build with the attributes:
  //! marker type : Aspect_TOM_STAR
  //! marker color : Quantity_NOC_GRAY90
  //! marker size : 3.0
  Standard_EXPORT void SetGridEcho (const Handle(Graphic3d_AspectMarker3d)& aMarker);
  
  //! Returns TRUE when grid echo must be displayed at hit point.
  Standard_Boolean GridEcho() const { return myGridEcho; }

  //! Returns Standard_True if a grid is activated in <me>.
  Standard_EXPORT Standard_Boolean IsGridActive();

  //! Returns the defined grid in <me>.
  Handle(Aspect_Grid) Grid (bool theToCreate = true) { return Grid (myGridType, theToCreate); }

  //! Returns the defined grid in <me>.
  Standard_EXPORT Handle(Aspect_Grid) Grid (Aspect_GridType theGridType, bool theToCreate = true);

  //! Returns the current grid type defined in <me>.
  Aspect_GridType GridType() const { return myGridType; }

  //! Returns the current grid draw mode defined in <me>.
  Standard_EXPORT Aspect_GridDrawMode GridDrawMode();

  //! Returns the definition of the rectangular grid.
  Standard_EXPORT void RectangularGridValues (Standard_Real& theXOrigin, Standard_Real& theYOrigin,
                                              Standard_Real& theXStep, Standard_Real& theYStep,
                                              Standard_Real& theRotationAngle);

  //! Sets the definition of the rectangular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <XStep> defines the interval between 2 vertical lines.
  //! <YStep> defines the interval between 2 horizontal lines.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetRectangularGridValues (const Standard_Real XOrigin, const Standard_Real YOrigin, const Standard_Real XStep, const Standard_Real YStep, const Standard_Real RotationAngle);

  //! Returns the definition of the circular grid.
  Standard_EXPORT void CircularGridValues (Standard_Real& theXOrigin, Standard_Real& theYOrigin,
                                           Standard_Real& theRadiusStep,
                                           Standard_Integer& theDivisionNumber,
                                           Standard_Real& theRotationAngle);

  //! Sets the definition of the circular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <RadiusStep> defines the interval between 2 circles.
  //! <DivisionNumber> defines the section number of one half circle.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetCircularGridValues (const Standard_Real XOrigin, const Standard_Real YOrigin, const Standard_Real RadiusStep, const Standard_Integer DivisionNumber, const Standard_Real RotationAngle);

  //! Returns the location and the size of the grid.
  Standard_EXPORT void CircularGridGraphicValues (Standard_Real& theRadius,
                                                  Standard_Real& theOffSet);

  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetCircularGridGraphicValues (const Standard_Real Radius, const Standard_Real OffSet);

  //! Returns the location and the size of the grid.
  Standard_EXPORT void RectangularGridGraphicValues (Standard_Real& theXSize, Standard_Real& theYSize,
                                                     Standard_Real& theOffSet);

  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetRectangularGridGraphicValues (const Standard_Real XSize, const Standard_Real YSize, const Standard_Real OffSet);
  
  //! Display grid echo at requested point in the view.
  Standard_EXPORT void ShowGridEcho (const Handle(V3d_View)& theView, const Graphic3d_Vertex& thePoint);

  //! Temporarly hide grid echo.
  Standard_EXPORT void HideGridEcho (const Handle(V3d_View)& theView);

public: //! @name deprecated methods

  //! Returns Standard_True if a grid is activated in <me>.
  Standard_DEPRECATED ("Deprecated method - IsGridActive() should be used instead")
  Standard_Boolean IsActive() { return IsGridActive(); }

  //! Initializes an internal iterator on the active views.
  Standard_DEPRECATED ("Deprecated method - ActiveViews() should be used instead")
  void InitActiveViews() { myActiveViewsIterator.Initialize (myActiveViews); }

  //! Returns true if there are more active view(s) to return.
  Standard_DEPRECATED ("Deprecated method - ActiveViews() should be used instead")
  Standard_Boolean MoreActiveViews() const { return myActiveViewsIterator.More(); }

  //! Go to the next active view (if there is not, ActiveView will raise an exception)
  Standard_DEPRECATED ("Deprecated method - ActiveViews() should be used instead")
  void NextActiveViews() { if (!myActiveViews.IsEmpty()) myActiveViewsIterator.Next(); }

  Standard_DEPRECATED ("Deprecated method - ActiveViews() should be used instead")
  const Handle(V3d_View)& ActiveView() const { return myActiveViewsIterator.Value(); }

  //! Initializes an internal iterator on the Defined views.
  Standard_DEPRECATED ("Deprecated method - DefinedViews() should be used instead")
  void InitDefinedViews() { myDefinedViewsIterator.Initialize (myDefinedViews); }

  //! returns true if there are more Defined view(s) to return.
  Standard_DEPRECATED ("Deprecated method - DefinedViews() should be used instead")
  Standard_Boolean MoreDefinedViews() const { return myDefinedViewsIterator.More(); }

  //! Go to the next Defined view (if there is not, DefinedView will raise an exception)
  Standard_DEPRECATED ("Deprecated method - DefinedViews() should be used instead")
  void NextDefinedViews() { if (!myDefinedViews.IsEmpty()) myDefinedViewsIterator.Next(); }

  Standard_DEPRECATED ("Deprecated method - DefinedViews() should be used instead")
  const Handle(V3d_View)& DefinedView() const { return myDefinedViewsIterator.Value(); }

  //! Initializes an internal iteratator on the active Lights.
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  void InitActiveLights() { myActiveLightsIterator.Initialize (myActiveLights); }

  //! returns true if there are more active Light(s) to return.
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  Standard_Boolean MoreActiveLights() const { return myActiveLightsIterator.More(); }

  //! Go to the next active Light (if there is not, ActiveLight() will raise an exception)
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  void NextActiveLights() { myActiveLightsIterator.Next(); }

  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  const Handle(V3d_Light)& ActiveLight() const { return myActiveLightsIterator.Value(); }

  //! Initializes an internal iterattor on the Defined Lights.
  Standard_DEPRECATED ("Deprecated method - DefinedLights() should be used instead")
  void InitDefinedLights() { myDefinedLightsIterator.Initialize (myDefinedLights); }

  //! Returns true if there are more Defined Light(s) to return.
  Standard_DEPRECATED ("Deprecated method - DefinedLights() should be used instead")
  Standard_Boolean MoreDefinedLights() const { return myDefinedLightsIterator.More(); }

  //! Go to the next Defined Light (if there is not, DefinedLight() will raise an exception)
  Standard_DEPRECATED ("Deprecated method - DefinedLights() should be used instead")
  void NextDefinedLights() { if (!myDefinedLights.IsEmpty()) myDefinedLightsIterator.Next(); }

  Standard_DEPRECATED ("Deprecated method - DefinedLights() should be used instead")
  const Handle(V3d_Light)& DefinedLight() const { return myDefinedLightsIterator.Value(); }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:

  //! Returns the default background colour.
  const Aspect_Background& GetBackgroundColor() const { return myBackground; }

  //! Adds View in Sequence Of Views.
  Standard_EXPORT void AddView (const Handle(V3d_View)& theView);
  
  //! Delete View in Sequence Of Views.
  Standard_EXPORT void DelView (const V3d_View* theView);
  
private:

  Handle(Graphic3d_GraphicDriver) myDriver;
  Handle(Graphic3d_StructureManager) myStructureManager;
  TColStd_MapOfInteger myLayerIds;
  Aspect_GenId myZLayerGenId;

  V3d_ListOfView  myDefinedViews;
  V3d_ListOfView  myActiveViews;
  V3d_ListOfLight myDefinedLights;
  V3d_ListOfLight myActiveLights;

  Aspect_Background myBackground;
  Aspect_GradientBackground myGradientBackground;
  Standard_Real myViewSize;
  V3d_TypeOfOrientation myViewProj;
  V3d_TypeOfVisualization myVisualization;
  V3d_TypeOfView myDefaultTypeOfView;
  Graphic3d_RenderingParams myDefaultRenderingParams;

  V3d_ListOfView::Iterator  myActiveViewsIterator;
  V3d_ListOfView::Iterator  myDefinedViewsIterator;
  V3d_ListOfLight::Iterator myActiveLightsIterator;
  V3d_ListOfLight::Iterator myDefinedLightsIterator;

  Standard_Boolean myComputedMode;
  Standard_Boolean myDefaultComputedMode;

  gp_Ax3 myPrivilegedPlane;
  Handle(Graphic3d_Structure) myPlaneStructure;
  Standard_Boolean myDisplayPlane;
  Standard_Real myDisplayPlaneLength;

  Handle(V3d_RectangularGrid) myRGrid;
  Handle(V3d_CircularGrid) myCGrid;
  Aspect_GridType myGridType;
  Standard_Boolean myGridEcho;
  Handle(Graphic3d_Structure) myGridEchoStructure;
  Handle(Graphic3d_Group) myGridEchoGroup;
  Handle(Graphic3d_AspectMarker3d) myGridEchoAspect;
  Graphic3d_Vertex myGridEchoLastVert;

};

DEFINE_STANDARD_HANDLE(V3d_Viewer, Standard_Transient)

#endif // _V3d_Viewer_HeaderFile
