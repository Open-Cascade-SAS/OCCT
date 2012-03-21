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

#include <IFSelect_WorkLibrary.ixx>
#include <TCollection_HAsciiString.hxx>

#include <Message_Messenger.hxx>
#include <Message.hxx>

//  all deferred but Copy (virtual default)

IFSelect_WorkLibrary::IFSelect_WorkLibrary ()  {  thelevdef = 0;  }

    Standard_Boolean  IFSelect_WorkLibrary::CopyModel
  (const Handle(Interface_InterfaceModel)& /*original*/,
   const Handle(Interface_InterfaceModel)& newmodel,
   const Interface_EntityIterator& list,
   Interface_CopyTool& TC) const
{
  for (list.Start(); list.More(); list.Next())
    TC.TransferEntity (list.Value());

  TC.FillModel(newmodel);

  return Standard_True;
}


    void  IFSelect_WorkLibrary::DumpEntity
  (const Handle(Interface_InterfaceModel)& model,
   const Handle(Interface_Protocol)& protocol,
   const Handle(Standard_Transient)& entity,
   const Handle(Message_Messenger)& S) const
{
  if (thelevhlp.IsNull()) DumpEntity (model,protocol,entity,S,0);
  else                    DumpEntity (model,protocol,entity,S,thelevdef);
}


    void  IFSelect_WorkLibrary::SetDumpLevels
  (const Standard_Integer def, const Standard_Integer max)
{
  thelevdef = def;
  thelevhlp.Nullify();
  if (max >= 0) thelevhlp = new Interface_HArray1OfHAsciiString (0,max);
}

    void  IFSelect_WorkLibrary::DumpLevels
  (Standard_Integer& def, Standard_Integer& max) const
{
  def = thelevdef;
  if (thelevhlp.IsNull()) {  def = 0;  max = -1;  }
  else max = thelevhlp->Upper();
}

    void  IFSelect_WorkLibrary::SetDumpHelp
  (const Standard_Integer level, const Standard_CString help)
{
  if (thelevhlp.IsNull()) return;
  if (level < 0 || level > thelevhlp->Upper()) return;
  Handle(TCollection_HAsciiString) str = new TCollection_HAsciiString (help);
  thelevhlp->SetValue (level,str);
}

    Standard_CString  IFSelect_WorkLibrary::DumpHelp
  (const Standard_Integer level) const
{
  if (thelevhlp.IsNull()) return "";
  if (level < 0 || level > thelevhlp->Upper()) return "";
  Handle(TCollection_HAsciiString) str = thelevhlp->Value (level);
  if (str.IsNull()) return "";
  return str->ToCString();
}
