// Created on: 2008-06-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_DataMapOfShapeAppearance_HeaderFile
#define VrmlData_DataMapOfShapeAppearance_HeaderFile

#include <NCollection_DataMap.hxx>
#include <VrmlData_Appearance.hxx>
#include <TopoDS_TShape.hxx>

/**
 * Data Map that provides the link between each Shape obtained in VRML import
 * and the corresponding Appearance.
 */
class VrmlData_DataMapOfShapeAppearance
  : public NCollection_DataMap<Handle(TopoDS_TShape), Handle(VrmlData_Appearance)>
{
public:
  //! Constructor
  inline VrmlData_DataMapOfShapeAppearance
                (const Standard_Integer                   NbBuckets=1,
                 const Handle(NCollection_BaseAllocator)& theAllocator = 0L)
    :  NCollection_DataMap<Handle(TopoDS_TShape),
                           Handle(VrmlData_Appearance)> (NbBuckets, theAllocator)
  {}

  //! Copy constructor
  inline VrmlData_DataMapOfShapeAppearance
                (const VrmlData_DataMapOfShapeAppearance& theOther)
    :  NCollection_DataMap<Handle(TopoDS_TShape),
                           Handle(VrmlData_Appearance)> (theOther)
  {}
  
  
  friend Standard_Boolean IsEqual (const Handle(TopoDS_TShape)& one,
                                   const Handle(TopoDS_TShape)& two);

};

//=======================================================================
//function : IsEqual
//purpose  : for NCollection_DataMap interface
//=======================================================================
inline Standard_Boolean IsEqual (const Handle(TopoDS_TShape)& one,
                                 const Handle(TopoDS_TShape)& two) 
{
  return one == two;
}


#endif
