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


#include <SampleViewer3DJni_SampleViewer3DPackage.h>
#include <SampleViewer3DPackage.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_ExtString.hxx>
#include <V3d_View.hxx>
#include <Standard_Integer.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Real.hxx>
#include <V3d_TypeOfZclipping.hxx>
#include <Standard_Boolean.hxx>
#include <V3d_TypeOfShadingModel.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateViewer (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleViewer3DPackage::CreateViewer(the_aName);
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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_SetWindow (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::SetWindow(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

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



JNIEXPORT jobject JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateGraphicDriver (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver)* theret = new Handle(Graphic3d_GraphicDriver);
*theret = SampleViewer3DPackage::CreateGraphicDriver();
thejret = jcas_CreateObject(env,"SampleViewer3DJni/Graphic3d_GraphicDriver",theret);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DisplayBox (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::DisplayBox(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DisplayCylinder (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::DisplayCylinder(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DisplaySphere (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::DisplaySphere(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_EraseAll (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::EraseAll(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateSpotLight (JNIEnv *env, jclass, jobject aContext, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::CreateSpotLight(the_aContext,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_SetSpotLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::SetSpotLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectingSpotLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::DirectingSpotLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectSpotLight (JNIEnv *env, jclass, jobject aView, jint X, jint Y, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::DirectSpotLight(the_aView,(Standard_Integer) X,(Standard_Integer) Y,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ExpandingSpotLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::ExpandingSpotLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ExpandSpotLight (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::ExpandSpotLight(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreatePositionalLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::CreatePositionalLight(the_aContext,the_aView,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectingPositionalLight (JNIEnv *env, jclass, jobject aView, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::DirectingPositionalLight(the_aView,(Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectPositionalLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::DirectPositionalLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateDirectionalLight (JNIEnv *env, jclass, jobject aContext, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::CreateDirectionalLight(the_aContext,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_SetDirectionalLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::SetDirectionalLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectingDirectionalLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::DirectingDirectionalLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DirectDirectionalLight (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleViewer3DPackage::DirectDirectionalLight(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateAmbientLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::CreateAmbientLight(the_aContext,the_aView,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ClearLights (JNIEnv *env, jclass, jobject aView, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ClearLights(the_aView,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZClippingDepth (JNIEnv *env, jclass, jobject aView, jdouble Depth, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZClippingDepth(the_aView,(Quantity_Length) Depth,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZClippingWidth (JNIEnv *env, jclass, jobject aView, jdouble Width, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZClippingWidth(the_aView,(Quantity_Length) Width,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZClippingType (JNIEnv *env, jclass, jobject aView, jshort Type, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZClippingType(the_aView,(V3d_TypeOfZclipping) Type,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZCueingDepth (JNIEnv *env, jclass, jobject aView, jdouble Depth, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZCueingDepth(the_aView,(Quantity_Length) Depth,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZCueingWidth (JNIEnv *env, jclass, jobject aView, jdouble Width, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZCueingWidth(the_aView,(Quantity_Length) Width,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeZCueingOnOff (JNIEnv *env, jclass, jobject aView, jboolean IsOn, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeZCueingOnOff(the_aView,(Standard_Boolean) IsOn,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeShadingModel (JNIEnv *env, jclass, jobject aView, jshort Model)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::ChangeShadingModel(the_aView,(V3d_TypeOfShadingModel) Model);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeAntialiasing (JNIEnv *env, jclass, jobject aView, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeAntialiasing(the_aView,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_CreateClippingPlane (JNIEnv *env, jclass, jobject aViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V3d_Viewer )*  )ptr_aViewer   );

SampleViewer3DPackage::CreateClippingPlane(the_aViewer);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_DisplayClippingPlane (JNIEnv *env, jclass, jobject aContext, jobject aView, jobject Z, jobject IsOn)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Standard_Real the_Z = jcas_GetReal(env,Z);
Standard_Boolean the_IsOn = jcas_GetBoolean(env,IsOn);
SampleViewer3DPackage::DisplayClippingPlane(the_aContext,the_aView,the_Z,the_IsOn);
jcas_SetReal(env,Z,the_Z);
jcas_SetBoolean(env,IsOn,the_IsOn);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeModelClippingZ (JNIEnv *env, jclass, jobject aContext, jobject aView, jdouble Z, jboolean IsOn, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeModelClippingZ(the_aContext,the_aView,(Standard_Real) Z,(Standard_Boolean) IsOn,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ChangeModelClippingOnOff (JNIEnv *env, jclass, jobject aContext, jobject aView, jboolean IsOn, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleViewer3DPackage::ChangeModelClippingOnOff(the_aContext,the_aView,(Standard_Boolean) IsOn,*the_Message);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_SampleViewer3DPackage_ClearClippingPlane (JNIEnv *env, jclass, jobject aContext, jobject aView, jboolean IsOn)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleViewer3DPackage::ClearClippingPlane(the_aContext,the_aView,(Standard_Boolean) IsOn);

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
