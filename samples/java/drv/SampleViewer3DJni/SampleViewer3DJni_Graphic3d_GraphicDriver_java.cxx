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

#include <SampleViewer3DJni_Graphic3d_GraphicDriver.h>
#include <Graphic3d_GraphicDriver.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <AlienImage_AlienImage.hxx>
#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <OSD_SharedLibrary.hxx>
#include <OSD_Function.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Create_10 (JNIEnv *env, jobject theobj, jobject AShrName)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AShrName = jcas_ConvertToCString(env,AShrName);
Handle(Graphic3d_GraphicDriver)* theret = new Handle(Graphic3d_GraphicDriver);
  OSD_SharedLibrary TheSharedLibrary(the_AShrName);
  Standard_Boolean Result = TheSharedLibrary.DlOpen (OSD_RTLD_LAZY);
  OSD_Function new_GLGraphicDriver = TheSharedLibrary.DlSymb ("MetaGraphicDriverFactory"); 
  Handle(Graphic3d_GraphicDriver) (*fp) (Standard_CString) = NULL;
  fp = (Handle(Graphic3d_GraphicDriver) (*) (Standard_CString)) new_GLGraphicDriver;
  if ( ! new_GLGraphicDriver) {
    Aspect_GraphicDeviceDefinitionError::Raise( TheSharedLibrary.DlError () );
   }
  Standard_CString aSharedName = TheSharedLibrary.Name ();
  *theret = (*fp) (aSharedName);
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



JNIEXPORT jboolean JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Begin_11 (JNIEnv *env, jobject theobj, jobject ADisplay)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_ADisplay = jcas_ConvertToCString(env,ADisplay);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Begin(the_ADisplay);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_End (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->End();

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



JNIEXPORT jint JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_InquireLightLimit (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->InquireLightLimit();

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



JNIEXPORT jint JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_InquirePlaneLimit (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->InquirePlaneLimit();

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



JNIEXPORT jint JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_InquireViewLimit (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->InquireViewLimit();

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



JNIEXPORT jboolean JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_InquireTextureAvailable (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->InquireTextureAvailable();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_InitPick (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->InitPick();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_ZBufferTriedronSetup (JNIEnv *env, jobject theobj, jshort XColor, jshort YColor, jshort ZColor, jdouble SizeRatio, jdouble AxisDiametr, jint NbFacettes)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->ZBufferTriedronSetup((Quantity_NameOfColor) XColor,(Quantity_NameOfColor) YColor,(Quantity_NameOfColor) ZColor,(Standard_Real) SizeRatio,(Standard_Real) AxisDiametr,(Standard_Integer) NbFacettes);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndAddMode (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndAddMode();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_BeginPolyline (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolyline();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Draw_11 (JNIEnv *env, jobject theobj, jfloat X, jfloat Y, jfloat Z)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Draw((Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) Z);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndImmediatMode (JNIEnv *env, jobject theobj, jint Synchronize)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndImmediatMode((Standard_Integer) Synchronize);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndPolyline (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndPolyline();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Move_11 (JNIEnv *env, jobject theobj, jfloat X, jfloat Y, jfloat Z)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Move((Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) Z);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetLineColor (JNIEnv *env, jobject theobj, jfloat R, jfloat G, jfloat B)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetLineColor((Standard_ShortReal) R,(Standard_ShortReal) G,(Standard_ShortReal) B);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetLineType (JNIEnv *env, jobject theobj, jint Type)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetLineType((Standard_Integer) Type);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetLineWidth (JNIEnv *env, jobject theobj, jfloat Width)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetLineWidth((Standard_ShortReal) Width);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetMinMax (JNIEnv *env, jobject theobj, jfloat X1, jfloat Y1, jfloat Z1, jfloat X2, jfloat Y2, jfloat Z2)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetMinMax((Standard_ShortReal) X1,(Standard_ShortReal) Y1,(Standard_ShortReal) Z1,(Standard_ShortReal) X2,(Standard_ShortReal) Y2,(Standard_ShortReal) Z2);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Transform (JNIEnv *env, jobject theobj, jobject AMatrix, jshort AType)
{

jcas_Locking alock(env);
{
try {
TColStd_Array2OfReal* the_AMatrix = (TColStd_Array2OfReal*) jcas_GetHandle(env,AMatrix);
if ( the_AMatrix == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AMatrix = new TColStd_Array2OfReal ();
 // jcas_SetHandle ( env, AMatrix, the_AMatrix );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Transform(*the_AMatrix,(Graphic3d_TypeOfComposition) AType);

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



JNIEXPORT jint JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_CreateTexture (JNIEnv *env, jobject theobj, jshort Type, jobject Image, jobject FileName, jobject TexUpperBounds)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 Handle( AlienImage_AlienImage ) the_Image;
 void*                ptr_Image = jcas_GetHandle(env,Image);
 
 if ( ptr_Image != NULL ) the_Image = *(   (  Handle( AlienImage_AlienImage )*  )ptr_Image   );

Standard_CString the_FileName = jcas_ConvertToCString(env,FileName);
 Handle( TColStd_HArray1OfReal ) the_TexUpperBounds;
 void*                ptr_TexUpperBounds = jcas_GetHandle(env,TexUpperBounds);
 
 if ( ptr_TexUpperBounds != NULL ) the_TexUpperBounds = *(   (  Handle( TColStd_HArray1OfReal )*  )ptr_TexUpperBounds   );

Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->CreateTexture((Graphic3d_TypeOfTexture) Type,the_Image,the_FileName,the_TexUpperBounds);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_DestroyTexture (JNIEnv *env, jobject theobj, jint TexId)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->DestroyTexture((Standard_Integer) TexId);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_BeginPolygon2d (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolygon2d();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_BeginPolyline2d (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->BeginPolyline2d();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Draw_12 (JNIEnv *env, jobject theobj, jfloat X, jfloat Y)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Draw((Standard_ShortReal) X,(Standard_ShortReal) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Edge (JNIEnv *env, jobject theobj, jfloat X, jfloat Y)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Edge((Standard_ShortReal) X,(Standard_ShortReal) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndLayer (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndLayer();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndPolygon2d (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndPolygon2d();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_EndPolyline2d (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->EndPolyline2d();

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Move_12 (JNIEnv *env, jobject theobj, jfloat X, jfloat Y)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Move((Standard_ShortReal) X,(Standard_ShortReal) Y);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Rectangle (JNIEnv *env, jobject theobj, jfloat X, jfloat Y, jfloat Width, jfloat Height)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Rectangle((Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) Width,(Standard_ShortReal) Height);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetColor (JNIEnv *env, jobject theobj, jfloat R, jfloat G, jfloat B)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetColor((Standard_ShortReal) R,(Standard_ShortReal) G,(Standard_ShortReal) B);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetLineAttributes (JNIEnv *env, jobject theobj, jint Type, jfloat Width)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetLineAttributes((Standard_Integer) Type,(Standard_ShortReal) Width);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetTextAttributes (JNIEnv *env, jobject theobj, jchar* Font, jint Type, jfloat R, jfloat G, jfloat B)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetTextAttributes((Standard_CString) Font,(Standard_Integer) Type,(Standard_ShortReal) R,(Standard_ShortReal) G,(Standard_ShortReal) B);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Graphic3d_1GraphicDriver_1Text_15 (JNIEnv *env, jobject theobj, jobject AText, jfloat X, jfloat Y, jfloat AHeight)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AText = jcas_ConvertToCString(env,AText);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->Text(the_AText,(Standard_ShortReal) X,(Standard_ShortReal) Y,(Standard_ShortReal) AHeight);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_TextSize (JNIEnv *env, jobject theobj, jobject AText, jfloat AHeight, jobject AWidth, jobject AnAscent, jobject ADescent)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AText = jcas_ConvertToCString(env,AText);
Standard_ShortReal the_AWidth = jcas_GetShortReal(env,AWidth);
Standard_ShortReal the_AnAscent = jcas_GetShortReal(env,AnAscent);
Standard_ShortReal the_ADescent = jcas_GetShortReal(env,ADescent);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->TextSize(the_AText,(Standard_ShortReal) AHeight,the_AWidth,the_AnAscent,the_ADescent);
jcas_SetShortReal(env,AWidth,the_AWidth);
jcas_SetShortReal(env,AnAscent,the_AnAscent);
jcas_SetShortReal(env,ADescent,the_ADescent);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintBoolean (JNIEnv *env, jobject theobj, jobject AComment, jboolean AValue)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AComment = jcas_ConvertToCString(env,AComment);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintBoolean(the_AComment,(Standard_Boolean) AValue);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintFunction (JNIEnv *env, jobject theobj, jobject AFunc)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AFunc = jcas_ConvertToCString(env,AFunc);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintFunction(the_AFunc);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintInteger (JNIEnv *env, jobject theobj, jobject AComment, jint AValue)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AComment = jcas_ConvertToCString(env,AComment);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintInteger(the_AComment,(Standard_Integer) AValue);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintIResult (JNIEnv *env, jobject theobj, jobject AFunc, jint AResult)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AFunc = jcas_ConvertToCString(env,AFunc);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintIResult(the_AFunc,(Standard_Integer) AResult);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintShortReal (JNIEnv *env, jobject theobj, jobject AComment, jfloat AValue)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AComment = jcas_ConvertToCString(env,AComment);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintShortReal(the_AComment,(Standard_ShortReal) AValue);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintMatrix (JNIEnv *env, jobject theobj, jobject AComment, jobject AMatrix)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AComment = jcas_ConvertToCString(env,AComment);
TColStd_Array2OfReal* the_AMatrix = (TColStd_Array2OfReal*) jcas_GetHandle(env,AMatrix);
if ( the_AMatrix == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AMatrix = new TColStd_Array2OfReal ();
 // jcas_SetHandle ( env, AMatrix, the_AMatrix );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintMatrix(the_AComment,*the_AMatrix);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_PrintString (JNIEnv *env, jobject theobj, jobject AComment, jobject AString)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_AComment = jcas_ConvertToCString(env,AComment);
Standard_CString the_AString = jcas_ConvertToCString(env,AString);
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->PrintString(the_AComment,the_AString);

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



JNIEXPORT void JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_SetTrace (JNIEnv *env, jobject theobj, jint ALevel)
{

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
the_this->SetTrace((Standard_Integer) ALevel);

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



JNIEXPORT jint JNICALL Java_SampleViewer3DJni_Graphic3d_1GraphicDriver_Trace (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Graphic3d_GraphicDriver) the_this = *((Handle(Graphic3d_GraphicDriver)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Trace();

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
