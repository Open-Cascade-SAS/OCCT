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

#include <IGESData_DefaultSpecific.ixx>
#include <IGESData_UndefinedEntity.hxx>
#include <Interface_UndefinedContent.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESData.hxx>
#include <IGESData_Protocol.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>


IGESData_DefaultSpecific::IGESData_DefaultSpecific ()
{  IGESData_SpecificLib::SetGlobal(this, IGESData::Protocol());  }

    void  IGESData_DefaultSpecific::OwnDump
  (const Standard_Integer /*CN*/, const Handle(IGESData_IGESEntity)& ent,
   const IGESData_IGESDumper& dumper, const Handle(Message_Messenger)& S,
   const Standard_Integer /*own*/) const 
{
  DeclareAndCast(IGESData_UndefinedEntity,lent,ent);
  if (lent.IsNull()) return;

  Standard_Integer dstat = lent->DirStatus();
  if (dstat != 0) S
    << " --  Directory Entry Error Status = " << dstat << "  --" << endl;
  Handle(Interface_UndefinedContent) cont = lent->UndefinedContent();
  Standard_Integer nb = cont->NbParams();
  S << " UNDEFINED ENTITY ...\n"<<nb
    <<" Parameters (WARNING : Odd Integer Values Interpreted as Entities)\n";
  for (Standard_Integer i = 1; i <= nb; i ++) {
    Interface_ParamType ptyp = cont->ParamType(i);
    if (ptyp == Interface_ParamVoid) S<<"	["<<i<<":Void]";
    else if (cont->IsParamEntity(i)) {
      DeclareAndCast(IGESData_IGESEntity,anent,cont->ParamEntity(i));
      S<<"	["<<i<<":IGES]=";  
      dumper.PrintDNum(anent,S);
    }
    else {  S<<"	["<<i<<"]=" << cont->ParamValue(i);  }
    if ( i == (i%5)*5) S << endl;
  }
  S << endl;
}
