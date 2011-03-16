
//Title:        Open CASCADE Technology Samples
//Version:
//Copyright:    Copyright (c) 2000
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;

import util.*;
import CASCADESamplesJni.*;


public class ViewPanel extends JPanel implements ActionListener,
                                                KeyListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static private ResourceBundle res1 = ResourceBundle.getBundle("properties.Viewer");
  static private ResourceBundle res = ResourceBundle.getBundle("properties.ViewerIcon");

  // Zooming
  static private ImageIcon imgFitAll = new ImageIcon(res.getString("IC_FIT_ALL"));
  static private ImageIcon imgZoom = new ImageIcon(res.getString("IC_ZOOM"));
  static private ImageIcon imgDynZoom = new ImageIcon(res.getString("IC_DYN_ZOOM"));

  // Panning
  static private ImageIcon imgDynPan = new ImageIcon(res.getString("IC_DYN_PAN"));
  static private ImageIcon imgGlobPan = new ImageIcon(res.getString("IC_GLOB_PAN"));

  // Projection
  static private ImageIcon imgFront = new ImageIcon(res.getString("IC_FRONT"));
  static private ImageIcon imgTop = new ImageIcon(res.getString("IC_TOP"));
  static private ImageIcon imgLeft = new ImageIcon(res.getString("IC_LEFT"));
  static private ImageIcon imgBack = new ImageIcon(res.getString("IC_BACK"));
  static private ImageIcon imgRight = new ImageIcon(res.getString("IC_RIGHT"));
  static private ImageIcon imgBottom = new ImageIcon(res.getString("IC_BOTTOM"));
  static private ImageIcon imgAxo = new ImageIcon(res.getString("IC_AXO"));

  // Rotation
  static private ImageIcon imgRotate = new ImageIcon(res.getString("IC_ROTATE"));

  // Reset view
  static private ImageIcon imgReset = new ImageIcon(res.getString("IC_RESET"));

  // Degenerated mode
  static private ImageIcon imgHiddenOff = new ImageIcon(res.getString("IC_HIDDEN_OFF"));
  static private ImageIcon imgHiddenOn = new ImageIcon(res.getString("IC_HIDDEN_ON"));

  // Grid management
  static private ImageIcon imgRectLine = new ImageIcon(res.getString("IC_RECT_LINE"));
  static private ImageIcon imgRectPoint = new ImageIcon(res.getString("IC_RECT_POINT"));
  static private ImageIcon imgCircLine = new ImageIcon(res.getString("IC_CIRC_LINE"));
  static private ImageIcon imgCircPoint = new ImageIcon(res.getString("IC_CIRC_POINT"));
  static private ImageIcon imgEditGrid = new ImageIcon(res.getString("IC_EDIT_GRID"));
  static private ImageIcon imgEraseGrid = new ImageIcon(res.getString("IC_ERASE_GRID"));


  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  private JToolBar buttonsBar = new JToolBar(JToolBar.HORIZONTAL);
  private ViewCanvas VPort;

  static private Insets margin = new Insets(1,1,1,1);


  //-----------------------------------------------------------//
  // Listeners
  //-----------------------------------------------------------//
  private transient Vector mouseMotionListeners;
  private transient Vector mouseListeners;
  private transient Vector keyListeners;


  //-----------------------------------------------------------//
  // OS type
  //-----------------------------------------------------------//
  private static int OsType = ViewCanvas.getOSType();


  //-----------------------------------------------------------//
  // View type
  //-----------------------------------------------------------//
  public static int VIEW_3D = 0;
  public static int VIEW_2D = 1;


//=======================================================================//
// Constructor
//=======================================================================//
  public ViewPanel()
  {
    this(VIEW_3D);
  }

  public ViewPanel(int type)
  {
    try
    {
      jbInit(type);
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }

//=======================================================================//
// Component initialization
//=======================================================================//
  private void jbInit(int type) throws Exception
  {
    this.setBackground(Color.gray);
    this.setLayout(new BorderLayout());

    // Create view port
    JPanel port = new JPanel(new BorderLayout());
    port.setBorder(BorderFactory.createLoweredBevelBorder());
    VPort = createViewPort();
    VPort.setParent(this);
    port.add(VPort, BorderLayout.CENTER);
    add(port, BorderLayout.CENTER);

    // Create toolbar
    buttonsBar = new JToolBar();
    buttonsBar.setBorder(BorderFactory.createEmptyBorder(3, 3, 3, 3));
    buttonsBar.setFloatable(true);
    if (type == VIEW_2D)
      createMenu2d();
    else
      createMenu3d();
    add(buttonsBar, BorderLayout.NORTH);

    addKeyListener(this);
  }

  //-----------------------------------------------------------//
  // Creation view port
  //-----------------------------------------------------------//
  public ViewCanvas createViewPort()
  {
    return new ViewCanvas();
  }

  public ViewCanvas getViewPort()
  {
    return VPort;
  }


  //-----------------------------------------------------------//
  // Creation menu for view3d
  //-----------------------------------------------------------//
  private void createMenu3d()
  {
    JButton button;
    JToggleButton toggle;

    // Zooming
    button = new HeavyButton(imgFitAll);
    button.setToolTipText(res1.getString("TT_FIT_ALL"));
    button.setActionCommand("FitAll");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgZoom);
    button.setToolTipText(res1.getString("TT_ZOOM"));
    button.setActionCommand("Zoom");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgDynZoom);
    button.setToolTipText(res1.getString("TT_DYN_ZOOM"));
    button.setActionCommand("DynZoom");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Panning
    button = new HeavyButton(imgDynPan);
    button.setToolTipText(res1.getString("TT_DYN_PAN"));
    button.setActionCommand("DynPan");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgGlobPan);
    button.setToolTipText(res1.getString("TT_GLOB_PAN"));
    button.setActionCommand("GlobPan");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Projection
    button = new HeavyButton(imgFront);
    button.setToolTipText(res1.getString("TT_FRONT"));
    button.setActionCommand("Front");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgTop);
    button.setToolTipText(res1.getString("TT_TOP"));
    button.setActionCommand("Top");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgLeft);
    button.setToolTipText(res1.getString("TT_LEFT"));
    button.setActionCommand("Left");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgBack);
    button.setToolTipText(res1.getString("TT_BACK"));
    button.setActionCommand("Back");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgRight);
    button.setToolTipText(res1.getString("TT_RIGHT"));
    button.setActionCommand("Right");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgBottom);
    button.setToolTipText(res1.getString("TT_BOTTOM"));
    button.setActionCommand("Bottom");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgAxo);
    button.setToolTipText(res1.getString("TT_AXO"));
    button.setActionCommand("Axo");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Rotation
    button = new HeavyButton(imgRotate);
    button.setToolTipText(res1.getString("TT_ROTATE"));
    button.setActionCommand("Rotate");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Reset view
    button = new HeavyButton(imgReset);
    button.setToolTipText(res1.getString("TT_RESET"));
    button.setActionCommand("Reset");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Degeneration mode
    ButtonGroup group = new ButtonGroup();
    toggle = new JToggleButton(imgHiddenOff, false);
    toggle.setToolTipText(res1.getString("TT_HIDDEN_OFF"));
    toggle.setActionCommand("HiddenOff");
    toggle.addActionListener(VPort);
    toggle.setMargin(margin);
    group.add(toggle);
    buttonsBar.add(toggle);

    toggle = new JToggleButton(imgHiddenOn, true);
    toggle.setToolTipText(res1.getString("TT_HIDDEN_ON"));
    toggle.setActionCommand("HiddenOn");
    toggle.addActionListener(VPort);
    toggle.setMargin(margin);
    group.add(toggle);
    buttonsBar.add(toggle);

    VPort.SetDegenerateMode(true);
  }


  //-----------------------------------------------------------//
  // Creation menu for view2d
  //-----------------------------------------------------------//
  private void createMenu2d()
  {
    JButton button;

    // Zooming
    button = new HeavyButton(imgFitAll);
    button.setToolTipText(res1.getString("TT_FIT_ALL"));
    button.setActionCommand("FitAll");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgZoom);
    button.setToolTipText(res1.getString("TT_ZOOM"));
    button.setActionCommand("Zoom");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgDynZoom);
    button.setToolTipText(res1.getString("TT_DYN_ZOOM"));
    button.setActionCommand("DynZoom");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Panning
    button = new HeavyButton(imgDynPan);
    button.setToolTipText(res1.getString("TT_DYN_PAN"));
    button.setActionCommand("DynPan");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgGlobPan);
    button.setToolTipText(res1.getString("TT_GLOB_PAN"));
    button.setActionCommand("GlobPan");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    buttonsBar.addSeparator();

    // Grid management
    button = new HeavyButton(imgRectLine);
    button.setToolTipText(res1.getString("TT_RECT_LINE"));
    button.setActionCommand("RectLine");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgRectPoint);
    button.setToolTipText(res1.getString("TT_RECT_POINT"));
    button.setActionCommand("RectPoint");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgCircLine);
    button.setToolTipText(res1.getString("TT_CIRC_LINE"));
    button.setActionCommand("CircLine");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgCircPoint);
    button.setToolTipText(res1.getString("TT_CIRC_POINT"));
    button.setActionCommand("CircPoint");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgEditGrid);
    button.setToolTipText(res1.getString("TT_EDIT_GRID"));
    button.setActionCommand("EditGrid");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);

    button = new HeavyButton(imgEraseGrid);
    button.setToolTipText(res1.getString("TT_ERASE_GRID"));
    button.setActionCommand("EraseGrid");
    button.addActionListener(VPort);
    button.setMargin(margin);
    buttonsBar.add(button);
  }


  //-----------------------------------------------------------//
  // Minimum size
  //-----------------------------------------------------------//
  public Dimension getMinimumSize()
  {
    return new Dimension(700, 100);
  }
  

  //-----------------------------------------------------------//
  // Background color
  //-----------------------------------------------------------//
  public void ChangeBackgroundColor()
  {
    Color newColor = JColorChooser.showDialog(this, res1.getString("DLG_CHANGECOLOR"),
                                              VPort.GetBackgroundColor());
    if (newColor != null)
      VPort.SetBackgroundColor(newColor);
  }


  //-----------------------------------------------------------//
  // Default popup
  //-----------------------------------------------------------//
  public PopupMenu getDefaultPopup()
  {
    PopupMenu popup = new PopupMenu(res1.getString("VIEW_POPUP"));

    MenuItem item = new MenuItem(res1.getString("MN_BACKGROUND"));
    item.setActionCommand("ChangeColor");
    item.addActionListener(this);
    popup.add(item);

    return popup;
  }


//=======================================================================//
//                             ActionListener                            //
//=======================================================================//
  public void actionPerformed(ActionEvent event)
  {
    String nameAction = event.getActionCommand();

    VPort.ResetState();

    // Backgroung color
    if (nameAction.equals("ChangeColor"))
      ChangeBackgroundColor();

  }

//=======================================================================//
//                             External Listeners                        //
//=======================================================================//
  public synchronized void removeMouseMotionListener(MouseMotionListener l)
  {
    super.removeMouseMotionListener(l);
    if (mouseMotionListeners != null && mouseMotionListeners.contains(l))
    {
      Vector v = (Vector) mouseMotionListeners.clone();
      v.removeElement(l);
      mouseMotionListeners = v;
    }
  }

//=======================================================================//
  public synchronized void addMouseMotionListener(MouseMotionListener l)
  {
    super.addMouseMotionListener(l);
    Vector v = mouseMotionListeners == null ? new Vector(2) : (Vector) mouseMotionListeners.clone();
    if (!v.contains(l))
    {
      v.addElement(l);
      mouseMotionListeners = v;
    }
  }

//=======================================================================//
  public void MouseDragged(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseDragged(event);
  }
  protected void fireMouseDragged(MouseEvent e)
  {
    if (mouseMotionListeners != null)
    {
      Vector listeners = mouseMotionListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseMotionListener) listeners.elementAt(i)).mouseDragged(e);
    }
  }

//=======================================================================//
  public void MouseMoved(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseMoved(event);
  }
  protected void fireMouseMoved(MouseEvent e)
  {
    if (mouseMotionListeners != null)
    {
      Vector listeners = mouseMotionListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseMotionListener) listeners.elementAt(i)).mouseMoved(e);
    }
  }

//=======================================================================//
  public synchronized void removeMouseListener(MouseListener l)
  {
    super.removeMouseListener(l);
    if (mouseListeners != null && mouseListeners.contains(l))
    {
      Vector v = (Vector) mouseListeners.clone();
      v.removeElement(l);
      mouseListeners = v;
    }
  }

//=======================================================================//
  public synchronized void addMouseListener(MouseListener l)
  {
    super.addMouseListener(l);
    Vector v = mouseListeners == null ? new Vector(2) : (Vector) mouseListeners.clone();
    if (!v.contains(l))
    {
      v.addElement(l);
      mouseListeners = v;
    }
  }

//=======================================================================//
  public void MouseClicked(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseClicked(event);
  }
  protected void fireMouseClicked(MouseEvent e)
  {
    if (mouseListeners != null)
    {
      Vector listeners = mouseListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseListener) listeners.elementAt(i)).mouseClicked(e);
    }
  }

//=======================================================================//
  public void MouseEntered(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseEntered(event);
  }
  protected void fireMouseEntered(MouseEvent e)
  {
    if (mouseListeners != null)
    {
      Vector listeners = mouseListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseListener) listeners.elementAt(i)).mouseEntered(e);
    }
  }

//=======================================================================//
  public void MouseExited(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseExited(event);
  }
  protected void fireMouseExited(MouseEvent e)
  {
    if (mouseListeners != null)
    {
      Vector listeners = mouseListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseListener) listeners.elementAt(i)).mouseExited(e);
    }
  }

//=======================================================================//
  public void MousePressed(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMousePressed(event);
  }
  protected void fireMousePressed(MouseEvent e)
  {
    if (mouseListeners != null)
    {
      Vector listeners = mouseListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseListener) listeners.elementAt(i)).mousePressed(e);
    }
  }

//=======================================================================//
  public void MouseReleased(MouseEvent e)
  {
    MouseEvent event = new MouseEvent(this, e.getID(), e.getWhen(),
                           e.getModifiers(), e.getX(), e.getY(),
                           e.getClickCount(), e.isPopupTrigger());
    if (VPort.hasWindow()) fireMouseReleased(event);
  }
  protected void fireMouseReleased(MouseEvent e)
  {
    if (mouseListeners != null)
    {
      Vector listeners = mouseListeners;
      int count = listeners.size();
      for (int i = 0; i < count; i++)
        ((MouseListener) listeners.elementAt(i)).mouseReleased(e);
    }
  }

//=======================================================================//
//                      KeyListener
//=======================================================================//
  public void keyTyped(KeyEvent e)
  {
  }

//=======================================================================//
  public void keyPressed(KeyEvent e)
  {
  }

//=======================================================================//
  public void keyReleased(KeyEvent e)
  {
    if (e.getKeyCode() == e.VK_ESCAPE) VPort.ResetState();
  }

//=======================================================================//
  public synchronized void removeKeyListener(KeyListener l)
  {
    if(keyListeners != null && keyListeners.contains(l))
    {
      Vector v = (Vector) keyListeners.clone();
      v.removeElement(l);
      keyListeners = v;
      super.removeKeyListener(l);
    }
  }

//=======================================================================//
  public synchronized void addKeyListener(KeyListener l)
  {
    Vector v = keyListeners == null ? new Vector(2) : (Vector) keyListeners.clone();
    if(!v.contains(l))
    {
      v.addElement(l);
      keyListeners = v;
      super.addKeyListener(l);
    }
  }
}


