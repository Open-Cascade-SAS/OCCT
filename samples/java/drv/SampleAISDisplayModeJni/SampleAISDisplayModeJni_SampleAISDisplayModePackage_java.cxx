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


#include <SampleAISDisplayModeJni_SampleAISDisplayModePackage.h>
#include <SampleAISDisplayModePackage.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_ExtString.hxx>
#include <V3d_View.hxx>
#include <Standard_Integer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <AIS_DisplayMode.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Real.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_CreateViewer (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleAISDisplayModePackage::CreateViewer(the_aName);
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



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetWindow (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleAISDisplayModePackage::SetWindow(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_DisplayBox (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISDisplayModePackage::DisplayBox(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_DisplaySphere (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISDisplayModePackage::DisplaySphere(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_DisplayCylinder (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISDisplayModePackage::DisplayCylinder(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_EraseAll (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISDisplayModePackage::EraseAll(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_InitContext (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleAISDisplayModePackage::InitContext(the_aContext);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_GetIsosNumber (JNIEnv *env, jclass, jobject aContext, jobject u, jobject v)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

Standard_Integer the_u = jcas_GetInteger(env,u);
Standard_Integer the_v = jcas_GetInteger(env,v);
SampleAISDisplayModePackage::GetIsosNumber(the_aContext,the_u,the_v);
jcas_SetInteger(env,u,the_u);
jcas_SetInteger(env,v,the_v);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetIsosNumber (JNIEnv *env, jclass, jobject aContext, jint u, jint v, jobject Message)
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
SampleAISDisplayModePackage::SetIsosNumber(the_aContext,(Standard_Integer) u,(Standard_Integer) v,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetDisplayMode (JNIEnv *env, jclass, jobject aContext, jshort aMode, jobject Message)
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
SampleAISDisplayModePackage::SetDisplayMode(the_aContext,(AIS_DisplayMode) aMode,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetObjectDisplayMode (JNIEnv *env, jclass, jobject aContext, jshort aMode, jobject Message)
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
SampleAISDisplayModePackage::SetObjectDisplayMode(the_aContext,(AIS_DisplayMode) aMode,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetObjectMaterial (JNIEnv *env, jclass, jobject aContext, jshort aName, jobject Message)
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
SampleAISDisplayModePackage::SetObjectMaterial(the_aContext,(Graphic3d_NameOfMaterial) aName,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_GetObjectColor (JNIEnv *env, jclass, jobject aContext)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

Quantity_Color* theret = new Quantity_Color(SampleAISDisplayModePackage::GetObjectColor(the_aContext));
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



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetObjectColor (JNIEnv *env, jclass, jobject aContext, jobject aColor, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleAISDisplayModePackage::SetObjectColor(the_aContext,*the_aColor,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jdouble JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_GetObjectTransparency (JNIEnv *env, jclass, jobject aContext)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleAISDisplayModePackage::GetObjectTransparency(the_aContext);

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



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetObjectTransparency (JNIEnv *env, jclass, jobject aContext, jdouble aValue, jobject Message)
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
SampleAISDisplayModePackage::SetObjectTransparency(the_aContext,(Standard_Real) aValue,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_CreateLight (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleAISDisplayModePackage::CreateLight(the_aContext);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetFirstPointOfLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y)
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

SampleAISDisplayModePackage::SetFirstPointOfLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_MoveSecondPointOfLight (JNIEnv *env, jclass, jobject aContext, jobject aView, jint X, jint Y)
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

SampleAISDisplayModePackage::MoveSecondPointOfLight(the_aContext,the_aView,(Standard_Integer) X,(Standard_Integer) Y);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISDisplayModeJni_SampleAISDisplayModePackage_SetSecondPointOfLight (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleAISDisplayModePackage::SetSecondPointOfLight(the_aContext);

}
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
