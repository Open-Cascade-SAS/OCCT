using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IE_WPF_WinForms
{
    public enum CurrentAction3d
    {
        CurAction3d_Nothing,
        CurAction3d_DynamicZooming,
        CurAction3d_WindowZooming,
        CurAction3d_DynamicPanning,
        CurAction3d_GlobalPanning,
        CurAction3d_DynamicRotation
    }
    public enum CurrentPressedKey
    {
        CurPressedKey_Nothing,
        CurPressedKey_Ctrl,
        CurPressedKey_Shift
    }
    public enum ModelFormat
    {
        BREP,
        STEP,
        IGES,
        VRML,
        STL,
        IMAGE
    }

    public enum DisplayMode
    {
        Wireframe,
        Shading
    }

    public class OCCViewer : System.Windows.Forms.Form
    {
        public event EventHandler ZoomingFinished;
        protected void RaiseZoomingFinished()
        {
            if ( ZoomingFinished != null )
            {
                ZoomingFinished( this, EventArgs.Empty );
            }
        }

        public event EventHandler AvaliabiltyOfOperationsChanged;
        protected void RaiseAvaliabiltyOfOperationsChanged()
        {
            if ( AvaliabiltyOfOperationsChanged != null )
            {
                AvaliabiltyOfOperationsChanged( this, EventArgs.Empty );
            }
        }

        public OCCTProxy View { get; private set; }
        public CurrentAction3d CurrentMode { get; private set; }
        private CurrentPressedKey CurrentPressedKey { get; set; }
        private bool IsRectVisible { get; set; }
        public bool DegenerateMode { get; private set; }

        public bool IsWireframeEnabled { get; private set; }
        public bool IsShadingEnabled { get; private set; }
        public bool IsTransparencyEnabled { get; private set; }
        public bool IsColorEnabled { get; private set; }
        public bool IsMaterialEnabled { get; private set; }
        public bool IsDeleteEnabled { get; private set; }

        private float myCurZoom;
        private int myXmin;
        private int myYmin;
        private int myXmax;
        private int myYmax;
        private int myRectDownX;
        private int myRectDownY;
        private int myButtonDownX;
        private int myButtonDownY;

        private ContextMenu Popup { get; set; }
        private MenuItem ContextWireframe;
        private MenuItem ContextShading;
        private MenuItem ContextColor;
        private MenuItem ContextMaterial;
        private MenuItem ContextDelete;
        private MenuItem ContextBackground;
        private MenuItem ContextTransparency;


        public OCCViewer()
        {
            InitializeComponent();

            View = new OCCTProxy();
            View.InitOCCTProxy();
            if ( !View.InitViewer( this.Handle ) )
            {
                MessageBox.Show( "Fatal Error during the graphic initialisation", "Error!" );
            }

            CurrentMode = CurrentAction3d.CurAction3d_Nothing;
            CurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
            IsRectVisible = false;
            DegenerateMode = true;
        }

        private void InitializeComponent()
        {
            ControlBox = false;
            TopLevel = false;

            this.ImeMode = System.Windows.Forms.ImeMode.NoControl;

            SizeChanged += new System.EventHandler( OnSizeChanged );
            Paint += new System.Windows.Forms.PaintEventHandler( OnPaint );

            MouseDown += new System.Windows.Forms.MouseEventHandler( OnMouseDown );
            MouseUp += new System.Windows.Forms.MouseEventHandler( OnMouseUp );
            MouseMove += new System.Windows.Forms.MouseEventHandler( OnMouseMove );

            Popup = new ContextMenu();
            ContextWireframe = new MenuItem();
            ContextShading = new MenuItem();
            ContextColor = new MenuItem();
            ContextMaterial = new MenuItem();
            ContextTransparency = new MenuItem();
            ContextDelete = new MenuItem();
            ContextBackground = new MenuItem();

            ContextWireframe.Text = "Wireframe";
            ContextShading.Text = "Shading";
            ContextColor.Text = "Color";
            ContextMaterial.Text = "Material";
            ContextTransparency.Text = "Transparency";
            ContextDelete.Text = "Delete";
            ContextBackground.Text = "Background";

            ContextWireframe.Click += new System.EventHandler( ContextWireframe_Click );
            ContextShading.Click += new System.EventHandler( ContextShading_Click );
            ContextColor.Click += new System.EventHandler( ContextColor_Click );
            ContextMaterial.Click += new System.EventHandler( ContextMaterial_Click );
            ContextTransparency.Click += new System.EventHandler( ContextTransparency_Click );
            ContextDelete.Click += new System.EventHandler( ContextDelete_Click );
            ContextBackground.Click += new System.EventHandler( ContextBackground_Click );

            Popup.MenuItems.AddRange( new MenuItem[] { ContextWireframe,
	                                                   ContextShading,
	                                                   ContextColor,
	                                                   ContextMaterial,
	                                                   ContextTransparency,
	                                                   ContextDelete,
                                                       ContextBackground } );
            Popup.Popup += new System.EventHandler( OnPopup );
        }

        private void OnPaint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            View.RedrawView();
            View.UpdateView();
        }

        private void OnSizeChanged(object sender, System.EventArgs e)
        {
            View.UpdateView();
        }

        public void ImportModel( ModelFormat theFormat )
        {
            int aFormat = 10;
            OpenFileDialog anOpenDialog = new OpenFileDialog();
            string aDataDir = Environment.GetEnvironmentVariable("CSF_OCCTDataPath");
            string aFilter = "";

            switch ( theFormat )
            {
                case ModelFormat.BREP:
                    anOpenDialog.InitialDirectory = (aDataDir + "\\occ");
                    aFormat = 0;
                    aFilter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
                    break;
                case ModelFormat.STEP:
                    anOpenDialog.InitialDirectory = (aDataDir + "\\step");
                    aFormat = 1;
                    aFilter = "STEP Files (*.stp *.step)|*.stp; *.step";
                    break;
                case ModelFormat.IGES:
                    anOpenDialog.InitialDirectory = (aDataDir + "\\iges");
                    aFormat = 2;
                    aFilter = "IGES Files (*.igs *.iges)|*.igs; *.iges";
                    break;
                default:
                    break;
            }

            anOpenDialog.Filter = aFilter + "|All files (*.*)|*.*";
            if (anOpenDialog.ShowDialog() == DialogResult.OK)
            {
                string aFileName = anOpenDialog.FileName;
                if (aFileName == "")
                {
                    return;
                }

                Cursor = System.Windows.Forms.Cursors.WaitCursor;
                if ( !View.TranslateModel( aFileName, aFormat, true ) )
                {
                    MessageBox.Show( "Cann't read this file", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Warning );
                }
                Cursor = System.Windows.Forms.Cursors.Default;
            }
            View.ZoomAllView();
        }

        public void ExportModel( ModelFormat theFormat )
        {
            int aFormat = 10;
            SaveFileDialog saveDialog = new SaveFileDialog();
            string aDataDir = Environment.GetEnvironmentVariable("CSF_OCCTDataPath");
            string aFilter = "";

            switch ( theFormat )
            {
                case ModelFormat.BREP:
                    saveDialog.InitialDirectory = ( aDataDir + "\\occ" );
                    aFormat = 0;
                    aFilter = "BREP Files (*.brep *.rle)|*.brep; *.rle";
                    break;
                case ModelFormat.STEP:
                    saveDialog.InitialDirectory = ( aDataDir + "\\step" );
                    aFormat = 1;
                    aFilter = "STEP Files (*.stp *.step)|*.step; *.stp";
                    break;
                case ModelFormat.IGES:
                    saveDialog.InitialDirectory = ( aDataDir + "\\iges" );
                    aFormat = 2;
                    aFilter = "IGES Files (*.igs *.iges)| *.iges; *.igs";
                    break;
                case ModelFormat.VRML:
                    saveDialog.InitialDirectory = ( aDataDir + "\\vrml" );
                    aFormat = 3;
                    aFilter = "VRML Files (*.vrml)|*.vrml";
                    break;
                case ModelFormat.STL:
                    saveDialog.InitialDirectory = ( aDataDir + "\\stl" );
                    aFormat = 4;
                    aFilter = "STL Files (*.stl)|*.stl";
                    break;
                case ModelFormat.IMAGE:
                    saveDialog.InitialDirectory = ( aDataDir + "\\images" );
                    aFormat = 5;
                    aFilter = "Images Files (*.bmp)|*.bmp";
                    break;
                default:
                    break;
            }

            saveDialog.Filter = aFilter;
            if ( saveDialog.ShowDialog() == DialogResult.OK )
            {
                string aFileName = saveDialog.FileName;
                if ( aFileName == "" )
                {
                    return;
                }

                Cursor = System.Windows.Forms.Cursors.WaitCursor;
                if ( !View.TranslateModel( aFileName, aFormat, false ) )
                {
                    MessageBox.Show( "Can not write this file", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Warning );
                }
                Cursor = System.Windows.Forms.Cursors.Default;
            }
        }

        public void FitAll()
        {
            View.ZoomAllView();
        }

        public void ZoomWindow()
        {
            CurrentMode = CurrentAction3d.CurAction3d_WindowZooming;
        }

        public void DynamicZooming()
        {
            CurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
        }

        public void DynamicPanning()
        {
            CurrentMode = CurrentAction3d.CurAction3d_DynamicPanning;
        }

        public void GlobalPanning()
        {
            myCurZoom = View.Scale();
            CurrentMode = CurrentAction3d.CurAction3d_GlobalPanning;
        }

        public void AxoView()
        {
            View.AxoView();
        }

        public void FrontView()
        {
            View.FrontView();
        }

        public void TopView()
        {
            View.TopView();
        }

        public void LeftView()
        {
            View.LeftView();
        }

        public void BackView()
        {
            View.BackView();
        }

        public void RightView()
        {
            View.RightView();
        }

        public void Reset()
        {
            View.Reset();
        }

        public void BottomView()
        {
            View.BottomView();
        }

        public void HiddenOff()
        {
            View.SetDegenerateModeOff();
            DegenerateMode = false;
        }

        public void HiddenOn()
        {
            View.SetDegenerateModeOn();
            DegenerateMode = true;
        }

        public void DynamicRotation()
        {
            CurrentMode = CurrentAction3d.CurAction3d_DynamicRotation;
        }

        public void SelectionChanged()
        {
            switch ( View.DisplayMode() )
            {
                case -1:
                    IsShadingEnabled = false;
                    IsWireframeEnabled = false;
                    break;
                case 0:
                    IsWireframeEnabled = false;
                    IsShadingEnabled = true;
                    IsTransparencyEnabled = false;
                    break;
                case 1:
                    IsWireframeEnabled = true;
                    IsShadingEnabled = false;
                    IsTransparencyEnabled = true;
                    break;
                case 10:
                    IsWireframeEnabled = true;
                    IsShadingEnabled = true;
                    IsTransparencyEnabled = true;
                    break;
                default:
                    break;
            }

            if ( View.IsObjectSelected() )
            {
                IsColorEnabled = true;
                IsMaterialEnabled = true;
                IsDeleteEnabled = true;
            }
            else
            {
                IsColorEnabled = false;
                IsMaterialEnabled = false;
                IsTransparencyEnabled = false;
                IsDeleteEnabled = false;
            }

            RaiseAvaliabiltyOfOperationsChanged();
        }

        public void ChangeColor( bool IsObjectColor )
        {
            int r, g, b;
            if ( IsObjectColor )
            {
                r = View.GetObjColR();
                g = View.GetObjColG();
                b = View.GetObjColB();
            }
            else
            {
                r = View.GetBGColR();
                g = View.GetBGColG();
                b = View.GetBGColB();
            }
            System.Windows.Forms.ColorDialog ColDlg = new System.Windows.Forms.ColorDialog();
            ColDlg.Color = System.Drawing.Color.FromArgb( r, g, b );
            if ( ColDlg.ShowDialog() == System.Windows.Forms.DialogResult.OK )
            {
                System.Drawing.Color c = ColDlg.Color;
                r = c.R;
                g = c.G;
                b = c.B;
                if ( IsObjectColor )
                {
                    View.SetColor( r, g, b );
                }
                else
                {
                    View.SetBackgroundColor( r, g, b );
                }
            }
            View.UpdateCurrentViewer();
        }

        public void Wireframe()
        {
            View.SetDisplayMode( (int)DisplayMode.Wireframe );
            View.UpdateCurrentViewer();

            SelectionChanged();
            RaiseZoomingFinished();
        }

        public void Shading()
        {
            View.SetDisplayMode( (int)DisplayMode.Shading );
            View.UpdateCurrentViewer();

            SelectionChanged();
            RaiseZoomingFinished();
        }

        public void Color()
        {
            ChangeColor( true );
        }

        public void Background()
        {
            ChangeColor( false );
        }

        public void Material()
        {
            MaterialDlg aDlg = new MaterialDlg( View );
            aDlg.ShowDialog();
        }

        public void Transparency()
        {
            TransparencyDialog dlg = new TransparencyDialog();
            dlg.View = View;
            dlg.ShowDialog( this );
        }

        public void Delete()
        {
            View.EraseObjects();
            SelectionChanged();
        }

        public void OnKeyDown( System.Windows.Input.Key theKey )
        {
            if ( theKey == System.Windows.Input.Key.LeftShift ||
                 theKey == System.Windows.Input.Key.RightShift )
            {
                CurrentPressedKey = CurrentPressedKey.CurPressedKey_Shift;
            }
            else if (theKey == System.Windows.Input.Key.LeftCtrl ||
                     theKey == System.Windows.Input.Key.RightCtrl )
            {
                CurrentPressedKey = CurrentPressedKey.CurPressedKey_Ctrl;
            }
        }

        public void OnKeyUp()
        {
            CurrentPressedKey = CurrentPressedKey.CurPressedKey_Nothing;
        }

        protected void MultiDragEvent( int x, int y, int theState )
        {
            if ( theState == -1 ) //mouse is down
            {
                myButtonDownX = x;
                myButtonDownY = y;
            }
            else if ( theState ==  1) //mouse is up
            {
                View.ShiftSelect( Math.Min( myButtonDownX, x ), Math.Min( myButtonDownY, y ),
                                  Math.Max( myButtonDownX, x ), Math.Max( myButtonDownY, y ) );
            }
        }

        protected void DragEvent( int x, int y, int theState )
        {
            if ( theState == -1 ) //mouse is down
            {
                myButtonDownX = x;
                myButtonDownY = y;
            }
            else if ( theState == 1 ) //mouse is up
            {
                View.Select( Math.Min( myButtonDownX, x ), Math.Min( myButtonDownY, y ),
                             Math.Max( myButtonDownX, x ), Math.Max( myButtonDownY, y ) );
            }
        }

        private void DrawRectangle( bool draw )
        {
            System.Drawing.Graphics gr = System.Drawing.Graphics.FromHwnd(Handle);
            System.Drawing.Pen p = null;
            if ( IsRectVisible || !draw )//erase the rect
            {
                int r = View.GetBGColR();
                int g = View.GetBGColG();
                int b = View.GetBGColB();
                p = new System.Drawing.Pen( System.Drawing.Color.FromArgb(r, g, b) );
                IsRectVisible = false;
                View.UpdateView();
            }
            else if ( draw )
            {
                p = new System.Drawing.Pen( System.Drawing.Color.White );
                IsRectVisible = true;
            }
            if ( p == null )
            {
                return;
            }
            int x = Math.Min( myXmin, myXmax );
            int y = Math.Min( myYmin, myYmax );
            gr.DrawRectangle( p, x, y, Math.Abs(myXmax - myXmin), Math.Abs(myYmax - myYmin) );
            myRectDownX = Math.Max( myXmin, myXmax );
            myRectDownY = Math.Max( myYmin, myYmax );
        }

        private void OnMouseDown( object sender, System.Windows.Forms.MouseEventArgs e )
        {
            if ( e.Button == MouseButtons.Left )
            {
                myXmin = e.X;
                myXmax = e.X;
                myYmin = e.Y;
                myYmax = e.Y;
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl )
                {
                    // start the dinamic zooming....
                    CurrentMode = CurrentAction3d.CurAction3d_DynamicZooming;
                }
                else
                {
                    switch ( CurrentMode )
                    {
                        case CurrentAction3d.CurAction3d_Nothing:
                            if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift )
                            {
                                MultiDragEvent( myXmax, myYmax, -1 );
                            }
                            else
                            {
                                DragEvent( myXmax, myYmax, -1 );
                            }
                            break;
                        case CurrentAction3d.CurAction3d_DynamicRotation:
                            if ( !DegenerateMode )
                            {
                                View.SetDegenerateModeOn();
                            }
                            View.StartRotation( e.X, e.Y );
                            break;
                        case CurrentAction3d.CurAction3d_WindowZooming:
                            Cursor = Cursors.Hand;
                            break;
                        default:
                            break;
                    }
                }
            }
            else if ( e.Button == MouseButtons.Right )
            {
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl )
                {
                    if ( !DegenerateMode )
                    {
                        View.SetDegenerateModeOn();
                    }
                    View.StartRotation( e.X, e.Y );
                }
                else
                {
                    Popup.Show( this, new System.Drawing.Point( e.X, e.Y ) );
                }
            }
        }

        private void OnMouseUp( object sender, System.Windows.Forms.MouseEventArgs e ) 
        {
            if ( e.Button == MouseButtons.Left )
            {
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl )
                {
                    return;
                }
                switch ( CurrentMode )
                {
                    case CurrentAction3d.CurAction3d_Nothing:
                        if ( e.X == myXmin && e.Y == myYmin )
                        {
                            myXmax = e.X;
                            myYmax = e.Y;
                            if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift )
                            {
                                View.ShiftSelect();
                            }
                            else
                            {
                                View.Select();
                            }
                        }
                        else
                        {
                            myXmax = e.X;
                            myYmax = e.Y;
                            DrawRectangle( false );
                            if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Shift )
                            {
                                MultiDragEvent( myXmax, myYmax, 1 );
                            }
                            else
                            {
                                DragEvent( myXmax, myYmax, 1 );
                            }
                        }
                        break;
                    case CurrentAction3d.CurAction3d_DynamicZooming:
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    case CurrentAction3d.CurAction3d_WindowZooming:
                        myXmax = e.X;
                        myYmax = e.Y;
                        DrawRectangle( false );
                        int ValZWMin = 1;
                        if ( Math.Abs(myXmax - myXmin) > ValZWMin && 
                             Math.Abs(myXmax - myYmax) > ValZWMin )
                        {
                            View.WindowFitAll( myXmin, myYmin, myXmax, myYmax );
                        }
                        Cursor = Cursors.Arrow;
                        RaiseZoomingFinished();
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    case CurrentAction3d.CurAction3d_DynamicPanning:
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    case CurrentAction3d.CurAction3d_GlobalPanning:
                        View.Place( e.X, e.Y, myCurZoom );
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        break;
                    case CurrentAction3d.CurAction3d_DynamicRotation:
                        CurrentMode = CurrentAction3d.CurAction3d_Nothing;
                        if ( !DegenerateMode )
                        {
                            View.SetDegenerateModeOff();
                        }
                        else
                        {
                            View.SetDegenerateModeOn();
                        }
                        break;
                    default:
                        break;
                }
            }
            else if ( e.Button == MouseButtons.Right )
            {
                if ( !DegenerateMode )
                {
                    View.SetDegenerateModeOff();
                }
                else
                {
                    View.SetDegenerateModeOn();
                }
            }

            SelectionChanged();
        }

        private void OnMouseMove( object sender, System.Windows.Forms.MouseEventArgs e )
        {
            if ( e.Button == MouseButtons.Left ) //left button is pressed
            {
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl )
                {
                    View.Zoom(myXmax, myYmax, e.X, e.Y);
                    myXmax = e.X;
                    myYmax = e.Y;
                }
                else
                {
                    switch ( CurrentMode )
                    {
                        case CurrentAction3d.CurAction3d_Nothing:
                            DrawRectangle( false );
                            myXmax = e.X;
                            myYmax = e.Y;
                            DrawRectangle( true );
                            break;
                        case CurrentAction3d.CurAction3d_DynamicZooming:
                            View.Zoom( myXmax, myYmax, e.X, e.Y );
                            myXmax = e.X;
                            myYmax = e.Y;
                            break;
                        case CurrentAction3d.CurAction3d_WindowZooming:
                            DrawRectangle( false );
                            myXmax = e.X;
                            myYmax = e.Y;
                            DrawRectangle( true );//add brush here
                            break;
                        case CurrentAction3d.CurAction3d_DynamicPanning:
                            View.Pan( e.X - myXmax, myYmax - e.Y );
                            myXmax = e.X;
                            myYmax = e.Y;
                            break;
                        case CurrentAction3d.CurAction3d_GlobalPanning:
                            break;
                        case CurrentAction3d.CurAction3d_DynamicRotation:
                            View.Rotation( e.X, e.Y );
                            View.RedrawView();
                            break;
                        default:
                            break;
                    }
                }
            }
            else if ( e.Button == MouseButtons.Middle ) //middle button is pressed
            {
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl )
                {
                    View.Pan( e.X - myXmax, myYmax - e.Y );
                    myXmax = e.X;
                    myYmax = e.Y;
                }
            }
            else if ( e.Button == MouseButtons.Right ) //right button is pressed
            {
                if ( CurrentPressedKey == CurrentPressedKey.CurPressedKey_Ctrl) 
                {
                    View.Rotation( e.X, e.Y );
                }
            }
            else // no buttons are pressed
            {
                myXmax = e.X;
                myYmax = e.Y;
                View.MoveTo( e.X, e.Y );
            }
        }

        private void OnPopup( object sender, System.EventArgs e )
        {
            ContextWireframe.Enabled = IsWireframeEnabled;
            ContextShading.Enabled = IsShadingEnabled;
            ContextColor.Enabled = IsColorEnabled;
            ContextMaterial.Enabled = IsMaterialEnabled;
            ContextDelete.Enabled = IsDeleteEnabled;
            ContextTransparency.Enabled = IsTransparencyEnabled;
            ContextBackground.Enabled = true;
        }

        private void ContextWireframe_Click( object sender, System.EventArgs e )
        {
            Wireframe();
        }

        private void ContextShading_Click( object sender, System.EventArgs e )
        {
            Shading();
        }

        private void ContextColor_Click( object sender, System.EventArgs e )
        {
            Color();
        }

        private void ContextMaterial_Click( object sender, System.EventArgs e )
        {
            Material();
        }

        private void ContextTransparency_Click( object sender, System.EventArgs e )
        {
            Transparency();
        }

        private void ContextDelete_Click( object sender, System.EventArgs e )
        {
            Delete();
        }

        private void ContextBackground_Click( object sender, System.EventArgs e )
        {
            Background();
        }
    }
}
