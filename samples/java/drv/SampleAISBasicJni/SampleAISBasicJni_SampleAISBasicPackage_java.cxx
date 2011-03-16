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


#include <SampleAISBasicJni_SampleAISBasicPackage.h>
#include <SampleAISBasicPackage.hxx>
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


JNIEXPORT jobject JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_CreateViewer (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleAISBasicPackage::CreateViewer(the_aName);
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



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_SetWindow (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleAISBasicPackage::SetWindow(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_DisplayTrihedron (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleAISBasicPackage::DisplayTrihedron(the_aContext);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_DisplayCircle (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISBasicPackage::DisplayCircle(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_DisplayLine (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISBasicPackage::DisplayLine(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_DisplaySphere (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISBasicPackage::DisplaySphere(the_aContext,*the_Message);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleAISBasicJni_SampleAISBasicPackage_DisplayCylinder (JNIEnv *env, jclass, jobject aContext, jobject Message)
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
SampleAISBasicPackage::DisplayCylinder(the_aContext,*the_Message);

}
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
