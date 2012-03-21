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

#include <StepData_DescrProtocol.ixx>
#include <StepData_DescrGeneral.hxx>
#include <StepData_DescrReadWrite.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <StepData_WriterLib.hxx>


StepData_DescrProtocol::StepData_DescrProtocol ()    {  }

    void  StepData_DescrProtocol::SetSchemaName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  }

    void  StepData_DescrProtocol::LibRecord () const
{
  if (!HasDescr()) return;  // rien a recorder dans la lib ?
  Handle(StepData_DescrGeneral)   gen = new StepData_DescrGeneral   (this);
  Handle(StepData_DescrReadWrite) rwm = new StepData_DescrReadWrite (this);
  Interface_GeneralLib::SetGlobal (gen,this);
  Interface_ReaderLib::SetGlobal   (rwm,this);
  StepData_WriterLib::SetGlobal   (rwm,this);
}

    Standard_CString  StepData_DescrProtocol::SchemaName () const
      {  return thename.ToCString();  }
