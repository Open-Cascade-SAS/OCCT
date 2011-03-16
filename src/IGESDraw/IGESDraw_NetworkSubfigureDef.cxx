//--------------------------------------------------------------------
//
//  File Name : IGESDraw_NetworkSubfigureDef.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDraw_NetworkSubfigureDef.ixx>
#include <Standard_DimensionMismatch.hxx>


    IGESDraw_NetworkSubfigureDef::IGESDraw_NetworkSubfigureDef ()    {  }


    void IGESDraw_NetworkSubfigureDef::Init
  (const Standard_Integer                       aDepth,
   const Handle(TCollection_HAsciiString)&      aName,
   const Handle(IGESData_HArray1OfIGESEntity)&  allEntities,
   const Standard_Integer                       aTypeFlag,
   const Handle(TCollection_HAsciiString)&      aDesignator,
   const Handle(IGESGraph_TextDisplayTemplate)& aTemplate,
   const Handle(IGESDraw_HArray1OfConnectPoint)& allPointEntities)
{
  if (!allPointEntities.IsNull())
    if (allPointEntities->Lower() != 1 || allEntities->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_NetworkSubfigureDef : Init");
  theDepth              = aDepth;
  theName               = aName;
  theEntities           = allEntities;
  theTypeFlag           = aTypeFlag;
  theDesignator         = aDesignator;
  theDesignatorTemplate = aTemplate;
  thePointEntities      = allPointEntities;
  InitTypeAndForm(320,0);
}

    Standard_Integer IGESDraw_NetworkSubfigureDef::Depth () const
{
  return theDepth;
}

    Handle(TCollection_HAsciiString) IGESDraw_NetworkSubfigureDef::Name () const
{
  return theName;
}

    Standard_Integer IGESDraw_NetworkSubfigureDef::NbEntities () const
{
  return theEntities->Length();
}

    Handle(IGESData_IGESEntity) IGESDraw_NetworkSubfigureDef::Entity
  (const Standard_Integer Index) const
{
  return theEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

    Standard_Integer IGESDraw_NetworkSubfigureDef::TypeFlag () const
{
  return theTypeFlag;
}

    Handle(TCollection_HAsciiString) IGESDraw_NetworkSubfigureDef::Designator
  () const
{
  return theDesignator;
}

    Standard_Boolean IGESDraw_NetworkSubfigureDef::HasDesignatorTemplate () const
{
  return (! theDesignatorTemplate.IsNull());
}

    Handle(IGESGraph_TextDisplayTemplate)
    IGESDraw_NetworkSubfigureDef::DesignatorTemplate () const
{
  return theDesignatorTemplate;
}

    Standard_Integer IGESDraw_NetworkSubfigureDef::NbPointEntities () const
{
  return (thePointEntities.IsNull() ? 0 : thePointEntities->Length());
}

    Standard_Boolean IGESDraw_NetworkSubfigureDef::HasPointEntity
  (const Standard_Integer Index) const
{
  if (thePointEntities.IsNull()) return Standard_False;
  return (! thePointEntities->Value(Index).IsNull());
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

    Handle(IGESDraw_ConnectPoint) IGESDraw_NetworkSubfigureDef::PointEntity
  (const Standard_Integer Index) const
{
  return thePointEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}
