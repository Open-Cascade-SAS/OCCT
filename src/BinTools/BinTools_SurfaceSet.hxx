// Created on: 2004-05-20
// Created by: Sergey ZARITCHNY <szy@opencascade.com>
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BinTools_SurfaceSet_HeaderFile
#define _BinTools_SurfaceSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_IndexedMapOfTransient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
class Standard_OutOfRange;
class Geom_Surface;


//! Stores a set of Surfaces from Geom in binary format.
class BinTools_SurfaceSet 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns an empty set of Surfaces.
  Standard_EXPORT BinTools_SurfaceSet();
  
  //! Clears the content of the set.
  Standard_EXPORT void Clear();
  
  //! Incorporate a new Surface in the  set and returns
  //! its index.
  Standard_EXPORT Standard_Integer Add (const Handle(Geom_Surface)& S);
  
  //! Returns the Surface of index <I>.
  Standard_EXPORT Handle(Geom_Surface) Surface (const Standard_Integer I) const;
  
  //! Returns the index of <L>.
  Standard_EXPORT Standard_Integer Index (const Handle(Geom_Surface)& S) const;
  
  //! Writes the content of  me  on the stream <OS> in
  //! binary format that can be read back by Read.
  Standard_EXPORT void Write (Standard_OStream& OS) const;
  
  //! Reads the content of me from the  stream  <IS>. me
  //! is first cleared.
  Standard_EXPORT void Read (Standard_IStream& IS);
  
  //! Dumps the surface on the stream in binary
  //! format that can be read back.
  Standard_EXPORT static void WriteSurface (const Handle(Geom_Surface)& S, Standard_OStream& OS);
  
  //! Reads the surface  from  the stream.  The  surface  is
  //! assumed   to have  been  written  with  the Write
  //! method.
  Standard_EXPORT static Standard_IStream& ReadSurface (Standard_IStream& IS, Handle(Geom_Surface)& S);




protected:





private:



  TColStd_IndexedMapOfTransient myMap;


};







#endif // _BinTools_SurfaceSet_HeaderFile
