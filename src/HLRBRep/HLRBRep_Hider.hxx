// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef _HLRBRep_Hider_HeaderFile
#define _HLRBRep_Hider_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <BRepTopAdaptor_MapOfShapeTool.hxx>
class HLRBRep_Data;



class HLRBRep_Hider 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Hider processing  the set  of  Edges and
  //! hiding faces described by <DS>.  Stores the hidden
  //! parts in <DS>.
  Standard_EXPORT HLRBRep_Hider(const Handle(HLRBRep_Data)& DS);
  
  //! own hiding the side face number <FI>.
  Standard_EXPORT void OwnHiding (const Standard_Integer FI);
  
  //! Removes from the edges,   the parts hidden by  the
  //! hiding face number <FI>.
  Standard_EXPORT void Hide (const Standard_Integer FI, BRepTopAdaptor_MapOfShapeTool& MST);




protected:





private:



  Handle(HLRBRep_Data) myDS;


};







#endif // _HLRBRep_Hider_HeaderFile
