// Created on: 2008-06-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
