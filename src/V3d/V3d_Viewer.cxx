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


#include <Aspect_Background.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_IdentDefinitionError.hxx>
#include <gp_Ax3.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Quantity_Color.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_Light.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_Viewer,MMgt_TShared)

// ========================================================================
// function : V3d_Viewer
// purpose  :
// ========================================================================
V3d_Viewer::V3d_Viewer (const Handle(Graphic3d_GraphicDriver)& theDriver,
                        const Standard_ExtString      theName,
                        const Standard_CString        theDomain,
                        const Standard_Real           theViewSize,
                        const V3d_TypeOfOrientation   theViewProj,
                        const Quantity_NameOfColor    theViewBackground,
                        const V3d_TypeOfVisualization theVisualization,
                        const V3d_TypeOfShadingModel  theShadingModel,
                        const V3d_TypeOfUpdate        theUpdateMode,
                        const Standard_Boolean        theComputedMode,
                        const Standard_Boolean        theDefaultComputedMode,
                        const V3d_TypeOfSurfaceDetail theSurfaceDetail)  
:myNextCount (-1),
myDriver (theDriver),
myName (TCollection_ExtendedString (theName)),
myDomain (TCollection_AsciiString (theDomain)),
myStructureManager (new Graphic3d_StructureManager (theDriver)),
MyDefinedViews(),
MyActiveViews(),
MyDefinedLights(),
MyActiveLights(),
myActiveViewsIterator(),
myDefinedViewsIterator(),
myActiveLightsIterator(),
myDefinedLightsIterator(),
myComputedMode (theComputedMode),
myDefaultComputedMode (theDefaultComputedMode),
myPrivilegedPlane (gp_Ax3 (gp_Pnt (0.,0.,0), gp_Dir (0.,0.,1.), gp_Dir (1.,0.,0.))),
myDisplayPlane (Standard_False),
myDisplayPlaneLength (theViewSize),
myGridEcho (Standard_True),
myGridEchoLastVert (ShortRealLast(), ShortRealLast(), ShortRealLast()),
myZLayerGenId (1, IntegerLast())
{
  SetUpdateMode (theUpdateMode);
  SetDefaultViewSize (theViewSize);
  SetDefaultViewProj (theViewProj);
  SetDefaultBackgroundColor (theViewBackground);
  SetDefaultVisualization (theVisualization);
  SetDefaultShadingModel (theShadingModel);
  SetDefaultSurfaceDetail (theSurfaceDetail); 
  SetDefaultAngle (M_PI / 2.);
  SetDefaultTypeOfView (V3d_ORTHOGRAPHIC);

  Quantity_Color Color1 (Quantity_NOC_GRAY50);
  Quantity_Color Color2 (Quantity_NOC_GRAY70);
//  Quantity_Color White (Quantity_NOC_WHITE);
  myRGrid = new V3d_RectangularGrid (this, Color1, Color2);
  myCGrid = new V3d_CircularGrid (this, Color1, Color2);
  myGridType = Aspect_GT_Rectangular;
}

// ========================================================================
// function : CreateView
// purpose  :
// ========================================================================
Handle(V3d_View) V3d_Viewer::CreateView ()
{
  return new V3d_View(this, MyDefaultTypeOfView);
}

// ========================================================================
// function : SetViewOn
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOn()
{
  for (InitDefinedViews();MoreDefinedViews();NextDefinedViews())
  {
    SetViewOn (ActiveView());
  }
}

// ========================================================================
// function : SetViewOff
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOff()
{
  for (InitDefinedViews();MoreDefinedViews();NextDefinedViews())
  {
    SetViewOff (ActiveView());
  }
}

// ========================================================================
// function : SetViewOn
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOn (const Handle(V3d_View)& theView)
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  if (aViewImpl->IsDefined() && !IsActive (theView))
  {
    MyActiveViews.Append (theView);
    aViewImpl->Activate();
    for (InitActiveLights();MoreActiveLights();NextActiveLights())
    {
      theView->SetLightOn (ActiveLight());
    }

    theView->SetGrid (myPrivilegedPlane, Grid ());
    theView->SetGridActivity (Grid ()->IsActive ());
    theView->Redraw();
  }
}

// ========================================================================
// function : SetViewOff
// purpose  :
// ========================================================================
void V3d_Viewer::SetViewOff (const Handle(V3d_View)& theView)
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  if (aViewImpl->IsDefined() && IsActive (theView))
  {
    MyActiveViews.Remove (theView);
    aViewImpl->Deactivate() ;
  }
}

// ========================================================================
// function : ComputedMode
// purpose  :
// ========================================================================
Standard_Boolean V3d_Viewer::ComputedMode() const
{
  return myComputedMode;
}

// ========================================================================
// function : DefaultComputedMode
// purpose  :
// ========================================================================
Standard_Boolean V3d_Viewer::DefaultComputedMode() const
{
  return myDefaultComputedMode;
}

// ========================================================================
// function : Update
// purpose  :
// ========================================================================
void V3d_Viewer::Update()
{
  // Redraw() is still here for compatibility with old code.
  // See comments, the method is deprecated - Redraw() should
  // be used instead.
  Redraw();
}

// ========================================================================
// function : Redraw
// purpose  :
// ========================================================================
void V3d_Viewer::Redraw()const
{
  TColStd_ListIteratorOfListOfTransient anIt (MyDefinedViews);
  for (; anIt.More(); anIt.Next())
  {
    Handle(V3d_View)::DownCast (anIt.Value())->Redraw();
  }
}

// ========================================================================
// function : RedrawImmediate
// purpose  :
// ========================================================================
void V3d_Viewer::RedrawImmediate() const
{
  TColStd_ListIteratorOfListOfTransient anIt (MyDefinedViews);
  for (; anIt.More(); anIt.Next())
  {
    Handle(V3d_View)::DownCast (anIt.Value())->RedrawImmediate();
  }
}

// ========================================================================
// function : Invalidate
// purpose  :
// ========================================================================
void V3d_Viewer::Invalidate() const
{
  TColStd_ListIteratorOfListOfTransient anIt (MyDefinedViews);
  for (; anIt.More(); anIt.Next())
  {
    Handle(V3d_View)::DownCast (anIt.Value())->Invalidate();
  }
}

// ========================================================================
// function : Remove
// purpose  :
// ========================================================================
void V3d_Viewer::Remove()
{
  myStructureManager->Remove();
}

// ========================================================================
// function : Erase
// purpose  :
// ========================================================================
void V3d_Viewer::Erase() const
{
  myStructureManager->Erase();
}

// ========================================================================
// function : UnHighlight
// purpose  :
// ========================================================================
void V3d_Viewer::UnHighlight() const
{
  myStructureManager->UnHighlight();
}

void V3d_Viewer::SetDefaultBackgroundColor(const Quantity_TypeOfColor Type, const Standard_Real v1, const Standard_Real v2, const Standard_Real v3) {
  Standard_Real V1 = v1 ;
  Standard_Real V2 = v2 ;
  Standard_Real V3 = v3 ;

  if( V1 < 0. ) V1 = 0. ; else if( V1 > 1. ) V1 = 1. ;
  if( V2 < 0. ) V2 = 0. ; else if( V2 > 1. ) V2 = 1. ;
  if( V3 < 0. ) V3 = 0. ; else if( V3 > 1. ) V3 = 1. ;

  Quantity_Color C(V1,V2,V3,Type) ;
  SetDefaultBackgroundColor(C);
}

void V3d_Viewer::SetDefaultBackgroundColor(const Quantity_NameOfColor Name)
{
  Quantity_Color C(Name) ;
  SetDefaultBackgroundColor(C);
}

void V3d_Viewer::SetDefaultBackgroundColor(const Quantity_Color &Color)
{
  MyBackground.SetColor(Color) ;
}

void V3d_Viewer::SetDefaultBgGradientColors( const Quantity_NameOfColor Name1,
                                             const Quantity_NameOfColor Name2,
                                             const Aspect_GradientFillMethod FillStyle){

  Quantity_Color C1(Name1) ;
  Quantity_Color C2(Name2) ;
  MyGradientBackground.SetColors(C1, C2, FillStyle);

} 

void V3d_Viewer::SetDefaultBgGradientColors( const Quantity_Color& Color1,
                                             const Quantity_Color& Color2,
                                             const Aspect_GradientFillMethod FillStyle ){

  MyGradientBackground.SetColors(Color1, Color2, FillStyle);

}  


void V3d_Viewer::SetDefaultViewSize(const Standard_Real Size) {

  V3d_BadValue_Raise_if( Size <= 0. ,"V3d_Viewer::SetDefaultViewSize, bad size");
  MyViewSize = Size ;
}

void V3d_Viewer::SetDefaultViewProj(const V3d_TypeOfOrientation Orientation) {

  MyViewProj = Orientation ;
}

void V3d_Viewer::SetDefaultVisualization(const V3d_TypeOfVisualization Type) {

  MyVisualization = Type ;
}

void V3d_Viewer::SetDefaultShadingModel(const V3d_TypeOfShadingModel Type) {

  MyShadingModel = Type ;
}

void V3d_Viewer::SetDefaultSurfaceDetail(const V3d_TypeOfSurfaceDetail Type) {

  MySurfaceDetail = Type ;
}

void V3d_Viewer::SetDefaultAngle(const Quantity_PlaneAngle Angle) {
  MyDefaultAngle = Angle;
}

void V3d_Viewer::SetDefaultTypeOfView(const V3d_TypeOfView Type) {
  MyDefaultTypeOfView = Type;}

// ========================================================================
// function : SetUpdateMode
// purpose  :
// ========================================================================
void V3d_Viewer::SetUpdateMode (const V3d_TypeOfUpdate theMode)
{
  myStructureManager->SetUpdateMode (static_cast<Aspect_TypeOfUpdate> (theMode));
}

void V3d_Viewer::DefaultBackgroundColor(const Quantity_TypeOfColor Type,Standard_Real &V1,Standard_Real &V2,Standard_Real &V3) const
{
  Quantity_Color C = DefaultBackgroundColor();
  C.Values(V1,V2,V3,Type) ;
}

Quantity_Color V3d_Viewer::DefaultBackgroundColor() const
{
  return MyBackground.Color() ;
}

void V3d_Viewer::DefaultBgGradientColors(Quantity_Color& Color1,Quantity_Color& Color2) const
{
  MyGradientBackground.Colors(Color1,Color2);     
}

Standard_Real V3d_Viewer::DefaultViewSize() const {
  return MyViewSize ;
}

V3d_TypeOfOrientation V3d_Viewer::DefaultViewProj() const {
  return MyViewProj ;
}

V3d_TypeOfVisualization V3d_Viewer::DefaultVisualization() const {
  return MyVisualization ;
}

V3d_TypeOfShadingModel V3d_Viewer::DefaultShadingModel() const {
  return MyShadingModel ;
}

V3d_TypeOfSurfaceDetail V3d_Viewer::DefaultSurfaceDetail() const {
  return MySurfaceDetail ;
}

Quantity_PlaneAngle V3d_Viewer::DefaultAngle() const {
  return MyDefaultAngle;
}

// ========================================================================
// function : UpdateMode
// purpose  :
// ========================================================================
V3d_TypeOfUpdate V3d_Viewer::UpdateMode() const
{
  return static_cast<V3d_TypeOfUpdate> (myStructureManager->UpdateMode());
}

Standard_Boolean V3d_Viewer::IfMoreViews() const {
  Standard_Boolean TheStatus = Standard_False ;

#ifdef NEW
  if (MyActiveViews->Length() < myDriver->InquireViewLimit())
#endif /*NEW*/
    TheStatus = Standard_True ;
  return TheStatus ;
}

// ========================================================================
// function : StructureManager
// purpose  :
// ========================================================================
Handle(Graphic3d_StructureManager) V3d_Viewer::StructureManager() const
{
  return myStructureManager;
}

Aspect_Background V3d_Viewer::GetBackgroundColor() const {
  return MyBackground ;
}

Aspect_GradientBackground V3d_Viewer::GetGradientBackground() const {
  return MyGradientBackground;
}   

void V3d_Viewer::AddView( const Handle(V3d_View)& TheView ) {

  MyDefinedViews.Append(TheView);
  IncrCount();
}

void V3d_Viewer::DelView( const Handle(V3d_View)& TheView ) {

  MyActiveViews.Remove(TheView);
  MyDefinedViews.Remove(TheView);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================
Standard_Boolean V3d_Viewer::AddZLayer (Standard_Integer& theLayerId)
{
  try
  {
    OCC_CATCH_SIGNALS
    theLayerId = myZLayerGenId.Next();
  }
  catch (Aspect_IdentDefinitionError)
  {
    // new index can't be generated
    return Standard_False;
  }

  myLayerIds.Add (theLayerId);
  myDriver->AddZLayer (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : RemoveZLayer
//purpose  : 
//=======================================================================
Standard_Boolean V3d_Viewer::RemoveZLayer (const Standard_Integer theLayerId)
{
  if (!myLayerIds.Contains (theLayerId)
    || theLayerId < myZLayerGenId.Lower()
    || theLayerId > myZLayerGenId.Upper())
  {
    return Standard_False;
  }

  myDriver->RemoveZLayer (theLayerId);
  myLayerIds.Remove  (theLayerId);
  myZLayerGenId.Free (theLayerId);

  return Standard_True;
}

//=======================================================================
//function : GetAllZLayers
//purpose  :
//=======================================================================
void V3d_Viewer::GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  myDriver->ZLayers (theLayerSeq);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void V3d_Viewer::SetZLayerSettings (const Standard_Integer theLayerId, const Graphic3d_ZLayerSettings& theSettings)
{
  myDriver->SetZLayerSettings (theLayerId, theSettings);
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
Graphic3d_ZLayerSettings V3d_Viewer::ZLayerSettings (const Standard_Integer theLayerId)
{
  return myDriver->ZLayerSettings (theLayerId);
}

//=======================================================================
//function : Domain
//purpose  :
//=======================================================================
Standard_CString V3d_Viewer::Domain() const
{
  return myDomain.ToCString();
}

//=======================================================================
//function : Driver
//purpose  :
//=======================================================================
const Handle(Graphic3d_GraphicDriver)& V3d_Viewer::Driver() const
{
  return myDriver;
}

//=======================================================================
//function : NextName
//purpose  :
//=======================================================================
Standard_ExtString V3d_Viewer::NextName() const
{
  TCollection_ExtendedString aNextName = TCollection_ExtendedString (myName.ToExtString());
  aNextName.AssignCat (TCollection_ExtendedString (myNextCount));
  
  return aNextName.ToExtString();
}

//=======================================================================
//function : IncrCount
//purpose  :
//=======================================================================
void V3d_Viewer::IncrCount()
{
  myNextCount++;
}

//=======================================================================
//function : DefaultRenderingParams
//purpose  :
//=======================================================================
const Graphic3d_RenderingParams& V3d_Viewer::DefaultRenderingParams() const
{
  return myDefaultRenderingParams;
}

//=======================================================================
//function : SetDefaultRenderingParams
//purpose  :
//=======================================================================
void V3d_Viewer::SetDefaultRenderingParams (const Graphic3d_RenderingParams& theParams)
{
  myDefaultRenderingParams = theParams;
}
