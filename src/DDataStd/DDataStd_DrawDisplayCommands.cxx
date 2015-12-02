// Created on: 1998-02-12
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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
#include <DDF.hxx>
#include <Draw.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Viewer.hxx>
#include <Draw_ColorKind.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Trsf.hxx>   
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx> 
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx> 
#include <TopAbs.hxx>
#include <TDF_AttributeList.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TNaming_Tool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_LabelList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx> 
#include <TNaming_NamedShape.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Constraint.hxx> 
#include <ElSLib.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>

#include <DDataStd_DrawPresentation.hxx>
#include <DDataStd_DrawDriver.hxx>

#ifndef _WIN32
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif
//=======================================================================
//function : DDataStd_PNT
//purpose  : SetPoint (DF, entry, x, y, z)
//=======================================================================

static Standard_Integer DDataStd_PNT (Draw_Interpretor& di,
				      Standard_Integer nb, 
				      const char** arg) 
{ 
  if (nb == 6) {
    Handle(TDF_Data) DF;
    if (!DDF::GetDF (arg[1], DF)) return 1;  
    TDF_Label L;
    DDF::AddLabel (DF, arg[2], L);    
    Standard_Real x = Draw::Atof(arg[3]);
    Standard_Real y = Draw::Atof(arg[4]);
    Standard_Real z = Draw::Atof(arg[5]);
    TDataXtd_Point::Set (L,gp_Pnt(x,y,z));
    return 0;
  }  
  di << "DDataStd_PNT : Error : not done\n";
  return 1;
}
 
//=======================================================================
//function : DDataStd_Rmdraw
//purpose  : Rmdraw (name)
//=======================================================================

static Standard_Integer DDataStd_Rmdraw (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg)
{
  if (nb == 2) { 
    Handle(Draw_Drawable3D) D3D;
    D3D = Draw::Get(arg[1],Standard_True);  
    if (!D3D.IsNull()) dout.RemoveDrawable(D3D); 
    return 0;
  }
  di << "DDataStd_Rmdraw : Error : not done\n";
  return 1;
}

//=======================================================================
//function : DDataStd_DrawOwner
//purpose  : DrawOwner (drawable)
//=======================================================================

static Standard_Integer DDataStd_DrawOwner (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char** arg)
{
  if (nb == 2) {
    Handle(Draw_Drawable3D) D = Draw::Get(arg[1]);
    if (!D.IsNull()) {
      TCollection_AsciiString entry;
      TCollection_AsciiString name (D->Name());
      Standard_Integer index = name.Search("_0:");
      if (index > 0) {
	entry = name.Split(index);
	name.Remove(index);
	di << entry.ToCString();
      }
      else di << name.ToCString();
    }
    else di << 0;
    return 0;
  }
  di << "DDataStd_DrawOwner : Error\n";
  return 1;
}

//=======================================================================
//function : DDataStd_DrawDisplay
//purpose  : DDisplay (DOC,entry)
//=======================================================================

static Standard_Integer DDataStd_DrawDisplay (Draw_Interpretor& di,
						Standard_Integer nb, 
						const char** arg) 
{   
  if (nb == 3) {   
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;   
    TDF_Label L;
    if (!DDF::FindLabel(DF,arg[2],L)) return 1;
    DDataStd_DrawPresentation::Display(L);
    return 0;
  }
  di << "DDataStd_DrawDisplay : Error\n";
  return 1;
}

// //=======================================================================
// //function : DDataStd_DrawRedisplay
// //purpose  : DrawRedisplay (DOC,entry)
// //=======================================================================

// static Standard_Integer DDataStd_DrawRedisplay (Draw_Interpretor&,
// 						  Standard_Integer nb, 
// 						  const char** arg) 
// {   
//   if (nb == 3) {     
//     Handle(TDF_Data) DF;
//     if (!DDF::GetDF(arg[1],DF)) return 1;   
//     TDF_Label L;
//     if (!DDF::FindLabel(DF,arg[2],L)) return 1;
//     DDataStd_DrawPresentation::Display(L,Standard_True);
//     return 0;
//   }
//   cout << "DDataStd_DrawRedisplay : Error" << endl;
//   return 1;
// }


//=======================================================================
//function : DDataStd_DrawErase
//purpose  : DrawErase (DOC,entry)
//=======================================================================

static Standard_Integer DDataStd_DrawErase (Draw_Interpretor& di,
					      Standard_Integer nb, 
					      const char** arg) 
{   
  if (nb == 3) {     
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;   
    TDF_Label L;
    if (!DDF::FindLabel(DF,arg[2],L)) return 1;
    DDataStd_DrawPresentation::Erase(L);
    return 0;
  }
  di << "DDataStd_DrawErase : Error\n";
  return 1;
}

//=======================================================================
//function : DDataStd_DrawUpdate
//purpose  : DrawUpdate (DOC,entry)
//=======================================================================

static Standard_Integer DDataStd_DrawUpdate (Draw_Interpretor& di,
					       Standard_Integer nb, 
					       const char** arg) 
{   
  if (nb == 3) {  
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;   
    TDF_Label L;
    if (!DDF::FindLabel(DF,arg[2],L)) return 1;     
    DDataStd_DrawPresentation::Update(L);
    return 0;
  }
  di << "DDataStd_DrawUpdate : Error\n";
  return 1;
}

//=======================================================================
//function : DDataStd_DrawRepaint
//purpose  : 
//=======================================================================

static Standard_Integer DDataStd_DrawRepaint (Draw_Interpretor& /*di*/,
					      Standard_Integer /*nb*/, 
					      const char** /*arg*/) 
{  
  dout.Repaint3D(); 
  dout.Flush();
  return 0;
}


//=======================================================================
//function : DrawDisplayCommands
//purpose  : 
//=======================================================================


void DDataStd::DrawDisplayCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "SKETCH commands" ;


  theCommands.Add ("PNT", 
                   "PNT (DF, entry, x, y, z)",
		   __FILE__, DDataStd_PNT, g);
  
  
  // remove drawable


  theCommands.Add ("rmdraw", 
                   "rmdraw(name)",
		   __FILE__, DDataStd_Rmdraw, g);


  // rtetrieve a label from a drawable


  theCommands.Add ("DrawOwner", 
                   "DrawOwner (drawable)",
		   __FILE__, DDataStd_DrawOwner, g); 
  
  // draw display 
 

  theCommands.Add ("DrawDisplay", 
                   "DrawDisplay (DF, entry)",
		   __FILE__, DDataStd_DrawDisplay, g);  

  theCommands.Add ("DrawErase", 
                   "DrawErase (DF, entry)",
		   __FILE__, DDataStd_DrawErase, g);    

  theCommands.Add ("DrawUpdate", 
                   "DrawUpdate (DF, entry)",
		   __FILE__, DDataStd_DrawUpdate, g);  

  theCommands.Add ("DrawRepaint", 
                   "update the draw viewer",
		   __FILE__, DDataStd_DrawRepaint, g);    
}











