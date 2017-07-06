// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <IFSelect_Activator.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Activator,Standard_Transient)

static NCollection_DataMap<TCollection_AsciiString, Standard_Integer> thedico;
static TColStd_SequenceOfInteger   thenums, themodes;
static TColStd_SequenceOfTransient theacts;


    void IFSelect_Activator::Adding
  (const Handle(IFSelect_Activator)& actor,
   const Standard_Integer number,
   const Standard_CString command,
   const Standard_Integer mode)
{
#ifdef OCCT_DEBUG
  if (thedico.IsBound(command)) {
    cout << "****  XSTEP commands, name conflict on " << command << " first defined remains  ****" << endl;
//    throw Standard_DomainError("IFSelect_Activator : Add");
  }
#endif

  thedico.Bind(command, thenums.Length() + 1);

  thenums.Append(number);
  theacts.Append(actor);
  themodes.Append(mode);
}

    void IFSelect_Activator::Add
  (const Standard_Integer number, const Standard_CString command) const
      {  Adding (this,number,command,0);  }

    void IFSelect_Activator::AddSet
  (const Standard_Integer number, const Standard_CString command) const
      {  Adding (this,number,command,1);  }

    void IFSelect_Activator::Remove (const Standard_CString command)
      {  thedico.UnBind(command);  }

    Standard_Boolean IFSelect_Activator::Select
  (const Standard_CString command, Standard_Integer& number,
   Handle(IFSelect_Activator)& actor)
{
  Standard_Integer num;
  if (!thedico.Find(command, num)) return Standard_False;
  number = thenums(num);
  actor = Handle(IFSelect_Activator)::DownCast(theacts(num));
  return Standard_True;
}

    Standard_Integer IFSelect_Activator::Mode
  (const Standard_CString command)
{
  Standard_Integer num;
  if (!thedico.Find(command, num)) return -1;
  return themodes(num);
}


    Handle(TColStd_HSequenceOfAsciiString) IFSelect_Activator::Commands
  (const Standard_Integer mode, const Standard_CString command)
{
  Standard_Integer num;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator iter(thedico);
  Handle(TColStd_HSequenceOfAsciiString) list =
    new  TColStd_HSequenceOfAsciiString();
  for (; iter.More(); iter.Next()) {
    if (!iter.Key().StartsWith(command))
      continue;
    if (mode < 0) {
      DeclareAndCast(IFSelect_Activator,acti,theacts(iter.Value()));
      if (acti.IsNull()) continue;
      if (command[0] == '\0' || !strcmp(command,acti->Group()) )
        list->Append(iter.Key());
    } else {
      num = iter.Value();
      if (themodes(num) == mode) list->Append(iter.Key());
    }
  }
  return list;
}


    IFSelect_Activator::IFSelect_Activator ()
    : thegroup ("XSTEP")    {  }

    void  IFSelect_Activator::SetForGroup
  (const Standard_CString group, const Standard_CString file)
    {  thegroup.Clear();  thegroup.AssignCat (group);
       thefile.Clear();   thefile.AssignCat  (file);   }

    Standard_CString  IFSelect_Activator::Group () const
    {  return thegroup.ToCString();  }

    Standard_CString  IFSelect_Activator::File  () const
    {  return thefile.ToCString();   }
