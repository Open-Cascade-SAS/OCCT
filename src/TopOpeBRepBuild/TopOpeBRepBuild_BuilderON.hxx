// Created on: 1993-06-14
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_BuilderON_HeaderFile
#define _TopOpeBRepBuild_BuilderON_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_PBuilder.hxx>
#include <TopOpeBRepBuild_PGTopo.hxx>
#include <TopOpeBRepTool_Plos.hxx>
#include <TopOpeBRepBuild_PWireEdgeSet.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
class TopOpeBRepDS_Interference;



class TopOpeBRepBuild_BuilderON 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_BuilderON();
  
  Standard_EXPORT TopOpeBRepBuild_BuilderON(const TopOpeBRepBuild_PBuilder& PB, const TopoDS_Shape& F, const TopOpeBRepBuild_PGTopo& PG, const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES);
  
  Standard_EXPORT void Perform (const TopOpeBRepBuild_PBuilder& PB, const TopoDS_Shape& F, const TopOpeBRepBuild_PGTopo& PG, const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES);
  
  Standard_EXPORT Standard_Boolean GFillONCheckI (const Handle(TopOpeBRepDS_Interference)& I) const;
  
  Standard_EXPORT void GFillONPartsWES1 (const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT void GFillONPartsWES2 (const Handle(TopOpeBRepDS_Interference)& I, const TopoDS_Shape& EspON);
  
  Standard_EXPORT void Perform2d (const TopOpeBRepBuild_PBuilder& PB, const TopoDS_Shape& F, const TopOpeBRepBuild_PGTopo& PG, const TopOpeBRepTool_Plos& PLSclass, const TopOpeBRepBuild_PWireEdgeSet& PWES);
  
  Standard_EXPORT void GFillONParts2dWES2 (const Handle(TopOpeBRepDS_Interference)& I, const TopoDS_Shape& EspON);




protected:





private:



  TopOpeBRepBuild_PBuilder myPB;
  TopOpeBRepBuild_PGTopo myPG;
  TopOpeBRepTool_Plos myPLSclass;
  TopOpeBRepBuild_PWireEdgeSet myPWES;
  TopoDS_Shape myFace;
  TopOpeBRepDS_ListOfInterference myFEI;


};







#endif // _TopOpeBRepBuild_BuilderON_HeaderFile
