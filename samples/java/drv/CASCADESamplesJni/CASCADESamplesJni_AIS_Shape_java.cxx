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

#include <CASCADESamplesJni_AIS_Shape.h>
#include <AIS_Shape.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_SStream.hxx>

#include <TopoDS_Shape.hxx>
#include <Standard_Integer.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Bnd_Box.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Prs3d_Drawer.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1Create_10 (JNIEnv *env, jobject theobj, jobject shap)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_shap = (TopoDS_Shape*) jcas_GetHandle(env,shap);
if ( the_shap == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_shap = new TopoDS_Shape ();
 // jcas_SetHandle ( env, shap, the_shap );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_Shape)* theret = new Handle(AIS_Shape);
*theret = new AIS_Shape(*the_shap);
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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1Shape_Signature (JNIEnv *env, jobject theobj)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Signature();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1Shape_Type (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Type();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_AcceptShapeDecomposition (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AcceptShapeDecomposition();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_Set (JNIEnv *env, jobject theobj, jobject ashap)
{

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_ashap = (TopoDS_Shape*) jcas_GetHandle(env,ashap);
if ( the_ashap == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_ashap = new TopoDS_Shape ();
 // jcas_SetHandle ( env, ashap, the_ashap );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->Set(*the_ashap);

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1Shape_Shape (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
const TopoDS_Shape& theret = the_this->Shape();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/TopoDS_Shape",&theret,0);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnDeviationCoefficient_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetOwnDeviationCoefficient();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnHLRDeviationCoefficient_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetOwnHLRDeviationCoefficient();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnDeviationAngle_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetOwnDeviationAngle();

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnHLRDeviationAngle_11 (JNIEnv *env, jobject theobj)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->SetOwnHLRDeviationAngle();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnDeviationCoefficient_12 (JNIEnv *env, jobject theobj, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetOwnDeviationCoefficient((Standard_Real) aCoefficient);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnHLRDeviationCoefficient_12 (JNIEnv *env, jobject theobj, jdouble aCoefficient)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetOwnHLRDeviationCoefficient((Standard_Real) aCoefficient);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_SetAngleAndDeviation (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetAngleAndDeviation((Standard_Real) anAngle);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1Shape_UserAngle (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->UserAngle();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnDeviationAngle_12 (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetOwnDeviationAngle((Standard_Real) anAngle);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_SetHLRAngleAndDeviation (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetHLRAngleAndDeviation((Standard_Real) anAngle);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetOwnHLRDeviationAngle_12 (JNIEnv *env, jobject theobj, jdouble anAngle)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetOwnHLRDeviationAngle((Standard_Real) anAngle);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_OwnDeviationCoefficient (JNIEnv *env, jobject theobj, jobject aCoefficient, jobject aPreviousCoefficient)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_aCoefficient = jcas_GetReal(env,aCoefficient);
Standard_Real the_aPreviousCoefficient = jcas_GetReal(env,aPreviousCoefficient);
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OwnDeviationCoefficient(the_aCoefficient,the_aPreviousCoefficient);
jcas_SetReal(env,aCoefficient,the_aCoefficient);
jcas_SetReal(env,aPreviousCoefficient,the_aPreviousCoefficient);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_OwnHLRDeviationCoefficient (JNIEnv *env, jobject theobj, jobject aCoefficient, jobject aPreviousCoefficient)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_aCoefficient = jcas_GetReal(env,aCoefficient);
Standard_Real the_aPreviousCoefficient = jcas_GetReal(env,aPreviousCoefficient);
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OwnHLRDeviationCoefficient(the_aCoefficient,the_aPreviousCoefficient);
jcas_SetReal(env,aCoefficient,the_aCoefficient);
jcas_SetReal(env,aPreviousCoefficient,the_aPreviousCoefficient);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_OwnDeviationAngle (JNIEnv *env, jobject theobj, jobject anAngle, jobject aPreviousAngle)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_anAngle = jcas_GetReal(env,anAngle);
Standard_Real the_aPreviousAngle = jcas_GetReal(env,aPreviousAngle);
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OwnDeviationAngle(the_anAngle,the_aPreviousAngle);
jcas_SetReal(env,anAngle,the_anAngle);
jcas_SetReal(env,aPreviousAngle,the_aPreviousAngle);

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



JNIEXPORT jboolean JNICALL Java_CASCADESamplesJni_AIS_1Shape_OwnHLRDeviationAngle (JNIEnv *env, jobject theobj, jobject anAngle, jobject aPreviousAngle)
{
jboolean thejret;

jcas_Locking alock(env);
{
try {
Standard_Real the_anAngle = jcas_GetReal(env,anAngle);
Standard_Real the_aPreviousAngle = jcas_GetReal(env,aPreviousAngle);
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->OwnHLRDeviationAngle(the_anAngle,the_aPreviousAngle);
jcas_SetReal(env,anAngle,the_anAngle);
jcas_SetReal(env,aPreviousAngle,the_aPreviousAngle);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetColor_11 (JNIEnv *env, jobject theobj, jshort aColor)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetColor((Quantity_NameOfColor) aColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetColor_12 (JNIEnv *env, jobject theobj, jobject aColor)
{

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
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetColor(*the_aColor);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_UnsetColor (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->UnsetColor();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_SetWidth (JNIEnv *env, jobject theobj, jdouble aValue)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetWidth((Standard_Real) aValue);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_UnsetWidth (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->UnsetWidth();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetMaterial_11 (JNIEnv *env, jobject theobj, jshort aName)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetMaterial((Graphic3d_NameOfMaterial) aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1SetMaterial_12 (JNIEnv *env, jobject theobj, jobject aName)
{

jcas_Locking alock(env);
{
try {
Graphic3d_MaterialAspect* the_aName = (Graphic3d_MaterialAspect*) jcas_GetHandle(env,aName);
if ( the_aName == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aName = new Graphic3d_MaterialAspect ();
 // jcas_SetHandle ( env, aName, the_aName );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetMaterial(*the_aName);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_UnsetMaterial (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->UnsetMaterial();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_SetTransparency (JNIEnv *env, jobject theobj, jdouble aValue)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->SetTransparency((Standard_Real) aValue);

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_UnsetTransparency (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->UnsetTransparency();

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



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_AIS_1Shape_BoundingBox (JNIEnv *env, jobject theobj)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
const Bnd_Box& theret = the_this->BoundingBox();
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Bnd_Box",&theret,0);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1Color_11 (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Color();

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



JNIEXPORT void JNICALL Java_CASCADESamplesJni_AIS_1Shape_AIS_1Shape_1Color_12 (JNIEnv *env, jobject theobj, jobject aColor)
{

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
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
the_this->Color(*the_aColor);

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1Shape_Material (JNIEnv *env, jobject theobj)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Material();

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1Shape_Transparency (JNIEnv *env, jobject theobj)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
Handle(AIS_Shape) the_this = *((Handle(AIS_Shape)*) jcas_GetHandle(env,theobj));
 thejret = the_this->Transparency();

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



JNIEXPORT jshort JNICALL Java_CASCADESamplesJni_AIS_1Shape_SelectionType (JNIEnv *env, jclass, jint aDecompositionMode)
{
jshort thejret;

jcas_Locking alock(env);
{
try {
 thejret = AIS_Shape::SelectionType((Standard_Integer) aDecompositionMode);

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



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_AIS_1Shape_SelectionMode (JNIEnv *env, jclass, jshort aShapeType)
{
jint thejret;

jcas_Locking alock(env);
{
try {
 thejret = AIS_Shape::SelectionMode((TopAbs_ShapeEnum) aShapeType);

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



JNIEXPORT jdouble JNICALL Java_CASCADESamplesJni_AIS_1Shape_GetDeflection (JNIEnv *env, jclass, jobject aShape, jobject aDrawer)
{
jdouble thejret;

jcas_Locking alock(env);
{
try {
TopoDS_Shape* the_aShape = (TopoDS_Shape*) jcas_GetHandle(env,aShape);
if ( the_aShape == NULL ) {

 // The following assumes availability of the default constructor (what may not
 // always be the case). Therefore explicit exception is thrown if the null
 // object has been passed.
 // the_aShape = new TopoDS_Shape ();
 // jcas_SetHandle ( env, aShape, the_aShape );
 jcas_ThrowException (env, "NULL object has been passed while expecting an object manipulated by value");

}  // end if
 Handle( Prs3d_Drawer ) the_aDrawer;
 void*                ptr_aDrawer = jcas_GetHandle(env,aDrawer);
 
 if ( ptr_aDrawer != NULL ) the_aDrawer = *(   (  Handle( Prs3d_Drawer )*  )ptr_aDrawer   );

 thejret = AIS_Shape::GetDeflection(*the_aShape,the_aDrawer);

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
