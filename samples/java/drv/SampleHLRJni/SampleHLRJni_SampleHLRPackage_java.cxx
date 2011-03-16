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


#include <SampleHLRJni_SampleHLRPackage.h>
#include <SampleHLRPackage.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_ExtString.hxx>
#include <V3d_View.hxx>
#include <Standard_Integer.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Standard_Boolean.hxx>
#include <ISession2D_InteractiveContext.hxx>
#include <Standard_CString.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleHLRJni_SampleHLRPackage_CreateViewer3d (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleHLRPackage::CreateViewer3d(the_aName);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_SetWindow3d (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleHLRPackage::SetWindow3d(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_SampleHLRJni_SampleHLRPackage_CreateViewer2d (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V2d_Viewer)* theret = new Handle(V2d_Viewer);
*theret = SampleHLRPackage::CreateViewer2d(the_aName);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V2d_Viewer",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_SampleHLRPackage_CreateView2d (JNIEnv *env, jclass, jobject aViewer, jint hiwin, jint lowin)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

Handle(V2d_View)* theret = new Handle(V2d_View);
*theret = SampleHLRPackage::CreateView2d(the_aViewer,(Standard_Integer) hiwin,(Standard_Integer) lowin);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V2d_View",theret);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_InitMaps (JNIEnv *env, jclass, jobject aViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

SampleHLRPackage::InitMaps(the_aViewer);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_DisplayTrihedron (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleHLRPackage::DisplayTrihedron(the_aContext);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_SampleHLRPackage_GetShapes (JNIEnv *env, jclass, jobject aSrcContext, jobject aDestContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aSrcContext;
 void*                ptr_aSrcContext = jcas_GetHandle(env,aSrcContext);
 
 if ( ptr_aSrcContext != NULL ) the_aSrcContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aSrcContext   );

 Handle( AIS_InteractiveContext ) the_aDestContext;
 void*                ptr_aDestContext = jcas_GetHandle(env,aDestContext);
 
 if ( ptr_aDestContext != NULL ) the_aDestContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aDestContext   );

 thejret = SampleHLRPackage::GetShapes(the_aSrcContext,the_aDestContext);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_Apply (JNIEnv *env, jclass, jobject aContext2d, jint aDisplayMode)
{

jcas_Locking alock(env);
{
try {
 Handle( ISession2D_InteractiveContext ) the_aContext2d;
 void*                ptr_aContext2d = jcas_GetHandle(env,aContext2d);
 
 if ( ptr_aContext2d != NULL ) the_aContext2d = *(   (  Handle( ISession2D_InteractiveContext )*  )ptr_aContext2d   );

SampleHLRPackage::Apply(the_aContext2d,(Standard_Integer) aDisplayMode);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_UpdateProjector (JNIEnv *env, jclass, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleHLRPackage::UpdateProjector(the_aView);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_SampleHLRJni_SampleHLRPackage_SetNbIsos (JNIEnv *env, jclass, jint aNbIsos)
{

jcas_Locking alock(env);
{
try {
SampleHLRPackage::SetNbIsos((Standard_Integer) aNbIsos);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_SampleHLRPackage_ReadBRep (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleHLRPackage::ReadBRep(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_SampleHLRPackage_SaveBRep (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleHLRPackage::SaveBRep(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_SampleHLRPackage_SampleHLRPackage_1SaveImage_11 (JNIEnv *env, jclass, jobject aFileName, jobject aFormat, jobject aView)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
Standard_CString the_aFormat = jcas_ConvertToCString(env,aFormat);
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

 thejret = SampleHLRPackage::SaveImage(the_aFileName,the_aFormat,the_aView);

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



JNIEXPORT jboolean JNICALL Java_SampleHLRJni_SampleHLRPackage_SampleHLRPackage_1SaveImage_12 (JNIEnv *env, jclass, jobject aFileName, jobject aFormat, jobject aView)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
Standard_CString the_aFormat = jcas_ConvertToCString(env,aFormat);
 Handle( V2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V2d_View )*  )ptr_aView   );

 thejret = SampleHLRPackage::SaveImage(the_aFileName,the_aFormat,the_aView);

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
