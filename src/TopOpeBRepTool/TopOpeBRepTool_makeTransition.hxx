// Created on: 1999-02-11
// Created by: Xuan PHAM PHU
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_makeTransition_HeaderFile
#define _TopOpeBRepTool_makeTransition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt2d.hxx>
#include <TopAbs_State.hxx>



class TopOpeBRepTool_makeTransition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepTool_makeTransition();
  
  Standard_EXPORT Standard_Boolean Initialize (const TopoDS_Edge& E, const Standard_Real pbef, const Standard_Real paft, const Standard_Real parE, const TopoDS_Face& FS, const gp_Pnt2d& uv, const Standard_Real factor);
  
  Standard_EXPORT void Setfactor (const Standard_Real factor);
  
  Standard_EXPORT Standard_Real Getfactor() const;
  
  Standard_EXPORT Standard_Boolean IsT2d() const;
  
  Standard_EXPORT Standard_Boolean SetRest (const TopoDS_Edge& ES, const Standard_Real parES);
  
  Standard_EXPORT Standard_Boolean HasRest() const;
  
  Standard_EXPORT Standard_Boolean MkT2donE (TopAbs_State& stb, TopAbs_State& sta) const;
  
  Standard_EXPORT Standard_Boolean MkT3onE (TopAbs_State& stb, TopAbs_State& sta) const;
  
  Standard_EXPORT Standard_Boolean MkT3dproj (TopAbs_State& stb, TopAbs_State& sta) const;
  
  Standard_EXPORT Standard_Boolean MkTonE (TopAbs_State& stb, TopAbs_State& sta);




protected:





private:



  TopoDS_Edge myE;
  Standard_Real mypb;
  Standard_Real mypa;
  Standard_Real mypE;
  TopoDS_Face myFS;
  gp_Pnt2d myuv;
  Standard_Boolean hasES;
  TopoDS_Edge myES;
  Standard_Real mypES;
  Standard_Boolean isT2d;
  Standard_Real myfactor;


};







#endif // _TopOpeBRepTool_makeTransition_HeaderFile
