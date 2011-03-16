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

#include <SampleHLRJni_Aspect_Driver.h>
#include <Aspect_Driver.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Boolean.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_MarkMap.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_ShortReal.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Aspect_TypeOfText.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_EndDraw (JNIEnv *env, jobject theobj, jboolean Synchronize)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->EndDraw((Standard_Boolean) Synchronize);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetColorMap (JNIEnv *env, jobject theobj, jobject aColorMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_ColorMap ) the_aColorMap;
 void*                ptr_aColorMap = jcas_GetHandle(env,aColorMap);
 
 if ( ptr_aColorMap != NULL ) the_aColorMap = *(   (  Handle( Aspect_ColorMap )*  )ptr_aColorMap   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetColorMap(the_aColorMap);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetTypeMap (JNIEnv *env, jobject theobj, jobject aTypeMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_TypeMap ) the_aTypeMap;
 void*                ptr_aTypeMap = jcas_GetHandle(env,aTypeMap);
 
 if ( ptr_aTypeMap != NULL ) the_aTypeMap = *(   (  Handle( Aspect_TypeMap )*  )ptr_aTypeMap   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetTypeMap(the_aTypeMap);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetWidthMap (JNIEnv *env, jobject theobj, jobject aWidthMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_WidthMap ) the_aWidthMap;
 void*                ptr_aWidthMap = jcas_GetHandle(env,aWidthMap);
 
 if ( ptr_aWidthMap != NULL ) the_aWidthMap = *(   (  Handle( Aspect_WidthMap )*  )ptr_aWidthMap   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetWidthMap(the_aWidthMap);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetFontMap (JNIEnv *env, jobject theobj, jobject aFontMap, jboolean useMFT)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_FontMap ) the_aFontMap;
 void*                ptr_aFontMap = jcas_GetHandle(env,aFontMap);
 
 if ( ptr_aFontMap != NULL ) the_aFontMap = *(   (  Handle( Aspect_FontMap )*  )ptr_aFontMap   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetFontMap(the_aFontMap,(Standard_Boolean) useMFT);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetMarkMap (JNIEnv *env, jobject theobj, jobject aMarkMap)
{

jcas_Locking alock(env);
{
try {
 Handle( Aspect_MarkMap ) the_aMarkMap;
 void*                ptr_aMarkMap = jcas_GetHandle(env,aMarkMap);
 
 if ( ptr_aMarkMap != NULL ) the_aMarkMap = *(   (  Handle( Aspect_MarkMap )*  )ptr_aMarkMap   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetMarkMap(the_aMarkMap);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetLineAttrib (JNIEnv *env, jobject theobj, jint ColorIndex, jint TypeIndex, jint WidthIndex)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetLineAttrib((Standard_Integer) ColorIndex,(Standard_Integer) TypeIndex,(Standard_Integer) WidthIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1SetTextAttrib_11 (JNIEnv *env, jobject theobj, jint ColorIndex, jint FontIndex)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetTextAttrib((Standard_Integer) ColorIndex,(Standard_Integer) FontIndex);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1SetTextAttrib_12 (JNIEnv *env, jobject theobj, jint ColorIndex, jint FontIndex, jdouble aSlant, jdouble aHScale, jdouble aWScale, jboolean isUnderlined)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetTextAttrib((Standard_Integer) ColorIndex,(Standard_Integer) FontIndex,(Quantity_PlaneAngle) aSlant,(Quantity_Factor) aHScale,(Quantity_Factor) aWScale,(Standard_Boolean) isUnderlined);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetPolyAttrib (JNIEnv *env, jobject theobj, jint ColorIndex, jint TileIndex, jboolean DrawEdge)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetPolyAttrib((Standard_Integer) ColorIndex,(Standard_Integer) TileIndex,(Standard_Boolean) DrawEdge);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_SetMarkerAttrib (JNIEnv *env, jobject theobj, jint ColorIndex, jint WidthIndex, jboolean FillMarker)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->SetMarkerAttrib((Standard_Integer) ColorIndex,(Standard_Integer) WidthIndex,(Standard_Boolean) FillMarker);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Driver_IsKnownImage (JNIEnv *env, jobject theobj, jobject anImage)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( Standard_Transient ) the_anImage;
 void*                ptr_anImage = jcas_GetHandle(env,anImage);
 
 if ( ptr_anImage != NULL ) the_anImage = *(   (  Handle( Standard_Transient )*  )ptr_anImage   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsKnownImage(the_anImage);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Driver_SizeOfImageFile (JNIEnv *env, jobject theobj, jobject anImageFile, jobject aWidth, jobject aHeight)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_anImageFile = jcas_ConvertToCString(env,anImageFile);
Standard_Integer the_aWidth = jcas_GetInteger(env,aWidth);
Standard_Integer the_aHeight = jcas_GetInteger(env,aHeight);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SizeOfImageFile(the_anImageFile,the_aWidth,the_aHeight);
jcas_SetInteger(env,aWidth,the_aWidth);
jcas_SetInteger(env,aHeight,the_aHeight);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_ClearImage (JNIEnv *env, jobject theobj, jobject anImageId)
{

jcas_Locking alock(env);
{
try {
 Handle( Standard_Transient ) the_anImageId;
 void*                ptr_anImageId = jcas_GetHandle(env,anImageId);
 
 if ( ptr_anImageId != NULL ) the_anImageId = *(   (  Handle( Standard_Transient )*  )ptr_anImageId   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->ClearImage(the_anImageId);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_ClearImageFile (JNIEnv *env, jobject theobj, jobject anImageFile)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_anImageFile = jcas_ConvertToCString(env,anImageFile);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->ClearImageFile(the_anImageFile);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawImage (JNIEnv *env, jobject theobj, jobject anImageId, jfloat aX, jfloat aY)
{

jcas_Locking alock(env);
{
try {
 Handle( Standard_Transient ) the_anImageId;
 void*                ptr_anImageId = jcas_GetHandle(env,anImageId);
 
 if ( ptr_anImageId != NULL ) the_anImageId = *(   (  Handle( Standard_Transient )*  )ptr_anImageId   );

Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawImage(the_anImageId,(Standard_ShortReal) aX,(Standard_ShortReal) aY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawImageFile (JNIEnv *env, jobject theobj, jobject anImageFile, jfloat aX, jfloat aY, jdouble aScale)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_anImageFile = jcas_ConvertToCString(env,anImageFile);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawImageFile(the_anImageFile,(Standard_ShortReal) aX,(Standard_ShortReal) aY,(Quantity_Factor) aScale);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawPolyline (JNIEnv *env, jobject theobj, jobject aListX, jobject aListY)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_aListX = (TShort_Array1OfShortReal*) jcas_GetHandle(env,aListX);
if ( the_aListX == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListX = new TShort_Array1OfShortReal ();
 // jcas_SetHandle ( env, aListX, the_aListX );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TShort_Array1OfShortReal* the_aListY = (TShort_Array1OfShortReal*) jcas_GetHandle(env,aListY);
if ( the_aListY == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListY = new TShort_Array1OfShortReal ();
 // jcas_SetHandle ( env, aListY, the_aListY );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawPolyline(*the_aListX,*the_aListY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawPolygon (JNIEnv *env, jobject theobj, jobject aListX, jobject aListY)
{

jcas_Locking alock(env);
{
try {
TShort_Array1OfShortReal* the_aListX = (TShort_Array1OfShortReal*) jcas_GetHandle(env,aListX);
if ( the_aListX == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListX = new TShort_Array1OfShortReal ();
 // jcas_SetHandle ( env, aListX, the_aListX );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TShort_Array1OfShortReal* the_aListY = (TShort_Array1OfShortReal*) jcas_GetHandle(env,aListY);
if ( the_aListY == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListY = new TShort_Array1OfShortReal ();
 // jcas_SetHandle ( env, aListY, the_aListY );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawPolygon(*the_aListX,*the_aListY);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawSegment (JNIEnv *env, jobject theobj, jfloat X1, jfloat Y1, jfloat X2, jfloat Y2)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawSegment((Standard_ShortReal) X1,(Standard_ShortReal) Y1,(Standard_ShortReal) X2,(Standard_ShortReal) Y2);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawText (JNIEnv *env, jobject theobj, jobject aText, jfloat Xpos, jfloat Ypos, jfloat anAngle, jshort aType)
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
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawText(*the_aText,(Standard_ShortReal) Xpos,(Standard_ShortReal) Ypos,(Standard_ShortReal) anAngle,(Aspect_TypeOfText) aType);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawPolyText (JNIEnv *env, jobject theobj, jobject aText, jfloat Xpos, jfloat Ypos, jdouble aMarge, jfloat anAngle, jshort aType)
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
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawPolyText(*the_aText,(Standard_ShortReal) Xpos,(Standard_ShortReal) Ypos,(Quantity_Ratio) aMarge,(Standard_ShortReal) anAngle,(Aspect_TypeOfText) aType);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawPoint (JNIEnv *env, jobject theobj, jfloat X, jfloat Y)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawPoint((Standard_ShortReal) X,(Standard_ShortReal) Y);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawMarker (JNIEnv *env, jobject theobj, jint aMarker, jfloat Xpos, jfloat Ypos, jfloat Width, jfloat Height, jfloat Angle)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->DrawMarker((Standard_Integer) aMarker,(Standard_ShortReal) Xpos,(Standard_ShortReal) Ypos,(Standard_ShortReal) Width,(Standard_ShortReal) Height,(Standard_ShortReal) Angle);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawArc (JNIEnv *env, jobject theobj, jfloat X, jfloat Y, jfloat anXradius, jfloat anYradius, jfloat aStartAngle, jfloat anOpenAngle)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DrawArc((Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) anXradius,(Standard_ShortReal) anYradius,(Standard_ShortReal) aStartAngle,(Standard_ShortReal) anOpenAngle);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Driver_DrawPolyArc (JNIEnv *env, jobject theobj, jfloat X, jfloat Y, jfloat anXradius, jfloat anYradius, jfloat aStartAngle, jfloat anOpenAngle)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DrawPolyArc((Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) anXradius,(Standard_ShortReal) anYradius,(Standard_ShortReal) aStartAngle,(Standard_ShortReal) anOpenAngle);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginPolyline (JNIEnv *env, jobject theobj, jint aNumber)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolyline((Standard_Integer) aNumber);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginPolygon (JNIEnv *env, jobject theobj, jint aNumber)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolygon((Standard_Integer) aNumber);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginSegments (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginSegments();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginArcs (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginArcs();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginPolyArcs (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolyArcs();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginMarkers (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginMarkers();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_BeginPoints (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->BeginPoints();

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_ClosePrimitive (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->ClosePrimitive();

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Driver_ColorMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_ColorMap)* theret = new Handle(Aspect_ColorMap);
*theret = the_this->ColorMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Driver_TypeMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_TypeMap)* theret = new Handle(Aspect_TypeMap);
*theret = the_this->TypeMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_TypeMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Driver_WidthMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_WidthMap)* theret = new Handle(Aspect_WidthMap);
*theret = the_this->WidthMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_WidthMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Driver_FontMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_FontMap)* theret = new Handle(Aspect_FontMap);
*theret = the_this->FontMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_FontMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Driver_MarkMap (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
Handle(Aspect_MarkMap)* theret = new Handle(Aspect_MarkMap);
*theret = the_this->MarkMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_MarkMap",theret);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_WorkSpace (JNIEnv *env, jobject theobj, jobject Width, jobject Heigth)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_Width = jcas_GetReal(env,Width);
Standard_Real the_Heigth = jcas_GetReal(env,Heigth);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->WorkSpace(the_Width,the_Heigth);
jcas_SetReal(env,Width,the_Width);
jcas_SetReal(env,Heigth,the_Heigth);

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



JNIEXPORT jdouble JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1Convert_11 (JNIEnv *env, jobject theobj, jint PV)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1Convert_12 (JNIEnv *env, jobject theobj, jdouble DV)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Convert((Quantity_Length) DV);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1Convert_13 (JNIEnv *env, jobject theobj, jint PX, jint PY, jobject DX, jobject DY)
{

jcas_Locking alock(env);
{
try {
Standard_Real the_DX = jcas_GetReal(env,DX);
Standard_Real the_DY = jcas_GetReal(env,DY);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Driver_Aspect_1Driver_1Convert_14 (JNIEnv *env, jobject theobj, jdouble DX, jdouble DY, jobject PX, jobject PY)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_PX = jcas_GetInteger(env,PX);
Standard_Integer the_PY = jcas_GetInteger(env,PY);
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
the_this->Convert((Quantity_Length) DX,(Quantity_Length) DY,the_PX,the_PY);
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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_Aspect_1Driver_UseMFT (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_Driver) the_this = *((Handle(Aspect_Driver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->UseMFT();

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
