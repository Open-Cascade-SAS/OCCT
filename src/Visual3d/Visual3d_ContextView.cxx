// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
MyClipPlanes (),
MyTextureEnv(),
MySurfaceDetail(Visual3d_TOD_NONE)
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

Handle(Visual3d_HSetOfLight) Visual3d_ContextView::ActivatedLights () const {

Handle(Visual3d_HSetOfLight) SG = new Visual3d_HSetOfLight ();
Standard_Integer Length	= MyLights.Length ();

	for (Standard_Integer i=1; i<=Length; i++)
		SG->Add ((Visual3d_Light *) (MyLights.Value (i)));

	return (SG);

}

Standard_Integer Visual3d_ContextView::NumberOfActivatedLights () const {

Standard_Integer Length	= MyLights.Length ();

	return (Length);

}

Handle(Visual3d_Light) Visual3d_ContextView::ActivatedLight (const Standard_Integer AnIndex) const {

	return (Visual3d_Light *) (MyLights.Value (AnIndex));

}

void Visual3d_ContextView::SetClipPlaneOn (const Handle(Visual3d_ClipPlane)& AClipPlane) {

Standard_Integer LengthC	= MyClipPlanes.Length ();
Standard_Integer indexC		= 0;

	// Find plane <AClipPlane> in the
	// sequence of already active planes 
	for (Standard_Integer i=1; i<=LengthC && indexC==0; i++)
		if ((void *) (MyClipPlanes.Value (i)) ==
		    (void *) (AClipPlane.operator->())) indexC = i;

	// This is the activation of a new plane
	if (indexC == 0)
		MyClipPlanes.Append ((void *) AClipPlane.operator->());

}

void Visual3d_ContextView::SetClipPlaneOff (const Handle(Visual3d_ClipPlane)& AClipPlane) {

Standard_Integer LengthC	= MyClipPlanes.Length ();
Standard_Integer indexC		= 0;

	// Find plane <AClipPlane> in the
	// sequence of already active planes 
	for (Standard_Integer i=1; i<=LengthC && indexC==0; i++)
		if ((void *) (MyClipPlanes.Value (i)) ==
		    (void *) (AClipPlane.operator->())) indexC = i;

	// This is an active plane
	if (indexC != 0) MyClipPlanes.Remove (indexC);

}

Handle(Visual3d_HSetOfClipPlane) Visual3d_ContextView::ActivatedClipPlanes () const {

Handle(Visual3d_HSetOfClipPlane) SG = new Visual3d_HSetOfClipPlane ();
Standard_Integer Length	= MyClipPlanes.Length ();

	for (Standard_Integer i=1; i<=Length; i++)
		SG->Add ((Visual3d_ClipPlane *) (MyClipPlanes.Value (i)));

	return (SG);

}

Standard_Integer Visual3d_ContextView::NumberOfActivatedClipPlanes () const {

Standard_Integer Length	= MyClipPlanes.Length ();

	return (Length);

}

Handle(Visual3d_ClipPlane) Visual3d_ContextView::ActivatedClipPlane (const Standard_Integer AnIndex) const {

	return (Visual3d_ClipPlane *) (MyClipPlanes.Value (AnIndex));
}


void Visual3d_ContextView::SetSurfaceDetail(const Visual3d_TypeOfSurfaceDetail TOSD)
{
  MySurfaceDetail = TOSD;
}


void Visual3d_ContextView::SetTextureEnv(const Handle(Graphic3d_TextureEnv)& ATexture)
{
  MyTextureEnv = ATexture;
}


Handle_Graphic3d_TextureEnv Visual3d_ContextView::TextureEnv() const
{
  return MyTextureEnv;
}


Visual3d_TypeOfSurfaceDetail Visual3d_ContextView::SurfaceDetail() const
{
  return MySurfaceDetail;
}
