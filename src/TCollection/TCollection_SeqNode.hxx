// Created on: 1998-01-21
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

#ifndef _TCollection_SeqNode_HeaderFile
#define _TCollection_SeqNode_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_SeqNodePtr.hxx>
#include <Standard_Transient.hxx>


class TCollection_SeqNode;
DEFINE_STANDARD_HANDLE(TCollection_SeqNode, Standard_Transient)


class TCollection_SeqNode : public Standard_Transient
{

public:

  
    TCollection_SeqNode(const TCollection_SeqNodePtr& n, const TCollection_SeqNodePtr& p);
  
    TCollection_SeqNodePtr& Next() const;
  
    TCollection_SeqNodePtr& Previous() const;




  DEFINE_STANDARD_RTTIEXT(TCollection_SeqNode,Standard_Transient)

protected:




private:


  TCollection_SeqNodePtr myNext;
  TCollection_SeqNodePtr myPrevious;


};


#include <TCollection_SeqNode.lxx>





#endif // _TCollection_SeqNode_HeaderFile
