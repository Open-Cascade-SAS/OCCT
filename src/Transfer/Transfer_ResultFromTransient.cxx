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

#include <Transfer_ResultFromTransient.ixx>
#include <TColStd_HArray1OfInteger.hxx>


static Handle(Interface_Check) voidcheck = new Interface_Check;

    Transfer_ResultFromTransient::Transfer_ResultFromTransient ()    {  }

    void  Transfer_ResultFromTransient::SetStart
  (const Handle(Standard_Transient)& start)
      {  thestart  = start;   }

    void  Transfer_ResultFromTransient::SetBinder
  (const Handle(Transfer_Binder)& binder)
      {  thebinder = binder;  }

    Handle(Standard_Transient)  Transfer_ResultFromTransient::Start  () const
      {  return thestart;   }

    Handle(Transfer_Binder)     Transfer_ResultFromTransient::Binder () const
      {  return thebinder;  }

    Standard_Boolean  Transfer_ResultFromTransient::HasResult   () const
{  return (thebinder.IsNull() ? Standard_False : thebinder->HasResult());  }


    const Handle(Interface_Check)  Transfer_ResultFromTransient::Check () const
{
  if (thebinder.IsNull()) return voidcheck;
  return thebinder->Check();
}

    Interface_CheckStatus  Transfer_ResultFromTransient::CheckStatus () const
{
  if (thebinder.IsNull()) return Interface_CheckOK;
  const Handle(Interface_Check) ach = thebinder->Check();
  return ach->Status();
}


Handle(Transfer_ResultFromTransient)  Transfer_ResultFromTransient::ResultFromKey
  (const Handle(Standard_Transient)& key) const
{
  Handle(Transfer_ResultFromTransient) res;
  if (key == thestart) return this;
  Standard_Integer i, nb = NbSubResults();
  for (i = 1; i <= nb; i ++) {
    res = SubResult(i)->ResultFromKey(key);
    if (!res.IsNull()) return res;
  }
  return res;
}

    void  Transfer_ResultFromTransient::FillMap
  (TColStd_IndexedMapOfTransient& map) const
{
  if (thesubs.IsNull()) return;
  Standard_Integer i, nb = thesubs->Length();
  for (i = 1; i <= nb; i ++)  map.Add (thesubs->Value(i));
  for (i = 1; i <= nb; i ++)  SubResult(i)->FillMap(map);
}

//  #####   SUBS   #####

    void  Transfer_ResultFromTransient::ClearSubs ()
      {  thesubs.Nullify();  }

    void  Transfer_ResultFromTransient::AddSubResult
  (const Handle(Transfer_ResultFromTransient)& sub)
{
  if (sub.IsNull()) return;
  if (thesubs.IsNull()) thesubs = new TColStd_HSequenceOfTransient();
  thesubs->Append (sub);
}

    Standard_Integer  Transfer_ResultFromTransient::NbSubResults () const
      {  return (thesubs.IsNull() ? 0 : thesubs->Length());  }

    Handle(Transfer_ResultFromTransient)  Transfer_ResultFromTransient::SubResult
  (const Standard_Integer num) const
{
  Handle(Transfer_ResultFromTransient) sub;
  if (thesubs.IsNull()) return sub;
  if (num < 1 || num > thesubs->Length()) return sub;
  return Handle(Transfer_ResultFromTransient)::DownCast(thesubs->Value(num));
}

    void  Transfer_ResultFromTransient::Fill
  (const Handle(Transfer_TransientProcess)& /*TP*/)
{
  //abv: WARNING: to be removed (scopes)
  return;
}

    void  Transfer_ResultFromTransient::Strip ()
{
  //abv: WARNING: to be removed (scopes)
}

    void  Transfer_ResultFromTransient::FillBack
  (const Handle(Transfer_TransientProcess)& /*TP*/) const
{
  //abv: WARNING: to be removed (scopes)
}
