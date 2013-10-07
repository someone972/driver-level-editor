namespace DriverLevelEditor
{
    partial class TexturesWidget
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.grpTexFlags = new System.Windows.Forms.GroupBox();
            this.nPcarID = new System.Windows.Forms.NumericUpDown();
            this.lblPcarID = new System.Windows.Forms.Label();
            this.rdPcarDamaged = new System.Windows.Forms.RadioButton();
            this.rdPcarClean = new System.Windows.Forms.RadioButton();
            this.ckIsPcarTex = new System.Windows.Forms.CheckBox();
            this.cbLoadRam = new System.Windows.Forms.CheckBox();
            this.ckHasTrans = new System.Windows.Forms.CheckBox();
            this.ckPaletted = new System.Windows.Forms.CheckBox();
            this.nTexID = new System.Windows.Forms.NumericUpDown();
            this.lTexList = new System.Windows.Forms.ListBox();
            this.ddTexFilter = new System.Windows.Forms.ComboBox();
            this.lblTexFilter = new System.Windows.Forms.Label();
            this.lblTexMode = new System.Windows.Forms.Label();
            this.ddTexMode = new System.Windows.Forms.ComboBox();
            this.lblTexSize = new System.Windows.Forms.Label();
            this.ddTexSize = new System.Windows.Forms.ComboBox();
            this.grpTextures = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.grpTexFlags.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nPcarID)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nTexID)).BeginInit();
            this.grpTextures.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            this.splitContainer1.Panel1.Padding = new System.Windows.Forms.Padding(3, 0, 0, 0);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.grpTexFlags);
            this.splitContainer1.Panel2.Controls.Add(this.grpTextures);
            this.splitContainer1.Panel2.Padding = new System.Windows.Forms.Padding(0, 3, 4, 4);
            this.splitContainer1.Size = new System.Drawing.Size(700, 500);
            this.splitContainer1.SplitterDistance = 480;
            this.splitContainer1.TabIndex = 7;
            this.splitContainer1.TabStop = false;
            // 
            // grpTexFlags
            // 
            this.grpTexFlags.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.grpTexFlags.Controls.Add(this.nPcarID);
            this.grpTexFlags.Controls.Add(this.lblPcarID);
            this.grpTexFlags.Controls.Add(this.rdPcarDamaged);
            this.grpTexFlags.Controls.Add(this.rdPcarClean);
            this.grpTexFlags.Controls.Add(this.ckIsPcarTex);
            this.grpTexFlags.Controls.Add(this.cbLoadRam);
            this.grpTexFlags.Controls.Add(this.ckHasTrans);
            this.grpTexFlags.Controls.Add(this.ckPaletted);
            this.grpTexFlags.Location = new System.Drawing.Point(0, 321);
            this.grpTexFlags.Margin = new System.Windows.Forms.Padding(0, 0, 3, 3);
            this.grpTexFlags.Name = "grpTexFlags";
            this.grpTexFlags.Padding = new System.Windows.Forms.Padding(12, 6, 12, 6);
            this.grpTexFlags.Size = new System.Drawing.Size(209, 172);
            this.grpTexFlags.TabIndex = 6;
            this.grpTexFlags.TabStop = false;
            // 
            // nPcarID
            // 
            this.nPcarID.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.nPcarID.Enabled = false;
            this.nPcarID.Location = new System.Drawing.Point(103, 143);
            this.nPcarID.Name = "nPcarID";
            this.nPcarID.Size = new System.Drawing.Size(91, 20);
            this.nPcarID.TabIndex = 12;
            // 
            // lblPcarID
            // 
            this.lblPcarID.AutoSize = true;
            this.lblPcarID.Enabled = false;
            this.lblPcarID.Location = new System.Drawing.Point(15, 146);
            this.lblPcarID.Name = "lblPcarID";
            this.lblPcarID.Size = new System.Drawing.Size(64, 13);
            this.lblPcarID.TabIndex = 12;
            this.lblPcarID.Text = "Car number:";
            this.lblPcarID.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // rdPcarDamaged
            // 
            this.rdPcarDamaged.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.rdPcarDamaged.AutoSize = true;
            this.rdPcarDamaged.Enabled = false;
            this.rdPcarDamaged.Location = new System.Drawing.Point(103, 114);
            this.rdPcarDamaged.Name = "rdPcarDamaged";
            this.rdPcarDamaged.Size = new System.Drawing.Size(71, 17);
            this.rdPcarDamaged.TabIndex = 11;
            this.rdPcarDamaged.TabStop = true;
            this.rdPcarDamaged.Text = "Damaged";
            this.rdPcarDamaged.UseVisualStyleBackColor = true;
            // 
            // rdPcarClean
            // 
            this.rdPcarClean.AutoSize = true;
            this.rdPcarClean.Enabled = false;
            this.rdPcarClean.Location = new System.Drawing.Point(15, 114);
            this.rdPcarClean.Name = "rdPcarClean";
            this.rdPcarClean.Size = new System.Drawing.Size(52, 17);
            this.rdPcarClean.TabIndex = 10;
            this.rdPcarClean.TabStop = true;
            this.rdPcarClean.Text = "Clean";
            this.rdPcarClean.UseVisualStyleBackColor = true;
            // 
            // ckIsPcarTex
            // 
            this.ckIsPcarTex.AutoSize = true;
            this.ckIsPcarTex.Location = new System.Drawing.Point(15, 91);
            this.ckIsPcarTex.Name = "ckIsPcarTex";
            this.ckIsPcarTex.Size = new System.Drawing.Size(118, 17);
            this.ckIsPcarTex.TabIndex = 9;
            this.ckIsPcarTex.Text = "Is player car texture";
            this.ckIsPcarTex.UseVisualStyleBackColor = true;
            // 
            // cbLoadRam
            // 
            this.cbLoadRam.AutoSize = true;
            this.cbLoadRam.Enabled = false;
            this.cbLoadRam.Location = new System.Drawing.Point(15, 68);
            this.cbLoadRam.Name = "cbLoadRam";
            this.cbLoadRam.Size = new System.Drawing.Size(82, 17);
            this.cbLoadRam.TabIndex = 8;
            this.cbLoadRam.Text = "Load to ram";
            this.cbLoadRam.UseVisualStyleBackColor = true;
            // 
            // ckHasTrans
            // 
            this.ckHasTrans.AutoSize = true;
            this.ckHasTrans.Location = new System.Drawing.Point(15, 45);
            this.ckHasTrans.Name = "ckHasTrans";
            this.ckHasTrans.Size = new System.Drawing.Size(113, 17);
            this.ckHasTrans.TabIndex = 7;
            this.ckHasTrans.Text = "Has Transparency";
            this.ckHasTrans.UseVisualStyleBackColor = true;
            // 
            // ckPaletted
            // 
            this.ckPaletted.AutoSize = true;
            this.ckPaletted.Location = new System.Drawing.Point(15, 22);
            this.ckPaletted.Name = "ckPaletted";
            this.ckPaletted.Size = new System.Drawing.Size(94, 17);
            this.ckPaletted.TabIndex = 6;
            this.ckPaletted.Text = "Paletted (8-bit)";
            this.ckPaletted.UseVisualStyleBackColor = true;
            // 
            // nTexID
            // 
            this.nTexID.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.nTexID.Location = new System.Drawing.Point(7, 158);
            this.nTexID.Name = "nTexID";
            this.nTexID.Size = new System.Drawing.Size(195, 20);
            this.nTexID.TabIndex = 4;
            // 
            // lTexList
            // 
            this.lTexList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lTexList.FormattingEnabled = true;
            this.lTexList.Location = new System.Drawing.Point(7, 187);
            this.lTexList.Name = "lTexList";
            this.lTexList.Size = new System.Drawing.Size(195, 121);
            this.lTexList.TabIndex = 5;
            // 
            // ddTexFilter
            // 
            this.ddTexFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ddTexFilter.BackColor = System.Drawing.SystemColors.Window;
            this.ddTexFilter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ddTexFilter.FormattingEnabled = true;
            this.ddTexFilter.Items.AddRange(new object[] {
            "All Textures",
            "Car Textures",
            "Paletted",
            "15-Bit",
            "Has Transparency"});
            this.ddTexFilter.Location = new System.Drawing.Point(7, 128);
            this.ddTexFilter.Margin = new System.Windows.Forms.Padding(6);
            this.ddTexFilter.Name = "ddTexFilter";
            this.ddTexFilter.Size = new System.Drawing.Size(195, 21);
            this.ddTexFilter.TabIndex = 3;
            // 
            // lblTexFilter
            // 
            this.lblTexFilter.AutoSize = true;
            this.lblTexFilter.Location = new System.Drawing.Point(7, 109);
            this.lblTexFilter.Name = "lblTexFilter";
            this.lblTexFilter.Size = new System.Drawing.Size(32, 13);
            this.lblTexFilter.TabIndex = 3;
            this.lblTexFilter.Text = "Filter:";
            // 
            // lblTexMode
            // 
            this.lblTexMode.AutoSize = true;
            this.lblTexMode.Location = new System.Drawing.Point(6, 63);
            this.lblTexMode.Name = "lblTexMode";
            this.lblTexMode.Size = new System.Drawing.Size(62, 13);
            this.lblTexMode.TabIndex = 2;
            this.lblTexMode.Text = "View mode:";
            // 
            // ddTexMode
            // 
            this.ddTexMode.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ddTexMode.BackColor = System.Drawing.SystemColors.Window;
            this.ddTexMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ddTexMode.FormattingEnabled = true;
            this.ddTexMode.Items.AddRange(new object[] {
            "Current Palette Only",
            "All Palettes",
            "All Palettes on One Line"});
            this.ddTexMode.Location = new System.Drawing.Point(7, 82);
            this.ddTexMode.Margin = new System.Windows.Forms.Padding(6);
            this.ddTexMode.Name = "ddTexMode";
            this.ddTexMode.Size = new System.Drawing.Size(195, 21);
            this.ddTexMode.TabIndex = 2;
            // 
            // lblTexSize
            // 
            this.lblTexSize.AutoSize = true;
            this.lblTexSize.Location = new System.Drawing.Point(4, 17);
            this.lblTexSize.Name = "lblTexSize";
            this.lblTexSize.Size = new System.Drawing.Size(67, 13);
            this.lblTexSize.TabIndex = 0;
            this.lblTexSize.Text = "Texture size:";
            // 
            // ddTexSize
            // 
            this.ddTexSize.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ddTexSize.BackColor = System.Drawing.SystemColors.Window;
            this.ddTexSize.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ddTexSize.FormattingEnabled = true;
            this.ddTexSize.Items.AddRange(new object[] {
            "64",
            "128",
            "256 (Native)",
            "512"});
            this.ddTexSize.Location = new System.Drawing.Point(7, 36);
            this.ddTexSize.Margin = new System.Windows.Forms.Padding(6);
            this.ddTexSize.Name = "ddTexSize";
            this.ddTexSize.Size = new System.Drawing.Size(195, 21);
            this.ddTexSize.TabIndex = 1;
            // 
            // grpTextures
            // 
            this.grpTextures.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.grpTextures.Controls.Add(this.lTexList);
            this.grpTextures.Controls.Add(this.ddTexSize);
            this.grpTextures.Controls.Add(this.ddTexMode);
            this.grpTextures.Controls.Add(this.lblTexMode);
            this.grpTextures.Controls.Add(this.lblTexFilter);
            this.grpTextures.Controls.Add(this.lblTexSize);
            this.grpTextures.Controls.Add(this.ddTexFilter);
            this.grpTextures.Controls.Add(this.nTexID);
            this.grpTextures.Location = new System.Drawing.Point(0, 3);
            this.grpTextures.Margin = new System.Windows.Forms.Padding(0, 0, 3, 0);
            this.grpTextures.Name = "grpTextures";
            this.grpTextures.Padding = new System.Windows.Forms.Padding(4);
            this.grpTextures.Size = new System.Drawing.Size(209, 318);
            this.grpTextures.TabIndex = 0;
            this.grpTextures.TabStop = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.BackColor = System.Drawing.SystemColors.Control;
            this.groupBox1.Location = new System.Drawing.Point(6, 3);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 3, 0, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(474, 490);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            // 
            // TexturesWidget
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.splitContainer1);
            this.DoubleBuffered = true;
            this.Name = "TexturesWidget";
            this.Size = new System.Drawing.Size(700, 500);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.grpTexFlags.ResumeLayout(false);
            this.grpTexFlags.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nPcarID)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nTexID)).EndInit();
            this.grpTextures.ResumeLayout(false);
            this.grpTextures.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.GroupBox grpTexFlags;
        private System.Windows.Forms.NumericUpDown nPcarID;
        private System.Windows.Forms.Label lblPcarID;
        private System.Windows.Forms.RadioButton rdPcarDamaged;
        private System.Windows.Forms.RadioButton rdPcarClean;
        private System.Windows.Forms.CheckBox ckIsPcarTex;
        private System.Windows.Forms.CheckBox cbLoadRam;
        private System.Windows.Forms.CheckBox ckHasTrans;
        private System.Windows.Forms.CheckBox ckPaletted;
        private System.Windows.Forms.NumericUpDown nTexID;
        private System.Windows.Forms.ListBox lTexList;
        private System.Windows.Forms.ComboBox ddTexFilter;
        private System.Windows.Forms.Label lblTexFilter;
        private System.Windows.Forms.Label lblTexMode;
        private System.Windows.Forms.ComboBox ddTexMode;
        private System.Windows.Forms.Label lblTexSize;
        private System.Windows.Forms.ComboBox ddTexSize;
        private System.Windows.Forms.GroupBox grpTextures;
        private System.Windows.Forms.GroupBox groupBox1;

    }
}
