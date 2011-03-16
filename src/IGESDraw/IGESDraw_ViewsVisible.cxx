//--------------------------------------------------------------------
//
//  File Name : IGESDraw_ViewsVisible.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDraw_ViewsVisible.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESDraw_ViewsVisible::IGESDraw_ViewsVisible ()    {  }


// This class inherits from IGESData_ViewKindEntity

    void IGESDraw_ViewsVisible::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViewEntities,
   const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayEntity)
{
  if (!allDisplayEntity.IsNull())
    if (allDisplayEntity->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_ViewsVisible : Init");
  if (!allViewEntities.IsNull())
    if (allViewEntities->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_ViewsVisible : Init");
  theViewEntities  = allViewEntities;
  theDisplayEntity = allDisplayEntity;
  InitTypeAndForm(402,3);
}

    void IGESDraw_ViewsVisible::InitImplied
  (const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayEntity)
{
  if (!allDisplayEntity.IsNull())
    if (allDisplayEntity->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_ViewsVisible : InitImplied");
  theDisplayEntity = allDisplayEntity;
}

    Standard_Boolean IGESDraw_ViewsVisible::IsSingle () const
{
  return Standard_False;
  // Is Redefined to return FALSE
}

    Standard_Integer IGESDraw_ViewsVisible::NbViews () const
{
  return (theViewEntities.IsNull() ? 0 : theViewEntities->Length());
}

    Standard_Integer IGESDraw_ViewsVisible::NbDisplayedEntities () const
{
  return (theDisplayEntity.IsNull() ? 0 : theDisplayEntity->Length());
  // return 0 if HArray1 theDisplayEntity is NULL Handle
}

    Handle(IGESData_ViewKindEntity) IGESDraw_ViewsVisible::ViewItem
  (const Standard_Integer Index) const
{
  return theViewEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

    Handle(IGESData_IGESEntity) IGESDraw_ViewsVisible::DisplayedEntity
  (const Standard_Integer Index) const
{
  return theDisplayEntity->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
  // if (theDisplayEntity.IsNull()) then NoSuchObject Exception is raised
}
