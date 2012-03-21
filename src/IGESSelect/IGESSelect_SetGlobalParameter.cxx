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

#include <IGESSelect_SetGlobalParameter.ixx>
#include <IGESData_GlobalSection.hxx>
#include <Interface_ParamSet.hxx>
#include <Interface_FileParameter.hxx>
#include <stdio.h>
#include <Interface_Check.hxx>


IGESSelect_SetGlobalParameter::IGESSelect_SetGlobalParameter
  (const Standard_Integer numpar)
    : IGESSelect_ModelModifier (Standard_False)
      {  thenum = numpar;  }

    Standard_Integer  IGESSelect_SetGlobalParameter::GlobalNumber () const
      {  return thenum;  }

    void  IGESSelect_SetGlobalParameter::SetValue
  (const Handle(TCollection_HAsciiString)& text)
      {  theval = text;  }

    Handle(TCollection_HAsciiString)  IGESSelect_SetGlobalParameter::Value
  () const
      {  return theval;  }


    void  IGESSelect_SetGlobalParameter::Performing
  (IFSelect_ContextModif& ctx,
   const Handle(IGESData_IGESModel)& target,
   Interface_CopyTool& ) const
{
  if (theval.IsNull()) {
    ctx.CCheck()->AddWarning("Set IGES Global Parameter, no value defined, ignored");
    return;
  }
  IGESData_GlobalSection GS = target->GlobalSection();
  Handle(Interface_ParamSet) oldset = GS.Params();
  if (thenum <= 0 || thenum > oldset->NbParams()) {
    char mess[80];
    sprintf(mess,"Set IGES Global Parameter : Number %d incorrect",thenum);
    ctx.CCheck()->AddFail(mess);
    return;
  }
  Interface_FileParameter& FP = oldset->ChangeParam (thenum);
  FP.Init (theval->ToCString(),FP.ParamType());
  Handle(Interface_Check) check = new Interface_Check;
  GS.Init (oldset,check);
  ctx.AddCheck(check);
  if (!check->HasFailed()) target->SetGlobalSection(GS);
}

    TCollection_AsciiString  IGESSelect_SetGlobalParameter::Label () const
{
  char mess[80];
  if (theval.IsNull()) sprintf(mess,"Set IGES Global Parameter (undefined)");
  else sprintf(mess,"Set IGES Global Parameter Number %d to %s",
	       thenum,theval->ToCString());
  return TCollection_AsciiString (mess);
}
