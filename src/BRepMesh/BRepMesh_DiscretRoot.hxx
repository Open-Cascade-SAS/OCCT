// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_DiscretRoot_HeaderFile
#define _BRepMesh_DiscretRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <Message_ProgressRange.hxx>

//! This is a common interface for meshing algorithms 
//! instantiated by Mesh Factory and implemented by plugins.
class BRepMesh_DiscretRoot : public Standard_Transient
{
public:
  
  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_DiscretRoot();

  //! Set the shape to triangulate.
  void SetShape(const TopoDS_Shape& theShape)
  {
    myShape = theShape;
  }
  
  const TopoDS_Shape& Shape() const
  {
    return myShape;
  }
  
  //! Returns true if triangualtion was performed and has success.
  Standard_Boolean IsDone() const
  {
    return myIsDone;
  }

  //! Compute triangulation for set shape.
  virtual void Perform(const Message_ProgressRange& theRange = Message_ProgressRange()) = 0;


  DEFINE_STANDARD_RTTIEXT(BRepMesh_DiscretRoot,Standard_Transient)

protected:
  
  //! Constructor
  Standard_EXPORT BRepMesh_DiscretRoot();
  
  //! Sets IsDone flag.
  void setDone()
  {
    myIsDone = Standard_True;
  }
  
  //! Clears IsDone flag.
  void setNotDone()
  {
    myIsDone = Standard_False;
  }
  
  Standard_EXPORT virtual void init();

  TopoDS_Shape      myShape;
  Standard_Boolean  myIsDone;
};

DEFINE_STANDARD_HANDLE(BRepMesh_DiscretRoot, Standard_Transient)

#endif
