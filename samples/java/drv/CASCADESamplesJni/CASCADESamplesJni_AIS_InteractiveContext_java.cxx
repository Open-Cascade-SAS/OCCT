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

#include <CASCADESamplesJni_AIS_InteractiveContext.h>
#include <AIS_InteractiveContext.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_Boolean.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_NameOfColor.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Standard_Real.hxx>
#include <TopLoc_Location.hxx>
#include <Aspect_TypeOfFacingModel.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Aspect_TypeOfDegenerateModel.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_DisplayStatus.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TCollection_ExtendedString.hxx>
#include <AIS_DisplayMode.hxx>
#include <Prs3d_LineAspect.hxx>
#include <AIS_TypeOfIso.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <V3d_View.hxx>
#include <AIS_StatusOfPick.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Transient.hxx>
#include <AIS_ClearMode.hxx>
#include <Geom_Transformation.hxx>
#include <Prs3d_Drawer.hxx>
#include <SelectMgr_Filter.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <StdSelect_ViewerSelector3d.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Create_11 (JNIEnv *env, jobject theobj, jobject MainViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_MainViewer;
 void*                ptr_MainViewer = jcas_GetHandle(env,MainViewer);
 
 if ( ptr_MainViewer != NULL ) the_MainViewer = *(   (  Handle( V3d_Viewer )*  )ptr_MainViewer   );

Handle(AIS_InteractiveContext)* theret = new Handle(AIS_InteractiveContext);
*theret = new AIS_InteractiveContext(the_MainViewer);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Create_12 (JNIEnv *env, jobject theobj, jobject MainViewer, jobject Collector)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_MainViewer;
 void*                ptr_MainViewer = jcas_GetHandle(env,MainViewer);
 
 if ( ptr_MainViewer != NULL ) the_MainViewer = *(   (  Handle( V3d_Viewer )*  )ptr_MainViewer   );

 Handle( V3d_Viewer ) the_Collector;
 void*                ptr_Collector = jcas_GetHandle(env,Collector);
 
 if ( ptr_Collector != NULL ) the_Collector = *(   (  Handle( V3d_Viewer )*  )ptr_Collector   );

Handle(AIS_InteractiveContext)* theret = new Handle(AIS_InteractiveContext);
*theret = new AIS_InteractiveContext(the_MainViewer,the_Collector);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Delete (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Delete();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsCollectorClosed (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsCollectorClosed();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CloseCollector (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->CloseCollector();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_OpenCollector (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->OpenCollector();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetAutoActivateSelection (JNIEnv *env, jobject theobj, jboolean Auto)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetAutoActivateSelection((Standard_Boolean) Auto);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_GetAutoActivateSelection (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->GetAutoActivateSelection();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Display_11 (JNIEnv *env, jobject theobj, jobject anIobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Display(the_anIobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Display_12 (JNIEnv *env, jobject theobj, jobject anIobj, jint amode, jint aSelectionMode, jboolean updateviewer, jboolean allowdecomposition)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Display(the_anIobj,(Standard_Integer) amode,(Standard_Integer) aSelectionMode,(Standard_Boolean) updateviewer,(Standard_Boolean) allowdecomposition);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Load (JNIEnv *env, jobject theobj, jobject aniobj, jint SelectionMode, jboolean AllowDecomp)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Load(the_aniobj,(Standard_Integer) SelectionMode,(Standard_Boolean) AllowDecomp);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Erase (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer, jboolean PutInCollector)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Erase(the_aniobj,(Standard_Boolean) updateviewer,(Standard_Boolean) PutInCollector);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_EraseMode (JNIEnv *env, jobject theobj, jobject aniobj, jint aMode, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->EraseMode(the_aniobj,(Standard_Integer) aMode,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_EraseAll (JNIEnv *env, jobject theobj, jboolean PutInCollector, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->EraseAll((Standard_Boolean) PutInCollector,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayAll (JNIEnv *env, jobject theobj, jboolean OnlyFromCollector, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayAll((Standard_Boolean) OnlyFromCollector,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayFromCollector (JNIEnv *env, jobject theobj, jobject anIObj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayFromCollector(the_anIObj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_EraseSelected (JNIEnv *env, jobject theobj, jboolean PutInCollector, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->EraseSelected((Standard_Boolean) PutInCollector,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplaySelected (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplaySelected((Standard_Boolean) updateviewer);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_KeepTemporary (JNIEnv *env, jobject theobj, jobject anIObj, jint InWhichLocal)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->KeepTemporary(the_anIObj,(Standard_Integer) InWhichLocal);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Clear (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Clear(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearPrs (JNIEnv *env, jobject theobj, jobject aniobj, jint aMode, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearPrs(the_aniobj,(Standard_Integer) aMode,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Remove (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Remove(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_RemoveAll (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->RemoveAll((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Hilight (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Hilight(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightWithColor (JNIEnv *env, jobject theobj, jobject aniobj, jshort aCol, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->HilightWithColor(the_aniobj,(Quantity_NameOfColor) aCol,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Unhilight (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Unhilight(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetDisplayPriority (JNIEnv *env, jobject theobj, jobject anIobj, jint aPriority)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDisplayPriority(the_anIobj,(Standard_Integer) aPriority);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Redisplay_11 (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer, jboolean allmodes)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Redisplay(the_aniobj,(Standard_Boolean) updateviewer,(Standard_Boolean) allmodes);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Redisplay_12 (JNIEnv *env, jobject theobj, jshort aTypeOfObject, jint Signature, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Redisplay((AIS_KindOfInteractive) aTypeOfObject,(Standard_Integer) Signature,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_RecomputePrsOnly (JNIEnv *env, jobject theobj, jobject anIobj, jboolean updateviewer, jboolean allmodes)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->RecomputePrsOnly(the_anIobj,(Standard_Boolean) updateviewer,(Standard_Boolean) allmodes);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_RecomputeSelectionOnly (JNIEnv *env, jobject theobj, jobject anIObj)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->RecomputeSelectionOnly(the_anIObj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Update (JNIEnv *env, jobject theobj, jobject anIobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Update(the_anIobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDisplayMode_11 (JNIEnv *env, jobject theobj, jobject aniobj, jint aMode, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDisplayMode(the_aniobj,(Standard_Integer) aMode,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetDisplayMode (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetDisplayMode(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetSelectionMode (JNIEnv *env, jobject theobj, jobject aniobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSelectionMode(the_aniobj,(Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetSelectionMode (JNIEnv *env, jobject theobj, jobject aniobj)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetSelectionMode(the_aniobj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetSensitivity_11 (JNIEnv *env, jobject theobj, jdouble aPrecision)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSensitivity((Standard_Real) aPrecision);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetSensitivity_12 (JNIEnv *env, jobject theobj, jint aPrecision)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSensitivity((Standard_Integer) aPrecision);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetLocation (JNIEnv *env, jobject theobj, jobject aniobj, jobject aLocation)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

TopLoc_Location* the_aLocation = (TopLoc_Location*) jcas_GetHandle(env,aLocation);
if ( the_aLocation == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aLocation = new TopLoc_Location ();
 // jcas_SetHandle ( env, aLocation, the_aLocation );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetLocation(the_aniobj,*the_aLocation);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ResetLocation (JNIEnv *env, jobject theobj, jobject aniobj)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ResetLocation(the_aniobj);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasLocation (JNIEnv *env, jobject theobj, jobject aniobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasLocation(the_aniobj);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Location (JNIEnv *env, jobject theobj, jobject aniobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TopLoc_Location& theret = the_this->Location(the_aniobj);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetCurrentFacingModel (JNIEnv *env, jobject theobj, jobject aniobj, jshort aModel)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetCurrentFacingModel(the_aniobj,(Aspect_TypeOfFacingModel) aModel);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetColor_11 (JNIEnv *env, jobject theobj, jobject aniobj, jshort aColor, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetColor(the_aniobj,(Quantity_NameOfColor) aColor,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetColor_12 (JNIEnv *env, jobject theobj, jobject aniobj, jobject aColor, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetColor(the_aniobj,*the_aColor,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetColor (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetColor(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetWidth (JNIEnv *env, jobject theobj, jobject aniobj, jdouble aValue, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetWidth(the_aniobj,(Standard_Real) aValue,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetWidth (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetWidth(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetMaterial (JNIEnv *env, jobject theobj, jobject aniobj, jshort aName, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetMaterial(the_aniobj,(Graphic3d_NameOfMaterial) aName,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetMaterial (JNIEnv *env, jobject theobj, jobject anObj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetMaterial(the_anObj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetTransparency (JNIEnv *env, jobject theobj, jobject aniobj, jdouble aValue, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetTransparency(the_aniobj,(Standard_Real) aValue,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetTransparency (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetTransparency(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDegenerateModel_11 (JNIEnv *env, jobject theobj, jobject aniobj, jshort aModel, jdouble aRatio)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDegenerateModel(the_aniobj,(Aspect_TypeOfDegenerateModel) aModel,(Quantity_Ratio) aRatio);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDegenerateModel_12 (JNIEnv *env, jobject theobj, jshort aModel, jdouble aSkipRatio)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDegenerateModel((Aspect_TypeOfDegenerateModel) aModel,(Quantity_Ratio) aSkipRatio);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetLocalAttributes (JNIEnv *env, jobject theobj, jobject aniobj, jobject aDrawer, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

 Handle( AIS_Drawer ) the_aDrawer;
 void*                ptr_aDrawer = jcas_GetHandle(env,aDrawer);
 
 if ( ptr_aDrawer != NULL ) the_aDrawer = *(   (  Handle( AIS_Drawer )*  )ptr_aDrawer   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetLocalAttributes(the_aniobj,the_aDrawer,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnsetLocalAttributes (JNIEnv *env, jobject theobj, jobject anObj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnsetLocalAttributes(the_anObj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetPolygonOffsets (JNIEnv *env, jobject theobj, jobject anObj, jint aMode, jdouble aFactor, jdouble aUnits, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetPolygonOffsets(the_anObj,(Standard_Integer) aMode,(Standard_Real) aFactor,(Standard_Real) aUnits,(Standard_Boolean) updateviewer);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasPolygonOffsets (JNIEnv *env, jobject theobj, jobject anObj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasPolygonOffsets(the_anObj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_PolygonOffsets (JNIEnv *env, jobject theobj, jobject anObj, jobject aMode, jobject aFactor, jobject aUnits)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

Standard_Integer the_aMode = jcas_GetInteger(env,aMode);
Standard_Real the_aFactor = jcas_GetReal(env,aFactor);
Standard_Real the_aUnits = jcas_GetReal(env,aUnits);
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->PolygonOffsets(the_anObj,the_aMode,the_aFactor,the_aUnits);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetTrihedronSize (JNIEnv *env, jobject theobj, jdouble aSize, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetTrihedronSize((Standard_Real) aSize,(Standard_Boolean) updateviewer);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_TrihedronSize (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->TrihedronSize();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetPlaneSize_11 (JNIEnv *env, jobject theobj, jdouble aSizeX, jdouble aSizeY, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneSize((Standard_Real) aSizeX,(Standard_Real) aSizeY,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetPlaneSize_12 (JNIEnv *env, jobject theobj, jdouble aSize, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetPlaneSize((Standard_Real) aSize,(Standard_Boolean) updateviewer);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_PlaneSize (JNIEnv *env, jobject theobj, jobject XSize, jobject YSize)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_XSize = jcas_GetReal(env,XSize);
Standard_Real the_YSize = jcas_GetReal(env,YSize);
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->PlaneSize(the_XSize,the_YSize);
jcas_SetReal(env,XSize,the_XSize);
jcas_SetReal(env,YSize,the_YSize);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayStatus (JNIEnv *env, jobject theobj, jobject anIobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DisplayStatus(the_anIobj);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayedModes (JNIEnv *env, jobject theobj, jobject aniobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TColStd_ListOfInteger& theret = the_this->DisplayedModes(the_aniobj);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TColStd_ListOfInteger",&theret,0);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsDisplayed_11 (JNIEnv *env, jobject theobj, jobject anIobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsDisplayed(the_anIobj);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsDisplayed_12 (JNIEnv *env, jobject theobj, jobject aniobj, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsDisplayed(the_aniobj,(Standard_Integer) aMode);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsHilighted_11 (JNIEnv *env, jobject theobj, jobject aniobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsHilighted(the_aniobj);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsHilighted_12 (JNIEnv *env, jobject theobj, jobject anIobj, jobject WithColor, jobject theHiCol)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Standard_Boolean the_WithColor = jcas_GetBoolean(env,WithColor);
Quantity_NameOfColor the_theHiCol = ( Quantity_NameOfColor )jcas_GetShort ( env, theHiCol );
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsHilighted(the_anIobj,the_WithColor,the_theHiCol);
jcas_SetBoolean(env,WithColor,the_WithColor);
jcas_SetShort(env,theHiCol,the_theHiCol);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsInCollector (JNIEnv *env, jobject theobj, jobject anIObj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsInCollector(the_anIObj);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayPriority (JNIEnv *env, jobject theobj, jobject anIobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DisplayPriority(the_anIobj);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasColor (JNIEnv *env, jobject theobj, jobject aniobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasColor(the_aniobj);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Color_11 (JNIEnv *env, jobject theobj, jobject aniobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Color(the_aniobj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Color_12 (JNIEnv *env, jobject theobj, jobject aniobj, jobject acolor)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Quantity_Color* the_acolor = (Quantity_Color*) jcas_GetHandle(env,acolor);
if ( the_acolor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_acolor = new Quantity_Color ();
 // jcas_SetHandle ( env, acolor, the_acolor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Color(the_aniobj,*the_acolor);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Width (JNIEnv *env, jobject theobj, jobject aniobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Width(the_aniobj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Status (JNIEnv *env, jobject theobj, jobject anObj, jobject astatus)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

TCollection_ExtendedString* the_astatus = (TCollection_ExtendedString*) jcas_GetHandle(env,astatus);
if ( the_astatus == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astatus = new TCollection_ExtendedString ();
 // jcas_SetHandle ( env, astatus, the_astatus );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Status(the_anObj,*the_astatus);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UpdateCurrentViewer (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UpdateCurrentViewer();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UpdateCollector (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UpdateCollector();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisplayMode (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightColor (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HilightColor();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SelectionColor_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SelectionColor();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_PreSelectionColor (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->PreSelectionColor();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DefaultColor (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DefaultColor();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SubIntensityColor (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SubIntensityColor();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetHilightColor (JNIEnv *env, jobject theobj, jshort aHiCol)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHilightColor((Quantity_NameOfColor) aHiCol);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SelectionColor_12 (JNIEnv *env, jobject theobj, jshort aCol)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SelectionColor((Quantity_NameOfColor) aCol);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetPreselectionColor (JNIEnv *env, jobject theobj, jshort aCol)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetPreselectionColor((Quantity_NameOfColor) aCol);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetSubIntensityColor (JNIEnv *env, jobject theobj, jshort aCol)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSubIntensityColor((Quantity_NameOfColor) aCol);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDisplayMode_12 (JNIEnv *env, jobject theobj, jshort AMode, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDisplayMode((AIS_DisplayMode) AMode,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDeviationCoefficient_11 (JNIEnv *env, jobject theobj, jobject aniobj, jdouble aCoefficient, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDeviationCoefficient(the_aniobj,(Standard_Real) aCoefficient,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDeviationAngle_11 (JNIEnv *env, jobject theobj, jobject aniobj, jdouble anAngle, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDeviationAngle(the_aniobj,(Standard_Real) anAngle,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetAngleAndDeviation (JNIEnv *env, jobject theobj, jobject aniobj, jdouble anAngle, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetAngleAndDeviation(the_aniobj,(Standard_Real) anAngle,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetHLRDeviationCoefficient_11 (JNIEnv *env, jobject theobj, jobject aniobj, jdouble aCoefficient, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRDeviationCoefficient(the_aniobj,(Standard_Real) aCoefficient,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetHLRDeviationAngle (JNIEnv *env, jobject theobj, jobject aniobj, jdouble anAngle, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRDeviationAngle(the_aniobj,(Standard_Real) anAngle,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetHLRAngleAndDeviation_11 (JNIEnv *env, jobject theobj, jobject aniobj, jdouble anAngle, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRAngleAndDeviation(the_aniobj,(Standard_Real) anAngle,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDeviationCoefficient_12 (JNIEnv *env, jobject theobj, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDeviationCoefficient((Standard_Real) aCoefficient);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DeviationCoefficient (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DeviationCoefficient();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetDeviationAngle_12 (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetDeviationAngle((Standard_Real) anAngle);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DeviationAngle (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DeviationAngle();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetHLRDeviationCoefficient_12 (JNIEnv *env, jobject theobj, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRDeviationCoefficient((Standard_Real) aCoefficient);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HLRDeviationCoefficient (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HLRDeviationCoefficient();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetHLRAngle (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRAngle((Standard_Real) anAngle);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HLRAngle (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HLRAngle();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SetHLRAngleAndDeviation_12 (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHLRAngleAndDeviation((Standard_Real) anAngle);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HiddenLineAspect (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(Prs3d_LineAspect)* theret = new Handle(Prs3d_LineAspect);
*theret = the_this->HiddenLineAspect();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Prs3d_LineAspect",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetHiddenLineAspect (JNIEnv *env, jobject theobj, jobject anAspect)
{

jcas_Locking alock(env);
{
try {
 Handle( Prs3d_LineAspect ) the_anAspect;
 void*                ptr_anAspect = jcas_GetHandle(env,anAspect);
 
 if ( ptr_anAspect != NULL ) the_anAspect = *(   (  Handle( Prs3d_LineAspect )*  )ptr_anAspect   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetHiddenLineAspect(the_anAspect);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DrawHiddenLine (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->DrawHiddenLine();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_EnableDrawHiddenLine (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->EnableDrawHiddenLine();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DisableDrawHiddenLine (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisableDrawHiddenLine();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetIsoNumber (JNIEnv *env, jobject theobj, jint NbIsos, jshort WhichIsos)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetIsoNumber((Standard_Integer) NbIsos,(AIS_TypeOfIso) WhichIsos);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsoNumber (JNIEnv *env, jobject theobj, jshort WhichIsos)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsoNumber((AIS_TypeOfIso) WhichIsos);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsoOnPlane_11 (JNIEnv *env, jobject theobj, jboolean SwitchOn)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->IsoOnPlane((Standard_Boolean) SwitchOn);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1IsoOnPlane_12 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsoOnPlane();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetSelectedAspect (JNIEnv *env, jobject theobj, jobject anAspect, jboolean globalChange, jboolean updateViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( Prs3d_BasicAspect ) the_anAspect;
 void*                ptr_anAspect = jcas_GetHandle(env,anAspect);
 
 if ( ptr_anAspect != NULL ) the_anAspect = *(   (  Handle( Prs3d_BasicAspect )*  )ptr_anAspect   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSelectedAspect(the_anAspect,(Standard_Boolean) globalChange,(Standard_Boolean) updateViewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MoveTo (JNIEnv *env, jobject theobj, jint XPix, jint YPix, jobject aView)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoveTo((Standard_Integer) XPix,(Standard_Integer) YPix,the_aView);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasNextDetected (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasNextDetected();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightNextDetected (JNIEnv *env, jobject theobj, jobject aView)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HilightNextDetected(the_aView);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightPreviousDetected (JNIEnv *env, jobject theobj, jobject aView)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HilightPreviousDetected(the_aView);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Select_11 (JNIEnv *env, jobject theobj, jint XPMin, jint YPMin, jint XPMax, jint YPMax, jobject aView, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Select((Standard_Integer) XPMin,(Standard_Integer) YPMin,(Standard_Integer) XPMax,(Standard_Integer) YPMax,the_aView,(Standard_Boolean) updateviewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Select_12 (JNIEnv *env, jobject theobj, jobject Polyline, jobject aView, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
TColgp_Array1OfPnt2d* the_Polyline = (TColgp_Array1OfPnt2d*) jcas_GetHandle(env,Polyline);
if ( the_Polyline == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Polyline = new TColgp_Array1OfPnt2d ();
 // jcas_SetHandle ( env, Polyline, the_Polyline );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Select(*the_Polyline,the_aView,(Standard_Boolean) updateviewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Select_13 (JNIEnv *env, jobject theobj, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Select((Standard_Boolean) updateviewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1ShiftSelect_11 (JNIEnv *env, jobject theobj, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ShiftSelect((Standard_Boolean) updateviewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1ShiftSelect_12 (JNIEnv *env, jobject theobj, jobject Polyline, jobject aView, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
TColgp_Array1OfPnt2d* the_Polyline = (TColgp_Array1OfPnt2d*) jcas_GetHandle(env,Polyline);
if ( the_Polyline == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Polyline = new TColgp_Array1OfPnt2d ();
 // jcas_SetHandle ( env, Polyline, the_Polyline );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ShiftSelect(*the_Polyline,the_aView,(Standard_Boolean) updateviewer);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1ShiftSelect_13 (JNIEnv *env, jobject theobj, jint XPMin, jint YPMin, jint XPMax, jint YPMax, jobject aView, jboolean updateviewer)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ShiftSelect((Standard_Integer) XPMin,(Standard_Integer) YPMin,(Standard_Integer) XPMax,(Standard_Integer) YPMax,the_aView,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetToHilightSelected (JNIEnv *env, jobject theobj, jboolean toHilight)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetToHilightSelected((Standard_Boolean) toHilight);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ToHilightSelected (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ToHilightSelected();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetCurrentObject (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetCurrentObject(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AddOrRemoveCurrentObject (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->AddOrRemoveCurrentObject(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UpdateCurrent (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UpdateCurrent();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_WasCurrentTouched (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->WasCurrentTouched();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetOkCurrent (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetOkCurrent();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsCurrent (JNIEnv *env, jobject theobj, jobject aniobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsCurrent(the_aniobj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_InitCurrent (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->InitCurrent();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MoreCurrent (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreCurrent();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NextCurrent (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->NextCurrent();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Current (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->Current();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NbCurrents (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->NbCurrents();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_FirstCurrentObject (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->FirstCurrentObject();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightCurrents (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->HilightCurrents((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnhilightCurrents (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnhilightCurrents((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearCurrents (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearCurrents((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetSelected (JNIEnv *env, jobject theobj, jobject aniObj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniObj;
 void*                ptr_aniObj = jcas_GetHandle(env,aniObj);
 
 if ( ptr_aniObj != NULL ) the_aniObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSelected(the_aniObj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetSelectedCurrent (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetSelectedCurrent();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UpdateSelected (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UpdateSelected((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1AddOrRemoveSelected_11 (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->AddOrRemoveSelected(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HilightSelected (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->HilightSelected((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UnhilightSelected (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UnhilightSelected((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearSelected (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearSelected((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1AddOrRemoveSelected_12 (JNIEnv *env, jobject theobj, jobject aShape, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_aShape = (TopoDS_Shape*) jcas_GetHandle(env,aShape);
if ( the_aShape == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aShape = new TopoDS_Shape ();
 // jcas_SetHandle ( env, aShape, the_aShape );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->AddOrRemoveSelected(*the_aShape,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1AddOrRemoveSelected_13 (JNIEnv *env, jobject theobj, jobject anOwner, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( SelectMgr_EntityOwner ) the_anOwner;
 void*                ptr_anOwner = jcas_GetHandle(env,anOwner);
 
 if ( ptr_anOwner != NULL ) the_anOwner = *(   (  Handle( SelectMgr_EntityOwner )*  )ptr_anOwner   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->AddOrRemoveSelected(the_anOwner,(Standard_Boolean) updateviewer);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsSelected (JNIEnv *env, jobject theobj, jobject aniobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsSelected(the_aniobj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_InitSelected (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->InitSelected();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MoreSelected (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreSelected();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NextSelected (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->NextSelected();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NbSelected (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->NbSelected();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasSelectedShape (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasSelectedShape();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SelectedShape (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
TopoDS_Shape* theret = new TopoDS_Shape(the_this->SelectedShape());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SelectedOwner (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(SelectMgr_EntityOwner)* theret = new Handle(SelectMgr_EntityOwner);
*theret = the_this->SelectedOwner();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_EntityOwner",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Interactive (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->Interactive();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SelectedInteractive (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->SelectedInteractive();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasApplicative (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasApplicative();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Applicative (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(Standard_Transient)* theret = new Handle(Standard_Transient);
*theret = the_this->Applicative();
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasDetected (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasDetected();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasDetectedShape (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasDetectedShape();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DetectedShape (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TopoDS_Shape& theret = the_this->DetectedShape();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DetectedInteractive (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->DetectedInteractive();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DetectedOwner (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(SelectMgr_EntityOwner)* theret = new Handle(SelectMgr_EntityOwner);
*theret = the_this->DetectedOwner();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_EntityOwner",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_InitDetected (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->InitDetected();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MoreDetected (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->MoreDetected();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NextDetected (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->NextDetected();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DetectedCurrentShape (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TopoDS_Shape& theret = the_this->DetectedCurrentShape();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DetectedCurrentObject (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(AIS_InteractiveObject)* theret = new Handle(AIS_InteractiveObject);
*theret = the_this->DetectedCurrentObject();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/AIS_InteractiveObject",theret);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_OpenLocalContext (JNIEnv *env, jobject theobj, jboolean UseDisplayedObjects, jboolean AllowShapeDecomposition, jboolean AcceptEraseOfObjects, jboolean BothViewers)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OpenLocalContext((Standard_Boolean) UseDisplayedObjects,(Standard_Boolean) AllowShapeDecomposition,(Standard_Boolean) AcceptEraseOfObjects,(Standard_Boolean) BothViewers);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CloseLocalContext (JNIEnv *env, jobject theobj, jint Index, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->CloseLocalContext((Standard_Integer) Index,(Standard_Boolean) updateviewer);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IndexOfCurrentLocal (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IndexOfCurrentLocal();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CloseAllContexts (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->CloseAllContexts((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ResetOriginalState (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ResetOriginalState((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearLocalContext (JNIEnv *env, jobject theobj, jshort TheMode)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearLocalContext((AIS_ClearMode) TheMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_UseDisplayedObjects (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->UseDisplayedObjects();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_NotUseDisplayedObjects (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->NotUseDisplayedObjects();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_BeginImmediateDraw (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->BeginImmediateDraw();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ImmediateAdd (JNIEnv *env, jobject theobj, jobject anIObj, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ImmediateAdd(the_anIObj,(Standard_Integer) aMode);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ImmediateRemove (JNIEnv *env, jobject theobj, jobject anIObj, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ImmediateRemove(the_anIObj,(Standard_Integer) aMode);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1EndImmediateDraw_11 (JNIEnv *env, jobject theobj, jobject aView, jboolean DoubleBuf)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->EndImmediateDraw(the_aView,(Standard_Boolean) DoubleBuf);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1EndImmediateDraw_12 (JNIEnv *env, jobject theobj, jboolean DoubleBuf)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->EndImmediateDraw((Standard_Boolean) DoubleBuf);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsImmediateModeOn (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsImmediateModeOn();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Drag (JNIEnv *env, jobject theobj, jobject aView, jobject anObject, jobject aTranformation, jboolean postConcatenate, jboolean update, jboolean zBuffer)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

 Handle( Geom_Transformation ) the_aTranformation;
 void*                ptr_aTranformation = jcas_GetHandle(env,aTranformation);
 
 if ( ptr_aTranformation != NULL ) the_aTranformation = *(   (  Handle( Geom_Transformation )*  )ptr_aTranformation   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Drag(the_aView,the_anObject,the_aTranformation,(Standard_Boolean) postConcatenate,(Standard_Boolean) update,(Standard_Boolean) zBuffer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetAutomaticHilight (JNIEnv *env, jobject theobj, jboolean aStatus)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetAutomaticHilight((Standard_Boolean) aStatus);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AutomaticHilight (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AutomaticHilight();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetZDetection (JNIEnv *env, jobject theobj, jboolean aStatus)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetZDetection((Standard_Boolean) aStatus);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ZDetection (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->ZDetection();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Activate (JNIEnv *env, jobject theobj, jobject anIobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Activate(the_anIobj,(Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Deactivate_11 (JNIEnv *env, jobject theobj, jobject anIObj)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIObj;
 void*                ptr_anIObj = jcas_GetHandle(env,anIObj);
 
 if ( ptr_anIObj != NULL ) the_anIObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIObj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Deactivate(the_anIObj);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1Deactivate_12 (JNIEnv *env, jobject theobj, jobject anIobj, jint aMode)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Deactivate(the_anIobj,(Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ActivatedModes (JNIEnv *env, jobject theobj, jobject anIobj, jobject theList)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

TColStd_ListOfInteger* the_theList = (TColStd_ListOfInteger*) jcas_GetHandle(env,theList);
if ( the_theList == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_theList = new TColStd_ListOfInteger ();
 // jcas_SetHandle ( env, theList, the_theList );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ActivatedModes(the_anIobj,*the_theList);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetShapeDecomposition (JNIEnv *env, jobject theobj, jobject anIobj, jboolean aStatus)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anIobj;
 void*                ptr_anIobj = jcas_GetHandle(env,anIobj);
 
 if ( ptr_anIobj != NULL ) the_anIobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anIobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetShapeDecomposition(the_anIobj,(Standard_Boolean) aStatus);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SetTemporaryAttributes (JNIEnv *env, jobject theobj, jobject anObj, jobject aDrawer, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObj;
 void*                ptr_anObj = jcas_GetHandle(env,anObj);
 
 if ( ptr_anObj != NULL ) the_anObj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObj   );

 Handle( Prs3d_Drawer ) the_aDrawer;
 void*                ptr_aDrawer = jcas_GetHandle(env,aDrawer);
 
 if ( ptr_aDrawer != NULL ) the_aDrawer = *(   (  Handle( Prs3d_Drawer )*  )ptr_aDrawer   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SetTemporaryAttributes(the_anObj,the_aDrawer,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SubIntensityOn_11 (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SubIntensityOn(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SubIntensityOff_11 (JNIEnv *env, jobject theobj, jobject aniobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_aniobj;
 void*                ptr_aniobj = jcas_GetHandle(env,aniobj);
 
 if ( ptr_aniobj != NULL ) the_aniobj = *(   (  Handle( AIS_InteractiveObject )*  )ptr_aniobj   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SubIntensityOff(the_aniobj,(Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SubIntensityOn_12 (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SubIntensityOn((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1SubIntensityOff_12 (JNIEnv *env, jobject theobj, jboolean updateviewer)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->SubIntensityOff((Standard_Boolean) updateviewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AddFilter (JNIEnv *env, jobject theobj, jobject aFilter)
{

jcas_Locking alock(env);
{
try {
 Handle( SelectMgr_Filter ) the_aFilter;
 void*                ptr_aFilter = jcas_GetHandle(env,aFilter);
 
 if ( ptr_aFilter != NULL ) the_aFilter = *(   (  Handle( SelectMgr_Filter )*  )ptr_aFilter   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->AddFilter(the_aFilter);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_RemoveFilter (JNIEnv *env, jobject theobj, jobject aFilter)
{

jcas_Locking alock(env);
{
try {
 Handle( SelectMgr_Filter ) the_aFilter;
 void*                ptr_aFilter = jcas_GetHandle(env,aFilter);
 
 if ( ptr_aFilter != NULL ) the_aFilter = *(   (  Handle( SelectMgr_Filter )*  )ptr_aFilter   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->RemoveFilter(the_aFilter);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_RemoveFilters (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->RemoveFilters();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ActivateStandardMode (JNIEnv *env, jobject theobj, jshort aStandardActivation)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ActivateStandardMode((TopAbs_ShapeEnum) aStandardActivation);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DeactivateStandardMode (JNIEnv *env, jobject theobj, jshort aStandardActivation)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DeactivateStandardMode((TopAbs_ShapeEnum) aStandardActivation);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ActivatedStandardModes (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TColStd_ListOfInteger& theret = the_this->ActivatedStandardModes();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TColStd_ListOfInteger",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Filters (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const SelectMgr_ListOfFilter& theret = the_this->Filters();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_ListOfFilter",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DefaultDrawer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(Prs3d_Drawer)* theret = new Handle(Prs3d_Drawer);
*theret = the_this->DefaultDrawer();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Prs3d_Drawer",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CurrentViewer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = the_this->CurrentViewer();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Viewer",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayedObjects_11 (JNIEnv *env, jobject theobj, jobject aListOfIO, jboolean OnlyFromNeutral)
{

jcas_Locking alock(env);
{
try {
AIS_ListOfInteractive* the_aListOfIO = (AIS_ListOfInteractive*) jcas_GetHandle(env,aListOfIO);
if ( the_aListOfIO == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListOfIO = new AIS_ListOfInteractive ();
 // jcas_SetHandle ( env, aListOfIO, the_aListOfIO );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayedObjects(*the_aListOfIO,(Standard_Boolean) OnlyFromNeutral);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayedObjects_12 (JNIEnv *env, jobject theobj, jshort WhichKind, jint WhichSignature, jobject aListOfIO, jboolean OnlyFromNeutral)
{

jcas_Locking alock(env);
{
try {
AIS_ListOfInteractive* the_aListOfIO = (AIS_ListOfInteractive*) jcas_GetHandle(env,aListOfIO);
if ( the_aListOfIO == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListOfIO = new AIS_ListOfInteractive ();
 // jcas_SetHandle ( env, aListOfIO, the_aListOfIO );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayedObjects((AIS_KindOfInteractive) WhichKind,(Standard_Integer) WhichSignature,*the_aListOfIO,(Standard_Boolean) OnlyFromNeutral);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_Collector (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = the_this->Collector();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_Viewer",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1ObjectsInCollector_11 (JNIEnv *env, jobject theobj, jobject aListOfIO)
{

jcas_Locking alock(env);
{
try {
AIS_ListOfInteractive* the_aListOfIO = (AIS_ListOfInteractive*) jcas_GetHandle(env,aListOfIO);
if ( the_aListOfIO == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListOfIO = new AIS_ListOfInteractive ();
 // jcas_SetHandle ( env, aListOfIO, the_aListOfIO );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ObjectsInCollector(*the_aListOfIO);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1ObjectsInCollector_12 (JNIEnv *env, jobject theobj, jshort WhichKind, jint WhichSignature, jobject aListOfIO)
{

jcas_Locking alock(env);
{
try {
AIS_ListOfInteractive* the_aListOfIO = (AIS_ListOfInteractive*) jcas_GetHandle(env,aListOfIO);
if ( the_aListOfIO == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListOfIO = new AIS_ListOfInteractive ();
 // jcas_SetHandle ( env, aListOfIO, the_aListOfIO );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ObjectsInCollector((AIS_KindOfInteractive) WhichKind,(Standard_Integer) WhichSignature,*the_aListOfIO);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ObjectsInside (JNIEnv *env, jobject theobj, jobject aListOfIO, jshort WhichKind, jint WhichSignature)
{

jcas_Locking alock(env);
{
try {
AIS_ListOfInteractive* the_aListOfIO = (AIS_ListOfInteractive*) jcas_GetHandle(env,aListOfIO);
if ( the_aListOfIO == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aListOfIO = new AIS_ListOfInteractive ();
 // jcas_SetHandle ( env, aListOfIO, the_aListOfIO );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ObjectsInside(*the_aListOfIO,(AIS_KindOfInteractive) WhichKind,(Standard_Integer) WhichSignature);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HasOpenedContext (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HasOpenedContext();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CurrentName (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TCollection_AsciiString& theret = the_this->CurrentName();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SelectionName (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
const TCollection_AsciiString& theret = the_this->SelectionName();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",&theret,0);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DomainOfMainViewer (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Standard_CString sret = the_this->DomainOfMainViewer();
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_DomainOfCollector (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Standard_CString sret = the_this->DomainOfCollector();
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_SelectionManager (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(SelectMgr_SelectionManager)* theret = new Handle(SelectMgr_SelectionManager);
*theret = the_this->SelectionManager();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/SelectMgr_SelectionManager",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MainPrsMgr (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(PrsMgr_PresentationManager3d)* theret = new Handle(PrsMgr_PresentationManager3d);
*theret = the_this->MainPrsMgr();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/PrsMgr_PresentationManager3d",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CollectorPrsMgr (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(PrsMgr_PresentationManager3d)* theret = new Handle(PrsMgr_PresentationManager3d);
*theret = the_this->CollectorPrsMgr();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/PrsMgr_PresentationManager3d",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_MainSelector (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(StdSelect_ViewerSelector3d)* theret = new Handle(StdSelect_ViewerSelector3d);
*theret = the_this->MainSelector();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/StdSelect_ViewerSelector3d",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_LocalSelector (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(StdSelect_ViewerSelector3d)* theret = new Handle(StdSelect_ViewerSelector3d);
*theret = the_this->LocalSelector();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/StdSelect_ViewerSelector3d",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_CollectorSelector (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
Handle(StdSelect_ViewerSelector3d)* theret = new Handle(StdSelect_ViewerSelector3d);
*theret = the_this->CollectorSelector();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/StdSelect_ViewerSelector3d",theret);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_PurgeDisplay (JNIEnv *env, jobject theobj, jboolean CollectorToo)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->PurgeDisplay((Standard_Boolean) CollectorToo);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_HighestIndex (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->HighestIndex();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayActiveAreas_11 (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayActiveAreas(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearActiveAreas (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearActiveAreas(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayActiveSensitive_11 (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayActiveSensitive(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_ClearActiveSensitive (JNIEnv *env, jobject theobj, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearActiveSensitive(the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayActiveSensitive_12 (JNIEnv *env, jobject theobj, jobject anObject, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayActiveSensitive(the_anObject,the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_AIS_1InteractiveContext_1DisplayActiveAreas_12 (JNIEnv *env, jobject theobj, jobject anObject, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayActiveAreas(the_anObject,the_aView);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1InteractiveContext_IsInLocal (JNIEnv *env, jobject theobj, jobject anObject, jobject TheIndex)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Standard_Integer the_TheIndex = jcas_GetInteger(env,TheIndex);
Handle(AIS_InteractiveContext) the_this = *((Handle(AIS_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsInLocal(the_anObject,the_TheIndex);
jcas_SetInteger(env,TheIndex,the_TheIndex);

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
