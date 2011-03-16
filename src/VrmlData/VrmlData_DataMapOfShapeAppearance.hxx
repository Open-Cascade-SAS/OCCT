// File:      VrmlData_DataMapOfShapeAppearance.hxx
// Created:   03.06.08 10:16
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2008


#ifndef VrmlData_DataMapOfShapeAppearance_HeaderFile
#define VrmlData_DataMapOfShapeAppearance_HeaderFile

#include <NCollection_DataMap.hxx>
#include <Handle_TopoDS_TShape.hxx>
#include <VrmlData_Appearance.hxx>
#include <TopoDS_TShape.hxx>

/**
 * Data Map that provides the link between each Shape obtained in VRML import
 * and the corresponding Appearance.
 */
class VrmlData_DataMapOfShapeAppearance
  : public NCollection_DataMap<Handle_TopoDS_TShape, Handle_VrmlData_Appearance>
{
public:
  //! Constructor
  inline VrmlData_DataMapOfShapeAppearance
                (const Standard_Integer                   NbBuckets=1,
                 const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    :  NCollection_DataMap<Handle_TopoDS_TShape,
                           Handle_VrmlData_Appearance> (NbBuckets, theAllocator)
  {}

  //! Copy constructor
  inline VrmlData_DataMapOfShapeAppearance
                (const VrmlData_DataMapOfShapeAppearance& theOther)
    :  NCollection_DataMap<Handle_TopoDS_TShape,
                           Handle_VrmlData_Appearance> (theOther)
  {}
  
  
  friend Standard_Boolean IsEqual (const Handle_TopoDS_TShape& one,
                                   const Handle_TopoDS_TShape& two);

};

//=======================================================================
//function : IsEqual
//purpose  : for NCollection_DataMap interface
//=======================================================================
inline Standard_Boolean IsEqual (const Handle_TopoDS_TShape& one,
                                 const Handle_TopoDS_TShape& two) 
{
  return one == two;
}


#endif
