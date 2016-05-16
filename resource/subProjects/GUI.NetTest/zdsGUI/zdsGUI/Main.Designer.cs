namespace zdsGUI
{
    partial class Main
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main));
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.ssText = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.ssConnectionStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.ssReconnect = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainerMain = new System.Windows.Forms.SplitContainer();
            this.lstSrv = new System.Windows.Forms.ListBox();
            this._core = new System.Windows.Forms.Panel();
            this.lstCoreServices = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.btnCoreGetII = new System.Windows.Forms.Button();
            this.UserManagement = new System.Windows.Forms.Panel();
            this.statuseIcons = new System.Windows.Forms.ImageList(this.components);
            this.tmrConnectionCheck = new System.Windows.Forms.Timer(this.components);
            this.trvInstallInfo = new System.Windows.Forms.TreeView();
            this.trvPermissions = new System.Windows.Forms.TreeView();
            this.label1 = new System.Windows.Forms.Label();
            this.btnUMplUpdate = new System.Windows.Forms.Button();
            this.statusStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerMain)).BeginInit();
            this.splitContainerMain.Panel1.SuspendLayout();
            this.splitContainerMain.Panel2.SuspendLayout();
            this.splitContainerMain.SuspendLayout();
            this._core.SuspendLayout();
            this.UserManagement.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ssText,
            this.toolStripStatusLabel1,
            this.ssConnectionStatus,
            this.ssReconnect});
            this.statusStrip.Location = new System.Drawing.Point(0, 481);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(737, 22);
            this.statusStrip.TabIndex = 0;
            this.statusStrip.Text = "statusStrip1";
            // 
            // ssText
            // 
            this.ssText.Name = "ssText";
            this.ssText.Size = new System.Drawing.Size(637, 17);
            this.ssText.Spring = true;
            this.ssText.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(69, 17);
            this.toolStripStatusLabel1.Text = "Connection";
            // 
            // ssConnectionStatus
            // 
            this.ssConnectionStatus.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.ssConnectionStatus.Image = global::zdsGUI.Properties.Resources.bullet_black;
            this.ssConnectionStatus.Name = "ssConnectionStatus";
            this.ssConnectionStatus.Size = new System.Drawing.Size(16, 17);
            this.ssConnectionStatus.Text = "toolStripStatusLabel2";
            // 
            // ssReconnect
            // 
            this.ssReconnect.BorderSides = ((System.Windows.Forms.ToolStripStatusLabelBorderSides)((((System.Windows.Forms.ToolStripStatusLabelBorderSides.Left | System.Windows.Forms.ToolStripStatusLabelBorderSides.Top) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Right) 
            | System.Windows.Forms.ToolStripStatusLabelBorderSides.Bottom)));
            this.ssReconnect.BorderStyle = System.Windows.Forms.Border3DStyle.Raised;
            this.ssReconnect.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.ssReconnect.IsLink = true;
            this.ssReconnect.LinkBehavior = System.Windows.Forms.LinkBehavior.HoverUnderline;
            this.ssReconnect.Name = "ssReconnect";
            this.ssReconnect.Size = new System.Drawing.Size(67, 19);
            this.ssReconnect.Text = "Reconnect";
            this.ssReconnect.Visible = false;
            this.ssReconnect.Click += new System.EventHandler(this.ssReconnect_Click);
            // 
            // splitContainerMain
            // 
            this.splitContainerMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerMain.Location = new System.Drawing.Point(0, 0);
            this.splitContainerMain.Name = "splitContainerMain";
            // 
            // splitContainerMain.Panel1
            // 
            this.splitContainerMain.Panel1.Controls.Add(this.lstSrv);
            // 
            // splitContainerMain.Panel2
            // 
            this.splitContainerMain.Panel2.Controls.Add(this.UserManagement);
            this.splitContainerMain.Panel2.Controls.Add(this._core);
            this.splitContainerMain.Size = new System.Drawing.Size(737, 481);
            this.splitContainerMain.SplitterDistance = 149;
            this.splitContainerMain.TabIndex = 1;
            // 
            // lstSrv
            // 
            this.lstSrv.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstSrv.FormattingEnabled = true;
            this.lstSrv.IntegralHeight = false;
            this.lstSrv.Location = new System.Drawing.Point(0, 0);
            this.lstSrv.Name = "lstSrv";
            this.lstSrv.Size = new System.Drawing.Size(149, 481);
            this.lstSrv.TabIndex = 2;
            this.lstSrv.SelectedIndexChanged += new System.EventHandler(this.lstSrv_SelectedIndexChanged);
            // 
            // _core
            // 
            this._core.Controls.Add(this.trvInstallInfo);
            this._core.Controls.Add(this.lstCoreServices);
            this._core.Controls.Add(this.btnCoreGetII);
            this._core.Dock = System.Windows.Forms.DockStyle.Fill;
            this._core.Location = new System.Drawing.Point(0, 0);
            this._core.Name = "_core";
            this._core.Size = new System.Drawing.Size(584, 481);
            this._core.TabIndex = 0;
            this._core.Tag = "Core";
            // 
            // lstCoreServices
            // 
            this.lstCoreServices.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
            this.lstCoreServices.Location = new System.Drawing.Point(3, 3);
            this.lstCoreServices.Name = "lstCoreServices";
            this.lstCoreServices.Size = new System.Drawing.Size(253, 255);
            this.lstCoreServices.TabIndex = 1;
            this.lstCoreServices.UseCompatibleStateImageBehavior = false;
            this.lstCoreServices.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Name";
            this.columnHeader1.Width = 117;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Version";
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "State";
            // 
            // btnCoreGetII
            // 
            this.btnCoreGetII.Location = new System.Drawing.Point(262, 3);
            this.btnCoreGetII.Name = "btnCoreGetII";
            this.btnCoreGetII.Size = new System.Drawing.Size(75, 23);
            this.btnCoreGetII.TabIndex = 0;
            this.btnCoreGetII.Text = "Get Service Info";
            this.btnCoreGetII.UseVisualStyleBackColor = true;
            this.btnCoreGetII.Click += new System.EventHandler(this.btnCoreGetII_Click);
            // 
            // UserManagement
            // 
            this.UserManagement.Controls.Add(this.btnUMplUpdate);
            this.UserManagement.Controls.Add(this.label1);
            this.UserManagement.Controls.Add(this.trvPermissions);
            this.UserManagement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.UserManagement.Location = new System.Drawing.Point(0, 0);
            this.UserManagement.Name = "UserManagement";
            this.UserManagement.Size = new System.Drawing.Size(584, 481);
            this.UserManagement.TabIndex = 1;
            this.UserManagement.Tag = "User Management";
            // 
            // statuseIcons
            // 
            this.statuseIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("statuseIcons.ImageStream")));
            this.statuseIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.statuseIcons.Images.SetKeyName(0, "black");
            this.statuseIcons.Images.SetKeyName(1, "green");
            this.statuseIcons.Images.SetKeyName(2, "red");
            this.statuseIcons.Images.SetKeyName(3, "yellow");
            // 
            // tmrConnectionCheck
            // 
            this.tmrConnectionCheck.Enabled = true;
            this.tmrConnectionCheck.Interval = 500;
            this.tmrConnectionCheck.Tick += new System.EventHandler(this.tmrConnectionCheck_Tick);
            // 
            // trvInstallInfo
            // 
            this.trvInstallInfo.Location = new System.Drawing.Point(3, 264);
            this.trvInstallInfo.Name = "trvInstallInfo";
            this.trvInstallInfo.Size = new System.Drawing.Size(578, 214);
            this.trvInstallInfo.TabIndex = 2;
            // 
            // trvPermissions
            // 
            this.trvPermissions.Location = new System.Drawing.Point(3, 24);
            this.trvPermissions.Name = "trvPermissions";
            this.trvPermissions.Size = new System.Drawing.Size(578, 454);
            this.trvPermissions.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(76, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "List Permission";
            // 
            // btnUMplUpdate
            // 
            this.btnUMplUpdate.Location = new System.Drawing.Point(506, 0);
            this.btnUMplUpdate.Name = "btnUMplUpdate";
            this.btnUMplUpdate.Size = new System.Drawing.Size(75, 23);
            this.btnUMplUpdate.TabIndex = 3;
            this.btnUMplUpdate.Text = "update";
            this.btnUMplUpdate.UseVisualStyleBackColor = true;
            this.btnUMplUpdate.Click += new System.EventHandler(this.btnUMplUpdate_Click);
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(737, 503);
            this.Controls.Add(this.splitContainerMain);
            this.Controls.Add(this.statusStrip);
            this.Name = "Main";
            this.Text = "Main";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Main_FormClosing);
            this.Load += new System.EventHandler(this.Main_Load);
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.splitContainerMain.Panel1.ResumeLayout(false);
            this.splitContainerMain.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerMain)).EndInit();
            this.splitContainerMain.ResumeLayout(false);
            this._core.ResumeLayout(false);
            this.UserManagement.ResumeLayout(false);
            this.UserManagement.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.SplitContainer splitContainerMain;
        private System.Windows.Forms.Panel _core;
        private System.Windows.Forms.Panel UserManagement;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStripStatusLabel ssConnectionStatus;
        private System.Windows.Forms.ImageList statuseIcons;
        private System.Windows.Forms.ToolStripStatusLabel ssText;
        private System.Windows.Forms.ToolStripStatusLabel ssReconnect;
        private System.Windows.Forms.Timer tmrConnectionCheck;
        private System.Windows.Forms.ListBox lstSrv;
        private System.Windows.Forms.Button btnCoreGetII;
        private System.Windows.Forms.ListView lstCoreServices;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.TreeView trvInstallInfo;
        private System.Windows.Forms.TreeView trvPermissions;
        private System.Windows.Forms.Button btnUMplUpdate;
        private System.Windows.Forms.Label label1;
    }
}