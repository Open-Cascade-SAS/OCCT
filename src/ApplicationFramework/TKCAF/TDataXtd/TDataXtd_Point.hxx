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

#ifndef _TDataXtd_Point_HeaderFile
#define _TDataXtd_Point_HeaderFile

#include <TDataStd_GenericEmpty.hxx>
class TDF_Label;
class gp_Pnt;

class TDataXtd_Point;
DEFINE_STANDARD_HANDLE(TDataXtd_Point, TDataStd_GenericEmpty)

//! The basis to define a point attribute.
//! The topological attribute must contain a vertex.
//! You use this class to create reference points in a design.
//!
//! Warning:  Use TDataXtd_Geometry  attribute  to retrieve the
//! gp_Pnt of the Point attribute
class TDataXtd_Point : public TDataStd_GenericEmpty
{

public:
  //! class methods
  //! =============
  //!
  //! Returns the GUID for point attributes.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Sets the label Label as a point attribute.
  //! If no object is found, a point attribute is created.
  Standard_EXPORT static Handle(TDataXtd_Point) Set(const TDF_Label& label);

  //! Sets the label Label as a point attribute containing the point P.
  //! If no object is found, a point attribute is created.
  //! Point methods
  //! =============
  Standard_EXPORT static Handle(TDataXtd_Point) Set(const TDF_Label& label, const gp_Pnt& P);

  Standard_EXPORT TDataXtd_Point();

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  DEFINE_DERIVED_ATTRIBUTE(TDataXtd_Point, TDataStd_GenericEmpty)

protected:
private:
};

#endif // _TDataXtd_Point_HeaderFile
