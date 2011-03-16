//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.

#include <CASCADESamplesJni_Aspect_MarkerStyle.h>
#include <Aspect_MarkerStyle.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeOfMarker.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TShort_Array1OfShortReal.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_11 (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_12 (JNIEnv *env, jobject theobj, jint aType)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_13 (JNIEnv *env, jobject theobj, jobject aXpoint, jobject aYpoint)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfReal* the_aXpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aXpoint);
TColStd_Array1OfReal* the_aYpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aYpoint);
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle(*the_aXpoint,*the_aYpoint);
jcas_SetHandle(env,theobj,theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Aspect_1MarkerStyle_1Create_14 (JNIEnv *env, jobject theobj, jobject aXpoint, jobject aYpoint, jobject aSpoint)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfReal* the_aXpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aXpoint);
TColStd_Array1OfReal* the_aYpoint = (TColStd_Array1OfReal*) jcas_GetHandle(env,aYpoint);
TColStd_Array1OfBoolean* the_aSpoint = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,aSpoint);
Aspect_MarkerStyle* theret = new Aspect_MarkerStyle(*the_aXpoint,*the_aYpoint,*the_aSpoint);
jcas_SetHandle(env,theobj,theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
const Aspect_MarkerStyle& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_MarkerStyle",&theret,0);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Type (JNIEnv *env, jobject theobj)
{
jint thejret;

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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Length (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_Values (JNIEnv *env, jobject theobj, jint aRank, jobject aX, jobject aY)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_XValues (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_YValues (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_SValues (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsEqual(*the_Other);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_IsNotEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Other = (Aspect_MarkerStyle*) jcas_GetHandle(env,Other);
Aspect_MarkerStyle* the_this = (Aspect_MarkerStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsNotEqual(*the_Other);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkerStyle_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_MarkerStyle* theobj = (Aspect_MarkerStyle*) theid;
  delete theobj;
}
}


}
