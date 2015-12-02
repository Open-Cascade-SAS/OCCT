// Created on: 1997-10-20
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#include <QADNaming.hxx>

#include <DDF.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDF_AttributeMap.hxx>
#include <TDF_MapIteratorOfAttributeMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Name.hxx>
#include <TNaming.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_MapOfNamedShape.hxx>
#include <TNaming_MapIteratorOfMapOfNamedShape.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_Selector.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_LabelMap.hxx>

#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Drawable3D.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <stdio.h>

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

static void Display (const Standard_CString Name, const TopoDS_Shape& S)
{  
  //char* name = Name;
  static Standard_Integer nbIsos  = 2;
  static Standard_Real size    = 100.;
  static Standard_Integer discret = 30;

  Handle(DBRep_DrawableShape) D = new DBRep_DrawableShape(S,
							  Draw_jaune,
							  Draw_vert,
							  Draw_bleu,
							  Draw_rouge,
							  size,
							  nbIsos,
							  discret);
  Draw::Set(Name,D);
}

//=======================================================================
//function : DumpNaming
//purpose  : 
//=======================================================================

static void DumpNaming (const Handle(TNaming_Naming)& naming, Draw_Interpretor& di) 
{
  TCollection_AsciiString Entry;
  const TNaming_Name& AName = naming->GetName();

  //TNaming::Print(AName.Type(),cout);
  Standard_SStream aSStream1;
  TNaming::Print(AName.Type(),aSStream1);
  di << aSStream1;
  di << " ";

  //TopAbs::Print(AName.ShapeType(),cout);
  Standard_SStream aSStream2;
  TopAbs::Print(AName.ShapeType(),aSStream2);
  di << aSStream2;

  const TNaming_ListOfNamedShape& NSS = AName.Arguments();
  for (TNaming_ListIteratorOfListOfNamedShape it(NSS); it.More(); it.Next()) {
    TDF_Tool::Entry(it.Value()->Label(),Entry); 
    di << " " << Entry.ToCString();
  }
  if(!AName.StopNamedShape().IsNull()) {
    TDF_Tool::Entry(AName.StopNamedShape()->Label(),Entry); 
    di << " Stop " << Entry.ToCString();
  }
}

//=======================================================================
//function : SelectShape ou SelectGeometry
//purpose  : "Select DF entry shape [context]", 
//=======================================================================

static Standard_Integer QADNaming_Select (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n == 4 || n == 5) {
    Standard_Boolean geometry = (strcmp(a[0],"SelectGeometry")==0?1:0); 
    Handle(TDF_Data) DF;
    if(!DDF::GetDF(a[1], DF))  return 1;
    TDF_Label L;
    DDF::AddLabel(DF,a[2],L);
    TNaming_Selector SL (L);
    if (n == 4) {
      TopoDS_Shape S = DBRep::Get(a[3], TopAbs_SHAPE);  
      SL.Select (S, geometry); 
    }
    if (n == 5) {  
      TopoDS_Shape S = DBRep::Get(a[3], TopAbs_SHAPE);
      TopoDS_Shape C = DBRep::Get(a[4], TopAbs_SHAPE);
      SL.Select (S, C, geometry); 
    }
    return 0;  
  }
  di << "QADNaming_Select : Error\n";
  return 1;
}

//=======================================================================
//function : SolveSelection
//purpose  : "SolveSelection DF entry", 
//=======================================================================

static Standard_Integer QADNaming_SolveSelection (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n >= 3) {
    TDF_Label aLabel;
    if (!QADNaming::Entry(a, aLabel)) return 1;
    char name[100];
    TNaming_Selector SL (aLabel);
    TDF_LabelMap valid;
    Standard_Integer i;
    for(i=3;i<n;i++) {
      TDF_Label aValidLab;
      if (!DDF::FindLabel(aLabel.Data(),a[i],aValidLab)) di<<"Warning: label "<<a[i]<<" not exists\n";
      else valid.Add(aValidLab);
    }
    Standard_Boolean done = SL.Solve(valid);
    TopoDS_Shape Res = TNaming_Tool::CurrentShape(SL.NamedShape());
    //TopoDS_Shape Res = TNaming_Tool::CurrentShape(NS);
    Sprintf (name,"%s_%s","new",a[2]);
    Display (name,Res);
    return done?0:1;
  }
  di << "Usage:  DumpSelection DF entry [validlabel1 validlabel2 ...]\n";
  return 1;
}

//=======================================================================
//function : DumpSelection
//purpose  : DumpSelection DF entry (R)
//=======================================================================

static Standard_Integer QADNaming_DumpSelection (Draw_Interpretor& di, 
					      Standard_Integer n, const char** a)
{
  if (n == 3 || n == 4) { 
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(DF,a[2],L)) return 1;
    Handle(TNaming_Naming) naming;
    if (!L.FindAttribute(TNaming_Naming::GetID(),naming)) {  
      di <<"QADNaming_DumpSelection : not a selection\n";
      return 1;
    }
    DumpNaming(naming, di);
    di << "\n";
    if (n == 4) { 
      Standard_Integer depth = L.Depth();  
      Standard_Integer curdepth = 0;
      TCollection_AsciiString Entry;
      TDF_ChildIterator it (naming->Label(),Standard_True);
      for (;it.More();it.Next()) {
	if (it.Value().FindAttribute(TNaming_Naming::GetID(),naming)) { 
	  curdepth = (naming->Label().Depth()- depth);
	  for (Standard_Integer i = 1; i <= curdepth; i++) di << " ";    
	  TDF_Tool::Entry (naming->Label(),Entry); 
	  di << Entry.ToCString() << " "; 
	  DumpNaming(naming, di);
	  di << "\n";
	}
      }
    }
    return 0;
  }
  di << "QADNaming_DumpSelection : Error\n";
  return 1;  
}

//=======================================================================
//function : ArgsSelection
//purpose  : ArgsSelection DF entry
//=======================================================================

static Standard_Integer QADNaming_ArgsSelection (Draw_Interpretor& di, 
					      Standard_Integer n, const char** a)
{
  if (n == 3) { 
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(DF,a[2],L)) return 1;
    Handle(TNaming_Naming) naming;
    if (!L.FindAttribute(TNaming_Naming::GetID(),naming)) {  
      di <<"QADNaming_DumpSelection : not a selection\n";
      return 1;
    }  
    TCollection_AsciiString Entry;
    TNaming_Selector SL (L);
    di <<" Selection Arguments : ";
    TDF_AttributeMap args;
    SL.Arguments(args);
    for (TDF_MapIteratorOfAttributeMap it(args); it.More(); it.Next()) {
      TDF_Tool::Entry(it.Key()->Label(),Entry); 
      di << Entry.ToCString() << " ";
    }
    di << "\n";
    return 0;
  }
  di << "QADNaming_ArgsSelection : Error\n";
  return 1;  
}

//=======================================================================
//function : CollectAttachment
//purpose  : 
//=======================================================================

static void CollectAttachment (const TDF_Label& root,
			       const Handle(TNaming_Naming)& naming, 
			       TNaming_MapOfNamedShape& attachment)
{
  TNaming_ListIteratorOfListOfNamedShape itarg;
  const TNaming_ListOfNamedShape& args = naming->GetName().Arguments();
  for (itarg.Initialize(args);itarg.More();itarg.Next()) {
    if (!itarg.Value()->Label().IsDescendant(root)) attachment.Add(itarg.Value());
  }  
  Handle(TNaming_Naming) subnaming; 
  for (TDF_ChildIterator it(naming->Label(),Standard_True);it.More();it.Next()) {
    if (it.Value().FindAttribute(TNaming_Naming::GetID(),subnaming)) {   
      const TNaming_ListOfNamedShape& subargs = subnaming->GetName().Arguments();
      for (itarg.Initialize(subargs);itarg.More();itarg.Next()) {
	if (!itarg.Value()->Label().IsDescendant(root)) attachment.Add(itarg.Value());
      }
    }
  }
}  


//=======================================================================
//function : Attachment
//purpose  : Attachment DF entry
//=======================================================================

static Standard_Integer QADNaming_Attachment (Draw_Interpretor& di, 
					    Standard_Integer n, 
					    const char** a)
{
  if (n == 3) { 
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(a[1],DF)) return 1;  
    TDF_Label L;
    if (!DDF::FindLabel(DF,a[2],L)) return 1;  
    Handle(TNaming_Naming) naming; 
    TNaming_MapOfNamedShape attachment;
    if (L.FindAttribute(TNaming_Naming::GetID(),naming)) {
      CollectAttachment (L,naming,attachment);
    }
    else {
      for (TDF_ChildIterator it (L,Standard_True); it.More();it.Next()) {
	if (it.Value().FindAttribute(TNaming_Naming::GetID(),naming)) {
	  CollectAttachment (L,naming,attachment);
	  it.NextBrother();
	}
      }
    }
    TCollection_AsciiString Entry;   
    TDF_Tool::Entry(L,Entry); 
    di << " Attachment of " << Entry.ToCString();
    di << "\n";
    for (TNaming_MapIteratorOfMapOfNamedShape ita (attachment); ita.More(); ita.Next()) {
      TDF_Tool::Entry (ita.Key()->Label(),Entry); 
      di << Entry.ToCString() << " ";
    }
    di << "\n";
    return 0;
  }
  di << "QADNaming_Attachment : Error\n";
  return 1;  
}

//=======================================================================
//function : SelectionCommands
//purpose  : 
//=======================================================================

void QADNaming::SelectionCommands(Draw_Interpretor& theCommands) 
{
  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  
  const char* g = "Naming data commands";

  theCommands.Add("SelectShape",
		  "SelectShape DF entry shape [context]", 
		  __FILE__, QADNaming_Select, g);
  
  theCommands.Add("SelectGeometry",
		  "SelectGeometry DF entry shape [context]", 
		  __FILE__, QADNaming_Select, g);
  
  theCommands.Add("DumpSelection",         
		  "DumpSelected DF entry", 
		  __FILE__, QADNaming_DumpSelection, g);    

  theCommands.Add("ArgsSelection",         
		  "ArgsSelection DF entry", 
		  __FILE__, QADNaming_ArgsSelection, g);  

  theCommands.Add("SolveSelection",         
		  "DumpSelection DF entry [validlabel1 validlabel2 ...]", 
		  __FILE__, QADNaming_SolveSelection, g);  

  theCommands.Add("Attachment",         
		  "Attachment DF entry", 
		  __FILE__, QADNaming_Attachment, g);

}
