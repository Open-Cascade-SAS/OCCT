
//Title:      Setup program
//Version:    
//Copyright:  Copyright (c) 1999
//Author:     User Interface group
//Company:    MatraDatavision
//Description:Sets graphics configuration.

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import java.io.*;
import util.*;
import CASCADESamplesJni.*;
import SampleAISBasicJni.*;
import jcas.Standard_CString;

public class SetupFrame extends JFrame implements ActionListener,
                                                  ChangeListener
{
  JPanel viewPanel = new JPanel(new BorderLayout());
  JPanel radioPanel = new JPanel();
  JRadioButton[] myButtons;

  JPanel controlPanel = new JPanel(new FlowLayout());
  JButton btnOK = new JButton("OK");
  JButton btnCancel = new JButton("Cancel");

  Hashtable confs = new Hashtable(2, 1);
  int defConf = -1;
  int osType = -1;

  int curValue = -1;
  TestFrame frame;

  //Construct the frame
  public SetupFrame()
  {
    enableEvents(AWTEvent.WINDOW_EVENT_MASK);
    try
    {
      jbInit();
    }
    catch(Exception e)
    {
      e.printStackTrace();
    }
  }

  //Component initialization
  private void jbInit() throws Exception
  {
    this.getContentPane().setLayout(new BorderLayout());
    this.setSize(new Dimension(400, 300));
    this.setResizable(false);
    this.setTitle("Setup");

    // View panel
    getConfigurations();    // Gets current set of configurations
    createRadioPanel();

    if ((myButtons != null) && (myButtons.length > 1))
    {
      JLabel lbl = new JLabel("Choose configuration:");
      lbl.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
      viewPanel.add(lbl, BorderLayout.NORTH);

      radioPanel.setBorder(BorderFactory.createEmptyBorder(0, 10, 0, 10));
      viewPanel.add(radioPanel, BorderLayout.CENTER);

      lbl = new JLabel("If you see two rectangles press OK button...");
      lbl.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
      viewPanel.add(lbl, BorderLayout.SOUTH);
    }
    else
    {
      JLabel lbl = new JLabel("Configuration is setted");
      lbl.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
      viewPanel.add(lbl, BorderLayout.CENTER);
    }

    // Control panel
    btnOK.setActionCommand("OK");
    btnOK.addActionListener(this);
    controlPanel.add(btnOK);

    btnCancel.setActionCommand("Cancel");
    btnCancel.addActionListener(this);
    controlPanel.add(btnCancel);

    // Composition
    viewPanel.setBorder(BorderFactory.createLoweredBevelBorder());
    this.getContentPane().add(controlPanel, BorderLayout.SOUTH);
    this.getContentPane().add(viewPanel, BorderLayout.CENTER);
    pack();
  }

  private void getConfigurations()
  {
    // Get OS type
 	  String os = System.getProperty("os.name");
 	  if (os.startsWith("Wind"))
	    osType = 1;
 	  else
	    osType = 0;

    // Get native data access
    handleAccess win_access = null;
 	  try
 	  {
      if (osType == 0)
   	    win_access = (handleAccess)
          Class.forName("util.x11.X11HandleAccess").newInstance();
      else
   	    win_access = (handleAccess)
          Class.forName("util.win32.WinHandleAccess").newInstance();
    }
    catch (Exception e)
 	  {
 	    System.out.println(e);
 	  }

    // Get default configuration visual
    GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
    GraphicsDevice gd0 = ge.getDefaultScreenDevice();
    GraphicsConfiguration gc0 = gd0.getDefaultConfiguration();
    defConf = win_access.getVisualId(gc0);

    // Get all configurations
//    System.out.println("\nGraphicsConfiguration:");
//    System.out.println("\ngraphics environment = " + ge);
//    System.out.println("\ndefault graphics configuration = " + gc0);

    if (osType == 0)  // x11
    {
      GraphicsDevice gd[] = ge.getScreenDevices();
//      System.out.println("count of graphics devices = " + gd.length);

      for (int i = 0; i < gd.length; i++)
      {
//      	System.out.println("graphics device [" + i + "] = " + gd[i]);

      	GraphicsConfiguration gc[] = gd[i].getConfigurations();
//      	System.out.println("count of graphics configurations = " + gc.length);

      	for (int j = 0; j < gc.length; j++)
      	{
//     	    System.out.println("\n\tgraphics configuration [" + j + "] = " + gc[j]);
//     	    System.out.println("\tvisual ID = " + win_access.getVisualId(gc[j]));

          confs.put(new Integer(win_access.getVisualId(gc[j])), gc[j]);
/*
        	java.awt.image.ColorModel cm = gc[j].getColorModel();
        	System.out.println("\tColor model = " + cm);

        	java.awt.color.ColorSpace cs = cm.getColorSpace();
        	System.out.println("\tColor space = " + cs);
        	System.out.println("\tColor space type = " + cs.getType());
        	System.out.println("\tColor space is CS_sRGB = " + cs.isCS_sRGB());
        	System.out.println("\tNumber of components = " + cs.getNumComponents());
*/
      	}
      }
    }
  }

  private void createRadioPanel()
  {
    curValue = defConf;

    if (confs.size() > 0)
    {
      radioPanel.setLayout(new GridLayout(0, 4, 10, 5));

      // Get list of visualID values
      //-------------------------------------------
      int values[] = new int[confs.size()];
      Enumeration keys = confs.keys();
      int i = 0;
      while (keys.hasMoreElements())
      {
        Integer k = (Integer) keys.nextElement();
        values[i] = k.intValue();
        i++;
      }

      // Sort the array
      //-------------------------------------------
      for (i = 0; i < values.length; i++)
      {
        int min = i;
        for (int j = i + 1; j < values.length; j++)
        {
          if (values[j] < values[min])
            min = j;
        }
        if (min > i)
        {
          int tmp = values[i];
          values[i] = values[min];
          values[min] = tmp;
        }
      }

      // Create buttons
      //-------------------------------------------
      myButtons = new JRadioButton[values.length];
      ButtonGroup group = new ButtonGroup();
      for (i = 0; i < values.length; i++)
      {
        Integer value = new Integer(values[i]);
        myButtons[i] = new JRadioButton(value.toString());
        myButtons[i].setActionCommand("changeConfig");
        myButtons[i].addActionListener(this);
        if (values[i] == curValue)
          myButtons[i].setSelected(true);
        else
          myButtons[i].setSelected(false);
        group.add(myButtons[i]);
        radioPanel.add(myButtons[i]);
      }
    }
  }

  private void showTestFrame()
  {
    GraphicsConfiguration gr = (GraphicsConfiguration) confs.get(new Integer(curValue));
    Point loc = this.getLocation();
    loc.translate(this.getWidth(), 0);
    Dimension size = new Dimension(300, 400);
    if (frame != null)
    {
      loc = frame.getLocation();
      size = frame.getSize();
      frame.dispose();
    }

    frame = new TestFrame(gr);
    frame.setLocation(loc);
    frame.setSize(size);
    frame.setVisible(true);
  }

  // Overridden so we can exit on System Close
  //------------------------------------------------
  protected void processWindowEvent(WindowEvent event)
  {
    super.processWindowEvent(event);
    if(event.getID() == WindowEvent.WINDOW_CLOSING)
    {
      System.exit(0);
    }
    else if (event.getID() == WindowEvent.WINDOW_OPENED)
    {
      showTestFrame();
    }
  }

  // ActionListener interface
  //------------------------------------------------
  public void actionPerformed(ActionEvent event)
  {
    if (event.getActionCommand().equals("changeConfig"))
    {
      JRadioButton src = (JRadioButton) event.getSource();
      Integer value = new Integer(src.getText());
      curValue = value.intValue();
      showTestFrame();
      return;
    }
    else if (event.getActionCommand().equals("OK"))
    {
      System.out.println("Setup graphics configuration as " + curValue);

      // Get application property file name
      String fileName = System.getProperty("user.dir");
      String separator = System.getProperty("file.separator");
      if (!fileName.endsWith(separator))
        fileName = fileName + separator;
      fileName = fileName + "properties" + separator + "AppRes.properties";
      System.out.println("Application property file is " + fileName);

      // Read the properties
      Vector propKey = new Vector(2);
      Vector propValue = new Vector(2);
      try
      {
        FileReader fileRead = new FileReader(fileName);
        LineNumberReader lnRead = new LineNumberReader(fileRead);

        while (true)
        {
          String line = lnRead.readLine();
          if (line == null)
            break;
          if (line.startsWith("#"))
            continue;
          int k = line.indexOf("=");
          if (k > 0)
          {
            propKey.add(line.substring(0, k));
            propValue.add(line.substring(k+1));
          }
        }
        lnRead.close();
        fileRead.close();
      }
      catch (FileNotFoundException fe)
      {
      }
      catch (IOException ie)
      {
        ie.printStackTrace();
      }

      // set value of property "VisualID"
      Integer newID = new Integer(curValue);
      if (propKey.contains("VisualID"))
      {
        propValue.setElementAt(newID.toString(), propKey.indexOf("VisualID"));
      }
      else
      {
        propKey.add("VisualID");
        propValue.add(newID.toString());
      }

      // Write new file
      try
      {
        FileWriter fileWr = new FileWriter(fileName, false);
        for (int i = 0; i < propKey.size(); i++)
        {
          fileWr.write((String) propKey.get(i));
          fileWr.write("=");
          fileWr.write((String) propValue.get(i));
          fileWr.write("\n");
        }
        fileWr.close();
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }
    }
    System.exit(0);
  }

  // ChangeListener interface
  //------------------------------------------------
  public void stateChanged(ChangeEvent event)
  {
    System.out.println("SetupFrame change event " + event);
  }

}

class TestFrame extends Frame implements WindowListener,
                                         ComponentListener
{
  CasCadeCanvas myCanvas;
  V3d_Viewer myViewer3d;
  V3d_View myView3d;

  static int cntViews = 0;

  public TestFrame(GraphicsConfiguration gr)
  {
    super();

    cntViews++;
    String title = new String("Test view ");
    title = title + cntViews;
    this.setTitle(title);

    myViewer3d = SampleAISBasicPackage.CreateViewer(title);
    myViewer3d.Init();
    myView3d = myViewer3d.CreateView();

    myCanvas = new CasCadeCanvas(gr, myView3d);
    add(myCanvas);

    this.addWindowListener(this);
    this.addComponentListener(this);
  }

  public void setWindow()
  {
    int windowHandle = myCanvas.getWinData();
    short hi, lo;
    lo = (short) windowHandle;
    hi = (short) (windowHandle >> 16);

    System.out.println("\nSet window by handle " + windowHandle + "\n");

    SampleAISBasicPackage.SetWindow(myView3d, hi, lo);
//    this.repaint();
  }

  public void windowOpened(WindowEvent e)
  {
    setWindow();
    repaint();
  }

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified(WindowEvent e)
  {
  }

  public void windowDeiconified(WindowEvent e)
  {
  }

  public void windowActivated(WindowEvent e)
  {
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  public void componentResized(ComponentEvent e)
  {
    repaint();
  }

  public void componentMoved(ComponentEvent e)
  {
    repaint();
  }

  public void componentShown(ComponentEvent e)
  {
  }

  public void componentHidden(ComponentEvent e)
  {
  }

}

class CasCadeCanvas extends Canvas
{
  V3d_View myView;

  public CasCadeCanvas(GraphicsConfiguration gr, V3d_View view)
  {
    super(gr);
    myView = view;
  }

  public void update(Graphics g)
  {
    paint(g);
  }

  public void paint(Graphics g)
  {
    myView.MustBeResized();
    myView.Redraw();

    Color currColor = g.getColor();
    g.setColor(Color.white);
    Dimension size = this.getSize();
    g.drawRect(10, 10, size.width-20, size.height-20);
    g.drawRect(20, 20, size.width-40, size.height-40);
    g.setColor(currColor);
  }

  public int getWinData()
  {
    Graphics g = this.getGraphics();
    int pData = 0;

    handleAccess win_access;
    try
    {
      String os = System.getProperty("os.name");
      if (os.startsWith("Wind"))
        win_access = (handleAccess) Class.forName("util.win32.WinHandleAccess").newInstance();
      else
        win_access = (handleAccess) Class.forName("util.x11.X11HandleAccess").newInstance();

      pData = win_access.getWinHandle(this, g);
    }
    catch (Exception e)
    {
      System.out.println(e);
    }
    return pData;
  }
}
