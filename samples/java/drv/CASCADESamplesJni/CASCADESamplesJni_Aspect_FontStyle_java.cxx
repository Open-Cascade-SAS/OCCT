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

#include <CASCADESamplesJni_Aspect_FontStyle.h>
#include <Aspect_FontStyle.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_TypeOfFont.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* theret = new Aspect_FontStyle();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1Create_12 (JNIEnv *env, jobject theobj, jint Type, jdouble Size, jdouble Slant, jboolean CapsHeight)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* theret = new Aspect_FontStyle((Aspect_TypeOfFont) Type,(Quantity_Length) Size,(Quantity_PlaneAngle) Slant,(Standard_Boolean) CapsHeight);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1Create_13 (JNIEnv *env, jobject theobj, jobject Style, jdouble Size, jdouble Slant, jboolean CapsHeight)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_Style = jcas_ConvertToCString(env,Style);
Aspect_FontStyle* theret = new Aspect_FontStyle(the_Style,(Quantity_Length) Size,(Quantity_PlaneAngle) Slant,(Standard_Boolean) CapsHeight);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1Create_14 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_Style = jcas_ConvertToCString(env,Style);
Aspect_FontStyle* theret = new Aspect_FontStyle(the_Style);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_Other = (Aspect_FontStyle*) jcas_GetHandle(env,Other);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
const Aspect_FontStyle& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_FontStyle",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1SetValues_11 (JNIEnv *env, jobject theobj, jint Type, jdouble Size, jdouble Slant, jboolean CapsHeight)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues((Aspect_TypeOfFont) Type,(Quantity_Length) Size,(Quantity_PlaneAngle) Slant,(Standard_Boolean) CapsHeight);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1SetValues_12 (JNIEnv *env, jobject theobj, jobject Style, jdouble Size, jdouble Slant, jboolean CapsHeight)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_Style = jcas_ConvertToCString(env,Style);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues(the_Style,(Quantity_Length) Size,(Quantity_PlaneAngle) Slant,(Standard_Boolean) CapsHeight);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Aspect_1FontStyle_1SetValues_13 (JNIEnv *env, jobject theobj, jobject Style)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_Style = jcas_ConvertToCString(env,Style);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetValues(the_Style);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SetFamily (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aName = jcas_ConvertToCString(env,aName);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetFamily(the_aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SetWeight (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aName = jcas_ConvertToCString(env,aName);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetWeight(the_aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SetRegistry (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aName = jcas_ConvertToCString(env,aName);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetRegistry(the_aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SetEncoding (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_aName = jcas_ConvertToCString(env,aName);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
the_this->SetEncoding(the_aName);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Style (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Value (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Value();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Size (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->Size();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Slant (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->Slant();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_CapsHeight (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
 thejret = the_this->CapsHeight();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_AliasName (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->AliasName();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_FullName (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->FullName();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Foundry (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Foundry();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Family (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Family();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Weight (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Weight();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Registry (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Registry();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Encoding (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->Encoding();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SSlant (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SSlant();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SWidth (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SWidth();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SStyle (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SStyle();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SPixelSize (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SPixelSize();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SPointSize (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SPointSize();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SResolutionX (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SResolutionX();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SResolutionY (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SResolutionY();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SSpacing (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SSpacing();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_SAverageWidth (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->SAverageWidth();
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_Dump (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_Other = (Aspect_FontStyle*) jcas_GetHandle(env,Other);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_IsNotEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Aspect_FontStyle* the_Other = (Aspect_FontStyle*) jcas_GetHandle(env,Other);
Aspect_FontStyle* the_this = (Aspect_FontStyle*) jcas_GetHandle(env,theobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1FontStyle_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_FontStyle* theobj = (Aspect_FontStyle*) theid;
  delete theobj;
}
}


}
