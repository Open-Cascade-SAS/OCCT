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

#include <CASCADESamplesJni_TColStd_Array1OfBoolean.h>
#include <TColStd_Array1OfBoolean.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_TColStd_1Array1OfBoolean_1Create_11 (JNIEnv *env, jobject theobj, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* theret = new TColStd_Array1OfBoolean((Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_TColStd_1Array1OfBoolean_1Create_12 (JNIEnv *env, jobject theobj, jboolean Item, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* theret = new TColStd_Array1OfBoolean((Standard_Boolean) Item,(Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Init (JNIEnv *env, jobject theobj, jboolean V)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
the_this->Init((Standard_Boolean) V);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_Other = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,Other);
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
const TColStd_Array1OfBoolean& theret = the_this->Assign(*the_Other);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Lower (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Upper (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_SetValue (JNIEnv *env, jobject theobj, jint Index, jboolean Value)
{

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) Index,(Standard_Boolean) Value);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_Value (JNIEnv *env, jobject theobj, jint Index)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_ChangeValue (JNIEnv *env, jobject theobj, jint Index)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColStd_Array1OfBoolean* the_this = (TColStd_Array1OfBoolean*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfBoolean_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TColStd_Array1OfBoolean* theobj = (TColStd_Array1OfBoolean*) theid;
  delete theobj;
}
}


}
