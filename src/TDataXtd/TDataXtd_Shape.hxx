// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Shape_HeaderFile
#define _TDataXtd_Shape_HeaderFile

#include <TDataStd_GenericEmpty.hxx>
class TopoDS_Shape;
class TDF_DataSet;


class TDataXtd_Shape;
DEFINE_STANDARD_HANDLE(TDataXtd_Shape, TDataStd_GenericEmpty)

//! A Shape is associated in the framework with :
//! a NamedShape attribute
class TDataXtd_Shape : public TDataStd_GenericEmpty
{

public:

  
  //! class methods
  //! =============
  //! try to retrieve a Shape attribute at <current> label
  //! or in  fathers  label of  <current>. Returns True  if
  //! found and set <S>.
  Standard_EXPORT static Standard_Boolean Find (const TDF_Label& current, Handle(TDataXtd_Shape)& S);
  
  //! Find, or create, a Shape attribute.  the Shape attribute
  //! is returned. Raises if <label> has attribute.
  Standard_EXPORT static Handle(TDataXtd_Shape) New (const TDF_Label& label);
  
  //! Create or update associated NamedShape attribute.  the
  //! Shape attribute is returned.
  Standard_EXPORT static Handle(TDataXtd_Shape) Set (const TDF_Label& label, const TopoDS_Shape& shape);
  
  //! the Shape from  associated NamedShape attribute
  //! is returned.
  Standard_EXPORT static TopoDS_Shape Get (const TDF_Label& label);
  
  //! Shape methods
  //! ============
  Standard_EXPORT static const Standard_GUID& GetID();
  
  Standard_EXPORT TDataXtd_Shape();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& DS) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(TDataXtd_Shape, TDataStd_GenericEmpty)

protected:




private:




};







#endif // _TDataXtd_Shape_HeaderFile
