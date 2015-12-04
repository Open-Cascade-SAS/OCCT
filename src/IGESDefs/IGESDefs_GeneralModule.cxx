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


#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_AssociativityDef.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <IGESDefs_GeneralModule.hxx>
#include <IGESDefs_GenericData.hxx>
#include <IGESDefs_MacroDef.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_ToolAssociativityDef.hxx>
#include <IGESDefs_ToolAttributeDef.hxx>
#include <IGESDefs_ToolAttributeTable.hxx>
#include <IGESDefs_ToolGenericData.hxx>
#include <IGESDefs_ToolMacroDef.hxx>
#include <IGESDefs_ToolTabularData.hxx>
#include <IGESDefs_ToolUnitsData.hxx>
#include <IGESDefs_UnitsData.hxx>
#include <Interface_Category.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_GeneralModule,IGESData_GeneralModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESDefs_GeneralModule::IGESDefs_GeneralModule ()    {  }


    void  IGESDefs_GeneralModule::OwnSharedCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   Interface_EntityIterator& iter) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDefs_AssociativityDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAssociativityDef tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDefs_AttributeDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAttributeDef tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDefs_AttributeTable,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAttributeTable tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDefs_GenericData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolGenericData tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDefs_MacroDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolMacroDef tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDefs_TabularData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolTabularData tool;
      tool.OwnShared(anent,iter);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDefs_UnitsData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolUnitsData tool;
      tool.OwnShared(anent,iter);
    }
      break;
    default : break;
  }
}


    IGESData_DirChecker  IGESDefs_GeneralModule::DirChecker
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDefs_AssociativityDef,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolAssociativityDef tool;
      return tool.DirChecker(anent);
    }
    case  2 : {
      DeclareAndCast(IGESDefs_AttributeDef,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolAttributeDef tool;
      return tool.DirChecker(anent);
    }
    case  3 : {
      DeclareAndCast(IGESDefs_AttributeTable,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolAttributeTable tool;
      return tool.DirChecker(anent);
    }
    case  4 : {
      DeclareAndCast(IGESDefs_GenericData,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolGenericData tool;
      return tool.DirChecker(anent);
    }
    case  5 : {
      DeclareAndCast(IGESDefs_MacroDef,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolMacroDef tool;
      return tool.DirChecker(anent);
    }
    case  6 : {
      DeclareAndCast(IGESDefs_TabularData,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolTabularData tool;
      return tool.DirChecker(anent);
    }
    case  7 : {
      DeclareAndCast(IGESDefs_UnitsData,anent,ent);
      if (anent.IsNull()) break;
      IGESDefs_ToolUnitsData tool;
      return tool.DirChecker(anent);
    }
    default : break;
  }
  return IGESData_DirChecker();    // by default, no specific criterium
}


    void  IGESDefs_GeneralModule::OwnCheckCase
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Interface_ShareTool& shares, Handle(Interface_Check)& ach) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDefs_AssociativityDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAssociativityDef tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDefs_AttributeDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAttributeDef tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDefs_AttributeTable,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolAttributeTable tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDefs_GenericData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolGenericData tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDefs_MacroDef,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolMacroDef tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDefs_TabularData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolTabularData tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDefs_UnitsData,anent,ent);
      if (anent.IsNull()) return;
      IGESDefs_ToolUnitsData tool;
      tool.OwnCheck(anent,shares,ach);
    }
      break;
    default : break;
  }
}


    Standard_Boolean  IGESDefs_GeneralModule::NewVoid
  (const Standard_Integer CN, Handle(Standard_Transient)& ent) const 
{
  switch (CN) {
    case  1 : ent = new IGESDefs_AssociativityDef;	break;
    case  2 : ent = new IGESDefs_AttributeDef;		break;
    case  3 : ent = new IGESDefs_AttributeTable;	break;
    case  4 : ent = new IGESDefs_GenericData;	break;
    case  5 : ent = new IGESDefs_MacroDef;	break;
    case  6 : ent = new IGESDefs_TabularData;	break;
    case  7 : ent = new IGESDefs_UnitsData;	break;
    default : return Standard_False;    // by default, Failure on Recognize
  }
  return Standard_True;
}


    void  IGESDefs_GeneralModule::OwnCopyCase
  (const Standard_Integer CN,
   const Handle(IGESData_IGESEntity)& entfrom,
   const Handle(IGESData_IGESEntity)& entto,
   Interface_CopyTool& TC) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESDefs_AssociativityDef,enfr,entfrom);
      DeclareAndCast(IGESDefs_AssociativityDef,ento,entto);
      IGESDefs_ToolAssociativityDef tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESDefs_AttributeDef,enfr,entfrom);
      DeclareAndCast(IGESDefs_AttributeDef,ento,entto);
      IGESDefs_ToolAttributeDef tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESDefs_AttributeTable,enfr,entfrom);
      DeclareAndCast(IGESDefs_AttributeTable,ento,entto);
      IGESDefs_ToolAttributeTable tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESDefs_GenericData,enfr,entfrom);
      DeclareAndCast(IGESDefs_GenericData,ento,entto);
      IGESDefs_ToolGenericData tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESDefs_MacroDef,enfr,entfrom);
      DeclareAndCast(IGESDefs_MacroDef,ento,entto);
      IGESDefs_ToolMacroDef tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESDefs_TabularData,enfr,entfrom);
      DeclareAndCast(IGESDefs_TabularData,ento,entto);
      IGESDefs_ToolTabularData tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESDefs_UnitsData,enfr,entfrom);
      DeclareAndCast(IGESDefs_UnitsData,ento,entto);
      IGESDefs_ToolUnitsData tool;
      tool.OwnCopy(enfr,ento,TC);
    }
      break;
    default : break;
  }
}


    Standard_Integer  IGESDefs_GeneralModule::CategoryNumber
  (const Standard_Integer /*CN*/, const Handle(Standard_Transient)& /*ent*/,
   const Interface_ShareTool& ) const
{
  return Interface_Category::Number("Auxiliary");
}
