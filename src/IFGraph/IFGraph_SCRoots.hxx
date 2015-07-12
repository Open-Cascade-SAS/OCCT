// Created on: 1992-09-23
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFGraph_SCRoots_HeaderFile
#define _IFGraph_SCRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IFGraph_StrongComponants.hxx>
#include <Standard_Boolean.hxx>
class Interface_Graph;
class IFGraph_StrongComponants;


//! determines strong componants in a graph which are Roots
class IFGraph_SCRoots  : public IFGraph_StrongComponants
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates with a Graph, and will analyse :
  //! whole True  : all the contents of the Model
  //! whole False : sub-parts which will be given later
  Standard_EXPORT IFGraph_SCRoots(const Interface_Graph& agraph, const Standard_Boolean whole);
  
  //! creates from a StrongComponants which was already computed
  Standard_EXPORT IFGraph_SCRoots(IFGraph_StrongComponants& subparts);
  
  //! does the computation
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;




protected:





private:





};







#endif // _IFGraph_SCRoots_HeaderFile
