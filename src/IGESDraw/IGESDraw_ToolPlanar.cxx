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

#include <IGESDraw_ToolPlanar.ixx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_TransformationMatrix.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_Dump.hxx>
#include <Interface_Macros.hxx>


IGESDraw_ToolPlanar::IGESDraw_ToolPlanar ()    {  }


void IGESDraw_ToolPlanar::ReadOwnParams
  (const Handle(IGESDraw_Planar)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  Standard_Boolean                          st;
  Standard_Integer                          nbval;

  Standard_Integer                          nbMatrices; 
  Handle(IGESGeom_TransformationMatrix)     transformationMatrix; 
  Handle(IGESData_HArray1OfIGESEntity) entities; 

  // Reading nbMatrices(Integer)
  st = PR.ReadInteger(PR.Current(), "No. of Transformation matrices", nbMatrices);
  if ( nbMatrices != 1 )
    PR.AddFail("No. of Transformation matrices != 1");

  // Reading nbval(Integer)
  st = PR.ReadInteger(PR.Current(), "No. of Entities in this plane", nbval);
  if (!st) nbval = 0; //szv#4:S4163:12Mar99 was bug: `nbval == 0`
  if (nbval <= 0) PR.AddFail ("No. of Entities in this plane : Not Positive");

  // Reading transformationMatrix(Instance of TransformationMatrix or Null)
  st = PR.ReadEntity(IR,PR.Current(), "Instance of TransformationMatrix",
		     STANDARD_TYPE(IGESGeom_TransformationMatrix), transformationMatrix,
		     Standard_True);

  if (nbval > 0)
    st = PR.ReadEnts (IR, PR.CurrentList(nbval), "Planar Entities", entities);
/*
    {
      entities = new IGESData_HArray1OfIGESEntity(1, nbval);
      // Reading entities(HArray1OfIGESEntity)
      Handle(IGESData_IGESEntity) tempEntity;
      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          st = PR.ReadEntity(IR, PR.Current(), "Plane entity", tempEntity);
	  if (st) entities->SetValue(i, tempEntity);
	}
    }
*/

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(nbMatrices,transformationMatrix, entities);
}

void IGESDraw_ToolPlanar::WriteOwnParams
  (const Handle(IGESDraw_Planar)& ent, IGESData_IGESWriter& IW)  const
{ 
  Standard_Integer Up  = ent->NbEntities();
  IW.Send( ent->NbMatrices() );
  IW.Send( Up );

  IW.Send( ent->TransformMatrix() );

  for ( Standard_Integer i = 1; i <= Up; i++)
    IW.Send( ent->Entity(i) );
}

void  IGESDraw_ToolPlanar::OwnShared
  (const Handle(IGESDraw_Planar)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer Up  = ent->NbEntities();
  iter.GetOneItem( ent->TransformMatrix() );
  for ( Standard_Integer i = 1; i <= Up; i++)
    iter.GetOneItem( ent->Entity(i) );
}

void IGESDraw_ToolPlanar::OwnCopy
  (const Handle(IGESDraw_Planar)& another,
   const Handle(IGESDraw_Planar)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer                          nbval;
  Standard_Integer                          nbMatrices; 
  Handle(IGESData_HArray1OfIGESEntity) entities; 
 
  nbval                = another->NbEntities();
  nbMatrices           = another->NbMatrices();
  DeclareAndCast(IGESGeom_TransformationMatrix, transformationMatrix, 
                 TC.Transferred(another->TransformMatrix()));

  entities = new IGESData_HArray1OfIGESEntity(1, nbval);
  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      DeclareAndCast(IGESData_IGESEntity, tempEntity, 
                     TC.Transferred(another->Entity(i)));
      entities->SetValue( i, tempEntity );
    }

  ent->Init(nbMatrices, transformationMatrix, entities);
}

Standard_Boolean  IGESDraw_ToolPlanar::OwnCorrect
  (const Handle(IGESDraw_Planar)& ent) const
{
  if (ent->NbMatrices() == 1) return Standard_False;
//  Forcer NbMNatrices a 1 -> Reconstruire
  Standard_Integer nb = ent->NbEntities();
  Handle(IGESData_HArray1OfIGESEntity) ents =
    new IGESData_HArray1OfIGESEntity(1,nb);
  for (Standard_Integer i = 1; i <= nb; i ++)
    ents->SetValue(i,ent->Entity(i));
  ent->Init (1,ent->TransformMatrix(),ents);
  return Standard_True;
}

IGESData_DirChecker IGESDraw_ToolPlanar::DirChecker
  (const Handle(IGESDraw_Planar)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (402, 16);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.UseFlagRequired(5);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_ToolPlanar::OwnCheck
  (const Handle(IGESDraw_Planar)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  if ( ent->NbMatrices() != 1 )
    ach->AddFail("No. of Transformation matrices : Value != 1");
}

void IGESDraw_ToolPlanar::OwnDump
  (const Handle(IGESDraw_Planar)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_Planar" << endl;

  S << "No. of Transformation Matrices : " << ent->NbMatrices() << "  ";
  S << "i.e. : ";
  if ( ent->TransformMatrix().IsNull() )
    S << "Null Handle";
  else
    dumper.OwnDump (ent->TransformMatrix(),S, sublevel);
  S << endl;
  S << "Array of Entities on the specified plane : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbEntities(),ent->Entity);
  S << endl;
}
