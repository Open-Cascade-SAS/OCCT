// File:	StepFile_CallFailure.cxx
// Created:	Tue Feb 05 17:46:31 2002
// Author:	Sergey KUUL
//		<skl@polox>

#include <Standard_Failure.hxx>
#include <StepFile_CallFailure.hxx>

void StepFile_CallFailure(char * const message)
{
  Standard_Failure::Raise(message);
}
