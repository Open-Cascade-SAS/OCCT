// Created on: 2014-10-15
// Created by: Denis Bogolepov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef OpenGl_Quadric_Header
#define OpenGl_Quadric_Header

#include <OpenGl_PrimitiveArray.hxx>

//! Class for rendering of arbitrary primitive array.
//! Tool class for generating tessellation of quadric surface.
class OpenGl_Quadric : public OpenGl_PrimitiveArray
{
public:

  //! Creates undefined abstract quadric surface
  Standard_EXPORT OpenGl_Quadric();

  //! Returns true if quadric has been defined
  Standard_Boolean IsDefined() const { return myNbSlices != 0; }

  //! Returns number of slices
  Standard_Integer NbSlices() const { return myNbSlices; }

  //! Returns number of stacks
  Standard_Integer NbStacks() const { return myNbStacks; }

  //! Returns total number of vertices
  Standard_Integer NbVertices() const { return (myNbSlices + 1) * (myNbStacks + 1); }

  //! Returns total number of triangles
  Standard_Integer NbTriangles() const { return myNbSlices * myNbStacks * 2; }

  //! Release GL resources
  virtual void Release (OpenGl_Context* theContext) Standard_OVERRIDE;

protected:

  //! Returns surface point for the given parameters.
  virtual OpenGl_Vec3 evalVertex (const Standard_ShortReal theU,
                                  const Standard_ShortReal theV) const = 0;

  //! Returns surface normal for the given parameters.
  virtual OpenGl_Vec3 evalNormal (const Standard_ShortReal theU,
                                  const Standard_ShortReal theV) const = 0;

  //! Initialize primitive.
  Standard_EXPORT Standard_Boolean init (const Standard_Integer theNbSlices,
                                         const Standard_Integer theNbStacks);

private:

  //! Initialize arrays.
  Standard_Boolean createArrays() const;

private:

  Standard_Integer myNbSlices; //!< Number of slices (u partitions)
  Standard_Integer myNbStacks; //!< Number of stacks (v partitions)

};

#endif // OpenGl_Quadric_Header
