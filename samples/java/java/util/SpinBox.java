
//Title:        Geological editor
//Version:
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import javax.swing.*;



public abstract class SpinBox extends JPanel implements ActionListener,
                                                 KeyListener,
                                                 FocusListener
{
  protected JButton btnUp = new JButton();
  protected JButton btnDown = new JButton();
  protected JTextField txtFld = new JTextField(1);
  JPanel pnlBtn = new JPanel(new GridLayout(2, 1));

  static Icon imgUp = getUpIcon();
  static Icon imgDown = getDownIcon();


//*********************************************************************
  public SpinBox()
  {
    super();
    try
    {
      jbInit();
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
    txtFld.setCaretPosition(0);
  }

//*********************************************************************
  private void jbInit() throws Exception
  {
    this.setLayout(new BorderLayout());
    this.add(txtFld, BorderLayout.CENTER);

    Insets aIns = new Insets(1, 1, 1, 1);
    btnUp.setMargin(aIns);
    btnDown.setMargin(aIns);

    btnUp.setIcon(imgUp);
    btnDown.setIcon(imgDown);
    btnUp.setFocusPainted(false);
    btnDown.setFocusPainted(false);

    btnUp.addActionListener(this);
    btnDown.addActionListener(this);

    pnlBtn.add(btnUp);
    pnlBtn.add(btnDown);
    this.add(pnlBtn, BorderLayout.EAST);

    txtFld.addKeyListener(this);
    txtFld.addFocusListener(this);
  }

//*********************************************************************
  private static Icon getUpIcon()
  {
    BufferedImage aImg = new BufferedImage(8, 4, BufferedImage.TYPE_INT_RGB);
    Graphics2D aGr = aImg.createGraphics();

    aGr.setColor(SystemColor.menu);
    aGr.fillRect(0, 0, 8, 4);
    int aX[] = {0, 8, 4};
    int aY[] = {4, 4, 0};
    aGr.setColor(Color.black);
    aGr.fillPolygon(aX, aY, 3);
    ImageIcon aNew = new ImageIcon(aImg);
    return aNew;
  }

//*********************************************************************
  private static Icon getDownIcon()
  {
    BufferedImage aImg = new BufferedImage(8, 4, BufferedImage.TYPE_INT_RGB);
    Graphics2D aGr = aImg.createGraphics();

    aGr.setColor(SystemColor.menu);
    aGr.fillRect(0, 0, 8, 4);
    int aX[] = {1, 3, 7};
    int aY[] = {0, 3, 0};
    aGr.setColor(Color.black);
    aGr.fillPolygon(aX, aY, 3);
    ImageIcon aNew = new ImageIcon(aImg);
    return aNew;
  }

  protected abstract void IncreaseValue();
  protected abstract void DecreaseValue();

//*********************************************************************
  public void setColumns(int aColumns)
  {
    txtFld.setColumns(aColumns);
  }

//*********************************************************************
  public int getColumns()
  {
    return txtFld.getColumns();
  }

//*********************************************************************
  public void setEnabled(boolean aState)
  {
    super.setEnabled(aState);
    txtFld.setEnabled(aState);
    btnDown.setEnabled(aState);
    btnUp.setEnabled(aState);
  }

//*********************************************************************
  public void addFocusListener(FocusListener l)
  {
    super.addFocusListener(l);
    if (txtFld != null) txtFld.addFocusListener(l);
    if (btnUp != null) btnUp.addFocusListener(l);
    if (btnDown != null) btnDown.addFocusListener(l);
  }

//*********************************************************************
//                   ActionListener
//*********************************************************************
  public void actionPerformed(ActionEvent e)
  {
    if (e.getSource() instanceof JButton)
    {
      JButton aBtn = (JButton)e.getSource();
      if (aBtn == btnUp) IncreaseValue();
      else if (aBtn == btnDown) DecreaseValue();
    }
  }

//*********************************************************************
//                   KeyListener
//*********************************************************************
  public void keyPressed(KeyEvent e)
  {
  }

  public void keyReleased(KeyEvent e)
  {
  }

  public void keyTyped(KeyEvent e)
  {
  }

//*********************************************************************
//                   FocusListener
//*********************************************************************
  public void focusGained(FocusEvent e)
  {
  }

  public void focusLost(FocusEvent e)
  {
  }
}
