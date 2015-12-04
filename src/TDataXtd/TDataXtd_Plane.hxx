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

#ifndef _TDataXtd_Plane_HeaderFile
#define _TDataXtd_Plane_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
class Standard_GUID;
class TDF_Label;
class gp_Pln;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataXtd_Plane;
DEFINE_STANDARD_HANDLE(TDataXtd_Plane, TDF_Attribute)

//! The basis to define a plane attribute.
//! Warning:  Use TDataXtd_Geometry  attribute  to retrieve  the
//! gp_Pln of the Plane attribute
class TDataXtd_Plane : public TDF_Attribute
{

public:

  
  //! class methods
  //! =============
  //!
  //! Returns the GUID for plane attributes.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates the plane attribute defined by
  //! the label label.
  //! Warning
  //! If you are creating the attribute with this syntax, a
  //! planar face should already be associated with label.
  Standard_EXPORT static Handle(TDataXtd_Plane) Set (const TDF_Label& label);
  
  //! Finds,  or creates,  a Plane  attribute  and sets <P>  as
  //! generated the associated NamedShape.
  //! Plane methods
  //! =============
  Standard_EXPORT static Handle(TDataXtd_Plane) Set (const TDF_Label& label, const gp_Pln& P);
  
  Standard_EXPORT TDataXtd_Plane();
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataXtd_Plane,TDF_Attribute)

protected:




private:




};







#endif // _TDataXtd_Plane_HeaderFile
