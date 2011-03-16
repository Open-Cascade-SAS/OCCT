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

#include <CASCADESamplesJni_V2d_Viewer.h>
#include <V2d_Viewer.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_GraphicDevice.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_CString.hxx>
#include <Graphic2d_View.hxx>
#include <V2d_View.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_MarkMap.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_GridType.hxx>
#include <Standard_Real.hxx>
#include <Aspect_GridDrawMode.hxx>
#include <Quantity_Color.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_V2d_1Viewer_1Create_11 (JNIEnv *env, jobject theobj, jobject aGraphicDevice, jstring aName, jobject aDomain)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_GraphicDevice ) the_aGraphicDevice;
 void*                ptr_aGraphicDevice = jcas_GetHandle(env,aGraphicDevice);
 
 if ( ptr_aGraphicDevice != NULL ) the_aGraphicDevice = *(   (  Handle( Aspect_GraphicDevice )*  )ptr_aGraphicDevice   );

Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Standard_CString the_aDomain = jcas_ConvertToCString(env,aDomain);
Handle(V2d_Viewer)* theret = new Handle(V2d_Viewer);
*theret = new V2d_Viewer(the_aGraphicDevice,the_aName,the_aDomain);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_V2d_1Viewer_1Create_12 (JNIEnv *env, jobject theobj, jobject aGraphicDevice, jobject aView, jstring aName, jobject aDomain)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_GraphicDevice ) the_aGraphicDevice;
 void*                ptr_aGraphicDevice = jcas_GetHandle(env,aGraphicDevice);
 
 if ( ptr_aGraphicDevice != NULL ) the_aGraphicDevice = *(   (  Handle( Aspect_GraphicDevice )*  )ptr_aGraphicDevice   );

 Handle( Graphic2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( Graphic2d_View )*  )ptr_aView   );

Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Standard_CString the_aDomain = jcas_ConvertToCString(env,aDomain);
Handle(V2d_Viewer)* theret = new Handle(V2d_Viewer);
*theret = new V2d_Viewer(the_aGraphicDevice,the_aView,the_aName,the_aDomain);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_AddView (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V2d_View )*  )ptr_aView   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->AddView(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_RemoveView (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V2d_View )*  )ptr_aView   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->RemoveView(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_UpdateNew (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetColorMap (JNIEnv *env, jobject theobj, jobject aColorMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_ColorMap ) the_aColorMap;
 void*                ptr_aColorMap = jcas_GetHandle(env,aColorMap);
 
 if ( ptr_aColorMap != NULL ) the_aColorMap = *(   (  Handle( Aspect_ColorMap )*  )ptr_aColorMap   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetColorMap(the_aColorMap);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetTypeMap (JNIEnv *env, jobject theobj, jobject aTypeMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_TypeMap ) the_aTypeMap;
 void*                ptr_aTypeMap = jcas_GetHandle(env,aTypeMap);
 
 if ( ptr_aTypeMap != NULL ) the_aTypeMap = *(   (  Handle( Aspect_TypeMap )*  )ptr_aTypeMap   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetTypeMap(the_aTypeMap);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetWidthMap (JNIEnv *env, jobject theobj, jobject aWidthMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_WidthMap ) the_aWidthMap;
 void*                ptr_aWidthMap = jcas_GetHandle(env,aWidthMap);
 
 if ( ptr_aWidthMap != NULL ) the_aWidthMap = *(   (  Handle( Aspect_WidthMap )*  )ptr_aWidthMap   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetWidthMap(the_aWidthMap);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetFontMap (JNIEnv *env, jobject theobj, jobject aFontMap, jboolean useMFT)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_FontMap ) the_aFontMap;
 void*                ptr_aFontMap = jcas_GetHandle(env,aFontMap);
 
 if ( ptr_aFontMap != NULL ) the_aFontMap = *(   (  Handle( Aspect_FontMap )*  )ptr_aFontMap   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetFontMap(the_aFontMap,(Standard_Boolean) useMFT);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetMarkMap (JNIEnv *env, jobject theobj, jobject aMarkMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_MarkMap ) the_aMarkMap;
 void*                ptr_aMarkMap = jcas_GetHandle(env,aMarkMap);
 
 if ( ptr_aMarkMap != NULL ) the_aMarkMap = *(   (  Handle( Aspect_MarkMap )*  )ptr_aMarkMap   );

Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetMarkMap(the_aMarkMap);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_ColorMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_ColorMap)* theret = new Handle(Aspect_ColorMap);
*theret = the_this->ColorMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMap",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_TypeMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_TypeMap)* theret = new Handle(Aspect_TypeMap);
*theret = the_this->TypeMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_TypeMap",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_WidthMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_WidthMap)* theret = new Handle(Aspect_WidthMap);
*theret = the_this->WidthMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_WidthMap",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_FontMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_FontMap)* theret = new Handle(Aspect_FontMap);
*theret = the_this->FontMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_FontMap",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_MarkMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_MarkMap)* theret = new Handle(Aspect_MarkMap);
*theret = the_this->MarkMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_MarkMap",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1Viewer_UseMFT (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->UseMFT();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_View (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_V2d_1Viewer_InitializeColor (JNIEnv *env, jobject theobj, jshort aColor)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
 thejret = the_this->InitializeColor((Quantity_NameOfColor) aColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_InitActiveViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1Viewer_MoreActiveViews (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_NextActiveViews (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V2d_1Viewer_ActiveView (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
Handle(V2d_View)* theret = new Handle(V2d_View);
*theret = the_this->ActiveView();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V2d_View",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1Viewer_IsEmpty (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_V2d_1Viewer_GridType (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_Hit (JNIEnv *env, jobject theobj, jdouble X, jdouble Y, jobject gx, jobject gy)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_gx = jcas_GetReal(env,gx);
Standard_Real the_gy = jcas_GetReal(env,gy);
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Hit((Quantity_Length) X,(Quantity_Length) Y,the_gx,the_gy);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_ActivateGrid (JNIEnv *env, jobject theobj, jshort aGridType, jshort aGridDrawMode)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_DeactivateGrid (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_V2d_1Viewer_IsActive (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_RectangularGridValues (JNIEnv *env, jobject theobj, jobject XOrigin, jobject YOrigin, jobject XStep, jobject YStep, jobject RotationAngle)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XOrigin = jcas_GetReal(env,XOrigin);
Standard_Real the_YOrigin = jcas_GetReal(env,YOrigin);
Standard_Real the_XStep = jcas_GetReal(env,XStep);
Standard_Real the_YStep = jcas_GetReal(env,YStep);
Standard_Real the_RotationAngle = jcas_GetReal(env,RotationAngle);
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetRectangularGridValues (JNIEnv *env, jobject theobj, jdouble XOrigin, jdouble YOrigin, jdouble XStep, jdouble YStep, jdouble RotationAngle)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_CircularGridValues (JNIEnv *env, jobject theobj, jobject XOrigin, jobject YOrigin, jobject RadiusStep, jobject DivisionNumber, jobject RotationAngle)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_XOrigin = jcas_GetReal(env,XOrigin);
Standard_Real the_YOrigin = jcas_GetReal(env,YOrigin);
Standard_Real the_RadiusStep = jcas_GetReal(env,RadiusStep);
Standard_Integer the_DivisionNumber = jcas_GetInteger(env,DivisionNumber);
Standard_Real the_RotationAngle = jcas_GetReal(env,RotationAngle);
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetCircularGridValues (JNIEnv *env, jobject theobj, jdouble XOrigin, jdouble YOrigin, jdouble RadiusStep, jint DivisionNumber, jdouble RotationAngle)
{

jcas_Locking alock(env);
{
try {
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V2d_1Viewer_SetGridColor (JNIEnv *env, jobject theobj, jobject color1, jobject color2)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_color1 = (Quantity_Color*) jcas_GetHandle(env,color1);
if ( the_color1 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color1 = new Quantity_Color ();
 // jcas_SetHandle ( env, color1, the_color1 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_color2 = (Quantity_Color*) jcas_GetHandle(env,color2);
if ( the_color2 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color2 = new Quantity_Color ();
 // jcas_SetHandle ( env, color2, the_color2 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(V2d_Viewer) the_this = *((Handle(V2d_Viewer)*) jcas_GetHandle(env,theobj));
the_this->SetGridColor(*the_color1,*the_color2);

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
