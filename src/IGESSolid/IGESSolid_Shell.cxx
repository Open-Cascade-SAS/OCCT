//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Shell.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_Shell.ixx>


    IGESSolid_Shell::IGESSolid_Shell ()    {  }


    void  IGESSolid_Shell::Init
  (const Handle(IGESSolid_HArray1OfFace)& Faces,
   const Handle(TColStd_HArray1OfInteger)& Orient)
{
  if (Faces->Lower()  != 1 || Orient->Lower() != 1 ||
      Faces->Length() != Orient->Length())
    Standard_DimensionError::Raise("IGESSolid_Shell : Init");

  theFaces = Faces;
  theOrientation = Orient;
  InitTypeAndForm(514,1);
}

    Standard_Boolean  IGESSolid_Shell::IsClosed () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_Shell::SetClosed (const Standard_Boolean closed)
      {  InitTypeAndForm(514, (closed ? 1 : 2));  }


    Standard_Integer  IGESSolid_Shell::NbFaces () const
{
  return theFaces->Length();
}

    Handle(IGESSolid_Face) IGESSolid_Shell::Face (const Standard_Integer Index) const
{
  return theFaces->Value(Index);
}

    Standard_Boolean IGESSolid_Shell::Orientation
  (const Standard_Integer Index) const
{
  return (theOrientation->Value(Index) != 0);
}
