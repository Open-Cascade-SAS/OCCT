// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
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
