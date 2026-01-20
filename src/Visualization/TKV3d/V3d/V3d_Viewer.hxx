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
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_ExtendedString.hxx>
#include <V3d_Light.hxx>
#include <NCollection_List.hxx>
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
  Standard_EXPORT V3d_Viewer(const occ::handle<Graphic3d_GraphicDriver>& theDriver);

  //! Returns True if One View more can be defined in this Viewer.
  Standard_EXPORT bool IfMoreViews() const;

  //! Creates a view in the viewer according to its default parameters.
  Standard_EXPORT occ::handle<V3d_View> CreateView();

  //! Activates all of the views of a viewer attached to a window.
  Standard_EXPORT void SetViewOn();

  //! Activates a particular view in the Viewer.
  //! Must be call if the Window attached to the view has been Deiconified.
  Standard_EXPORT void SetViewOn(const occ::handle<V3d_View>& theView);

  //! Deactivates all the views of a Viewer
  //! attached to a window.
  Standard_EXPORT void SetViewOff();

  //! Deactivates a particular view in the Viewer.
  //! Must be call if the Window attached to the view
  //! has been Iconified .
  Standard_EXPORT void SetViewOff(const occ::handle<V3d_View>& theView);

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
  const occ::handle<Graphic3d_GraphicDriver>& Driver() const { return myDriver; }

  //! Returns the structure manager associated to this viewer.
  occ::handle<Graphic3d_StructureManager> StructureManager() const { return myStructureManager; }

  //! Return default Rendering Parameters.
  //! By default these parameters are set in a new V3d_View.
  const Graphic3d_RenderingParams& DefaultRenderingParams() const
  {
    return myDefaultRenderingParams;
  }

  //! Set default Rendering Parameters.
  void SetDefaultRenderingParams(const Graphic3d_RenderingParams& theParams)
  {
    myDefaultRenderingParams = theParams;
  }

  //! Defines the default background colour of views
  //! attached to the viewer by supplying the color object
  void SetDefaultBackgroundColor(const Quantity_Color& theColor)
  {
    myBackground.SetColor(theColor);
  }

  //! Returns the gradient background of the view.
  const Aspect_GradientBackground& GetGradientBackground() const { return myGradientBackground; }

  //! Defines the default gradient background colours of views
  //! attached to the viewer by supplying the colour objects
  void SetDefaultBgGradientColors(
    const Quantity_Color&           theColor1,
    const Quantity_Color&           theColor2,
    const Aspect_GradientFillMethod theFillStyle = Aspect_GradientFillMethod_Horizontal)
  {
    myGradientBackground.SetColors(theColor1, theColor2, theFillStyle);
  }

  //! Returns the default size of the view.
  double DefaultViewSize() const { return myViewSize; }

  //! Gives a default size for the creation of views of the viewer.
  Standard_EXPORT void SetDefaultViewSize(const double theSize);

  //! Returns the default Projection.
  V3d_TypeOfOrientation DefaultViewProj() const { return myViewProj; }

  //! Sets the default projection for creating views in the viewer.
  void SetDefaultViewProj(const V3d_TypeOfOrientation theOrientation)
  {
    myViewProj = theOrientation;
  }

  //! Returns the default type of Visualization.
  V3d_TypeOfVisualization DefaultVisualization() const { return myVisualization; }

  //! Gives the default visualization mode.
  void SetDefaultVisualization(const V3d_TypeOfVisualization theType) { myVisualization = theType; }

  //! Returns the default type of Shading; Graphic3d_TypeOfShadingModel_Phong by default.
  Graphic3d_TypeOfShadingModel DefaultShadingModel() const
  {
    return myDefaultRenderingParams.ShadingModel;
  }

  //! Gives the default type of SHADING.
  void SetDefaultShadingModel(const Graphic3d_TypeOfShadingModel theType)
  {
    myDefaultRenderingParams.ShadingModel = theType;
  }

  //! Returns the default type of View (orthographic or perspective projection) to be returned by
  //! CreateView() method.
  V3d_TypeOfView DefaultTypeOfView() const { return myDefaultTypeOfView; }

  //! Set the default type of View (orthographic or perspective projection) to be returned by
  //! CreateView() method.
  void SetDefaultTypeOfView(const V3d_TypeOfView theType) { myDefaultTypeOfView = theType; }

  //! Returns the default background colour object.
  Quantity_Color DefaultBackgroundColor() const { return myBackground.Color(); }

  //! Returns the gradient background colour objects of the view.
  void DefaultBgGradientColors(Quantity_Color& theColor1, Quantity_Color& theColor2) const
  {
    myGradientBackground.Colors(theColor1, theColor2);
  }

  //! Return all Z layer ids in sequence ordered by overlay level from lowest layer to highest (
  //! foreground ). The first layer ID in sequence is the default layer that can't be removed.
  Standard_EXPORT void GetAllZLayers(NCollection_Sequence<int>& theLayerSeq) const;

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a
  //! particular view. Custom layers will be inserted before Graphic3d_ZLayerId_Top (e.g. between
  //! Graphic3d_ZLayerId_Default and before Graphic3d_ZLayerId_Top).
  //! @param[out] theLayerId  id of created layer
  //! @param[in] theSettings  new layer settings
  //! @return FALSE if the layer can not be created
  bool AddZLayer(Graphic3d_ZLayerId&             theLayerId,
                 const Graphic3d_ZLayerSettings& theSettings = Graphic3d_ZLayerSettings())
  {
    return InsertLayerBefore(theLayerId, theSettings, Graphic3d_ZLayerId_Top);
  }

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a
  //! particular view. Layer rendering order is defined by its position in list (altered by
  //! theLayerAfter) and IsImmediate() flag (all layers with IsImmediate() flag are drawn
  //! afterwards);
  //! @param[out] theNewLayerId  id of created layer; layer id is arbitrary and does not depend on
  //! layer position in the list
  //! @param[in] theSettings     new layer settings
  //! @param[in] theLayerAfter   id of layer to append new layer before
  //! @return FALSE if the layer can not be created
  Standard_EXPORT bool InsertLayerBefore(Graphic3d_ZLayerId&             theNewLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings,
                                         const Graphic3d_ZLayerId        theLayerAfter);

  //! Add a new top-level Z layer to all managed views and get its ID as <theLayerId> value.
  //! The Z layers are controlled entirely by viewer, it is not possible to add a layer to a
  //! particular view. Layer rendering order is defined by its position in list (altered by
  //! theLayerAfter) and IsImmediate() flag (all layers with IsImmediate() flag are drawn
  //! afterwards);
  //! @param[out] theNewLayerId  id of created layer; layer id is arbitrary and does not depend on
  //! layer position in the list
  //! @param[in] theSettings     new layer settings
  //! @param[in] theLayerBefore  id of layer to append new layer after
  //! @return FALSE if the layer can not be created
  Standard_EXPORT bool InsertLayerAfter(Graphic3d_ZLayerId&             theNewLayerId,
                                        const Graphic3d_ZLayerSettings& theSettings,
                                        const Graphic3d_ZLayerId        theLayerBefore);

  //! Remove Z layer with ID <theLayerId>.
  //! Method returns false if the layer can not be removed or doesn't exists.
  //! By default, there are always default bottom-level layer that can't be removed.
  Standard_EXPORT bool RemoveZLayer(const Graphic3d_ZLayerId theLayerId);

  //! Returns the settings of a single Z layer.
  Standard_EXPORT const Graphic3d_ZLayerSettings& ZLayerSettings(
    const Graphic3d_ZLayerId theLayerId) const;

  //! Sets the settings for a single Z layer.
  Standard_EXPORT void SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings);

public:
  //! Return a list of active views.
  const NCollection_List<occ::handle<V3d_View>>& ActiveViews() const { return myActiveViews; }

  //! Return an iterator for active views.
  NCollection_List<occ::handle<V3d_View>>::Iterator ActiveViewIterator() const
  {
    return NCollection_List<occ::handle<V3d_View>>::Iterator(myActiveViews);
  }

  //! returns true if there is only one active view.
  bool LastActiveView() const { return myActiveViews.Extent() == 1; }

public:
  //! Return a list of defined views.
  const NCollection_List<occ::handle<V3d_View>>& DefinedViews() const { return myDefinedViews; }

  //! Return an iterator for defined views.
  NCollection_List<occ::handle<V3d_View>>::Iterator DefinedViewIterator() const
  {
    return NCollection_List<occ::handle<V3d_View>>::Iterator(myDefinedViews);
  }

public: //! @name lights management
  //! Defines default lights:
  //!  positional-light 0.3 0. 0.
  //!  directional-light V3d_XnegYposZpos
  //!  directional-light V3d_XnegYneg
  //!  ambient-light
  Standard_EXPORT void SetDefaultLights();

  //! Activates MyLight in the viewer.
  Standard_EXPORT void SetLightOn(const occ::handle<V3d_Light>& theLight);

  //! Activates all the lights defined in this viewer.
  Standard_EXPORT void SetLightOn();

  //! Deactivates MyLight in this viewer.
  Standard_EXPORT void SetLightOff(const occ::handle<V3d_Light>& theLight);

  //! Deactivate all the Lights defined in this viewer.
  Standard_EXPORT void SetLightOff();

  //! Adds Light in Sequence Of Lights.
  Standard_EXPORT void AddLight(const occ::handle<V3d_Light>& theLight);

  //! Delete Light in Sequence Of Lights.
  Standard_EXPORT void DelLight(const occ::handle<V3d_Light>& theLight);

  //! Updates the lights of all the views of a viewer.
  Standard_EXPORT void UpdateLights();

  Standard_EXPORT bool IsGlobalLight(const occ::handle<V3d_Light>& TheLight) const;

  //! Return a list of active lights.
  const NCollection_List<occ::handle<Graphic3d_CLight>>& ActiveLights() const
  {
    return myActiveLights;
  }

  //! Return an iterator for defined lights.
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator ActiveLightIterator() const
  {
    return NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator(myActiveLights);
  }

public:
  //! Return a list of defined lights.
  const NCollection_List<occ::handle<Graphic3d_CLight>>& DefinedLights() const
  {
    return myDefinedLights;
  }

  //! Return an iterator for defined lights.
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator DefinedLightIterator() const
  {
    return NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator(myDefinedLights);
  }

public: //! @name objects management
  //! Erase all Objects in All the views.
  Standard_EXPORT void Erase() const;

  //! UnHighlight all Objects in All the views.
  Standard_EXPORT void UnHighlight() const;

public:
  //! returns true if the computed mode can be used.
  bool ComputedMode() const { return myComputedMode; }

  //! Set if the computed mode can be used.
  void SetComputedMode(const bool theMode) { myComputedMode = theMode; }

  //! returns true if by default the computed mode must be used.
  bool DefaultComputedMode() const { return myDefaultComputedMode; }

  //! Set if by default the computed mode must be used.
  void SetDefaultComputedMode(const bool theMode) { myDefaultComputedMode = theMode; }

public: //! @name privileged plane management
  const gp_Ax3& PrivilegedPlane() const { return myPrivilegedPlane; }

  Standard_EXPORT void SetPrivilegedPlane(const gp_Ax3& thePlane);

  Standard_EXPORT void DisplayPrivilegedPlane(const bool theOnOff, const double theSize = 1);

public: //! @name grid management
  //! Activates the grid in all views of <me>.
  Standard_EXPORT void ActivateGrid(const Aspect_GridType     aGridType,
                                    const Aspect_GridDrawMode aGridDrawMode);

  //! Deactivates the grid in all views of <me>.
  Standard_EXPORT void DeactivateGrid();

  //! Show/Don't show grid echo to the hit point.
  //! If TRUE,the grid echo will be shown at ConvertToGrid() time.
  Standard_EXPORT void SetGridEcho(const bool showGrid = true);

  //! Show grid echo <aMarker> to the hit point.
  //! Warning: When the grid echo marker is not set,
  //! a default marker is build with the attributes:
  //! marker type : Aspect_TOM_STAR
  //! marker color : Quantity_NOC_GRAY90
  //! marker size : 3.0
  Standard_EXPORT void SetGridEcho(const occ::handle<Graphic3d_AspectMarker3d>& aMarker);

  //! Returns TRUE when grid echo must be displayed at hit point.
  bool GridEcho() const { return myGridEcho; }

  //! Returns true if a grid is activated in <me>.
  Standard_EXPORT bool IsGridActive();

  //! Returns the defined grid in <me>.
  occ::handle<Aspect_Grid> Grid(bool theToCreate = true) { return Grid(myGridType, theToCreate); }

  //! Returns the defined grid in <me>.
  Standard_EXPORT occ::handle<Aspect_Grid> Grid(Aspect_GridType theGridType,
                                                bool            theToCreate = true);

  //! Returns the current grid type defined in <me>.
  Aspect_GridType GridType() const { return myGridType; }

  //! Returns the current grid draw mode defined in <me>.
  Standard_EXPORT Aspect_GridDrawMode GridDrawMode();

  //! Returns the definition of the rectangular grid.
  Standard_EXPORT void RectangularGridValues(double& theXOrigin,
                                             double& theYOrigin,
                                             double& theXStep,
                                             double& theYStep,
                                             double& theRotationAngle);

  //! Sets the definition of the rectangular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <XStep> defines the interval between 2 vertical lines.
  //! <YStep> defines the interval between 2 horizontal lines.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetRectangularGridValues(const double XOrigin,
                                                const double YOrigin,
                                                const double XStep,
                                                const double YStep,
                                                const double RotationAngle);

  //! Returns the definition of the circular grid.
  Standard_EXPORT void CircularGridValues(double& theXOrigin,
                                          double& theYOrigin,
                                          double& theRadiusStep,
                                          int&    theDivisionNumber,
                                          double& theRotationAngle);

  //! Sets the definition of the circular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <RadiusStep> defines the interval between 2 circles.
  //! <DivisionNumber> defines the section number of one half circle.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetCircularGridValues(const double XOrigin,
                                             const double YOrigin,
                                             const double RadiusStep,
                                             const int    DivisionNumber,
                                             const double RotationAngle);

  //! Returns the location and the size of the grid.
  Standard_EXPORT void CircularGridGraphicValues(double& theRadius, double& theOffSet);

  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetCircularGridGraphicValues(const double Radius, const double OffSet);

  //! Returns the location and the size of the grid.
  Standard_EXPORT void RectangularGridGraphicValues(double& theXSize,
                                                    double& theYSize,
                                                    double& theOffSet);

  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetRectangularGridGraphicValues(const double XSize,
                                                       const double YSize,
                                                       const double OffSet);

  //! Display grid echo at requested point in the view.
  Standard_EXPORT void ShowGridEcho(const occ::handle<V3d_View>& theView,
                                    const Graphic3d_Vertex&      thePoint);

  //! Temporarily hide grid echo.
  Standard_EXPORT void HideGridEcho(const occ::handle<V3d_View>& theView);

public: //! @name deprecated methods
  //! Returns true if a grid is activated in <me>.
  Standard_DEPRECATED("Deprecated method - IsGridActive() should be used instead")
  bool IsActive() { return IsGridActive(); }

  //! Initializes an internal iterator on the active views.
  Standard_DEPRECATED("Deprecated method - ActiveViews() should be used instead")
  void InitActiveViews() { myActiveViewsIterator.Initialize(myActiveViews); }

  //! Returns true if there are more active view(s) to return.
  Standard_DEPRECATED("Deprecated method - ActiveViews() should be used instead")
  bool MoreActiveViews() const { return myActiveViewsIterator.More(); }

  //! Go to the next active view (if there is not, ActiveView will raise an exception)
  Standard_DEPRECATED("Deprecated method - ActiveViews() should be used instead")
  void NextActiveViews()
  {
    if (!myActiveViews.IsEmpty())
      myActiveViewsIterator.Next();
  }

  Standard_DEPRECATED("Deprecated method - ActiveViews() should be used instead")
  const occ::handle<V3d_View>& ActiveView() const { return myActiveViewsIterator.Value(); }

  //! Initializes an internal iterator on the Defined views.
  Standard_DEPRECATED("Deprecated method - DefinedViews() should be used instead")
  void InitDefinedViews() { myDefinedViewsIterator.Initialize(myDefinedViews); }

  //! returns true if there are more Defined view(s) to return.
  Standard_DEPRECATED("Deprecated method - DefinedViews() should be used instead")
  bool MoreDefinedViews() const { return myDefinedViewsIterator.More(); }

  //! Go to the next Defined view (if there is not, DefinedView will raise an exception)
  Standard_DEPRECATED("Deprecated method - DefinedViews() should be used instead")
  void NextDefinedViews()
  {
    if (!myDefinedViews.IsEmpty())
      myDefinedViewsIterator.Next();
  }

  Standard_DEPRECATED("Deprecated method - DefinedViews() should be used instead")
  const occ::handle<V3d_View>& DefinedView() const { return myDefinedViewsIterator.Value(); }

  //! Initializes an internal iteratator on the active Lights.
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  void InitActiveLights() { myActiveLightsIterator.Initialize(myActiveLights); }

  //! returns true if there are more active Light(s) to return.
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  bool MoreActiveLights() const { return myActiveLightsIterator.More(); }

  //! Go to the next active Light (if there is not, ActiveLight() will raise an exception)
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  void NextActiveLights() { myActiveLightsIterator.Next(); }

  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  const occ::handle<V3d_Light>& ActiveLight() const { return myActiveLightsIterator.Value(); }

  //! Initializes an internal iterattor on the Defined Lights.
  Standard_DEPRECATED("Deprecated method - DefinedLights() should be used instead")
  void InitDefinedLights() { myDefinedLightsIterator.Initialize(myDefinedLights); }

  //! Returns true if there are more Defined Light(s) to return.
  Standard_DEPRECATED("Deprecated method - DefinedLights() should be used instead")
  bool MoreDefinedLights() const { return myDefinedLightsIterator.More(); }

  //! Go to the next Defined Light (if there is not, DefinedLight() will raise an exception)
  Standard_DEPRECATED("Deprecated method - DefinedLights() should be used instead")
  void NextDefinedLights()
  {
    if (!myDefinedLights.IsEmpty())
      myDefinedLightsIterator.Next();
  }

  Standard_DEPRECATED("Deprecated method - DefinedLights() should be used instead")
  const occ::handle<V3d_Light>& DefinedLight() const { return myDefinedLightsIterator.Value(); }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

private:
  //! Returns the default background colour.
  const Aspect_Background& GetBackgroundColor() const { return myBackground; }

  //! Adds View in Sequence Of Views.
  Standard_EXPORT void AddView(const occ::handle<V3d_View>& theView);

  //! Delete View in Sequence Of Views.
  Standard_EXPORT void DelView(const V3d_View* theView);

private:
  occ::handle<Graphic3d_GraphicDriver>    myDriver;
  occ::handle<Graphic3d_StructureManager> myStructureManager;
  NCollection_Map<int>                    myLayerIds;
  Aspect_GenId                            myZLayerGenId;

  NCollection_List<occ::handle<V3d_View>>         myDefinedViews;
  NCollection_List<occ::handle<V3d_View>>         myActiveViews;
  NCollection_List<occ::handle<Graphic3d_CLight>> myDefinedLights;
  NCollection_List<occ::handle<Graphic3d_CLight>> myActiveLights;

  Aspect_Background         myBackground;
  Aspect_GradientBackground myGradientBackground;
  double                    myViewSize;
  V3d_TypeOfOrientation     myViewProj;
  V3d_TypeOfVisualization   myVisualization;
  V3d_TypeOfView            myDefaultTypeOfView;
  Graphic3d_RenderingParams myDefaultRenderingParams;

  NCollection_List<occ::handle<V3d_View>>::Iterator         myActiveViewsIterator;
  NCollection_List<occ::handle<V3d_View>>::Iterator         myDefinedViewsIterator;
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator myActiveLightsIterator;
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator myDefinedLightsIterator;

  bool myComputedMode;
  bool myDefaultComputedMode;

  gp_Ax3                           myPrivilegedPlane;
  occ::handle<Graphic3d_Structure> myPlaneStructure;
  bool                             myDisplayPlane;
  double                           myDisplayPlaneLength;

  occ::handle<V3d_RectangularGrid>      myRGrid;
  occ::handle<V3d_CircularGrid>         myCGrid;
  Aspect_GridType                       myGridType;
  bool                                  myGridEcho;
  occ::handle<Graphic3d_Structure>      myGridEchoStructure;
  occ::handle<Graphic3d_Group>          myGridEchoGroup;
  occ::handle<Graphic3d_AspectMarker3d> myGridEchoAspect;
  Graphic3d_Vertex                      myGridEchoLastVert;
};

#endif // _V3d_Viewer_HeaderFile
