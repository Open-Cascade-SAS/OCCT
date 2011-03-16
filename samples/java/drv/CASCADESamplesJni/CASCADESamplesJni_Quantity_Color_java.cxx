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

#include <CASCADESamplesJni_Quantity_Color.h>
#include <Quantity_Color.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* theret = new Quantity_Color();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1Create_12 (JNIEnv *env, jobject theobj, jshort AName)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* theret = new Quantity_Color((Quantity_NameOfColor) AName);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1Create_13 (JNIEnv *env, jobject theobj, jdouble R1, jdouble R2, jdouble R3, jshort AType)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* theret = new Quantity_Color((Quantity_Parameter) R1,(Quantity_Parameter) R2,(Quantity_Parameter) R3,(Quantity_TypeOfColor) AType);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Quantity_1Color_Assign (JNIEnv *env, jobject theobj, jobject Other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_Other = (Quantity_Color*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Quantity_Color ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
const Quantity_Color& theret = the_this->Assign(*the_Other);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Quantity_Color",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_ChangeContrast (JNIEnv *env, jobject theobj, jdouble ADelta)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->ChangeContrast((Quantity_Rate) ADelta);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_ChangeIntensity (JNIEnv *env, jobject theobj, jdouble ADelta)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->ChangeIntensity((Quantity_Rate) ADelta);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1SetValues_11 (JNIEnv *env, jobject theobj, jshort AName)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->SetValues((Quantity_NameOfColor) AName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1SetValues_12 (JNIEnv *env, jobject theobj, jdouble R1, jdouble R2, jdouble R3, jshort AType)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->SetValues((Quantity_Parameter) R1,(Quantity_Parameter) R2,(Quantity_Parameter) R3,(Quantity_TypeOfColor) AType);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Delta (JNIEnv *env, jobject theobj, jobject AColor, jobject DC, jobject DI)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_AColor = (Quantity_Color*) jcas_GetHandle(env,AColor);
if ( the_AColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AColor = new Quantity_Color ();
 // jcas_SetHandle ( env, AColor, the_AColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_Real the_DC = jcas_GetReal(env,DC);
Standard_Real the_DI = jcas_GetReal(env,DI);
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->Delta(*the_AColor,the_DC,the_DI);
jcas_SetReal(env,DC,the_DC);
jcas_SetReal(env,DI,the_DI);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Distance (JNIEnv *env, jobject theobj, jobject AColor)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_AColor = (Quantity_Color*) jcas_GetHandle(env,AColor);
if ( the_AColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AColor = new Quantity_Color ();
 // jcas_SetHandle ( env, AColor, the_AColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Distance(*the_AColor);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_SquareDistance (JNIEnv *env, jobject theobj, jobject AColor)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_AColor = (Quantity_Color*) jcas_GetHandle(env,AColor);
if ( the_AColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AColor = new Quantity_Color ();
 // jcas_SetHandle ( env, AColor, the_AColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->SquareDistance(*the_AColor);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Blue (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Blue();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Green (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Green();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Hue (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Hue();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Quantity_1Color_IsDifferent (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_Other = (Quantity_Color*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Quantity_Color ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsDifferent(*the_Other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Quantity_1Color_IsEqual (JNIEnv *env, jobject theobj, jobject Other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_Other = (Quantity_Color*) jcas_GetHandle(env,Other);
if ( the_Other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Other = new Quantity_Color ();
 // jcas_SetHandle ( env, Other, the_Other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Light (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Light();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1Name_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Name();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Red (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Red();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Saturation (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
 thejret = the_this->Saturation();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Values (JNIEnv *env, jobject theobj, jobject R1, jobject R2, jobject R3, jshort AType)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_R1 = jcas_GetReal(env,R1);
Standard_Real the_R2 = jcas_GetReal(env,R2);
Standard_Real the_R3 = jcas_GetReal(env,R3);
Quantity_Color* the_this = (Quantity_Color*) jcas_GetHandle(env,theobj);
the_this->Values(the_R1,the_R2,the_R3,(Quantity_TypeOfColor) AType);
jcas_SetReal(env,R1,the_R1);
jcas_SetReal(env,R2,the_R2);
jcas_SetReal(env,R3,the_R3);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_SetEpsilon (JNIEnv *env, jclass, jdouble AnEpsilon)
{

jcas_Locking alock(env);
{
try {
Quantity_Color::SetEpsilon((Quantity_Parameter) AnEpsilon);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_Quantity_1Color_Epsilon (JNIEnv *env, jclass)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
 thejret = Quantity_Color::Epsilon();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_Quantity_1Color_Quantity_1Color_1Name_12 (JNIEnv *env, jclass, jdouble R, jdouble G, jdouble B)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 thejret = Quantity_Color::Name((Quantity_Parameter) R,(Quantity_Parameter) G,(Quantity_Parameter) B);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Quantity_1Color_StringName (JNIEnv *env, jclass, jshort AColor)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_CString sret = Quantity_Color::StringName((Quantity_NameOfColor) AColor);
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_HlsRgb (JNIEnv *env, jclass, jdouble H, jdouble L, jdouble S, jobject R, jobject G, jobject B)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_R = jcas_GetReal(env,R);
Standard_Real the_G = jcas_GetReal(env,G);
Standard_Real the_B = jcas_GetReal(env,B);
Quantity_Color::HlsRgb((Quantity_Parameter) H,(Quantity_Parameter) L,(Quantity_Parameter) S,the_R,the_G,the_B);
jcas_SetReal(env,R,the_R);
jcas_SetReal(env,G,the_G);
jcas_SetReal(env,B,the_B);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_RgbHls (JNIEnv *env, jclass, jdouble R, jdouble G, jdouble B, jobject H, jobject L, jobject S)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_H = jcas_GetReal(env,H);
Standard_Real the_L = jcas_GetReal(env,L);
Standard_Real the_S = jcas_GetReal(env,S);
Quantity_Color::RgbHls((Quantity_Parameter) R,(Quantity_Parameter) G,(Quantity_Parameter) B,the_H,the_L,the_S);
jcas_SetReal(env,H,the_H);
jcas_SetReal(env,L,the_L);
jcas_SetReal(env,S,the_S);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_Test (JNIEnv *env, jclass)
{

jcas_Locking alock(env);
{
try {
Quantity_Color::Test();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Quantity_1Color_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Quantity_Color* theobj = (Quantity_Color*) theid;
  delete theobj;
}
}


}
