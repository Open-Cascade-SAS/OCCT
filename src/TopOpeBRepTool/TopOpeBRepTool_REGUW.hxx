// Created on: 1998-12-08
// Created by: Xuan PHAM PHU
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

#ifndef _TopOpeBRepTool_REGUW_HeaderFile
#define _TopOpeBRepTool_REGUW_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_CORRISO.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopOpeBRepTool_IndexedDataMapOfShapeconnexity.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt2d.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Dir2d.hxx>
class TopoDS_Face;
class TopOpeBRepTool_connexity;



class TopOpeBRepTool_REGUW 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepTool_REGUW(const TopoDS_Face& FRef);
  
  Standard_EXPORT const TopoDS_Face& Fref() const;
  
  Standard_EXPORT void SetEsplits (TopTools_DataMapOfShapeListOfShape& Esplits);
  
  Standard_EXPORT void GetEsplits (TopTools_DataMapOfShapeListOfShape& Esplits) const;
  
  Standard_EXPORT void SetOwNw (TopTools_DataMapOfShapeListOfShape& OwNw);
  
  Standard_EXPORT void GetOwNw (TopTools_DataMapOfShapeListOfShape& OwNw) const;
  
  Standard_EXPORT Standard_Boolean SplitEds();
  
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT const TopoDS_Shape& S() const;
  
  Standard_EXPORT Standard_Boolean HasInit() const;
  
  Standard_EXPORT Standard_Boolean MapS();
  
  Standard_EXPORT Standard_Boolean REGU (const Standard_Integer istep, const TopoDS_Shape& Scur, TopTools_ListOfShape& Splits);
  
  Standard_EXPORT Standard_Boolean REGU();
  
  Standard_EXPORT Standard_Boolean GetSplits (TopTools_ListOfShape& Splits) const;
  
  Standard_EXPORT Standard_Boolean InitBlock();
  
  Standard_EXPORT Standard_Boolean NextinBlock();
  
  Standard_EXPORT Standard_Boolean NearestE (const TopTools_ListOfShape& loe, TopoDS_Edge& efound) const;
  
  Standard_EXPORT Standard_Boolean Connexity (const TopoDS_Vertex& v, TopOpeBRepTool_connexity& co) const;
  
  Standard_EXPORT Standard_Boolean AddNewConnexity (const TopoDS_Vertex& v, const Standard_Integer OriKey, const TopoDS_Edge& e);
  
  Standard_EXPORT Standard_Boolean RemoveOldConnexity (const TopoDS_Vertex& v, const Standard_Integer OriKey, const TopoDS_Edge& e);
  
  Standard_EXPORT Standard_Boolean UpdateMultiple (const TopoDS_Vertex& v);




protected:





private:

  
  Standard_EXPORT void InitStep (const TopoDS_Shape& S);


  TopoDS_Shape myS;
  TopOpeBRepTool_CORRISO myCORRISO;
  Standard_Boolean hasnewsplits;
  TopTools_DataMapOfShapeListOfShape myEsplits;
  TopTools_DataMapOfShapeListOfShape myOwNw;
  TopOpeBRepTool_IndexedDataMapOfShapeconnexity mymapvEds;
  TopTools_MapOfShape mymapvmultiple;
  TopTools_ListOfShape myListVmultiple;
  Standard_Integer iStep;
  Standard_Real mytol2d;
  Standard_Boolean isinit0;
  TopoDS_Vertex myv0;
  gp_Pnt2d myp2d0;
  TopoDS_Vertex myv;
  TopoDS_Edge myed;
  gp_Pnt2d myp2d;
  gp_Dir2d mytg2d;


};







#endif // _TopOpeBRepTool_REGUW_HeaderFile
