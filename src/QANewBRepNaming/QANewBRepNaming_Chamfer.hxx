// Created on: 1997-09-22
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _QANewBRepNaming_Chamfer_HeaderFile
#define _QANewBRepNaming_Chamfer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
class TDF_Label;
class TopoDS_Shape;
class BRepFilletAPI_MakeChamfer;


//! To load the Chamfer results
class QANewBRepNaming_Chamfer  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Chamfer();
  
  Standard_EXPORT QANewBRepNaming_Chamfer(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Load (const TopoDS_Shape& part, BRepFilletAPI_MakeChamfer& mkChamfer) const;
  
  //! Returns the label of faces generated from edges
  Standard_EXPORT TDF_Label FacesFromEdges() const;
  
  //! Returns the label of modified faces
  Standard_EXPORT TDF_Label ModifiedFaces() const;
  
  //! Returns the label of deleted faces
  Standard_EXPORT TDF_Label DeletedFaces() const;




protected:





private:





};







#endif // _QANewBRepNaming_Chamfer_HeaderFile
