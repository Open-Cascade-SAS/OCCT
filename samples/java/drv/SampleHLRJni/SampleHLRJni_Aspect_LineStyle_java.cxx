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

#include <SampleHLRJni_Aspect_LineStyle.h>
#include <Aspect_LineStyle.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* theret = new Aspect_LineStyle();
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_12 (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* theret = new Aspect_LineStyle((Aspect_TypeOfLine) Type);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_13 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Style = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Style);
if ( the_Style == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Style = new TColQuantity_Array1OfLength ();
 // jcas_SetHandle ( env, Style, the_Style );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_LineStyle* theret = new Aspect_LineStyle(*the_Style);
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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_LineStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
const Aspect_LineStyle& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_LineStyle",&theret,0);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Aspect_1LineStyle_1SetValues_11 (JNIEnv *env, jobject theobj, jshort Type)
{

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues((Aspect_TypeOfLine) Type);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Aspect_1LineStyle_1SetValues_12 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Style = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Style);
if ( the_Style == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Style = new TColQuantity_Array1OfLength ();
 // jcas_SetHandle ( env, Style, the_Style );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues(*the_Style);

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



JNIEXPORT jshort JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Style (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->Style();

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1LineStyle_Values (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
const TColQuantity_Array1OfLength& theret = the_this->Values();
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1LineStyle_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_LineStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsEqual(*the_Other);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1LineStyle_IsNotEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_LineStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsNotEqual(*the_Other);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1LineStyle_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_LineStyle* theobj = (Aspect_LineStyle*) theid;
  delete theobj;
}
}


}
