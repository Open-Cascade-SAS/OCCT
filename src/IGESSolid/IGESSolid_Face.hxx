// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (SIVA)
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESSolid_Face_HeaderFile
#define _IGESSolid_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <IGESSolid_HArray1OfLoop.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class Standard_OutOfRange;
class IGESSolid_Loop;


class IGESSolid_Face;
DEFINE_STANDARD_HANDLE(IGESSolid_Face, IGESData_IGESEntity)

//! defines Face, Type <510> Form Number <1>
//! in package IGESSolid
//! Face entity is a bound (partial) which has finite area
class IGESSolid_Face : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESSolid_Face();
  
  //! This method is used to set the fields of the class Face
  //! - aSurface      : Pointer to the underlying surface
  //! - outerLoopFlag : True means the first loop is the outer loop
  //! - loops         : Array of loops bounding the face
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aSurface, const Standard_Boolean outerLoopFlag, const Handle(IGESSolid_HArray1OfLoop)& loops);
  
  //! returns the underlying surface of the face
  Standard_EXPORT Handle(IGESData_IGESEntity) Surface() const;
  
  //! returns the number of the loops bounding the face
  Standard_EXPORT Standard_Integer NbLoops() const;
  
  //! checks whether there is an outer loop or not
  Standard_EXPORT Standard_Boolean HasOuterLoop() const;
  
  //! returns the Index'th loop that bounds the face
  //! raises exception if Index < 0 or Index >= NbLoops
  Standard_EXPORT Handle(IGESSolid_Loop) Loop (const Standard_Integer Index) const;




  DEFINE_STANDARD_RTTIEXT(IGESSolid_Face,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theSurface;
  Standard_Boolean hasOuterLoop;
  Handle(IGESSolid_HArray1OfLoop) theLoops;


};







#endif // _IGESSolid_Face_HeaderFile
