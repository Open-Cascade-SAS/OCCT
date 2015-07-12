// Created on: 1998-02-05
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_DSS_HeaderFile
#define _TopOpeBRepDS_DSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepDS_DoubleMapOfIntegerShape.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_MapOfIntegerShapeData.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Boolean.hxx>
#include <TopOpeBRepDS_Config.hxx>
class TopoDS_Shape;
class TopOpeBRepDS_Interference;



class TopOpeBRepDS_DSS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_DSS();
  
  Standard_EXPORT void Clear();
  
  //! Insert a shape S. Returns the index.
  Standard_EXPORT Standard_Integer AddShape (const TopoDS_Shape& S);
  
  //! Insert a shape S which ancestor is I = 1 or 2. Returns the index.
  Standard_EXPORT Standard_Integer AddShape (const TopoDS_Shape& S, const Standard_Integer I);
  
  Standard_EXPORT Standard_Boolean KeepShape (const Standard_Integer I, const Standard_Boolean K = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean KeepShape (const TopoDS_Shape& S, const Standard_Boolean K = Standard_True) const;
  
  Standard_EXPORT void ChangeKeepShape (const Standard_Integer I, const Standard_Boolean K);
  
  Standard_EXPORT void ChangeKeepShape (const TopoDS_Shape& S, const Standard_Boolean K);
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& ShapeInterferences (const TopoDS_Shape& S, const Standard_Boolean K = Standard_True) const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeShapeInterferences (const TopoDS_Shape& S);
  
  Standard_EXPORT const TopOpeBRepDS_ListOfInterference& ShapeInterferences (const Standard_Integer I, const Standard_Boolean K = Standard_True) const;
  
  Standard_EXPORT TopOpeBRepDS_ListOfInterference& ChangeShapeInterferences (const Standard_Integer I);
  
  Standard_EXPORT const TopTools_ListOfShape& ShapeSameDomain (const TopoDS_Shape& S) const;
  
  Standard_EXPORT TopTools_ListOfShape& ChangeShapeSameDomain (const TopoDS_Shape& S);
  
  Standard_EXPORT const TopTools_ListOfShape& ShapeSameDomain (const Standard_Integer I) const;
  
  Standard_EXPORT TopTools_ListOfShape& ChangeShapeSameDomain (const Standard_Integer I);
  
  Standard_EXPORT TopOpeBRepDS_MapOfIntegerShapeData& ChangeShapeData();
  
  Standard_EXPORT void AddShapeSameDomain (const TopoDS_Shape& S, const TopoDS_Shape& SSD);
  
  Standard_EXPORT void RemoveShapeSameDomain (const TopoDS_Shape& S, const TopoDS_Shape& SSD);
  
  Standard_EXPORT Standard_Integer SameDomainRef (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer SameDomainRef (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void SameDomainRef (const Standard_Integer I, const Standard_Integer Ref);
  
  Standard_EXPORT void SameDomainRef (const TopoDS_Shape& S, const Standard_Integer Ref);
  
  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri (const Standard_Integer I) const;
  
  Standard_EXPORT TopOpeBRepDS_Config SameDomainOri (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void SameDomainOri (const Standard_Integer I, const TopOpeBRepDS_Config Ori);
  
  Standard_EXPORT void SameDomainOri (const TopoDS_Shape& S, const TopOpeBRepDS_Config Ori);
  
  Standard_EXPORT Standard_Integer SameDomainInd (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer SameDomainInd (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void SameDomainInd (const Standard_Integer I, const Standard_Integer Ind);
  
  Standard_EXPORT void SameDomainInd (const TopoDS_Shape& S, const Standard_Integer Ind);
  
  Standard_EXPORT Standard_Integer AncestorRank (const Standard_Integer I) const;
  
  Standard_EXPORT Standard_Integer AncestorRank (const TopoDS_Shape& S) const;
  
  Standard_EXPORT void AncestorRank (const Standard_Integer I, const Standard_Integer Ianc);
  
  Standard_EXPORT void AncestorRank (const TopoDS_Shape& S, const Standard_Integer Ianc);
  
  Standard_EXPORT void AddShapeInterference (const TopoDS_Shape& S, const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT void RemoveShapeInterference (const TopoDS_Shape& S, const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT void FillShapesSameDomain (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT void UnfillShapesSameDomain (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! returns the shape of index I stored the maps
  Standard_EXPORT const TopoDS_Shape& Shape (const Standard_Integer I, const Standard_Boolean K = Standard_True) const;
  
  //! returns the index of shape <S>, 0 if <S> is not in the maps.
  Standard_EXPORT Standard_Integer Shape (const TopoDS_Shape& S, const Standard_Boolean K = Standard_True) const;
  
  //! Returns True if <S> has new geometries, i.e :
  //! True if S is stored and has an interference list not empty
  Standard_EXPORT Standard_Boolean HasGeometry (const TopoDS_Shape& S) const;
  
  //! Returns True if <S> is stored in the maps
  Standard_EXPORT Standard_Boolean HasShape (const TopoDS_Shape& S, const Standard_Boolean K = Standard_True) const;




protected:





private:



  TopOpeBRepDS_DoubleMapOfIntegerShape myDMOIS;
  Standard_Integer myNbDMOIS;
  TopOpeBRepDS_MapOfIntegerShapeData myIMOSD;
  TopOpeBRepDS_ListOfInterference myEmptyListOfInterference;
  TopTools_ListOfShape myEmptyListOfShape;
  TopoDS_Shape myEmptyShape;


};







#endif // _TopOpeBRepDS_DSS_HeaderFile
