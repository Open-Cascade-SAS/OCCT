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

#include <CASCADESamplesJni_TopoDS_Shape.h>
#include <TopoDS_Shape.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_Boolean.hxx>
#include <TopLoc_Location.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_TShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Create_10 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* theret = new TopoDS_Shape();
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_IsNull (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsNull();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Nullify (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Nullify();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Location_11 (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
const TopLoc_Location& theret = the_this->Location();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopLoc_Location",&theret,0);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Location_12 (JNIEnv *env, jobject theobj, jobject Loc)
{

jcas_Locking alock(env);
{
try {
TopLoc_Location* the_Loc = (TopLoc_Location*) jcas_GetHandle(env,Loc);
if ( the_Loc == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Loc = new TopLoc_Location ();
 // jcas_SetHandle ( env, Loc, the_Loc );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Location(*the_Loc);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Located (JNIEnv *env, jobject theobj, jobject Loc)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopLoc_Location* the_Loc = (TopLoc_Location*) jcas_GetHandle(env,Loc);
if ( the_Loc == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Loc = new TopLoc_Location ();
 // jcas_SetHandle ( env, Loc, the_Loc );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Located(*the_Loc));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Orientation_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Orientation();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Orientation_12 (JNIEnv *env, jobject theobj, jshort Orient)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Orientation((TopAbs_Orientation) Orient);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Oriented (JNIEnv *env, jobject theobj, jshort Or)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Oriented((TopAbs_Orientation) Or));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1TShape_11 (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
Handle(TopoDS_TShape)* theret = new Handle(TopoDS_TShape);
*theret = the_this->TShape();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_TShape",theret);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_ShapeType (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->ShapeType();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Free_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Free();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Free_12 (JNIEnv *env, jobject theobj, jboolean F)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Free((Standard_Boolean) F);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Modified_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Modified();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Modified_12 (JNIEnv *env, jobject theobj, jboolean M)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Modified((Standard_Boolean) M);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Checked_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Checked();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Checked_12 (JNIEnv *env, jobject theobj, jboolean C)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Checked((Standard_Boolean) C);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Orientable_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Orientable();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Orientable_12 (JNIEnv *env, jobject theobj, jboolean C)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Orientable((Standard_Boolean) C);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Closed_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Closed();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Closed_12 (JNIEnv *env, jobject theobj, jboolean C)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Closed((Standard_Boolean) C);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Infinite_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Infinite();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Infinite_12 (JNIEnv *env, jobject theobj, jboolean C)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Infinite((Standard_Boolean) C);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Convex_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->Convex();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1Convex_12 (JNIEnv *env, jobject theobj, jboolean C)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Convex((Standard_Boolean) C);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Move (JNIEnv *env, jobject theobj, jobject position)
{

jcas_Locking alock(env);
{
try {
TopLoc_Location* the_position = (TopLoc_Location*) jcas_GetHandle(env,position);
if ( the_position == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_position = new TopLoc_Location ();
 // jcas_SetHandle ( env, position, the_position );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Move(*the_position);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Moved (JNIEnv *env, jobject theobj, jobject position)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopLoc_Location* the_position = (TopLoc_Location*) jcas_GetHandle(env,position);
if ( the_position == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_position = new TopLoc_Location ();
 // jcas_SetHandle ( env, position, the_position );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Moved(*the_position));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Reverse (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Reverse();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Reversed (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Reversed());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Complement (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Complement();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Complemented (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Complemented());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Compose (JNIEnv *env, jobject theobj, jshort Orient)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->Compose((TopAbs_Orientation) Orient);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_Composed (JNIEnv *env, jobject theobj, jshort Orient)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->Composed((TopAbs_Orientation) Orient));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_IsPartner (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_other = (TopoDS_Shape*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TopoDS_Shape ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsPartner(*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_IsSame (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_other = (TopoDS_Shape*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TopoDS_Shape ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsSame(*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_IsEqual (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_other = (TopoDS_Shape*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TopoDS_Shape ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsEqual(*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_IsNotEqual (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_other = (TopoDS_Shape*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TopoDS_Shape ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsNotEqual(*the_other);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_HashCode (JNIEnv *env, jobject theobj, jint Upper)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
 thejret = the_this->HashCode((Standard_Integer) Upper);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_EmptyCopy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->EmptyCopy();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_EmptyCopied (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
TopoDS_Shape* theret = new TopoDS_Shape(the_this->EmptyCopied());
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_TopoDS_1Shape_1TShape_12 (JNIEnv *env, jobject theobj, jobject T)
{

jcas_Locking alock(env);
{
try {
 Handle( TopoDS_TShape ) the_T;
 void*                ptr_T = jcas_GetHandle(env,T);
 
 if ( ptr_T != NULL ) the_T = *(   (  Handle( TopoDS_TShape )*  )ptr_T   );

TopoDS_Shape* the_this = (TopoDS_Shape*) jcas_GetHandle(env,theobj);
the_this->TShape(the_T);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TopoDS_1Shape_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TopoDS_Shape* theobj = (TopoDS_Shape*) theid;
  delete theobj;
}
}


}
