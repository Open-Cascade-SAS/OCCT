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

#include <CASCADESamplesJni_Viewer_View.h>
#include <Viewer_View.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Viewer_1View_Update (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Viewer_View) the_this = *((Handle(Viewer_View)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_Viewer_1View_SetImmediateUpdate (JNIEnv *env, jobject theobj, jboolean onoff)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(Viewer_View) the_this = *((Handle(Viewer_View)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetImmediateUpdate((Standard_Boolean) onoff);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Viewer_1View_WindowFit (JNIEnv *env, jobject theobj, jint Xmin, jint Ymin, jint Xmax, jint Ymax)
{

jcas_Locking alock(env);
{
try {
Handle(Viewer_View) the_this = *((Handle(Viewer_View)*) jcas_GetHandle(env,theobj));
the_this->WindowFit((Standard_Integer) Xmin,(Standard_Integer) Ymin,(Standard_Integer) Xmax,(Standard_Integer) Ymax);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,Err.str().c_str());
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Viewer_1View_Place (JNIEnv *env, jobject theobj, jint x, jint y, jdouble aZoomFactor)
{

jcas_Locking alock(env);
{
try {
Handle(Viewer_View) the_this = *((Handle(Viewer_View)*) jcas_GetHandle(env,theobj));
the_this->Place((Standard_Integer) x,(Standard_Integer) y,(Quantity_Factor) aZoomFactor);

}
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
