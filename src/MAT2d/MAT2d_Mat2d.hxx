// Created on: 1992-09-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _MAT2d_Mat2d_HeaderFile
#define _MAT2d_Mat2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <MAT_DataMapOfIntegerBisector.hxx>
#include <Standard_Real.hxx>
class MAT_ListOfEdge;
class MAT_ListOfBisector;
class MAT2d_Tool2d;
class MAT_Bisector;


//! this class contains the generic algorithm of
//! computation of the bisecting locus.
class MAT2d_Mat2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT MAT2d_Mat2d(const Standard_Boolean IsOpenResult = Standard_False);
  
  Standard_EXPORT ~MAT2d_Mat2d();
  
  //! Algorithm of computation of the bisecting locus.
  Standard_EXPORT void CreateMat (MAT2d_Tool2d& aTool);
  
  //! Algorithm of computation of the bisecting locus for
  //! open wire.
  Standard_EXPORT void CreateMatOpen (MAT2d_Tool2d& aTool);
  
  //! Returns <TRUE> if CreateMat has succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Initialize an iterator on the set of the roots
  //! of the trees of bisectors.
  Standard_EXPORT void Init();
  
  //! Return False if there is no more roots.
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Move to the next root.
  Standard_EXPORT void Next();
  
  //! Returns the current root.
  Standard_EXPORT Handle(MAT_Bisector) Bisector() const;
  
  //! Returns True if there are semi_infinite bisectors.
  //! So there is a tree for each semi_infinte bisector.
  Standard_EXPORT Standard_Boolean SemiInfinite() const;
  
  //! Returns the total number of bisectors.
  Standard_EXPORT Standard_Integer NumberOfBisectors() const;




protected:





private:

  
  Standard_EXPORT void LoadBisectorsToRemove (Standard_Integer& noofbisectorstoremove, const Standard_Real distance1, const Standard_Real distance2, const Handle(MAT_Bisector)& bisector1, const Handle(MAT_Bisector)& bisector2, const Handle(MAT_Bisector)& bisector3, const Handle(MAT_Bisector)& bisector4);
  
  Standard_EXPORT void Intersect (MAT2d_Tool2d& atool, const Standard_Integer aside, Standard_Integer& noofbisectorstoremove, const Handle(MAT_Bisector)& bisector1, const Handle(MAT_Bisector)& bisector2);


  Standard_Boolean myIsOpenResult;
  Standard_Integer thenumberofbisectors;
  Standard_Integer thenumberofedges;
  Standard_Boolean semiInfinite;
  Handle(MAT_ListOfEdge) theedgelist;
  Handle(MAT_ListOfEdge) RemovedEdgesList;
  TColStd_DataMapOfIntegerInteger typeofbisectortoremove;
  MAT_DataMapOfIntegerBisector bisectoronetoremove;
  MAT_DataMapOfIntegerBisector bisectortwotoremove;
  MAT_DataMapOfIntegerBisector bisectormap;
  Handle(MAT_ListOfBisector) roots;
  Standard_Boolean isDone;


};







#endif // _MAT2d_Mat2d_HeaderFile
