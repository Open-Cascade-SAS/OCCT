// Created on: 1992-02-17
// Created by: Christian CAILLET
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

#ifndef _TransferBRep_BinderOfShape_HeaderFile
#define _TransferBRep_BinderOfShape_HeaderFile

#include <Standard.hxx>

#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>

class TransferBRep_BinderOfShape;
DEFINE_STANDARD_HANDLE(TransferBRep_BinderOfShape, Transfer_Binder)

//! Allows direct binding between a starting Object and the Result
//! of its transfer when it is Unique.
//! The Result itself is defined as a formal parameter <Shape from TopoDS>
//! Warning : While it is possible to instantiate BinderOfShape with any Type
//! for the Result, it is not advisable to instantiate it with
//! Transient Classes, because such Results are directly known and
//! managed by TransferProcess & Co, through
//! SimpleBinderOfTransient : this class looks like instantiation
//! of BinderOfShape, but its method ResultType
//! is adapted (reads DynamicType of the Result)
class TransferBRep_BinderOfShape : public Transfer_Binder
{

public:
  //! normal standard constructor, creates an empty BinderOfShape
  Standard_EXPORT TransferBRep_BinderOfShape();

  //! constructor which in the same time defines the result
  //! Returns True if a starting object is bound with SEVERAL
  //! results : Here, returns always False
  //! But it can have next results
  Standard_EXPORT TransferBRep_BinderOfShape(const TopoDS_Shape& res);

  //! Returns the Type permitted for the Result, i.e. the Type
  //! of the Parameter Class <Shape from TopoDS> (statically defined)
  Standard_EXPORT Handle(Standard_Type) ResultType() const Standard_OVERRIDE;

  //! Returns the Type Name computed for the Result (dynamic)
  Standard_EXPORT Standard_CString ResultTypeName() const Standard_OVERRIDE;

  //! Defines the Result
  Standard_EXPORT void SetResult(const TopoDS_Shape& res);

  //! Returns the defined Result, if there is one
  Standard_EXPORT const TopoDS_Shape& Result() const;

  //! Returns the defined Result, if there is one, and allows to
  //! change it (avoids Result + SetResult).
  //! Admits that Result can be not yet defined
  //! Warning : a call to CResult causes Result to be known as defined
  Standard_EXPORT TopoDS_Shape& CResult();

  DEFINE_STANDARD_RTTIEXT(TransferBRep_BinderOfShape, Transfer_Binder)

protected:
private:
  TopoDS_Shape theres;
};

#endif // _TransferBRep_BinderOfShape_HeaderFile
