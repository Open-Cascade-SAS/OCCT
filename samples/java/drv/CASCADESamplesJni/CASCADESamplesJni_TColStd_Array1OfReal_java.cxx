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

#include <CASCADESamplesJni_TColStd_Array1OfReal.h>
#include <TColStd_Array1OfReal.hxx>
#include <jcas.hxx>
#include <stdlib.h>
#include <Standard_ErrorHandler.hxx>
#include <Standard_SStream.hxx>



extern "C" {


JNIEXPORT void JNICALL Java_CASCADESamplesJni_TColStd_1Array1OfReal_FinalizeValue(JNIEnv *, jclass, jlong theid)
{
if (theid) {
  TColStd_Array1OfReal* theobj = (TColStd_Array1OfReal*) theid;
  delete theobj;
}
}


}
