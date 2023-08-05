// Created on: 1993-04-30
// Created by: Yves FRICAUD
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

#ifndef _MAT_Node_HeaderFile
#define _MAT_Node_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <MAT_SequenceOfArc.hxx>
#include <MAT_SequenceOfBasicElt.hxx>
class MAT_Arc;


class MAT_Node;
DEFINE_STANDARD_HANDLE(MAT_Node, Standard_Transient)

//! Node of Graph.
class MAT_Node : public Standard_Transient
{

public:

  
  Standard_EXPORT MAT_Node(const Standard_Integer GeomIndex, const Handle(MAT_Arc)& LinkedArc, const Standard_Real Distance);
  
  //! Returns the index associated of the geometric
  //! representation of <me>.
  Standard_EXPORT Standard_Integer GeomIndex() const;
  
  //! Returns the index associated of the node
  Standard_EXPORT Standard_Integer Index() const;
  
  //! Returns in <S> the Arcs linked to <me>.
  Standard_EXPORT void LinkedArcs (MAT_SequenceOfArc& S) const;
  
  //! Returns  in <S> the BasicElts equidistant
  //! to <me>.
  Standard_EXPORT void NearElts (MAT_SequenceOfBasicElt& S) const;
  
  Standard_EXPORT Standard_Real Distance() const;
  
  //! Returns True if <me> is a pending Node.
  //! (ie : the number of Arc Linked = 1)
  Standard_EXPORT Standard_Boolean PendingNode() const;
  
  //! Returns True if <me> belongs to the figure.
  Standard_EXPORT Standard_Boolean OnBasicElt() const;
  
  //! Returns True if the distance of <me> is Infinite
  Standard_EXPORT Standard_Boolean Infinite() const;
  
  //! Set the index associated of the node
  Standard_EXPORT void SetIndex (const Standard_Integer anIndex);
  
  Standard_EXPORT void SetLinkedArc (const Handle(MAT_Arc)& anArc);




  DEFINE_STANDARD_RTTIEXT(MAT_Node,Standard_Transient)

protected:




private:


  Standard_Integer nodeIndex;
  Standard_Integer geomIndex;
  Standard_Address aLinkedArc;
  Standard_Real distance;


};







#endif // _MAT_Node_HeaderFile
