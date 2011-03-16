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

#include <CASCADESamplesJni_Viewer_Viewer.h>
#include <Viewer_Viewer.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_GraphicDevice.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_CString.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Viewer_1Viewer_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Viewer_Viewer) the_this = *((Handle(Viewer_Viewer)*) jcas_GetHandle(env,theobj));
the_this->Update();

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Viewer_1Viewer_Device (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Viewer_Viewer) the_this = *((Handle(Viewer_Viewer)*) jcas_GetHandle(env,theobj));
Handle(Aspect_GraphicDevice)* theret = new Handle(Aspect_GraphicDevice);
*theret = the_this->Device();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_GraphicDevice",theret);

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



JNIEXPORT jstring JNICALL Java_CASCADESamplesJni_Viewer_1Viewer_NextName (JNIEnv *env, jobject theobj)
{
jstring thejret;

jcas_Locking alock(env);
{
try {
Handle(Viewer_Viewer) the_this = *((Handle(Viewer_Viewer)*) jcas_GetHandle(env,theobj));
Standard_ExtString sret = the_this->NextName();
thejret = jcas_ConvertTojstring(env,sret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Viewer_1Viewer_Domain (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Viewer_Viewer) the_this = *((Handle(Viewer_Viewer)*) jcas_GetHandle(env,theobj));
Standard_CString sret = the_this->Domain();
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


}
