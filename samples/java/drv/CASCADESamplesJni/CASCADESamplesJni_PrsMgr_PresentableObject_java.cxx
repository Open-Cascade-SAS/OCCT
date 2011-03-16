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

#include <CASCADESamplesJni_PrsMgr_PresentableObject.h>
#include <PrsMgr_PresentableObject.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopLoc_Location.hxx>
#include <Standard_Boolean.hxx>
#include <Prs3d_Presentation.hxx>


extern "C" {


JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_TypeOfPresentation3d (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->TypeOfPresentation3d();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_GetTransformPersistencePoint (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
gp_Pnt* theret = new gp_Pnt(the_this->GetTransformPersistencePoint());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/gp_Pnt",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_SetTypeOfPresentation (JNIEnv *env, jobject theobj, jshort aType)
{

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->SetTypeOfPresentation((PrsMgr_TypeOfPresentation3d) aType);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_PrsMgr_1PresentableObject_1SetToUpdate_11 (JNIEnv *env, jobject theobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->SetToUpdate((Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_PrsMgr_1PresentableObject_1SetToUpdate_12 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->SetToUpdate();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_ToBeUpdated (JNIEnv *env, jobject theobj, jobject ListOfMode)
{

jcas_Locking alock(env);
{
try {
TColStd_ListOfInteger* the_ListOfMode = (TColStd_ListOfInteger*) jcas_GetHandle(env,ListOfMode);
if ( the_ListOfMode == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_ListOfMode = new TColStd_ListOfInteger ();
 // jcas_SetHandle ( env, ListOfMode, the_ListOfMode );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->ToBeUpdated(*the_ListOfMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_SetLocation (JNIEnv *env, jobject theobj, jobject aLoc)
{

jcas_Locking alock(env);
{
try {
TopLoc_Location* the_aLoc = (TopLoc_Location*) jcas_GetHandle(env,aLoc);
if ( the_aLoc == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aLoc = new TopLoc_Location ();
 // jcas_SetHandle ( env, aLoc, the_aLoc );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->SetLocation(*the_aLoc);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_HasLocation (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasLocation();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_Location (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
const TopLoc_Location& theret = the_this->Location();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopLoc_Location",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_ResetLocation (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->ResetLocation();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_PrsMgr_1PresentableObject_1UpdateLocation_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->UpdateLocation();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_PrsMgr_1PresentableObject_PrsMgr_1PresentableObject_1UpdateLocation_12 (JNIEnv *env, jobject theobj, jobject P)
{

jcas_Locking alock(env);
{
try {
 Handle( Prs3d_Presentation ) the_P;
 void*                ptr_P = jcas_GetHandle(env,P);
 
 if ( ptr_P != NULL ) the_P = *(   (  Handle( Prs3d_Presentation )*  )ptr_P   );

Handle(PrsMgr_PresentableObject) the_this = *((Handle(PrsMgr_PresentableObject)*) jcas_GetHandle(env,theobj));
the_this->UpdateLocation(the_P);

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
