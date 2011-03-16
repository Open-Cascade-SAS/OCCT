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

#include <CASCADESamplesJni_SelectMgr_SelectableObject.h>
#include <SelectMgr_SelectableObject.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <Quantity_NameOfColor.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Prs3d_Presentation.hxx>


extern "C" {


JNIEXPORT jint JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_NbPossibleSelection (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->NbPossibleSelection();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_SelectMgr_1SelectableObject_1UpdateSelection_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->UpdateSelection();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_SelectMgr_1SelectableObject_1UpdateSelection_12 (JNIEnv *env, jobject theobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->UpdateSelection((Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_AddSelection (JNIEnv *env, jobject theobj, jobject aSelection, jint aMode)
{

jcas_Locking alock(env);
{
try {
 Handle( SelectMgr_Selection ) the_aSelection;
 void*                ptr_aSelection = jcas_GetHandle(env,aSelection);
 
 if ( ptr_aSelection != NULL ) the_aSelection = *(   (  Handle( SelectMgr_Selection )*  )ptr_aSelection   );

Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->AddSelection(the_aSelection,(Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_ClearSelections (JNIEnv *env, jobject theobj, jboolean update)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->ClearSelections((Standard_Boolean) update);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_Selection (JNIEnv *env, jobject theobj, jint aMode)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
Handle(SelectMgr_Selection)* theret = new Handle(SelectMgr_Selection);
*theret = the_this->Selection((Standard_Integer) aMode);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_Selection",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_HasSelection (JNIEnv *env, jobject theobj, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasSelection((Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_Init (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->Init();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_More (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->More();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_Next (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->Next();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_CurrentSelection (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
Handle(SelectMgr_Selection)* theret = new Handle(SelectMgr_Selection);
*theret = the_this->CurrentSelection();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_Selection",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_ResetLocation (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_SelectMgr_1SelectableObject_1UpdateLocation_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_HilightSelected (JNIEnv *env, jobject theobj, jobject PM, jobject Seq)
{

jcas_Locking alock(env);
{
try {
 Handle( PrsMgr_PresentationManager3d ) the_PM;
 void*                ptr_PM = jcas_GetHandle(env,PM);
 
 if ( ptr_PM != NULL ) the_PM = *(   (  Handle( PrsMgr_PresentationManager3d )*  )ptr_PM   );

SelectMgr_SequenceOfOwner* the_Seq = (SelectMgr_SequenceOfOwner*) jcas_GetHandle(env,Seq);
if ( the_Seq == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Seq = new SelectMgr_SequenceOfOwner ();
 // jcas_SetHandle ( env, Seq, the_Seq );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->HilightSelected(the_PM,*the_Seq);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_ClearSelected (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->ClearSelected();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_HilightOwnerWithColor (JNIEnv *env, jobject theobj, jobject thePM, jshort theColor, jobject theOwner)
{

jcas_Locking alock(env);
{
try {
 Handle( PrsMgr_PresentationManager3d ) the_thePM;
 void*                ptr_thePM = jcas_GetHandle(env,thePM);
 
 if ( ptr_thePM != NULL ) the_thePM = *(   (  Handle( PrsMgr_PresentationManager3d )*  )ptr_thePM   );

 Handle( SelectMgr_EntityOwner ) the_theOwner;
 void*                ptr_theOwner = jcas_GetHandle(env,theOwner);
 
 if ( ptr_theOwner != NULL ) the_theOwner = *(   (  Handle( SelectMgr_EntityOwner )*  )ptr_theOwner   );

Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->HilightOwnerWithColor(the_thePM,(Quantity_NameOfColor) theColor,the_theOwner);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_IsAutoHilight (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsAutoHilight();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_SetAutoHilight (JNIEnv *env, jobject theobj, jboolean newAutoHilight)
{

jcas_Locking alock(env);
{
try {
Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
the_this->SetAutoHilight((Standard_Boolean) newAutoHilight);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_GetHilightPresentation (JNIEnv *env, jobject theobj, jobject TheMgr)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( PrsMgr_PresentationManager3d ) the_TheMgr;
 void*                ptr_TheMgr = jcas_GetHandle(env,TheMgr);
 
 if ( ptr_TheMgr != NULL ) the_TheMgr = *(   (  Handle( PrsMgr_PresentationManager3d )*  )ptr_TheMgr   );

Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
Handle(Prs3d_Presentation)* theret = new Handle(Prs3d_Presentation);
*theret = the_this->GetHilightPresentation(the_TheMgr);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_SelectMgr_1SelectableObject_GetSelectPresentation (JNIEnv *env, jobject theobj, jobject TheMgr)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( PrsMgr_PresentationManager3d ) the_TheMgr;
 void*                ptr_TheMgr = jcas_GetHandle(env,TheMgr);
 
 if ( ptr_TheMgr != NULL ) the_TheMgr = *(   (  Handle( PrsMgr_PresentationManager3d )*  )ptr_TheMgr   );

Handle(SelectMgr_SelectableObject) the_this = *((Handle(SelectMgr_SelectableObject)*) jcas_GetHandle(env,theobj));
Handle(Prs3d_Presentation)* theret = new Handle(Prs3d_Presentation);
*theret = the_this->GetSelectPresentation(the_TheMgr);
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


}
