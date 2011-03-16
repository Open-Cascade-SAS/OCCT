
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      Matra Datavision
//Description:


import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.ResourceBundle;
import jcas.Standard_CString;
import jcas.Standard_Integer;
import jcas.Standard_Real;
import CASCADESamplesJni.*;
import SampleDisplayAnimationJni.*;
import util.*;

public class SampleDisplayAnimationPanel extends SamplePanel
                                 implements ActionListener,
                                            MouseListener,
                                            MouseMotionListener
{
  //-----------------------------------------------------------//
  // Resources
  //-----------------------------------------------------------//
  static protected ResourceBundle resGui =
         ResourceBundle.getBundle("SampleDisplayAnimation");

  static private ImageIcon imgRestart = new ImageIcon(resGui.getString("Icon-Restart"));
  static private ImageIcon imgStop = new ImageIcon(resGui.getString("Icon-Stop"));
  static private ImageIcon imgThread = new ImageIcon(resGui.getString("Icon-Thread"));
  static private ImageIcon imgDeviation = new ImageIcon(resGui.getString("Icon-Deviation"));


  //-----------------------------------------------------------//
  // Components
  //-----------------------------------------------------------//
  private ViewPanel myView3d;

  private static V3d_Viewer myViewer3d = null;

  private static AIS_InteractiveContext myAISContext = null;
  private static boolean isDataLoaded = false;
  private Timer myTimer;

  private int startX = 0, startY = 0;
  private boolean Dragged = false;


  //-----------------------------------------------------------//
  // External access
  //-----------------------------------------------------------//
  public static V3d_Viewer getViewer3d()
  {
    return myViewer3d;
  }

  public static AIS_InteractiveContext getAISContext()
  {
    return myAISContext;
  }

  public static void setDataLoaded(boolean b)
  {
    isDataLoaded = b;
  }



//=======================================================================//
//                             Constructor                               //
//=======================================================================//
  public SampleDisplayAnimationPanel()
  {
    super(false);

    // 3D Initialization
    //------------------------------------------
    myViewer3d.SetDefaultLights();
    myViewer3d.SetLightOn();

//   Moved to the createViewPanel() function

    if (myAISContext == null)
      myAISContext = new AIS_InteractiveContext(myViewer3d);

    String path = System.getProperty("user.dir") +
              System.getProperty("file.separator") +
              "data" + System.getProperty("file.separator");
    isDataLoaded = SampleDisplayAnimationPackage.LoadData(myAISContext,
         new Standard_CString(path));


    myTimer = new Timer(1, this);
    myTimer.setRepeats(true);
  }

//-----------------------------------------------------------------------//
  public JPanel createViewPanel()
  {
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new GridBagLayout());
    mainPanel.setBorder(BorderFactory.createEmptyBorder(0, 5, 5, 0));


    // Viewer 3D
    //------------------------------------------
    myViewer3d = SampleDisplayAnimationPackage.CreateViewer("DisplayAnimation");
    myAISContext = new AIS_InteractiveContext(myViewer3d);

    myView3d = new ViewPanel() {
      	public ViewCanvas createViewPort() {
      	  return new CASCADEView3d(SampleDisplayAnimationPanel.getViewer3d()) {
              public void setWindow3d(V3d_View view, int hiwin, int lowin) {
                // set the native window
                SampleDisplayAnimationPackage.SetWindow(view, hiwin, lowin);

                // loading BRep data...
                SamplesStarter.getFrame().setCursor(
                      Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
                String path = System.getProperty("user.dir") +
                      System.getProperty("file.separator") +
                      "data" + System.getProperty("file.separator");
                SampleDisplayAnimationPanel.setDataLoaded(
                      SampleDisplayAnimationPackage.LoadData(
                            SampleDisplayAnimationPanel.getAISContext(),
                            new Standard_CString(path)));
                SamplesStarter.getFrame().setCursor(
                      Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));

              }
          };
      	}
    };

    myView3d.addMouseListener(this);
    myView3d.addMouseMotionListener(this);


    // Layout
    //------------------------------------------
    mainPanel.add(myView3d, new GridBagConstraints(0, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));

    return mainPanel;
  }

//-----------------------------------------------------------------------//
  public Component createToolbar()
  {
    JToolBar tools = (JToolBar) super.createToolbar();

    Insets margin = new Insets(1, 1, 1, 1);
    JButton button;

    button = new HeavyButton(imgRestart);
    button.setToolTipText(resGui.getString("Help-Restart"));
    button.setActionCommand("Restart");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgStop);
    button.setToolTipText(resGui.getString("Help-Stop"));
    button.setActionCommand("Stop");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgThread);
    button.setToolTipText(resGui.getString("Help-Thread"));
    button.setActionCommand("Thread");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    button = new HeavyButton(imgDeviation);
    button.setToolTipText(resGui.getString("Help-Deviation"));
    button.setActionCommand("Deviation");
    button.addActionListener(this);
    button.setMargin(margin);
    tools.add(button);

    return tools;
  }


//=======================================================================//
//                              Actions                                  //
//=======================================================================//
  public void onRestart()
  {
    if (!isDataLoaded)
    {
      JOptionPane.showMessageDialog(this,
           "Shape(s) not found.\n" +
           "Check the data directory path!",
           "Warning!!!", JOptionPane.WARNING_MESSAGE);
      return;
    }
    myTimer.start();
  }

//=======================================================================//
  public void onStop()
  {
    myTimer.stop();
  }

//=======================================================================//
  public void onThread()
  {
    onStop();
    ThreadDlg aDlg = new ThreadDlg(SamplesStarter.getFrame(),
            SampleDisplayAnimationPackage.GetAngleIncrement());
    Position.centerWindow(aDlg);
    aDlg.show();

    if (aDlg.isOK())
      SampleDisplayAnimationPackage.SetAngleIncrement(aDlg.getValue());

    onRestart();
  }

//=======================================================================//
  public void onDeviation()
  {
    onStop();

    double value = SampleDisplayAnimationPackage.GetDeviationCoefficient();

    DeviationDlg aDlg = new DeviationDlg(SamplesStarter.getFrame(),
            (int) ((value-0.00003)/0.00003));

    Position.centerWindow(aDlg);
    aDlg.show();

    if (aDlg.isOK() && isDataLoaded)
    {
      int newValue = aDlg.getValue();
      SampleDisplayAnimationPackage.SetDeviationCoefficient(myAISContext,
              (newValue*0.00003)+0.00003);
    }

    onRestart();
  }

//=======================================================================//
  public void onTimer()
  {
    if (isDataLoaded)
      SampleDisplayAnimationPackage.ChangePosition(myAISContext);
  }


//=======================================================================//
  public void setVisible(boolean b)
  {
    if (!b) onStop();
    super.setVisible(b);
  }


//=======================================================================//
// ActionListener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    if (e.getSource().equals(myTimer))
      onTimer();
    else
    {
      String command = e.getActionCommand();

      if (command.equals("Restart")) onRestart();
      else if (command.equals("Stop")) onStop();
      else if (command.equals("Thread")) onThread();
      else if (command.equals("Deviation")) onDeviation();
    }
  }


//=======================================================================//
// MouseListener interface
//=======================================================================//
  public void mouseClicked(MouseEvent e)
  {
  }

//-----------------------------------------------------------------------//
  public void mousePressed(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e))
    {
      startX = e.getX();
      startY = e.getY();
      Object src = e.getSource();

      if (src == myView3d)
      {
        V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
        if (view3d != null)
          myAISContext.MoveTo(startX, startY, view3d);
      }
    }
    else if (SwingUtilities.isRightMouseButton(e))
    {
      if (e.getSource() == myView3d)
      {
      	PopupMenu defPopup = myView3d.getDefaultPopup();
        	myView3d.add(defPopup);
        	defPopup.show(myView3d, e.getX(), e.getY() + 30);
      }
    }
  }

//-----------------------------------------------------------------------//
  public void mouseReleased(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e))
    {
      Object src = e.getSource();

      if (Dragged)
      {
        if (src == myView3d)
        {
          V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
          if (view3d != null)
          {
            if (e.isShiftDown())
              myAISContext.ShiftSelect(startX, startY, e.getX(), e.getY(), view3d, true);
            else
              myAISContext.Select(startX, startY, e.getX(), e.getY(), view3d, true);
          }
        }
      }
      else
      {
        if (src == myView3d)
        {
          if (e.isShiftDown())
            myAISContext.ShiftSelect(true);
          else
            myAISContext.Select(true);
        }
      }
      Dragged = false;
    }
  }

//-----------------------------------------------------------------------//
  public void mouseEntered(MouseEvent e)
  {
  }

//-----------------------------------------------------------------------//
  public void mouseExited(MouseEvent e)
  {
  }

//=======================================================================//
// MouseMotionListener interface
//=======================================================================//
  public void mouseDragged(MouseEvent e)
  {
    if (SwingUtilities.isLeftMouseButton(e) &&
        e.getSource() == myView3d)
      Dragged = true;
  }

//-----------------------------------------------------------------------//
  public void mouseMoved(MouseEvent e)
  {
    Object src = e.getSource();

    if (src == myView3d)
    {
      V3d_View view3d = ((CASCADEView3d) myView3d.getViewPort()).getView();
      if (myAISContext != null && view3d != null)
        myAISContext.MoveTo(e.getX(), e.getY(), view3d);
    }
  }


}

