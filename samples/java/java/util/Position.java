
//Title:        JAD test application
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:  A simple application for testing new JAD architecture.


package util;

import java.awt.*;

/**
 * The set of static methods to support window management.
 */

public class Position
{
  /**
   * Contains the screen size
   */
  static private Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

  /**
   * Defines top-left point of area enabled to locate windows.
   */
  static private Point startPoint = new Point(100, 100);

  /**
   * Defines bottom-right point of area enabled to locate windows.
   */
  static private Point lastPoint = new Point(screenSize.width-100, screenSize.height-30);

  /**
   * Defines default size of new window
   */
  static private Dimension defaultSize = new Dimension(600, 800);

  /**
   * Defines size of area enabled to locate window.
   */
  static private Dimension enabledArea = new Dimension(lastPoint.x - startPoint.x,
                                                       lastPoint.y - startPoint.y);

  /**
   * Defines horisontal indent to locate next window.
   */
  static private int indentX = 30;

  /**
   * Defines vertical indent to locate next window.
   */
  static private int indentY = 30;


  /**
   * Defines current location of new window.
   */
  static private Point currentLocation = new Point(startPoint);


//=======================================================================//
//                          Setting Display Area                         //
//=======================================================================//
  static
  {
    changeLocation();
  }

//=======================================================================//
  /**
   * Sets the area enabled to locate windows.
   */
  static public void setEnabledArea(Rectangle rect)
  {
    startPoint = new Point(rect.getLocation());
    lastPoint = new Point(rect.x + rect.width, rect.y + rect.height);
    changeLocation();
  }

  /**
   * Sets the area enabled to locate windows.
   */
  static public void setEnabledArea(Point pnt, Dimension size)
  {
    startPoint = new Point(pnt);
    lastPoint = new Point(pnt.x + size.width, pnt.y + size.height);
    changeLocation();
  }

  /**
   * Sets the area enabled to locate windows.
   */
  static public void setEnabledArea(int x, int y, int width, int height)
  {
    startPoint = new Point(x, y);
    lastPoint = new Point(x + width, y + height);
    changeLocation();
  }

//=======================================================================//
  /**
   * Returns the area enabled to locate windows.
   */
  static public Rectangle getEnabledArea()
  {
    return new Rectangle(startPoint, enabledArea);
  }

//=======================================================================//
  /**
   * Sets top-left point of enabled area.
   */
  static public void setStartPoint(Point pnt)
  {
    startPoint = new Point(pnt);
    changeLocation();
  }

  /**
   * Sets top-left point of enabled area.
   */
  static public void setStartPoint(int x, int y)
  {
    startPoint = new Point(x, y);
    changeLocation();
  }

//=======================================================================//
  /**
   * Returns top-left point of enabled area.
   */
  static public Point getStartPoint()
  {
    return startPoint;
  }

//=======================================================================//
  /**
   * Sets bottom-right point of enabled area.
   */
  static public void setLastPoint(Point pnt)
  {
    lastPoint = new Point(pnt);
    changeLocation();
  }

  /**
   * Sets bottom-right point of enabled area.
   */
  static public void setLastPoint(int x, int y)
  {
    lastPoint = new Point(x, y);
    changeLocation();
  }

//=======================================================================//
  /**
   * Returns bottom-right point of enabled area.
   */
  static public Point getLastPoint()
  {
    return lastPoint;
  }

//=======================================================================//
  /**
   * Sets vertical indent to locate next window.
   */
  static public void setIndent(int y)
  {
    indentY = y;
    changeLocation();
  }

//=======================================================================//
  /**
   * Returns horisontal and vertical indents to locate next window.
   */
  static public Dimension getIndent()
  {
    return new Dimension(indentX, indentY);
  }

//=======================================================================//
  /**
   * Returns default size of new window.
   */
  static public Dimension getDefaultSize()
  {
    return defaultSize;
  }

//=======================================================================//
  /**
   * Recalculates dependent parameters according to new values.
   */
  static private void changeLocation()
  {
    currentLocation = new Point(startPoint);
    enabledArea = new Dimension(lastPoint.x - startPoint.x,
                                lastPoint.y - startPoint.y);

    indentX = indentY * (enabledArea.width/enabledArea.height);

    defaultSize.width = enabledArea.width - indentX*10;
    defaultSize.height = enabledArea.height - indentY*10;
  }

//=======================================================================//
//                           Windows Arrangement                         //
//=======================================================================//
/**
 * Locates window in the center of enabled area
 */
  static public void centerWindow(Window w)
  {
    Dimension wndSize = w.getSize();
    w.setLocation(startPoint.x + ((enabledArea.width - wndSize.width) / 2),
                  startPoint.y + ((enabledArea.height - wndSize.height) / 2));
  }

//=======================================================================//
/**
 * Locates new window with indention from previous one
 */
  static public void locateWindow(Window w)
  {
    Dimension wndSize = w.getSize();

    if (wndSize.height > screenSize.height)
      currentLocation.y = 0;
    else if (wndSize.height > (screenSize.height - startPoint.y))
      currentLocation.y = screenSize.height - wndSize.height;
    else if (wndSize.height > (enabledArea.height - currentLocation.y + startPoint.y))
      currentLocation.y = startPoint.y;

    if (wndSize.width > screenSize.width)
      currentLocation.x = 0;
    else if (wndSize.width > (screenSize.width - startPoint.x))
      currentLocation.x = screenSize.width - wndSize.width;
    else if (wndSize.width > (enabledArea.width - currentLocation.x + startPoint.x))
      currentLocation.x = startPoint.x;

    w.setLocation(currentLocation);
    currentLocation.translate(indentX, indentY);
  }

//=======================================================================//
/**
 * Locates windows by cascade
 * List of windows should be sorted so that the active window would be first
 */
  static public void arrangeCascade(Window[] wlist)
  {
    Point currentPoint = new Point(startPoint);

    for (int i = wlist.length; i > 0; i--)
    {
      Window w = wlist[i-1];
      if (w == null) continue;

      w.setSize(defaultSize);

      if (defaultSize.height > (lastPoint.y - currentPoint.y))
        currentPoint.y = startPoint.y;
      if (defaultSize.width > (lastPoint.x - currentPoint.x))
        currentPoint.x = startPoint.x;

      w.setLocation(currentPoint);

      if (w instanceof Frame)
      {
        Frame fr = (Frame) w;
        if (fr.getState() == Frame.ICONIFIED)
          fr.setState(Frame.NORMAL);
      }

      currentPoint.translate(indentX, indentY);
    }
  }

//=======================================================================//
/**
 * Locates windows by vertical or horizontal tile
 * List of windows should be sorted so that the active window would be first
 */
  static public void arrangeTile(Window[] wlist, boolean isVert)
  {
    Point currentPoint = new Point(startPoint);

    int count = wlist.length;
    if (count == 0) return;

    // find counts of frames along horizontal and vertical sides
    int k, cntX, cntY;
    for (k = 1; k < 10; k++)
    {
      if (count <= k*k) break;
    }

    if (count == k*k)
    {
      cntX = k; cntY = k;
    }
    else
    {
      if (isVert)
      {
        cntX = k;
        if (count > k*(k-1))
          cntY = k;
        else
          cntY = k-1;
      }
      else
      {
        cntY = k;
        if (count > k*(k-1))
          cntX = k;
        else
          cntX = k-1;
      }
    }

    // find size of each frame
    Dimension rectSize = new Dimension();
    rectSize.width = enabledArea.width/cntX;
    rectSize.height = enabledArea.height/cntY;

    // arrange windows
    for (int i = 0; i < count; i++)
    {
      Window w = wlist[i];
      if (w == null) continue;

      w.setSize(rectSize);
      w.setLocation(currentPoint);

      if (w instanceof Frame)
      {
        Frame fr = (Frame) w;
        if (fr.getState() == Frame.ICONIFIED)
          fr.setState(Frame.NORMAL);
      }

      currentPoint.x = currentPoint.x + rectSize.width;
      if (currentPoint.x >= rectSize.width*cntX)
      {
        currentPoint.x = startPoint.x;
        currentPoint.y = currentPoint.y + rectSize.height;
      }
    }
  }

}
