// Created on: 1997-03-17
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_HeaderFile
#define _TNaming_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <TDF_IDList.hxx>
#include <Standard_OStream.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_NameType.hxx>
class TDF_Label;
class TopLoc_Location;
class gp_Trsf;
class TNaming_NamedShape;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Wire;
class TopoDS_Solid;
class TopoDS_Shell;
class TNaming_CopyShape;
class TNaming_TranslateTool;
class TNaming_Translator;
class TNaming_NamedShape;
class TNaming_UsedShapes;
class TNaming_Builder;
class TNaming_Tool;
class TNaming_Iterator;
class TNaming_NewShapeIterator;
class TNaming_OldShapeIterator;
class TNaming_SameShapeIterator;
class TNaming_Name;
class TNaming_Naming;
class TNaming_Selector;
class TNaming_DeltaOnRemoval;
class TNaming_DeltaOnModification;
class TNaming_RefShape;
class TNaming_Scope;
class TNaming_Identifier;
class TNaming_Localizer;
class TNaming_ShapesSet;
class TNaming_IteratorOnShapesSet;
class TNaming_NamingTool;


//! A topological attribute can be seen as a hook
//! into the topological structure. To this hook,
//! data can be attached and references defined.
//! It is used for keeping and access to
//! topological objects and their evolution. All
//! topological objects are stored in the one
//! user-protected TNaming_UsedShapes
//! attribute at the root label of the data
//! framework. This attribute contains map with all
//! topological shapes, used in this document.
//! To all other labels TNaming_NamedShape
//! attribute can be added. This attribute contains
//! references (hooks) to shapes from the
//! TNaming_UsedShapes attribute and evolution
//! of these shapes. TNaming_NamedShape
//! attribute contains a set of pairs of hooks: old
//! shape and new shape (see the figure below).
//! It allows not only get the topological shapes by
//! the labels, but also trace evolution of the
//! shapes and correctly resolve dependent
//! shapes by the changed one.
//! If shape is just-created, then the old shape for
//! accorded named shape is an empty shape. If
//! a shape is deleted, then the new shape in this named shape is empty.
//! Different algorithms may dispose sub-shapes
//! of the result shape at the individual label depending on necessity:
//! -  If a sub-shape must have some extra attributes (material of
//! each face or color of each edge). In this case a specific sub-shape is
//! placed to the separate label (usually, sub-label of the result shape label)
//! with all attributes of this sub-shape.
//! -  If topological naming is needed, a necessary and sufficient
//! (for selected sub-shapes identification) set of sub-shapes is
//! placed to the child labels of the result
//! shape label. As usual, as far as basic solids and closed shells are
//! concerned, all faces of the shape are disposed. Edges and vertices
//! sub-shapes can be identified as intersection of contiguous faces.
//! Modified/generated shapes may be placed to one named shape and
//! identified as this named shape and source named shape that also can be
//! identified with used algorithms.
//! TNaming_NamedShape may contain a few
//! pairs of hooks with the same evolution. In this
//! case topology shape, which belongs to the
//! named shape, is a compound of new shapes.
//! The data model contains both the topology
//! and the hooks, and functions handle both
//! topological entities and hooks. Consider the
//! case of a box function, which creates a solid
//! with six faces and six hooks. Each hook is
//! attached to a face. If you want, you can also
//! have this function create hooks for edges and
//! vertices as well as for faces. For the sake of
//! simplicity though, let's limit the example.
//! Not all functions can define explicit hooks for
//! all topological entities they create, but all
//! topological entities can be turned into hooks
//! when necessary. This is where topological naming is necessary.
class TNaming 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Subtituter les  shapes  sur les structures de   source
  //! vers cible
  Standard_EXPORT static void Substitute (const TDF_Label& labelsource, const TDF_Label& labelcible, TopTools_DataMapOfShapeShape& mapOldNew);
  
  //! Mise a jour des shapes du label  et de ses fils en
  //! tenant compte des  substitutions decrite par
  //! mapOldNew.
  //!
  //! Warning: le  remplacement du shape est  fait    dans tous
  //! les    attributs  qui  le contiennent meme si ceux
  //! ci ne sont pas associees a des sous-labels de <Label>.
  Standard_EXPORT static void Update (const TDF_Label& label, TopTools_DataMapOfShapeShape& mapOldNew);
  
  //! Application de la Location sur les shapes du label
  //! et  de   ses   sous   labels.
  Standard_EXPORT static void Displace (const TDF_Label& label, const TopLoc_Location& aLocation, const Standard_Boolean WithOld = Standard_True);
  
  //! Remplace  les  shapes du label et  des sous-labels
  //! par des copies.
  Standard_EXPORT static void ChangeShapes (const TDF_Label& label, TopTools_DataMapOfShapeShape& M);
  
  //! Application de la transformation sur les shapes du
  //! label et de ses sous labels.
  //! Warning: le  remplacement du shape est  fait    dans tous
  //! les    attributs  qui  le contiennent meme si ceux
  //! ci ne sont pas associees a des sous-labels de <Label>.
  Standard_EXPORT static void Transform (const TDF_Label& label, const gp_Trsf& aTransformation);
  
  //! Replicates the named shape with the transformation <T>
  //! on the label <L> (and sub-labels if necessary)
  //! (TNaming_GENERATED is set)
  Standard_EXPORT static void Replicate (const Handle(TNaming_NamedShape)& NS, const gp_Trsf& T, const TDF_Label& L);
  
  //! Replicates the shape with the transformation <T>
  //! on the label <L> (and sub-labels if necessary)
  //! (TNaming_GENERATED is set)
  Standard_EXPORT static void Replicate (const TopoDS_Shape& SH, const gp_Trsf& T, const TDF_Label& L);
  
  //! Builds shape from map content
  Standard_EXPORT static TopoDS_Shape MakeShape (const TopTools_MapOfShape& MS);
  
  //! Find unique context of shape <S>
  Standard_EXPORT static TopoDS_Shape FindUniqueContext (const TopoDS_Shape& S, const TopoDS_Shape& Context);
  
  //! Find unique context of shape <S>,which is pure concatenation
  //! of atomic shapes (Compound). The result is concatenation of
  //! single contexts
  Standard_EXPORT static TopoDS_Shape FindUniqueContextSet (const TopoDS_Shape& S, const TopoDS_Shape& Context, Handle(TopTools_HArray1OfShape)& Arr);
  
  //! Subtitutes shape in source structure
  Standard_EXPORT static Standard_Boolean SubstituteSShape (const TDF_Label& accesslabel, const TopoDS_Shape& From, TopoDS_Shape& To);
  
  //! Returns True if outer wire is found and the found wire in <theWire>.
  Standard_EXPORT static Standard_Boolean OuterWire (const TopoDS_Face& theFace, TopoDS_Wire& theWire);
  
  //! Returns True if outer Shell is found and the found shell in <theShell>.
  //! Print of TNaming enumeration
  //! =============================
  Standard_EXPORT static Standard_Boolean OuterShell (const TopoDS_Solid& theSolid, TopoDS_Shell& theShell);
  
  //! Appends to <anIDList> the list of the attributes
  //! IDs of this package. CAUTION: <anIDList> is NOT
  //! cleared before use.
  Standard_EXPORT static void IDList (TDF_IDList& anIDList);
  
  //! Prints the  evolution  <EVOL> as  a String on  the
  //! Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TNaming_Evolution EVOL, Standard_OStream& S);
  
  //! Prints the name of name type <NAME> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TNaming_NameType NAME, Standard_OStream& S);
  
  //! Prints the content of UsedShapes private  attribute as a String Table on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TDF_Label& ACCESS, Standard_OStream& S);




protected:





private:




friend class TNaming_CopyShape;
friend class TNaming_TranslateTool;
friend class TNaming_Translator;
friend class TNaming_NamedShape;
friend class TNaming_UsedShapes;
friend class TNaming_Builder;
friend class TNaming_Tool;
friend class TNaming_Iterator;
friend class TNaming_NewShapeIterator;
friend class TNaming_OldShapeIterator;
friend class TNaming_SameShapeIterator;
friend class TNaming_Name;
friend class TNaming_Naming;
friend class TNaming_Selector;
friend class TNaming_DeltaOnRemoval;
friend class TNaming_DeltaOnModification;
friend class TNaming_RefShape;
friend class TNaming_Scope;
friend class TNaming_Identifier;
friend class TNaming_Localizer;
friend class TNaming_ShapesSet;
friend class TNaming_IteratorOnShapesSet;
friend class TNaming_NamingTool;

};







#endif // _TNaming_HeaderFile
