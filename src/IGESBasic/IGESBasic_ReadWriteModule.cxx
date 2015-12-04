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


#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ExternalRefFileName.hxx>
#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_Hierarchy.hxx>
#include <IGESBasic_Name.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_ReadWriteModule.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESBasic_ToolAssocGroupType.hxx>
#include <IGESBasic_ToolExternalReferenceFile.hxx>
#include <IGESBasic_ToolExternalRefFile.hxx>
#include <IGESBasic_ToolExternalRefFileIndex.hxx>
#include <IGESBasic_ToolExternalRefFileName.hxx>
#include <IGESBasic_ToolExternalRefLibName.hxx>
#include <IGESBasic_ToolExternalRefName.hxx>
#include <IGESBasic_ToolGroup.hxx>
#include <IGESBasic_ToolGroupWithoutBackP.hxx>
#include <IGESBasic_ToolHierarchy.hxx>
#include <IGESBasic_ToolName.hxx>
#include <IGESBasic_ToolOrderedGroup.hxx>
#include <IGESBasic_ToolOrderedGroupWithoutBackP.hxx>
#include <IGESBasic_ToolSingleParent.hxx>
#include <IGESBasic_ToolSingularSubfigure.hxx>
#include <IGESBasic_ToolSubfigureDef.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamReader.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_ReadWriteModule,IGESData_ReadWriteModule)

//  Each Module is attached to a Protocol : it must interprete Case Numbers
//  (arguments <CN> of various methods) in accordance to values returned by
//  the method TypeNumber from this Protocol
IGESBasic_ReadWriteModule::IGESBasic_ReadWriteModule ()    {  }


    Standard_Integer  IGESBasic_ReadWriteModule::CaseIGES
  (const Standard_Integer typenum, const Standard_Integer formnum) const 
{
  switch (typenum) {
    case 308 : return 16;
    case 402 :
      switch (formnum) {
        case  1 : return  8;
        case  7 : return  9;
        case  9 : return 14;
        case 12 : return  3;
        case 14 : return 12;
        case 15 : return 13;
	default : break;
      }
      break;
    case 406 :
      switch (formnum) {
        case 10 : return 10;
        case 12 : return  7;
        case 15 : return 11;
        case 23 : return  1;
	default : break;
      }
      break;
    case 408 : return 15;
    case 416 :
      switch (formnum) {
	case  0 : return  4;
	case  1 : return  2;
	case  2 : return  4;
	case  3 : return  6;
	case  4 : return  5;
	default : break;
      }
      break;
    default : break;
  }
  return 0;
}


    void  IGESBasic_ReadWriteModule::ReadOwnParams
  (const Standard_Integer CN, const Handle(IGESData_IGESEntity)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const 
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESBasic_AssocGroupType,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolAssocGroupType tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESBasic_ExternalRefFile,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFile tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESBasic_ExternalRefFileIndex,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileIndex tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESBasic_ExternalRefFileName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileName tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESBasic_ExternalRefLibName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefLibName tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESBasic_ExternalRefName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefName tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESBasic_ExternalReferenceFile,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalReferenceFile tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESBasic_Group,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroup tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroupWithoutBackP tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESBasic_Hierarchy,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolHierarchy tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESBasic_Name,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolName tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESBasic_OrderedGroup,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroup tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroupWithoutBackP tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESBasic_SingleParent,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingleParent tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESBasic_SingularSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingularSubfigure tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESBasic_SubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSubfigureDef tool;
      tool.ReadOwnParams(anent,IR,PR);
    }
      break;
    default : break;
  }
}


    void  IGESBasic_ReadWriteModule::WriteOwnParams
  (const Standard_Integer CN,  const Handle(IGESData_IGESEntity)& ent,
   IGESData_IGESWriter& IW) const
{
  switch (CN) {
    case  1 : {
      DeclareAndCast(IGESBasic_AssocGroupType,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolAssocGroupType tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  2 : {
      DeclareAndCast(IGESBasic_ExternalRefFile,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFile tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  3 : {
      DeclareAndCast(IGESBasic_ExternalRefFileIndex,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileIndex tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  4 : {
      DeclareAndCast(IGESBasic_ExternalRefFileName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefFileName tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  5 : {
      DeclareAndCast(IGESBasic_ExternalRefLibName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefLibName tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  6 : {
      DeclareAndCast(IGESBasic_ExternalRefName,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalRefName tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  7 : {
      DeclareAndCast(IGESBasic_ExternalReferenceFile,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolExternalReferenceFile tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  8 : {
      DeclareAndCast(IGESBasic_Group,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroup tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case  9 : {
      DeclareAndCast(IGESBasic_GroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolGroupWithoutBackP tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 10 : {
      DeclareAndCast(IGESBasic_Hierarchy,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolHierarchy tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 11 : {
      DeclareAndCast(IGESBasic_Name,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolName tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 12 : {
      DeclareAndCast(IGESBasic_OrderedGroup,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroup tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 13 : {
      DeclareAndCast(IGESBasic_OrderedGroupWithoutBackP,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolOrderedGroupWithoutBackP tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 14 : {
      DeclareAndCast(IGESBasic_SingleParent,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingleParent tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 15 : {
      DeclareAndCast(IGESBasic_SingularSubfigure,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSingularSubfigure tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    case 16 : {
      DeclareAndCast(IGESBasic_SubfigureDef,anent,ent);
      if (anent.IsNull()) return;
      IGESBasic_ToolSubfigureDef tool;
      tool.WriteOwnParams(anent,IW);
    }
      break;
    default : break;
  }
}
