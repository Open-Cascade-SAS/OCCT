// Created on: 2000-06-15
// Created by: Edward AGAPOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_ShapeTool_HeaderFile
#define _XCAFDoc_ShapeTool_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Boolean.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Map.hxx>
#include <TDF_Label.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
class Standard_GUID;
class TDF_Label;
class TopoDS_Shape;
class TopLoc_Location;
class XCAFDoc_GraphNode;

//! A tool to store shapes in an XDE
//! document in the form of assembly structure, and to maintain this structure.
//! Attribute containing Shapes section of DECAF document.
//! Provide tools for management of Shapes section.
//! The API provided by this class allows to work with this
//! structure regardless of its low-level implementation.
//! All the shapes are stored on child labels of a main label which is
//! XCAFDoc_DocumentTool::LabelShapes(). The label for assembly also has
//! sub-labels, each of which represents the instance of
//! another shape in that assembly (component). Such sub-label
//! stores reference to the label of the original shape in the form
//! of TDataStd_TreeNode with GUID XCAFDoc::ShapeRefGUID(), and its
//! location encapsulated into the NamedShape.
//! For correct work with an XDE document, it is necessary to use
//! methods for analysis and methods for working with shapes.
//! For example:
//! if ( STool->IsAssembly(aLabel) )
//! { bool subchilds = false; (default)
//! int nbc = STool->NbComponents
//! (aLabel[,subchilds]);
//! }
//! If subchilds is True, commands also consider sub-levels. By
//! default, only level one is checked.
//! In this example, number of children from the first level of
//! assembly will be returned. Methods for creation and initialization:
//! Constructor:
//! XCAFDoc_ShapeTool::XCAFDoc_ShapeTool()
//! Getting a guid:
//! Standard_GUID GetID ();
//! Creation (if does not exist) of ShapeTool on label L:
//! occ::handle<XCAFDoc_ShapeTool> XCAFDoc_ShapeTool::Set(const TDF_Label& L)
//! Analyze whether shape is a simple shape or an instance or a
//! component of an assembly or it is an assembly ( methods of analysis).
//! For example:
//! STool->IsShape(aLabel) ;
//! Analyze that the label represents a shape (simple
//! shape, assembly or reference) or
//! STool->IsTopLevel(aLabel);
//! Analyze that the label is a label of a top-level shape.
//! Work with simple shapes, assemblies and instances (
//! methods for work with shapes).
//! For example:
//! Add shape:
//! bool makeAssembly;
//! // True to interpret a Compound as an Assembly, False to take it
//! as a whole
//! aLabel = STool->AddShape(aShape, makeAssembly);
//! Get shape:
//! TDF_Label aLabel...
//! // A label must be present if
//! (aLabel.IsNull()) { ... no such label : abandon .. }
//! TopoDS_Shape aShape;
//! aShape = STool->GetShape(aLabel);
//! if (aShape.IsNull())
//! { ... this label is not for a Shape ... }
//! To get a label from shape.
//! bool findInstance = false;
//! (this is default value)
//! aLabel = STool->FindShape(aShape [,findInstance]);
//! if (aLabel.IsNull())
//! { ... no label found for this shape ... }
class XCAFDoc_ShapeTool : public TDataStd_GenericEmpty
{

public:
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Create (if not exist) ShapeTool from XCAFDoc on <L>.
  Standard_EXPORT static occ::handle<XCAFDoc_ShapeTool> Set(const TDF_Label& L);

  //! Creates an empty tool
  //! Creates a tool to work with a document <Doc>
  //! Attaches to label XCAFDoc::LabelShapes()
  Standard_EXPORT XCAFDoc_ShapeTool();

  //! Returns True if the label is a label of top-level shape,
  //! as opposed to component of assembly or subshape
  Standard_EXPORT bool IsTopLevel(const TDF_Label& L) const;

  //! Returns True if the label is not used by any assembly, i.e.
  //! contains sublabels which are assembly components
  //! This is relevant only if IsShape() is True
  //! (There is no Father TreeNode on this <L>)
  Standard_EXPORT static bool IsFree(const TDF_Label& L);

  //! Returns True if the label represents a shape (simple shape,
  //! assembly or reference)
  Standard_EXPORT static bool IsShape(const TDF_Label& L);

  //! Returns True if the label is a label of simple shape
  Standard_EXPORT static bool IsSimpleShape(const TDF_Label& L);

  //! Return true if <L> is a located instance of other shape
  //! i.e. reference
  Standard_EXPORT static bool IsReference(const TDF_Label& L);

  //! Returns True if the label is a label of assembly, i.e.
  //! contains sublabels which are assembly components
  //! This is relevant only if IsShape() is True
  Standard_EXPORT static bool IsAssembly(const TDF_Label& L);

  //! Return true if <L> is reference serving as component
  //! of assembly
  Standard_EXPORT static bool IsComponent(const TDF_Label& L);

  //! Returns True if the label is a label of compound, i.e.
  //! contains some sublabels
  //! This is relevant only if IsShape() is True
  Standard_EXPORT static bool IsCompound(const TDF_Label& L);

  //! Return true if <L> is subshape of the top-level shape
  Standard_EXPORT static bool IsSubShape(const TDF_Label& L);

  //! Checks whether shape <sub> is subshape of shape stored on
  //! label shapeL
  Standard_EXPORT bool IsSubShape(const TDF_Label&    shapeL,
                                              const TopoDS_Shape& sub) const;

  Standard_EXPORT bool SearchUsingMap(const TopoDS_Shape&    S,
                                                  TDF_Label&             L,
                                                  const bool findWithoutLoc,
                                                  const bool findSubshape) const;

  //! General tool to find a (sub) shape in the document
  //! * If findInstance is True, and S has a non-null location,
  //! first tries to find the shape among the top-level shapes
  //! with this location
  //! * If not found, and findComponent is True, tries to find the shape
  //! among the components of assemblies
  //! * If not found, tries to find the shape without location
  //! among top-level shapes
  //! * If not found and findSubshape is True, tries to find a
  //! shape as a subshape of top-level simple shapes
  //! Returns False if nothing is found
  Standard_EXPORT bool
    Search(const TopoDS_Shape&    S,
           TDF_Label&             L,
           const bool findInstance  = true,
           const bool findComponent = true,
           const bool findSubshape  = true) const;

  //! Returns the label corresponding to shape S
  //! (searches among top-level shapes, not including subcomponents
  //! of assemblies and subshapes)
  //! If findInstance is False (default), search for the
  //! input shape without location
  //! If findInstance is True, searches for the
  //! input shape as is.
  //! Return True if <S> is found.
  Standard_EXPORT bool
    FindShape(const TopoDS_Shape&    S,
              TDF_Label&             L,
              const bool findInstance = false) const;

  //! Does the same as previous method
  //! Returns Null label if not found
  Standard_EXPORT TDF_Label FindShape(const TopoDS_Shape&    S,
                                      const bool findInstance = false) const;

  //! To get TopoDS_Shape from shape's label
  //! For component, returns new shape with correct location
  //! Returns False if label does not contain shape
  Standard_EXPORT static bool GetShape(const TDF_Label& L, TopoDS_Shape& S);

  //! To get TopoDS_Shape from shape's label
  //! For component, returns new shape with correct location
  //! Returns Null shape if label does not contain shape
  Standard_EXPORT static TopoDS_Shape GetShape(const TDF_Label& L);

  //! Gets shape from a sequence of shape's labels
  //! @param[in] theLabels a sequence of labels to get shapes from
  //! @return original shape in case of one label and a compound of shapes in case of more
  Standard_EXPORT static TopoDS_Shape GetOneShape(const NCollection_Sequence<TDF_Label>& theLabels);

  //! Gets shape from a sequence of all top-level shapes which are free
  //! @return original shape in case of one label and a compound of shapes in case of more
  Standard_EXPORT TopoDS_Shape GetOneShape() const;

  //! Creates new (empty) top-level shape.
  //! Initially it holds empty TopoDS_Compound
  Standard_EXPORT TDF_Label NewShape() const;

  //! Sets representation (TopoDS_Shape) for top-level shape.
  Standard_EXPORT void SetShape(const TDF_Label& L, const TopoDS_Shape& S);

  //! Adds a new top-level (creates and returns a new label)
  //! If makeAssembly is True, treats TopAbs_COMPOUND shapes
  //! as assemblies (creates assembly structure).
  //! NOTE: <makePrepare> replace components without location
  //! in assembly by located components to avoid some problems.
  //! If AutoNaming() is True then automatically attaches names.
  Standard_EXPORT TDF_Label AddShape(const TopoDS_Shape&    S,
                                     const bool makeAssembly = true,
                                     const bool makePrepare  = true);

  //! Removes shape (whole label and all its sublabels)
  //! If removeCompletely is true, removes complete shape
  //! If removeCompletely is false, removes instance(location) only
  //! Returns False (and does nothing) if shape is not free
  //! or is not top-level shape
  Standard_EXPORT bool
    RemoveShape(const TDF_Label& L, const bool removeCompletely = true) const;

  //! set hasComponents into false
  Standard_EXPORT void Init();

  //! Sets auto-naming mode to <V>. If True then for added
  //! shapes, links, assemblies and SHUO's, the TDataStd_Name attribute
  //! is automatically added. For shapes it contains a shape type
  //! (e.g. "SOLID", "SHELL", etc); for links it has a form
  //! "=>[0:1:1:2]" (where a tag is a label containing a shape
  //! without a location); for assemblies it is "ASSEMBLY", and
  //! "SHUO" for SHUO's.
  //! This setting is global; it cannot be made a member function
  //! as it is used by static methods as well.
  //! By default, auto-naming is enabled.
  //! See also AutoNaming().
  Standard_EXPORT static void SetAutoNaming(const bool V);

  //! Returns current auto-naming mode. See SetAutoNaming() for
  //! description.
  Standard_EXPORT static bool AutoNaming();

  //! recursive
  Standard_EXPORT void ComputeShapes(const TDF_Label& L);

  //! Compute a sequence of simple shapes
  Standard_EXPORT void ComputeSimpleShapes();

  //! Returns a sequence of all top-level shapes
  Standard_EXPORT void GetShapes(NCollection_Sequence<TDF_Label>& Labels) const;

  //! Returns a sequence of all top-level shapes
  //! which are free (i.e. not referred by any other)
  Standard_EXPORT void GetFreeShapes(NCollection_Sequence<TDF_Label>& FreeLabels) const;

  //! Returns list of labels which refer shape L as component
  //! Returns number of users (0 if shape is free)
  Standard_EXPORT static int GetUsers(
    const TDF_Label&       L,
    NCollection_Sequence<TDF_Label>&     Labels,
    const bool getsubchilds = false);

  //! Returns location of instance
  Standard_EXPORT static TopLoc_Location GetLocation(const TDF_Label& L);

  //! Returns label which corresponds to a shape referred by L
  //! Returns False if label is not reference
  Standard_EXPORT static bool GetReferredShape(const TDF_Label& L, TDF_Label& Label);

  //! Returns number of Assembles components
  Standard_EXPORT static int NbComponents(
    const TDF_Label&       L,
    const bool getsubchilds = false);

  //! Returns list of components of assembly
  //! Returns False if label is not assembly
  Standard_EXPORT static bool GetComponents(
    const TDF_Label&       L,
    NCollection_Sequence<TDF_Label>&     Labels,
    const bool getsubchilds = false);

  //! Adds a component given by its label and location to the assembly
  //! Note: assembly must be IsAssembly() or IsSimpleShape()
  Standard_EXPORT TDF_Label AddComponent(const TDF_Label&       assembly,
                                         const TDF_Label&       comp,
                                         const TopLoc_Location& Loc);

  //! Adds a shape (located) as a component to the assembly
  //! If necessary, creates an additional top-level shape for
  //! component and return the Label of component.
  //! If expand is True and component is Compound, it will
  //! be created as assembly also
  //! Note: assembly must be IsAssembly() or IsSimpleShape()
  Standard_EXPORT TDF_Label AddComponent(const TDF_Label&       assembly,
                                         const TopoDS_Shape&    comp,
                                         const bool expand = false);

  //! Removes a component from its assembly
  Standard_EXPORT void RemoveComponent(const TDF_Label& comp) const;

  //! Top-down update for all assembly compounds stored in the document.
  Standard_EXPORT void UpdateAssemblies();

  //! Finds a label for subshape <sub> of shape stored on
  //! label shapeL
  //! Returns Null label if it is not found
  Standard_EXPORT bool FindSubShape(const TDF_Label&    shapeL,
                                                const TopoDS_Shape& sub,
                                                TDF_Label&          L) const;

  //! Adds a label for subshape <sub> of shape stored on
  //! label shapeL
  //! Returns Null label if it is not subshape
  Standard_EXPORT TDF_Label AddSubShape(const TDF_Label& shapeL, const TopoDS_Shape& sub) const;

  //! Adds (of finds already existed) a label for subshape <sub> of shape stored on
  //! label shapeL. Label addedSubShapeL returns added (found) label or empty in case of wrong
  //! subshape. Returns True, if new shape was added, False in case of already existed
  //! subshape/wrong subshape
  Standard_EXPORT bool AddSubShape(const TDF_Label&    shapeL,
                                               const TopoDS_Shape& sub,
                                               TDF_Label&          addedSubShapeL) const;

  Standard_EXPORT TDF_Label FindMainShapeUsingMap(const TopoDS_Shape& sub) const;

  //! Performs a search among top-level shapes to find
  //! the shape containing <sub> as subshape
  //! Checks only simple shapes, and returns the first found
  //! label (which should be the only one for valid model)
  Standard_EXPORT TDF_Label FindMainShape(const TopoDS_Shape& sub) const;

  //! Returns list of labels identifying subshapes of the given shape
  //! Returns False if no subshapes are placed on that label
  Standard_EXPORT static bool GetSubShapes(const TDF_Label&   L,
                                                       NCollection_Sequence<TDF_Label>& Labels);

  //! returns the label under which shapes are stored
  Standard_EXPORT TDF_Label BaseLabel() const;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream&      theDumpLog,
                                         const bool deep) const;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& theDumpLog) const
    override;

  //! Print to std::ostream <theDumpLog> type of shape found on <L> label
  //! and the entry of <L>, with <level> tabs before.
  //! If <deep>, print also TShape and Location addresses
  Standard_EXPORT static void DumpShape(Standard_OStream&      theDumpLog,
                                        const TDF_Label&       L,
                                        const int level = 0,
                                        const bool deep  = false);

  Standard_EXPORT const Standard_GUID& ID() const override;

  //! Returns True if the label is a label of external references, i.e.
  //! there are some reference on the no-step files, which are
  //! described in document only their names
  Standard_EXPORT static bool IsExternRef(const TDF_Label& L);

  //! Sets the names of references on the no-step files
  Standard_EXPORT TDF_Label SetExternRefs(const NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& SHAS) const;

  //! Sets the names of references on the no-step files
  Standard_EXPORT void SetExternRefs(const TDF_Label&                      L,
                                     const NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& SHAS) const;

  //! Gets the names of references on the no-step files
  Standard_EXPORT static void GetExternRefs(const TDF_Label&                L,
                                            NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& SHAS);

  //! Sets the SHUO structure between upper_usage and next_usage
  //! create multy-level (if number of labels > 2) SHUO from first to last
  //! Initialise out <MainSHUOAttr> by main upper_usage SHUO attribute.
  //! Returns FALSE if some of labels in not component label
  Standard_EXPORT bool SetSHUO(const NCollection_Sequence<TDF_Label>&   Labels,
                                           occ::handle<XCAFDoc_GraphNode>& MainSHUOAttr) const;

  //! Returns founded SHUO GraphNode attribute <aSHUOAttr>
  //! Returns false in other case
  Standard_EXPORT static bool GetSHUO(const TDF_Label&           SHUOLabel,
                                                  occ::handle<XCAFDoc_GraphNode>& aSHUOAttr);

  //! Returns founded SHUO GraphNodes of indicated component
  //! Returns false in other case
  Standard_EXPORT static bool GetAllComponentSHUO(const TDF_Label&       CompLabel,
                                                              NCollection_Sequence<occ::handle<TDF_Attribute>>& SHUOAttrs);

  //! Returns the sequence of labels of SHUO attributes,
  //! which is upper_usage for this next_usage SHUO attribute
  //! (that indicated by label)
  //! NOTE: returns upper_usages only on one level (not recurse)
  //! NOTE: do not clear the sequence before filling
  Standard_EXPORT static bool GetSHUOUpperUsage(const TDF_Label&   NextUsageL,
                                                            NCollection_Sequence<TDF_Label>& Labels);

  //! Returns the sequence of labels of SHUO attributes,
  //! which is next_usage for this upper_usage SHUO attribute
  //! (that indicated by label)
  //! NOTE: returns next_usages only on one level (not recurse)
  //! NOTE: do not clear the sequence before filling
  Standard_EXPORT static bool GetSHUONextUsage(const TDF_Label&   UpperUsageL,
                                                           NCollection_Sequence<TDF_Label>& Labels);

  //! Remove SHUO from component sublabel,
  //! remove all dependencies on other SHUO.
  //! Returns FALSE if cannot remove SHUO dependencies.
  //! NOTE: remove any styles that associated with this SHUO.
  Standard_EXPORT bool RemoveSHUO(const TDF_Label& SHUOLabel) const;

  //! Search the path of labels in the document,
  //! that corresponds the component from any assembly
  //! Try to search the sequence of labels with location that
  //! produce this shape as component of any assembly
  //! NOTE: Clear sequence of labels before filling
  Standard_EXPORT bool FindComponent(const TopoDS_Shape& theShape,
                                                 NCollection_Sequence<TDF_Label>&  Labels) const;

  //! Search for the component shape that styled by shuo
  //! Returns null shape if no any shape is found.
  Standard_EXPORT TopoDS_Shape GetSHUOInstance(const occ::handle<XCAFDoc_GraphNode>& theSHUO) const;

  //! Search for the component shape by labelks path
  //! and set SHUO structure for founded label structure
  //! Returns null attribute if no component in any assembly found.
  Standard_EXPORT occ::handle<XCAFDoc_GraphNode> SetInstanceSHUO(const TopoDS_Shape& theShape) const;

  //! Searching for component shapes that styled by shuo
  //! Returns empty sequence of shape if no any shape is found.
  Standard_EXPORT bool
    GetAllSHUOInstances(const occ::handle<XCAFDoc_GraphNode>& theSHUO,
                        NCollection_Sequence<TopoDS_Shape>&        theSHUOShapeSeq) const;

  //! Searches the SHUO by labels of components
  //! from upper_usage component to next_usage
  //! Returns null attribute if no SHUO found
  Standard_EXPORT static bool FindSHUO(const NCollection_Sequence<TDF_Label>&   Labels,
                                                   occ::handle<XCAFDoc_GraphNode>& theSHUOAttr);

  //! Sets location to the shape label
  //! If label is reference -> changes location attribute
  //! If label is free shape -> creates reference with location to it
  //! @param[in] theShapeLabel the shape label to change location
  //! @param[in] theLoc location to set
  //! @param[out] theRefLabel the reference label with new location
  //! @return TRUE if new location was set
  Standard_EXPORT bool SetLocation(const TDF_Label&       theShapeLabel,
                                               const TopLoc_Location& theLoc,
                                               TDF_Label&             theRefLabel);

  //! Convert Shape (compound/compsolid/shell/wire) to assembly
  Standard_EXPORT bool Expand(const TDF_Label& Shape);

  //! Method to get NamedData attribute assigned to the given shape label.
  //! @param[in] theLabel     the shape Label
  //! @param[in] theToCreate  create and assign attribute if it doesn't exist
  //! @return Handle to the NamedData attribute or Null if there is none
  Standard_EXPORT occ::handle<TDataStd_NamedData> GetNamedProperties(
    const TDF_Label&       theLabel,
    const bool theToCreate = false) const;

  //! Method to get NamedData attribute assigned to a label of the given shape.
  //! @param[in] theShape     input shape
  //! @param[in] theToCreate  create and assign attribute if it doesn't exist
  //! @return Handle to the NamedData attribute or Null if there is none
  Standard_EXPORT occ::handle<TDataStd_NamedData> GetNamedProperties(
    const TopoDS_Shape&    theShape,
    const bool theToCreate = false) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

  DEFINE_DERIVED_ATTRIBUTE(XCAFDoc_ShapeTool, TDataStd_GenericEmpty)

private:
  //! Checks recursively if the given assembly item is modified. If so, its
  //! associated compound is updated. Returns true if the assembly item is
  //! modified, false -- otherwise.
  Standard_EXPORT bool updateComponent(const TDF_Label& theAssmLabel,
                                                   TopoDS_Shape&    theUpdatedShape,
                                                   NCollection_Map<TDF_Label>&    theUpdated) const;

  //! Adds a new top-level (creates and returns a new label)
  //! For internal use. Used by public method AddShape.
  Standard_EXPORT TDF_Label addShape(const TopoDS_Shape&    S,
                                     const bool makeAssembly = true);

  //! Makes a shape on label L to be a reference to shape refL
  //! with location loc
  Standard_EXPORT static void MakeReference(const TDF_Label&       L,
                                            const TDF_Label&       refL,
                                            const TopLoc_Location& loc);

  //! Auxiliary method for Expand
  //! Add declared under expanded theMainShapeL subshapes to new part label thePart
  //! Recursively iterate all subshapes of shape from thePart, current shape to iterate its
  //! subshapes is theShape.
  Standard_EXPORT void makeSubShape(const TDF_Label&       theMainShapeL,
                                    const TDF_Label&       thePart,
                                    const TopoDS_Shape&    theShape,
                                    const TopLoc_Location& theLoc);

  NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher> myShapeLabels;
  NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher> mySubShapes;
  NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher> mySimpleShapes;
  bool            hasSimpleShapes;
};

#endif // _XCAFDoc_ShapeTool_HeaderFile
