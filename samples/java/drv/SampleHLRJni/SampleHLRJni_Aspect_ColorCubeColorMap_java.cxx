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

#include <SampleHLRJni_Aspect_ColorCubeColorMap.h>
#include <Aspect_ColorCubeColorMap.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Quantity_Color.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_Aspect_1ColorCubeColorMap_1Create_10 (JNIEnv *env, jobject theobj, jint base_pixel, jint redmax, jint redmult, jint greenmax, jint greenmult, jint bluemax, jint bluemult)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorCubeColorMap)* theret = new Handle(Aspect_ColorCubeColorMap);
*theret = new Aspect_ColorCubeColorMap((Standard_Integer) base_pixel,(Standard_Integer) redmax,(Standard_Integer) redmult,(Standard_Integer) greenmax,(Standard_Integer) greenmult,(Standard_Integer) bluemax,(Standard_Integer) bluemult);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_ColorCubeDefinition (JNIEnv *env, jobject theobj, jobject base_pixel, jobject redmax, jobject redmult, jobject greenmax, jobject greenmult, jobject bluemax, jobject bluemult)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_base_pixel = jcas_GetInteger(env,base_pixel);
Standard_Integer the_redmax = jcas_GetInteger(env,redmax);
Standard_Integer the_redmult = jcas_GetInteger(env,redmult);
Standard_Integer the_greenmax = jcas_GetInteger(env,greenmax);
Standard_Integer the_greenmult = jcas_GetInteger(env,greenmult);
Standard_Integer the_bluemax = jcas_GetInteger(env,bluemax);
Standard_Integer the_bluemult = jcas_GetInteger(env,bluemult);
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
the_this->ColorCubeDefinition(the_base_pixel,the_redmax,the_redmult,the_greenmax,the_greenmult,the_bluemax,the_bluemult);
jcas_SetInteger(env,base_pixel,the_base_pixel);
jcas_SetInteger(env,redmax,the_redmax);
jcas_SetInteger(env,redmult,the_redmult);
jcas_SetInteger(env,greenmax,the_greenmax);
jcas_SetInteger(env,greenmult,the_greenmult);
jcas_SetInteger(env,bluemax,the_bluemax);
jcas_SetInteger(env,bluemult,the_bluemult);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_FindColorMapIndex (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->FindColorMapIndex((Standard_Integer) AColorMapEntryIndex);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_FindEntry (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
const Aspect_ColorMapEntry& theret = the_this->FindEntry((Standard_Integer) AColorMapEntryIndex);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMapEntry",&theret,0);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_NearestColorMapIndex (JNIEnv *env, jobject theobj, jobject aColor)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->NearestColorMapIndex(*the_aColor);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_NearestEntry (JNIEnv *env, jobject theobj, jobject aColor)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
const Aspect_ColorMapEntry& theret = the_this->NearestEntry(*the_aColor);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMapEntry",&theret,0);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorCubeColorMap_AddEntry (JNIEnv *env, jobject theobj, jobject aColor)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
if ( the_aColor == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aColor = new Quantity_Color ();
 // jcas_SetHandle ( env, aColor, the_aColor );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_ColorCubeColorMap) the_this = *((Handle(Aspect_ColorCubeColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AddEntry(*the_aColor);

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
