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

#include <SampleHLRJni_Aspect_Window.h>
#include <Aspect_Window.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_Background.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Aspect_FillMethod.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Aspect_GraphicDevice.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1SetBackground_11 (JNIEnv *env, jobject theobj, jobject ABack)
{

jcas_Locking alock(env);
{
try {
Aspect_Background* the_ABack = (Aspect_Background*) jcas_GetHandle(env,ABack);
if ( the_ABack == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_ABack = new Aspect_Background ();
 // jcas_SetHandle ( env, ABack, the_ABack );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->SetBackground(*the_ABack);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1SetBackground_12 (JNIEnv *env, jobject theobj, jshort BackColor)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->SetBackground((Quantity_NameOfColor) BackColor);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1SetBackground_13 (JNIEnv *env, jobject theobj, jobject color)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_color = (Quantity_Color*) jcas_GetHandle(env,color);
if ( the_color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color = new Quantity_Color ();
 // jcas_SetHandle ( env, color, the_color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->SetBackground(*the_color);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1SetBackground_14 (JNIEnv *env, jobject theobj, jobject aName, jshort aMethod)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aName = jcas_ConvertToCString(env,aName);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetBackground(the_aName,(Aspect_FillMethod) aMethod);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_SetDoubleBuffer (JNIEnv *env, jobject theobj, jboolean DBmode)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->SetDoubleBuffer((Standard_Boolean) DBmode);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Flush (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Flush();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Map (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Map();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Unmap (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Unmap();

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



JNIEXPORT jshort JNICALL Java_SampleHLRJni_Aspect_1Window_DoResize (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DoResize();

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_DoMapping (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DoMapping();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Destroy();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Clear (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Clear();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_ClearArea (JNIEnv *env, jobject theobj, jint XCenter, jint YCenter, jint Width, jint Height)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->ClearArea((Standard_Integer) XCenter,(Standard_Integer) YCenter,(Standard_Integer) Width,(Standard_Integer) Height);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Restore (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Restore();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_RestoreArea (JNIEnv *env, jobject theobj, jint XCenter, jint YCenter, jint Width, jint Height)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->RestoreArea((Standard_Integer) XCenter,(Standard_Integer) YCenter,(Standard_Integer) Width,(Standard_Integer) Height);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_Dump (JNIEnv *env, jobject theobj, jobject aFilename, jdouble aGammaValue)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFilename = jcas_ConvertToCString(env,aFilename);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Dump(the_aFilename,(Standard_Real) aGammaValue);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_DumpArea (JNIEnv *env, jobject theobj, jobject aFilename, jint Xc, jint Yc, jint Width, jint Height, jdouble aGammaValue)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFilename = jcas_ConvertToCString(env,aFilename);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DumpArea(the_aFilename,(Standard_Integer) Xc,(Standard_Integer) Yc,(Standard_Integer) Width,(Standard_Integer) Height,(Standard_Real) aGammaValue);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_Load (JNIEnv *env, jobject theobj, jobject aFilename)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFilename = jcas_ConvertToCString(env,aFilename);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Load(the_aFilename);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_LoadArea (JNIEnv *env, jobject theobj, jobject aFilename, jint Xc, jint Yc, jint Width, jint Height)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFilename = jcas_ConvertToCString(env,aFilename);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LoadArea(the_aFilename,(Standard_Integer) Xc,(Standard_Integer) Yc,(Standard_Integer) Width,(Standard_Integer) Height);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Window_Background (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
Aspect_Background* theret = new Aspect_Background(the_this->Background());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_Background",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Window_BackgroundImage (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
Standard_CString sret = the_this->BackgroundImage();
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



JNIEXPORT jshort JNICALL Java_SampleHLRJni_Aspect_1Window_BackgroundFillMethod (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BackgroundFillMethod();

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Window_GraphicDevice (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
Handle(Aspect_GraphicDevice)* theret = new Handle(Aspect_GraphicDevice);
*theret = the_this->GraphicDevice();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_GraphicDevice",theret);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_IsMapped (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsMapped();

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



JNIEXPORT jdouble JNICALL Java_SampleHLRJni_Aspect_1Window_Ratio (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Ratio();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Position_11 (JNIEnv *env, jobject theobj, jobject X1, jobject Y1, jobject X2, jobject Y2)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_X1 = jcas_GetReal(env,X1);
Standard_Real the_Y1 = jcas_GetReal(env,Y1);
Standard_Real the_X2 = jcas_GetReal(env,X2);
Standard_Real the_Y2 = jcas_GetReal(env,Y2);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Position(the_X1,the_Y1,the_X2,the_Y2);
jcas_SetReal(env,X1,the_X1);
jcas_SetReal(env,Y1,the_Y1);
jcas_SetReal(env,X2,the_X2);
jcas_SetReal(env,Y2,the_Y2);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Position_12 (JNIEnv *env, jobject theobj, jobject X1, jobject Y1, jobject X2, jobject Y2)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_X1 = jcas_GetInteger(env,X1);
Standard_Integer the_Y1 = jcas_GetInteger(env,Y1);
Standard_Integer the_X2 = jcas_GetInteger(env,X2);
Standard_Integer the_Y2 = jcas_GetInteger(env,Y2);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Position(the_X1,the_Y1,the_X2,the_Y2);
jcas_SetInteger(env,X1,the_X1);
jcas_SetInteger(env,Y1,the_Y1);
jcas_SetInteger(env,X2,the_X2);
jcas_SetInteger(env,Y2,the_Y2);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Size_11 (JNIEnv *env, jobject theobj, jobject Width, jobject Height)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Width = jcas_GetReal(env,Width);
Standard_Real the_Height = jcas_GetReal(env,Height);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Size(the_Width,the_Height);
jcas_SetReal(env,Width,the_Width);
jcas_SetReal(env,Height,the_Height);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Size_12 (JNIEnv *env, jobject theobj, jobject Width, jobject Height)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_Width = jcas_GetInteger(env,Width);
Standard_Integer the_Height = jcas_GetInteger(env,Height);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Size(the_Width,the_Height);
jcas_SetInteger(env,Width,the_Width);
jcas_SetInteger(env,Height,the_Height);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_MMSize (JNIEnv *env, jobject theobj, jobject Width, jobject Height)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Width = jcas_GetReal(env,Width);
Standard_Real the_Height = jcas_GetReal(env,Height);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->MMSize(the_Width,the_Height);
jcas_SetReal(env,Width,the_Width);
jcas_SetReal(env,Height,the_Height);

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



JNIEXPORT jdouble JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Convert_11 (JNIEnv *env, jobject theobj, jint PV)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Standard_Integer) PV);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Convert_12 (JNIEnv *env, jobject theobj, jdouble DV)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Quantity_Parameter) DV);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Convert_13 (JNIEnv *env, jobject theobj, jint PX, jint PY, jobject DX, jobject DY)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_DX = jcas_GetReal(env,DX);
Standard_Real the_DY = jcas_GetReal(env,DY);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Convert((Standard_Integer) PX,(Standard_Integer) PY,the_DX,the_DY);
jcas_SetReal(env,DX,the_DX);
jcas_SetReal(env,DY,the_DY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Window_Aspect_1Window_1Convert_14 (JNIEnv *env, jobject theobj, jdouble DX, jdouble DY, jobject PX, jobject PY)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_PX = jcas_GetInteger(env,PX);
Standard_Integer the_PY = jcas_GetInteger(env,PY);
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
the_this->Convert((Quantity_Parameter) DX,(Quantity_Parameter) DY,the_PX,the_PY);
jcas_SetInteger(env,PX,the_PX);
jcas_SetInteger(env,PY,the_PY);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_BackingStore (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BackingStore();

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Window_DoubleBuffer (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Window) the_this = *((Handle(Aspect_Window)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DoubleBuffer();

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


}
