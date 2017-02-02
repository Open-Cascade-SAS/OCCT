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

#include <IGESSolid_Face.hxx>
#include <IGESSolid_Shell.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_Shell,IGESData_IGESEntity)

IGESSolid_Shell::IGESSolid_Shell ()    {  }


    void  IGESSolid_Shell::Init
  (const Handle(IGESSolid_HArray1OfFace)& Faces,
   const Handle(TColStd_HArray1OfInteger)& Orient)
{
  if (Faces->Lower()  != 1 || Orient->Lower() != 1 ||
      Faces->Length() != Orient->Length())
    throw Standard_DimensionError("IGESSolid_Shell : Init");

  theFaces = Faces;
  theOrientation = Orient;
  InitTypeAndForm(514,1);
}

    Standard_Boolean  IGESSolid_Shell::IsClosed () const
      {  return (FormNumber() == 1);  }

    void  IGESSolid_Shell::SetClosed (const Standard_Boolean closed)
      {  InitTypeAndForm(514, (closed ? 1 : 2));  }


    Standard_Integer  IGESSolid_Shell::NbFaces () const
{
  return theFaces->Length();
}

    Handle(IGESSolid_Face) IGESSolid_Shell::Face (const Standard_Integer Index) const
{
  return theFaces->Value(Index);
}

    Standard_Boolean IGESSolid_Shell::Orientation
  (const Standard_Integer Index) const
{
  return (theOrientation->Value(Index) != 0);
}
