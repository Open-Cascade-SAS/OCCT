//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#include <CASCADESamplesJni_V3d_Viewer.h>
#include <V3d_Viewer.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_GraphicDevice.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <Quantity_NameOfColor.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <V3d_TypeOfShadingModel.hxx>
#include <V3d_TypeOfUpdate.hxx>
#include <Standard_Boolean.hxx>
#include <V3d_TypeOfSurfaceDetail.hxx>
#include <V3d_View.hxx>
#include <V3d_OrthographicView.hxx>
#include <V3d_PerspectiveView.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Quantity_Color.hxx>
#include <V3d_TypeOfView.hxx>
#include <gp_Ax3.hxx>
#include <V3d_Light.hxx>
#include <V3d_Plane.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Aspect_GridType.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Aspect_Grid.hxx>
#include <Standard_Integer.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1Create_10 (JNIEnv *env, jobject theobj, jobject Device, jstring aName, jobject aDomain, jdouble ViewSize, jshort ViewProj, jshort ViewBackground, jshort Visualization, jshort ShadingModel, jshort UpdateMode, jboolean ComputedMode, jboolean DefaultComputedMode, jshort SurfaceDetail)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_GraphicDevice ) the_Device;
 void*                ptr_Device = jcas_GetHandle(env,Device);
 
 if ( ptr_Device != NULL ) the_Device = *(   (  Handle( Aspect_GraphicDevice )*  )ptr_Device   );

Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Standard_CString the_aDomain = jcas_ConvertToCString(env,aDomain);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = new V3d_Viewer(the_Device,the_aName,the_aDomain,(Quantity_Length) ViewSize,(V3d_TypeOfOrientation) ViewProj,(Quantity_NameOfColor) ViewBackground,(V3d_TypeOfVisualization) Visualization,(V3d_TypeOfShadingModel) ShadingModel,(V3d_TypeOfUpdate) UpdateMode,(Standard_Boolean) ComputedMode,(Standard_Boolean) DefaultComputedMode,(V3d_TypeOfSurfaceDetail) SurfaceDetail);
jcas_SetHandle(env,theobj,theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_CreateView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_View)* theret = new Handle(V3d_View);
*theret = the_this->CreateView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_View",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultOrthographicView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_OrthographicView)* theret = new Handle(V3d_OrthographicView);
*theret = the_this->DefaultOrthographicView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_OrthographicView",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultPerspectiveView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_PerspectiveView)* theret = new Handle(V3d_PerspectiveView);
*theret = the_this->DefaultPerspectiveView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_PerspectiveView",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetViewOn_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetViewOn();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetViewOn_12 (JNIEnv *env, jobject theobj, jobject View)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_View;
 void*                ptr_View = jcas_GetHandle(env,View);
 
 if ( ptr_View != NULL ) the_View = *(   (  Handle( V3d_View )*  )ptr_View   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetViewOn(the_View);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetViewOff_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetViewOff();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetViewOff_12 (JNIEnv *env, jobject theobj, jobject View)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_View;
 void*                ptr_View = jcas_GetHandle(env,View);
 
 if ( ptr_View != NULL ) the_View = *(   (  Handle( V3d_View )*  )ptr_View   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetViewOff(the_View);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Update();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_UpdateLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->UpdateLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Redraw (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Redraw();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Remove (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Remove();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Erase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Erase();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_UnHighlight (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->UnHighlight();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetDefaultBackgroundColor_11 (JNIEnv *env, jobject theobj, jshort Type, jdouble V1, jdouble V2, jdouble V3)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultBackgroundColor((Quantity_TypeOfColor) Type,(Quantity_Parameter) V1,(Quantity_Parameter) V2,(Quantity_Parameter) V3);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetDefaultBackgroundColor_12 (JNIEnv *env, jobject theobj, jshort Name)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultBackgroundColor((Quantity_NameOfColor) Name);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetDefaultBackgroundColor_13 (JNIEnv *env, jobject theobj, jobject Color)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_Color = (Quantity_Color*) jcas_GetHandle(env,Color);
if ( the_Color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Color = new Quantity_Color ();
 // jcas_SetHandle ( env, Color, the_Color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultBackgroundColor(*the_Color);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultViewSize (JNIEnv *env, jobject theobj, jdouble Size)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultViewSize((Quantity_Length) Size);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultViewProj (JNIEnv *env, jobject theobj, jshort Orientation)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultViewProj((V3d_TypeOfOrientation) Orientation);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultVisualization (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultVisualization((V3d_TypeOfVisualization) Type);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetZBufferManagment (JNIEnv *env, jobject theobj, jboolean Automatic)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetZBufferManagment((Standard_Boolean) Automatic);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ZBufferManagment (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ZBufferManagment();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultShadingModel (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultShadingModel((V3d_TypeOfShadingModel) Type);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultSurfaceDetail (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultSurfaceDetail((V3d_TypeOfSurfaceDetail) Type);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultAngle (JNIEnv *env, jobject theobj, jdouble Angle)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultAngle((Quantity_PlaneAngle) Angle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetUpdateMode (JNIEnv *env, jobject theobj, jshort Mode)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetUpdateMode((V3d_TypeOfUpdate) Mode);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultTypeOfView (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultTypeOfView((V3d_TypeOfView) Type);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetPrivilegedPlane (JNIEnv *env, jobject theobj, jobject aPlane)
{

jcas_Locking alock(env);
{
try {
gp_Ax3* the_aPlane = (gp_Ax3*) jcas_GetHandle(env,aPlane);
if ( the_aPlane == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aPlane = new gp_Ax3 ();
 // jcas_SetHandle ( env, aPlane, the_aPlane );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetPrivilegedPlane(*the_aPlane);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_PrivilegedPlane (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
gp_Ax3* theret = new gp_Ax3(the_this->PrivilegedPlane());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/gp_Ax3",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DisplayPrivilegedPlane (JNIEnv *env, jobject theobj, jboolean OnOff, jdouble aSize)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->DisplayPrivilegedPlane((Standard_Boolean) OnOff,(Quantity_Length) aSize);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetLightOn_11 (JNIEnv *env, jobject theobj, jobject MyLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_MyLight;
 void*                ptr_MyLight = jcas_GetHandle(env,MyLight);
 
 if ( ptr_MyLight != NULL ) the_MyLight = *(   (  Handle( V3d_Light )*  )ptr_MyLight   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetLightOn(the_MyLight);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetLightOn_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetLightOn();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetLightOff_11 (JNIEnv *env, jobject theobj, jobject MyLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_MyLight;
 void*                ptr_MyLight = jcas_GetHandle(env,MyLight);
 
 if ( ptr_MyLight != NULL ) the_MyLight = *(   (  Handle( V3d_Light )*  )ptr_MyLight   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetLightOff(the_MyLight);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetLightOff_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetLightOff();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DelLight (JNIEnv *env, jobject theobj, jobject MyLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_MyLight;
 void*                ptr_MyLight = jcas_GetHandle(env,MyLight);
 
 if ( ptr_MyLight != NULL ) the_MyLight = *(   (  Handle( V3d_Light )*  )ptr_MyLight   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->DelLight(the_MyLight);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetCurrentSelectedLight (JNIEnv *env, jobject theobj, jobject TheLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_TheLight;
 void*                ptr_TheLight = jcas_GetHandle(env,TheLight);
 
 if ( ptr_TheLight != NULL ) the_TheLight = *(   (  Handle( V3d_Light )*  )ptr_TheLight   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetCurrentSelectedLight(the_TheLight);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ClearCurrentSelectedLight (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->ClearCurrentSelectedLight();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1DefaultBackgroundColor_11 (JNIEnv *env, jobject theobj, jshort Type, jobject V1, jobject V2, jobject V3)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_V1 = jcas_GetReal(env,V1);
Standard_Real the_V2 = jcas_GetReal(env,V2);
Standard_Real the_V3 = jcas_GetReal(env,V3);
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->DefaultBackgroundColor((Quantity_TypeOfColor) Type,the_V1,the_V2,the_V3);
jcas_SetReal(env,V1,the_V1);
jcas_SetReal(env,V2,the_V2);
jcas_SetReal(env,V3,the_V3);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1DefaultBackgroundColor_12 (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Quantity_Color* theret = new Quantity_Color(the_this->DefaultBackgroundColor());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Quantity_Color",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultViewSize (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultViewSize();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultViewProj (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultViewProj();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultVisualization (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultVisualization();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultShadingModel (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultShadingModel();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultSurfaceDetail (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultSurfaceDetail();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultAngle (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultAngle();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_UpdateMode (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->UpdateMode();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_IfMoreViews (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IfMoreViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_InitActiveViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->InitActiveViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_MoreActiveViews (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreActiveViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_NextActiveViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->NextActiveViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ActiveView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_View)* theret = new Handle(V3d_View);
*theret = the_this->ActiveView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_View",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_LastActiveView (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LastActiveView();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_InitDefinedViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->InitDefinedViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_MoreDefinedViews (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreDefinedViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_NextDefinedViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->NextDefinedViews();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefinedView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_View)* theret = new Handle(V3d_View);
*theret = the_this->DefinedView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_View",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_InitActiveLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->InitActiveLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_MoreActiveLights (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreActiveLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_NextActiveLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->NextActiveLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ActiveLight (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_Light)* theret = new Handle(V3d_Light);
*theret = the_this->ActiveLight();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Light",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_InitDefinedLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->InitDefinedLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_MoreDefinedLights (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreDefinedLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_NextDefinedLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->NextDefinedLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefinedLight (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_Light)* theret = new Handle(V3d_Light);
*theret = the_this->DefinedLight();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Light",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_InitDefinedPlanes (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->InitDefinedPlanes();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_MoreDefinedPlanes (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreDefinedPlanes();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_NextDefinedPlanes (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->NextDefinedPlanes();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefinedPlane (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_Plane)* theret = new Handle(V3d_Plane);
*theret = the_this->DefinedPlane();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Plane",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Viewer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Visual3d_ViewManager)* theret = new Handle(Visual3d_ViewManager);
*theret = the_this->Viewer();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Visual3d_ViewManager",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_CurrentSelectedLight (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V3d_Light)* theret = new Handle(V3d_Light);
*theret = the_this->CurrentSelectedLight();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Light",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_IsGlobalLight (JNIEnv *env, jobject theobj, jobject TheLight)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_TheLight;
 void*                ptr_TheLight = jcas_GetHandle(env,TheLight);
 
 if ( ptr_TheLight != NULL ) the_TheLight = *(   (  Handle( V3d_Light )*  )ptr_TheLight   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsGlobalLight(the_TheLight);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ComputedMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ComputedMode();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DefaultComputedMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultComputedMode();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_ActivateGrid (JNIEnv *env, jobject theobj, jshort aGridType, jshort aGridDrawMode)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->ActivateGrid((Aspect_GridType) aGridType,(Aspect_GridDrawMode) aGridDrawMode);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_DeactivateGrid (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->DeactivateGrid();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetGridEcho_11 (JNIEnv *env, jobject theobj, jboolean showGrid)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetGridEcho((Standard_Boolean) showGrid);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1SetGridEcho_12 (JNIEnv *env, jobject theobj, jobject aMarker)
{

jcas_Locking alock(env);
{
try {
 Handle( Graphic3d_AspectMarker3d ) the_aMarker;
 void*                ptr_aMarker = jcas_GetHandle(env,aMarker);
 
 if ( ptr_aMarker != NULL ) the_aMarker = *(   (  Handle( Graphic3d_AspectMarker3d )*  )ptr_aMarker   );

Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetGridEcho(the_aMarker);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_GridEcho (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->GridEcho();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1Viewer_V3d_1Viewer_1IsActive_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsActive();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Grid (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_Grid)* theret = new Handle(Aspect_Grid);
*theret = the_this->Grid();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_Grid",theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_GridType (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->GridType();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1Viewer_GridDrawMode (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->GridDrawMode();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_RectangularGridValues (JNIEnv *env, jobject theobj, jobject XOrigin, jobject YOrigin, jobject XStep, jobject YStep, jobject RotationAngle)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XOrigin = jcas_GetReal(env,XOrigin);
Standard_Real the_YOrigin = jcas_GetReal(env,YOrigin);
Standard_Real the_XStep = jcas_GetReal(env,XStep);
Standard_Real the_YStep = jcas_GetReal(env,YStep);
Standard_Real the_RotationAngle = jcas_GetReal(env,RotationAngle);
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->RectangularGridValues(the_XOrigin,the_YOrigin,the_XStep,the_YStep,the_RotationAngle);
jcas_SetReal(env,XOrigin,the_XOrigin);
jcas_SetReal(env,YOrigin,the_YOrigin);
jcas_SetReal(env,XStep,the_XStep);
jcas_SetReal(env,YStep,the_YStep);
jcas_SetReal(env,RotationAngle,the_RotationAngle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetRectangularGridValues (JNIEnv *env, jobject theobj, jdouble XOrigin, jdouble YOrigin, jdouble XStep, jdouble YStep, jdouble RotationAngle)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetRectangularGridValues((Quantity_Length) XOrigin,(Quantity_Length) YOrigin,(Quantity_Length) XStep,(Quantity_Length) YStep,(Quantity_PlaneAngle) RotationAngle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_CircularGridValues (JNIEnv *env, jobject theobj, jobject XOrigin, jobject YOrigin, jobject RadiusStep, jobject DivisionNumber, jobject RotationAngle)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XOrigin = jcas_GetReal(env,XOrigin);
Standard_Real the_YOrigin = jcas_GetReal(env,YOrigin);
Standard_Real the_RadiusStep = jcas_GetReal(env,RadiusStep);
Standard_Integer the_DivisionNumber = jcas_GetInteger(env,DivisionNumber);
Standard_Real the_RotationAngle = jcas_GetReal(env,RotationAngle);
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->CircularGridValues(the_XOrigin,the_YOrigin,the_RadiusStep,the_DivisionNumber,the_RotationAngle);
jcas_SetReal(env,XOrigin,the_XOrigin);
jcas_SetReal(env,YOrigin,the_YOrigin);
jcas_SetReal(env,RadiusStep,the_RadiusStep);
jcas_SetInteger(env,DivisionNumber,the_DivisionNumber);
jcas_SetReal(env,RotationAngle,the_RotationAngle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetCircularGridValues (JNIEnv *env, jobject theobj, jdouble XOrigin, jdouble YOrigin, jdouble RadiusStep, jint DivisionNumber, jdouble RotationAngle)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetCircularGridValues((Quantity_Length) XOrigin,(Quantity_Length) YOrigin,(Quantity_Length) RadiusStep,(Standard_Integer) DivisionNumber,(Quantity_PlaneAngle) RotationAngle);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_CircularGridGraphicValues (JNIEnv *env, jobject theobj, jobject Radius, jobject OffSet)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Radius = jcas_GetReal(env,Radius);
Standard_Real the_OffSet = jcas_GetReal(env,OffSet);
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->CircularGridGraphicValues(the_Radius,the_OffSet);
jcas_SetReal(env,Radius,the_Radius);
jcas_SetReal(env,OffSet,the_OffSet);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetCircularGridGraphicValues (JNIEnv *env, jobject theobj, jdouble Radius, jdouble OffSet)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetCircularGridGraphicValues((Quantity_Length) Radius,(Quantity_Length) OffSet);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_RectangularGridGraphicValues (JNIEnv *env, jobject theobj, jobject XSize, jobject YSize, jobject OffSet)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XSize = jcas_GetReal(env,XSize);
Standard_Real the_YSize = jcas_GetReal(env,YSize);
Standard_Real the_OffSet = jcas_GetReal(env,OffSet);
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->RectangularGridGraphicValues(the_XSize,the_YSize,the_OffSet);
jcas_SetReal(env,XSize,the_XSize);
jcas_SetReal(env,YSize,the_YSize);
jcas_SetReal(env,OffSet,the_OffSet);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetRectangularGridGraphicValues (JNIEnv *env, jobject theobj, jdouble XSize, jdouble YSize, jdouble OffSet)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetRectangularGridGraphicValues((Quantity_Length) XSize,(Quantity_Length) YSize,(Quantity_Length) OffSet);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_SetDefaultLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultLights();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1Viewer_Init (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_Viewer) the_this = *((Handle(V3d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Init();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}


}
