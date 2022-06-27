// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TransferBRep_ShapeListBinder_HeaderFile
#define _TransferBRep_ShapeListBinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;


class TransferBRep_ShapeListBinder;
DEFINE_STANDARD_HANDLE(TransferBRep_ShapeListBinder, Transfer_Binder)

//! This binder binds several (a list of) shapes with a starting
//! entity, when this entity itself corresponds to a simple list
//! of shapes. Each part is not seen as a sub-result of an
//! independent component, but as an item of a built-in list
class TransferBRep_ShapeListBinder : public Transfer_Binder
{

public:

  
  Standard_EXPORT TransferBRep_ShapeListBinder();
  
  Standard_EXPORT TransferBRep_ShapeListBinder(const Handle(TopTools_HSequenceOfShape)& list);
  
  Standard_EXPORT virtual Standard_Boolean IsMultiple() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;
  
  //! Adds an item to the result list
  Standard_EXPORT void AddResult (const TopoDS_Shape& res);
  
  Standard_EXPORT Handle(TopTools_HSequenceOfShape) Result() const;
  
  //! Changes an already defined sub-result
  Standard_EXPORT void SetResult (const Standard_Integer num, const TopoDS_Shape& res);
  
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  Standard_EXPORT const TopoDS_Shape& Shape (const Standard_Integer num) const;
  
  Standard_EXPORT TopAbs_ShapeEnum ShapeType (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Vertex Vertex (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Edge Edge (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Wire Wire (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Face Face (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Shell Shell (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Solid Solid (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_CompSolid CompSolid (const Standard_Integer num) const;
  
  Standard_EXPORT TopoDS_Compound Compound (const Standard_Integer num) const;




  DEFINE_STANDARD_RTTIEXT(TransferBRep_ShapeListBinder,Transfer_Binder)

protected:




private:


  Handle(TopTools_HSequenceOfShape) theres;


};







#endif // _TransferBRep_ShapeListBinder_HeaderFile
