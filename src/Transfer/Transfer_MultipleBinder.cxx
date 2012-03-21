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

#include <Transfer_MultipleBinder.ixx>
#include <Transfer_TransferFailure.hxx>



// Resultat Multiple
// Possibilite de definir un Resultat Multiple : plusieurs objets resultant
//  d un Transfert, sans pouvoir les distinguer
//  N.B. : Pour l heure, tous Transients (pourra evoluer)



Transfer_MultipleBinder::Transfer_MultipleBinder ()      { }


    Standard_Boolean Transfer_MultipleBinder::IsMultiple () const
{
  if (themulres.IsNull()) return Standard_False;
  return (themulres->Length() != 1);
}

    Handle(Standard_Type) Transfer_MultipleBinder::ResultType () const
      {  return STANDARD_TYPE(Standard_Transient);  }

    Standard_CString Transfer_MultipleBinder::ResultTypeName () const
      {  return "(list)";  }

//  ....        Gestion du Resultat Multiple        ....

    void Transfer_MultipleBinder::AddResult
  (const Handle(Standard_Transient)& res)
{
  if (themulres.IsNull()) themulres = new TColStd_HSequenceOfTransient();
  themulres->Append(res);
}

    Standard_Integer  Transfer_MultipleBinder::NbResults () const
      {  return (themulres.IsNull() ? 0 : themulres->Length());  }

    Handle(Standard_Transient) Transfer_MultipleBinder::ResultValue
  (const Standard_Integer num) const
      {  return themulres->Value(num);  }

    Handle(TColStd_HSequenceOfTransient) Transfer_MultipleBinder::MultipleResult
  () const
{
  if (!themulres.IsNull()) return themulres;
  return new TColStd_HSequenceOfTransient();
}

    void Transfer_MultipleBinder::SetMultipleResult
  (const Handle(TColStd_HSequenceOfTransient)& mulres)
      {  themulres = mulres;  }
