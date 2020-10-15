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

#include "OcctWindow.h"

IMPLEMENT_STANDARD_RTTIEXT(OcctWindow, Aspect_Window)

// =======================================================================
// function : OcctWindow
// purpose  :
// =======================================================================
OcctWindow::OcctWindow(QWidget* theWidget, const Quantity_NameOfColor theBackColor)
: myWidget (theWidget)
{
  SetBackground(theBackColor);
  myXLeft   = myWidget->rect().left();
  myYTop    = myWidget->rect().top();
  myXRight  = myWidget->rect().right();
  myYBottom = myWidget->rect().bottom();
}

// =======================================================================
// function : NativeParentHandle
// purpose  :
// =======================================================================
Aspect_Drawable OcctWindow::NativeParentHandle() const
{
  QWidget* aParentWidget = myWidget->parentWidget();
  if (aParentWidget != NULL)
  {
    return (Aspect_Drawable)aParentWidget->winId();
  }
  else
  {
    return 0;
  }
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void OcctWindow::Map() const
{
  myWidget->show();
  myWidget->update();
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void OcctWindow::Unmap() const
{
  myWidget->hide();
  myWidget->update();
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize OcctWindow::DoResize()
{
  int                 aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (!myWidget->isMinimized())
  {
    if (Abs(myWidget->rect().left() - myXLeft) > 2)
    {
      aMask |= 1;
    }
    if (Abs(myWidget->rect().right() - myXRight) > 2)
    {
      aMask |= 2;
    }
    if (Abs(myWidget->rect().top() - myYTop) > 2)
    {
      aMask |= 4;
    }
    if (Abs(myWidget->rect().bottom() - myYBottom) > 2)
    {
      aMask |= 8;
    }

    switch (aMask)
    {
    case 0:
      aMode = Aspect_TOR_NO_BORDER;
      break;
    case 1:
      aMode = Aspect_TOR_LEFT_BORDER;
      break;
    case 2:
      aMode = Aspect_TOR_RIGHT_BORDER;
      break;
    case 4:
      aMode = Aspect_TOR_TOP_BORDER;
      break;
    case 5:
      aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;
      break;
    case 6:
      aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
      break;
    case 8:
      aMode = Aspect_TOR_BOTTOM_BORDER;
      break;
    case 9:
      aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
      break;
    case 10:
      aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
      break;
    default:
      break;
    }  // end switch

    myXLeft   = myWidget->rect().left();
    myXRight  = myWidget->rect().right();
    myYTop    = myWidget->rect().top();
    myYBottom = myWidget->rect().bottom();
  }

  return aMode;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real OcctWindow::Ratio() const
{
  QRect aRect = myWidget->rect();
  return Standard_Real(aRect.right() - aRect.left()) / Standard_Real(aRect.bottom() - aRect.top());
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void OcctWindow::Size(Standard_Integer& theWidth, Standard_Integer& theHeight) const
{
  QRect aRect = myWidget->rect();
  theWidth  = aRect.width();
  theHeight = aRect.height();
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void OcctWindow::Position(Standard_Integer& theX1, Standard_Integer& theY1,
  Standard_Integer& theX2, Standard_Integer& theY2) const
{
  theX1 = myWidget->rect().left();
  theX2 = myWidget->rect().right();
  theY1 = myWidget->rect().top();
  theY2 = myWidget->rect().bottom();
}
