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

#include <SampleHLRJni_TColQuantity_Array1OfLength.h>
#include <TColQuantity_Array1OfLength.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_TColQuantity_1Array1OfLength_1Create_11 (JNIEnv *env, jobject theobj, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* theret = new TColQuantity_Array1OfLength((Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_TColQuantity_1Array1OfLength_1Create_12 (JNIEnv *env, jobject theobj, jdouble Item, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* theret = new TColQuantity_Array1OfLength((Quantity_Length) Item,(Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Init (JNIEnv *env, jobject theobj, jdouble V)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
the_this->Init((Quantity_Length) V);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Other = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new TColQuantity_Array1OfLength ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
const TColQuantity_Array1OfLength& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TColQuantity_Array1OfLength",&theret,0);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Lower (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Upper (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_SetValue (JNIEnv *env, jobject theobj, jint Index, jdouble Value)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) Index,(Quantity_Length) Value);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jdouble JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_Value (JNIEnv *env, jobject theobj, jint Index)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jdouble JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_ChangeValue (JNIEnv *env, jobject theobj, jint Index)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_TColQuantity_1Array1OfLength_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TColQuantity_Array1OfLength* theobj = (TColQuantity_Array1OfLength*) theid;
  delete theobj;
}
}


}
