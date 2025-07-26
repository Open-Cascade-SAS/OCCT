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

#ifndef ADAPTOR_PNT2D_AIS_H
#define ADAPTOR_PNT2D_AIS_H

#include <Geom2d_Point.hxx>
#include <AIS_InteractiveObject.hxx>

//! AIS interactive Object for Geom2d_Point
class AdaptorPnt2d_AIS : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(AdaptorPnt2d_AIS, AIS_InteractiveObject)
public:
  AdaptorPnt2d_AIS (const Handle(Geom2d_Point)& thePoint,
                    Standard_Real theElevation = 0)
  : myPoint(thePoint->X(), thePoint->Y(), theElevation) {}

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

  gp_Pnt myPoint;

};

#endif // ADAPTOR_PNT2D_AIS_H
