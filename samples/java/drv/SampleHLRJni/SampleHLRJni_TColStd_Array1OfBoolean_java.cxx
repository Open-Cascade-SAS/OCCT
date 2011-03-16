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

#include <SampleHLRJni_TColStd_Array1OfBoolean.h>
#include <TColStd_Array1OfBoolean.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_TColStd_1Array1OfBoolean_1Create_11 (JNIEnv *env, jobject theobj, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* theret = new TColStd_Array1OfBoolean((Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_TColStd_1Array1OfBoolean_1Create_12 (JNIEnv *env, jobject theobj, jboolean Item, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* theret = new TColStd_Array1OfBoolean((Standard_Boolean) Item,(Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Init (JNIEnv *env, jobject theobj, jboolean V)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
the_this->Init((Standard_Boolean) V);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
the_this->Destroy();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsAllocated();

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_Other = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new TColStd_Array1OfBoolean ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
const TColStd_Array1OfBoolean& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TColStd_Array1OfBoolean",&theret,0);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->Length();

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Lower (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->Lower();

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Upper (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->Upper();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_SetValue (JNIEnv *env, jobject theobj, jint Index, jboolean Value)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) Index,(Standard_Boolean) Value);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_Value (JNIEnv *env, jobject theobj, jint Index)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->Value((Standard_Integer) Index);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_ChangeValue (JNIEnv *env, jobject theobj, jint Index)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
 thejret = the_this->ChangeValue((Standard_Integer) Index);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColStd_1Array1OfBoolean_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TColStd_Array1OfBoolean* theobj = (TColStd_Array1OfBoolean*) theid;
  delete theobj;
}
}


}
