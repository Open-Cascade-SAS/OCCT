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

/***********************************************************************

     FUNCTION :
     ----------
        Class Visual3d_ContextView.cxx :

	Declaration of variables specific to view contexts.

	A view context is defined by :
		- the activity of aliasing
		- the activity of depth-cueing
		- the activity of Z clipping
		- the activity of defined light sources
		- the required type of visualization
		- the model of shading if required


************************************************************************/

#define BUC60572	//GG 03-08-99 Move Zcueing and Zclipping front & back planes
//                  coherence checking in Visual3d_View::SetContext()


/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Visual3d_ContextView.ixx>

/*----------------------------------------------------------------------*/

Visual3d_ContextView::Visual3d_ContextView ():
AliasingIsActive (Standard_False),
ZcueingIsActive (Standard_False),
FrontZclippingIsActive (Standard_False),
BackZclippingIsActive (Standard_False),
MyZclippingFrontPlane (Standard_ShortReal (1.0)),
MyZclippingBackPlane (Standard_ShortReal (0.0)),
MyDepthCueingFrontPlane (Standard_ShortReal (1.0)),
MyDepthCueingBackPlane (Standard_ShortReal (0.0)),
MyModel (Visual3d_TOM_NONE),
MyVisual (Visual3d_TOV_WIREFRAME),
MyLights (),
MyTextureEnv(),
MySurfaceDetail(Visual3d_TOD_NONE),
myClipPlanes()
{
}

/*----------------------------------------------------------------------*/

void Visual3d_ContextView::SetAliasingOn () {

	AliasingIsActive	= Standard_True;

}

void Visual3d_ContextView::SetAliasingOff () {

	AliasingIsActive	= Standard_False;

}

Standard_Boolean Visual3d_ContextView::AliasingIsOn () const {

	return (AliasingIsActive);

}

void Visual3d_ContextView::SetZClippingOn () {

	BackZclippingIsActive	= Standard_True;
	FrontZclippingIsActive	= Standard_True;

}

void Visual3d_ContextView::SetZClippingOff () {

	BackZclippingIsActive	= Standard_False;
	FrontZclippingIsActive	= Standard_False;

}

Standard_Boolean Visual3d_ContextView::BackZClippingIsOn () const {

	return (BackZclippingIsActive);

}

void Visual3d_ContextView::SetBackZClippingOn () {

	BackZclippingIsActive	= Standard_True;

}

void Visual3d_ContextView::SetBackZClippingOff () {

	BackZclippingIsActive	= Standard_False;

}

Standard_Boolean Visual3d_ContextView::FrontZClippingIsOn () const {

	return (FrontZclippingIsActive);

}

void Visual3d_ContextView::SetFrontZClippingOn () {

	FrontZclippingIsActive	= Standard_True;

}

void Visual3d_ContextView::SetFrontZClippingOff () {

	FrontZclippingIsActive	= Standard_False;

}

void Visual3d_ContextView::SetZClippingFrontPlane (const Standard_Real AFront) {

#ifndef BUC60572
	if ( (FrontZclippingIsActive) && (BackZclippingIsActive) &&
					(MyZclippingBackPlane >= AFront) )
		Visual3d_ZClippingDefinitionError::Raise
			("Bad value for ZClippingFrontPlane");
#endif

	MyZclippingFrontPlane	= Standard_ShortReal (AFront);

}

Standard_Real Visual3d_ContextView::ZClippingFrontPlane () const {

	return (Standard_Real (MyZclippingFrontPlane));

}

void Visual3d_ContextView::SetDepthCueingFrontPlane (const Standard_Real AFront) {

#ifndef BUC60572
	if ( (ZcueingIsActive) && (MyDepthCueingBackPlane >= AFront) )
		Visual3d_DepthCueingDefinitionError::Raise
			("Bad value for DepthCueingFrontPlane");
#endif

	MyDepthCueingFrontPlane	= Standard_ShortReal (AFront);

}

Standard_Real Visual3d_ContextView::DepthCueingFrontPlane () const {

	return (Standard_Real (MyDepthCueingFrontPlane));

}

void Visual3d_ContextView::SetZClippingBackPlane (const Standard_Real ABack) {

#ifndef BUC60572
	if ( (FrontZclippingIsActive) && (FrontZclippingIsActive) &&
					(MyZclippingFrontPlane <= ABack) )
		Visual3d_ZClippingDefinitionError::Raise
			("Bad value for ZClippingBackPlane");
#endif

	MyZclippingBackPlane	= Standard_ShortReal (ABack);

}

Standard_Real Visual3d_ContextView::ZClippingBackPlane () const {

	return (Standard_Real (MyZclippingBackPlane));

}

void Visual3d_ContextView::SetDepthCueingBackPlane (const Standard_Real ABack) {

#ifndef BUC60572
	if ( (ZcueingIsActive) && (MyDepthCueingFrontPlane <= ABack) )
		Visual3d_DepthCueingDefinitionError::Raise
			("Bad value for DepthCueingBackPlane");
#endif

	MyDepthCueingBackPlane	= Standard_ShortReal (ABack);

}

Standard_Real Visual3d_ContextView::DepthCueingBackPlane () const {

	return (Standard_Real (MyDepthCueingBackPlane));

}

void Visual3d_ContextView::SetDepthCueingOn () {

	ZcueingIsActive	= Standard_True;

}

void Visual3d_ContextView::SetDepthCueingOff () {

	ZcueingIsActive	= Standard_False;

}

Standard_Boolean Visual3d_ContextView::DepthCueingIsOn () const {

	return (ZcueingIsActive);

}

void Visual3d_ContextView::SetModel (const Visual3d_TypeOfModel AModel) {

	MyModel	= AModel;

}

Visual3d_TypeOfModel Visual3d_ContextView::Model () const {

	return (MyModel);

}

void Visual3d_ContextView::SetVisualization (const Visual3d_TypeOfVisualization AVisual) {

	MyVisual	= AVisual;

}

Visual3d_TypeOfVisualization Visual3d_ContextView::Visualization () const {

	return (MyVisual);

}

void Visual3d_ContextView::SetLightOn (const Handle(Visual3d_Light)& ALight) {

Standard_Integer LengthL	= MyLights.Length ();
Standard_Integer indexL = 0;

	// Find light <ALight> in 
	// the sequence of already active lights
	for (Standard_Integer i=1; i<=LengthL && indexL==0; i++)
		if ((void *) (MyLights.Value (i)) ==
		    (void *) (ALight.operator->())) indexL = i;

	// This is the activation of a new light
	if (indexL == 0)
		MyLights.Append ((void *) ALight.operator->());

}

void Visual3d_ContextView::SetLightOff (const Handle(Visual3d_Light)& ALight) {

Standard_Integer LengthL	= MyLights.Length ();
Standard_Integer indexL = 0;

	// Find light <ALight> in 
	// the sequence of already active lights
	for (Standard_Integer i=1; i<=LengthL && indexL==0; i++)
		if ((void *) (MyLights.Value (i)) ==
		    (void *) (ALight.operator->())) indexL = i;

	// This is the activation of a new light
	if (indexL != 0) MyLights.Remove (indexL);

}

Handle(Visual3d_HSequenceOfLight) Visual3d_ContextView::ActivatedLights () const {

Handle(Visual3d_HSequenceOfLight) SG = new Visual3d_HSequenceOfLight();
Standard_Integer Length	= MyLights.Length ();

	for (Standard_Integer i=1; i<=Length; i++)
		SG->Append((Visual3d_Light *) (MyLights.Value (i)));

	return (SG);

}

Standard_Integer Visual3d_ContextView::NumberOfActivatedLights () const {

Standard_Integer Length	= MyLights.Length ();

	return (Length);

}

Handle(Visual3d_Light) Visual3d_ContextView::ActivatedLight (const Standard_Integer AnIndex) const {

	return (Visual3d_Light *) (MyLights.Value (AnIndex));

}

void Visual3d_ContextView::SetSurfaceDetail(const Visual3d_TypeOfSurfaceDetail TOSD)
{
  MySurfaceDetail = TOSD;
}


void Visual3d_ContextView::SetTextureEnv(const Handle(Graphic3d_TextureEnv)& ATexture)
{
  MyTextureEnv = ATexture;
}


Handle(Graphic3d_TextureEnv) Visual3d_ContextView::TextureEnv() const
{
  return MyTextureEnv;
}


Visual3d_TypeOfSurfaceDetail Visual3d_ContextView::SurfaceDetail() const
{
  return MySurfaceDetail;
}

//=======================================================================
//function : ClipPlanes
//purpose  :
//=======================================================================
const Graphic3d_SequenceOfHClipPlane& Visual3d_ContextView::ClipPlanes() const
{
  return myClipPlanes;
}

//=======================================================================
//function : ChangeClipPlanes
//purpose  :
//=======================================================================
Graphic3d_SequenceOfHClipPlane& Visual3d_ContextView::ChangeClipPlanes()
{
  return myClipPlanes;
}
