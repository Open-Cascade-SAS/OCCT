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

#include <SampleHLRJni_Aspect_ColorMapEntry.h>
#include <Aspect_ColorMapEntry.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Aspect_1ColorMapEntry_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* theret = new Aspect_ColorMapEntry();
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Aspect_1ColorMapEntry_1Create_12 (JNIEnv *env, jobject theobj, jint index, jobject rgb)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_rgb = (Quantity_Color*) jcas_GetHandle(env,rgb);
if ( the_rgb == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_rgb = new Quantity_Color ();
 // jcas_SetHandle ( env, rgb, the_rgb );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_ColorMapEntry* theret = new Aspect_ColorMapEntry((Standard_Integer) index,*the_rgb);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Aspect_1ColorMapEntry_1Create_13 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_entry = (Aspect_ColorMapEntry*) jcas_GetHandle(env,entry);
if ( the_entry == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_entry = new Aspect_ColorMapEntry ();
 // jcas_SetHandle ( env, entry, the_entry );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_ColorMapEntry* theret = new Aspect_ColorMapEntry(*the_entry);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Aspect_1ColorMapEntry_1SetValue_11 (JNIEnv *env, jobject theobj, jint index, jobject rgb)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_rgb = (Quantity_Color*) jcas_GetHandle(env,rgb);
if ( the_rgb == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_rgb = new Quantity_Color ();
 // jcas_SetHandle ( env, rgb, the_rgb );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) index,*the_rgb);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Aspect_1ColorMapEntry_1SetValue_12 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_entry = (Aspect_ColorMapEntry*) jcas_GetHandle(env,entry);
if ( the_entry == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_entry = new Aspect_ColorMapEntry ();
 // jcas_SetHandle ( env, entry, the_entry );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue(*the_entry);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_SetColor (JNIEnv *env, jobject theobj, jobject rgb)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_rgb = (Quantity_Color*) jcas_GetHandle(env,rgb);
if ( the_rgb == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_rgb = new Quantity_Color ();
 // jcas_SetHandle ( env, rgb, the_rgb );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetColor(*the_rgb);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Color (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
const Quantity_Color& theret = the_this->Color();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Quantity_Color",&theret,0);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_SetIndex (JNIEnv *env, jobject theobj, jint index)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetIndex((Standard_Integer) index);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Index (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
 thejret = the_this->Index();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Free (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
the_this->Free();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_this = (Aspect_ColorMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorMapEntry_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_ColorMapEntry* theobj = (Aspect_ColorMapEntry*) theid;
  delete theobj;
}
}


}
