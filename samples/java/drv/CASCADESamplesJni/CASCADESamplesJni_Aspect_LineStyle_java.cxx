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

#include <CASCADESamplesJni_Aspect_LineStyle.h>
#include <Aspect_LineStyle.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_11 (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_12 (JNIEnv *env, jobject theobj, jint Type)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Aspect_1LineStyle_1Create_13 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Style = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Style);
Aspect_LineStyle* theret = new Aspect_LineStyle(*the_Style);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
const Aspect_LineStyle& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_LineStyle",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Aspect_1LineStyle_1SetValues_11 (JNIEnv *env, jobject theobj, jint Type)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Aspect_1LineStyle_1SetValues_12 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Style = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Style);
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues(*the_Style);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Style (JNIEnv *env, jobject theobj)
{
jint thejret;

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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Length (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_Values (JNIEnv *env, jobject theobj)
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
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_IsNotEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_LineStyle* the_Other = (Aspect_LineStyle*) jcas_GetHandle(env,Other);
Aspect_LineStyle* the_this = (Aspect_LineStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1LineStyle_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_LineStyle* theobj = (Aspect_LineStyle*) theid;
  delete theobj;
}
}


}
