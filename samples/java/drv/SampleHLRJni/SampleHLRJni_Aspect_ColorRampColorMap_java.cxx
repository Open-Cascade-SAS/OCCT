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

#include <SampleHLRJni_Aspect_ColorRampColorMap.h>
#include <Aspect_ColorRampColorMap.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Integer.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Aspect_ColorMapEntry.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_Aspect_1ColorRampColorMap_1Create_11 (JNIEnv *env, jobject theobj, jint basepixel, jint dimension, jobject color)
{

jcas_Locking alock(env);
{
try {
Quantity_Color* the_color = (Quantity_Color*) jcas_GetHandle(env,color);
if ( the_color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color = new Quantity_Color ();
 // jcas_SetHandle ( env, color, the_color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_ColorRampColorMap)* theret = new Handle(Aspect_ColorRampColorMap);
*theret = new Aspect_ColorRampColorMap((Standard_Integer) basepixel,(Standard_Integer) dimension,*the_color);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_Aspect_1ColorRampColorMap_1Create_12 (JNIEnv *env, jobject theobj, jint basepixel, jint dimension, jshort colorName)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorRampColorMap)* theret = new Handle(Aspect_ColorRampColorMap);
*theret = new Aspect_ColorRampColorMap((Standard_Integer) basepixel,(Standard_Integer) dimension,(Quantity_NameOfColor) colorName);
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



JNIEXPORT void JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_ColorRampDefinition (JNIEnv *env, jobject theobj, jobject basepixel, jobject dimension, jobject color)
{

jcas_Locking alock(env);
{
try {
Standard_Integer the_basepixel = jcas_GetInteger(env,basepixel);
Standard_Integer the_dimension = jcas_GetInteger(env,dimension);
Quantity_Color* the_color = (Quantity_Color*) jcas_GetHandle(env,color);
if ( the_color == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_color = new Quantity_Color ();
 // jcas_SetHandle ( env, color, the_color );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
the_this->ColorRampDefinition(the_basepixel,the_dimension,*the_color);
jcas_SetInteger(env,basepixel,the_basepixel);
jcas_SetInteger(env,dimension,the_dimension);

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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_FindColorMapIndex (JNIEnv *env, jobject theobj, jint ColorMapEntryIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->FindColorMapIndex((Standard_Integer) ColorMapEntryIndex);

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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_FindEntry (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_NearestColorMapIndex (JNIEnv *env, jobject theobj, jobject aColor)
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
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jobject JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_NearestEntry (JNIEnv *env, jobject theobj, jobject aColor)
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
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
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



JNIEXPORT jint JNICALL Java_SampleHLRJni_Aspect_1ColorRampColorMap_AddEntry (JNIEnv *env, jobject theobj, jobject aColor)
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
Handle(Aspect_ColorRampColorMap) the_this = *((Handle(Aspect_ColorRampColorMap)*) jcas_GetHandle(env,theobj));
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
