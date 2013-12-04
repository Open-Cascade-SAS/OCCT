// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_Viewer :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      15-11-97 : FMN ; Ajout texture mapping
      02-02-98 : FMN ; Mise a niveau WNT
      23-02-98 : FMN ; Remplacement PI par Standard_PI
      16-07-98 : CAL ; S3892. Ajout grilles 3d.

************************************************************************/

//GER61351	//GG_15/12/99 Adds SetDefaultBackgroundColor() and DefaultBackgroundColor() methods

#define IMP240100	//GG 
//			Initalize grid echo fields

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d.hxx>
#include <Visual3d_View.hxx>
#include <Visual3d_Light.hxx>
#include <V3d_Viewer.ixx>
#include <V3d_View.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_OrthographicView.hxx>
#include <V3d_PerspectiveView.hxx>

/*----------------------------------------------------------------------*/

//-Constructor:
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
myDisplayPlaneLength (theViewSize)
#ifdef IMP240100
,myGridEcho (Standard_True), myGridEchoStructure(), myGridEchoGroup()
#endif 
{
  MyViewer = new Visual3d_ViewManager (theDriver);
  // san (16/09/2010): It has been decided to turn depth test ON
  // by default, as this is important for new font rendering
  // (without it, there are numerous texture rendering artefacts)
  MyViewer->SetZBufferAuto (Standard_False);
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

//-Methods, in order


Handle(V3d_View) V3d_Viewer::CreateView () {
  if (MyDefaultTypeOfView == V3d_ORTHOGRAPHIC) 
    return new V3d_OrthographicView(this);
  else
    return new V3d_PerspectiveView(this);
}

void V3d_Viewer::SetViewOn( ) {

  for (InitDefinedViews();MoreDefinedViews();NextDefinedViews()){
    SetViewOn(ActiveView());};
}

void V3d_Viewer::SetViewOff( ) {

  for (InitDefinedViews();MoreDefinedViews();NextDefinedViews()){
    SetViewOff(ActiveView());};
}

void V3d_Viewer::SetViewOn( const Handle(V3d_View)& TheView ) {

  Handle(Visual3d_View) MyView = TheView->View() ;
  if( MyView->IsDefined() && !IsActive(TheView)) {
    MyActiveViews.Append(TheView) ;
    MyView->Activate();
    for (InitActiveLights();MoreActiveLights();NextActiveLights()){
      TheView->SetLightOn(ActiveLight());}
    // Grid
    TheView->SetGrid (myPrivilegedPlane, Grid ());
    TheView->SetGridActivity (Grid ()->IsActive ());
    // Update
    MyView->Redraw() ;
  }
}

void V3d_Viewer::SetViewOff( const Handle(V3d_View)& TheView ) {

  Handle(Visual3d_View) MyView =TheView->View(); 
  if( MyView->IsDefined() && IsActive(TheView) ) {
    MyActiveViews.Remove(TheView);
    MyView->Deactivate() ;
  }
}

Standard_Boolean V3d_Viewer::ComputedMode() const {
  return myComputedMode;
}

Standard_Boolean V3d_Viewer::DefaultComputedMode() const {
  return myDefaultComputedMode;
}

void V3d_Viewer::Update() {

  MyViewer->Update();
}

void V3d_Viewer::Redraw()const  {

  MyViewer->Redraw();
}

void V3d_Viewer::Remove() {

  MyViewer->Remove();
}

void V3d_Viewer::Erase() const {
  
  MyViewer->Erase();
}

void V3d_Viewer::UnHighlight() const {

  //FMN MyViewer->UnHighlight();
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
void V3d_Viewer::SetZBufferManagment(const Standard_Boolean Automatic) {
  MyViewer->SetZBufferAuto (Automatic);
}
Standard_Boolean V3d_Viewer::ZBufferManagment() const {
  return MyViewer->ZBufferAuto();
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


void V3d_Viewer::SetUpdateMode(const V3d_TypeOfUpdate Mode) {
  
  MyViewer->SetUpdateMode((Aspect_TypeOfUpdate)Mode) ;
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

V3d_TypeOfUpdate V3d_Viewer::UpdateMode() const {

  V3d_TypeOfUpdate Mode = (V3d_TypeOfUpdate) MyViewer->UpdateMode() ;
  return Mode ;
}

Standard_Boolean V3d_Viewer::IfMoreViews() const {
  Standard_Boolean TheStatus = Standard_False ;

#ifdef NEW
  if( MyActiveViews->Length() < Visual3d_View::Limit() )
#endif /*NEW*/
    TheStatus = Standard_True ;
  return TheStatus ;
}

Handle(Visual3d_ViewManager) V3d_Viewer::Viewer() const {
  return MyViewer ;
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
  return MyViewer->AddZLayer (theLayerId);
}

//=======================================================================
//function : RemoveZLayer
//purpose  : 
//=======================================================================

Standard_Boolean V3d_Viewer::RemoveZLayer (const Standard_Integer theLayerId)
{
  return MyViewer->RemoveZLayer (theLayerId);
}

//=======================================================================
//function : GetAllZLayers
//purpose  :
//=======================================================================

void V3d_Viewer::GetAllZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  MyViewer->GetAllZLayers (theLayerSeq);
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
