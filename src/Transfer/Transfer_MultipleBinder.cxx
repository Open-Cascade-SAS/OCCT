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


#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Transfer_MultipleBinder.hxx>
#include <Transfer_TransferFailure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Transfer_MultipleBinder,Transfer_Binder)

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
