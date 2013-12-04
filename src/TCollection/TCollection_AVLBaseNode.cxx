// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <TCollection_AVLBaseNode.ixx>

Standard_Integer TCollection_AVLBaseNode::Height(const TCollection_AVLBaseNodePtr& ANode)
     // Length of the longest child
{
  if (!ANode) return 0;
  else return (1 + Max(Height(ANode->myLeft),Height(ANode->myRight)));
}


Standard_Integer TCollection_AVLBaseNode::RecursiveExtent(const TCollection_AVLBaseNodePtr &  ANode) 
     // Number of different items in the current tree
{
  if ( ! ANode ) return 0;
  else return  (1 +  RecursiveExtent(ANode->myLeft)   
		+  RecursiveExtent(ANode->myRight)  );
}

Standard_Integer TCollection_AVLBaseNode::RecursiveTotalExtent(const TCollection_AVLBaseNodePtr&  ANode) 
{
  // Number of different items in the current tree according to
  // the multiplicity
  if ( ! ANode  ) return 0;
  else return ( RecursiveTotalExtent(ANode->myLeft) + RecursiveTotalExtent(ANode->myRight) + ANode->myCount);
}


