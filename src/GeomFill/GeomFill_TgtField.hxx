// Created on: 1995-12-04
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomFill_TgtField_HeaderFile
#define _GeomFill_TgtField_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
class Law_BSpline;
class gp_Vec;


class GeomFill_TgtField;
DEFINE_STANDARD_HANDLE(GeomFill_TgtField, Standard_Transient)

//! Root class defining the methods we need to make an
//! algorithmic tangents field.
class GeomFill_TgtField : public Standard_Transient
{

public:

  
  Standard_EXPORT virtual Standard_Boolean IsScalable() const;
  
  Standard_EXPORT virtual void Scale (const Handle(Law_BSpline)& Func);
  
  //! Computes  the value  of the    field of tangency    at
  //! parameter W.
  Standard_EXPORT virtual gp_Vec Value (const Standard_Real W) const = 0;
  
  //! Computes the  derivative of  the field of  tangency at
  //! parameter W.
  Standard_EXPORT virtual gp_Vec D1 (const Standard_Real W) const = 0;
  
  //! Computes the value and the  derivative of the field of
  //! tangency at parameter W.
  Standard_EXPORT virtual void D1 (const Standard_Real W, gp_Vec& V, gp_Vec& DV) const = 0;




  DEFINE_STANDARD_RTTIEXT(GeomFill_TgtField,Standard_Transient)

protected:




private:




};







#endif // _GeomFill_TgtField_HeaderFile
