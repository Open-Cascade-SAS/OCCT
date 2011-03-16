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

#include <SampleHLRJni_Aspect_Background.h>
#include <Aspect_Background.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Quantity_Color.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Background_Aspect_1Background_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Aspect_Background* theret = new Aspect_Background();
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Background_Aspect_1Background_1Create_12 (JNIEnv *env, jobject theobj, jobject AColor)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_AColor = (Quantity_Color*) jcas_GetHandle(env,AColor);
if ( the_AColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AColor = new Quantity_Color ();
 // jcas_SetHandle ( env, AColor, the_AColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_Background* theret = new Aspect_Background(*the_AColor);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Background_SetColor (JNIEnv *env, jobject theobj, jobject AColor)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_AColor = (Quantity_Color*) jcas_GetHandle(env,AColor);
if ( the_AColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_AColor = new Quantity_Color ();
 // jcas_SetHandle ( env, AColor, the_AColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Aspect_Background* the_this = (Aspect_Background*) jcas_GetHandle(env,theobj);
the_this->SetColor(*the_AColor);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1Background_Color (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Aspect_Background* the_this = (Aspect_Background*) jcas_GetHandle(env,theobj);
Quantity_Color* theret = new Quantity_Color(the_this->Color());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Quantity_Color",theret);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1Background_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  Aspect_Background* theobj = (Aspect_Background*) theid;
  delete theobj;
}
}


}
