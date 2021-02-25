// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef ADAPTOR_VEC_AIS_H
#define ADAPTOR_VEC_AIS_H

#include <AIS_InteractiveObject.hxx>

//! AIS interactive Object for vector with arrow and text
class AdaptorVec_AIS : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(AdaptorVec_AIS, AIS_InteractiveObject)
public:

  AdaptorVec_AIS()
  : myLength (1.0),
    myArrowLength (1.0)
  {}

  AdaptorVec_AIS (const gp_Pnt& thePnt,
                  const gp_Dir& theDir,
                  Standard_Real theLength = 1,
                  Standard_Real theArrowLength = 1)
  : myPnt (thePnt),
    myDir (theDir),
    myLength (theLength),
    myArrowLength (theArrowLength)
  {
    //
  }

  AdaptorVec_AIS (const gp_Pnt& thePnt,
                  const gp_Vec& theVec,
                  Standard_Real theArrowLength = 1)
  : myPnt (thePnt),
    myDir (theVec),
    myLength (theVec.Magnitude()),
    myArrowLength (theArrowLength)
  {
    //
  }

  AdaptorVec_AIS (const gp_Pnt2d& thePnt2d,
                  const gp_Dir2d& theDir2d,
                  Standard_Real theLength = 1,
                  Standard_Real theArrowLength = 1)
  : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
    myDir (gp_Dir(theDir2d.X(), theDir2d.Y(), 0.0)),
    myLength (theLength),
    myArrowLength (theArrowLength)
  {
    //
  }

  AdaptorVec_AIS (const gp_Pnt2d& thePnt2d,
                  const gp_Vec2d& theVec2d,
                  Standard_Real theArrowLength = 1)
  : myPnt (gp_Pnt(thePnt2d.X(), thePnt2d.Y(), 0.0)),
    myDir (gp_Dir(theVec2d.X(), theVec2d.Y(), 0.0)),
    myLength (theVec2d.Magnitude()),
    myArrowLength (theArrowLength)
  {
    //
  }

  AdaptorVec_AIS (const gp_Pnt2d& thePnt1,
                  const gp_Pnt2d& thePnt2,
                  Standard_Real theArrowLength = 1);

  void SetText (const TCollection_AsciiString& theText)
  {
    myText = theText;
  }

  void SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
  {
    myDrawer->SetLineAspect(theAspect);
  }

private:

  //! Return TRUE for supported display modes (only mode 0 is supported).
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection (not implemented).
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)&,
                                 const Standard_Integer) Standard_OVERRIDE {}

private:

  gp_Pnt myPnt;
  gp_Dir myDir;
  Standard_Real myLength;
  Standard_Real myArrowLength;
  TCollection_AsciiString myText;
};

#endif // ADAPTOR_VEC2D_AIS_H
