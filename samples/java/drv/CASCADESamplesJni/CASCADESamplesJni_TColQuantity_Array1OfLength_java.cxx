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

#include <CASCADESamplesJni_TColQuantity_Array1OfLength.h>
#include <TColQuantity_Array1OfLength.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_TColQuantity_1Array1OfLength_1Create_11 (JNIEnv *env, jobject theobj, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* theret = new TColQuantity_Array1OfLength((Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_TColQuantity_1Array1OfLength_1Create_12 (JNIEnv *env, jobject theobj, jdouble Item, jint Low, jint Up)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* theret = new TColQuantity_Array1OfLength((Quantity_Length) Item,(Standard_Integer) Low,(Standard_Integer) Up);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Init (JNIEnv *env, jobject theobj, jdouble V)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
the_this->Init((Quantity_Length) V);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_IsAllocated (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_Other = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,Other);
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
const TColQuantity_Array1OfLength& theret = the_this->Assign(*the_Other);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Lower (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Upper (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_SetValue (JNIEnv *env, jobject theobj, jint Index, jdouble Value)
{

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) Index,(Quantity_Length) Value);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_Value (JNIEnv *env, jobject theobj, jint Index)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_ChangeValue (JNIEnv *env, jobject theobj, jint Index)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TColQuantity_Array1OfLength* the_this = (TColQuantity_Array1OfLength*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColQuantity_1Array1OfLength_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TColQuantity_Array1OfLength* theobj = (TColQuantity_Array1OfLength*) theid;
  delete theobj;
}
}


}
