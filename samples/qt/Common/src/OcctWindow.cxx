#include <OcctWindow.h>

IMPLEMENT_STANDARD_RTTIEXT(OcctWindow,Aspect_Window)

// =======================================================================
// function : OcctWindow
// purpose  :
// =======================================================================
OcctWindow::OcctWindow ( QWidget* theWidget, const Quantity_NameOfColor theBackColor )
: Aspect_Window(), 
  myWidget( theWidget )
{
  SetBackground (theBackColor);
  myXLeft   = myWidget->rect().left();
  myYTop    = myWidget->rect().top();
  myXRight  = myWidget->rect().right();
  myYBottom = myWidget->rect().bottom();
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void OcctWindow::Destroy()
{
  myWidget = NULL;
}

// =======================================================================
// function : NativeParentHandle
// purpose  :
// =======================================================================
Aspect_Drawable OcctWindow::NativeParentHandle() const
{
  QWidget* aParentWidget = myWidget->parentWidget();
  if ( aParentWidget != NULL )
    return (Aspect_Drawable)aParentWidget->winId();
  else
    return 0;
}

// =======================================================================
// function : NativeHandle
// purpose  :
// =======================================================================
Aspect_Drawable OcctWindow::NativeHandle() const
{
  return (Aspect_Drawable)myWidget->winId();
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean OcctWindow::IsMapped() const
{
  return !( myWidget->isMinimized() || myWidget->isHidden() );
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
Aspect_TypeOfResize OcctWindow::DoResize() const
{
  int                 aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if ( !myWidget->isMinimized() )
  {
    if ( Abs ( myWidget->rect().left()   - myXLeft   ) > 2 ) aMask |= 1;
    if ( Abs ( myWidget->rect().right()  - myXRight  ) > 2 ) aMask |= 2;
    if ( Abs ( myWidget->rect().top()    - myYTop    ) > 2 ) aMask |= 4;
    if ( Abs ( myWidget->rect().bottom() - myYBottom ) > 2 ) aMask |= 8;

    switch ( aMask )
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

    *( ( Standard_Integer* )&myXLeft  ) = myWidget->rect().left();
    *( ( Standard_Integer* )&myXRight ) = myWidget->rect().right();
    *( ( Standard_Integer* )&myYTop   ) = myWidget->rect().top();
    *( ( Standard_Integer* )&myYBottom) = myWidget->rect().bottom();
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
  return Standard_Real( aRect.right() - aRect.left() ) / Standard_Real( aRect.bottom() - aRect.top() );
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void OcctWindow::Size ( Standard_Integer& theWidth, Standard_Integer& theHeight ) const
{
  QRect aRect = myWidget->rect();
  theWidth  = aRect.right();
  theHeight = aRect.bottom();
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void OcctWindow::Position ( Standard_Integer& theX1, Standard_Integer& theY1,
                            Standard_Integer& theX2, Standard_Integer& theY2 ) const
{
  theX1 = myWidget->rect().left();
  theX2 = myWidget->rect().right();
  theY1 = myWidget->rect().top();
  theY2 = myWidget->rect().bottom();
}