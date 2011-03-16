
//Title:        OpenCASCADE Samples
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;


public class SamplesTopologyPanel extends JPanel
                            implements ChangeListener
{
  private Component myCurrentPage = null;
  private JTabbedPane myTabbedPane = null;

//=======================================================================//
// Constructor
//=======================================================================//
  public SamplesTopologyPanel()
  {
    setLayout(new BorderLayout());

    //-----------------------------------------------------------//
    // Create a tab pane
    //-----------------------------------------------------------//
    myTabbedPane = new JTabbedPane();
    myTabbedPane.setVisible(false);
    myTabbedPane.addChangeListener(this);
    add(myTabbedPane, BorderLayout.CENTER);

    // The Topology Primitives sample
    myTabbedPane.addTab("Topology Primitives", new SampleTopologyPrimitivesPanel());

    // The Topological Operations sample
    myTabbedPane.addTab("Topological Operations", new SampleTopologicalOperationsPanel());

    // The Topology Building sample
    myTabbedPane.addTab("Topology Building", new SampleTopologyBuildingPanel());

    // The Topology Analysis sample
    myTabbedPane.addTab("Topology Analysis", new SampleTopologyAnalysisPanel());

    // The Topology Transformations sample
    myTabbedPane.addTab("Topology Transformations", new SampleTopologyTransformationsPanel());

    // The Local Operations sample
    myTabbedPane.addTab("Local Operations", new SampleLocalOperationsPanel());

    // The Triangulation sample
    myTabbedPane.addTab("Triangulation", new SampleTriangulationPanel());
  
    myTabbedPane.setSelectedIndex(0);
  }

//-----------------------------------------------------------------------//
  public void setVisible(boolean b)
  {
    super.setVisible(b);
    myTabbedPane.setVisible(b);
    if (myCurrentPage != null)
      myCurrentPage.setVisible(b);
    if (b) this.validate();
  }


//=======================================================================//
//                          Change Listener                              //
//=======================================================================//
  public void stateChanged(ChangeEvent event)
  {
    SamplesStarter.put_info("");

    JTabbedPane tab = (JTabbedPane) event.getSource();
    int index = tab.getSelectedIndex();
    Component currentPage = tab.getComponentAt(index);

    if (myCurrentPage != null)
    {
      myCurrentPage.setVisible(false);
      myCurrentPage.setEnabled(false);
      currentPage.setVisible(true);
      currentPage.setEnabled(true);
    }
    else
    {
      currentPage.setVisible(false);
      currentPage.setEnabled(false);
    }
    myCurrentPage = currentPage;
  }

}
