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

#include <IFSelect_GeneralModifier.ixx>



IFSelect_GeneralModifier::IFSelect_GeneralModifier
  (const Standard_Boolean maychangegraph)    {  thechgr = maychangegraph;  }

    Standard_Boolean  IFSelect_GeneralModifier::MayChangeGraph () const
      {  return thechgr;  }

    void  IFSelect_GeneralModifier::SetDispatch
  (const Handle(IFSelect_Dispatch)& disp)
      {  thedisp = disp;  }

    Handle(IFSelect_Dispatch)  IFSelect_GeneralModifier::Dispatch () const
      {  return thedisp;  }

    Standard_Boolean  IFSelect_GeneralModifier::Applies
  (const Handle(IFSelect_Dispatch)& disp) const
{
  if (thedisp.IsNull()) return Standard_True;
  if (thedisp != disp) return Standard_False;
  return Standard_True;
}


    void  IFSelect_GeneralModifier::SetSelection
  (const Handle(IFSelect_Selection)& sel)
      {  thesel = sel;  }

    void  IFSelect_GeneralModifier::ResetSelection ()
      {  thesel.Nullify();  }

    Standard_Boolean  IFSelect_GeneralModifier::HasSelection () const 
      {  return !thesel.IsNull();  }

    Handle(IFSelect_Selection)  IFSelect_GeneralModifier::Selection () const 
      {  return thesel;  }
