// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_IGESEntity.hxx>
#include <IGESSolid_Face.hxx>
#include <IGESSolid_Loop.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Face,IGESData_IGESEntity)

IGESSolid_Face::IGESSolid_Face ()    {  }


    void  IGESSolid_Face::Init
  (const Handle(IGESData_IGESEntity)&     aSurface,
   const Standard_Boolean                 OuterLoopFlag,
   const Handle(IGESSolid_HArray1OfLoop)& Loops)
{
  if (Loops->Lower() != 1)
    throw Standard_DimensionMismatch("IGESSolid_Face : Init");
  theSurface   = aSurface;
  hasOuterLoop = OuterLoopFlag;
  theLoops     = Loops;
  InitTypeAndForm(510,1);
}

    Handle(IGESData_IGESEntity)  IGESSolid_Face::Surface () const
{
  return theSurface;
}

    Standard_Integer  IGESSolid_Face::NbLoops () const
{
  return theLoops->Length();
}

    Standard_Boolean  IGESSolid_Face::HasOuterLoop () const
{
  return hasOuterLoop;
}

    Handle(IGESSolid_Loop)  IGESSolid_Face::Loop (const Standard_Integer Index) const
{
  return theLoops->Value(Index);
}

