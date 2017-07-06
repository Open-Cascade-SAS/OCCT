// Created on: 1998-01-19
// Created by: Kernel
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TCollection_MapNode_HeaderFile
#define _TCollection_MapNode_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_MapNodePtr.hxx>
#include <Standard_Transient.hxx>


class TCollection_MapNode;
DEFINE_STANDARD_HANDLE(TCollection_MapNode, Standard_Transient)

//! Basic class root of all the Maps.
class TCollection_MapNode : public Standard_Transient
{

public:

  
    TCollection_MapNode(const TCollection_MapNodePtr& n);
  
    TCollection_MapNodePtr& Next() const;




  DEFINE_STANDARD_RTTIEXT(TCollection_MapNode,Standard_Transient)

protected:




private:


  TCollection_MapNodePtr myNext;


};


#include <TCollection_MapNode.lxx>





#endif // _TCollection_MapNode_HeaderFile
