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

#include <CASCADESamplesJni_Aspect_MarkMapEntry.h>
#include <Aspect_MarkMapEntry.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_MarkerStyle.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Aspect_1MarkMapEntry_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* theret = new Aspect_MarkMapEntry();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Aspect_1MarkMapEntry_1Create_12 (JNIEnv *env, jobject theobj, jint index, jobject style)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_style = (Aspect_MarkerStyle*) jcas_GetHandle(env,style);
Aspect_MarkMapEntry* theret = new Aspect_MarkMapEntry((Standard_Integer) index,*the_style);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Aspect_1MarkMapEntry_1Create_13 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_entry = (Aspect_MarkMapEntry*) jcas_GetHandle(env,entry);
Aspect_MarkMapEntry* theret = new Aspect_MarkMapEntry(*the_entry);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Aspect_1MarkMapEntry_1SetValue_11 (JNIEnv *env, jobject theobj, jint index, jobject style)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_style = (Aspect_MarkerStyle*) jcas_GetHandle(env,style);
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) index,*the_style);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Aspect_1MarkMapEntry_1SetValue_12 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_entry = (Aspect_MarkMapEntry*) jcas_GetHandle(env,entry);
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue(*the_entry);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_SetStyle (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkerStyle* the_Style = (Aspect_MarkerStyle*) jcas_GetHandle(env,Style);
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetStyle(*the_Style);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Style (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
const Aspect_MarkerStyle& theret = the_this->Style();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_SetIndex (JNIEnv *env, jobject theobj, jint index)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetIndex((Standard_Integer) index);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Index (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
 thejret = the_this->Index();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Free (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->Free();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsAllocated();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_MarkMapEntry* the_this = (Aspect_MarkMapEntry*) jcas_GetHandle(env,theobj);
the_this->Dump();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1MarkMapEntry_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_MarkMapEntry* theobj = (Aspect_MarkMapEntry*) theid;
  delete theobj;
}
}


}
