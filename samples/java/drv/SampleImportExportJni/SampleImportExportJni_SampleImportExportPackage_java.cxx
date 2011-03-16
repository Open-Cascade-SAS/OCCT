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


#include <SampleImportExportJni_SampleImportExportPackage.h>
#include <SampleImportExportPackage.hxx>
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
#include <Standard_CString.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <MgtBRep_TriangleMode.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleImportExportJni_SampleImportExportPackage_CreateViewer3d (JNIEnv *env, jclass, jstring aName)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_ExtString the_aName = jcas_ConvertToExtString(env,aName);
Handle(V3d_Viewer)* theret = new Handle(V3d_Viewer);
*theret = SampleImportExportPackage::CreateViewer3d(the_aName);
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



JNIEXPORT void JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SetWindow3d (JNIEnv *env, jclass, jobject aView, jint hiwin, jint lowin)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V3d_View )*  )ptr_aView   );

SampleImportExportPackage::SetWindow3d(the_aView,(Standard_Integer) hiwin,(Standard_Integer) lowin);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_ReadBREP (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::ReadBREP(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SaveBREP (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::SaveBREP(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_ReadCSFDB (JNIEnv *env, jclass, jobject aFileName, jobject aContext, jobject ReturnMessage)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

TCollection_AsciiString* the_ReturnMessage = (TCollection_AsciiString*) jcas_GetHandle(env,ReturnMessage);
if ( the_ReturnMessage == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_ReturnMessage = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, ReturnMessage, the_ReturnMessage );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = SampleImportExportPackage::ReadCSFDB(the_aFileName,the_aContext,*the_ReturnMessage);

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



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SaveCSFDB (JNIEnv *env, jclass, jobject aFileName, jobject aContext, jobject ReturnMessage, jshort aTriangleMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

TCollection_AsciiString* the_ReturnMessage = (TCollection_AsciiString*) jcas_GetHandle(env,ReturnMessage);
if ( the_ReturnMessage == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_ReturnMessage = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, ReturnMessage, the_ReturnMessage );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = SampleImportExportPackage::SaveCSFDB(the_aFileName,the_aContext,*the_ReturnMessage,(MgtBRep_TriangleMode) aTriangleMode);

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



JNIEXPORT jshort JNICALL Java_SampleImportExportJni_SampleImportExportPackage_ReadSTEP (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::ReadSTEP(the_aFileName,the_aContext);

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



JNIEXPORT jshort JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SaveSTEP (JNIEnv *env, jclass, jobject aFileName, jobject aContext, jshort aValue)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::SaveSTEP(the_aFileName,the_aContext,(STEPControl_StepModelType) aValue);

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



JNIEXPORT jint JNICALL Java_SampleImportExportJni_SampleImportExportPackage_ReadIGES (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::ReadIGES(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SaveIGES (JNIEnv *env, jclass, jobject aFileName, jobject aContext)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_aFileName = jcas_ConvertToCString(env,aFileName);
 Handle( AIS_InteractiveContext ) the_aContext;
 void*                ptr_aContext = jcas_GetHandle(env,aContext);
 
 if ( ptr_aContext != NULL ) the_aContext = *(   (  Handle( AIS_InteractiveContext )*  )ptr_aContext   );

 thejret = SampleImportExportPackage::SaveIGES(the_aFileName,the_aContext);

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



JNIEXPORT jboolean JNICALL Java_SampleImportExportJni_SampleImportExportPackage_SaveImage (JNIEnv *env, jclass, jobject aFileName, jobject aFormat, jobject aView)
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

 thejret = SampleImportExportPackage::SaveImage(the_aFileName,the_aFormat,the_aView);

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
