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


#include <IFSelect_Act.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Act,IFSelect_Activator)

static TCollection_AsciiString thedefgr, thedefil;



    IFSelect_Act::IFSelect_Act
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
    : thename (name) , thehelp (help) , thefunc (func)    {  }

    IFSelect_ReturnStatus  IFSelect_Act::Do
  (const Standard_Integer, const Handle(IFSelect_SessionPilot)& pilot)
{
  if (!thefunc) return IFSelect_RetVoid;
  return thefunc (pilot);
}

    Standard_CString  IFSelect_Act::Help (const Standard_Integer) const
      {  return thehelp.ToCString();  }


    void  IFSelect_Act::SetGroup
  (const Standard_CString group, const Standard_CString file)
{  thedefgr.Clear();  if (group[0] != '\0') thedefgr.AssignCat(group);
   thedefil.Clear();  if (file [0] != '\0') thedefil.AssignCat(file);  }

    void  IFSelect_Act::AddFunc
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
{
  Handle(IFSelect_Act) act = new IFSelect_Act (name,help,func);
  if (thedefgr.Length() > 0) act->SetForGroup (thedefgr.ToCString());
  act->Add    (1,name);
}

    void  IFSelect_Act::AddFSet
  (const Standard_CString name, const Standard_CString help,
   const IFSelect_ActFunc  func)
{
  Handle(IFSelect_Act) act = new IFSelect_Act (name,help,func);
  if (thedefgr.Length() > 0)
    act->SetForGroup (thedefgr.ToCString(),thedefil.ToCString());
  act->AddSet (1,name);
}
