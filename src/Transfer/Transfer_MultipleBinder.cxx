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
