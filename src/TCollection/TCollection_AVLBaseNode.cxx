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


