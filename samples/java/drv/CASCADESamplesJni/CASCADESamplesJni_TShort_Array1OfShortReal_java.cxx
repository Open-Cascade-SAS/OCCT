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

#include <CASCADESamplesJni_TShort_Array1OfShortReal.h>
#include <TShort_Array1OfShortReal.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_TShort_1Array1OfShortReal_1Create_11 (JNIEnv *env, jobject theobj, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* theret = new TShort_Array1OfShortReal((Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_TShort_1Array1OfShortReal_1Create_12 (JNIEnv *env, jobject theobj, jfloat Item, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* theret = new TShort_Array1OfShortReal((Standard_ShortReal) Item,(Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Init (JNIEnv *env, jobject theobj, jfloat V)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
the_this->Init((Standard_ShortReal) V);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
the_this->Destroy();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_Other = (TShort_Array1OfShortReal*) jcas_GetHandle(env,Other);
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
const TShort_Array1OfShortReal& theret = the_this->Assign(*the_Other);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Lower (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
 thejret = the_this->Lower();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Upper (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
 thejret = the_this->Upper();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_SetValue (JNIEnv *env, jobject theobj, jint Index, jfloat Value)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) Index,(Standard_ShortReal) Value);

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



JNIEXPORT jfloat JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_Value (JNIEnv *env, jobject theobj, jint Index)
{
jfloat thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
 thejret = the_this->Value((Standard_Integer) Index);

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



JNIEXPORT jfloat JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_ChangeValue (JNIEnv *env, jobject theobj, jint Index)
{
jfloat thejret;

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_this = (TShort_Array1OfShortReal*) jcas_GetHandle(env,theobj);
 thejret = the_this->ChangeValue((Standard_Integer) Index);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TShort_1Array1OfShortReal_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TShort_Array1OfShortReal* theobj = (TShort_Array1OfShortReal*) theid;
  delete theobj;
}
}


}
