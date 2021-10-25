// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESSolid_Shell_HeaderFile
#define _IGESSolid_Shell_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESSolid_HArray1OfFace.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class IGESSolid_Face;


class IGESSolid_Shell;
DEFINE_STANDARD_HANDLE(IGESSolid_Shell, IGESData_IGESEntity)

//! defines Shell, Type <514> Form Number <1>
//! in package IGESSolid
//! Shell entity is a connected entity of dimensionality 2
//! which divides R3 into two arcwise connected open subsets,
//! one of which is finite. Inside of the shell is defined to
//! be the finite region.
//! From IGES-5.3, Form can be <1> for Closed or <2> for Open
class IGESSolid_Shell : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_Shell();
  
  //! This method is used to set the fields of the class Shell
  //! - allFaces  : the faces comprising the shell
  //! - allOrient : the orientation flags of the shell
  //! raises exception if length of allFaces & allOrient do not match
  Standard_EXPORT void Init (const Handle(IGESSolid_HArray1OfFace)& allFaces, const Handle(TColStd_HArray1OfInteger)& allOrient);
  
  //! Tells if a Shell is Closed, i.e. if its FormNumber is 1
  //! (this is the default)
  Standard_EXPORT Standard_Boolean IsClosed() const;
  
  //! Sets or Unsets the Closed status (FormNumber = 1 else 2)
  Standard_EXPORT void SetClosed (const Standard_Boolean closed);
  
  //! returns the number of the face entities in the shell
  Standard_EXPORT Standard_Integer NbFaces() const;
  
  //! returns the Index'th face entity of the shell
  //! raises exception if Index <= 0 or Index > NbFaces()
  Standard_EXPORT Handle(IGESSolid_Face) Face (const Standard_Integer Index) const;
  
  //! returns the orientation of Index'th face w.r.t the direction of
  //! the underlying surface
  //! raises exception if Index <= 0 or Index > NbFaces()
  Standard_EXPORT Standard_Boolean Orientation (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_Shell,IGESData_IGESEntity)

protected:




private:


  Handle(IGESSolid_HArray1OfFace) theFaces;
  Handle(TColStd_HArray1OfInteger) theOrientation;


};







#endif // _IGESSolid_Shell_HeaderFile
