// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_ToolDrawingWithRotation.hxx>
#include <IGESDraw_View.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_HArray1OfXY.hxx>
#include <TColStd_HArray1OfReal.hxx>

IGESDraw_ToolDrawingWithRotation::IGESDraw_ToolDrawingWithRotation ()    {  }


void IGESDraw_ToolDrawingWithRotation::ReadOwnParams
  (const Handle(IGESDraw_DrawingWithRotation)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{ 
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer nbval;

  Handle(IGESDraw_HArray1OfViewKindEntity) views;
  Handle(TColgp_HArray1OfXY)               viewOrigins; 
  Handle(TColStd_HArray1OfReal)            orientationAngles; 
  Handle(IGESData_HArray1OfIGESEntity)     annotations; 

  // Reading nbval(Count of Array Views)
  Standard_Boolean st = PR.ReadInteger(PR.Current(), "count of array views", nbval);
  if (st && nbval > 0)
    {
      views             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
      viewOrigins       = new TColgp_HArray1OfXY(1, nbval);
      orientationAngles = new TColStd_HArray1OfReal(1, nbval);

      Handle(IGESData_ViewKindEntity) tempView;
      gp_XY tempXY;
      Standard_Real tempOrient;

      for (Standard_Integer i = 1; i <= nbval; i++)
	{
          // Reading views(HArray1OfView)
          //st = PR.ReadEntity (IR, PR.Current(), "Instance of views",
				//STANDARD_TYPE(IGESData_ViewKindEntity), tempView,Standard_True); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadEntity (IR, PR.Current(), "Instance of views",
			     STANDARD_TYPE(IGESData_ViewKindEntity), tempView,Standard_True))
	    views->SetValue(i, tempView);
	  
          // Reading viewOrigins(HArray1OfXY)
          //st = PR.ReadXY(PR.CurrentList(1, 2), "array viewOrigins", tempXY); //szv#4:S4163:12Mar99 moved in if
	  if (PR.ReadXY(PR.CurrentList(1, 2), "array viewOrigins", tempXY))
	    viewOrigins->SetValue(i, tempXY);
	  
          if (PR.DefinedElseSkip())
	    {
              // Reading orientationAngles(HArray1OfReal)
              //st = PR.ReadReal(PR.Current(), "array viewOrigins", tempOrient); //szv#4:S4163:12Mar99 moved in if
	      if (PR.ReadReal(PR.Current(), "array viewOrigins", tempOrient))
		orientationAngles->SetValue(i, tempOrient);
	    }
          else  orientationAngles->SetValue(i, 0.0); // Default Value
	}
    }
  else if (nbval <= 0)
    PR.AddFail("Count of view entities : Not Positive");

  // Reading nbval(No. of Annotation Entities)
  //st = PR.ReadInteger(PR.Current(), "Count of array of Annotation entities", nbval); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(), "Count of array of Annotation entities", nbval)) {
    if ( nbval > 0 )
      PR.ReadEnts (IR,PR.CurrentList(nbval), "Annotation Entities", annotations); //szv#4:S4163:12Mar99 `st=` not needed
/*
      {
	// Reading annotations(HArray1OfIGESEntity)
	annotations = new IGESData_HArray1OfIGESEntity(1, nbval);
	Handle(IGESData_IGESEntity) tempAnnotation;
	for (Standard_Integer i = 1; i <= nbval; i++)
          {
	    st = PR.ReadEntity
	      (IR, PR.Current(), "annotation entity", tempAnnotation,Standard_True);
	    if (st) annotations->SetValue(i, tempAnnotation);
          }
      }
*/
    else if (nbval < 0)
      PR.AddFail("Count of Annotation entities : Less than zero");
  }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init (views, viewOrigins, orientationAngles, annotations);
}

void IGESDraw_ToolDrawingWithRotation::WriteOwnParams
  (const Handle(IGESDraw_DrawingWithRotation)& ent, IGESData_IGESWriter& IW)  const
{ 
  Standard_Integer Up  = ent->NbViews();
  IW.Send( Up );
  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for ( i = 1; i <= Up; i++)
    {
      IW.Send( ent->ViewItem(i) );
      IW.Send( (ent->ViewOrigin(i)).X() );
      IW.Send( (ent->ViewOrigin(i)).Y() );
      IW.Send( ent->OrientationAngle(i) );
    }

  Up  = ent->NbAnnotations();
  IW.Send( Up );
  for ( i = 1; i <= Up; i++)
    IW.Send( ent->Annotation(i) );
}

void  IGESDraw_ToolDrawingWithRotation::OwnShared
  (const Handle(IGESDraw_DrawingWithRotation)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer Up  = ent->NbViews();
  Standard_Integer i; // svv Jan 10 2000 : porting on DEC
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( ent->ViewItem(i) );
  Up  = ent->NbAnnotations();
  for ( i = 1; i <= Up; i++)
    iter.GetOneItem( ent->Annotation(i) );
}

void IGESDraw_ToolDrawingWithRotation::OwnCopy
  (const Handle(IGESDraw_DrawingWithRotation)& another,
   const Handle(IGESDraw_DrawingWithRotation)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer                         nbanot;
  Standard_Integer                         nbval;
  Handle(IGESDraw_HArray1OfViewKindEntity) views;
  Handle(TColgp_HArray1OfXY)               viewOrigins;
  Handle(TColStd_HArray1OfReal)            orientationAngles;
  Handle(IGESData_HArray1OfIGESEntity)     annotations;
 
  nbanot            = another->NbAnnotations();
  nbval             = another->NbViews();
  views             = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
  viewOrigins       = new TColgp_HArray1OfXY(1, nbval);
  orientationAngles = new TColStd_HArray1OfReal(1, nbval);
 
  if ( nbanot > 0 )
    {
      annotations = new IGESData_HArray1OfIGESEntity(1, nbanot);
      for (Standard_Integer i = 1; i <= nbanot; i++)
	{
          DeclareAndCast(IGESData_IGESEntity, tempAnnotation, 
                         TC.Transferred(another->Annotation(i)));
          annotations->SetValue( i, tempAnnotation );
	}
    }
 
  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      DeclareAndCast(IGESData_ViewKindEntity, tempView,
                     TC.Transferred(another->ViewItem(i)));
      views->SetValue( i, tempView );
 
      viewOrigins->SetValue( i, (another->ViewOrigin(i)).XY() );
 
      orientationAngles->SetValue( i, another->OrientationAngle(i) );
    }

  ent->Init(views, viewOrigins,orientationAngles, annotations);
}

Standard_Boolean IGESDraw_ToolDrawingWithRotation::OwnCorrect
  (const Handle(IGESDraw_DrawingWithRotation)& ent )  const
{
//  Vues vides : les supprimer
  Standard_Integer i, nb = ent->NbViews();
  Standard_Integer nbtrue = nb;
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_ViewKindEntity) val = ent->ViewItem(i);
    if (val.IsNull()) nbtrue --;
    else if (val->TypeNumber() == 0) nbtrue --;
  }
  if (nbtrue == nb) return Standard_False;
  Handle(IGESDraw_HArray1OfViewKindEntity)  views;
  Handle(TColgp_HArray1OfXY)                viewOrigins; 
  Handle(TColStd_HArray1OfReal)             orientationAngles;
  if (nbtrue > 0) {
    views = new IGESDraw_HArray1OfViewKindEntity (1, nbtrue);
    viewOrigins = new TColgp_HArray1OfXY(1, nbtrue);
    orientationAngles = new TColStd_HArray1OfReal(1, nbtrue);
  }
  nbtrue = 0;
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_ViewKindEntity) val = ent->ViewItem(i);
    if (val.IsNull()) continue;
    else if (val->TypeNumber() == 0) continue;
    nbtrue ++;
    views->SetValue (nbtrue,val);
    viewOrigins->SetValue (nbtrue, ent->ViewOrigin(i).XY() );
    orientationAngles->SetValue (nbtrue,ent->OrientationAngle(i));
  }

//  Ne pas oublier les annotations ...
  Standard_Integer  nbanot = ent->NbAnnotations();
  Handle(IGESData_HArray1OfIGESEntity)      annotations =
    new IGESData_HArray1OfIGESEntity(1, nbanot);
  for (i = 1; i <= nbanot; i ++)  annotations->SetValue (i,ent->Annotation(i));

  ent->Init(views, viewOrigins,orientationAngles, annotations);
  return Standard_True;
}

IGESData_DirChecker IGESDraw_ToolDrawingWithRotation::DirChecker
  (const Handle(IGESDraw_DrawingWithRotation)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (404, 1);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefVoid);
  DC.LineWeight(IGESData_DefVoid);
  DC.Color(IGESData_DefVoid);
  DC.BlankStatusIgnored();
  DC.SubordinateStatusRequired(0);
  DC.UseFlagRequired(1);
  DC.HierarchyStatusIgnored();
  return DC;
}

void IGESDraw_ToolDrawingWithRotation::OwnCheck
  (const Handle(IGESDraw_DrawingWithRotation)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{
  Standard_Boolean ianul = Standard_False;
  Standard_Integer i, nb = ent->NbViews();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_ViewKindEntity) tempView = ent->ViewItem(i);
    if (tempView.IsNull()) ianul = Standard_True;
    else if (tempView->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one View is Null");
      break;
    }
  }
  nb = ent->NbAnnotations();
  for (i = 1; i <= nb; i ++) {
    Handle(IGESData_IGESEntity) ann = ent->Annotation(i);
    if (ann.IsNull()) ianul = Standard_True;
    else if (ann->TypeNumber() == 0) ianul = Standard_True;
    if (ianul) {
      ach->AddWarning ("At least one Annotation is Null");
      break;
    }
  }
}

void IGESDraw_ToolDrawingWithRotation::OwnDump
  (const Handle(IGESDraw_DrawingWithRotation)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_DrawingWithRotation" << endl;

  S << "View Entities            : " << endl
    << "Transformed View Origins : " << endl
    << "Orientation Angles : ";
  S << "Count = "      << ent->NbViews() << endl;

  if (level > 4)    // Level = 4 : no Dump. Level = 5 & 6 have same Dump
    {
      Standard_Integer I;
      Standard_Integer up  = ent->NbViews();
      for (I = 1; I <= up; I++)
	{
	  S << "[" << I << "]:" << endl;
	  S << "View Entity : ";
	  dumper.Dump (ent->ViewItem(I),S, sublevel);
	  S << endl;
	  S << "Transformed View Origin : ";
	  IGESData_DumpXY(S, ent->ViewOrigin(I));
	  S << "  Orientation Angle : " << ent->OrientationAngle(I) << endl;
	}
    }
  S << "Annotation Entities : ";
  IGESData_DumpEntities(S,dumper ,level,1, ent->NbAnnotations(),ent->Annotation);
  S << endl;
}
