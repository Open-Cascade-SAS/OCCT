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

#include <CASCADESamplesJni_TCollection_AsciiString.h>
#include <TCollection_AsciiString.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Character.hxx>
#include <Standard_Real.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Boolean.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_11 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* theret = new TCollection_AsciiString();
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_12 (JNIEnv *env, jobject theobj, jobject message)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_message = jcas_ConvertToCString(env,message);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_message);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_13 (JNIEnv *env, jobject theobj, jobject message, jint aLen)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_message = jcas_ConvertToCString(env,message);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_message,(Standard_Integer) aLen);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_14 (JNIEnv *env, jobject theobj, jobject aChar)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_aChar = jcas_GetCharacter(env,aChar);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_aChar);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_15 (JNIEnv *env, jobject theobj, jint length, jobject filler)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_filler = jcas_GetCharacter(env,filler);
TCollection_AsciiString* theret = new TCollection_AsciiString((Standard_Integer) length,the_filler);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_16 (JNIEnv *env, jobject theobj, jint value)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* theret = new TCollection_AsciiString((Standard_Integer) value);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_17 (JNIEnv *env, jobject theobj, jdouble value)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* theret = new TCollection_AsciiString((Standard_Real) value);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_18 (JNIEnv *env, jobject theobj, jobject astring)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* theret = new TCollection_AsciiString(*the_astring);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_19 (JNIEnv *env, jobject theobj, jobject astring, jobject message)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_Character the_message = jcas_GetCharacter(env,message);
TCollection_AsciiString* theret = new TCollection_AsciiString(*the_astring,the_message);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_110 (JNIEnv *env, jobject theobj, jobject astring, jobject message)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_CString the_message = jcas_ConvertToCString(env,message);
TCollection_AsciiString* theret = new TCollection_AsciiString(*the_astring,the_message);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_111 (JNIEnv *env, jobject theobj, jobject astring, jobject message)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_message = (TCollection_AsciiString*) jcas_GetHandle(env,message);
if ( the_message == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_message = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, message, the_message );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* theret = new TCollection_AsciiString(*the_astring,*the_message);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Create_112 (JNIEnv *env, jobject theobj, jobject astring)
{

jcas_Locking alock(env);
{
try {
TCollection_ExtendedString* the_astring = (TCollection_ExtendedString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_ExtendedString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* theret = new TCollection_AsciiString(*the_astring);
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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1AssignCat_11 (JNIEnv *env, jobject theobj, jobject other)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_other = jcas_GetCharacter(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->AssignCat(the_other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1AssignCat_12 (JNIEnv *env, jobject theobj, jint other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->AssignCat((Standard_Integer) other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1AssignCat_13 (JNIEnv *env, jobject theobj, jdouble other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->AssignCat((Standard_Real) other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1AssignCat_14 (JNIEnv *env, jobject theobj, jobject other)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->AssignCat(the_other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1AssignCat_15 (JNIEnv *env, jobject theobj, jobject other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->AssignCat(*the_other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Capitalize (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Capitalize();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Cat_11 (JNIEnv *env, jobject theobj, jobject other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_Character the_other = jcas_GetCharacter(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Cat(the_other));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Cat_12 (JNIEnv *env, jobject theobj, jint other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Cat((Standard_Integer) other));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Cat_13 (JNIEnv *env, jobject theobj, jdouble other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Cat((Standard_Real) other));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Cat_14 (JNIEnv *env, jobject theobj, jobject other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Cat(the_other));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Cat_15 (JNIEnv *env, jobject theobj, jobject other)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Cat(*the_other));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Center (JNIEnv *env, jobject theobj, jint Width, jobject Filler)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_Filler = jcas_GetCharacter(env,Filler);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Center((Standard_Integer) Width,the_Filler);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_ChangeAll (JNIEnv *env, jobject theobj, jobject aChar, jobject NewChar, jboolean CaseSensitive)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_aChar = jcas_GetCharacter(env,aChar);
Standard_Character the_NewChar = jcas_GetCharacter(env,NewChar);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->ChangeAll(the_aChar,the_NewChar,(Standard_Boolean) CaseSensitive);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Clear (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Clear();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Copy_11 (JNIEnv *env, jobject theobj, jobject fromwhere)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_fromwhere = jcas_ConvertToCString(env,fromwhere);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Copy(the_fromwhere);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Copy_12 (JNIEnv *env, jobject theobj, jobject fromwhere)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_fromwhere = (TCollection_AsciiString*) jcas_GetHandle(env,fromwhere);
if ( the_fromwhere == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_fromwhere = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, fromwhere, the_fromwhere );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Copy(*the_fromwhere);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Destroy (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Destroy();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_FirstLocationInSet (JNIEnv *env, jobject theobj, jobject Set, jint FromIndex, jint ToIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_Set = (TCollection_AsciiString*) jcas_GetHandle(env,Set);
if ( the_Set == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Set = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Set, the_Set );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->FirstLocationInSet(*the_Set,(Standard_Integer) FromIndex,(Standard_Integer) ToIndex);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_FirstLocationNotInSet (JNIEnv *env, jobject theobj, jobject Set, jint FromIndex, jint ToIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_Set = (TCollection_AsciiString*) jcas_GetHandle(env,Set);
if ( the_Set == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_Set = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, Set, the_Set );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->FirstLocationNotInSet(*the_Set,(Standard_Integer) FromIndex,(Standard_Integer) ToIndex);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Insert_11 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_what = jcas_GetCharacter(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Insert((Standard_Integer) where,the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Insert_12 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_what = jcas_ConvertToCString(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Insert((Standard_Integer) where,the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Insert_13 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_what = (TCollection_AsciiString*) jcas_GetHandle(env,what);
if ( the_what == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_what = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, what, the_what );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Insert((Standard_Integer) where,*the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_InsertAfter (JNIEnv *env, jobject theobj, jint Index, jobject other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->InsertAfter((Standard_Integer) Index,*the_other);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_InsertBefore (JNIEnv *env, jobject theobj, jint Index, jobject other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->InsertBefore((Standard_Integer) Index,*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_IsEmpty (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsEmpty();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsEqual_11 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsEqual(the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsEqual_12 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsDifferent_11 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsDifferent(the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsDifferent_12 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsDifferent(*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsLess_11 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsLess(the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsLess_12 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsLess(*the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsGreater_11 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_other = jcas_ConvertToCString(env,other);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsGreater(the_other);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsGreater_12 (JNIEnv *env, jobject theobj, jobject other)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsGreater(*the_other);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_IntegerValue (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IntegerValue();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_IsIntegerValue (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsIntegerValue();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_IsRealValue (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsRealValue();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_IsAscii (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->IsAscii();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_LeftAdjust (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->LeftAdjust();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_LeftJustify (JNIEnv *env, jobject theobj, jint Width, jobject Filler)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_Filler = jcas_GetCharacter(env,Filler);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->LeftJustify((Standard_Integer) Width,the_Filler);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Length (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->Length();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Location_11 (JNIEnv *env, jobject theobj, jobject other, jint FromIndex, jint ToIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->Location(*the_other,(Standard_Integer) FromIndex,(Standard_Integer) ToIndex);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Location_12 (JNIEnv *env, jobject theobj, jint N, jobject C, jint FromIndex, jint ToIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Standard_Character the_C = jcas_GetCharacter(env,C);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->Location((Standard_Integer) N,the_C,(Standard_Integer) FromIndex,(Standard_Integer) ToIndex);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_LowerCase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->LowerCase();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Prepend (JNIEnv *env, jobject theobj, jobject other)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_other = (TCollection_AsciiString*) jcas_GetHandle(env,other);
if ( the_other == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_other = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, other, the_other );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Prepend(*the_other);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_RealValue (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->RealValue();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1RemoveAll_11 (JNIEnv *env, jobject theobj, jobject C, jboolean CaseSensitive)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_C = jcas_GetCharacter(env,C);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->RemoveAll(the_C,(Standard_Boolean) CaseSensitive);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1RemoveAll_12 (JNIEnv *env, jobject theobj, jobject what)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_what = jcas_GetCharacter(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->RemoveAll(the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Remove (JNIEnv *env, jobject theobj, jint where, jint ahowmany)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Remove((Standard_Integer) where,(Standard_Integer) ahowmany);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_RightAdjust (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->RightAdjust();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_RightJustify (JNIEnv *env, jobject theobj, jint Width, jobject Filler)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_Filler = jcas_GetCharacter(env,Filler);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->RightJustify((Standard_Integer) Width,the_Filler);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Search_11 (JNIEnv *env, jobject theobj, jobject what)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_what = jcas_ConvertToCString(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->Search(the_what);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Search_12 (JNIEnv *env, jobject theobj, jobject what)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_what = (TCollection_AsciiString*) jcas_GetHandle(env,what);
if ( the_what == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_what = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, what, the_what );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->Search(*the_what);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SearchFromEnd_11 (JNIEnv *env, jobject theobj, jobject what)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_what = jcas_ConvertToCString(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->SearchFromEnd(the_what);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SearchFromEnd_12 (JNIEnv *env, jobject theobj, jobject what)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_what = (TCollection_AsciiString*) jcas_GetHandle(env,what);
if ( the_what == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_what = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, what, the_what );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->SearchFromEnd(*the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SetValue_11 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
Standard_Character the_what = jcas_GetCharacter(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) where,the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SetValue_12 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
Standard_CString the_what = jcas_ConvertToCString(env,what);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) where,the_what);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SetValue_13 (JNIEnv *env, jobject theobj, jint where, jobject what)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_what = (TCollection_AsciiString*) jcas_GetHandle(env,what);
if ( the_what == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_what = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, what, the_what );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->SetValue((Standard_Integer) where,*the_what);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Split_12 (JNIEnv *env, jobject theobj, jint where)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Split((Standard_Integer) where));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1SubString_12 (JNIEnv *env, jobject theobj, jint FromIndex, jint ToIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->SubString((Standard_Integer) FromIndex,(Standard_Integer) ToIndex));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_ToCString (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
Standard_CString sret = the_this->ToCString();
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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1Token_12 (JNIEnv *env, jobject theobj, jobject separators, jint whichone)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Standard_CString the_separators = jcas_ConvertToCString(env,separators);
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
TCollection_AsciiString* theret = new TCollection_AsciiString(the_this->Token(the_separators,(Standard_Integer) whichone));
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TCollection_AsciiString",theret);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Trunc (JNIEnv *env, jobject theobj, jint ahowmany)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->Trunc((Standard_Integer) ahowmany);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_UpperCase (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
the_this->UpperCase();

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_UsefullLength (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
 thejret = the_this->UsefullLength();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_Value (JNIEnv *env, jobject theobj, jint where)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_this = (TCollection_AsciiString*) jcas_GetHandle(env,theobj);
Standard_Character *theret = ((Standard_Character *) malloc(sizeof(Standard_Character)));
*theret = the_this->Value((Standard_Integer) where);
thejret = jcas_CreateObject(env,"jcas/Standard_Character",theret);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_HashCode (JNIEnv *env, jclass, jobject astring, jint Upper)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = TCollection_AsciiString::HashCode(*the_astring,(Standard_Integer) Upper);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsEqual_13 (JNIEnv *env, jclass, jobject string1, jobject string2)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_string1 = (TCollection_AsciiString*) jcas_GetHandle(env,string1);
if ( the_string1 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_string1 = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, string1, the_string1 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_string2 = (TCollection_AsciiString*) jcas_GetHandle(env,string2);
if ( the_string2 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_string2 = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, string2, the_string2 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = TCollection_AsciiString::IsEqual(*the_string1,*the_string2);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_TCollection_1AsciiString_1IsEqual_14 (JNIEnv *env, jclass, jobject string1, jobject string2)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_string1 = (TCollection_AsciiString*) jcas_GetHandle(env,string1);
if ( the_string1 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_string1 = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, string1, the_string1 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Standard_CString the_string2 = jcas_ConvertToCString(env,string2);
 thejret = TCollection_AsciiString::IsEqual(*the_string1,the_string2);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_HASHCODE (JNIEnv *env, jclass, jobject astring, jint Upper)
{
jint thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_astring = (TCollection_AsciiString*) jcas_GetHandle(env,astring);
if ( the_astring == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_astring = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, astring, the_astring );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = TCollection_AsciiString::HASHCODE(*the_astring,(Standard_Integer) Upper);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_ISSIMILAR (JNIEnv *env, jclass, jobject string1, jobject string2)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
TCollection_AsciiString* the_string1 = (TCollection_AsciiString*) jcas_GetHandle(env,string1);
if ( the_string1 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_string1 = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, string1, the_string1 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
TCollection_AsciiString* the_string2 = (TCollection_AsciiString*) jcas_GetHandle(env,string2);
if ( the_string2 == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_string2 = new TCollection_AsciiString ();
 // jcas_SetHandle ( env, string2, the_string2 );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 thejret = TCollection_AsciiString::ISSIMILAR(*the_string1,*the_string2);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_TCollection_1AsciiString_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TCollection_AsciiString* theobj = (TCollection_AsciiString*) theid;
  delete theobj;
}
}


}
