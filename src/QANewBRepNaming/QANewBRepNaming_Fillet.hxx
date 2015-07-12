// Created on: 1999-10-08
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
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

#ifndef _QANewBRepNaming_Fillet_HeaderFile
#define _QANewBRepNaming_Fillet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
class TDF_Label;
class TopoDS_Shape;
class BRepFilletAPI_MakeFillet;


//! For topological naming of a fillet
class QANewBRepNaming_Fillet  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Fillet();
  
  Standard_EXPORT QANewBRepNaming_Fillet(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  //! Loads a fillet in a data framework
  Standard_EXPORT void Load (const TopoDS_Shape& part, BRepFilletAPI_MakeFillet& mkFillet) const;
  
  //! Returns a label for deleted faces of the part.
  Standard_EXPORT TDF_Label DeletedFaces() const;
  
  //! Returns a label for modified faces of the part.
  Standard_EXPORT TDF_Label ModifiedFaces() const;
  
  //! Returns a label for faces generated from edges of the part.
  Standard_EXPORT TDF_Label FacesFromEdges() const;
  
  //! Returns a label for faces generated from vertices of the part.
  Standard_EXPORT TDF_Label FacesFromVertices() const;
  
  //! Returns a label for WorkAround Face number 1
  Standard_EXPORT TDF_Label WRFace1() const;
  
  //! Returns a label for WorkAround Face number 2
  Standard_EXPORT TDF_Label WRFace2() const;
  
  //! Returns a label for WorkAround Edge number 1
  Standard_EXPORT TDF_Label WREdge1() const;
  
  //! Returns a label for WorkAround Edge number 2
  Standard_EXPORT TDF_Label WREdge2() const;




protected:





private:





};







#endif // _QANewBRepNaming_Fillet_HeaderFile
