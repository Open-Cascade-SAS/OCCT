// Created on: 1994-03-10
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TopClass_SolidExplorer_HeaderFile
#define _TopClass_SolidExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Lin;
class TopoDS_Face;


//! Provide an   exploration of a  BRep Shape   for the
//! classification. Defines the description of a solid for the
//! SolidClassifier.
class TopClass_SolidExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Should  return  True  if the  point  is  outside a
  //! bounding volume of the shape.
  Standard_EXPORT virtual Standard_Boolean Reject (const gp_Pnt& P) const = 0;
  
  //! Returns  in <L>, <Par>  a segment having at least
  //! one  intersection  with  the  shape  boundary  to
  //! compute  intersections.
  Standard_EXPORT virtual void Segment (const gp_Pnt& P, gp_Lin& L, Standard_Real& Par) = 0;
  
  //! Returns  in <L>, <Par>  a segment having at least
  //! one  intersection  with  the  shape  boundary  to
  //! compute  intersections.
  //!
  //! The First Call to this method returns a line which
  //! point to a point of the first face of the shape.
  //! The Second Call provide a line to the second face
  //! and so on.
  //!
  //! if the method is called N times on a shape with F
  //! faces (N>F) the line point to other points on the
  //! face 1,2,3 ... N
  Standard_EXPORT virtual void OtherSegment (const gp_Pnt& P, gp_Lin& L, Standard_Real& Par) = 0;
  
  //! Starts an exploration of the shells.
  Standard_EXPORT virtual void InitShell() = 0;
  
  //! Returns True if there is  a current shell.
  Standard_EXPORT virtual Standard_Boolean MoreShells() const = 0;
  
  //! Sets the explorer  to the  next  shell and  returns
  //! False if there are no more wires.
  Standard_EXPORT virtual void NextShell() = 0;
  
  //! Returns True  if the shell  bounding volume does not
  //! intersect the segment.
  Standard_EXPORT virtual Standard_Boolean RejectShell (const gp_Lin& L, const Standard_Real Par) const = 0;
  
  //! Starts an exploration of the faces.
  Standard_EXPORT virtual void InitFace() = 0;
  
  //! Returns True if there is  a current face.
  Standard_EXPORT virtual Standard_Boolean MoreFaces() const = 0;
  
  //! Sets the explorer  to the  next  face and  returns
  //! False if there are no more wires.
  Standard_EXPORT virtual void NextFace() = 0;
  
  //! Returns the current face.
  Standard_EXPORT virtual TopoDS_Face CurrentFace() const = 0;
  
  //! Returns True  if the face  bounding volume does not
  //! intersect the segment.
  Standard_EXPORT virtual Standard_Boolean RejectFace (const gp_Lin& L, const Standard_Real Par) const = 0;




protected:





private:





};







#endif // _TopClass_SolidExplorer_HeaderFile
