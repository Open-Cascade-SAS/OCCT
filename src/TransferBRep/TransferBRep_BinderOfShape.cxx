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


#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_TransferFailure.hxx>
#include <TransferBRep_BinderOfShape.hxx>
#include <TransferBRep_ShapeInfo.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TransferBRep_BinderOfShape,Transfer_Binder)

TransferBRep_BinderOfShape::TransferBRep_BinderOfShape (){ }

TransferBRep_BinderOfShape::TransferBRep_BinderOfShape (const TopoDS_Shape& res)
: theres (res)
{ SetResultPresent();  }


//    Standard_Boolean  TransferBRep_BinderOfShape::IsMultiple() const
//      {  return Standard_False;  }


Handle(Standard_Type)  TransferBRep_BinderOfShape::ResultType () const
{  return  TransferBRep_ShapeInfo::Type (theres);  }  // correspond a "STANDARD_TYPE(TopoDS_Shape)"

Standard_CString  TransferBRep_BinderOfShape::ResultTypeName () const
{  return  TransferBRep_ShapeInfo::TypeName (theres);  }  // correspond a "STANDARD_TYPE(TopoDS_Shape)"


void  TransferBRep_BinderOfShape::SetResult (const TopoDS_Shape& res)
{
  SetResultPresent();
  theres = res;
}

const TopoDS_Shape&  TransferBRep_BinderOfShape::Result () const
{  return theres;  }

TopoDS_Shape&  TransferBRep_BinderOfShape::CResult ()
{  SetResultPresent(); return theres;  }
