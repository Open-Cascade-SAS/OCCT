#include <IGESSelect_SelectVisibleStatus.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>



    IGESSelect_SelectVisibleStatus::IGESSelect_SelectVisibleStatus ()    {  }


    Standard_Boolean  IGESSelect_SelectVisibleStatus::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  return (igesent->BlankStatus() == 0);
}

    TCollection_AsciiString  IGESSelect_SelectVisibleStatus::ExtractLabel
  () const
      {  return TCollection_AsciiString ("IGES Entity, Status Visible");  }
