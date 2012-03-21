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

#include <IFSelect_IntParam.ixx>
#include <Interface_Static.hxx>


IFSelect_IntParam::IFSelect_IntParam ()
      {  theval = 0;  }

    void  IFSelect_IntParam::SetStaticName (const Standard_CString statname)
      {  thestn.Clear();  thestn.AssignCat (statname);  }

    Standard_Integer  IFSelect_IntParam::Value () const 
{
  if (thestn.Length() == 0) return theval;
  if (!Interface_Static::IsSet(thestn.ToCString()) ) return theval;
  return Interface_Static::IVal(thestn.ToCString());
}

    void  IFSelect_IntParam::SetValue (const Standard_Integer val)
{
  theval = val;
  if (thestn.Length() == 0) return;
  if (!Interface_Static::IsPresent(thestn.ToCString()) ) return;
  Interface_Static::SetIVal (thestn.ToCString(),theval);
}
