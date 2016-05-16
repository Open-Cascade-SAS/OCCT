// Created on: 1995-09-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepFeat_SplitShape_HeaderFile
#define _BRepFeat_SplitShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <LocOpe_Spliter.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

class LocOpe_WiresOnShape;
class StdFail_NotDone;
class Standard_ConstructionError;
class Standard_NoSuchObject;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Compound;


//! One of the most significant aspects of BRepFeat functionality is the use of local
//! operations as opposed to global ones. In a global operation, you would first construct a
//! form of the type you wanted in your final feature, and then remove matter so that it could
//! fit into your initial basis object. In a local operation, however, you specify the domain of
//! the feature construction with aspects of the shape on which the feature is being created.
//! These semantics are expressed in terms of a member shape of the basis shape from which -
//! or up to which - matter will be added or removed. As a result, local operations make
//! calculations simpler and faster than global operations.
//! In BRepFeat, the semantics of local operations define features constructed from a contour or a
//! part of the basis shape referred to as the tool. In a SplitShape object, wires or edges of a
//! face in the basis shape to be used as a part of the feature are cut out and projected to a plane
//! outside or inside the basis shape. By rebuilding the initial shape incorporating the edges and
//! the faces of the tool, protrusion or depression features can be constructed.
class BRepFeat_SplitShape  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
    BRepFeat_SplitShape();
  
  //! Creates the process  with the shape <S>.
  BRepFeat_SplitShape(const TopoDS_Shape& S);

  //! Add splitting edges or wires for whole initial shape
  //! withot additional specification edge->face, edge->edge
  //! This method puts edge on the corresponding faces from initial shape
  Standard_EXPORT Standard_Boolean Add(const TopTools_SequenceOfShape& theEdges);
  
  //! Initializes the process on the shape <S>.
    void Init (const TopoDS_Shape& S);
  
  //! Set the flag of check internal intersections
  //! default value is True (to check)
    void SetCheckInterior (const Standard_Boolean ToCheckInterior);
  
  //! Adds the wire <W> on the face <F>.
  //! Raises NoSuchObject  if <F> does not belong to the original shape.
    void Add (const TopoDS_Wire& W, const TopoDS_Face& F);
  
  //! Adds the edge <E> on the face <F>.
    void Add (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Adds the compound <Comp> on the face <F>. The
  //! compound <Comp> must consist of edges lying on the
  //! face <F>. If edges are geometrically connected,
  //! they must be connected topologically, i.e. they
  //! must share common vertices.
  //!
  //! Raises NoSuchObject  if <F> does not belong to the original shape.
    void Add (const TopoDS_Compound& Comp, const TopoDS_Face& F);
  
  //! Adds the edge <E> on the existing edge <EOn>.
    void Add (const TopoDS_Edge& E, const TopoDS_Edge& EOn);
  
  //! Returns  the faces   which  are the  left of   the
  //! projected wires.
  Standard_EXPORT const TopTools_ListOfShape& DirectLeft() const;
  
  //! Returns the faces of the "left" part on the shape.
  //! (It  is build   from  DirectLeft,  with  the faces
  //! connected to this set, and so on...).
  //! Raises NotDone if IsDone returns <Standard_False>.
  Standard_EXPORT const TopTools_ListOfShape& Left() const;
  
  //! Builds the cut and the resulting faces and edges as well.
  Standard_EXPORT void Build() Standard_OVERRIDE;
  
  //! Returns true if the shape has been deleted.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns the list of generated Faces.
  Standard_EXPORT const TopTools_ListOfShape& Modified (const TopoDS_Shape& F) Standard_OVERRIDE;




protected:





private:



  LocOpe_Spliter mySShape;
  Handle(LocOpe_WiresOnShape) myWOnShape;


};


#include <BRepFeat_SplitShape.lxx>





#endif // _BRepFeat_SplitShape_HeaderFile
