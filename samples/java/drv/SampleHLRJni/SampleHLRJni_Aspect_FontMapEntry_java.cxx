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

#include <SampleHLRJni_Aspect_FontMapEntry.h>
#include <Aspect_FontMapEntry.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_FontStyle.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Aspect_1FontMapEntry_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* theret = new Aspect_FontMapEntry();
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Aspect_1FontMapEntry_1Create_12 (JNIEnv *env, jobject theobj, jint index, jobject style)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_style = (Aspect_FontStyle*) jcas_GetHandle(env,style);
if ( the_style == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_style = new Aspect_FontStyle ();
 // jcas_SetHandle ( env, style, the_style );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_FontMapEntry* theret = new Aspect_FontMapEntry((Standard_Integer) index,*the_style);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Aspect_1FontMapEntry_1Create_13 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_entry = (Aspect_FontMapEntry*) jcas_GetHandle(env,entry);
if ( the_entry == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_entry = new Aspect_FontMapEntry ();
 // jcas_SetHandle ( env, entry, the_entry );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_FontMapEntry* theret = new Aspect_FontMapEntry(*the_entry);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Aspect_1FontMapEntry_1SetValue_11 (JNIEnv *env, jobject theobj, jint index, jobject style)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_style = (Aspect_FontStyle*) jcas_GetHandle(env,style);
if ( the_style == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_style = new Aspect_FontStyle ();
 // jcas_SetHandle ( env, style, the_style );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) index,*the_style);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Aspect_1FontMapEntry_1SetValue_12 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_entry = (Aspect_FontMapEntry*) jcas_GetHandle(env,entry);
if ( the_entry == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_entry = new Aspect_FontMapEntry ();
 // jcas_SetHandle ( env, entry, the_entry );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_SetType (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_Style = (Aspect_FontStyle*) jcas_GetHandle(env,Style);
if ( the_Style == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Style = new Aspect_FontStyle ();
 // jcas_SetHandle ( env, Style, the_Style );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetType(*the_Style);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Type (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
const Aspect_FontStyle& theret = the_this->Type();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_FontStyle",&theret,0);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_SetIndex (JNIEnv *env, jobject theobj, jint index)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Index (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Free (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_FontMapEntry* the_this = (Aspect_FontMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1FontMapEntry_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_FontMapEntry* theobj = (Aspect_FontMapEntry*) theid;
  delete theobj;
}
}


}
