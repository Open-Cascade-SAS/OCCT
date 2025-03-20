// Created on: 1996-01-02
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepCheck_Shell_HeaderFile
#define _BRepCheck_Shell_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepCheck_Result.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shell;
class TopoDS_Shape;

class BRepCheck_Shell;
DEFINE_STANDARD_HANDLE(BRepCheck_Shell, BRepCheck_Result)

class BRepCheck_Shell : public BRepCheck_Result
{

public:
  Standard_EXPORT BRepCheck_Shell(const TopoDS_Shell& S);

  Standard_EXPORT void InContext(const TopoDS_Shape& ContextShape) Standard_OVERRIDE;

  Standard_EXPORT void Minimum() Standard_OVERRIDE;

  Standard_EXPORT void Blind() Standard_OVERRIDE;

  //! Checks if the oriented  faces of the shell  give a
  //! closed shell.    If the  wire is  closed,  returns
  //! BRepCheck_NoError.If      <Update>     is  set  to
  //! Standard_True, registers the status in the list.
  Standard_EXPORT BRepCheck_Status Closed(const Standard_Boolean Update = Standard_False);

  //! Checks if the   oriented faces  of  the shell  are
  //! correctly oriented.  An internal  call is  made to
  //! the  method  Closed.   If  <Update>    is set   to
  //! Standard_True, registers the status in the list.
  Standard_EXPORT BRepCheck_Status Orientation(const Standard_Boolean Update = Standard_False);

  Standard_EXPORT void SetUnorientable();

  Standard_EXPORT Standard_Boolean IsUnorientable() const;

  Standard_EXPORT Standard_Integer NbConnectedSet(TopTools_ListOfShape& theSets);

  DEFINE_STANDARD_RTTIEXT(BRepCheck_Shell, BRepCheck_Result)

protected:
private:
  Standard_Integer                          myNbori;
  Standard_Boolean                          myCdone;
  BRepCheck_Status                          myCstat;
  Standard_Boolean                          myOdone;
  BRepCheck_Status                          myOstat;
  TopTools_IndexedDataMapOfShapeListOfShape myMapEF;
};

#endif // _BRepCheck_Shell_HeaderFile
