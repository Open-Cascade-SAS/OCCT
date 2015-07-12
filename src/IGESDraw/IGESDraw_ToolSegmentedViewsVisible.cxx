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

#include <IGESBasic_HArray1OfLineFontEntity.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_HArray1OfViewKindEntity.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESDraw_ToolSegmentedViewsVisible.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESGraph_HArray1OfColor.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

IGESDraw_ToolSegmentedViewsVisible::IGESDraw_ToolSegmentedViewsVisible ()  {  }


void IGESDraw_ToolSegmentedViewsVisible::ReadOwnParams
  (const Handle(IGESDraw_SegmentedViewsVisible)& ent,
   const Handle(IGESData_IGESReaderData)& IR, IGESData_ParamReader& PR) const
{
  //Standard_Boolean                              st; //szv#4:S4163:12Mar99 moved down
  Standard_Integer                              nbval;

  Handle(IGESDraw_HArray1OfViewKindEntity) views; 
  Handle(TColStd_HArray1OfReal)             breakpointParameters; 
  Handle(TColStd_HArray1OfInteger)          displayFlags;
  Handle(TColStd_HArray1OfInteger)          colorValues;
  Handle(IGESGraph_HArray1OfColor)          colorDefinitions;
  Handle(TColStd_HArray1OfInteger)          lineFontValues;
  Handle(IGESBasic_HArray1OfLineFontEntity) lineFontDefinitions;
  Handle(TColStd_HArray1OfInteger)          lineWeights;

  // Reading nbval(Integer)
  Standard_Boolean st = PR.ReadInteger( PR.Current(), "No. of View/segment blocks", nbval );
  if (st && nbval > 0) {
    views                = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
    breakpointParameters = new TColStd_HArray1OfReal(1, nbval);
    displayFlags         = new TColStd_HArray1OfInteger(1, nbval);
    colorValues          = new TColStd_HArray1OfInteger(1, nbval);
    colorDefinitions     = new IGESGraph_HArray1OfColor(1, nbval);
    lineFontValues       = new TColStd_HArray1OfInteger(1, nbval);
    lineFontDefinitions  = new IGESBasic_HArray1OfLineFontEntity(1, nbval);
    lineWeights          = new TColStd_HArray1OfInteger(1, nbval);
    
    Handle(IGESData_ViewKindEntity) tempView;
    Standard_Real                   tempBreak;
    Standard_Integer                tempDisplay;
    Standard_Integer                tempColorValue;
    Handle(IGESGraph_Color)         tempColorDef;
    Standard_Integer                tempLineFontValue;
    Handle(IGESData_LineFontEntity) tempLineFontDef;
    Standard_Integer                tempLine;
    
    for (Standard_Integer i = 1; i <= nbval; i++)
      {
	// Reading views(HArray1OfView)
	//st = PR.ReadEntity( IR, PR.Current(), "Instance of views",
			     //STANDARD_TYPE(IGESData_ViewKindEntity), tempView ); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadEntity(IR, PR.Current(), "Instance of views",
			  STANDARD_TYPE(IGESData_ViewKindEntity), tempView))
	  views->SetValue(i, tempView);
	
	// Reading breakpointParameters(HArray1OfReal)
	//st = PR.ReadReal(PR.Current(), "array breakpointParameters", tempBreak ); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadReal(PR.Current(), "array breakpointParameters", tempBreak ))
	  breakpointParameters->SetValue(i, tempBreak);
	
	// Reading displayFlags(HArray1OfInteger)
	//st = PR.ReadInteger( PR.Current(), "array displayFlags", tempDisplay ); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger( PR.Current(), "array displayFlags", tempDisplay ))
	  displayFlags->SetValue(i, tempDisplay);

	Standard_Integer curnum = PR.CurrentNumber();

	//  Reading Color : Value (>0) or Definition (<0 = D.E. Pointer)
	PR.ReadInteger( PR.Current(), "array colorValues", tempColorValue); //szv#4:S4163:12Mar99 `st=` not needed
	if (tempColorValue < 0) {
	  colorValues->SetValue(i, -1);
	  tempColorDef = GetCasted(IGESGraph_Color,PR.ParamEntity(IR,curnum));
	  if (tempColorDef.IsNull()) PR.AddFail
	    ("A Color Definition Entity is incorrect");
	  else  colorDefinitions->SetValue(i, tempColorDef);
	}
	else
	  colorValues->SetValue(i, tempColorValue);

	curnum = PR.CurrentNumber();
	//  Reading Line Font : Value (>0) or Definition (<0 = D.E. Pointer)
	PR.ReadInteger( PR.Current(), "array lineFontValues", tempLineFontValue ); //szv#4:S4163:12Mar99 `st=` not needed
	if (tempLineFontValue < 0 ) {
	  lineFontValues->SetValue(i, -1);
	  tempLineFontDef = GetCasted(IGESData_LineFontEntity,
				      PR.ParamEntity(IR,curnum));
	  if (tempLineFontDef.IsNull()) PR.AddFail
	    ("A Line Font Definition Entity is incorrect");
	  lineFontDefinitions->SetValue(i,tempLineFontDef);
	}
	else
	  lineFontValues->SetValue(i, tempLineFontValue);
	
	// Reading lineWeights(HArray1OfInteger)
	//st = PR.ReadInteger( PR.Current(), "array lineWeights", tempLine ); //szv#4:S4163:12Mar99 moved in if
	if (PR.ReadInteger( PR.Current(), "array lineWeights", tempLine ))
	  lineWeights->SetValue(i, tempLine);
      }
  }
  else
    PR.AddFail("No. of View/segment blocks : Not Positive");

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (views, breakpointParameters, displayFlags,colorValues, colorDefinitions,
     lineFontValues, lineFontDefinitions, lineWeights);
}

void IGESDraw_ToolSegmentedViewsVisible::WriteOwnParams
  (const Handle(IGESDraw_SegmentedViewsVisible)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer Up  = ent->NbSegmentBlocks();
  IW.Send( Up );
  for ( Standard_Integer i = 1; i <= Up; i++)
    {
      IW.Send( ent->ViewItem(i) );
      IW.Send( ent->BreakpointParameter(i) );
      IW.Send( ent->DisplayFlag(i) );

      if (ent->IsColorDefinition(i))
	IW.Send( ent->ColorDefinition(i), Standard_True );     // negative
      else
	IW.Send( ent->ColorValue(i) );

      if (ent->IsFontDefinition(i))
	IW.Send( ent->LineFontDefinition(i), Standard_True );  // negative
      else
	IW.Send( ent->LineFontValue(i) );

      IW.Send( ent->LineWeightItem(i) );
    }
}

void  IGESDraw_ToolSegmentedViewsVisible::OwnShared
  (const Handle(IGESDraw_SegmentedViewsVisible)& ent, Interface_EntityIterator& iter) const
{
  Standard_Integer Up  = ent->NbSegmentBlocks();
  for ( Standard_Integer i = 1; i <= Up; i++)
    {
      iter.GetOneItem( ent->ViewItem(i) );
      if (ent->IsColorDefinition(i))
	iter.GetOneItem( ent->ColorDefinition(i) );
      if (!ent->IsFontDefinition(i))
	iter.GetOneItem( ent->LineFontDefinition(i) );
    }
}

void IGESDraw_ToolSegmentedViewsVisible::OwnCopy
  (const Handle(IGESDraw_SegmentedViewsVisible)& another,
   const Handle(IGESDraw_SegmentedViewsVisible)& ent, Interface_CopyTool& TC) const
{
  Standard_Integer                              nbval;
  Handle(IGESDraw_HArray1OfViewKindEntity) views;
  Handle(TColStd_HArray1OfReal)          breakpointParameters;
  Handle(TColStd_HArray1OfInteger)       displayFlags;
  Handle(TColStd_HArray1OfInteger)       colorValues;
  Handle(IGESGraph_HArray1OfColor)          colorDefinitions;
  Handle(TColStd_HArray1OfInteger)       lineFontValues;
  Handle(IGESBasic_HArray1OfLineFontEntity) lineFontDefinitions;
  Handle(TColStd_HArray1OfInteger)       lineWeights;
 
  Handle(IGESData_ViewKindEntity)               retView; 
  Handle(IGESGraph_Color)                       retColorDef; 
  Handle(IGESData_LineFontEntity)               retLineFontDef; 
 
  nbval                = another->NbSegmentBlocks();
  views                = new IGESDraw_HArray1OfViewKindEntity(1, nbval);
  breakpointParameters = new TColStd_HArray1OfReal(1, nbval);
  displayFlags         = new TColStd_HArray1OfInteger(1, nbval);
  colorValues          = new TColStd_HArray1OfInteger(1, nbval);
  lineFontValues       = new TColStd_HArray1OfInteger(1, nbval);
  colorDefinitions     = new IGESGraph_HArray1OfColor(1, nbval);
  lineFontDefinitions  = new IGESBasic_HArray1OfLineFontEntity(1, nbval);
  lineWeights          = new TColStd_HArray1OfInteger(1, nbval);
 
  for (Standard_Integer i = 1; i <= nbval; i++)
    {
      retView = another->ViewItem(i);
      DeclareAndCast(IGESData_ViewKindEntity,
                     tempView, TC.Transferred(retView));
      views->SetValue( i, tempView );
 
      breakpointParameters->SetValue( i, another->BreakpointParameter(i) );
 
      displayFlags->SetValue( i, another->DisplayFlag(i) );
 
      if ( another->IsColorDefinition(i) )
	{
          retColorDef = another->ColorDefinition(i);
          DeclareAndCast(IGESGraph_Color, tempColorDef, 
			 TC.Transferred(retColorDef));
          colorDefinitions->SetValue( i, tempColorDef );
	}
      else
	{
          colorValues->SetValue( i, another->ColorValue(i) );
////          colorDefinitions->SetValue( i, NULL );    par defaut
	}
 
      if ( another->IsFontDefinition(i) )
	{
          retLineFontDef = another->LineFontDefinition(i);
          DeclareAndCast(IGESData_LineFontEntity, tempLineFontDef, 
			 TC.Transferred(retLineFontDef));
          lineFontDefinitions->SetValue( i, tempLineFontDef );
	}
      else
	{
          lineFontValues->SetValue( i, another->LineFontValue(i) );
////          lineFontDefinitions->SetValue( i, NULL );  par defaut
	}
 
      lineWeights->SetValue( i, another->LineWeightItem(i) );
    }

  ent->Init(views, breakpointParameters, displayFlags,
	    colorValues, colorDefinitions,
	    lineFontValues, lineFontDefinitions, lineWeights);
}

IGESData_DirChecker IGESDraw_ToolSegmentedViewsVisible::DirChecker
  (const Handle(IGESDraw_SegmentedViewsVisible)& /*ent*/)  const
{ 
  IGESData_DirChecker DC (402, 19);
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

void IGESDraw_ToolSegmentedViewsVisible::OwnCheck
  (const Handle(IGESDraw_SegmentedViewsVisible)& /*ent*/,
   const Interface_ShareTool& , Handle(Interface_Check)& /*ach*/)  const
{
}

void IGESDraw_ToolSegmentedViewsVisible::OwnDump
  (const Handle(IGESDraw_SegmentedViewsVisible)& ent, const IGESData_IGESDumper& dumper,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  Standard_Integer sublevel = (level <= 4) ? 0 : 1;

  S << "IGESDraw_SegmentedViewsVisible" << endl;

  S << "View Entities            : " << endl
    << "Breakpoint parameters    : " << endl
    << "Display flags            : " << endl
    << "Color Values             : " << endl
    << "Color Definitions        : " << endl
    << "LineFont Values          : " << endl
    << "LineFont Definitions     : " << endl
    << "Line Weights : " << endl;
  S << "Count Of Blocks = "      << ent->NbSegmentBlocks() << endl;
  switch (level)
    {
    case 4 : S << " [ for content, ask level > 4 ]" << endl;
      break; // Nothing to be dumped here
    case 5 :        // Presently level 5 and 6 have the same Dump
    case 6 :
      {
	Standard_Integer I;
	Standard_Integer up  = ent->NbSegmentBlocks();
	for (I = 1; I <= up; I++)
          {
	    S << "[" << I << "]:" << endl;
	    S << "View Entity : ";
	    dumper.Dump (ent->ViewItem(I),S, sublevel);
	    S << endl;
	    S << "Breakpoint parameter : " <<ent->BreakpointParameter(I)<<endl;
	    S << "Display Flag : " << ent->DisplayFlag(I) << endl;
	    if ( (ent->ColorDefinition(I)).IsNull() )
              {
		S << "Color Value : ";
		S << ent->ColorValue(I);
              }
	    else
              {
		S << "Color Definition : ";
		dumper.Dump (ent->ColorDefinition(I),S, sublevel);
              }
	    S << endl;
	    if ( (ent->LineFontDefinition(I)).IsNull() )
              {
		S << "LineFont Value : " << ent->LineFontValue(I);
              }
	    else
              {
		S << "LineFont Definition : ";
		dumper.Dump (ent->LineFontDefinition(I),S, sublevel);
              }
	    S << endl;
	    S << "Line Weight : " << ent->LineWeightItem(I) << endl;
          }
      }
      break;
    }
  S << endl;
}
