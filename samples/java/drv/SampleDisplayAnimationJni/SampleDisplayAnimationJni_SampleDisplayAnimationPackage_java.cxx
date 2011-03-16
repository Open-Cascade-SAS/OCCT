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


#include <SampleDisplayAnimationJni_SampleDisplayAnimationPackage.h>
#include <SampleDisplayAnimationPackage.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <Standard_ExtString.hxx>
#include <V3d_View.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_CreateViewer (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleDisplayAnimationPackage::CreateViewer(the_aName);
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



JNIEXPORT void JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_SetWindow (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleDisplayAnimationPackage::SetWindow(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_LoadData (JNIEnv *env, jclass, jobject aContext, jobject aPath)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

Standard_CString the_aPath = jcas_ConvertToCString(env,aPath);
 thejret = SampleDisplayAnimationPackage::LoadData(the_aContext,the_aPath);

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



JNIEXPORT void JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_ChangePosition (JNIEnv *env, jclass, jobject aContext)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleDisplayAnimationPackage::ChangePosition(the_aContext);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jdouble JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_GetDeviationCoefficient (JNIEnv *env, jclass)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
 thejret = SampleDisplayAnimationPackage::GetDeviationCoefficient();

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



JNIEXPORT void JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_SetDeviationCoefficient (JNIEnv *env, jclass, jobject aContext, jdouble aValue)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

SampleDisplayAnimationPackage::SetDeviationCoefficient(the_aContext,(Standard_Real) aValue);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jint JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_GetAngleIncrement (JNIEnv *env, jclass)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 thejret = SampleDisplayAnimationPackage::GetAngleIncrement();

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



JNIEXPORT void JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_SetAngleIncrement (JNIEnv *env, jclass, jint aValue)
{

jcas_Locking alock(env);
{
try {
SampleDisplayAnimationPackage::SetAngleIncrement((Standard_Integer) aValue);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleDisplayAnimationJni_SampleDisplayAnimationPackage_SaveImage (JNIEnv *env, jclass, jobject aFileName, jobject aFormat, jobject aView)
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

 thejret = SampleDisplayAnimationPackage::SaveImage(the_aFileName,the_aFormat,the_aView);

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
