// Created on: 2015-05-14
// Created by: Ilya Novikov
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_Editor_HeaderFile
#define _XCAFDoc_Editor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <Standard_Boolean.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>



//! Tool for edit structure of document.
class XCAFDoc_Editor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Convert Shape(compound) to assembly
  Standard_EXPORT static   Standard_Boolean Expand (const TDF_Label& Doc, const TDF_Label& Shape, const Standard_Boolean recursively = Standard_True) ;
  
  //! Convert all compounds in Doc to assembly
  Standard_EXPORT static   Standard_Boolean Expand (const TDF_Label& Doc, const Standard_Boolean recursively = Standard_True) ;




protected:





private:

  
  //! Get colors, layers and name from Label
  Standard_EXPORT static   Standard_Boolean getParams (const TDF_Label& Doc, const TDF_Label& Label, TDF_LabelSequence& Colors, TDF_LabelSequence& Layers, Handle(TDataStd_Name)& Name) ;
  
  //! Set colors, layers and name from Label
  Standard_EXPORT static   Standard_Boolean setParams (const TDF_Label& Doc, const TDF_Label& Label, const TDF_LabelSequence& Colors, const TDF_LabelSequence& Layers, const Handle(TDataStd_Name)& Name) ;




};







#endif // _XCAFDoc_Editor_HeaderFile
