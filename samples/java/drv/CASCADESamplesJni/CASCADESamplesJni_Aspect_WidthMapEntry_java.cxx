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

#include <CASCADESamplesJni_Aspect_WidthMapEntry.h>
#include <Aspect_WidthMapEntry.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_WidthOfLine.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* theret = new Aspect_WidthMapEntry();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1Create_12 (JNIEnv *env, jobject theobj, jint index, jint style)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* theret = new Aspect_WidthMapEntry((Standard_Integer) index,(Aspect_WidthOfLine) style);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1Create_13 (JNIEnv *env, jobject theobj, jint index, jdouble width)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* theret = new Aspect_WidthMapEntry((Standard_Integer) index,(Quantity_Length) width);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1Create_14 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_entry = (Aspect_WidthMapEntry*) jcas_GetHandle(env,entry);
Aspect_WidthMapEntry* theret = new Aspect_WidthMapEntry(*the_entry);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1SetValue_11 (JNIEnv *env, jobject theobj, jint index, jint style)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) index,(Aspect_WidthOfLine) style);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1SetValue_12 (JNIEnv *env, jobject theobj, jint index, jdouble width)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) index,(Quantity_Length) width);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Aspect_1WidthMapEntry_1SetValue_13 (JNIEnv *env, jobject theobj, jobject entry)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_entry = (Aspect_WidthMapEntry*) jcas_GetHandle(env,entry);
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_SetIndex (JNIEnv *env, jobject theobj, jint index)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_SetType (JNIEnv *env, jobject theobj, jint Style)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetType((Aspect_WidthOfLine) Style);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_SetWidth (JNIEnv *env, jobject theobj, jdouble Width)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
the_this->SetWidth((Quantity_Length) Width);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Type (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Width (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
 thejret = the_this->Width();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Index (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Free (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_WidthMapEntry* the_this = (Aspect_WidthMapEntry*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1WidthMapEntry_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_WidthMapEntry* theobj = (Aspect_WidthMapEntry*) theid;
  delete theobj;
}
}


}
