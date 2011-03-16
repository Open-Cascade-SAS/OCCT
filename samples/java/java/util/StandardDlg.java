
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      DNN CC
//Description:  Prototipe of BRGM project

package util;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class StandardDlg extends JDialog implements ActionListener
{
  static private ResourceBundle res =
         ResourceBundle.getBundle("properties.Desktop", Locale.getDefault());

//  static private StandardDlg PreviousDlg = null;

  public JPanel ControlsPanel = new JPanel();

  public JButton OkBtn = new JButton(res.getString("BTN_OK"));
  public JButton CancelBtn = new JButton(res.getString("BTN_CANCEL"));
  public JButton HlpBtn = new JButton(res.getString("BTN_HELP"));

  final public String OkAction = "OkAction";
  final public String CancelAction = "CancelAction";
  final public String HelpAction = "HelpAction";

  
//**********************************************************************
  public StandardDlg(Frame frame, String title, boolean isOk,
                     boolean isCancel, boolean isHelp)
  {
    super(frame, title, false);
    CreateDlg(isOk, isCancel, isHelp);
  }

//**********************************************************************
  public StandardDlg(Frame frame, String title, boolean isOk,
                     boolean isCancel, boolean isHelp, boolean isModal)
  {
    super(frame, title, isModal);
    CreateDlg(isOk, isCancel, isHelp);
  }

//**********************************************************************
  public StandardDlg(Dialog frame, String title, boolean isOk,
                     boolean isCancel, boolean isHelp)
  {
    super(frame, title, false);
    CreateDlg(isOk, isCancel, isHelp);
  }

//**********************************************************************
  public StandardDlg(Dialog frame, String title, boolean isOk,
                     boolean isCancel, boolean isHelp, boolean isModal)
  {
    super(frame, title, isModal);
    CreateDlg(isOk, isCancel, isHelp);
  }

//**********************************************************************
  private void CreateDlg(boolean isOk, boolean isCancel, boolean isHelp)
  {
    InitDlg(isOk, isCancel, isHelp);
    pack();
//    if (PreviousDlg != null) PreviousDlg.dispose();
//    PreviousDlg = this;
  }

//**********************************************************************
  void InitDlg(boolean isOk, boolean isCancel, boolean isHelp)
  {
    getContentPane().setLayout(new BorderLayout());
    JPanel actionPane = new JPanel(new FlowLayout());
    JPanel helpPane = new JPanel(new FlowLayout());
    JPanel BtnPanel = new JPanel(new BorderLayout());

    if (isOk)
    {
      OkBtn.setActionCommand(OkAction);
      OkBtn.addActionListener(this);
      actionPane.add(OkBtn);
    }
    if (isCancel)
    {
      CancelBtn.setActionCommand(CancelAction);
      CancelBtn.addActionListener(this);
      actionPane.add(CancelBtn);
    }
    if (isHelp)
    {
      HlpBtn.setActionCommand(HelpAction);
      HlpBtn.addActionListener(this);
      helpPane.add(HlpBtn);
    }

    BtnPanel.add(actionPane, BorderLayout.WEST);
    BtnPanel.add(helpPane, BorderLayout.EAST);

    ControlsPanel.setBorder(BorderFactory.createLoweredBevelBorder());
    getContentPane().add(ControlsPanel, BorderLayout.CENTER);
    getContentPane().add(BtnPanel, BorderLayout.SOUTH);
  }

//**********************************************************************
  public void OkAction()
  {
  }

//**********************************************************************
  public void CancelAction()
  {
  }

//**********************************************************************
  public void HelpAction()
  {
  }

//**********************************************************************
  public void UpdateBox()
  {
  }

//**********************************************************************
//                  ActionListener
//**********************************************************************
  public void actionPerformed(ActionEvent e)
  {
    String strAction = e.getActionCommand();

    if (strAction.equals(OkAction)) OkAction();
    else if (strAction.equals(CancelAction)) CancelAction();
    else if (strAction.equals(HelpAction)) HelpAction();
  }



}

