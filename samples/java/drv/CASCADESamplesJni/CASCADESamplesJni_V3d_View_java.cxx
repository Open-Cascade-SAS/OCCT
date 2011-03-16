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

#include <CASCADESamplesJni_V3d_View.h>
#include <V3d_View.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_Window.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Standard_Real.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_CString.hxx>
#include <Aspect_FillMethod.hxx>
#include <V3d_TypeOfShadingModel.hxx>
#include <V3d_TypeOfSurfaceDetail.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <V3d_TypeOfZclipping.hxx>
#include <V3d_Light.hxx>
#include <V3d_Plane.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Aspect_TypeOfTriedronEcho.hxx>
#include <Aspect_ColorScale.hxx>
#include <V3d_TypeOfAxe.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_TypeOfView.hxx>
#include <Visual3d_View.hxx>
#include <Graphic3d_Plotter.hxx>
#include <gp_Ax3.hxx>
#include <Aspect_Grid.hxx>
#include <PlotMgt_PlotterDriver.hxx>
#include <Aspect_FormatOfSheetPaper.hxx>
#include <Aspect_PixMap.hxx>
#include <V3d_TypeOfProjectionModel.hxx>
#include <V3d_TypeOfBackfacingModel.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetWindow_11 (JNIEnv *env, jobject theobj, jobject IdWin)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_Window ) the_IdWin;
 void*                ptr_IdWin = jcas_GetHandle(env,IdWin);
 
 if ( ptr_IdWin != NULL ) the_IdWin = *(   (  Handle( Aspect_Window )*  )ptr_IdWin   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetWindow(the_IdWin);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetMagnify (JNIEnv *env, jobject theobj, jobject IdWin, jobject aPreviousView, jint x1, jint y1, jint x2, jint y2)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_Window ) the_IdWin;
 void*                ptr_IdWin = jcas_GetHandle(env,IdWin);
 
 if ( ptr_IdWin != NULL ) the_IdWin = *(   (  Handle( Aspect_Window )*  )ptr_IdWin   );

 Handle( V3d_View ) the_aPreviousView;
 void*                ptr_aPreviousView = jcas_GetHandle(env,aPreviousView);
 
 if ( ptr_aPreviousView != NULL ) the_aPreviousView = *(   (  Handle( V3d_View )*  )ptr_aPreviousView   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetMagnify(the_IdWin,the_aPreviousView,(Standard_Integer) x1,(Standard_Integer) y1,(Standard_Integer) x2,(Standard_Integer) y2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Remove (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Redraw_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Redraw_12 (JNIEnv *env, jobject theobj, jint x, jint y, jint width, jint height)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Redraw((Standard_Integer) x,(Standard_Integer) y,(Standard_Integer) width,(Standard_Integer) height);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_MustBeResized (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->MustBeResized();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_DoMapping (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->DoMapping();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IsEmpty (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsEmpty();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_UpdateLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetBackgroundColor_11 (JNIEnv *env, jobject theobj, jshort Type, jdouble V1, jdouble V2, jdouble V3)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackgroundColor((Quantity_TypeOfColor) Type,(Quantity_Parameter) V1,(Quantity_Parameter) V2,(Quantity_Parameter) V3);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetBackgroundColor_12 (JNIEnv *env, jobject theobj, jobject Color)
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
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackgroundColor(*the_Color);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetBackgroundColor_13 (JNIEnv *env, jobject theobj, jshort Name)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackgroundColor((Quantity_NameOfColor) Name);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetBackgroundImage (JNIEnv *env, jobject theobj, jobject FileName, jshort FillStyle, jboolean update)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_FileName = jcas_ConvertToCString(env,FileName);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackgroundImage(the_FileName,(Aspect_FillMethod) FillStyle,(Standard_Boolean) update);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetBgImageStyle (JNIEnv *env, jobject theobj, jshort FillStyle, jboolean update)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBgImageStyle((Aspect_FillMethod) FillStyle,(Standard_Boolean) update);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAxis (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z, jdouble Vx, jdouble Vy, jdouble Vz)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAxis((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,(Quantity_Parameter) Vx,(Quantity_Parameter) Vy,(Quantity_Parameter) Vz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetShadingModel (JNIEnv *env, jobject theobj, jshort Model)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetShadingModel((V3d_TypeOfShadingModel) Model);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetSurfaceDetail (JNIEnv *env, jobject theobj, jshort SurfaceDetail)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetSurfaceDetail((V3d_TypeOfSurfaceDetail) SurfaceDetail);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetTextureEnv (JNIEnv *env, jobject theobj, jobject ATexture)
{

jcas_Locking alock(env);
{
try {
 Handle( Graphic3d_TextureEnv ) the_ATexture;
 void*                ptr_ATexture = jcas_GetHandle(env,ATexture);
 
 if ( ptr_ATexture != NULL ) the_ATexture = *(   (  Handle( Graphic3d_TextureEnv )*  )ptr_ATexture   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetTextureEnv(the_ATexture);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetVisualization (JNIEnv *env, jobject theobj, jshort Mode)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetVisualization((V3d_TypeOfVisualization) Mode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAntialiasingOn (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAntialiasingOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAntialiasingOff (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAntialiasingOff();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZClippingDepth (JNIEnv *env, jobject theobj, jdouble Depth)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZClippingDepth((Quantity_Length) Depth);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZClippingWidth (JNIEnv *env, jobject theobj, jdouble Width)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZClippingWidth((Quantity_Length) Width);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZClippingType (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZClippingType((V3d_TypeOfZclipping) Type);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZCueingDepth (JNIEnv *env, jobject theobj, jdouble Depth)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZCueingDepth((Quantity_Length) Depth);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZCueingWidth (JNIEnv *env, jobject theobj, jdouble Width)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZCueingWidth((Quantity_Length) Width);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZCueingOn (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZCueingOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZCueingOff (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZCueingOff();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetLightOn_11 (JNIEnv *env, jobject theobj, jobject MyLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_MyLight;
 void*                ptr_MyLight = jcas_GetHandle(env,MyLight);
 
 if ( ptr_MyLight != NULL ) the_MyLight = *(   (  Handle( V3d_Light )*  )ptr_MyLight   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetLightOn_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetLightOff_11 (JNIEnv *env, jobject theobj, jobject MyLight)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_MyLight;
 void*                ptr_MyLight = jcas_GetHandle(env,MyLight);
 
 if ( ptr_MyLight != NULL ) the_MyLight = *(   (  Handle( V3d_Light )*  )ptr_MyLight   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetLightOff_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IsActiveLight (JNIEnv *env, jobject theobj, jobject aLight)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_Light ) the_aLight;
 void*                ptr_aLight = jcas_GetHandle(env,aLight);
 
 if ( ptr_aLight != NULL ) the_aLight = *(   (  Handle( V3d_Light )*  )ptr_aLight   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsActiveLight(the_aLight);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetTransparency (JNIEnv *env, jobject theobj, jboolean AnActivity)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetTransparency((Standard_Boolean) AnActivity);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetPlaneOn_11 (JNIEnv *env, jobject theobj, jobject MyPlane)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Plane ) the_MyPlane;
 void*                ptr_MyPlane = jcas_GetHandle(env,MyPlane);
 
 if ( ptr_MyPlane != NULL ) the_MyPlane = *(   (  Handle( V3d_Plane )*  )ptr_MyPlane   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneOn(the_MyPlane);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetPlaneOn_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetPlaneOff_11 (JNIEnv *env, jobject theobj, jobject MyPlane)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Plane ) the_MyPlane;
 void*                ptr_MyPlane = jcas_GetHandle(env,MyPlane);
 
 if ( ptr_MyPlane != NULL ) the_MyPlane = *(   (  Handle( V3d_Plane )*  )ptr_MyPlane   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneOff(the_MyPlane);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetPlaneOff_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneOff();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IsActivePlane (JNIEnv *env, jobject theobj, jobject aPlane)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_Plane ) the_aPlane;
 void*                ptr_aPlane = jcas_GetHandle(env,aPlane);
 
 if ( ptr_aPlane != NULL ) the_aPlane = *(   (  Handle( V3d_Plane )*  )ptr_aPlane   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsActivePlane(the_aPlane);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ZBufferTriedronSetup (JNIEnv *env, jobject theobj, jshort XColor, jshort YColor, jshort ZColor, jdouble SizeRatio, jdouble AxisDiametr, jint NbFacettes)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ZBufferTriedronSetup((Quantity_NameOfColor) XColor,(Quantity_NameOfColor) YColor,(Quantity_NameOfColor) ZColor,(Standard_Real) SizeRatio,(Standard_Real) AxisDiametr,(Standard_Integer) NbFacettes);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_TriedronDisplay (JNIEnv *env, jobject theobj, jshort APosition, jshort AColor, jdouble AScale, jshort AMode)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->TriedronDisplay((Aspect_TypeOfTriedronPosition) APosition,(Quantity_NameOfColor) AColor,(Standard_Real) AScale,(V3d_TypeOfVisualization) AMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_TriedronErase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->TriedronErase();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_TriedronEcho (JNIEnv *env, jobject theobj, jshort AType)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->TriedronEcho((Aspect_TypeOfTriedronEcho) AType);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ColorScaleDisplay (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ColorScaleDisplay();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ColorScaleErase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ColorScaleErase();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_ColorScaleIsDisplayed (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ColorScaleIsDisplayed();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_ColorScale (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(Aspect_ColorScale)* theret = new Handle(Aspect_ColorScale);
*theret = the_this->ColorScale();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorScale",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetFront (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetFront();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Rotate_11 (JNIEnv *env, jobject theobj, jdouble Ax, jdouble Ay, jdouble Az, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotate((Quantity_PlaneAngle) Ax,(Quantity_PlaneAngle) Ay,(Quantity_PlaneAngle) Az,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Rotate_12 (JNIEnv *env, jobject theobj, jdouble Ax, jdouble Ay, jdouble Az, jdouble X, jdouble Y, jdouble Z, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotate((Quantity_PlaneAngle) Ax,(Quantity_PlaneAngle) Ay,(Quantity_PlaneAngle) Az,(V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Rotate_13 (JNIEnv *env, jobject theobj, jshort Axe, jdouble Angle, jdouble X, jdouble Y, jdouble Z, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotate((V3d_TypeOfAxe) Axe,(Quantity_PlaneAngle) Angle,(V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Rotate_14 (JNIEnv *env, jobject theobj, jshort Axe, jdouble Angle, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotate((V3d_TypeOfAxe) Axe,(Quantity_PlaneAngle) Angle,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Rotate_15 (JNIEnv *env, jobject theobj, jdouble Angle, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotate((Quantity_PlaneAngle) Angle,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Move_11 (JNIEnv *env, jobject theobj, jdouble Dx, jdouble Dy, jdouble Dz, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Move((Quantity_Length) Dx,(Quantity_Length) Dy,(Quantity_Length) Dz,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Move_12 (JNIEnv *env, jobject theobj, jshort Axe, jdouble Length, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Move((V3d_TypeOfAxe) Axe,(Quantity_Length) Length,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Move_13 (JNIEnv *env, jobject theobj, jdouble Length, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Move((Quantity_Length) Length,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Translate_11 (JNIEnv *env, jobject theobj, jdouble Dx, jdouble Dy, jdouble Dz, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Translate((Quantity_Length) Dx,(Quantity_Length) Dy,(Quantity_Length) Dz,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Translate_12 (JNIEnv *env, jobject theobj, jshort Axe, jdouble Length, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Translate((V3d_TypeOfAxe) Axe,(Quantity_Length) Length,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Translate_13 (JNIEnv *env, jobject theobj, jdouble Length, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Translate((Quantity_Length) Length,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Place (JNIEnv *env, jobject theobj, jint x, jint y, jdouble aZoomFactor)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Place((Standard_Integer) x,(Standard_Integer) y,(Quantity_Factor) aZoomFactor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Turn_11 (JNIEnv *env, jobject theobj, jdouble Ax, jdouble Ay, jdouble Az, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Turn((Quantity_PlaneAngle) Ax,(Quantity_PlaneAngle) Ay,(Quantity_PlaneAngle) Az,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Turn_12 (JNIEnv *env, jobject theobj, jshort Axe, jdouble Angle, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Turn((V3d_TypeOfAxe) Axe,(Quantity_PlaneAngle) Angle,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Turn_13 (JNIEnv *env, jobject theobj, jdouble Angle, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Turn((Quantity_PlaneAngle) Angle,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetTwist (JNIEnv *env, jobject theobj, jdouble Angle)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetTwist((Quantity_PlaneAngle) Angle);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetEye (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetEye((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetDepth (JNIEnv *env, jobject theobj, jdouble Depth)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDepth((Quantity_Length) Depth);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetProj_11 (JNIEnv *env, jobject theobj, jdouble Vx, jdouble Vy, jdouble Vz)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetProj((Quantity_Parameter) Vx,(Quantity_Parameter) Vy,(Quantity_Parameter) Vz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetProj_12 (JNIEnv *env, jobject theobj, jshort Orientation)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetProj((V3d_TypeOfOrientation) Orientation);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAt (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAt((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetUp_11 (JNIEnv *env, jobject theobj, jdouble Vx, jdouble Vy, jdouble Vz)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetUp((Quantity_Parameter) Vx,(Quantity_Parameter) Vy,(Quantity_Parameter) Vz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetUp_12 (JNIEnv *env, jobject theobj, jshort Orientation)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetUp((V3d_TypeOfOrientation) Orientation);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetViewOrientationDefault (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetViewOrientationDefault();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ResetViewOrientation (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ResetViewOrientation();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Panning (JNIEnv *env, jobject theobj, jdouble Dx, jdouble Dy, jdouble aZoomFactor, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Panning((Quantity_Length) Dx,(Quantity_Length) Dy,(Quantity_Factor) aZoomFactor,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetCenter_11 (JNIEnv *env, jobject theobj, jdouble Xc, jdouble Yc)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetCenter((V3d_Coordinate) Xc,(V3d_Coordinate) Yc);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1SetCenter_12 (JNIEnv *env, jobject theobj, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetCenter((Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetSize (JNIEnv *env, jobject theobj, jdouble Size)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetSize((Quantity_Length) Size);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZSize (JNIEnv *env, jobject theobj, jdouble Size)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZSize((Quantity_Length) Size);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetZoom (JNIEnv *env, jobject theobj, jdouble Coef, jboolean Start)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetZoom((Quantity_Factor) Coef,(Standard_Boolean) Start);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetScale (JNIEnv *env, jobject theobj, jdouble Coef)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetScale((Quantity_Factor) Coef);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAxialScale (JNIEnv *env, jobject theobj, jdouble Sx, jdouble Sy, jdouble Sz)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAxialScale((Standard_Real) Sx,(Standard_Real) Sy,(Standard_Real) Sz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1FitAll_11 (JNIEnv *env, jobject theobj, jdouble Coef, jboolean FitZ, jboolean update)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->FitAll((Quantity_Coefficient) Coef,(Standard_Boolean) FitZ,(Standard_Boolean) update);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ZFitAll (JNIEnv *env, jobject theobj, jdouble Coef)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ZFitAll((Quantity_Coefficient) Coef);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1FitAll_12 (JNIEnv *env, jobject theobj, jdouble Umin, jdouble Vmin, jdouble Umax, jdouble Vmax)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->FitAll((V3d_Coordinate) Umin,(V3d_Coordinate) Vmin,(V3d_Coordinate) Umax,(V3d_Coordinate) Vmax);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_WindowFit (JNIEnv *env, jobject theobj, jint Xmin, jint Ymin, jint Xmax, jint Ymax)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->WindowFit((Standard_Integer) Xmin,(Standard_Integer) Ymin,(Standard_Integer) Xmax,(Standard_Integer) Ymax);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetViewMappingDefault (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetViewMappingDefault();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ResetViewMapping (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ResetViewMapping();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Reset (JNIEnv *env, jobject theobj, jboolean update)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Reset((Standard_Boolean) update);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_11 (JNIEnv *env, jobject theobj, jint Vp)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Standard_Integer) Vp);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_12 (JNIEnv *env, jobject theobj, jint Xp, jint Yp, jobject Xv, jobject Yv)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Xv = jcas_GetReal(env,Xv);
Standard_Real the_Yv = jcas_GetReal(env,Yv);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((Standard_Integer) Xp,(Standard_Integer) Yp,the_Xv,the_Yv);
jcas_SetReal(env,Xv,the_Xv);
jcas_SetReal(env,Yv,the_Yv);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_13 (JNIEnv *env, jobject theobj, jdouble Vv)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Quantity_Length) Vv);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_14 (JNIEnv *env, jobject theobj, jdouble Xv, jdouble Yv, jobject Xp, jobject Yp)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_Xp = jcas_GetInteger(env,Xp);
Standard_Integer the_Yp = jcas_GetInteger(env,Yp);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((V3d_Coordinate) Xv,(V3d_Coordinate) Yv,the_Xp,the_Yp);
jcas_SetInteger(env,Xp,the_Xp);
jcas_SetInteger(env,Yp,the_Yp);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_15 (JNIEnv *env, jobject theobj, jint Xp, jint Yp, jobject X, jobject Y, jobject Z)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_X = jcas_GetReal(env,X);
Standard_Real the_Y = jcas_GetReal(env,Y);
Standard_Real the_Z = jcas_GetReal(env,Z);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((Standard_Integer) Xp,(Standard_Integer) Yp,the_X,the_Y,the_Z);
jcas_SetReal(env,X,the_X);
jcas_SetReal(env,Y,the_Y);
jcas_SetReal(env,Z,the_Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1ConvertToGrid_11 (JNIEnv *env, jobject theobj, jint Xp, jint Yp, jobject Xg, jobject Yg, jobject Zg)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Xg = jcas_GetReal(env,Xg);
Standard_Real the_Yg = jcas_GetReal(env,Yg);
Standard_Real the_Zg = jcas_GetReal(env,Zg);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ConvertToGrid((Standard_Integer) Xp,(Standard_Integer) Yp,the_Xg,the_Yg,the_Zg);
jcas_SetReal(env,Xg,the_Xg);
jcas_SetReal(env,Yg,the_Yg);
jcas_SetReal(env,Zg,the_Zg);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1ConvertToGrid_12 (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z, jobject Xg, jobject Yg, jobject Zg)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Xg = jcas_GetReal(env,Xg);
Standard_Real the_Yg = jcas_GetReal(env,Yg);
Standard_Real the_Zg = jcas_GetReal(env,Zg);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ConvertToGrid((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,the_Xg,the_Yg,the_Zg);
jcas_SetReal(env,Xg,the_Xg);
jcas_SetReal(env,Yg,the_Yg);
jcas_SetReal(env,Zg,the_Zg);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Convert_16 (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z, jobject Xp, jobject Yp)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_Xp = jcas_GetInteger(env,Xp);
Standard_Integer the_Yp = jcas_GetInteger(env,Yp);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,the_Xp,the_Yp);
jcas_SetInteger(env,Xp,the_Xp);
jcas_SetInteger(env,Yp,the_Yp);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Project (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jdouble Z, jobject Xp, jobject Yp)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Xp = jcas_GetReal(env,Xp);
Standard_Real the_Yp = jcas_GetReal(env,Yp);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Project((V3d_Coordinate) X,(V3d_Coordinate) Y,(V3d_Coordinate) Z,the_Xp,the_Yp);
jcas_SetReal(env,Xp,the_Xp);
jcas_SetReal(env,Yp,the_Yp);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1BackgroundColor_11 (JNIEnv *env, jobject theobj, jshort Type, jobject V1, jobject V2, jobject V3)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_V1 = jcas_GetReal(env,V1);
Standard_Real the_V2 = jcas_GetReal(env,V2);
Standard_Real the_V3 = jcas_GetReal(env,V3);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->BackgroundColor((Quantity_TypeOfColor) Type,the_V1,the_V2,the_V3);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1BackgroundColor_12 (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Quantity_Color* theret = new Quantity_Color(the_this->BackgroundColor());
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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_Scale (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Scale();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1AxialScale_11 (JNIEnv *env, jobject theobj, jobject Sx, jobject Sy, jobject Sz)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Sx = jcas_GetReal(env,Sx);
Standard_Real the_Sy = jcas_GetReal(env,Sy);
Standard_Real the_Sz = jcas_GetReal(env,Sz);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->AxialScale(the_Sx,the_Sy,the_Sz);
jcas_SetReal(env,Sx,the_Sx);
jcas_SetReal(env,Sy,the_Sy);
jcas_SetReal(env,Sz,the_Sz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Center (JNIEnv *env, jobject theobj, jobject Xc, jobject Yc)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Xc = jcas_GetReal(env,Xc);
Standard_Real the_Yc = jcas_GetReal(env,Yc);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Center(the_Xc,the_Yc);
jcas_SetReal(env,Xc,the_Xc);
jcas_SetReal(env,Yc,the_Yc);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Size (JNIEnv *env, jobject theobj, jobject Width, jobject Height)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Width = jcas_GetReal(env,Width);
Standard_Real the_Height = jcas_GetReal(env,Height);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Size(the_Width,the_Height);
jcas_SetReal(env,Width,the_Width);
jcas_SetReal(env,Height,the_Height);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_ZSize (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ZSize();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Eye (JNIEnv *env, jobject theobj, jobject X, jobject Y, jobject Z)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_X = jcas_GetReal(env,X);
Standard_Real the_Y = jcas_GetReal(env,Y);
Standard_Real the_Z = jcas_GetReal(env,Z);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Eye(the_X,the_Y,the_Z);
jcas_SetReal(env,X,the_X);
jcas_SetReal(env,Y,the_Y);
jcas_SetReal(env,Z,the_Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_FocalReferencePoint (JNIEnv *env, jobject theobj, jobject X, jobject Y, jobject Z)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_X = jcas_GetReal(env,X);
Standard_Real the_Y = jcas_GetReal(env,Y);
Standard_Real the_Z = jcas_GetReal(env,Z);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->FocalReferencePoint(the_X,the_Y,the_Z);
jcas_SetReal(env,X,the_X);
jcas_SetReal(env,Y,the_Y);
jcas_SetReal(env,Z,the_Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ProjReferenceAxe (JNIEnv *env, jobject theobj, jint Xpix, jint Ypix, jobject XP, jobject YP, jobject ZP, jobject VX, jobject VY, jobject VZ)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XP = jcas_GetReal(env,XP);
Standard_Real the_YP = jcas_GetReal(env,YP);
Standard_Real the_ZP = jcas_GetReal(env,ZP);
Standard_Real the_VX = jcas_GetReal(env,VX);
Standard_Real the_VY = jcas_GetReal(env,VY);
Standard_Real the_VZ = jcas_GetReal(env,VZ);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ProjReferenceAxe((Standard_Integer) Xpix,(Standard_Integer) Ypix,the_XP,the_YP,the_ZP,the_VX,the_VY,the_VZ);
jcas_SetReal(env,XP,the_XP);
jcas_SetReal(env,YP,the_YP);
jcas_SetReal(env,ZP,the_ZP);
jcas_SetReal(env,VX,the_VX);
jcas_SetReal(env,VY,the_VY);
jcas_SetReal(env,VZ,the_VZ);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_Depth (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Depth();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Proj (JNIEnv *env, jobject theobj, jobject Vx, jobject Vy, jobject Vz)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Vx = jcas_GetReal(env,Vx);
Standard_Real the_Vy = jcas_GetReal(env,Vy);
Standard_Real the_Vz = jcas_GetReal(env,Vz);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Proj(the_Vx,the_Vy,the_Vz);
jcas_SetReal(env,Vx,the_Vx);
jcas_SetReal(env,Vy,the_Vy);
jcas_SetReal(env,Vz,the_Vz);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_At (JNIEnv *env, jobject theobj, jobject X, jobject Y, jobject Z)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_X = jcas_GetReal(env,X);
Standard_Real the_Y = jcas_GetReal(env,Y);
Standard_Real the_Z = jcas_GetReal(env,Z);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->At(the_X,the_Y,the_Z);
jcas_SetReal(env,X,the_X);
jcas_SetReal(env,Y,the_Y);
jcas_SetReal(env,Z,the_Z);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Up (JNIEnv *env, jobject theobj, jobject Vx, jobject Vy, jobject Vz)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Vx = jcas_GetReal(env,Vx);
Standard_Real the_Vy = jcas_GetReal(env,Vy);
Standard_Real the_Vz = jcas_GetReal(env,Vz);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Up(the_Vx,the_Vy,the_Vz);
jcas_SetReal(env,Vx,the_Vx);
jcas_SetReal(env,Vy,the_Vy);
jcas_SetReal(env,Vz,the_Vz);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_Twist (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Twist();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_ShadingModel (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ShadingModel();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_SurfaceDetail (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SurfaceDetail();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_TextureEnv (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(Graphic3d_TextureEnv)* theret = new Handle(Graphic3d_TextureEnv);
*theret = the_this->TextureEnv();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Graphic3d_TextureEnv",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_Transparency (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Transparency();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_Visualization (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Visualization();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_Antialiasing (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Antialiasing();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_ZCueing (JNIEnv *env, jobject theobj, jobject Depth, jobject Width)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_Depth = jcas_GetReal(env,Depth);
Standard_Real the_Width = jcas_GetReal(env,Width);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ZCueing(the_Depth,the_Width);
jcas_SetReal(env,Depth,the_Depth);
jcas_SetReal(env,Width,the_Width);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_ZClipping (JNIEnv *env, jobject theobj, jobject Depth, jobject Width)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_Depth = jcas_GetReal(env,Depth);
Standard_Real the_Width = jcas_GetReal(env,Width);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ZClipping(the_Depth,the_Width);
jcas_SetReal(env,Depth,the_Depth);
jcas_SetReal(env,Width,the_Width);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IfMoreLights (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IfMoreLights();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_InitActiveLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_MoreActiveLights (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_NextActiveLights (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_ActiveLight (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IfMorePlanes (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IfMorePlanes();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_InitActivePlanes (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->InitActivePlanes();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_MoreActivePlanes (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreActivePlanes();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_NextActivePlanes (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->NextActivePlanes();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_ActivePlane (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(V3d_Plane)* theret = new Handle(V3d_Plane);
*theret = the_this->ActivePlane();
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_Viewer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = the_this->Viewer();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Viewer",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IfWindow (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IfWindow();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_Window (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(Aspect_Window)* theret = new Handle(Aspect_Window);
*theret = the_this->Window();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_Window",theret);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_Type (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Type();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Pan (JNIEnv *env, jobject theobj, jint Dx, jint Dy, jdouble aZoomFactor)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Pan((Standard_Integer) Dx,(Standard_Integer) Dy,(Quantity_Factor) aZoomFactor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Zoom_11 (JNIEnv *env, jobject theobj, jint X1, jint Y1, jint X2, jint Y2)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Zoom((Standard_Integer) X1,(Standard_Integer) Y1,(Standard_Integer) X2,(Standard_Integer) Y2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Zoom_12 (JNIEnv *env, jobject theobj, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Zoom((Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1AxialScale_12 (JNIEnv *env, jobject theobj, jint Dx, jint Dy, jshort Axis)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->AxialScale((Standard_Integer) Dx,(Standard_Integer) Dy,(V3d_TypeOfAxe) Axis);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_StartRotation (JNIEnv *env, jobject theobj, jint X, jint Y, jdouble zRotationThreshold)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->StartRotation((Standard_Integer) X,(Standard_Integer) Y,(Quantity_Ratio) zRotationThreshold);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Rotation (JNIEnv *env, jobject theobj, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Rotation((Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetFocale (JNIEnv *env, jobject theobj, jdouble Focale)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetFocale((Quantity_Length) Focale);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_Focale (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Focale();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_View (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(Visual3d_View)* theret = new Handle(Visual3d_View);
*theret = the_this->View();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Visual3d_View",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_TransientManagerBeginDraw (JNIEnv *env, jobject theobj, jboolean DoubleBuffer, jboolean RetainMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->TransientManagerBeginDraw((Standard_Boolean) DoubleBuffer,(Standard_Boolean) RetainMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_TransientManagerClearDraw (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->TransientManagerClearDraw();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_TransientManagerBeginAddDraw (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->TransientManagerBeginAddDraw();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAnimationModeOn (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAnimationModeOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAnimationModeOff (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAnimationModeOff();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_AnimationModeIsOn (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AnimationModeIsOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetAnimationMode (JNIEnv *env, jobject theobj, jboolean anAnimationFlag, jboolean aDegenerationFlag)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetAnimationMode((Standard_Boolean) anAnimationFlag,(Standard_Boolean) aDegenerationFlag);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_AnimationMode (JNIEnv *env, jobject theobj, jobject isDegenerate)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Boolean the_isDegenerate = jcas_GetBoolean(env,isDegenerate);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AnimationMode(the_isDegenerate);
jcas_SetBoolean(env,isDegenerate,the_isDegenerate);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetDegenerateModeOn (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDegenerateModeOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetDegenerateModeOff (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDegenerateModeOff();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_DegenerateModeIsOn (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DegenerateModeIsOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetComputedMode (JNIEnv *env, jobject theobj, jboolean aMode)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetComputedMode((Standard_Boolean) aMode);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_ComputedMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_WindowFitAll (JNIEnv *env, jobject theobj, jint Xmin, jint Ymin, jint Xmax, jint Ymax)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->WindowFitAll((Standard_Integer) Xmin,(Standard_Integer) Ymin,(Standard_Integer) Xmax,(Standard_Integer) Ymax);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetPlotter (JNIEnv *env, jobject theobj, jobject aPlotter)
{

jcas_Locking alock(env);
{
try {
 Handle( Graphic3d_Plotter ) the_aPlotter;
 void*                ptr_aPlotter = jcas_GetHandle(env,aPlotter);
 
 if ( ptr_aPlotter != NULL ) the_aPlotter = *(   (  Handle( Graphic3d_Plotter )*  )ptr_aPlotter   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetPlotter(the_aPlotter);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_Plot (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->Plot();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetGrid (JNIEnv *env, jobject theobj, jobject aPlane, jobject aGrid)
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
 Handle( Aspect_Grid ) the_aGrid;
 void*                ptr_aGrid = jcas_GetHandle(env,aGrid);
 
 if ( ptr_aGrid != NULL ) the_aGrid = *(   (  Handle( Aspect_Grid )*  )ptr_aGrid   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetGrid(*the_aPlane,the_aGrid);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetGridGraphicValues (JNIEnv *env, jobject theobj, jobject aGrid)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_Grid ) the_aGrid;
 void*                ptr_aGrid = jcas_GetHandle(env,aGrid);
 
 if ( ptr_aGrid != NULL ) the_aGrid = *(   (  Handle( Aspect_Grid )*  )ptr_aGrid   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetGridGraphicValues(the_aGrid);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetGridActivity (JNIEnv *env, jobject theobj, jboolean aFlag)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetGridActivity((Standard_Boolean) aFlag);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1View_Tumble (JNIEnv *env, jobject theobj, jint NbImages, jboolean AnimationMode)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Tumble((Standard_Integer) NbImages,(Standard_Boolean) AnimationMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_ScreenCopy (JNIEnv *env, jobject theobj, jobject aPlotterDriver, jboolean fWhiteBackground, jdouble aScale)
{

jcas_Locking alock(env);
{
try {
 Handle( PlotMgt_PlotterDriver ) the_aPlotterDriver;
 void*                ptr_aPlotterDriver = jcas_GetHandle(env,aPlotterDriver);
 
 if ( ptr_aPlotterDriver != NULL ) the_aPlotterDriver = *(   (  Handle( PlotMgt_PlotterDriver )*  )ptr_aPlotterDriver   );

Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->ScreenCopy(the_aPlotterDriver,(Standard_Boolean) fWhiteBackground,(Quantity_Factor) aScale);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Dump_11 (JNIEnv *env, jobject theobj, jobject aFile)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFile = jcas_ConvertToCString(env,aFile);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Dump(the_aFile);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_V3d_1View_1Dump_12 (JNIEnv *env, jobject theobj, jobject aFile, jshort aFormat)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFile = jcas_ConvertToCString(env,aFile);
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Dump(the_aFile,(Aspect_FormatOfSheetPaper) aFormat);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1View_ToPixMap (JNIEnv *env, jobject theobj, jint aWidth, jint aHeight, jint aCDepth)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
Handle(Aspect_PixMap)* theret = new Handle(Aspect_PixMap);
*theret = the_this->ToPixMap((Standard_Integer) aWidth,(Standard_Integer) aHeight,(Standard_Integer) aCDepth);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_PixMap",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetProjModel (JNIEnv *env, jobject theobj, jshort amOdel)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetProjModel((V3d_TypeOfProjectionModel) amOdel);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_ProjModel (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ProjModel();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_SetBackFacingModel (JNIEnv *env, jobject theobj, jshort aModel)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackFacingModel((V3d_TypeOfBackfacingModel) aModel);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V3d_1View_BackFacingModel (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BackFacingModel();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_EnableDepthTest (JNIEnv *env, jobject theobj, jboolean enable)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->EnableDepthTest((Standard_Boolean) enable);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IsDepthTestEnabled (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsDepthTestEnabled();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1View_EnableGLLight (JNIEnv *env, jobject theobj, jboolean enable)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
the_this->EnableGLLight((Standard_Boolean) enable);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V3d_1View_IsGLLightEnabled (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_View) the_this = *((Handle(V3d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsGLLightEnabled();

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


}
