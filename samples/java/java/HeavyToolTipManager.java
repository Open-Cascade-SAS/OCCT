/*
 * @(#)ToolTipManager.java	1.40 99/04/22
 *
 * Copyright 1997-1999 by Sun Microsystems, Inc.,
 * 901 San Antonio Road, Palo Alto, California, 94303, U.S.A.
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information
 * of Sun Microsystems, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Sun.
 */


import javax.swing.*;
import java.awt.event.*;
import java.applet.*;
import java.awt.*;

/**
 * Manages all the ToolTips in the system.
 *
 * @see JComponent#createToolTip
 * @version 1.40 04/22/99
 * @author Dave Moore
 * @author Rich Schiavi
 */
public class HeavyToolTipManager extends MouseAdapter
                                 implements MouseMotionListener
{
  Timer enterTimer, exitTimer, insideTimer;
  String toolTipText;
  Point  preferredLocation;
  JComponent insideComponent;
  MouseEvent mouseEvent;
  boolean showImmediately;
  Popup tipWindow;
  JToolTip tip;

  private Rectangle popupRect = null;
  private Rectangle popupFrameRect = null;

  boolean enabled = true;
  boolean mouseAboveToolTip = false;
  private boolean tipShowing = false;
  private long timerEnter = 0;

  private KeyStroke postTip,hideTip;
  private AbstractAction postTipAction, hideTipAction;

  private FocusListener focusChangeListener = null;

  protected boolean lightWeightPopupEnabled = true;
  protected boolean heavyWeightPopupEnabled = false;

  final static HeavyToolTipManager sharedInstance = new HeavyToolTipManager();
  /** Returns a shared HeavyToolTipManager instance. */
  public static HeavyToolTipManager sharedInstance()
  {
      return sharedInstance;
  }


//=======================================================================//
// Constructor
//=======================================================================//
  HeavyToolTipManager()
  {
    enterTimer = new Timer(750, new insideTimerAction());
    enterTimer.setRepeats(false);
    exitTimer = new Timer(500, new outsideTimerAction());
    exitTimer.setRepeats(false);
    insideTimer = new Timer(4000, new stillInsideTimerAction());
    insideTimer.setRepeats(false);

	  // create accessibility actions
  	postTip = KeyStroke.getKeyStroke(KeyEvent.VK_F1,Event.CTRL_MASK);
    postTipAction = new AbstractAction()
    {
	    public void actionPerformed(ActionEvent e)
      {
	      if (tipWindow != null) // showing we unshow
	        hideTipWindow();
  	    else
        {
	        hideTipWindow(); // be safe
  	      enterTimer.stop();
  	      exitTimer.stop();
  	      insideTimer.stop();
  	      insideComponent = (JComponent)e.getSource();
  	      if (insideComponent != null)
          {
        		toolTipText = insideComponent.getToolTipText();
        		preferredLocation = new Point(10,insideComponent.getHeight()+10);  // manual set
        		showTipWindow();
        		// put a focuschange listener on to bring the tip down
        		if (focusChangeListener == null)
        		  focusChangeListener = createFocusChangeListener();
		    		insideComponent.addFocusListener(focusChangeListener);
  	      }
  	    }
  	  }
  	};

  	hideTip = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE,0);
  	hideTipAction = new AbstractAction()
    {
  	  public void actionPerformed(ActionEvent e)
      {
  	    hideTipWindow();
  	    JComponent jc = (JComponent)e.getSource();
  	    jc.removeFocusListener(focusChangeListener);
  	    preferredLocation = null;
      }
  	  public boolean isEnabled()
      {
	      // Only enable when the tooltip is showing, otherwise
	      // we will get in the way of any UI actions.
	      return tipShowing;
	    }
	  };
  }

//=======================================================================//
// Properties
//=======================================================================//
  /** Enables or disables the tooltip. */
  public void setEnabled(boolean flag)
  {
    enabled = flag;
    if (!flag) hideTipWindow();
  }

  /** Returns true if this object is enabled. */
  public boolean isEnabled()
  {
    return enabled;
  }

  /** Specifies the initial delay value. */
  public void setInitialDelay(int microSeconds)
  {
    enterTimer.setInitialDelay(microSeconds);
  }

  /** Returns the initial delay value. */
  public int getInitialDelay()
  {
    return enterTimer.getInitialDelay();
  }

  /** Specifies the dismisal delay value. */
  public void setDismissDelay(int microSeconds)
  {
      insideTimer.setInitialDelay(microSeconds);
  }

  /** Returns the dismisal delay value. */
  public int getDismissDelay()
  {
    return insideTimer.getInitialDelay();
  }

  /** Specifies the time to delay before reshowing the tooltip. */
  public void setReshowDelay(int microSeconds)
  {
    exitTimer.setInitialDelay(microSeconds);
  }

  /** Returns the reshow delay value. */
  public int getReshowDelay()
  {
    return exitTimer.getInitialDelay();
  }

//=======================================================================//
// Action
//=======================================================================//
  void showTipWindow()
  {
    if (insideComponent == null || !insideComponent.isShowing())
      return;

    if (enabled)
    {
      Dimension size;
      Point screenLocation = insideComponent.getLocationOnScreen();
      Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
      Point location = new Point();

      // Just to be paranoid
      hideTipWindow();

      tip = insideComponent.createToolTip();
      tip.setTipText(toolTipText);
      size = tip.getPreferredSize();

      // support only heavy tooltips
      tipWindow = new WindowPopup((frameForComponent(insideComponent)),tip,size);

      tipWindow.addMouseListener(this);

      if (preferredLocation != null)
      {
        location.x = screenLocation.x + preferredLocation.x;
        location.y = screenLocation.y + preferredLocation.y;
      }
      else
      {
        location.x = screenLocation.x + mouseEvent.getX();
        location.y = screenLocation.y + mouseEvent.getY() + 20;

        if (location.x + size.width > screenSize.width)
          location.x -= size.width;
        if (location.y + size.height > screenSize.height)
          location.y -= (size.height + 20);
      }

	    tipWindow.show(insideComponent,location.x,location.y);
      insideTimer.start();
	    timerEnter = System.currentTimeMillis();
	    tipShowing = true;
    }
  }

  void hideTipWindow()
  {
    if (tipWindow != null)
    {
      tipWindow.removeMouseListener(this);
	    tipWindow.hide();
	    tipWindow = null;
	    tipShowing = false;
	    timerEnter = 0;
	    (tip.getUI()).uninstallUI(tip);
      tip = null;
      insideTimer.stop();
    }
  }

  /**
   * Register a component for tooltip management.
   * <p>This will register key bindings to show and hide the tooltip text
   * only if <code>component</code> has focus bindings. This is done
   * so that components that are not normally focus traversable, such
   * as JLabel, are not made focus traversable as a result of invoking
   * this method.
   */
  public void registerComponent(JComponent component)
  {
    component.removeMouseListener(this);
    component.addMouseListener(this);
  	if (shouldRegisterBindings(component))
    {
	    // register our accessibility keybindings for this component
	    // this will apply globally across L&F
	    // Post Tip: Ctrl+F1
	    // Unpost Tip: Esc and Ctrl+F1
	    InputMap inputMap = component.getInputMap(JComponent.WHEN_FOCUSED);
	    ActionMap actionMap = component.getActionMap();

	    if (inputMap != null && actionMap != null)
      {
    		inputMap.put(postTip, "postTip");
    		inputMap.put(hideTip, "hideTip");
    		actionMap.put("postTip", postTipAction);
    		actionMap.put("hideTip", hideTipAction);
	    }
    }
  }

  /** Remove a component from tooltip control. */
  public void unregisterComponent(JComponent component)
  {
    component.removeMouseListener(this);
  	if (shouldRegisterBindings(component))
    {
	    InputMap inputMap = component.getInputMap(JComponent.WHEN_FOCUSED);
	    ActionMap actionMap = component.getActionMap();

	    if (inputMap != null && actionMap != null)
      {
    		inputMap.remove(postTip);
    		inputMap.remove(hideTip);
    		actionMap.remove("postTip");
    		actionMap.remove("hideTip");
	    }
  	}
  }

  /**
   * Returns whether or not bindings should be registered on the given
   * Component. This is implemented to return true if the receiver has
   * a binding in any one of the InputMaps registered under the condition
   * <code>WHEN_FOCUSED</code>.
   * <p>
   * This does not use <code>isFocusTraversable</code> as
   * some components may override <code>isFocusTraversable</code> and
   * base the return value on something other than bindings. For example,
   * JButton bases its return value on its enabled state.
   */
  private boolean shouldRegisterBindings(JComponent component)
  {
  	InputMap inputMap = component.getInputMap(JComponent.WHEN_FOCUSED);
  	while (inputMap != null && inputMap.size() == 0)
    {
	    inputMap = inputMap.getParent();
  	}
	  return (inputMap != null);
  }

//=======================================================================//
// Subsidiary functions
//=======================================================================//
  static Frame frameForComponent(Component component)
  {
    while (!(component instanceof Frame))
      component = component.getParent();
    return (Frame)component;
  }

  private FocusListener createFocusChangeListener()
  {
    return new FocusAdapter()
    {
      public void focusLost(FocusEvent evt)
      {
      	hideTipWindow();
      	JComponent c = (JComponent)evt.getSource();
      	c.removeFocusListener(focusChangeListener);
      }
    };
  }


//=======================================================================//
// MouseListener interface
//=======================================================================//
  public void mouseEntered(MouseEvent event)
  {
    // this is here for a workaround for a Solaris *application* only bug
    // in which an extra MouseExit/Enter events are generated when a Panel
    // initially is shown
    if (tipShowing)
 	  {
	    if (System.currentTimeMillis() - timerEnter < 200)
    		return;
  	}

    if (event.getSource() == tipWindow)
      return;

    JComponent component = (JComponent)event.getSource();
    toolTipText = component.getToolTipText(event);
    preferredLocation = component.getToolTipLocation(event);

    exitTimer.stop();

  	Point location = event.getPoint();
  	// ensure tooltip shows only in proper place
  	if (location.x < 0 ||
	      location.x >=component.getWidth() ||
	      location.y < 0 ||
	      location.y >= component.getHeight())
	  {
	    return;
	  }

    if (insideComponent != null)
    {
      enterTimer.stop();
      insideComponent = null;
    }

    component.addMouseMotionListener(this);

    insideComponent = component;
  	if (tipWindow != null)
	    return;

  }

  public void mouseExited(MouseEvent event)
  {
    // this is here for a workaround for a Solaris *application* only bug
    //  when Panels are used
    if (tipShowing)
	  {
	    if (System.currentTimeMillis() - timerEnter < 200)
    		return;
  	}

    boolean shouldHide = true;

    if(event.getSource() == tipWindow)
    {
	    // if we get an exit and have a heavy window
  	  // we need to check if it if overlapping the inside component
      Container insideComponentWindow = insideComponent.getTopLevelAncestor();
      Rectangle b = tipWindow.getBounds();
      Point location = event.getPoint();
      location.x += b.x;
      location.y += b.y;

      b = insideComponentWindow.getBounds();
      location.x -= b.x;
      location.y -= b.y;

      location = SwingUtilities.convertPoint(null,location,insideComponent);
      if (location.x >= 0 && location.x < insideComponent.getWidth() &&
          location.y >= 0 && location.y < insideComponent.getHeight())
        shouldHide = false;
      else
	      shouldHide = true;
    }
    else if(event.getSource() == insideComponent && tipWindow != null)
    {
      Point location = SwingUtilities.convertPoint(insideComponent, event.getPoint(), null);
      Rectangle bounds = insideComponent.getTopLevelAncestor().getBounds();
      location.x += bounds.x;
      location.y += bounds.y;

      bounds = tipWindow.getBounds();
      if (location.x >= bounds.x && location.x < (bounds.x + bounds.width) &&
          location.y >= bounds.y && location.y < (bounds.y + bounds.height))
        shouldHide = false;
      else
        shouldHide = true;
    }

    if(shouldHide)
    {
      enterTimer.stop();
	    if (insideComponent != null)
	      insideComponent.removeMouseMotionListener(this);
      insideComponent = null;
      toolTipText = null;
      mouseEvent = null;
      hideTipWindow();
      exitTimer.start();
    }
  }

  public void mousePressed(MouseEvent event)
  {
    hideTipWindow();
    enterTimer.stop();
    showImmediately = false;
  }

//=======================================================================//
// MouseMotionListener interface
//=======================================================================//
  public void mouseDragged(MouseEvent event)
  {
  }

  public void mouseMoved(MouseEvent event)
  {
    JComponent component = (JComponent)event.getSource();
    String newText = component.getToolTipText(event);
    Point  newPreferredLocation = component.getToolTipLocation(event);

    if (newText != null || newPreferredLocation != null)
    {
      mouseEvent = event;
      if (((newText != null && newText.equals(toolTipText)) || newText == null) &&
          ((newPreferredLocation != null && newPreferredLocation.equals(preferredLocation))
          || newPreferredLocation == null))
      {
        if (tipWindow != null)
          insideTimer.restart();
        else
          enterTimer.restart();
      }
      else
      {
        toolTipText = newText;
        preferredLocation = newPreferredLocation;
        if (showImmediately)
        {
          hideTipWindow();
          showTipWindow();
        }
        else
          enterTimer.restart();
      }
    }
    else
    {
      toolTipText = null;
      preferredLocation = null;
      mouseEvent = null;
      hideTipWindow();
      enterTimer.stop();
      exitTimer.start();
    }
  }

//=======================================================================//
// Class insideTimerAction
//=======================================================================//
  protected class insideTimerAction implements ActionListener
  {
    public void actionPerformed(ActionEvent e)
    {
      if (insideComponent != null && insideComponent.isShowing())
      {
        showImmediately = true;
        showTipWindow();
      }
    }
  }

//=======================================================================//
// Class insideTimerAction
//=======================================================================//
  protected class outsideTimerAction implements ActionListener
  {
    public void actionPerformed(ActionEvent e)
    {
      showImmediately = false;
    }
  }

//=======================================================================//
// Class insideTimerAction
//=======================================================================//
  protected class stillInsideTimerAction implements ActionListener
  {
    public void actionPerformed(ActionEvent e)
    {
      hideTipWindow();
      enterTimer.stop();
      showImmediately = false;
    }
  }


//=======================================================================//
// Interface Popup
//=======================================================================//
  /*
   * The following interface describes what a popup should implement.
   * We do this because the ToolTip manager uses popup that can be windows or
   * panels. The reason is two-fold: We'd like to use panels mostly, but when the
   * panel (or tooltip) would not fit, we need to use a Window to avoid the panel
   * being clipped or not shown.
   *
   */
  private interface Popup
  {
    public void show(JComponent invoker, int x, int y);
    public void hide();
    public void addMouseListener(HeavyToolTipManager c);
    public void removeMouseListener(HeavyToolTipManager c);
    public Rectangle getBounds();
  }

//=======================================================================//
// Class WindowPopup
//=======================================================================//
  class WindowPopup extends Window implements Popup
  {
    boolean  firstShow = true;
    JComponent tip;
    Frame frame;

    public WindowPopup(Frame f,JComponent t, Dimension size)
    {
      super(f);
      this.tip = t;
      this.frame = f;
      add(t, BorderLayout.CENTER);
      pack();
      // setSize(size);
    }

    public Rectangle getBounds()
    {
    	return super.getBounds();
    }

    public void show(JComponent invoker, int x, int y)
    {
      this.setLocation(x,y);
      this.setVisible(true);

      /** This hack is to workaround a bug on Solaris where the windows does not really show
       *  the first time
       *  It causes a side effect of MS JVM reporting IllegalArumentException: null source
       *  fairly frequently - also happens if you use HeavyWeight JPopup, ie JComboBox
       */
      if(firstShow)
      {
      	this.hide();
      	this.setVisible(true);
      	firstShow = false;
      }
    }

    public void hide()
    {
      super.hide();
      /** We need to call removeNotify() here because hide() does something only if
       *  Component.visible is true. When the app frame is miniaturized, the parent
       *  frame of this frame is invisible, causing AWT to believe that this frame
       *  is invisible and causing hide() to do nothing
       */
      removeNotify();
    }

    public void addMouseListener(HeavyToolTipManager c)
    {
    	super.addMouseListener(c);
    }

    public void removeMouseListener(HeavyToolTipManager c)
    {
    	super.removeMouseListener(c);
    }

  }

}
