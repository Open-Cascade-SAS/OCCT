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

#include <CASCADESamplesJni_ISession2D_InteractiveContext.h>
#include <ISession2D_InteractiveContext.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <V2d_Viewer.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <V2d_View.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext)* theret = new Handle(ISession2D_InteractiveContext);
*theret = new ISession2D_InteractiveContext();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Create_12 (JNIEnv *env, jobject theobj, jobject aViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

Handle(ISession2D_InteractiveContext)* theret = new Handle(ISession2D_InteractiveContext);
*theret = new ISession2D_InteractiveContext(the_aViewer);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Initialize (JNIEnv *env, jobject theobj, jobject aViewer)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_Viewer ) the_aViewer;
 void*                ptr_aViewer = jcas_GetHandle(env,aViewer);
 
 if ( ptr_aViewer != NULL ) the_aViewer = *(   (  Handle( V2d_Viewer )*  )ptr_aViewer   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Initialize(the_aViewer);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Display_11 (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Display(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Display_12 (JNIEnv *env, jobject theobj, jobject anObject, jint aDisplayMode, jint aSelectionMode, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Display(the_anObject,(Standard_Integer) aDisplayMode,(Standard_Integer) aSelectionMode,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Erase (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Erase(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_DisplayAll (JNIEnv *env, jobject theobj, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayAll((Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_EraseAll (JNIEnv *env, jobject theobj, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->EraseAll((Standard_Boolean) Redraw);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_IsDisplayed (JNIEnv *env, jobject theobj, jobject anObject, jint aMode)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsDisplayed(the_anObject,(Standard_Integer) aMode);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Redisplay (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw, jboolean allmodes)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Redisplay(the_anObject,(Standard_Boolean) Redraw,(Standard_Boolean) allmodes);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Clear (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Clear(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Remove (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Remove(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Highlight (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Highlight(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Unhighlight (JNIEnv *env, jobject theobj, jobject anObject, jboolean Redraw)
{

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Unhighlight(the_anObject,(Standard_Boolean) Redraw);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_IsHilighted (JNIEnv *env, jobject theobj, jobject anObject)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
 Handle( AIS_InteractiveObject ) the_anObject;
 void*                ptr_anObject = jcas_GetHandle(env,anObject);
 
 if ( ptr_anObject != NULL ) the_anObject = *(   (  Handle( AIS_InteractiveObject )*  )ptr_anObject   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
 thejret = the_this->IsHilighted(the_anObject);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Move_11 (JNIEnv *env, jobject theobj, jint x1, jint y1, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V2d_View )*  )ptr_aView   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Move((Standard_Integer) x1,(Standard_Integer) y1,the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ISession2D_1InteractiveContext_1Move_12 (JNIEnv *env, jobject theobj, jint x1, jint y1, jint x2, jint y2, jobject aView)
{

jcas_Locking alock(env);
{
try {
 Handle( V2d_View ) the_aView;
 void*                ptr_aView = jcas_GetHandle(env,aView);
 
 if ( ptr_aView != NULL ) the_aView = *(   (  Handle( V2d_View )*  )ptr_aView   );

Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Move((Standard_Integer) x1,(Standard_Integer) y1,(Standard_Integer) x2,(Standard_Integer) y2,the_aView);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_Pick (JNIEnv *env, jobject theobj, jboolean MultiSelection)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->Pick((Standard_Boolean) MultiSelection);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_DisplayAreas (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->DisplayAreas();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_ISession2D_1InteractiveContext_ClearAreas (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(ISession2D_InteractiveContext) the_this = *((Handle(ISession2D_InteractiveContext)*) jcas_GetHandle(env,theobj));
the_this->ClearAreas();

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
