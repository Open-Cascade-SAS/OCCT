// Created on: 1999-09-24
// Created by: Sergey ZARITCHNY
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

#ifndef _QANewBRepNaming_TopNaming_HeaderFile
#define _QANewBRepNaming_TopNaming_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_Label.hxx>
class TDF_Label;


//! The root class for all the primitives, features, ...
class QANewBRepNaming_TopNaming 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the result label.
    const TDF_Label& ResultLabel() const;




protected:

  
  Standard_EXPORT QANewBRepNaming_TopNaming();
  
  Standard_EXPORT QANewBRepNaming_TopNaming(const TDF_Label& Label);


  TDF_Label myResultLabel;


private:





};


#include <QANewBRepNaming_TopNaming.lxx>





#endif // _QANewBRepNaming_TopNaming_HeaderFile
