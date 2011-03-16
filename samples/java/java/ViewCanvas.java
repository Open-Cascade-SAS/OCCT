
//Title:        Open CASCADE Technology Samples
//Version:
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:  


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

import util.*;

/**
 * Native window component to output graphic data by Cas.Cade tools.
 */
public class ViewCanvas extends Canvas
                        implements ActionListener,
                                   MouseListener,
                                   MouseMotionListener,
                                   ComponentListener
{
  protected ViewPanel parent;

  static{
    System.out.println("Info: Load NativePaint lib...");
    System.loadLibrary("TKNativePaint");
    System.out.println("Info: Loading NativePaint lib done");
  }

  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui = ResourceBundle.getBundle("properties.Viewer");
  static protected ResourceBundle AppRes =
         ResourceBundle.getBundle("properties.AppRes");

  //-----------------------------------------------------------//
  // Internal state
  //-----------------------------------------------------------//
  private int currX = 0, currY = 0;   // current position of cursor
  private int startX = 0, startY = 0; // position of cursor
                                      // at the beginning of operation

  //-----------------------------------------------------------//
  // Signs of operation
  //-----------------------------------------------------------//
  static final int NOTHING = 0;
  static final int WINDOWFIT = 1;
  static final int ZOOMVIEW = 2;
  static final int PANVIEW = 3;
  static final int MAGNIFY = 4;
  static final int PANGLOBAL = 5;
  static final int ROTATE = 6;

  private int theOperation = NOTHING;

  private Cursor myCursor = defCursor;
  private boolean drawrect = false;
  private boolean CursorIsHand = false;

  protected boolean needResize = true;
  protected boolean hasWindow = false;

  //-----------------------------------------------------------//
  // Cursors
  //-----------------------------------------------------------//
  static Cursor defCursor = new Cursor(Cursor.DEFAULT_CURSOR);
  static Cursor handCursor = new Cursor(Cursor.HAND_CURSOR);


  //-----------------------------------------------------------//
  // Type of Operation System
  //-----------------------------------------------------------//
  public static final int OS_UNKNOWN = 0;
  public static final int OS_WINDOWS = 1;
  public static final int OS_X11 = 2;

  private static int osType = getOSType();

  public static int getOSType()
  {
    String os = System.getProperty("os.name");
    if (os.startsWith("Wind"))
      return OS_WINDOWS;
    else
      return OS_X11;
  }


  //-----------------------------------------------------------//
  // Graphics configurations
  //-----------------------------------------------------------//
  static GraphicsConfiguration theGraphicsConfiguration = getInternalGraphicsConfiguration();

  public static GraphicsConfiguration getInternalGraphicsConfiguration()
  {
    GraphicsConfiguration theGC = null;
    GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();

    boolean grInfo = Boolean.getBoolean("JADCanvas_INFO");
//    int vid = Integer.getInteger("JADCanvas_VisualID", -1).intValue();
    int vid = -1;

    // Check if there is property "VisualID"
    //--------------------------------------------
    Enumeration keys = AppRes.getKeys();
    while (keys.hasMoreElements())
    {
      String prop = (String) keys.nextElement();
      if (prop.equals("VisualID"))
      {
        Integer iVid = new Integer(AppRes.getString("VisualID"));
        vid = iVid.intValue();
        break;
      }
    }
    int gc_index = -1;

    // Find the proper graphics configuration
    //--------------------------------------------
    if (grInfo)
    {
      System.out.println("\nGraphicsConfiguration:");
      System.out.println("\ngraphics environment = " + ge);
    }

    if (getOSType() == OS_X11)
    {
      GraphicsDevice gd[] = ge.getScreenDevices();
      if (grInfo)
        System.out.println("count of graphics devices = " + gd.length);

      for (int i = 0; i < gd.length; i++)
      {
        if (grInfo)
        	System.out.println("graphics device [" + i + "] = " + gd[i]);

      	GraphicsConfiguration gc[] = gd[i].getConfigurations();
        if (grInfo)
        	System.out.println("count of graphics configurations = " + gc.length);

      	for (int j = 0; j < gc.length; j++)
      	{
      	  try
      	  {
      	    handleAccess win_access = (handleAccess)
      	        Class.forName("util.x11.X11HandleAccess").newInstance();

            if (grInfo)
            {
	      System.out.println("\n\tgraphics configuration [" + j + "] = " + gc[j]);
	      System.out.println("\tvisual ID = " + win_access.getVisualId(gc[j]));
            }
            if (win_access.getVisualId(gc[j]) == vid)
	      gc_index = j;
      	  }
      	  catch (Exception e)
      	  {
      	    System.out.println(e);
      	  }

          if (grInfo)
          {
	    java.awt.image.ColorModel cm = gc[j].getColorModel();
	    System.out.println("\tColor model = " + cm);

	    java.awt.color.ColorSpace cs = cm.getColorSpace();
	    System.out.println("\tColor space = " + cs);
	    System.out.println("\tColor space type = " + cs.getType());
	    System.out.println("\tColor space is CS_sRGB = " + cs.isCS_sRGB());
	    System.out.println("\tNumber of components = " + cs.getNumComponents());
          }
      	}
        if (gc_index != -1)
        	theGC = gc[gc_index];
      }
    }

    if (gc_index == -1)
    {
      GraphicsDevice gd = ge.getDefaultScreenDevice();
      theGC = gd.getDefaultConfiguration();
    }

    return theGC;
  }


//=======================================================================//
// Constructor
//=======================================================================//
  public ViewCanvas()
  {
//    super(theGraphicsConfiguration);
    addComponentListener(this);
    addMouseListener(this);
    addMouseMotionListener(this);
  }

  public ViewCanvas(ViewPanel panel)
  {
    this();
    setParent(panel);
  }

  public void setParent(ViewPanel panel)
  {
    parent = panel;
  }



//=======================================================================//
// Settings of window
//=======================================================================//
  /**
   * Returns handle of native window for this component.
   */
//   public int getWinData()
//   {
//     Graphics g = this.getGraphics();
//     int pData = 0;

//     handleAccess win_access;

//     try
//     {
//       if (osType == OS_WINDOWS)
//         win_access = (handleAccess) Class.forName("util.win32.WinHandleAccess").newInstance();
//       else
//         win_access = (handleAccess) Class.forName("util.x11.X11HandleAccess").newInstance();

//       pData = win_access.getWinHandle(this, g);
//     }
//     catch (Exception e)
//     {
//       System.out.println(e);
//     }
//     return pData;
//   }

  public boolean hasWindow()
  {
    return hasWindow;
  }

// //-----------------------------------------------------------------------//
//   /**
//    * Updates this component.
//    */
//   public void update(Graphics g)
//   {
//     if (!isWindow)
//     {
//       SetWindow();
//       isWindow = true;
//     }
//     if (isWindow) paint(g);
//   }

//-----------------------------------------------------------------------//
  /** Updates this component. */
  public void update(Graphics g)
//-----------------------------------------------------------------------//
  {
    System.out.println("Info: start update");
    paint(g);
    paintRect(g);
    System.out.println("Info: end update");
  }

  public void paintRect(Graphics g)
  {
    if (drawrect)
    {
      System.out.println("Info: Paint Rect");
      g.setXORMode(Color.white);
      g.setColor(Color.black);
      Rectangle rect = this.getRectangle(startX, startY, currX, currY);
      g.drawRect(rect.x, rect.y, rect.width, rect.height);
      g.setPaintMode();
    }
  }


//=======================================================================//
// Additional operations
//=======================================================================//
  static public Rectangle getRectangle(int x1, int y1, int x2, int y2)
  {
    Rectangle rect = new Rectangle();
    rect.setLocation(Math.min(x1, x2), Math.min(y1, y2));
    rect.setSize(Math.abs(x2-x1), Math.abs(y2-y1));
    return rect;
  }


//-----------------------------------------------------------------------//
  public void ActivateWindowFit()
  {
    SamplesStarter.put_info(resGui.getString("Help_Zoom"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = WINDOWFIT;
    this.setCursor(handCursor);
  }

//-----------------------------------------------------------------------//
  public void ActivateZoom()
  {
    SamplesStarter.put_info(resGui.getString("Help_DynZoom"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = ZOOMVIEW;
    this.setCursor(defCursor);
  }

//-----------------------------------------------------------------------//
  public void ActivatePanning()
  {
    SamplesStarter.put_info(resGui.getString("Help_DynPan"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = PANVIEW;
    this.setCursor(defCursor);
  }

//-----------------------------------------------------------------------//
  public void ActivateGlobalPanning()
  {
    SamplesStarter.put_info(resGui.getString("Help_GlobPan"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = PANGLOBAL;
    this.setCursor(defCursor);
  }

//-----------------------------------------------------------------------//
  public void ActivateMagnify()
  {
    SamplesStarter.put_info(resGui.getString("Help_MagnifyView"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = MAGNIFY;
    this.setCursor(handCursor);
  }

//-----------------------------------------------------------------------//
  public void ActivateRotation()
  {
    SamplesStarter.put_info(resGui.getString("Help_Rotate"));
    if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
    theOperation = ROTATE;
    this.setCursor(defCursor);
  }

//-----------------------------------------------------------------------//
  public void ResetState()
  {
    drawrect = false;
    if ((theOperation != NOTHING) || CursorIsHand)
    {
      this.setCursor(myCursor);
    }
    if (theOperation != NOTHING)
    {
      SamplesStarter.put_info("");
      theOperation = NOTHING;
    }
    CursorIsHand = false;
  }


//=======================================================================//
// Redefined operations
//=======================================================================//
  public final void SetWindow()
  {
  }

  public void Magnify(Rectangle rect)
  {
  }

  public void FitAll()
  {
  }

  public void WindowFit(Rectangle rect)
  {
  }

  public void Zoom(int x0, int y0, int x, int y)
  {
  }

  public void Pan(int dx, int dy)
  {
  }

  public void SetCenter(int x, int y)
  {
  }

  public void StartRotation(int x0, int y0)
  {
  }

  public void Rotate(int x, int y)
  {
  }

  public void EndRotation()
  {
  }

  public void SetProjection(String type)
  {
  }

  public void SetDegenerateMode(boolean isOn)
  {
  }

  public void EditGrid()
  {
  }

  public void EraseGrid()
  {
  }

  public void ResetView()
  {
  }

  public void ActivateGrid(String type)
  {
  }

  public Color GetBackgroundColor()
  {
    return Color.black;
  }

  public void SetBackgroundColor(Color color)
  {
  }


//=======================================================================//
//                             ActionListener                            //
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String nameAction = e.getActionCommand();

    ResetState();

    // Zoom buttons
    if (nameAction.equals("FitAll")) FitAll();
    else if (nameAction.equals("DynZoom")) ActivateZoom();
    else if (nameAction.equals("Zoom")) ActivateWindowFit();
    // Pann buttons
    else if (nameAction.equals("DynPan")) ActivatePanning();
    else if (nameAction.equals("GlobPan")) ActivateGlobalPanning();
    // Rotation
    else if (nameAction.equals("Rotate")) ActivateRotation();
    // Projection
    else if (nameAction.equals("Front") ||
             nameAction.equals("Top") ||
             nameAction.equals("Left") ||
             nameAction.equals("Back") ||
             nameAction.equals("Right") ||
             nameAction.equals("Bottom") ||
             nameAction.equals("Axo")) SetProjection(nameAction);
    // Reset view
    else if (nameAction.equals("Reset")) ResetView();
    // Degeneration mode
    else if (nameAction.equals("HiddenOff")) SetDegenerateMode(false);
    else if (nameAction.equals("HiddenOn")) SetDegenerateMode(true);
    // Grid management
    else if (nameAction.equals("RectLine") ||
             nameAction.equals("RectPoint") ||
             nameAction.equals("CircLine") ||
             nameAction.equals("CircPoint")) ActivateGrid(nameAction);
    else if (nameAction.equals("EditGrid")) EditGrid();
    else if (nameAction.equals("EraseGrid")) EraseGrid();
  }

//=======================================================================//
//                             MouseListener                             //
//=======================================================================//
  public void mouseClicked(MouseEvent event)
  {
    if (theOperation == PANGLOBAL)
    {
      if (event.getModifiers() == event.BUTTON1_MASK)
      {
        SetCenter(event.getX(), event.getY());
        ResetState();
      }
    }
    else
      if (parent != null) parent.MouseClicked(event);
  }

//-----------------------------------------------------------------------//
  public void mouseEntered(MouseEvent event)
  {
    if (parent != null) parent.MouseEntered(event);
  }

//-----------------------------------------------------------------------//
  public void mouseExited(MouseEvent event)
  {
    if (parent != null) parent.MouseExited(event);
  }

//-----------------------------------------------------------------------//
  public void mousePressed(MouseEvent event)
  {
    switch (theOperation)
    {
    case MAGNIFY:
    case WINDOWFIT:
    case ZOOMVIEW:
    case PANVIEW:
      if (event.getModifiers() == event.BUTTON1_MASK)
      {
        startX = event.getX();
        startY = event.getY();
      }
      break;

    case ROTATE:
      if (event.getModifiers() == event.BUTTON1_MASK)
      {
        startX = event.getX();
        startY = event.getY();
        StartRotation(startX, startY);
      }
      break;

    default:
      startX = event.getX();
      startY = event.getY();
      if (event.getModifiers() == (event.CTRL_MASK + event.BUTTON1_MASK))
      {
        if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
        theOperation = ZOOMVIEW;
      }
      else if (event.getModifiers() == (event.CTRL_MASK + event.BUTTON2_MASK))
      {
        if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
        theOperation = PANVIEW;
      }
      else if (event.getModifiers() == (event.CTRL_MASK + event.BUTTON3_MASK))
      {
        if (theOperation == NOTHING && !CursorIsHand) myCursor = this.getCursor();
        theOperation = ROTATE;
        StartRotation(startX, startY);
      }
      else
      {
	if (SwingUtilities.isLeftMouseButton(event))
	  drawrect = true;
        if (parent != null) parent.MousePressed(event);
      }
    }
  }

//-----------------------------------------------------------------------//
  public void mouseReleased(MouseEvent event)
  {
    switch (theOperation)
    {
    case ROTATE:
      EndRotation();
      ResetState();
      break;
    case MAGNIFY:
    case WINDOWFIT:
      if (event.getModifiers() == event.BUTTON1_MASK)
      {
        currX = event.getX();
        currY = event.getY();
        Rectangle rect = getRectangle(startX, startY, currX, currY);
        if (!rect.isEmpty())
        {
          if (theOperation == MAGNIFY)
            Magnify(rect);
          else if (theOperation == WINDOWFIT)
            WindowFit(rect);
        }
        ResetState();
      }
      break;
    case PANGLOBAL:
      break;
    default:
      if (SwingUtilities.isLeftMouseButton(event) && drawrect)
        repaint();
      ResetState();
      if (parent != null) parent.MouseReleased(event);
    }
  }

//=======================================================================//
//                         MouseMotionListener                           //
//=======================================================================//
  public void mouseDragged(MouseEvent event)
  {
    currX = event.getX();
    currY = event.getY();

    switch (theOperation)
    {
      case ROTATE:
	Rotate(currX, currY);
	break;
      case ZOOMVIEW:
        Zoom(startX, startY, currX, currY);
	startX = currX;
	startY = currY;
	break;
      case PANVIEW:
	Pan(currX - startX, startY - currY);
	startX = currX;
	startY = currY;
	break;
      case MAGNIFY:
      case WINDOWFIT:
	drawrect = true;
	repaint();
	break;
      default:
	if (SwingUtilities.isLeftMouseButton(event))
	{
	  if (drawrect) repaint();
	  if (!CursorIsHand)
	  {
	    if (theOperation == NOTHING && !CursorIsHand) myCursor = getCursor();
	    setCursor(handCursor);
	    CursorIsHand = true;
	  }
	}
	if (parent != null) parent.MouseDragged(event);
    }
  }

//-----------------------------------------------------------------------//
  public void mouseMoved(MouseEvent event)
  {
    currX = event.getX();
    currY = event.getY();

    if (parent != null) parent.MouseMoved(event);
  }


//=======================================================================//
//                          ComponentListener                            //
//=======================================================================//
  public void componentResized(ComponentEvent e)
  {
    needResize = true;
    repaint();
  }

  public void componentMoved(ComponentEvent e)
  {
  }

  public void componentShown(ComponentEvent e)
  {
  }

  public void componentHidden(ComponentEvent e)
  {
  }

}
