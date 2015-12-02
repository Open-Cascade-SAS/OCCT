// Created on: 1999-07-15
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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

#include <DDataStd.hxx>
#include <DDataStd_DrawPresentation.hxx>
#include <DDF.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <TDF_Data.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataXtd_Shape.hxx>


// LES ATTRIBUTES

#include <TDataStd_NoteBook.hxx>
#include <TDataXtd_Shape.hxx>


#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>


//=======================================================================
//function : NewDirectory (DF, entry )
//=======================================================================
static Standard_Integer DDataStd_NewDirectory (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char** arg) 
{
  if( nb != 3 ) {
    di << "Too few arguments"  << "\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF))  return 1;  
  TDF_Label label;
  DDF::AddLabel(DF, arg[2], label);
  TDataStd_Directory::New(label );
  return 0;
}


//=======================================================================
//function : AddDirectory (DF, entry )
//=======================================================================
static Standard_Integer DDataStd_AddDirectory (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char** arg) 
{
  if( nb != 3 ) {
    di << "Too few arguments"  << "\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF))  return 1;  
  TDF_Label label;
  if( !DDF::FindLabel(DF, arg[2], label) ) { 
    di << "No label for entry"  << "\n";
    return 1; 
  }
  Handle(TDataStd_Directory) A;
  if (TDataStd_Directory::Find(label, A)) {   
    Handle(TDataStd_Directory) Dir = TDataStd_Directory::AddDirectory (A);
    TCollection_AsciiString entry;          
    TDF_Tool::Entry(Dir->Label(), entry);
    di << entry.ToCString()<<" ";                     //return a label to draw
    return 0;
  }
  di << "No Object Attribute on label"  << "\n";
  return 1;
}

//=======================================================================
//function : MakeObjectLabel (DF, entry )
//=======================================================================
static Standard_Integer DDataStd_MakeObjectLabel (Draw_Interpretor& di,
						 Standard_Integer nb, 
						 const char** arg) 
{
  if( nb != 3 ) {
    di << "Too few arguments"  << "\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF))  return 1;   
  TDF_Label label;
  if( !DDF::FindLabel(DF, arg[2], label) ) {  
    di << "No label for entry"  << "\n";
    return 1; 
  }
  Handle(TDataStd_Directory) A;
  if(TDataStd_Directory::Find(label,A)) {
    TCollection_AsciiString entry;       
    TDF_Tool::Entry(TDataStd_Directory::MakeObjectLabel(A), entry);
    di << entry.ToCString()<<" ";                     //return a label to draw
    return 0;
  }
  di << "No Object Attribute on label"  << "\n";
  return 1;
}



//=======================================================================
//function : DDataStd_NewNoteBook
//purpose  : NewNoteBook (DF, entry)
//=======================================================================

static Standard_Integer DDataStd_NewNoteBook (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg) 
{     
  if (nb == 3) {    
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);
    TDataStd_NoteBook::New(L);
    return 0;
  }
  di << "DDataStd_NewNoteBook : Error\n";
  return 1;
}

//=======================================================================
//function : NewShape (DF, entry,  [in_shape] )
//=======================================================================
static Standard_Integer DDataStd_NewShape (Draw_Interpretor& di,
					   Standard_Integer nb, 
					   const char** arg) 
{
  //di << "nb = " <<nb   << "\n";
  if( nb < 3 ) {
    di << "Too few arguments"  << "\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF))  return 1;   
  TDF_Label label;
  DDF::AddLabel(DF, arg[2], label); 
  if( nb == 4 ) {
    TopoDS_Shape shape = DBRep::Get( arg[3] );
    if( shape.IsNull() ) {
      di << "Shape argument is invalid"   << "\n";
      return 1;
    } 
    TDataXtd_Shape::Set(label, shape );
  }
  else TDataXtd_Shape::New(label);
  return 0; 
}


//=======================================================================
//function : GetShape2 (DF, entry, out_shape )
//=======================================================================
static Standard_Integer DDataStd_GetShape2 (Draw_Interpretor& di,
					    Standard_Integer nb, 
					    const char** arg) 
{   
  if( nb < 4 ) {
    di << "Too few arguments"  << "\n";
    return 1;
  }
  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF))  return 1;   
  TDF_Label label;
  if( !DDF::FindLabel(DF, arg[2], label) ) {  
    di << "No label for entry"  << "\n";
    return 1;  
  }
  DBRep::Set(arg[3], TDataXtd_Shape::Get(label));
 
  return 0;
}



//=======================================================================
//function : ObjectComands
//purpose  : 
//=======================================================================

void DDataStd::ObjectCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DData : Standard Attribute Commands";
  
  theCommands.Add ("NewNoteBook", 
                   "NewNoteBook (DF, entry)",
		   __FILE__, DDataStd_NewNoteBook, g);  

  theCommands.Add ("NewShape", 
                   "NewShape (DF, entry, [in_shape] )",
		   __FILE__, DDataStd_NewShape, g);

  theCommands.Add ("GetShape2", 
                   "GetShape2 (DF, entry, out_shape )",
		   __FILE__, DDataStd_GetShape2, g);
  
  theCommands.Add ("NewDirectory", 
                   "NewDirectory (DF, entry)",
		   __FILE__, DDataStd_NewDirectory, g);

  theCommands.Add ("AddDirectory", 
                   "AddDirectory (DF, entry)",
		   __FILE__, DDataStd_AddDirectory, g);

  theCommands.Add ("MakeObjectLabel", 
                   "MakeObjectLabel (DF, entry)",
		   __FILE__, DDataStd_MakeObjectLabel, g);

}

