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

#include "AdaptorCurve_AIS.h"

#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_LineAspect.hxx>
#include <StdPrs_PoleCurve.hxx>
#include <StdPrs_Curve.hxx>

void AdaptorCurve_AIS::Compute (const Handle(PrsMgr_PresentationManager)&,
                                const Handle(Prs3d_Presentation)& thePrs,
                                const Standard_Integer theMode)
{
  GeomAdaptor_Curve anAdaptorCurve(myCurve);
  switch (theMode)
  {
    case 1:
    {
      Handle(Prs3d_Drawer) aPoleDrawer = new Prs3d_Drawer();
      aPoleDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0));
      StdPrs_PoleCurve::Add(thePrs, anAdaptorCurve, aPoleDrawer);
    }
    Standard_FALLTHROUGH
    case 0:
    {
      StdPrs_Curve::Add(thePrs, anAdaptorCurve, myDrawer);
      break;
    }
  }
}
