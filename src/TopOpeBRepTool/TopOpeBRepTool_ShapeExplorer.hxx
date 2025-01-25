// Created on: 1995-07-13
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepTool_ShapeExplorer_HeaderFile
#define _TopOpeBRepTool_ShapeExplorer_HeaderFile

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>

//! Extends TopExp_Explorer by counting index of current item
//! (for tracing and debug)

class TopOpeBRepTool_ShapeExplorer : public TopExp_Explorer
{
public:
  //! Creates an empty explorer, becomes useful after Init.
  TopOpeBRepTool_ShapeExplorer()
      : myIndex(0)
  {
  }

  //! Creates an Explorer on the Shape <S>.
  //!
  //! <ToFind> is the type of shapes to search.
  //! TopAbs_VERTEX, TopAbs_EDGE, ...
  //!
  //! <ToAvoid>   is the type   of shape to  skip in the
  //! exploration.   If   <ToAvoid>  is  equal  or  less
  //! complex than <ToFind> or if  <ToAVoid> is SHAPE it
  //! has no effect on the exploration.
  TopOpeBRepTool_ShapeExplorer(const TopoDS_Shape&    S,
                               const TopAbs_ShapeEnum ToFind,
                               const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE)
      : TopExp_Explorer(S, ToFind, ToAvoid),
        myIndex(More() ? 1 : 0)
  {
  }

  void Init(const TopoDS_Shape&    S,
            const TopAbs_ShapeEnum ToFind,
            const TopAbs_ShapeEnum ToAvoid = TopAbs_SHAPE)
  {
    TopExp_Explorer::Init(S, ToFind, ToAvoid);
    myIndex = (More() ? 1 : 0);
  }

  //! Moves to the next Shape in the exploration.
  void Next()
  {
    if (More())
      myIndex++;
    TopExp_Explorer::Next();
  }

  //! Index of current sub-shape
  Standard_Integer Index() const { return myIndex; }

  //! Dump info on current shape to stream
  Standard_OStream& DumpCurrent(Standard_OStream& OS) const
  {
    if (More())
    {
      TopAbs::Print(Current().ShapeType(), OS);
      OS << "(" << Index() << ",";
      TopAbs::Print(Current().Orientation(), OS);
      OS << ") ";
    }
    return OS;
  }

private:
  Standard_Integer myIndex;
};

#endif // _TopOpeBRepTool_ShapeExplorer_HeaderFile
