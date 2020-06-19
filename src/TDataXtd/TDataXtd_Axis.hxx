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

#ifndef _TDataXtd_Axis_HeaderFile
#define _TDataXtd_Axis_HeaderFile

#include <TDataStd_GenericEmpty.hxx>
class TDF_Label;
class gp_Lin;

class TDataXtd_Axis;
DEFINE_STANDARD_HANDLE(TDataXtd_Axis, TDataStd_GenericEmpty)

//! The basis to define an axis attribute.
//!
//! Warning: Use TDataXtd_Geometry  attribute  to retrieve  the
//! gp_Lin of the Axis attribute
class TDataXtd_Axis : public TDataStd_GenericEmpty
{

public:

  
  //! class methods
  //! =============
  //! Returns the GUID for an axis.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates an axis attribute defined by the  label.
  //! In the case of a creation of an axis, a compatible
  //! named shape should already be associated with label.
  //! Exceptions
  //! Standard_NullObject if no compatible named
  //! shape is associated with the label.
  Standard_EXPORT static Handle(TDataXtd_Axis) Set (const TDF_Label& label);
  
  //! Find,  or create,  an Axis  attribute  and set <P>  as
  //! generated in the associated NamedShape.
  //! Axis methods
  //! ============
  Standard_EXPORT static Handle(TDataXtd_Axis) Set (const TDF_Label& label, const gp_Lin& L);
  
  Standard_EXPORT TDataXtd_Axis();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;


  DEFINE_DERIVED_ATTRIBUTE(TDataXtd_Axis, TDataStd_GenericEmpty)

protected:




private:




};







#endif // _TDataXtd_Axis_HeaderFile
