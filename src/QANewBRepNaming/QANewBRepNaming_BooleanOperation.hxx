// Created on: 1999-11-11
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

#ifndef _QANewBRepNaming_BooleanOperation_HeaderFile
#define _QANewBRepNaming_BooleanOperation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
class TDF_Label;
class BRepAlgo_BooleanOperation;


//! To load the BooleanOperation results
class QANewBRepNaming_BooleanOperation  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_BooleanOperation();
  
  Standard_EXPORT QANewBRepNaming_BooleanOperation(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  //! Load the boolean operation.
  //! Makes a new part attached to the ResultLabel.
  Standard_EXPORT void Load (BRepAlgo_BooleanOperation& mkBoolOp) const;
  
  //! Returns the label of the modified faces
  //! of the first shape (S1).
  Standard_EXPORT TDF_Label FirstModified() const;
  
  //! Returns the label of the deleted faces
  //! of the first shape (S1).
  Standard_EXPORT TDF_Label FirstDeleted() const;
  
  //! Returns the label of the modified faces
  //! of the second shape (S2).
  Standard_EXPORT TDF_Label SecondModified() const;
  
  //! Returns the label of the deleted faces
  //! of the second shape (S2).
  Standard_EXPORT TDF_Label SecondDeleted() const;
  
  //! Returns the label of intersections
  Standard_EXPORT TDF_Label Intersections() const;




protected:





private:





};







#endif // _QANewBRepNaming_BooleanOperation_HeaderFile
