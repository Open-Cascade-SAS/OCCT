//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SolidAssembly.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SolidAssembly.ixx>


    IGESSolid_SolidAssembly::IGESSolid_SolidAssembly ()    {  }


    void  IGESSolid_SolidAssembly::Init
  (const Handle(IGESData_HArray1OfIGESEntity)& Items,
   const Handle(IGESGeom_HArray1OfTransformationMatrix)& Matrices)
{
  if (Items->Lower()  != 1 || Matrices->Lower() != 1 ||
      Items->Length() != Matrices->Length())
    Standard_DimensionError::Raise("IGESSolid_SolidAssembly : Init");

  theItems    = Items;
  theMatrices = Matrices;
  InitTypeAndForm(184,0);
}

    Standard_Boolean  IGESSolid_SolidAssembly::HasBrep () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_SolidAssembly::SetBrep (const Standard_Boolean hasbrep)
      {  InitTypeAndForm(184, (hasbrep ? 1 : 0));  }


    Standard_Integer  IGESSolid_SolidAssembly::NbItems () const
{
  return theItems->Length();
}

    Handle(IGESData_IGESEntity)  IGESSolid_SolidAssembly::Item
  (const Standard_Integer Index) const
{
  return theItems->Value(Index);
}

    Handle(IGESGeom_TransformationMatrix) IGESSolid_SolidAssembly::TransfMatrix
  (const Standard_Integer Index) const
{
  return theMatrices->Value(Index);
}
