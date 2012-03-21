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

#include <StepData_ReadWriteModule.ixx>
#include <StepData_StepReaderData.hxx>
#include <Interface_Macros.hxx>



Standard_Integer  StepData_ReadWriteModule::CaseNum
  (const Handle(Interface_FileReaderData)& data,
   const Standard_Integer num) const
{
  DeclareAndCast(StepData_StepReaderData,stepdat,data);
  if (stepdat.IsNull()) return 0;
  if (stepdat->IsComplex(num)) {
    TColStd_SequenceOfAsciiString types;
    stepdat->ComplexType (num,types);
    if (types.IsEmpty()) return 0;
    if (types.Length() == 1) return CaseStep (types.Value(1));
    else return CaseStep (types);
  }
  return CaseStep (stepdat->RecordType(num));
}

    Standard_Integer  StepData_ReadWriteModule::CaseStep
  (const TColStd_SequenceOfAsciiString& types) const
      {  return 0;  }        // par defaut

    Standard_Boolean  StepData_ReadWriteModule::IsComplex
  (const Standard_Integer CN) const
      {  return Standard_False;  }  // par defaut

    TCollection_AsciiString  StepData_ReadWriteModule::ShortType
  (const Standard_Integer ) const
      {  return TCollection_AsciiString("");  }  // par defaut vide

    Standard_Boolean  StepData_ReadWriteModule::ComplexType
  (const Standard_Integer ,
   TColStd_SequenceOfAsciiString& ) const
      {  return Standard_False;  }


//=======================================================================
//function : Read
//purpose  : 
//=======================================================================

void StepData_ReadWriteModule::Read(const Standard_Integer CN,
                                    const Handle(Interface_FileReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(Standard_Transient)& ent) const 
{
  DeclareAndCast(StepData_StepReaderData,stepdat,data);
  if (stepdat.IsNull()) return;
  ReadStep (CN,stepdat,num,ach,ent);
}
