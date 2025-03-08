// Created by: DAUTRY Philippe
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

#ifndef _DDataStd_TreeBrowser_HeaderFile
#define _DDataStd_TreeBrowser_HeaderFile

#include <Standard.hxx>

#include <TDF_Label.hxx>
#include <Draw_Drawable3D.hxx>
#include <Standard_OStream.hxx>
#include <Draw_Interpretor.hxx>
class Draw_Display;
class TCollection_AsciiString;
class TDataStd_TreeNode;

class DDataStd_TreeBrowser;
DEFINE_STANDARD_HANDLE(DDataStd_TreeBrowser, Draw_Drawable3D)

//! Browses a TreeNode from TDataStd.
//! =================================
class DDataStd_TreeBrowser : public Draw_Drawable3D
{

public:
  Standard_EXPORT DDataStd_TreeBrowser(const TDF_Label& root);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Draw_Drawable3D) Copy() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Dump(Standard_OStream& S) const Standard_OVERRIDE;

  //! Specific methods
  //! ================
  Standard_EXPORT virtual void Whatis(Draw_Interpretor& I) const Standard_OVERRIDE;

  Standard_EXPORT void Label(const TDF_Label& root);

  Standard_EXPORT TDF_Label Label() const;

  //! Returns   a   string composed with  the   TreeNode  of
  //! <myLabel>.
  Standard_EXPORT TCollection_AsciiString OpenRoot() const;

  //! Returns a string composed   with the sub-TreeNodes of
  //! <L>
  Standard_EXPORT TCollection_AsciiString OpenNode(const TDF_Label& L) const;

  DEFINE_STANDARD_RTTIEXT(DDataStd_TreeBrowser, Draw_Drawable3D)

protected:
private:
  //! Returns a string composed with the sub-TreeNodes
  //! of <aTreeNode>. Used to implement other methods.
  Standard_EXPORT void OpenNode(const Handle(TDataStd_TreeNode)& aTreeNode,
                                TCollection_AsciiString&         aList) const;

  TDF_Label myRoot;
};

#endif // _DDataStd_TreeBrowser_HeaderFile
