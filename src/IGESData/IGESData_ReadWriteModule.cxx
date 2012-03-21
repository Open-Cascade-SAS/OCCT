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

#include <IGESData_ReadWriteModule.ixx>
#include <IGESData_IGESType.hxx>
#include <Interface_Macros.hxx>




Standard_Integer  IGESData_ReadWriteModule::CaseNum
  (const Handle(Interface_FileReaderData)& data,
   const Standard_Integer num) const 
{
  IGESData_IGESType DT = GetCasted(IGESData_IGESReaderData,data)->DirType(num);
  return CaseIGES (DT.Type() , DT.Form());
}


void  IGESData_ReadWriteModule::Read
  (const Standard_Integer ,
   const Handle(Interface_FileReaderData)& , const Standard_Integer ,
   Handle(Interface_Check)& , const Handle(Standard_Transient)& ) const 
{
#ifdef DEB
  cout<<"IGESData_ReadWriteModule, Read called"<<endl;
#endif
}
  // IGESReaderTool fait tout
