
//Title:        JAD demo application
//Version:
//Copyright:    Copyright (c) 1999
//Author:       User Interface group
//Company:      MatraDatavision
//Description:  Example of using JAD architecture
package util;

import java.awt.*;


public class ToolbarLayout implements LayoutManager
{
//=======================================================================//
// Constants
//=======================================================================//
  // Alignment
  public static final int CENTER = 0;
  public static final int LEFT = 1;
  public static final int RIGHT = 2;
  public static final int TOP = 3;
  public static final int BOTTOM = 4;


//=======================================================================//
// Internal variables
//=======================================================================//
  private int xAlign;       // horizontal alignment of components
  private int yAlign;       // vertical alignment of components
  private int hgap;         // horizontal gap between components
  private int vgap;         // vertical gap between components


//=======================================================================//
// Constructors
//=======================================================================//
  public ToolbarLayout()
  {
    this(CENTER, CENTER, 1, 1);
  }

  public ToolbarLayout(int xAlign, int yAlign)
  {
    this(xAlign, yAlign, 1, 1);
  }

  public ToolbarLayout(int xAlign, int yAlign, int hgap, int vgap)
  {
    this.xAlign = xAlign;
    this.yAlign = yAlign;
    this.hgap = hgap;
    this.vgap = vgap;
  }

//=======================================================================//
// Setting/Getting parameters
//=======================================================================//
  public int getAlignmentX()
  {
    return xAlign;
  }

  public void setAlignmentX(int xAlign)
  {
    this.xAlign = xAlign;
  }

  public int getAlignmentY()
  {
    return yAlign;
  }

  public void setAlignmentY(int yAlign)
  {
    this.yAlign = yAlign;
  }

  public int getHgap()
  {
    return hgap;
  }

  public void setHgap(int hgap)
  {
    this.hgap = hgap;
  }

  public int getVgap()
  {
    return vgap;
  }

  public void setVgap(int vgap)
  {
    this.vgap = vgap;
  }

//=======================================================================//
// LayoutManager interface implementation
//=======================================================================//
  public void addLayoutComponent(String name, Component comp)
  {
  }

  public void removeLayoutComponent(Component comp)
  {
  }

  public Dimension preferredLayoutSize(Container parent)
  {
    synchronized (parent.getTreeLock())
    {
    	Dimension dim = new Dimension(0, 0);
    	Insets insets = parent.getInsets();
      int maxWidth = parent.getWidth() - (insets.left + insets.right + hgap*2);
    	int compCount = parent.getComponentCount();
      int maxRowWidth = 0, maxRowHeight = 0;
      int x = 0;

    	for (int i = 0 ; i < compCount ; i++)
      {
        Component m = parent.getComponent(i);
  	    if (m.isVisible())
        {
      		Dimension d = m.getPreferredSize();
          if ((x == 0) || ((x + d.width) <= maxWidth))
          {
            if (x > 0) x += hgap;
            x += d.width;
            maxRowHeight = Math.max(maxRowHeight, d.height);
          }
          else
          {
            // It's time to start new line
            if (dim.height > 0) dim.height += vgap;
            dim.height += maxRowHeight;
            maxRowWidth = Math.max(maxRowWidth, x);

            x = d.width;
            maxRowHeight = d.height;
          }
	      }
  	  }
      // Process the last row
      if (dim.height > 0) dim.height += vgap;
      dim.height += maxRowHeight;
      maxRowWidth = Math.max(maxRowWidth, x);

      dim.width = maxRowWidth;

    	dim.width += insets.left + insets.right + hgap*2;
    	dim.height += insets.top + insets.bottom + vgap*2;

    	return dim;
    }
  }


  public Dimension minimumLayoutSize(Container parent)
  {
    synchronized (parent.getTreeLock())
    {
    	Dimension dim = new Dimension(0, 0);
    	Insets insets = parent.getInsets();
      int maxWidth = parent.getWidth() - (insets.left + insets.right + hgap*2);
    	int compCount = parent.getComponentCount();
      int maxRowWidth = 0, maxRowHeight = 0;
      int x = 0;

    	for (int i = 0 ; i < compCount ; i++)
      {
        Component m = parent.getComponent(i);
  	    if (m.isVisible())
        {
      		Dimension d = m.getMinimumSize();
          if ((x == 0) || ((x + d.width) <= maxWidth))
          {
            if (x > 0) x += hgap;
            x += d.width;
            maxRowHeight = Math.max(maxRowHeight, d.height);
          }
          else
          {
            // It's time to start new line
            if (dim.height > 0) dim.height += vgap;
            dim.height += maxRowHeight;
            maxRowWidth = Math.max(maxRowWidth, x);

            x = d.width;
            maxRowHeight = d.height;
          }
	      }
  	  }
      // Process the last row
      if (dim.height > 0) dim.height += vgap;
      dim.height += maxRowHeight;
      maxRowWidth = Math.max(maxRowWidth, x);

      dim.width = maxRowWidth;

    	dim.width += insets.left + insets.right + hgap*2;
    	dim.height += insets.top + insets.bottom + vgap*2;

    	return dim;
    }
/*
    synchronized (parent.getTreeLock())
    {
	    Dimension dim = new Dimension(0, 0);
    	int nmembers = parent.getComponentCount();

    	for (int i = 0 ; i < nmembers ; i++)
      {
	      Component m = parent.getComponent(i);
  	    if (m.isVisible())
        {
      		Dimension d = m.getMinimumSize();
      		dim.height = Math.max(dim.height, d.height);
      		if (i > 0) dim.width += hgap;
      		dim.width += d.width;
        }
      }
    	Insets insets = parent.getInsets();
    	dim.width += insets.left + insets.right + hgap*2;
    	dim.height += insets.top + insets.bottom + vgap*2;
    	return dim;
    }
*/
  }

  public void layoutContainer(Container parent)
  {
    synchronized (parent.getTreeLock())
    {
    	Insets insets = parent.getInsets();
    	int maxwidth = parent.getWidth() - (insets.left + insets.right + hgap*2);
    	int compCount = parent.getComponentCount();
    	int x = 0, y = insets.top + vgap;
    	int rowHeight = 0, start = 0;

      boolean ltr = parent.getComponentOrientation().isLeftToRight();

    	for (int i = 0; i < compCount; i++)
      {
	      Component m = parent.getComponent(i);
	      if (m.isVisible())
        {
      		Dimension d = m.getPreferredSize();
      		m.setSize(d.width, d.height);

      		if ((x == 0) || ((x + d.width) <= maxwidth))
          {
    		    if (x > 0) x += hgap;
    		    x += d.width;
		        rowHeight = Math.max(rowHeight, d.height);
      		}
          else
          {
    		    layoutRow(parent, insets.left + hgap, y, maxwidth - x,
                      rowHeight, start, i, ltr);
		        x = d.width;
		        y += vgap + rowHeight;
		        rowHeight = d.height;
		        start = i;
		      }
	      }
	    }
	    layoutRow(parent, insets.left + hgap, y, maxwidth - x,
                rowHeight, start, compCount, ltr);
    }
  }

  
  private void layoutRow(Container parent,
                         int x, int y, int freeWidth, int rowHeight,
                         int rowStart, int rowEnd, boolean ltr)
  {
    synchronized (parent.getTreeLock())
    {
    	switch (xAlign)
      {
      	case LEFT:
	        x += ltr ? 0 : freeWidth;
    	    break;
      	case CENTER:
    	    x += freeWidth/2;
    	    break;
      	case RIGHT:
    	    x += ltr ? freeWidth : 0;
    	    break;
    	}
    	for (int i = rowStart; i < rowEnd; i++)
      {
	      Component m = parent.getComponent(i);
	      if (m.isVisible())
        {
          int y2 = y;
          switch (yAlign)
          {
            case TOP:
              y2 += 0;
              break;
            case CENTER:
              y2 += (rowHeight - m.getHeight())/2;
              break;
            case BOTTOM:
              y2 += rowHeight - m.getHeight();
              break;
          }
	        if (ltr)
       	    m.setLocation(x, y2);
	        else
            m.setLocation(parent.getWidth() - x - m.getWidth(), y2);
          x += m.getWidth() + hgap;
  	    }
    	}
    }
  }


}



