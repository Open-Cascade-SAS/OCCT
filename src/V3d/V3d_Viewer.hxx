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

#include <Aspect_Background.hxx>
#include <Aspect_GenId.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Aspect_GridType.hxx>

#include <gp_Ax3.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_ZLayerSettings.hxx>

#include <MMgt_TShared.hxx>

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <V3d_ListOfTransient.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfShadingModel.hxx>
#include <V3d_TypeOfSurfaceDetail.hxx>
#include <V3d_TypeOfUpdate.hxx>
#include <V3d_TypeOfView.hxx>
#include <V3d_TypeOfVisualization.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_Parameter.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Quantity_TypeOfColor.hxx>

class Aspect_Grid;
class Graphic3d_AspectMarker3d;
class Graphic3d_GraphicDriver;
class Graphic3d_Group;
class Graphic3d_Structure;
class V3d_BadValue;
class V3d_CircularGrid;
class V3d_Light;
class V3d_RectangularGrid;
class V3d_View;
class Quantity_Color;

class V3d_Viewer;
DEFINE_STANDARD_HANDLE(V3d_Viewer, MMgt_TShared)

//! Defines services on Viewer type objects.
//! The methods of this class allow editing and
//! interrogation of the parameters linked to the viewer
//! its friend classes (View,light,plane).
class V3d_Viewer : public MMgt_TShared
{

public:

  
  //! Create a Viewer with the given graphic driver and the given parameters  or
  //! with their default values.
  //! Currently creating of more than 100 viewer instances
  //! is not supported and leads to an exception.
  //! This limitation might be addressed in some future OCCT releases.
  //! If the size of the view is <= 0
  //! Warning: Client must creates a graphic driver
  Standard_EXPORT V3d_Viewer(const Handle(Graphic3d_GraphicDriver)& theDriver, const Standard_ExtString theName, const Standard_CString theDomain = "", const Quantity_Length theViewSize = 1000.0, const V3d_TypeOfOrientation theViewProj = V3d_XposYnegZpos, const Quantity_NameOfColor theViewBackground = Quantity_NOC_GRAY30, const V3d_TypeOfVisualization theVisualization = V3d_ZBUFFER, const V3d_TypeOfShadingModel theShadingModel = V3d_GOURAUD, const V3d_TypeOfUpdate theUpdateMode = V3d_WAIT, const Standard_Boolean theComputedMode = Standard_True, const Standard_Boolean theDefaultComputedMode = Standard_True, const V3d_TypeOfSurfaceDetail theSurfaceDetail = V3d_TEX_NONE);
  
  //! creates a view in the viewer according to its
  //! default parameters.
  Standard_EXPORT Handle(V3d_View) CreateView();
  
  //! Activates all of the views of a viewer attached
  //! to a window.
  Standard_EXPORT void SetViewOn();
  
  //! Activates a particular view in the Viewer .
  //! Must be call if the Window attached to the view
  //! has been Deiconified .
  Standard_EXPORT void SetViewOn (const Handle(V3d_View)& View);
  
  //! Deactivates all the views of a Viewer
  //! attached to a window.
  Standard_EXPORT void SetViewOff();
  
  //! Deactivates a particular view in the Viewer.
  //! Must be call if the Window attached to the view
  //! has been Iconified .
  Standard_EXPORT void SetViewOff (const Handle(V3d_View)& View);
  
  //! Deprecated, Redraw() should be used instead.
  Standard_EXPORT void Update();
  
  //! Updates the lights of all the views of a viewer.
  Standard_EXPORT void UpdateLights();
  
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
  
  //! Erase all Objects in All the views.
  Standard_EXPORT void Erase() const;
  
  //! UnHighlight all Objects in All the views.
  Standard_EXPORT void UnHighlight() const;
  
  //! Defines the default base colour of views attached
  //! to the Viewer by supplying the type of colour
  //! definition and the three component values..
  Standard_EXPORT void SetDefaultBackgroundColor (const Quantity_TypeOfColor Type, const Quantity_Parameter V1, const Quantity_Parameter V2, const Quantity_Parameter V3);
  
  //! Defines the default background colour of views
  //! attached to the viewer by supplying the name of the
  //! colour under the form Quantity_NOC_xxxx .
  Standard_EXPORT void SetDefaultBackgroundColor (const Quantity_NameOfColor Name);
  
  //! Defines the default background colour of views
  //! attached to the viewer by supplying the color object
  Standard_EXPORT void SetDefaultBackgroundColor (const Quantity_Color& Color);
  
  //! Defines the default gradient background colours of view
  //! attached to the viewer by supplying the name of the
  //! colours under the form Quantity_NOC_xxxx .
  Standard_EXPORT void SetDefaultBgGradientColors (const Quantity_NameOfColor Name1, const Quantity_NameOfColor Name2, const Aspect_GradientFillMethod FillStyle = Aspect_GFM_HOR);
  
  //! Defines the default gradient background colours of views
  //! attached to the viewer by supplying the colour objects
  Standard_EXPORT void SetDefaultBgGradientColors (const Quantity_Color& Color1, const Quantity_Color& Color2, const Aspect_GradientFillMethod FillStyle = Aspect_GFM_HOR);
  
  //! Gives a default size for the creation of views of
  //! the viewer.
  Standard_EXPORT void SetDefaultViewSize (const Quantity_Length Size);
  
  //! Gives the default projection for creating views
  //! in the viewer.
  Standard_EXPORT void SetDefaultViewProj (const V3d_TypeOfOrientation Orientation);
  
  //! Gives the default visualization mode..
  Standard_EXPORT void SetDefaultVisualization (const V3d_TypeOfVisualization Type);

  //! Gives the default type of SHADING.
  Standard_EXPORT void SetDefaultShadingModel (const V3d_TypeOfShadingModel Type);
  
  //! Gives the default type of texture mapping.
  Standard_EXPORT void SetDefaultSurfaceDetail (const V3d_TypeOfSurfaceDetail Type);
  
  Standard_EXPORT void SetDefaultAngle (const Quantity_PlaneAngle Angle);
  
  //! Defines the mode of regenerating the views making
  //! up the viewer. This can be immediate <ASAP> or
  //! deferred <WAIT>. In this latter case, the views are
  //! updated when the method Update(me) is called.
  Standard_EXPORT void SetUpdateMode (const V3d_TypeOfUpdate theMode);
  
  Standard_EXPORT void SetDefaultTypeOfView (const V3d_TypeOfView Type);
  
  Standard_EXPORT void SetPrivilegedPlane (const gp_Ax3& aPlane);
  
  Standard_EXPORT gp_Ax3 PrivilegedPlane() const;
  
  Standard_EXPORT void DisplayPrivilegedPlane (const Standard_Boolean OnOff, const Quantity_Length aSize = 1);
  
  //! Activates MyLight in the viewer.
  Standard_EXPORT void SetLightOn (const Handle(V3d_Light)& MyLight);
  
  //! Activates all the lights defined in this viewer.
  Standard_EXPORT void SetLightOn();
  
  //! Deactivates MyLight in this viewer.
  Standard_EXPORT void SetLightOff (const Handle(V3d_Light)& MyLight);
  
  //! Deactivate all the Lights defined in this viewer.
  Standard_EXPORT void SetLightOff();
  
  //! Delete Light in Sequence Of Lights.
  Standard_EXPORT void DelLight (const Handle(V3d_Light)& MyLight);
  
  //! Defines the selected light.
  Standard_EXPORT void SetCurrentSelectedLight (const Handle(V3d_Light)& TheLight);
  
  //! Defines the selected light at NULL.
  Standard_EXPORT void ClearCurrentSelectedLight();
  
  //! Returns the default background colour depending of the type.
  Standard_EXPORT void DefaultBackgroundColor (const Quantity_TypeOfColor Type, Quantity_Parameter& V1, Quantity_Parameter& V2, Quantity_Parameter& V3) const;
  
  //! Returns the default background colour object.
  Standard_EXPORT Quantity_Color DefaultBackgroundColor() const;
  
  //! Returns the gradient background colour objects of the view.
  Standard_EXPORT void DefaultBgGradientColors (Quantity_Color& Color1, Quantity_Color& Color2) const;
  
  //! Returns the default size of the view.
  Standard_EXPORT Quantity_Length DefaultViewSize() const;
  
  //! Returns the default Projection.
  Standard_EXPORT V3d_TypeOfOrientation DefaultViewProj() const;
  
  //! Returns the default type of Visualization.
  Standard_EXPORT V3d_TypeOfVisualization DefaultVisualization() const;
  
  //! Returns the default type of Shading
  Standard_EXPORT V3d_TypeOfShadingModel DefaultShadingModel() const;
  
  //! Returns the default type of texture mapping
  Standard_EXPORT V3d_TypeOfSurfaceDetail DefaultSurfaceDetail() const;
  
  Standard_EXPORT Quantity_PlaneAngle DefaultAngle() const;
  
  //! Returns the regeneration mode of views in the viewer.
  Standard_EXPORT V3d_TypeOfUpdate UpdateMode() const;
  
  //! Returns True if One View more can be
  //! activated in this Viewer.
  Standard_EXPORT Standard_Boolean IfMoreViews() const;
  
  //! initializes an iteration on the active views.
  Standard_EXPORT void InitActiveViews();
  
  //! returns true if there are more active view(s) to return.
  Standard_EXPORT Standard_Boolean MoreActiveViews() const;
  
  //! Go to the next active view
  //! (if there is not, ActiveView will raise an exception)
  Standard_EXPORT void NextActiveViews();
  
  Standard_EXPORT Handle(V3d_View) ActiveView() const;
  
  //! returns true if there is only
  //! one active view.
  Standard_EXPORT Standard_Boolean LastActiveView() const;
  
  //! initializes an iteration on the Defined views.
  Standard_EXPORT void InitDefinedViews();
  
  //! returns true if there are more Defined view(s) to return.
  Standard_EXPORT Standard_Boolean MoreDefinedViews() const;
  
  //! Go to the next Defined view
  //! (if there is not, DefinedView will raise an exception)
  Standard_EXPORT void NextDefinedViews();
  
  Standard_EXPORT Handle(V3d_View) DefinedView() const;
  
  //! initializes an iteration on the active Lights.
  Standard_EXPORT void InitActiveLights();
  
  //! returns true if there are more active Light(s) to return.
  Standard_EXPORT Standard_Boolean MoreActiveLights() const;
  
  //! Go to the next active Light
  //! (if there is not, ActiveLight will raise an exception)
  Standard_EXPORT void NextActiveLights();
  
  Standard_EXPORT Handle(V3d_Light) ActiveLight() const;
  
  //! initializes an iteration on the Defined Lights.
  Standard_EXPORT void InitDefinedLights();
  
  //! returns true if there are more Defined Light(s) to return.
  Standard_EXPORT Standard_Boolean MoreDefinedLights() const;
  
  //! Go to the next Defined Light
  //! (if there is not, DefinedLight will raise an exception)
  Standard_EXPORT void NextDefinedLights();
  
  Standard_EXPORT Handle(V3d_Light) DefinedLight() const;
  
  //! Returns the structure manager associated to this viewer.
  Standard_EXPORT Handle(Graphic3d_StructureManager) StructureManager() const;
  
  //! Returns the Selected Light.
  Standard_EXPORT Handle(V3d_Light) CurrentSelectedLight() const;
  
  Standard_EXPORT Standard_Boolean IsGlobalLight (const Handle(V3d_Light)& TheLight) const;
  
  //! returns true if the computed mode can be used.
  Standard_EXPORT Standard_Boolean ComputedMode() const;
  
  //! returns true if by default the computed mode must be used.
  Standard_EXPORT Standard_Boolean DefaultComputedMode() const;
  
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
  
  //! Returns TRUE when grid echo must be displayed
  //! at hit point.
  Standard_EXPORT Standard_Boolean GridEcho() const;
  
  //! Returns Standard_True if a grid is activated in <me>.
  Standard_EXPORT Standard_Boolean IsActive() const;
  
  //! Returns the defined grid in <me>.
  Standard_EXPORT Handle(Aspect_Grid) Grid() const;
  
  //! Returns the current grid type defined in <me>.
  Standard_EXPORT Aspect_GridType GridType() const;
  
  //! Returns the current grid draw mode defined in <me>.
  Standard_EXPORT Aspect_GridDrawMode GridDrawMode() const;
  
  //! Returns the definition of the rectangular grid.
  Standard_EXPORT void RectangularGridValues (Quantity_Length& XOrigin, Quantity_Length& YOrigin, Quantity_Length& XStep, Quantity_Length& YStep, Quantity_PlaneAngle& RotationAngle) const;
  
  //! Sets the definition of the rectangular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <XStep> defines the interval between 2 vertical lines.
  //! <YStep> defines the interval between 2 horizontal lines.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetRectangularGridValues (const Quantity_Length XOrigin, const Quantity_Length YOrigin, const Quantity_Length XStep, const Quantity_Length YStep, const Quantity_PlaneAngle RotationAngle);
  
  //! Returns the definition of the circular grid.
  Standard_EXPORT void CircularGridValues (Quantity_Length& XOrigin, Quantity_Length& YOrigin, Quantity_Length& RadiusStep, Standard_Integer& DivisionNumber, Quantity_PlaneAngle& RotationAngle) const;
  
  //! Sets the definition of the circular grid.
  //! <XOrigin>, <YOrigin> defines the origin of the grid.
  //! <RadiusStep> defines the interval between 2 circles.
  //! <DivisionNumber> defines the section number of one half circle.
  //! <RotationAngle> defines the rotation angle of the grid.
  Standard_EXPORT void SetCircularGridValues (const Quantity_Length XOrigin, const Quantity_Length YOrigin, const Quantity_Length RadiusStep, const Standard_Integer DivisionNumber, const Quantity_PlaneAngle RotationAngle);
  
  //! Returns the location and the size of the grid.
  Standard_EXPORT void CircularGridGraphicValues (Quantity_Length& Radius, Quantity_Length& OffSet) const;
  
  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetCircularGridGraphicValues (const Quantity_Length Radius, const Quantity_Length OffSet);
  
  //! Returns the location and the size of the grid.
  Standard_EXPORT void RectangularGridGraphicValues (Quantity_Length& XSize, Quantity_Length& YSize, Quantity_Length& OffSet) const;
  
  //! Sets the location and the size of the grid.
  //! <XSize> defines the width of the grid.
  //! <YSize> defines the height of the grid.
  //! <OffSet> defines the displacement along the plane normal.
  Standard_EXPORT void SetRectangularGridGraphicValues (const Quantity_Length XSize, const Quantity_Length YSize, const Quantity_Length OffSet);
  
  //! Returns the gradient background of the view.
  Standard_EXPORT Aspect_GradientBackground GetGradientBackground() const;
  
  //! defines default lights  -
  //! positional-light 0.3 0. 0.
  //! directional-light V3d_XnegYposZpos
  //! directional-light V3d_XnegYneg
  //! ambient-light
  Standard_EXPORT void SetDefaultLights();
  

  //! Display grid echo at requested point in the view.
  Standard_EXPORT void ShowGridEcho (const Handle(V3d_View)& theView, const Graphic3d_Vertex& thePoint);
  

  //! Temporarly hide grid echo.
  Standard_EXPORT void HideGridEcho (const Handle(V3d_View)& theView);

  //! Add a new top-level Z layer to all managed views and get
  //! its ID as <theLayerId> value. The Z layers are controlled entirely
  //! by viewer, it is not possible to add a layer to a
  //! particular view. The method returns Standard_False if the layer can
  //! not be created. The layer mechanism allows to display structures
  //! in higher layers in overlay of structures in lower layers.
  Standard_EXPORT Standard_Boolean AddZLayer (Standard_Integer& theLayerId);

  //! Remove Z layer with ID <theLayerId>. Method returns
  //! Standard_False if the layer can not be removed or doesn't exists.
  //! By default, there are always default bottom-level layer that can't
  //! be removed.
  Standard_EXPORT Standard_Boolean RemoveZLayer (const Standard_Integer theLayerId);

  //! Return all Z layer ids in sequence ordered by overlay level
  //! from lowest layer to highest ( foreground ). The first layer ID
  //! in sequence is the default layer that can't be removed.
  Standard_EXPORT void GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const;

  //! Sets the settings for a single Z layer.
  Standard_EXPORT void SetZLayerSettings (const Standard_Integer theLayerId, const Graphic3d_ZLayerSettings& theSettings);

  //! Returns the settings of a single Z layer.
  Standard_EXPORT Graphic3d_ZLayerSettings ZLayerSettings (const Standard_Integer theLayerId);

  Standard_EXPORT const Handle(Graphic3d_GraphicDriver)& Driver() const;

  Standard_EXPORT Standard_ExtString NextName() const;

  Standard_EXPORT Standard_CString Domain() const;

friend class V3d_View;
friend class V3d_Light;

  DEFINE_STANDARD_RTTI(V3d_Viewer,MMgt_TShared)

protected:

  Standard_EXPORT void IncrCount();

private:

  //! Returns the default background colour.
  Standard_EXPORT Aspect_Background GetBackgroundColor() const;
  
  //! Adds View in Sequence Of Views.
  Standard_EXPORT void AddView (const Handle(V3d_View)& MyView);
  
  //! Delete View in Sequence Of Views.
  Standard_EXPORT void DelView (const Handle(V3d_View)& MyView);
  
  //! Adds Light in Sequence Of Lights.
  Standard_EXPORT void AddLight (const Handle(V3d_Light)& MyLight);
  
  Standard_EXPORT Standard_Boolean IsActive (const Handle(V3d_View)& aView) const;

private:

  Standard_Integer myNextCount;
  Handle(Graphic3d_GraphicDriver) myDriver;
  TCollection_ExtendedString myName;
  TCollection_AsciiString myDomain;
  Handle(Graphic3d_StructureManager) myStructureManager;
  V3d_ListOfTransient MyDefinedViews;
  V3d_ListOfTransient MyActiveViews;
  V3d_ListOfTransient MyDefinedLights;
  V3d_ListOfTransient MyActiveLights;
  Aspect_Background MyBackground;
  Aspect_GradientBackground MyGradientBackground;
  Standard_Real MyViewSize;
  V3d_TypeOfOrientation MyViewProj;
  V3d_TypeOfVisualization MyVisualization;
  V3d_TypeOfShadingModel MyShadingModel;
  V3d_TypeOfSurfaceDetail MySurfaceDetail;
  Quantity_PlaneAngle MyDefaultAngle;
  V3d_TypeOfView MyDefaultTypeOfView;
  Handle(V3d_Light) MyCurrentSelectedLight;
  TColStd_ListIteratorOfListOfTransient myActiveViewsIterator;
  TColStd_ListIteratorOfListOfTransient myDefinedViewsIterator;
  TColStd_ListIteratorOfListOfTransient myActiveLightsIterator;
  TColStd_ListIteratorOfListOfTransient myDefinedLightsIterator;
  Standard_Boolean myComputedMode;
  Standard_Boolean myDefaultComputedMode;
  gp_Ax3 myPrivilegedPlane;
  Handle(Graphic3d_Structure) myPlaneStructure;
  Standard_Boolean myDisplayPlane;
  Quantity_Length myDisplayPlaneLength;
  Handle(V3d_RectangularGrid) myRGrid;
  Handle(V3d_CircularGrid) myCGrid;
  Aspect_GridType myGridType;
  Standard_Boolean myGridEcho;
  Handle(Graphic3d_Structure) myGridEchoStructure;
  Handle(Graphic3d_Group) myGridEchoGroup;
  Handle(Graphic3d_AspectMarker3d) myGridEchoAspect;
  Graphic3d_Vertex myGridEchoLastVert;
  TColStd_MapOfInteger myLayerIds;
  Aspect_GenId myZLayerGenId;
};

#endif // _V3d_Viewer_HeaderFile
