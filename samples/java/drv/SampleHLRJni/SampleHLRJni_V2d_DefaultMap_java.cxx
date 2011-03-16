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

#include <SampleHLRJni_V2d_DefaultMap.h>
#include <V2d_DefaultMap.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_GenericColorMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_MarkMap.hxx>


extern "C" {


JNIEXPORT jobject JNICALL Java_SampleHLRJni_V2d_1DefaultMap_ColorMap (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_GenericColorMap)* theret = new Handle(Aspect_GenericColorMap);
*theret = V2d_DefaultMap::ColorMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_GenericColorMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_V2d_1DefaultMap_TypeMap (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_TypeMap)* theret = new Handle(Aspect_TypeMap);
*theret = V2d_DefaultMap::TypeMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_TypeMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_V2d_1DefaultMap_WidthMap (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_WidthMap)* theret = new Handle(Aspect_WidthMap);
*theret = V2d_DefaultMap::WidthMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_WidthMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_V2d_1DefaultMap_FontMap (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_FontMap)* theret = new Handle(Aspect_FontMap);
*theret = V2d_DefaultMap::FontMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_FontMap",theret);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_V2d_1DefaultMap_MarkMap (JNIEnv *env, jclass)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_MarkMap)* theret = new Handle(Aspect_MarkMap);
*theret = V2d_DefaultMap::MarkMap();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_MarkMap",theret);

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



JNIEXPORT void JNICALL Java_SampleHLRJni_V2d_1DefaultMap_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  V2d_DefaultMap* theobj = (V2d_DefaultMap*) theid;
  delete theobj;
}
}


}
