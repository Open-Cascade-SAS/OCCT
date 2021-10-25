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

#ifndef _IGESSolid_ManifoldSolid_HeaderFile
#define _IGESSolid_ManifoldSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IGESSolid_HArray1OfShell.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESSolid_Shell;


class IGESSolid_ManifoldSolid;
DEFINE_STANDARD_HANDLE(IGESSolid_ManifoldSolid, IGESData_IGESEntity)

//! defines ManifoldSolid, Type <186> Form Number <0>
//! in package IGESSolid
//! A manifold solid is a bounded, closed, and finite volume
//! in three dimensional Euclidean space
class IGESSolid_ManifoldSolid : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_ManifoldSolid();
  
  //! This method is used to set the fields of the class
  //! ManifoldSolid
  //! - aShell         : pointer to the shell
  //! - shellflag      : orientation flag of shell
  //! - voidShells     : the void shells
  //! - voidShellFlags : orientation of the void shells
  //! raises exception if length of voidShells and voidShellFlags
  //! do not match
  Standard_EXPORT void Init (const Handle(IGESSolid_Shell)& aShell, const Standard_Boolean shellflag, const Handle(IGESSolid_HArray1OfShell)& voidShells, const Handle(TColStd_HArray1OfInteger)& voidShellFlags);
  
  //! returns the Shell entity which is being referred
  Standard_EXPORT Handle(IGESSolid_Shell) Shell() const;
  
  //! returns the orientation flag of the shell
  Standard_EXPORT Standard_Boolean OrientationFlag() const;
  
  //! returns the number of void shells
  Standard_EXPORT Standard_Integer NbVoidShells() const;
  
  //! returns Index'th void shell.
  //! raises exception if Index  <= 0 or Index > NbVoidShells()
  Standard_EXPORT Handle(IGESSolid_Shell) VoidShell (const Standard_Integer Index) const;
  
  //! returns Index'th orientation flag.
  //! raises exception if Index  <= 0 or Index > NbVoidShells()
  Standard_EXPORT Standard_Boolean VoidOrientationFlag (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_ManifoldSolid,IGESData_IGESEntity)

protected:




private:


  Handle(IGESSolid_Shell) theShell;
  Standard_Boolean theOrientationFlag;
  Handle(IGESSolid_HArray1OfShell) theVoidShells;
  Handle(TColStd_HArray1OfInteger) theOrientFlags;


};







#endif // _IGESSolid_ManifoldSolid_HeaderFile
