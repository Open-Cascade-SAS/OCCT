// ----------------------------------------------------------------------
// Internal C++ class that provides tool to manipulate a tree node.
// ----------------------------------------------------------------------

//void ShallowDump(const Item&, Standard_OStream& );

class AVLNode{
  public :

  Item      Value;
  AVLNode*  Left ;
  AVLNode*  Right;
  Standard_Integer   Count;

  friend class AVLList;       // For iterator 
};
