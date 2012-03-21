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

#include <IFSelect_Act.ixx>
#include <TCollection_AsciiString.hxx>

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
