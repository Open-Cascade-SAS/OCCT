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

#include <SampleHLRJni_Aspect_MarkerStyle.h>
#include <Aspect_MarkerStyle.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeOfMarker.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TShort_Array1OfShortReal.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle();
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_12 (JNIEnv *env, jobject theobj, jshort aType)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle((Aspect_TypeOfMarker) aType);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_13 (JNIEnv *env, jobject theobj, jobject aXpoint, jobject aYpoint)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfReal* the_aXpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aXpoint);
if ( the_aXpoint == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aXpoint = new TColStd_Array1OfReal ();
 // jcas_SetHandle ( env, aXpoint, the_aXpoint );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TColStd_Array1OfReal* the_aYpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aYpoint);
if ( the_aYpoint == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aYpoint = new TColStd_Array1OfReal ();
 // jcas_SetHandle ( env, aYpoint, the_aYpoint );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle(*the_aXpoint,*the_aYpoint);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_14 (JNIEnv *env, jobject theobj, jobject aXpoint, jobject aYpoint, jobject aSpoint)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfReal* the_aXpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aXpoint);
if ( the_aXpoint == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aXpoint = new TColStd_Array1OfReal ();
 // jcas_SetHandle ( env, aXpoint, the_aXpoint );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TColStd_Array1OfReal* the_aYpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aYpoint);
if ( the_aYpoint == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aYpoint = new TColStd_Array1OfReal ();
 // jcas_SetHandle ( env, aYpoint, the_aYpoint );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TColStd_Array1OfBoolean* the_aSpoint = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,aSpoint);
if ( the_aSpoint == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aSpoint = new TColStd_Array1OfBoolean ();
 // jcas_SetHandle ( env, aSpoint, the_aSpoint );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle(*the_aXpoint,*the_aYpoint,*the_aSpoint);
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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_MarkerStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
const Aspect_MarkerStyle& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_MarkerStyle",&theret,0);

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



JNIEXPORT jshort JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Type (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->Type();

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_Values (JNIEnv *env, jobject theobj, jint aRank, jobject aX, jobject aY)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_aX = jcas_GetReal(env,aX);
Standard_Real the_aY = jcas_GetReal(env,aY);
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->Values((Standard_Integer) aRank,the_aX,the_aY);
jcas_SetReal(env,aX,the_aX);
jcas_SetReal(env,aY,the_aY);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_XValues (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
const TShort_Array1OfShortReal& theret = the_this->XValues();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TShort_Array1OfShortReal",&theret,0);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_YValues (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
const TShort_Array1OfShortReal& theret = the_this->YValues();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TShort_Array1OfShortReal",&theret,0);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_SValues (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
const TColStd_Array1OfBoolean& theret = the_this->SValues();
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_MarkerStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_IsNotEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Aspect_MarkerStyle ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1MarkerStyle_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_MarkerStyle* theobj = (Aspect_MarkerStyle*) theid;
  delete theobj;
}
}


}
