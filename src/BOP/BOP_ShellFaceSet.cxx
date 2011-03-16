#include <BOP_ShellFaceSet.ixx>

// =============================================================================
// function: BOP_ShellFaceSet::BOP_ShellFaceSet
// purpose: 
// =============================================================================
  BOP_ShellFaceSet::BOP_ShellFaceSet() :
  BOP_ShapeSet(TopAbs_EDGE)
{
}

// =============================================================================
// function: BOP_ShellFaceSet::BOP_ShellFaceSet
// purpose: 
// =============================================================================
  BOP_ShellFaceSet::BOP_ShellFaceSet(const TopoDS_Solid& theSolid) :
  BOP_ShapeSet(TopAbs_EDGE)
{
  mySolid = theSolid;
}
