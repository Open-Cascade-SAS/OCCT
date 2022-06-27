// Created on: 1999-03-22
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

#ifndef _TopOpeBRepTool_mkTondgE_HeaderFile
#define _TopOpeBRepTool_mkTondgE_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt2d.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>



class TopOpeBRepTool_mkTondgE 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepTool_mkTondgE();
  
  Standard_EXPORT Standard_Boolean Initialize (const TopoDS_Edge& dgE, const TopoDS_Face& F, const gp_Pnt2d& uvi, const TopoDS_Face& Fi);
  
  Standard_EXPORT Standard_Boolean SetclE (const TopoDS_Edge& clE);
  
  Standard_EXPORT Standard_Boolean IsT2d() const;
  
  Standard_EXPORT Standard_Boolean SetRest (const Standard_Real pari, const TopoDS_Edge& Ei);
  
  Standard_EXPORT Standard_Integer GetAllRest (TopTools_ListOfShape& lEi);
  
  Standard_EXPORT Standard_Boolean MkTonE (Standard_Integer& mkT, Standard_Real& par1, Standard_Real& par2);
  
  Standard_EXPORT Standard_Boolean MkTonE (const TopoDS_Edge& Ei, Standard_Integer& mkT, Standard_Real& par1, Standard_Real& par2);




protected:





private:



  TopoDS_Edge mydgE;
  TopoDS_Face myF;
  TopoDS_Edge myclE;
  gp_Dir mydirINcle;
  TopoDS_Face myFi;
  gp_Pnt2d myuvi;
  Standard_Boolean isT2d;
  TopTools_DataMapOfShapeReal myEpari;
  Standard_Boolean hasRest;
  gp_Dir myngf;
  gp_Dir myngfi;


};







#endif // _TopOpeBRepTool_mkTondgE_HeaderFile
