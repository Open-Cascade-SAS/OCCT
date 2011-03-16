
//Title:        OpenCASCADE Samples
//Version:      
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      Matra Datavision
//Description:
package util;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;
import java.awt.datatransfer.*;


public class TracePanel extends JPanel implements ActionListener

{
  //-----------------------------------------------------------//
  // GUI components
  //-----------------------------------------------------------//
  JLabel titleLbl = new JLabel();
  JEditorPane traceTxt = new JEditorPane();
  static private Clipboard clip = Toolkit.getDefaultToolkit().getSystemClipboard();

  TextArea copyArea = new TextArea("",1,1,TextArea.SCROLLBARS_NONE);

//=======================================================================//
// Construction
//=======================================================================//
  public TracePanel()
  {
    try
    {
      jbInit();
    }
    catch(Exception ex)
    {
      ex.printStackTrace();
    }
  }

  void jbInit() throws Exception
  {
    setLayout(new BorderLayout());

    titleLbl.setBorder(BorderFactory.createEmptyBorder(10, 10, 0, 10));
    add(titleLbl, BorderLayout.NORTH);

    JPanel textPanel = new JPanel() {
      public Dimension getPreferredSize() {
        return new Dimension(300, super.getPreferredSize().height);
        }
      };
    textPanel.setBorder(BorderFactory.createCompoundBorder(
                        BorderFactory.createEmptyBorder(10, 10, 10, 10),
                        BorderFactory.createLoweredBevelBorder()));
    textPanel.setLayout(new GridBagLayout());

    traceTxt.setEditable(false);
    traceTxt.setAutoscrolls(false);
    JScrollPane scrollPane = new JScrollPane(traceTxt);

    textPanel.add(scrollPane, new GridBagConstraints(0, 0, 1, 1, 1.0, 1.0,
            GridBagConstraints.CENTER, GridBagConstraints.BOTH,
            new Insets(0, 0, 0, 0), 0, 0));
    add(textPanel, BorderLayout.CENTER);

    JPanel controlPanel = new JPanel();
    controlPanel.setLayout(new GridBagLayout());

    JButton copySel = new JButton("Copy Selection To Clipboard");
    copySel.addActionListener(this);
    copySel.setActionCommand("CopySelection");
    controlPanel.add(copySel, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 10, 5, 10), 0, 0));

    JButton copyAll = new JButton("Copy All To Clipboard");
    copyAll.addActionListener(this);
    copyAll.setActionCommand("CopyAll");
    controlPanel.add(copyAll, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0,
            GridBagConstraints.CENTER, GridBagConstraints.HORIZONTAL,
            new Insets(5, 10, 5, 10), 0, 0));

    copyArea.setVisible(false);
    copyArea.setEnabled(false);
    controlPanel.add(copyArea);

    add(controlPanel, BorderLayout.SOUTH);
  }


  public Dimension getMinimumSize()
  {
    return new Dimension(400, super.getMinimumSize().height);
  }



//=======================================================================//
// Commands
//=======================================================================//
  private void copySelection()
  {
    String text = traceTxt.getSelectedText();
    if (text == null)
      return;
    StringSelection clipString = new StringSelection(text);
    clip.setContents(clipString, clipString);

    // Add selected text to the copyArea
    copyArea.setText(text);
    copyArea.selectAll();
  }

  private void copyAll()
  {
    String text = traceTxt.getText();
    if (text == null)
      return;
    StringSelection clipString = new StringSelection(text);
    clip.setContents(clipString, clipString);
              
    // Add selected text to the copyArea
    copyArea.setText(text);
    copyArea.selectAll();
  }

  public void setText(String text)
  {
    traceTxt.setText(text);
    traceTxt.setCaretPosition(0);
    validate();
  }

  public void setTitle(String text)
  {
    titleLbl.setText(text);
    validate();
  }


//=======================================================================//
// Action listener interface
//=======================================================================//
  public void actionPerformed(ActionEvent e)
  {
    String nameAction = e.getActionCommand();
    if (nameAction.equals("CopySelection"))
      copySelection();
    else if (nameAction.equals("CopyAll"))
      copyAll();
  }

}
 
