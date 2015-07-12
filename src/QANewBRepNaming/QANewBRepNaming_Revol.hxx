// Created on: 1999-11-05
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

#ifndef _QANewBRepNaming_Revol_HeaderFile
#define _QANewBRepNaming_Revol_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <QANewBRepNaming_TopNaming.hxx>
class TDF_Label;
class BRepPrimAPI_MakeRevol;
class TopoDS_Shape;


//! To load the Revol results
class QANewBRepNaming_Revol  : public QANewBRepNaming_TopNaming
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT QANewBRepNaming_Revol();
  
  Standard_EXPORT QANewBRepNaming_Revol(const TDF_Label& ResultLabel);
  
  Standard_EXPORT void Init (const TDF_Label& ResultLabel);
  
  //! Loads the revol in the data framework
  Standard_EXPORT void Load (BRepPrimAPI_MakeRevol& mkRevol, const TopoDS_Shape& basis) const;
  
  //! Returns the insertion label of the start face of the Revol.
  Standard_EXPORT TDF_Label Start() const;
  
  //! Returns the insertion label of the end face of the Revol.
  Standard_EXPORT TDF_Label End() const;
  
  //! Returns the insertion label of the lateral faces of the Revol.
  Standard_EXPORT TDF_Label Lateral() const;
  
  //! Returns the label of degenerated edges.
  Standard_EXPORT TDF_Label Degenerated() const;
  
  //! Returns the label of the content of the result.
  Standard_EXPORT TDF_Label Content() const;




protected:





private:





};







#endif // _QANewBRepNaming_Revol_HeaderFile
