// File:	TopClass.cxx
// Created:	Thu Sep 14 14:35:43 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>

#include <Standard_Type.hxx>

// trace functions called in TopClass_FaceClassifier.gxx

static Standard_Boolean TopClass_traceFC = Standard_False;
Standard_EXPORT void TopClass_SettraceFC(const Standard_Boolean b) { TopClass_traceFC = b; }
Standard_EXPORT Standard_Boolean TopClass_GettraceFC() { return TopClass_traceFC; }

