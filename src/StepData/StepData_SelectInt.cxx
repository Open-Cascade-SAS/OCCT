// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_SelectInt.ixx>

StepData_SelectInt::StepData_SelectInt  ()    {  thekind = 0;  theval = 0;  }

Standard_Integer  StepData_SelectInt::Kind () const    {  return thekind;  }
void  StepData_SelectInt::SetKind (const Standard_Integer kind)    {  thekind = kind;  }

Standard_Integer  StepData_SelectInt::Int () const    {  return theval;  }
void  StepData_SelectInt::SetInt (const Standard_Integer val)    {  theval = val;  }
