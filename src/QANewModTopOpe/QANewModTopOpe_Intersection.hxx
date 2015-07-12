// Created on: 2000-12-25
// Created by: Igor FEOKTISTOV <ifv@nnov.matra-dtv.fr>
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_Intersection_HeaderFile
#define _QANewModTopOpe_Intersection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;


//! provides  intersection  of  two  shapes;
class QANewModTopOpe_Intersection  : public BRepAlgoAPI_BooleanOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewModTopOpe_Intersection(const TopoDS_Shape& theObject1, const TopoDS_Shape& theObject2);
  
  //! Returns the list  of shapes generated from the shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns true if there is at least one generated shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& aS) Standard_OVERRIDE;
  
  //! Returns true if there is at least one deleted shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const Standard_OVERRIDE;




protected:





private:



  TopTools_DataMapOfShapeListOfShape myMapGener;


};







#endif // _QANewModTopOpe_Intersection_HeaderFile
