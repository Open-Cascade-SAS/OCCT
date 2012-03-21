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

#include <IGESDefs.ixx>
#include <IGESDefs_Protocol.hxx>
#include <IGESDefs_GeneralModule.hxx>
#include <IGESDefs_ReadWriteModule.hxx>
#include <IGESDefs_SpecificModule.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_ReaderLib.hxx>
#include <IGESData_WriterLib.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESGraph.hxx>

//  Ancillary data to work on a Package of IGES Entities with a Protocol
//  (Modules are created and loaded in appropriate libraries, once by Init)

static Handle(IGESDefs_Protocol) protocol;


    void  IGESDefs::Init ()
{
  IGESGraph::Init();
  if (protocol.IsNull()) {
    protocol = new IGESDefs_Protocol;
    Interface_GeneralLib::SetGlobal (new IGESDefs_GeneralModule,  protocol);
    Interface_ReaderLib::SetGlobal  (new IGESDefs_ReadWriteModule,protocol);
    IGESData_WriterLib::SetGlobal   (new IGESDefs_ReadWriteModule,protocol);
    IGESData_SpecificLib::SetGlobal (new IGESDefs_SpecificModule, protocol);
  }
}

    Handle(IGESDefs_Protocol)  IGESDefs::Protocol ()
{
  return protocol;
}
