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

#include <CASCADESamplesJni_V3d_PerspectiveView.h>
#include <V3d_PerspectiveView.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_OrthographicView.hxx>
#include <Standard_Real.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_V3d_1PerspectiveView_1Create_11 (JNIEnv *env, jobject theobj, jobject VM)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_VM;
 void*                ptr_VM = jcas_GetHandle(env,VM);
 
 if ( ptr_VM != NULL ) the_VM = *(   (  Handle( V3d_Viewer )*  )ptr_VM   );

Handle(V3d_PerspectiveView)* theret = new Handle(V3d_PerspectiveView);
*theret = new V3d_PerspectiveView(the_VM);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_V3d_1PerspectiveView_1Create_12 (JNIEnv *env, jobject theobj, jobject VM, jobject V)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_VM;
 void*                ptr_VM = jcas_GetHandle(env,VM);
 
 if ( ptr_VM != NULL ) the_VM = *(   (  Handle( V3d_Viewer )*  )ptr_VM   );

 Handle( V3d_OrthographicView ) the_V;
 void*                ptr_V = jcas_GetHandle(env,V);
 
 if ( ptr_V != NULL ) the_V = *(   (  Handle( V3d_OrthographicView )*  )ptr_V   );

Handle(V3d_PerspectiveView)* theret = new Handle(V3d_PerspectiveView);
*theret = new V3d_PerspectiveView(the_VM,the_V);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_V3d_1PerspectiveView_1Create_13 (JNIEnv *env, jobject theobj, jobject VM, jobject V)
{

jcas_Locking alock(env);
{
try {
 Handle( V3d_Viewer ) the_VM;
 void*                ptr_VM = jcas_GetHandle(env,VM);
 
 if ( ptr_VM != NULL ) the_VM = *(   (  Handle( V3d_Viewer )*  )ptr_VM   );

 Handle( V3d_PerspectiveView ) the_V;
 void*                ptr_V = jcas_GetHandle(env,V);
 
 if ( ptr_V != NULL ) the_V = *(   (  Handle( V3d_PerspectiveView )*  )ptr_V   );

Handle(V3d_PerspectiveView)* theret = new Handle(V3d_PerspectiveView);
*theret = new V3d_PerspectiveView(the_VM,the_V);
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_Copy (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_PerspectiveView) the_this = *((Handle(V3d_PerspectiveView)*) jcas_GetHandle(env,theobj));
Handle(V3d_PerspectiveView)* theret = new Handle(V3d_PerspectiveView);
*theret = the_this->Copy();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/V3d_PerspectiveView",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_SetAngle (JNIEnv *env, jobject theobj, jdouble Angle)
{

jcas_Locking alock(env);
{
try {
Handle(V3d_PerspectiveView) the_this = *((Handle(V3d_PerspectiveView)*) jcas_GetHandle(env,theobj));
the_this->SetAngle((Quantity_PlaneAngle) Angle);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_V3d_1PerspectiveView_Angle (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(V3d_PerspectiveView) the_this = *((Handle(V3d_PerspectiveView)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Angle();

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
