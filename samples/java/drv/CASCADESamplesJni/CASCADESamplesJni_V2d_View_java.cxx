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

#include <CASCADESamplesJni_V2d_View.h>
#include <V2d_View.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_WindowDriver.hxx>
#include <V2d_Viewer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Graphic2d_DisplayList.hxx>
#include <Graphic2d_PickMode.hxx>
#include <V2d_TypeOfWindowResizingEffect.hxx>
#include <PlotMgt_PlotterDriver.hxx>
#include <Aspect_TypeOfColorSpace.hxx>
#include <Graphic2d_View.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_FillMethod.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Create_10 (JNIEnv *env, jobject theobj, jobject aWindowDriver, jobject aViewer, jdouble aXCenter, jdouble aYCenter, jdouble aSize)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_WindowDriver ) the_aWindowDriver;
 void*                ptr_aWindowDriver = jcas_GetHandle(env,aWindowDriver);
 
 if ( ptr_aWindowDriver != NULL ) the_aWindowDriver = *(   (  Handle( Aspect_WindowDriver )*  )ptr_aWindowDriver   );

 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

Handle(V2d_View)* theret = new Handle(V2d_View);
*theret = new V2d_View(the_aWindowDriver,the_aViewer,(Quantity_Length) aXCenter,(Quantity_Length) aYCenter,(Quantity_Length) aSize);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDefaultPosition (JNIEnv *env, jobject theobj, jdouble aXCenter, jdouble aYCenter, jdouble aSize)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultPosition((Quantity_Length) aXCenter,(Quantity_Length) aYCenter,(Quantity_Length) aSize);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Fitall (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Fitall();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_WindowFit (JNIEnv *env, jobject theobj, jint aX1, jint aY1, jint aX2, jint aY2)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->WindowFit((Standard_Integer) aX1,(Standard_Integer) aY1,(Standard_Integer) aX2,(Standard_Integer) aY2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Fit_11 (JNIEnv *env, jobject theobj, jdouble aX1, jdouble aY1, jdouble aX2, jdouble aY2, jboolean UseMinimum)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Fit((Quantity_Length) aX1,(Quantity_Length) aY1,(Quantity_Length) aX2,(Quantity_Length) aY2,(Standard_Boolean) UseMinimum);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetFitallRatio (JNIEnv *env, jobject theobj, jdouble aRatio)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetFitallRatio((Quantity_Ratio) aRatio);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_11 (JNIEnv *env, jobject theobj, jdouble Zoom)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Zoom((Quantity_Factor) Zoom);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_12 (JNIEnv *env, jobject theobj, jint aX1, jint aY1, jint aX2, jint aY2, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Zoom((Standard_Integer) aX1,(Standard_Integer) aY1,(Standard_Integer) aX2,(Standard_Integer) aY2,(Quantity_Ratio) aCoefficient);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_13 (JNIEnv *env, jobject theobj, jint aX, jint aY, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Zoom((Standard_Integer) aX,(Standard_Integer) aY,(Quantity_Ratio) aCoefficient);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Magnify (JNIEnv *env, jobject theobj, jobject anOriginView, jint X1, jint Y1, jint X2, jint Y2)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_View ) the_anOriginView;
 void*                ptr_anOriginView = jcas_GetHandle(env,anOriginView);
 
 if ( ptr_anOriginView != NULL ) the_anOriginView = *(   (  Handle( V2d_View )*  )ptr_anOriginView   );

Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Magnify(the_anOriginView,(Standard_Integer) X1,(Standard_Integer) Y1,(Standard_Integer) X2,(Standard_Integer) Y2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Translate (JNIEnv *env, jobject theobj, jdouble dx, jdouble dy)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Translate((Quantity_Length) dx,(Quantity_Length) dy);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Place (JNIEnv *env, jobject theobj, jint x, jint y, jdouble aZoomFactor)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenPlace (JNIEnv *env, jobject theobj, jdouble x, jdouble y, jdouble aZoomFactor)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->ScreenPlace((Quantity_Length) x,(Quantity_Length) y,(Quantity_Factor) aZoomFactor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Pan (JNIEnv *env, jobject theobj, jint dx, jint dy)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Pan((Standard_Integer) dx,(Standard_Integer) dy);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_11 (JNIEnv *env, jobject theobj, jint V)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Standard_Integer) V);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_12 (JNIEnv *env, jobject theobj, jint X, jint Y, jobject ViewX, jobject ViewY)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_ViewX = jcas_GetReal(env,ViewX);
Standard_Real the_ViewY = jcas_GetReal(env,ViewY);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((Standard_Integer) X,(Standard_Integer) Y,the_ViewX,the_ViewY);
jcas_SetReal(env,ViewX,the_ViewX);
jcas_SetReal(env,ViewY,the_ViewY);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_13 (JNIEnv *env, jobject theobj, jdouble ViewX, jdouble ViewY, jobject X, jobject Y)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_X = jcas_GetInteger(env,X);
Standard_Integer the_Y = jcas_GetInteger(env,Y);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Convert((Quantity_Length) ViewX,(Quantity_Length) ViewY,the_X,the_Y);
jcas_SetInteger(env,X,the_X);
jcas_SetInteger(env,Y,the_Y);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Convert_14 (JNIEnv *env, jobject theobj, jdouble aDriverSize)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Quantity_Length) aDriverSize);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Reset (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Reset();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Previous (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Previous();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_DisableStorePrevious (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->DisableStorePrevious();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_EnableStorePrevious (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->EnableStorePrevious();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_UpdateNew (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->UpdateNew();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_RestoreArea (JNIEnv *env, jobject theobj, jint Xc, jint Yc, jint Width, jint Height)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->RestoreArea((Standard_Integer) Xc,(Standard_Integer) Yc,(Standard_Integer) Width,(Standard_Integer) Height);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Restore (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Restore();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Dump_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Dump();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Dump_12 (JNIEnv *env, jobject theobj, jobject aFileName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Dump(the_aFileName);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Pick_11 (JNIEnv *env, jobject theobj, jint X, jint Y, jint aPrecision)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(Graphic2d_DisplayList)* theret = new Handle(Graphic2d_DisplayList);
*theret = the_this->Pick((Standard_Integer) X,(Standard_Integer) Y,(Standard_Integer) aPrecision);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Graphic2d_DisplayList",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_PickByCircle (JNIEnv *env, jobject theobj, jint X, jint Y, jint Radius)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(Graphic2d_DisplayList)* theret = new Handle(Graphic2d_DisplayList);
*theret = the_this->PickByCircle((Standard_Integer) X,(Standard_Integer) Y,(Standard_Integer) Radius);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Graphic2d_DisplayList",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Pick_12 (JNIEnv *env, jobject theobj, jint Xmin, jint Ymin, jint Xmax, jint Ymax, jshort aPickMode)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(Graphic2d_DisplayList)* theret = new Handle(Graphic2d_DisplayList);
*theret = the_this->Pick((Standard_Integer) Xmin,(Standard_Integer) Ymin,(Standard_Integer) Xmax,(Standard_Integer) Ymax,(Graphic2d_PickMode) aPickMode);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Graphic2d_DisplayList",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Erase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_MustBeResized (JNIEnv *env, jobject theobj, jshort anEffect)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->MustBeResized((V2d_TypeOfWindowResizingEffect) anEffect);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_HasBeenMoved (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->HasBeenMoved();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Plot_11 (JNIEnv *env, jobject theobj, jobject aPlotterDriver, jdouble aXCenter, jdouble aYCenter, jdouble aScale)
{

jcas_Locking alock(env);
{
try {
 Handle( PlotMgt_PlotterDriver ) the_aPlotterDriver;
 void*                ptr_aPlotterDriver = jcas_GetHandle(env,aPlotterDriver);
 
 if ( ptr_aPlotterDriver != NULL ) the_aPlotterDriver = *(   (  Handle( PlotMgt_PlotterDriver )*  )ptr_aPlotterDriver   );

Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Plot(the_aPlotterDriver,(Quantity_Length) aXCenter,(Quantity_Length) aYCenter,(Quantity_Factor) aScale);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Plot_12 (JNIEnv *env, jobject theobj, jobject aPlotterDriver, jdouble aScale)
{

jcas_Locking alock(env);
{
try {
 Handle( PlotMgt_PlotterDriver ) the_aPlotterDriver;
 void*                ptr_aPlotterDriver = jcas_GetHandle(env,aPlotterDriver);
 
 if ( ptr_aPlotterDriver != NULL ) the_aPlotterDriver = *(   (  Handle( PlotMgt_PlotterDriver )*  )ptr_aPlotterDriver   );

Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Plot(the_aPlotterDriver,(Quantity_Factor) aScale);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_PlotScreen (JNIEnv *env, jobject theobj, jobject aPlotterDriver)
{

jcas_Locking alock(env);
{
try {
 Handle( PlotMgt_PlotterDriver ) the_aPlotterDriver;
 void*                ptr_aPlotterDriver = jcas_GetHandle(env,aPlotterDriver);
 
 if ( ptr_aPlotterDriver != NULL ) the_aPlotterDriver = *(   (  Handle( PlotMgt_PlotterDriver )*  )ptr_aPlotterDriver   );

Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->PlotScreen(the_aPlotterDriver);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenCopy (JNIEnv *env, jobject theobj, jobject aPlotterDriver, jboolean fWhiteBackground, jdouble aScale)
{

jcas_Locking alock(env);
{
try {
 Handle( PlotMgt_PlotterDriver ) the_aPlotterDriver;
 void*                ptr_aPlotterDriver = jcas_GetHandle(env,aPlotterDriver);
 
 if ( ptr_aPlotterDriver != NULL ) the_aPlotterDriver = *(   (  Handle( PlotMgt_PlotterDriver )*  )ptr_aPlotterDriver   );

Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_PostScriptOutput (JNIEnv *env, jobject theobj, jobject aFile, jdouble aWidth, jdouble aHeight, jdouble aXCenter, jdouble aYCenter, jdouble aScale, jshort aTypeOfColorSpace)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aFile = jcas_ConvertToCString(env,aFile);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->PostScriptOutput(the_aFile,(Quantity_Length) aWidth,(Quantity_Length) aHeight,(Quantity_Length) aXCenter,(Quantity_Length) aYCenter,(Quantity_Factor) aScale,(Aspect_TypeOfColorSpace) aTypeOfColorSpace);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ScreenPostScriptOutput (JNIEnv *env, jobject theobj, jobject aFile, jdouble aWidth, jdouble aHeight, jshort aTypeOfColorSpace)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aFile = jcas_ConvertToCString(env,aFile);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->ScreenPostScriptOutput(the_aFile,(Quantity_Length) aWidth,(Quantity_Length) aHeight,(Aspect_TypeOfColorSpace) aTypeOfColorSpace);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Hit (JNIEnv *env, jobject theobj, jint X, jint Y, jobject gx, jobject gy)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_gx = jcas_GetReal(env,gx);
Standard_Real the_gy = jcas_GetReal(env,gy);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Hit((Standard_Integer) X,(Standard_Integer) Y,the_gx,the_gy);
jcas_SetReal(env,gx,the_gx);
jcas_SetReal(env,gy,the_gy);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_ShowHit (JNIEnv *env, jobject theobj, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->ShowHit((Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_EraseHit (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->EraseHit();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDefaultHighlightColor (JNIEnv *env, jobject theobj, jint aColorIndex)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDefaultHighlightColor((Standard_Integer) aColorIndex);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_SetDeflection (JNIEnv *env, jobject theobj, jdouble aDeflection)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetDeflection((Quantity_Length) aDeflection);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_Deflection (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Deflection();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_View (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(Graphic2d_View)* theret = new Handle(Graphic2d_View);
*theret = the_this->View();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Graphic2d_View",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_Viewer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(V2d_Viewer)* theret = new Handle(V2d_Viewer);
*theret = the_this->Viewer();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V2d_Viewer",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1View_Driver (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
Handle(Aspect_WindowDriver)* theret = new Handle(Aspect_WindowDriver);
*theret = the_this->Driver();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_WindowDriver",theret);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Zoom_14 (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Zoom();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Center (JNIEnv *env, jobject theobj, jobject aX, jobject aY)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_aX = jcas_GetReal(env,aX);
Standard_Real the_aY = jcas_GetReal(env,aY);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Center(the_aX,the_aY);
jcas_SetReal(env,aX,the_aX);
jcas_SetReal(env,aY,the_aY);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V2d_1View_Size (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Size();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Color_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Color();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Color_12 (JNIEnv *env, jobject theobj, jobject color)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_color = (Quantity_Color*) jcas_GetHandle(env,color);
if ( the_color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color = new Quantity_Color ();
 // jcas_SetHandle ( env, color, the_color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Color(*the_color);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_Scroll (JNIEnv *env, jobject theobj, jobject XCenter, jobject YCenter, jobject DX, jobject DY)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_XCenter = jcas_GetInteger(env,XCenter);
Standard_Integer the_YCenter = jcas_GetInteger(env,YCenter);
Standard_Integer the_DX = jcas_GetInteger(env,DX);
Standard_Integer the_DY = jcas_GetInteger(env,DY);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Scroll(the_XCenter,the_YCenter,the_DX,the_DY);
jcas_SetInteger(env,XCenter,the_XCenter);
jcas_SetInteger(env,YCenter,the_YCenter);
jcas_SetInteger(env,DX,the_DX);
jcas_SetInteger(env,DY,the_DY);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_V2d_1View_DefaultHighlightColor (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultHighlightColor();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1Fit_12 (JNIEnv *env, jobject theobj, jint aX1, jint aY1, jint aX2, jint aY2)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->Fit((Standard_Integer) aX1,(Standard_Integer) aY1,(Standard_Integer) aX2,(Standard_Integer) aY2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_11 (JNIEnv *env, jobject theobj, jshort aNameColor)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackground((Quantity_NameOfColor) aNameColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_12 (JNIEnv *env, jobject theobj, jobject color)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_color = (Quantity_Color*) jcas_GetHandle(env,color);
if ( the_color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color = new Quantity_Color ();
 // jcas_SetHandle ( env, color, the_color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
the_this->SetBackground(*the_color);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1View_V2d_1View_1SetBackground_13 (JNIEnv *env, jobject theobj, jobject aNameFile, jshort aMethod)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aNameFile = jcas_ConvertToCString(env,aNameFile);
Handle(V2d_View) the_this = *((Handle(V2d_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetBackground(the_aNameFile,(Aspect_FillMethod) aMethod);

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
