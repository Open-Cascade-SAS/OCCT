//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.

#include <CASCADESamplesJni_Aspect_GenericColorMap.h>
#include <Aspect_GenericColorMap.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>

#include <Aspect_ColorMapEntry.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_Color.hxx>


extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_Aspect_1GenericColorMap_1Create_10 (JNIEnv *env, jobject theobj)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_GenericColorMap)* theret = new Handle(Aspect_GenericColorMap);
*theret = new Aspect_GenericColorMap();
jcas_SetHandle(env,theobj,theret);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_Aspect_1GenericColorMap_1AddEntry_11 (JNIEnv *env, jobject theobj, jobject AnEntry)
{

jcas_Locking alock(env);
{
try {
Aspect_ColorMapEntry* the_AnEntry = (Aspect_ColorMapEntry*) jcas_GetHandle(env,AnEntry);
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
the_this->AddEntry(*the_AnEntry);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_Aspect_1GenericColorMap_1AddEntry_12 (JNIEnv *env, jobject theobj, jobject aColor)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->AddEntry(*the_aColor);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT void JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_RemoveEntry (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{

jcas_Locking alock(env);
{
try {
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
the_this->RemoveEntry((Standard_Integer) AColorMapEntryIndex);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();

}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_FindColorMapIndex (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->FindColorMapIndex((Standard_Integer) AColorMapEntryIndex);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_FindEntry (JNIEnv *env, jobject theobj, jint AColorMapEntryIndex)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
const Aspect_ColorMapEntry& theret = the_this->FindEntry((Standard_Integer) AColorMapEntryIndex);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMapEntry",&theret,0);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jint JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_NearestColorMapIndex (JNIEnv *env, jobject theobj, jobject aColor)
{
jint thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
 thejret = the_this->NearestColorMapIndex(*the_aColor);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}



JNIEXPORT jobject JNICALL Java_CASCADESamplesJni_Aspect_1GenericColorMap_NearestEntry (JNIEnv *env, jobject theobj, jobject aColor)
{
jobject thejret;

jcas_Locking alock(env);
{
try {
Quantity_Color* the_aColor = (Quantity_Color*) jcas_GetHandle(env,aColor);
Handle(Aspect_GenericColorMap) the_this = *((Handle(Aspect_GenericColorMap)*) jcas_GetHandle(env,theobj));
const Aspect_ColorMapEntry& theret = the_this->NearestEntry(*the_aColor);
thejret = jcas_CreateObject(env,"CASCADESamplesJni/Aspect_ColorMapEntry",&theret,0);

}
catch (Standard_Failure) {
  Standard_SStream Err;
  Err <<   Standard_Failure::Caught(); 
  Err << (char) 0;
  jcas_ThrowException(env,GetSString(Err));
}
}
alock.Release();
return thejret;
}


}
