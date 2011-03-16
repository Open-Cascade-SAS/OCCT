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

#include <SampleHLRJni_Aspect_WindowDriver.h>
#include <Aspect_WindowDriver.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Aspect_TypeOfResize.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_TypeOfDrawMode.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_CString.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_BeginDraw (JNIEnv *env, jobject theobj, jboolean DoubleBuffer, jint aRetainBuffer)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->BeginDraw((Standard_Boolean) DoubleBuffer,(Standard_Integer) aRetainBuffer);

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



JNIEXPORT jshort JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_ResizeSpace (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ResizeSpace();

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_Window (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_Window)* theret = new Handle(Aspect_Window);
*theret = the_this->Window();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_Window",theret);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_SetDrawMode (JNIEnv *env, jobject theobj, jshort aMode)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->SetDrawMode((Aspect_TypeOfDrawMode) aMode);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_OpenBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jfloat aPivotX, jfloat aPivotY, jint aWidthIndex, jint aColorIndex, jint aFontIndex, jshort aDrawMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OpenBuffer((Standard_Integer) aRetainBuffer,(Standard_ShortReal) aPivotX,(Standard_ShortReal) aPivotY,(Standard_Integer) aWidthIndex,(Standard_Integer) aColorIndex,(Standard_Integer) aFontIndex,(Aspect_TypeOfDrawMode) aDrawMode);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_CloseBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->CloseBuffer((Standard_Integer) aRetainBuffer);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_ClearBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->ClearBuffer((Standard_Integer) aRetainBuffer);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_DrawBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->DrawBuffer((Standard_Integer) aRetainBuffer);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_EraseBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->EraseBuffer((Standard_Integer) aRetainBuffer);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_MoveBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jfloat aPivotX, jfloat aPivotY)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->MoveBuffer((Standard_Integer) aRetainBuffer,(Standard_ShortReal) aPivotX,(Standard_ShortReal) aPivotY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_ScaleBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jdouble aScaleX, jdouble aScaleY)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->ScaleBuffer((Standard_Integer) aRetainBuffer,(Quantity_Factor) aScaleX,(Quantity_Factor) aScaleY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_RotateBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->RotateBuffer((Standard_Integer) aRetainBuffer,(Quantity_PlaneAngle) anAngle);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_BufferIsOpen (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BufferIsOpen((Standard_Integer) aRetainBuffer);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_BufferIsEmpty (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BufferIsEmpty((Standard_Integer) aRetainBuffer);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_BufferIsDrawn (JNIEnv *env, jobject theobj, jint aRetainBuffer)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BufferIsDrawn((Standard_Integer) aRetainBuffer);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_AngleOfBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jobject anAngle)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_anAngle = jcas_GetReal(env,anAngle);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->AngleOfBuffer((Standard_Integer) aRetainBuffer,the_anAngle);
jcas_SetReal(env,anAngle,the_anAngle);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_ScaleOfBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jobject aScaleX, jobject aScaleY)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_aScaleX = jcas_GetReal(env,aScaleX);
Standard_Real the_aScaleY = jcas_GetReal(env,aScaleY);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->ScaleOfBuffer((Standard_Integer) aRetainBuffer,the_aScaleX,the_aScaleY);
jcas_SetReal(env,aScaleX,the_aScaleX);
jcas_SetReal(env,aScaleY,the_aScaleY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_PositionOfBuffer (JNIEnv *env, jobject theobj, jint aRetainBuffer, jobject aPivotX, jobject aPivotY)
{

jcas_Locking alock(env);
{
try {
Standard_ShortReal the_aPivotX = jcas_GetShortReal(env,aPivotX);
Standard_ShortReal the_aPivotY = jcas_GetShortReal(env,aPivotY);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->PositionOfBuffer((Standard_Integer) aRetainBuffer,the_aPivotX,the_aPivotY);
jcas_SetShortReal(env,aPivotX,the_aPivotX);
jcas_SetShortReal(env,aPivotY,the_aPivotY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_Aspect_1WindowDriver_1TextSize_11 (JNIEnv *env, jobject theobj, jobject aText, jobject aWidth, jobject aHeight, jint aFontIndex)
{

jcas_Locking alock(env);
{
try {
TCollection_ExtendedString* the_aText = (TCollection_ExtendedString*) jcas_GetHandle(env,aText);
if ( the_aText == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aText = new TCollection_ExtendedString ();
 // jcas_SetHandle ( env, aText, the_aText );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_ShortReal the_aWidth = jcas_GetShortReal(env,aWidth);
Standard_ShortReal the_aHeight = jcas_GetShortReal(env,aHeight);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->TextSize(*the_aText,the_aWidth,the_aHeight,(Standard_Integer) aFontIndex);
jcas_SetShortReal(env,aWidth,the_aWidth);
jcas_SetShortReal(env,aHeight,the_aHeight);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_Aspect_1WindowDriver_1TextSize_12 (JNIEnv *env, jobject theobj, jobject aText, jobject aWidth, jobject aHeight, jobject anXoffset, jobject anYoffset, jint aFontIndex)
{

jcas_Locking alock(env);
{
try {
TCollection_ExtendedString* the_aText = (TCollection_ExtendedString*) jcas_GetHandle(env,aText);
if ( the_aText == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aText = new TCollection_ExtendedString ();
 // jcas_SetHandle ( env, aText, the_aText );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_ShortReal the_aWidth = jcas_GetShortReal(env,aWidth);
Standard_ShortReal the_aHeight = jcas_GetShortReal(env,aHeight);
Standard_ShortReal the_anXoffset = jcas_GetShortReal(env,anXoffset);
Standard_ShortReal the_anYoffset = jcas_GetShortReal(env,anYoffset);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->TextSize(*the_aText,the_aWidth,the_aHeight,the_anXoffset,the_anYoffset,(Standard_Integer) aFontIndex);
jcas_SetShortReal(env,aWidth,the_aWidth);
jcas_SetShortReal(env,aHeight,the_aHeight);
jcas_SetShortReal(env,anXoffset,the_anXoffset);
jcas_SetShortReal(env,anYoffset,the_anYoffset);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_FontSize (JNIEnv *env, jobject theobj, jobject aSlant, jobject aSize, jobject aBheight, jint aFontIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_aSlant = jcas_GetReal(env,aSlant);
Standard_ShortReal the_aSize = jcas_GetShortReal(env,aSize);
Standard_ShortReal the_aBheight = jcas_GetShortReal(env,aBheight);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
Standard_CString sret = the_this->FontSize(the_aSlant,the_aSize,the_aBheight,(Standard_Integer) aFontIndex);
jclass CLSret  = env->FindClass("jcas/Standard_CString");
thejret = env->AllocObject(CLSret);
jcas_SetCStringValue(env,thejret,sret);
jcas_SetReal(env,aSlant,the_aSlant);
jcas_SetShortReal(env,aSize,the_aSize);
jcas_SetShortReal(env,aBheight,the_aBheight);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_ColorBoundIndexs (JNIEnv *env, jobject theobj, jobject aMinIndex, jobject aMaxIndex)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMinIndex = jcas_GetInteger(env,aMinIndex);
Standard_Integer the_aMaxIndex = jcas_GetInteger(env,aMaxIndex);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->ColorBoundIndexs(the_aMinIndex,the_aMaxIndex);
jcas_SetInteger(env,aMinIndex,the_aMinIndex);
jcas_SetInteger(env,aMaxIndex,the_aMaxIndex);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_LocalColorIndex (JNIEnv *env, jobject theobj, jint anIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LocalColorIndex((Standard_Integer) anIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_FontBoundIndexs (JNIEnv *env, jobject theobj, jobject aMinIndex, jobject aMaxIndex)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMinIndex = jcas_GetInteger(env,aMinIndex);
Standard_Integer the_aMaxIndex = jcas_GetInteger(env,aMaxIndex);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->FontBoundIndexs(the_aMinIndex,the_aMaxIndex);
jcas_SetInteger(env,aMinIndex,the_aMinIndex);
jcas_SetInteger(env,aMaxIndex,the_aMaxIndex);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_LocalFontIndex (JNIEnv *env, jobject theobj, jint anIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LocalFontIndex((Standard_Integer) anIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_TypeBoundIndexs (JNIEnv *env, jobject theobj, jobject aMinIndex, jobject aMaxIndex)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMinIndex = jcas_GetInteger(env,aMinIndex);
Standard_Integer the_aMaxIndex = jcas_GetInteger(env,aMaxIndex);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->TypeBoundIndexs(the_aMinIndex,the_aMaxIndex);
jcas_SetInteger(env,aMinIndex,the_aMinIndex);
jcas_SetInteger(env,aMaxIndex,the_aMaxIndex);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_LocalTypeIndex (JNIEnv *env, jobject theobj, jint anIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LocalTypeIndex((Standard_Integer) anIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_WidthBoundIndexs (JNIEnv *env, jobject theobj, jobject aMinIndex, jobject aMaxIndex)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMinIndex = jcas_GetInteger(env,aMinIndex);
Standard_Integer the_aMaxIndex = jcas_GetInteger(env,aMaxIndex);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->WidthBoundIndexs(the_aMinIndex,the_aMaxIndex);
jcas_SetInteger(env,aMinIndex,the_aMinIndex);
jcas_SetInteger(env,aMaxIndex,the_aMaxIndex);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_LocalWidthIndex (JNIEnv *env, jobject theobj, jint anIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LocalWidthIndex((Standard_Integer) anIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_MarkBoundIndexs (JNIEnv *env, jobject theobj, jobject aMinIndex, jobject aMaxIndex)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMinIndex = jcas_GetInteger(env,aMinIndex);
Standard_Integer the_aMaxIndex = jcas_GetInteger(env,aMaxIndex);
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
the_this->MarkBoundIndexs(the_aMinIndex,the_aMaxIndex);
jcas_SetInteger(env,aMinIndex,the_aMinIndex);
jcas_SetInteger(env,aMaxIndex,the_aMaxIndex);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1WindowDriver_LocalMarkIndex (JNIEnv *env, jobject theobj, jint anIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WindowDriver) the_this = *((Handle(Aspect_WindowDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->LocalMarkIndex((Standard_Integer) anIndex);

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
