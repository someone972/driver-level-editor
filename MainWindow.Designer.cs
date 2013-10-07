namespace DriverLevelEditor
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.TopMenu = new System.Windows.Forms.MenuStrip();
            this.tsFile = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_Custom = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.tsFile_Open_Miami = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_Frisco = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_LA = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_NewYork = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_Newcastle = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_Desert = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_Open_CarPark = new System.Windows.Forms.ToolStripMenuItem();
            this.tsFile_SaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.tsFile_Exit = new System.Windows.Forms.ToolStripMenuItem();
            this.tsEdit = new System.Windows.Forms.ToolStripMenuItem();
            this.tsView = new System.Windows.Forms.ToolStripMenuItem();
            this.tsView_Models = new System.Windows.Forms.ToolStripMenuItem();
            this.tsView_Textures = new System.Windows.Forms.ToolStripMenuItem();
            this.tsView_TextureDefs = new System.Windows.Forms.ToolStripMenuItem();
            this.tsDriver = new System.Windows.Forms.ToolStripMenuItem();
            this.tsDriver_Launch = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.tsDriver_OpenRoot = new System.Windows.Forms.ToolStripMenuItem();
            this.tsDriver_Patch = new System.Windows.Forms.ToolStripMenuItem();
            this.tsSettings = new System.Windows.Forms.ToolStripMenuItem();
            this.tsHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.tsHelp_HelpTopics = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.tsHelp_About = new System.Windows.Forms.ToolStripMenuItem();
            this.WidgetPanel = new System.Windows.Forms.Panel();
            this.TopMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // TopMenu
            // 
            this.TopMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsFile,
            this.tsEdit,
            this.tsView,
            this.tsDriver,
            this.tsSettings,
            this.tsHelp});
            this.TopMenu.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.TopMenu.Location = new System.Drawing.Point(0, 0);
            this.TopMenu.Name = "TopMenu";
            this.TopMenu.Size = new System.Drawing.Size(664, 24);
            this.TopMenu.TabIndex = 0;
            // 
            // tsFile
            // 
            this.tsFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsFile_Open,
            this.tsFile_SaveAs,
            this.toolStripSeparator4,
            this.tsFile_Exit});
            this.tsFile.Name = "tsFile";
            this.tsFile.Size = new System.Drawing.Size(37, 20);
            this.tsFile.Text = "File";
            // 
            // tsFile_Open
            // 
            this.tsFile_Open.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsFile_Open_Custom,
            this.toolStripSeparator3,
            this.tsFile_Open_Miami,
            this.tsFile_Open_Frisco,
            this.tsFile_Open_LA,
            this.tsFile_Open_NewYork,
            this.tsFile_Open_Newcastle,
            this.tsFile_Open_Desert,
            this.tsFile_Open_CarPark});
            this.tsFile_Open.Name = "tsFile_Open";
            this.tsFile_Open.Size = new System.Drawing.Size(123, 22);
            this.tsFile_Open.Text = "Open";
            // 
            // tsFile_Open_Custom
            // 
            this.tsFile_Open_Custom.Name = "tsFile_Open_Custom";
            this.tsFile_Open_Custom.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_Custom.Text = "Choose Files...";
            this.tsFile_Open_Custom.Click += new System.EventHandler(this.LoadCustomFiles);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(146, 6);
            // 
            // tsFile_Open_Miami
            // 
            this.tsFile_Open_Miami.Name = "tsFile_Open_Miami";
            this.tsFile_Open_Miami.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_Miami.Text = "Miami";
            this.tsFile_Open_Miami.Click += new System.EventHandler(this.LoadMiamiFiles);
            // 
            // tsFile_Open_Frisco
            // 
            this.tsFile_Open_Frisco.Name = "tsFile_Open_Frisco";
            this.tsFile_Open_Frisco.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_Frisco.Text = "San Francisco";
            this.tsFile_Open_Frisco.Click += new System.EventHandler(this.LoadFriscoFiles);
            // 
            // tsFile_Open_LA
            // 
            this.tsFile_Open_LA.Name = "tsFile_Open_LA";
            this.tsFile_Open_LA.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_LA.Text = "Los Angeles";
            this.tsFile_Open_LA.Click += new System.EventHandler(this.LoadLosAngelesFiles);
            // 
            // tsFile_Open_NewYork
            // 
            this.tsFile_Open_NewYork.Name = "tsFile_Open_NewYork";
            this.tsFile_Open_NewYork.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_NewYork.Text = "New York";
            this.tsFile_Open_NewYork.Click += new System.EventHandler(this.LoadNewYorkFiles);
            // 
            // tsFile_Open_Newcastle
            // 
            this.tsFile_Open_Newcastle.Name = "tsFile_Open_Newcastle";
            this.tsFile_Open_Newcastle.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_Newcastle.Text = "Newcastle";
            this.tsFile_Open_Newcastle.Click += new System.EventHandler(this.LoadNewcastleFiles);
            // 
            // tsFile_Open_Desert
            // 
            this.tsFile_Open_Desert.Name = "tsFile_Open_Desert";
            this.tsFile_Open_Desert.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_Desert.Text = "Desert";
            this.tsFile_Open_Desert.Click += new System.EventHandler(this.LoadTrainFiles);
            // 
            // tsFile_Open_CarPark
            // 
            this.tsFile_Open_CarPark.Name = "tsFile_Open_CarPark";
            this.tsFile_Open_CarPark.Size = new System.Drawing.Size(149, 22);
            this.tsFile_Open_CarPark.Text = "Car Park";
            this.tsFile_Open_CarPark.Click += new System.EventHandler(this.LoadIViewFiles);
            // 
            // tsFile_SaveAs
            // 
            this.tsFile_SaveAs.Name = "tsFile_SaveAs";
            this.tsFile_SaveAs.Size = new System.Drawing.Size(123, 22);
            this.tsFile_SaveAs.Text = "Save As...";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(120, 6);
            // 
            // tsFile_Exit
            // 
            this.tsFile_Exit.Name = "tsFile_Exit";
            this.tsFile_Exit.Size = new System.Drawing.Size(123, 22);
            this.tsFile_Exit.Text = "Exit";
            // 
            // tsEdit
            // 
            this.tsEdit.Enabled = false;
            this.tsEdit.Name = "tsEdit";
            this.tsEdit.Size = new System.Drawing.Size(39, 20);
            this.tsEdit.Text = "Edit";
            // 
            // tsView
            // 
            this.tsView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsView_Models,
            this.tsView_Textures,
            this.tsView_TextureDefs});
            this.tsView.Name = "tsView";
            this.tsView.Size = new System.Drawing.Size(44, 20);
            this.tsView.Text = "View";
            // 
            // tsView_Models
            // 
            this.tsView_Models.Name = "tsView_Models";
            this.tsView_Models.Size = new System.Drawing.Size(173, 22);
            this.tsView_Models.Text = "Models";
            this.tsView_Models.Click += new System.EventHandler(this.ViewModels);
            // 
            // tsView_Textures
            // 
            this.tsView_Textures.Name = "tsView_Textures";
            this.tsView_Textures.Size = new System.Drawing.Size(173, 22);
            this.tsView_Textures.Text = "Textures";
            this.tsView_Textures.Click += new System.EventHandler(this.ViewTextures);
            // 
            // tsView_TextureDefs
            // 
            this.tsView_TextureDefs.Name = "tsView_TextureDefs";
            this.tsView_TextureDefs.Size = new System.Drawing.Size(173, 22);
            this.tsView_TextureDefs.Text = "Texture Definitions";
            // 
            // tsDriver
            // 
            this.tsDriver.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsDriver_Launch,
            this.toolStripSeparator2,
            this.tsDriver_OpenRoot,
            this.tsDriver_Patch});
            this.tsDriver.Name = "tsDriver";
            this.tsDriver.Size = new System.Drawing.Size(50, 20);
            this.tsDriver.Text = "Driver";
            // 
            // tsDriver_Launch
            // 
            this.tsDriver_Launch.Name = "tsDriver_Launch";
            this.tsDriver_Launch.Size = new System.Drawing.Size(182, 22);
            this.tsDriver_Launch.Text = "Run Game";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(179, 6);
            // 
            // tsDriver_OpenRoot
            // 
            this.tsDriver_OpenRoot.Name = "tsDriver_OpenRoot";
            this.tsDriver_OpenRoot.Size = new System.Drawing.Size(182, 22);
            this.tsDriver_OpenRoot.Text = "Open Root Directory";
            // 
            // tsDriver_Patch
            // 
            this.tsDriver_Patch.Name = "tsDriver_Patch";
            this.tsDriver_Patch.Size = new System.Drawing.Size(182, 22);
            this.tsDriver_Patch.Text = "Install Vertex Patch";
            // 
            // tsSettings
            // 
            this.tsSettings.Enabled = false;
            this.tsSettings.Name = "tsSettings";
            this.tsSettings.Size = new System.Drawing.Size(61, 20);
            this.tsSettings.Text = "Settings";
            // 
            // tsHelp
            // 
            this.tsHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsHelp_HelpTopics,
            this.toolStripSeparator1,
            this.tsHelp_About});
            this.tsHelp.Name = "tsHelp";
            this.tsHelp.Size = new System.Drawing.Size(44, 20);
            this.tsHelp.Text = "Help";
            // 
            // tsHelp_HelpTopics
            // 
            this.tsHelp_HelpTopics.Name = "tsHelp_HelpTopics";
            this.tsHelp_HelpTopics.Size = new System.Drawing.Size(137, 22);
            this.tsHelp_HelpTopics.Text = "Help Topics";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(134, 6);
            // 
            // tsHelp_About
            // 
            this.tsHelp_About.Name = "tsHelp_About";
            this.tsHelp_About.Size = new System.Drawing.Size(137, 22);
            this.tsHelp_About.Text = "About";
            // 
            // WidgetPanel
            // 
            this.WidgetPanel.BackColor = System.Drawing.SystemColors.Control;
            this.WidgetPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.WidgetPanel.Location = new System.Drawing.Point(0, 24);
            this.WidgetPanel.Name = "WidgetPanel";
            this.WidgetPanel.Size = new System.Drawing.Size(664, 528);
            this.WidgetPanel.TabIndex = 1;
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(664, 552);
            this.Controls.Add(this.WidgetPanel);
            this.Controls.Add(this.TopMenu);
            this.DoubleBuffered = true;
            this.MainMenuStrip = this.TopMenu;
            this.MaximizeBox = false;
            this.MinimumSize = new System.Drawing.Size(680, 590);
            this.Name = "MainWindow";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Driver Level Editor";
            this.TopMenu.ResumeLayout(false);
            this.TopMenu.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip TopMenu;
        private System.Windows.Forms.ToolStripMenuItem tsFile;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_Custom;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_Miami;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_Frisco;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_LA;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_NewYork;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_Newcastle;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_Desert;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Open_CarPark;
        private System.Windows.Forms.ToolStripMenuItem tsFile_SaveAs;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem tsFile_Exit;
        private System.Windows.Forms.ToolStripMenuItem tsEdit;
        private System.Windows.Forms.ToolStripMenuItem tsView;
        private System.Windows.Forms.ToolStripMenuItem tsView_Models;
        private System.Windows.Forms.ToolStripMenuItem tsView_Textures;
        private System.Windows.Forms.ToolStripMenuItem tsView_TextureDefs;
        private System.Windows.Forms.ToolStripMenuItem tsDriver;
        private System.Windows.Forms.ToolStripMenuItem tsDriver_Launch;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem tsDriver_OpenRoot;
        private System.Windows.Forms.ToolStripMenuItem tsDriver_Patch;
        private System.Windows.Forms.ToolStripMenuItem tsSettings;
        private System.Windows.Forms.ToolStripMenuItem tsHelp;
        private System.Windows.Forms.ToolStripMenuItem tsHelp_HelpTopics;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem tsHelp_About;
        private System.Windows.Forms.Panel WidgetPanel;
    }
}

