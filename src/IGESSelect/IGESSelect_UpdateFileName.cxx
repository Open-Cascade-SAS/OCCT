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

#include <IGESSelect_UpdateFileName.ixx>
#include <IGESData_GlobalSection.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Check.hxx>


IGESSelect_UpdateFileName::IGESSelect_UpdateFileName  ()
    : IGESSelect_ModelModifier (Standard_False)    {  }

    void  IGESSelect_UpdateFileName::Performing
  (IFSelect_ContextModif& ctx, const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& TC) const
{
  if (!ctx.HasFileName()) {
    ctx.CCheck(0)->AddWarning("New File Name unknown, former one is kept");
    return;
  }
  IGESData_GlobalSection GS = target->GlobalSection();
  GS.SetFileName (new TCollection_HAsciiString (ctx.FileName()) );
  target->SetGlobalSection(GS);
  Handle(Interface_Check) check = new Interface_Check;
  target->VerifyCheck(check);
  ctx.AddCheck(check);
}

    TCollection_AsciiString  IGESSelect_UpdateFileName::Label () const
{ return TCollection_AsciiString("Updates IGES File Name to new current one"); }
