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


#include <SamplesTopologyJni_SamplesTopologyPackage.h>
#include <SamplesTopologyPackage.hxx>
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


extern "C" {


JNIEXPORT jobject JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_CreateViewer (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SamplesTopologyPackage::CreateViewer(the_aName);
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



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_SetWindow (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SamplesTopologyPackage::SetWindow(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeBox (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeBox(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeCylinder (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeCylinder(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeCone (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeCone(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeSphere (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeSphere(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeTorus (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeTorus(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeWedge (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeWedge(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakePrism (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakePrism(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeRevol (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeRevol(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakePipe (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakePipe(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeThru (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeThru(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeEvolved (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeEvolved(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_MakeDraft (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::MakeDraft(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Cut (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Cut(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Fuse (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Fuse(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Common (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Common(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Section (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Section(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_PSection (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::PSection(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Blend (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Blend(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_EvolvedBlend (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::EvolvedBlend(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Chamfer (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Chamfer(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Vertex (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Vertex(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Edge (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Edge(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Wire (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Wire(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Face (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Face(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Shell (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Shell(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Compound (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Compound(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Sewing (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Sewing(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Builder (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Builder(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Geometry (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Geometry(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Explorer (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Explorer(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Validity (JNIEnv *env, jclass, jobject aContext, jobject Message, jobject Result)
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
TCollection_AsciiString* the_Result = (TCollection_AsciiString*) jcas_GetHandle(env,Result);
if ( the_Result == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Result = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Result, the_Result );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SamplesTopologyPackage::Validity(the_aContext,*the_Message,*the_Result);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_LinearProp (JNIEnv *env, jclass, jobject aContext, jobject Message, jobject Result)
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
TCollection_AsciiString* the_Result = (TCollection_AsciiString*) jcas_GetHandle(env,Result);
if ( the_Result == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Result = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Result, the_Result );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SamplesTopologyPackage::LinearProp(the_aContext,*the_Message,*the_Result);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_SurfaceProp (JNIEnv *env, jclass, jobject aContext, jobject Message, jobject Result)
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
TCollection_AsciiString* the_Result = (TCollection_AsciiString*) jcas_GetHandle(env,Result);
if ( the_Result == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Result = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Result, the_Result );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SamplesTopologyPackage::SurfaceProp(the_aContext,*the_Message,*the_Result);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_VolumeProp (JNIEnv *env, jclass, jobject aContext, jobject Message, jobject Result)
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
TCollection_AsciiString* the_Result = (TCollection_AsciiString*) jcas_GetHandle(env,Result);
if ( the_Result == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Result = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Result, the_Result );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SamplesTopologyPackage::VolumeProp(the_aContext,*the_Message,*the_Result);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Mirror (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Mirror(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Mirroraxis (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Mirroraxis(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Rotate (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Rotate(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Scale (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Scale(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Translation (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Translation(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Displacement (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Displacement(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Deform (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Deform(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_LocalPrism (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::LocalPrism(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_LocalDPrism (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::LocalDPrism(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_LocalRevol (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::LocalRevol(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_LocalPipe (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::LocalPipe(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Rib (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Rib(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Glue (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Glue(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Split (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Split(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Thick (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Thick(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_Offset (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::Offset(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_BuildMesh (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::BuildMesh(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_DisplayMesh (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::DisplayMesh(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SamplesTopologyJni_SamplesTopologyPackage_ClearMesh (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SamplesTopologyPackage::ClearMesh(the_aContext,*the_Message);

}
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
