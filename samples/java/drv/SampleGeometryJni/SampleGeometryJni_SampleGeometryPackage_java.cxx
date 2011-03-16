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


#include <SampleGeometryJni_SampleGeometryPackage.h>
#include <SampleGeometryPackage.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_ExtString.hxx>
#include <V3d_View.hxx>
#include <Standard_Integer.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <ISession2D_InteractiveContext.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleGeometryJni_SampleGeometryPackage_CreateViewer3d (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleGeometryPackage::CreateViewer3d(the_aName);
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



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_SetWindow3d (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleGeometryPackage::SetWindow3d(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_SampleGeometryJni_SampleGeometryPackage_CreateViewer2d (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V2d_Viewer)* theret = new Handle(V2d_Viewer);
*theret = SampleGeometryPackage::CreateViewer2d(the_aName);
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



JNIEXPORT jobject JNICALL Java_SampleGeometryJni_SampleGeometryPackage_CreateView2d (JNIEnv *env, jclass, jobject aViewer, jint hiwin, jint lowin)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

Handle(V2d_View)* theret = new Handle(V2d_View);
*theret = SampleGeometryPackage::CreateView2d(the_aViewer,(Standard_Integer) hiwin,(Standard_Integer) lowin);
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



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest1 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest1(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest2 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest2(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest3 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest3(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest4 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest4(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest5 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest5(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest6 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest6(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest7 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest7(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest8 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest8(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest9 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest9(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest10 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest10(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest11 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest11(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest12 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest12(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest13 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest13(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest14 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest14(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest15 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest15(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest16 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest16(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest17 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest17(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest18 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest18(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest19 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest19(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest20 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest20(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest21 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest21(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest22 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest22(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest23 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest23(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest24 (JNIEnv *env, jclass, jobject aContext, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest24(the_aContext,the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest25 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest25(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest26 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest26(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest27 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest27(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest28 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest28(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest29 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest29(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest30 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest30(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest31 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest31(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest32 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest32(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest33 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest33(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest34 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest34(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest35 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest35(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest36 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest36(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest37 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest37(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest38 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest38(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest39 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest39(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest40 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest40(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest41 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest41(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest42 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest42(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest43 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest43(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest44 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest44(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest45 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest45(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest46 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest46(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest47 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest47(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest48 (JNIEnv *env, jclass, jobject aContext2D, jobject Message)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2D;
 void*                ptr_aContext2D = jcas_GetHandle(env,aContext2D);
 
 if ( ptr_aContext2D != NULL ) the_aContext2D = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2D   );

TCollection_AsciiString* the_Message = (TCollection_AsciiString*) jcas_GetHandle(env,Message);
if ( the_Message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Message, the_Message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
SampleGeometryPackage::gpTest48(the_aContext2D,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest49 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest49(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleGeometryJni_SampleGeometryPackage_gpTest50 (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleGeometryPackage::gpTest50(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleGeometryJni_SampleGeometryPackage_SaveImage (JNIEnv *env, jclass, jobject aFileName, jobject aFormat, jobject aView)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
Standard_CString the_aFormat = jcas_ConvertToCString(env,aFormat);
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

 thejret = SampleGeometryPackage::SaveImage(the_aFileName,the_aFormat,the_aView);

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
