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

#include <IFSelect_ModifReorder.ixx>
#include <Interface_ShareTool.hxx>
#include <Interface_EntityIterator.hxx>

IFSelect_ModifReorder::IFSelect_ModifReorder (const Standard_Boolean rootlast)
    : IFSelect_Modifier (Standard_True)    {  thertl = rootlast;  }

    void  IFSelect_ModifReorder::Perform
  (IFSelect_ContextModif& ctx, const Handle(Interface_InterfaceModel)& target,
   const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC) const
{
  Interface_ShareTool sht (ctx.OriginalGraph());
  Interface_EntityIterator list = sht.All (ctx.OriginalModel(),thertl);
  target->ClearEntities();
  for (list.Start(); list.More(); list.Next())  target->AddEntity (list.Value());
}

TCollection_AsciiString  IFSelect_ModifReorder::Label () const
{
  Standard_CString astr = (Standard_CString ) ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
  return TCollection_AsciiString( astr ) ;
//    ( thertl ? "Reorder, Roots last" : "Reorder, Roots first");
}
