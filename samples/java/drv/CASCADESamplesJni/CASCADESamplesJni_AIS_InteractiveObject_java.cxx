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

#include <CASCADESamplesJni_AIS_InteractiveObject.h>
#include <AIS_InteractiveObject.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <AIS_KindOfInteractive.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_ListOfTransient.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <AIS_Drawer.hxx>
#include <Aspect_TypeOfDegenerateModel.hxx>
#include <Geom_Transformation.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_BasicAspect.hxx>


extern "C" {


JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Type (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Type();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Signature (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Signature();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AcceptShapeDecomposition (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AcceptShapeDecomposition();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetCurrentFacingModel (JNIEnv *env, jobject theobj, jshort aModel)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetCurrentFacingModel((Aspect_TypeOfFacingModel) aModel);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_CurrentFacingModel (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->CurrentFacingModel();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1SetColor_11 (JNIEnv *env, jobject theobj, jobject aColor)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetColor(*the_aColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1SetColor_12 (JNIEnv *env, jobject theobj, jshort aColor)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetColor((Quantity_NameOfColor) aColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetColor (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetColor();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetWidth (JNIEnv *env, jobject theobj, jdouble aValue)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetWidth((Standard_Real) aValue);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetWidth (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetWidth();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AcceptDisplayMode (JNIEnv *env, jobject theobj, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AcceptDisplayMode((Standard_Integer) aMode);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_DefaultDisplayMode (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultDisplayMode();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Redisplay (JNIEnv *env, jobject theobj, jboolean AllModes)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->Redisplay((Standard_Boolean) AllModes);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetInfiniteState (JNIEnv *env, jobject theobj, jboolean aFlag)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetInfiniteState((Standard_Boolean) aFlag);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_IsInfinite (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsInfinite();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasInteractiveContext (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasInteractiveContext();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_GetContext (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveContext)* theret = new Handle(AIS_InteractiveContext);
*theret = the_this->GetContext();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveContext",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetContext (JNIEnv *env, jobject theobj, jobject aCtx)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aCtx;
 void*                ptr_aCtx = jcas_GetHandle(env,aCtx);
 
 if ( ptr_aCtx != NULL ) the_aCtx = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aCtx   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetContext(the_aCtx);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasOwner (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasOwner();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_GetOwner (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
Handle(Standard_Transient)* theret = new Handle(Standard_Transient);
*theret = the_this->GetOwner();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Standard_Transient",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetOwner (JNIEnv *env, jobject theobj, jobject ApplicativeEntity)
{

jcas_Locking alock(env);
{
try {
 Handle( Standard_Transient ) the_ApplicativeEntity;
 void*                ptr_ApplicativeEntity = jcas_GetHandle(env,ApplicativeEntity);
 
 if ( ptr_ApplicativeEntity != NULL ) the_ApplicativeEntity = *(   (  Handle( Standard_Transient )*  )ptr_ApplicativeEntity   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetOwner(the_ApplicativeEntity);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_ClearOwner (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->ClearOwner();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasUsers (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasUsers();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Users (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
const TColStd_ListOfTransient& theret = the_this->Users();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TColStd_ListOfTransient",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AddUser (JNIEnv *env, jobject theobj, jobject aUser)
{

jcas_Locking alock(env);
{
try {
 Handle( Standard_Transient ) the_aUser;
 void*                ptr_aUser = jcas_GetHandle(env,aUser);
 
 if ( ptr_aUser != NULL ) the_aUser = *(   (  Handle( Standard_Transient )*  )ptr_aUser   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->AddUser(the_aUser);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_ClearUsers (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->ClearUsers();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasDisplayMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasDisplayMode();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetDisplayMode (JNIEnv *env, jobject theobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetDisplayMode((Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetDisplayMode (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetDisplayMode();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_DisplayMode (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DisplayMode();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasSelectionMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasSelectionMode();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SelectionMode (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SelectionMode();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetSelectionMode (JNIEnv *env, jobject theobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetSelectionMode((Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetSelectionMode (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetSelectionMode();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SelectionPriority (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SelectionPriority();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetSelectionPriority (JNIEnv *env, jobject theobj, jint aPriority)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetSelectionPriority((Standard_Integer) aPriority);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetSelectionPriority (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetSelectionPriority();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasSelectionPriority (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasSelectionPriority();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasHilightMode (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasHilightMode();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HilightMode (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HilightMode();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetHilightMode (JNIEnv *env, jobject theobj, jint anIndex)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetHilightMode((Standard_Integer) anIndex);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetHilightMode (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetHilightMode();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasColor (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasColor();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1Color_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Color();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1Color_12 (JNIEnv *env, jobject theobj, jobject aColor)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->Color(*the_aColor);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasWidth (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasWidth();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Width (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Width();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasMaterial (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasMaterial();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Material (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Material();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1SetMaterial_11 (JNIEnv *env, jobject theobj, jshort aName)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetMaterial((Graphic3d_NameOfMaterial) aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1SetMaterial_12 (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Graphic3d_MaterialAspect* the_aName = (Graphic3d_MaterialAspect*) jcas_GetHandle(env,aName);
if ( the_aName == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aName = new Graphic3d_MaterialAspect ();
 // jcas_SetHandle ( env, aName, the_aName );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetMaterial(*the_aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetMaterial (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetMaterial();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetTransparency (JNIEnv *env, jobject theobj, jdouble aValue)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetTransparency((Standard_Real) aValue);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_IsTransparent (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsTransparent();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Transparency (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Transparency();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetTransparency (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetTransparency();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetAttributes (JNIEnv *env, jobject theobj, jobject aDrawer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_Drawer ) the_aDrawer;
 void*                ptr_aDrawer = jcas_GetHandle(env,aDrawer);
 
 if ( ptr_aDrawer != NULL ) the_aDrawer = *(   (  Handle( AIS_Drawer )*  )ptr_aDrawer   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetAttributes(the_aDrawer);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Attributes (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
Handle(AIS_Drawer)* theret = new Handle(AIS_Drawer);
*theret = the_this->Attributes();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_Drawer",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetAttributes (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetAttributes();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1State_11 (JNIEnv *env, jobject theobj, jint theState)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->State((Standard_Integer) theState);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_AIS_1InteractiveObject_1State_12 (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->State();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetDegenerateModel (JNIEnv *env, jobject theobj, jshort aModel, jdouble aRatio)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetDegenerateModel((Aspect_TypeOfDegenerateModel) aModel,(Quantity_Ratio) aRatio);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_DegenerateModel (JNIEnv *env, jobject theobj, jobject aRatio)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_aRatio = jcas_GetReal(env,aRatio);
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DegenerateModel(the_aRatio);
jcas_SetReal(env,aRatio,the_aRatio);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetTransformation (JNIEnv *env, jobject theobj, jobject aTranformation, jboolean postConcatenate, jboolean updateSelection)
{

jcas_Locking alock(env);
{
try {
 Handle( Geom_Transformation ) the_aTranformation;
 void*                ptr_aTranformation = jcas_GetHandle(env,aTranformation);
 
 if ( ptr_aTranformation != NULL ) the_aTranformation = *(   (  Handle( Geom_Transformation )*  )ptr_aTranformation   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetTransformation(the_aTranformation,(Standard_Boolean) postConcatenate,(Standard_Boolean) updateSelection);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_UnsetTransformation (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->UnsetTransformation();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Transformation (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
Handle(Geom_Transformation)* theret = new Handle(Geom_Transformation);
*theret = the_this->Transformation();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Geom_Transformation",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasTransformation (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasTransformation();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasPresentation (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasPresentation();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_Presentation (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
Handle(Prs3d_Presentation)* theret = new Handle(Prs3d_Presentation);
*theret = the_this->Presentation();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Prs3d_Presentation",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetAspect (JNIEnv *env, jobject theobj, jobject anAspect, jboolean globalChange)
{

jcas_Locking alock(env);
{
try {
 Handle( Prs3d_BasicAspect ) the_anAspect;
 void*                ptr_anAspect = jcas_GetHandle(env,anAspect);
 
 if ( ptr_anAspect != NULL ) the_anAspect = *(   (  Handle( Prs3d_BasicAspect )*  )ptr_anAspect   );

Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetAspect(the_anAspect,(Standard_Boolean) globalChange);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_SetPolygonOffsets (JNIEnv *env, jobject theobj, jint aMode, jdouble aFactor, jdouble aUnits)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->SetPolygonOffsets((Standard_Integer) aMode,(Standard_Real) aFactor,(Standard_Real) aUnits);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_HasPolygonOffsets (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasPolygonOffsets();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveObject_PolygonOffsets (JNIEnv *env, jobject theobj, jobject aMode, jobject aFactor, jobject aUnits)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_aMode = jcas_GetInteger(env,aMode);
Standard_Real the_aFactor = jcas_GetReal(env,aFactor);
Standard_Real the_aUnits = jcas_GetReal(env,aUnits);
Handle(AIS_InteractiveObject) the_this = *((Handle(AIS_InteractiveObject)*) jcas_GetHandle(env,theobj));
the_this->PolygonOffsets(the_aMode,the_aFactor,the_aUnits);
jcas_SetInteger(env,aMode,the_aMode);
jcas_SetReal(env,aFactor,the_aFactor);
jcas_SetReal(env,aUnits,the_aUnits);

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


}
