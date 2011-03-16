/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include <OpenGl_GraphicDriver.jxx>
#include <OSD_Localizer.hxx>

#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

#include <locale.h>



/************************************************************************/
/* Print Methods                                                        */
/************************************************************************/

void OpenGl_GraphicDriver::Export( const Standard_CString FileName,
                                   const Graphic3d_ExportFormat Format,
                                   const Graphic3d_SortType aSortType,
                                   const Standard_Integer W,
                                   const Standard_Integer H,
                                   const Graphic3d_CView& View,
                                   const Aspect_CLayer2d& Under,
                                   const Aspect_CLayer2d& Over,
                                   const Standard_Real Precision,
                                   const Standard_Address ProgressBarFunc,
                                   const Standard_Address ProgressObject )
{
#ifdef HAVE_GL2PS

  OSD_Localizer locate(LC_NUMERIC,"C");
 
  Standard_Integer mode = -1;
  Standard_Integer sortType = Graphic3d_ST_BSP_Tree;
  switch( Format )
  {
  case Graphic3d_EF_PostScript:
    mode = GL2PS_PS;
    break;
  case Graphic3d_EF_EnhPostScript:
    mode = GL2PS_EPS;
    break;
  case Graphic3d_EF_TEX:
    mode = GL2PS_TEX;
    break;
  case Graphic3d_EF_PDF:
    mode = GL2PS_PDF;
    break;  
  case Graphic3d_EF_SVG:
    mode = GL2PS_SVG;
    break;
  case Graphic3d_EF_PGF:
    mode = GL2PS_PGF;
    break;
  }

  switch( aSortType )
  {
  case Graphic3d_ST_Simple:
    sortType = GL2PS_SIMPLE_SORT;
    break;
  case Graphic3d_ST_BSP_Tree:
    sortType = GL2PS_BSP_SORT;
    break;
  }     

  GLint viewport[4];
  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = W;
  viewport[3] = H;

  GLint buffersize = 1024*1024;   

  while( buffersize>0 )
  {
    FILE *file = fopen(FileName,"wb");
    gl2psBeginPage( "", "", viewport, mode, sortType,
                    GL2PS_DRAW_BACKGROUND | GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT/* | GL2PS_SIMPLE_LINE_OFFSET*/,
                    GL_RGBA, 0, NULL,
                    0, 0, 0, buffersize, file, FileName);

    Redraw( View, Under, Over );

    GLint num = gl2psEndPage();

    fclose(file);

    if( num==GL2PS_OVERFLOW )
       buffersize *= 2;
     else
       break;
  }

  locate.Restore();

#endif

}
