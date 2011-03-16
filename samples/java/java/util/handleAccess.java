
//Title:        Geological editor
//Version:      
//Copyright:    Copyright (c) 1998
//Author:       User Interface Group (Nizhny Novgorod)
//Company:      EQCC
//Description:  Prototype of BRGM project


package util;


public interface handleAccess
{
  public int getWinHandle(java.awt.Component c, java.awt.Graphics g);
  public int getWinDepth(java.awt.Component c, java.awt.Graphics g);
  public int getVisualId(java.awt.GraphicsConfiguration gc);
}
