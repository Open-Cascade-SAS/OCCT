// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_DocumentExplorer_HeaderFile
#define _XCAFPrs_DocumentExplorer_HeaderFile

#include <NCollection_Vector.hxx>
#include <XCAFPrs_DocumentNode.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Shape.hxx>

class TDocStd_Document;
class XCAFDoc_ColorTool;
class XCAFDoc_VisMaterialTool;

typedef Standard_Integer XCAFPrs_DocumentExplorerFlags;

//! Document explorer flags.
enum
{
  XCAFPrs_DocumentExplorerFlags_None = 0x00, //!< no flags
  XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes =
    0x01,                                       //!< explore only leaf nodes (skip assembly nodes)
  XCAFPrs_DocumentExplorerFlags_NoStyle = 0x02, //!< do not fetch styles
};

//! Document iterator through shape nodes.
class XCAFPrs_DocumentExplorer
{
public: //! @name string identification tools
  //! Construct a unique string identifier for the given label.
  //! The identifier is a concatenation of label entries (TDF_Tool::Entry() with tailing '.') of
  //! hierarchy from parent to child joined via '/' and looking like this:
  //! @code
  //!   0:1:1:1./0:1:1:1:9./0:1:1:5:7.
  //! @endcode
  //! This generation scheme also allows finding originating labels using TDF_Tool::Label().
  //! The tailing dot simplifies parent equality check.
  //! @param theLabel child label to define id
  //! @param theParentId parent string identifier defined by this method
  Standard_EXPORT static TCollection_AsciiString DefineChildId(
    const TDF_Label&               theLabel,
    const TCollection_AsciiString& theParentId);

  //! Find a shape entity based on a text identifier constructed from OCAF labels defining full
  //! path.
  //! @sa DefineChildId()
  Standard_EXPORT static TDF_Label FindLabelFromPathId(const Handle(TDocStd_Document)& theDocument,
                                                       const TCollection_AsciiString&  theId,
                                                       TopLoc_Location& theParentLocation,
                                                       TopLoc_Location& theLocation);

  //! Find a shape entity based on a text identifier constructed from OCAF labels defining full
  //! path.
  //! @sa DefineChildId()
  static TDF_Label FindLabelFromPathId(const Handle(TDocStd_Document)& theDocument,
                                       const TCollection_AsciiString&  theId,
                                       TopLoc_Location&                theLocation)
  {
    TopLoc_Location aDummy;
    return FindLabelFromPathId(theDocument, theId, aDummy, theLocation);
  }

  //! Find a shape entity based on a text identifier constructed from OCAF labels defining full
  //! path.
  //! @sa DefineChildId()
  Standard_EXPORT static TopoDS_Shape FindShapeFromPathId(
    const Handle(TDocStd_Document)& theDocument,
    const TCollection_AsciiString&  theId);

public:
  //! Empty constructor.
  Standard_EXPORT XCAFPrs_DocumentExplorer();

  //! Constructor for exploring the whole document.
  //! @param theDocument document to explore
  //! @param theFlags    iteration flags
  //! @param theDefStyle default style for nodes with undefined style
  Standard_EXPORT XCAFPrs_DocumentExplorer(const Handle(TDocStd_Document)&     theDocument,
                                           const XCAFPrs_DocumentExplorerFlags theFlags,
                                           const XCAFPrs_Style& theDefStyle = XCAFPrs_Style());

  //! Constructor for exploring specified list of root shapes in the document.
  //! @param theDocument  document to explore
  //! @param theRoots     root labels to explore within specified document
  //! @param theFlags     iteration flags
  //! @param theDefStyle  default style for nodes with undefined style
  Standard_EXPORT XCAFPrs_DocumentExplorer(const Handle(TDocStd_Document)&     theDocument,
                                           const TDF_LabelSequence&            theRoots,
                                           const XCAFPrs_DocumentExplorerFlags theFlags,
                                           const XCAFPrs_Style& theDefStyle = XCAFPrs_Style());

  //! Initialize the iterator from a single root shape in the document.
  //! @param theDocument  document to explore
  //! @param theRoot      single root label to explore within specified document
  //! @param theFlags     iteration flags
  //! @param theDefStyle  default style for nodes with undefined style
  Standard_EXPORT void Init(const Handle(TDocStd_Document)&     theDocument,
                            const TDF_Label&                    theRoot,
                            const XCAFPrs_DocumentExplorerFlags theFlags,
                            const XCAFPrs_Style&                theDefStyle = XCAFPrs_Style());

  //! Initialize the iterator from the list of root shapes in the document.
  //! @param theDocument  document to explore
  //! @param theRoots     root labels to explore within specified document
  //! @param theFlags     iteration flags
  //! @param theDefStyle  default style for nodes with undefined style
  Standard_EXPORT void Init(const Handle(TDocStd_Document)&     theDocument,
                            const TDF_LabelSequence&            theRoots,
                            const XCAFPrs_DocumentExplorerFlags theFlags,
                            const XCAFPrs_Style&                theDefStyle = XCAFPrs_Style());

  //! Return TRUE if iterator points to the valid node.
  Standard_Boolean More() const { return myHasMore; }

  //! Return current position.
  const XCAFPrs_DocumentNode& Current() const { return myCurrent; }

  //! Return current position.
  XCAFPrs_DocumentNode& ChangeCurrent() { return myCurrent; }

  //! Return current position within specified assembly depth.
  const XCAFPrs_DocumentNode& Current(Standard_Integer theDepth) const
  {
    const Standard_Integer aCurrDepth = CurrentDepth();
    if (theDepth == aCurrDepth)
    {
      return myCurrent;
    }

    Standard_OutOfRange_Raise_if(theDepth < 0 || theDepth > myTop,
                                 "XCAFPrs_DocumentExplorer::Current() out of range");
    return myNodeStack.Value(theDepth);
  }

  //! Return depth of the current node in hierarchy, starting from 0.
  //! Zero means Root label.
  Standard_Integer CurrentDepth() const { return myCurrent.IsAssembly ? myTop : myTop + 1; }

  //! Go to the next node.
  Standard_EXPORT void Next();

  //! Return color tool.
  const Handle(XCAFDoc_ColorTool)& ColorTool() const { return myColorTool; }

  //! Return material tool.
  const Handle(XCAFDoc_VisMaterialTool)& VisMaterialTool() const { return myVisMatTool; }

protected:
  //! Initialize root label.
  Standard_EXPORT void initRoot();

  //! Initialize properties for a current label.
  Standard_EXPORT void initCurrent(Standard_Boolean theIsAssembly);

protected:
  Handle(XCAFDoc_ColorTool)                myColorTool;  //!< color tool
  Handle(XCAFDoc_VisMaterialTool)          myVisMatTool; //!< visual material tool
  TDF_LabelSequence                        myRoots;      //!< sequence of root labels
  TDF_LabelSequence::Iterator              myRootIter;   //!< current root label
  NCollection_Vector<XCAFPrs_DocumentNode> myNodeStack;  //!< node stack
  Standard_Integer                         myTop;        //!< top position in the node stack
                                                         // clang-format off
  Standard_Boolean                myHasMore;    //!< global flag indicating that iterator points to the label
                                                         // clang-format on
  XCAFPrs_Style                 myDefStyle;              //!< default style
  XCAFPrs_DocumentNode          myCurrent;               //!< current label info
  XCAFPrs_DocumentExplorerFlags myFlags;                 //!< iteration flags
};

#endif // _XCAFPrs_DocumentExplorer_HeaderFile
