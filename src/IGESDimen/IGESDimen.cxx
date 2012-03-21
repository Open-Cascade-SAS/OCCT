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

#include <IGESDimen.ixx>
#include <IGESDimen_Protocol.hxx>
#include <IGESDimen_GeneralModule.hxx>
#include <IGESDimen_ReadWriteModule.hxx>
#include <IGESDimen_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESGeom.hxx>
#include <IGESGraph.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESDimen_Protocol) protocol;


    void  IGESDimen::Init ()
{
  IGESGeom::Init();
  IGESGraph::Init();
  if (protocol.IsNull()) {
    protocol = new IGESDimen_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESDimen_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESDimen_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESDimen_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESDimen_SpecificModule, protocol);
  }
}

    Handle(IGESDimen_Protocol)  IGESDimen::Protocol ()
{
  return protocol;
}
