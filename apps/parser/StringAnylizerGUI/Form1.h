#pragma        once
#include        "MyMainClass.h"
#include		"process.h"
#include		"iostream"
#include		<string>
#include		<time.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace System::IO;
using namespace std;


namespace StringAnylizerGUI {
	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			array<String^>^ drives = Directory::GetLogicalDrives();

			for (int i = 0; i < drives->Length; i++)
			{
				String^ name = dynamic_cast<String^>(drives->GetValue(i));
				TreeNode^ tn = gcnew TreeNode(name);
				treeView1->Nodes->Add(tn);
				tn->Nodes->Add(gcnew TreeNode("<dummy>"));
			}
		}
	protected:

		MyMainClass^ mainClass;
		bool IsPress;
		int hour;
		int minute;
		int second;

	private: System::Windows::Forms::Timer^ myTimer;

	private: System::Windows::Forms::ListBox^ listboxInput;
	private: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::ImageList^ imageList1;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel4;

	private: System::Windows::Forms::Button^  Choose;
	private: System::Windows::Forms::Button^  Remove_Selected;
	private: System::Windows::Forms::Button^  RemoveSinglets;
	private: System::Windows::Forms::Button^  Pre_Filter;

	private: System::Windows::Forms::CheckBox^  ByCells;
	private: System::Windows::Forms::CheckBox^  SC3save;
	private: System::Windows::Forms::CheckBox^  cb_Stringent;
	private: System::Windows::Forms::CheckBox^  cb_Moderate;

	private: System::Windows::Forms::CheckBox^  cb_None;
	private: System::Windows::Forms::CheckBox^ checkBadLines;
	private: System::Windows::Forms::TextBox^  WhereAt;
	private: System::Windows::Forms::TextBox^  Counting;
	private: System::Windows::Forms::CheckBox^  cb_RandomCheck;
	private: System::Windows::Forms::CheckBox^  cb_Singlets;
	private: System::Windows::Forms::ToolTip^  tt_None;
	private: System::Windows::Forms::ToolTip^  tt_Moderate;
	private: System::Windows::Forms::ToolTip^  tt_Stringent;
	private: System::Windows::Forms::ToolTip^  tt_Analyze;
	private: System::Windows::Forms::ToolTip^  tt_AutoRunSinglets;
	private: System::Windows::Forms::ToolTip^  tt_RC_Correct;
	private: System::Windows::Forms::ToolTip^  tt_SaveBad;
	private: System::Windows::Forms::ToolTip^  tt_SaveByCells;

	private: System::Windows::Forms::ToolTip^  tt_RunPreFilter;
	private: System::Windows::Forms::ToolTip^  tt_ManualRunSinglets;
	private: System::Windows::Forms::ToolTip^  tt_CUDADevice;
	private: System::Windows::Forms::ToolTip^  tt_ChooseFolders;
	private: System::Windows::Forms::ToolTip^  tt_NavigateFolders;




	protected:
		Thread^ thread;
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:


	private: System::Windows::Forms::ComboBox^ cbDevices;

	private: System::Windows::Forms::TextBox^ tbInput;
	private: System::Windows::Forms::TextBox^ tbSecond;
	private: System::Windows::Forms::TextBox^  tbHour;
	private: System::Windows::Forms::TextBox^ tbMinutes;

	private: System::ComponentModel::IContainer^ components;


			 /// <summary>
			 /// Required designer variable.
			 /// </summary>

#pragma        region Windows Form Designer generated code
			 /// <summary>
			 /// Required method for Designer support - do not modify
			 /// the contents of this method with the code editor.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
				 this->cbDevices = (gcnew System::Windows::Forms::ComboBox());
				 this->checkBadLines = (gcnew System::Windows::Forms::CheckBox());
				 this->tbInput = (gcnew System::Windows::Forms::TextBox());
				 this->tbSecond = (gcnew System::Windows::Forms::TextBox());
				 this->tbMinutes = (gcnew System::Windows::Forms::TextBox());
				 this->myTimer = (gcnew System::Windows::Forms::Timer(this->components));
				 this->listboxInput = (gcnew System::Windows::Forms::ListBox());
				 this->treeView1 = (gcnew System::Windows::Forms::TreeView());
				 this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
				 this->panel4 = (gcnew System::Windows::Forms::Panel());
				 this->tbHour = (gcnew System::Windows::Forms::TextBox());
				 this->Choose = (gcnew System::Windows::Forms::Button());
				 this->Remove_Selected = (gcnew System::Windows::Forms::Button());
				 this->RemoveSinglets = (gcnew System::Windows::Forms::Button());
				 this->Pre_Filter = (gcnew System::Windows::Forms::Button());
				 this->ByCells = (gcnew System::Windows::Forms::CheckBox());
				 this->panel2 = (gcnew System::Windows::Forms::Panel());
				 this->cb_Singlets = (gcnew System::Windows::Forms::CheckBox());
				 this->cb_RandomCheck = (gcnew System::Windows::Forms::CheckBox());
				 this->cb_Stringent = (gcnew System::Windows::Forms::CheckBox());
				 this->cb_Moderate = (gcnew System::Windows::Forms::CheckBox());
				 this->cb_None = (gcnew System::Windows::Forms::CheckBox());
				 this->SC3save = (gcnew System::Windows::Forms::CheckBox());
				 this->panel1 = (gcnew System::Windows::Forms::Panel());
				 this->WhereAt = (gcnew System::Windows::Forms::TextBox());
				 this->Counting = (gcnew System::Windows::Forms::TextBox());
				 this->tt_None = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_Moderate = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_Stringent = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_Analyze = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_AutoRunSinglets = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_RC_Correct = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_SaveBad = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_SaveByCells = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_RunPreFilter = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_ManualRunSinglets = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_CUDADevice = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_ChooseFolders = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->tt_NavigateFolders = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->panel4->SuspendLayout();
				 this->panel2->SuspendLayout();
				 this->panel1->SuspendLayout();
				 this->SuspendLayout();
				 // 
				 // cbDevices
				 // 
				 this->cbDevices->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(224)),
					 static_cast<System::Int32>(static_cast<System::Byte>(192)));
				 this->cbDevices->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cbDevices->FormattingEnabled = true;
				 this->cbDevices->Location = System::Drawing::Point(235, 190);
				 this->cbDevices->Margin = System::Windows::Forms::Padding(4);
				 this->cbDevices->Name = L"cbDevices";
				 this->cbDevices->Size = System::Drawing::Size(153, 21);
				 this->cbDevices->TabIndex = 1;
				 // 
				 // checkBadLines
				 // 
				 this->checkBadLines->AutoSize = true;
				 this->checkBadLines->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->checkBadLines->Location = System::Drawing::Point(19, 28);
				 this->checkBadLines->Margin = System::Windows::Forms::Padding(4);
				 this->checkBadLines->Name = L"checkBadLines";
				 this->checkBadLines->Size = System::Drawing::Size(112, 19);
				 this->checkBadLines->TabIndex = 2;
				 this->checkBadLines->Text = L"Save bad seq";
				 this->checkBadLines->UseVisualStyleBackColor = true;
				 // 
				 // tbInput
				 // 
				 this->tbInput->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->tbInput->Location = System::Drawing::Point(7, 111);
				 this->tbInput->Margin = System::Windows::Forms::Padding(4);
				 this->tbInput->Name = L"tbInput";
				 this->tbInput->ReadOnly = true;
				 this->tbInput->Size = System::Drawing::Size(220, 18);
				 this->tbInput->TabIndex = 6;
				 // 
				 // tbSecond
				 // 
				 this->tbSecond->BackColor = System::Drawing::SystemColors::ActiveCaption;
				 this->tbSecond->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->tbSecond->Location = System::Drawing::Point(62, 2);
				 this->tbSecond->Margin = System::Windows::Forms::Padding(1);
				 this->tbSecond->Name = L"tbSecond";
				 this->tbSecond->ReadOnly = true;
				 this->tbSecond->Size = System::Drawing::Size(25, 23);
				 this->tbSecond->TabIndex = 2;
				 this->tbSecond->Text = L"0";
				 this->tbSecond->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
				 // 
				 // tbMinutes
				 // 
				 this->tbMinutes->BackColor = System::Drawing::SystemColors::ActiveCaption;
				 this->tbMinutes->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->tbMinutes->Location = System::Drawing::Point(31, 1);
				 this->tbMinutes->Margin = System::Windows::Forms::Padding(4);
				 this->tbMinutes->Name = L"tbMinutes";
				 this->tbMinutes->ReadOnly = true;
				 this->tbMinutes->Size = System::Drawing::Size(30, 26);
				 this->tbMinutes->TabIndex = 1;
				 this->tbMinutes->Text = L"0";
				 this->tbMinutes->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
				 // 
				 // myTimer
				 // 
				 this->myTimer->Interval = 1000;
				 this->myTimer->Tick += gcnew System::EventHandler(this, &Form1::myTimer_Tick);
				 // 
				 // listboxInput
				 // 
				 this->listboxInput->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->listboxInput->Location = System::Drawing::Point(7, 3);
				 this->listboxInput->Margin = System::Windows::Forms::Padding(4);
				 this->listboxInput->Name = L"listboxInput";
				 this->listboxInput->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
				 this->listboxInput->Size = System::Drawing::Size(220, 108);
				 this->listboxInput->TabIndex = 13;
				 this->listboxInput->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::listboxInput_MouseDoubleClick);
				 this->listboxInput->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::listboxInput_MouseDoubleClick);
				 // 
				 // treeView1
				 // 
				 this->treeView1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->treeView1->Indent = 10;
				 this->treeView1->Location = System::Drawing::Point(7, 129);
				 this->treeView1->Margin = System::Windows::Forms::Padding(4);
				 this->treeView1->Name = L"treeView1";
				 this->treeView1->Size = System::Drawing::Size(220, 355);
				 this->treeView1->TabIndex = 12;
				 this->treeView1->BeforeExpand += gcnew System::Windows::Forms::TreeViewCancelEventHandler(this, &Form1::treeView1_BeforeExpand);
				 this->treeView1->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &Form1::treeView1_AfterSelect);
				 this->treeView1->DoubleClick += gcnew System::EventHandler(this, &Form1::treeView1_DoubleClick);
				 // 
				 // imageList1
				 // 
				 this->imageList1->ColorDepth = System::Windows::Forms::ColorDepth::Depth8Bit;
				 this->imageList1->ImageSize = System::Drawing::Size(16, 16);
				 this->imageList1->TransparentColor = System::Drawing::Color::Transparent;
				 // 
				 // panel4
				 // 
				 this->panel4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(192)),
					 static_cast<System::Int32>(static_cast<System::Byte>(255)));
				 this->panel4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->panel4->Controls->Add(this->tbHour);
				 this->panel4->Controls->Add(this->tbMinutes);
				 this->panel4->Controls->Add(this->tbSecond);
				 this->panel4->Location = System::Drawing::Point(298, 40);
				 this->panel4->Name = L"panel4";
				 this->panel4->Size = System::Drawing::Size(90, 31);
				 this->panel4->TabIndex = 17;
				 // 
				 // tbHour
				 // 
				 this->tbHour->BackColor = System::Drawing::SystemColors::ActiveCaption;
				 this->tbHour->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->tbHour->Location = System::Drawing::Point(0, 1);
				 this->tbHour->Margin = System::Windows::Forms::Padding(4);
				 this->tbHour->Name = L"tbHour";
				 this->tbHour->ReadOnly = true;
				 this->tbHour->Size = System::Drawing::Size(30, 26);
				 this->tbHour->TabIndex = 3;
				 this->tbHour->Text = L"0";
				 this->tbHour->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
				 // 
				 // Choose
				 // 
				 this->Choose->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
				 this->Choose->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->Choose->ForeColor = System::Drawing::Color::Black;
				 this->Choose->Location = System::Drawing::Point(243, 7);
				 this->Choose->Margin = System::Windows::Forms::Padding(4);
				 this->Choose->Name = L"Choose";
				 this->Choose->Size = System::Drawing::Size(138, 30);
				 this->Choose->TabIndex = 15;
				 this->Choose->Text = L"Analyze";
				 this->Choose->UseVisualStyleBackColor = true;
				 this->Choose->Click += gcnew System::EventHandler(this, &Form1::Choose_Click);
				 // 
				 // Remove_Selected
				 // 
				 this->Remove_Selected->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->Remove_Selected->ForeColor = System::Drawing::SystemColors::WindowFrame;
				 this->Remove_Selected->Location = System::Drawing::Point(231, 42);
				 this->Remove_Selected->Margin = System::Windows::Forms::Padding(0);
				 this->Remove_Selected->Name = L"Remove_Selected";
				 this->Remove_Selected->Size = System::Drawing::Size(64, 26);
				 this->Remove_Selected->TabIndex = 14;
				 this->Remove_Selected->Text = L"Remove";
				 this->Remove_Selected->UseVisualStyleBackColor = true;
				 this->Remove_Selected->Click += gcnew System::EventHandler(this, &Form1::Remove_Selected_Click);
				 // 
				 // RemoveSinglets
				 // 
				 this->RemoveSinglets->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->RemoveSinglets->Location = System::Drawing::Point(77, 1);
				 this->RemoveSinglets->Name = L"RemoveSinglets";
				 this->RemoveSinglets->Size = System::Drawing::Size(68, 28);
				 this->RemoveSinglets->TabIndex = 19;
				 this->RemoveSinglets->Text = L"Singlets";
				 this->RemoveSinglets->UseVisualStyleBackColor = true;
				 this->RemoveSinglets->Click += gcnew System::EventHandler(this, &Form1::RemoveSinglets_Click);
				 // 
				 // Pre_Filter
				 // 
				 this->Pre_Filter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->Pre_Filter->Location = System::Drawing::Point(5, 1);
				 this->Pre_Filter->Name = L"Pre_Filter";
				 this->Pre_Filter->Size = System::Drawing::Size(69, 28);
				 this->Pre_Filter->TabIndex = 18;
				 this->Pre_Filter->Text = L"Pre_Filter";
				 this->Pre_Filter->UseVisualStyleBackColor = true;
				 this->Pre_Filter->Click += gcnew System::EventHandler(this, &Form1::Pre_Filter_Click);
				 // 
				 // ByCells
				 // 
				 this->ByCells->AutoSize = true;
				 this->ByCells->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->ByCells->Location = System::Drawing::Point(19, 45);
				 this->ByCells->Name = L"ByCells";
				 this->ByCells->Size = System::Drawing::Size(108, 19);
				 this->ByCells->TabIndex = 16;
				 this->ByCells->Text = L"Save ByCells";
				 this->ByCells->UseVisualStyleBackColor = true;
				 // 
				 // panel2
				 // 
				 this->panel2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
					 static_cast<System::Int32>(static_cast<System::Byte>(255)));
				 this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->panel2->Controls->Add(this->cb_Singlets);
				 this->panel2->Controls->Add(this->cb_RandomCheck);
				 this->panel2->Controls->Add(this->cb_Stringent);
				 this->panel2->Controls->Add(this->cb_Moderate);
				 this->panel2->Controls->Add(this->cb_None);
				 this->panel2->Controls->Add(this->SC3save);
				 this->panel2->Location = System::Drawing::Point(229, 111);
				 this->panel2->Margin = System::Windows::Forms::Padding(4);
				 this->panel2->Name = L"panel2";
				 this->panel2->Size = System::Drawing::Size(164, 79);
				 this->panel2->TabIndex = 15;
				 // 
				 // cb_Singlets
				 // 
				 this->cb_Singlets->AutoSize = true;
				 this->cb_Singlets->Checked = true;
				 this->cb_Singlets->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->cb_Singlets->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cb_Singlets->Location = System::Drawing::Point(7, 42);
				 this->cb_Singlets->Name = L"cb_Singlets";
				 this->cb_Singlets->Size = System::Drawing::Size(103, 17);
				 this->cb_Singlets->TabIndex = 25;
				 this->cb_Singlets->Text = L"Filter Singlets";
				 this->cb_Singlets->UseVisualStyleBackColor = true;
				 // 
				 // cb_RandomCheck
				 // 
				 this->cb_RandomCheck->AutoSize = true;
				 this->cb_RandomCheck->Checked = true;
				 this->cb_RandomCheck->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->cb_RandomCheck->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cb_RandomCheck->Location = System::Drawing::Point(7, 60);
				 this->cb_RandomCheck->Name = L"cb_RandomCheck";
				 this->cb_RandomCheck->Size = System::Drawing::Size(59, 17);
				 this->cb_RandomCheck->TabIndex = 24;
				 this->cb_RandomCheck->Text = L"RC (-)";
				 this->cb_RandomCheck->UseVisualStyleBackColor = true;
				 // 
				 // cb_Stringent
				 // 
				 this->cb_Stringent->AutoSize = true;
				 this->cb_Stringent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cb_Stringent->Location = System::Drawing::Point(73, 19);
				 this->cb_Stringent->Name = L"cb_Stringent";
				 this->cb_Stringent->Size = System::Drawing::Size(77, 17);
				 this->cb_Stringent->TabIndex = 23;
				 this->cb_Stringent->Text = L"Stringent";
				 this->cb_Stringent->UseVisualStyleBackColor = true;
				 // 
				 // cb_Moderate
				 // 
				 this->cb_Moderate->AutoSize = true;
				 this->cb_Moderate->Checked = true;
				 this->cb_Moderate->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->cb_Moderate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cb_Moderate->Location = System::Drawing::Point(73, 4);
				 this->cb_Moderate->Name = L"cb_Moderate";
				 this->cb_Moderate->Size = System::Drawing::Size(79, 17);
				 this->cb_Moderate->TabIndex = 22;
				 this->cb_Moderate->Text = L"Moderate";
				 this->cb_Moderate->UseVisualStyleBackColor = true;
				 // 
				 // cb_None
				 // 
				 this->cb_None->AutoSize = true;
				 this->cb_None->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->cb_None->Location = System::Drawing::Point(7, 4);
				 this->cb_None->Name = L"cb_None";
				 this->cb_None->Size = System::Drawing::Size(56, 17);
				 this->cb_None->TabIndex = 21;
				 this->cb_None->Text = L"None";
				 this->cb_None->UseVisualStyleBackColor = true;
				 // 
				 // SC3save
				 // 
				 this->SC3save->AutoSize = true;
				 this->SC3save->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->SC3save->Location = System::Drawing::Point(73, 60);
				 this->SC3save->Name = L"SC3save";
				 this->SC3save->Size = System::Drawing::Size(93, 17);
				 this->SC3save->TabIndex = 20;
				 this->SC3save->Text = L"SC3 dupe(-)";
				 this->SC3save->UseVisualStyleBackColor = true;
				 // 
				 // panel1
				 // 
				 this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)),
					 static_cast<System::Int32>(static_cast<System::Byte>(192)));
				 this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				 this->panel1->Controls->Add(this->checkBadLines);
				 this->panel1->Controls->Add(this->ByCells);
				 this->panel1->Controls->Add(this->Pre_Filter);
				 this->panel1->Controls->Add(this->RemoveSinglets);
				 this->panel1->Location = System::Drawing::Point(235, 211);
				 this->panel1->Name = L"panel1";
				 this->panel1->Size = System::Drawing::Size(153, 65);
				 this->panel1->TabIndex = 16;
				 // 
				 // WhereAt
				 // 
				 this->WhereAt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->WhereAt->Location = System::Drawing::Point(229, 73);
				 this->WhereAt->Margin = System::Windows::Forms::Padding(4);
				 this->WhereAt->Name = L"WhereAt";
				 this->WhereAt->ReadOnly = true;
				 this->WhereAt->Size = System::Drawing::Size(164, 18);
				 this->WhereAt->TabIndex = 18;
				 // 
				 // Counting
				 // 
				 this->Counting->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->Counting->Location = System::Drawing::Point(229, 92);
				 this->Counting->Margin = System::Windows::Forms::Padding(4);
				 this->Counting->Name = L"Counting";
				 this->Counting->ReadOnly = true;
				 this->Counting->Size = System::Drawing::Size(164, 18);
				 this->Counting->TabIndex = 19;
				 // 
				 // tt_None
				 // 
				 this->tt_None->IsBalloon = true;
				 this->tt_None->Tag = L"";
				 this->tt_None->ToolTipTitle = L"Cull:  None";
				 // 
				 // tt_Moderate
				 // 
				 this->tt_Moderate->IsBalloon = true;
				 this->tt_Moderate->ToolTipTitle = L"Cull:  Moderate";
				 // 
				 // tt_Stringent
				 // 
				 this->tt_Stringent->IsBalloon = true;
				 this->tt_Stringent->ToolTipTitle = L"Cull:  Stringent";
				 // 
				 // tt_Analyze
				 // 
				 this->tt_Analyze->IsBalloon = true;
				 this->tt_Analyze->ToolTipTitle = L"Analyze:";
				 // 
				 // tt_AutoRunSinglets
				 // 
				 this->tt_AutoRunSinglets->IsBalloon = true;
				 // 
				 // tt_RC_Correct
				 // 
				 this->tt_RC_Correct->IsBalloon = true;
				 // 
				 // tt_SaveBad
				 // 
				 this->tt_SaveBad->IsBalloon = true;
				 // 
				 // tt_SaveByCells
				 // 
				 this->tt_SaveByCells->IsBalloon = true;
				 // 
				 // tt_RunPreFilter
				 // 
				 this->tt_RunPreFilter->IsBalloon = true;
				 // 
				 // tt_ManualRunSinglets
				 // 
				 this->tt_ManualRunSinglets->IsBalloon = true;
				 // 
				 // tt_CUDADevice
				 // 
				 this->tt_CUDADevice->IsBalloon = true;
				 // 
				 // tt_ChooseFolders
				 // 
				 this->tt_ChooseFolders->IsBalloon = true;
				 // 
				 // tt_NavigateFolders
				 // 
				 this->tt_NavigateFolders->IsBalloon = true;
				 // 
				 // Form1
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)),
					 static_cast<System::Int32>(static_cast<System::Byte>(128)));
				 this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
				 this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
				 this->ClientSize = System::Drawing::Size(397, 486);
				 this->Controls->Add(this->Counting);
				 this->Controls->Add(this->WhereAt);
				 this->Controls->Add(this->treeView1);
				 this->Controls->Add(this->Choose);
				 this->Controls->Add(this->tbInput);
				 this->Controls->Add(this->listboxInput);
				 this->Controls->Add(this->cbDevices);
				 this->Controls->Add(this->panel4);
				 this->Controls->Add(this->Remove_Selected);
				 this->Controls->Add(this->panel1);
				 this->Controls->Add(this->panel2);
				 this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->Margin = System::Windows::Forms::Padding(4);
				 this->MaximizeBox = false;
				 this->MaximumSize = System::Drawing::Size(413, 525);
				 this->MinimumSize = System::Drawing::Size(413, 525);
				 this->Name = L"Form1";
				 this->Text = L"Apprise CUDA Parser";
				 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
				 this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
				 this->panel4->ResumeLayout(false);
				 this->panel4->PerformLayout();
				 this->panel2->ResumeLayout(false);
				 this->panel2->PerformLayout();
				 this->panel1->ResumeLayout(false);
				 this->panel1->PerformLayout();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma        endregion

	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
		String^ fileName = ".SAProject";
		try
		{
			StreamReader^ din = File::OpenText(fileName);
			String^ str;

			while ((str = din->ReadLine()) != nullptr)
			{
				listboxInput->Items->Add(str);
			}
			// Moderate:  Cull sequences if random & SC3 is the same.  Keep the HIGHEST expressed RC-SC3.
			// Stringent: Cull sequences if random       is the same.  Keep the HIGHEST expressed RC.
			tt_Analyze->SetToolTip(Choose,				"Click to start analysis.\n-> Cycles through all folders highlighted in ListBox.");
			tt_ChooseFolders->SetToolTip(listboxInput,	"Click to highlight folders to be analyzed.");
			tt_NavigateFolders->SetToolTip(treeView1,	"Choose a folder to add to the ListBox above.");
			tt_CUDADevice->SetToolTip(cbDevices,		"Choose the CUDA device on which to run the analysis.");

			tt_None->		SetToolTip(cb_None,			"No RC/SC3-based culling.");
			tt_Moderate->	SetToolTip(cb_Moderate,		"Cull identical RC-SC3. \n-> Keep the most common RC-SC3.");
			tt_Stringent->SetToolTip(cb_Stringent,		"Cull identical RC. \n-> Keep the most common RC");
			tt_RC_Correct->SetToolTip(cb_RandomCheck,	"Remove RC duplicates based on point mutation.");
				
			tt_AutoRunSinglets->SetToolTip(cb_Singlets,			"AutoRun singlet filtering after analysis is complete");
			tt_ManualRunSinglets->SetToolTip(RemoveSinglets,	"Manual run to filter singlets.");
			tt_RunPreFilter->SetToolTip(Pre_Filter,				"Manual run to pre-filter for template integrity.");
			
			tt_SaveBad->SetToolTip(checkBadLines,	"Save bad sequences for analysis to text file.");
			tt_SaveByCells->SetToolTip(ByCells,		"Save data as a cell-based tree heirarchy.");

			din->Close();
		}
		catch (Exception^ e)
		{
		}

		minute = 0;
		second = 0;
		IsPress = false;
		mainClass = gcnew MyMainClass();
		vector<string> deviceLists = mainClass->getDevices();

		for (unsigned int i = 0; i < deviceLists.size(); i++)
		{
			String^ sDevice = gcnew String(deviceLists[i].c_str());
			cbDevices->Items->Add(sDevice);
		}

		cbDevices->SelectedIndex = 0;

		if (listboxInput->Items->Count > 0)
		{
			tbInput->Text = listboxInput->Items[0]->ToString();
		}
		else
		{
			tbInput->Text = L"C:\\";
		}
		if (listboxInput->Items->Count != 0)
			listboxInput->SetSelected(0, true);
		treeView1->SelectedNode = GetNodeAtPath(treeView1, tbInput->Text, 0);
	}

	private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
		mainClass->~MyMainClass();
		String^ fileName = ".SAProject";

		StreamWriter^ sw = gcnew StreamWriter(fileName);
		for (int i = 0; i < listboxInput->Items->Count; i++)
		{
			sw->WriteLine(listboxInput->Items[i]->ToString());
		}

		sw->Close();
	}

	private: TreeNode^ GetNodeAtPath(TreeView ^treeView, String ^nodePath, int ^colorit)
	{
		array<String^>^ pathList = nodePath->Split('\\');
		int i = 0;
		int jj = 0;

		TreeNode^ nxtNode = nullptr;

		int foldercount = 0;

		for each(String^folder in pathList)
		{
			if (folder == "")
				continue;
			if (i == 1)
				jj = 1;

			if (i == 0)
			{
				for each (TreeNode ^topNode in treeView1->Nodes)
				{
					String^ nnstring = topNode->FullPath;
					array<String^>^ shortpath = nnstring->Split('\\');
					System::Collections::IEnumerator^ myEnum = shortpath->GetEnumerator();

					while (myEnum->MoveNext())
					{
						String^ entry = safe_cast<String^>(myEnum->Current);
						if (folder->Equals(entry))
						{
							treeView1->SelectedNode = topNode;
							topNode->Expand();
							nxtNode = topNode;
							i = 1;
							break;
						}
					}
				}
			}

			if (i == 1 && jj == 1)
			{
				for each (TreeNode ^topNode in nxtNode->Nodes)
				{
					String^ nnstring = topNode->FullPath;
					array<String^>^ shortpath = nnstring->Split('\\');
					System::Collections::IEnumerator^ myEnum = shortpath->GetEnumerator();

					while (myEnum->MoveNext())
					{
						String^ entry = safe_cast<String^>(myEnum->Current);
						if (folder->Equals(entry))
						{
							treeView1->SelectedNode = topNode;
							topNode->Expand();
							nxtNode = topNode;
							break;
						}
					}
				}
			}
			foldercount++;
		}

		if (nxtNode == nullptr)
		{
			MessageBox::Show("Null");
		}

		treeView1->SelectedNode->BackColor = Color::Yellow;
		return nxtNode;
	}

	private: System::Void Choose_Click(System::Object^ sender, System::EventArgs^ e) {
		if (IsPress == true)
		{
			IsPress = false;
			thread->Abort();
			myTimer->Stop();
			Choose->Text = "Analyze";
		}
		else
		{
			if (listboxInput->SelectedIndex != -1)
			{
				IsPress = true;
				Choose->Text = "Running";
				myTimer->Start();
				minute = 0;
				second = 0;
				tbSecond->Text = "0";
				tbMinutes->Text = "0";

				mainClass->setDevice(cbDevices->SelectedIndex);

				if (ByCells->Checked)		{ mainClass->setByCells(true);   }
				else			            { mainClass->setByCells(false);  }

				if (cb_None->Checked)		{ mainClass->setNone(true); }
				else			            { mainClass->setNone(false);  }

				if (cb_Moderate->Checked)	{ mainClass->setModerate(true);  }
				else						{ mainClass->setModerate(false); }

				if (cb_Stringent->Checked)	{ mainClass->setStringent(true);  }
				else			            { mainClass->setStringent(false);  }

				if (SC3save->Checked)		{ mainClass->setSC3save(true);  }
				else			            { mainClass->setSC3save(false);  }

				if (cb_RandomCheck->Checked){ mainClass->setRC(true); }
				else			            { mainClass->setRC(false); }

				if (checkBadLines->Checked){ mainClass->setSaveBadLines(true); }
				else			            { mainClass->setSaveBadLines(false); }

				if (cb_Singlets->Checked)	{ mainClass->setSinglets(true); }
				else			            { mainClass->setSinglets(false); }

				printf("\nThe following files will be processed:\n");
				array<String^>^ managedArray = gcnew array<String^>(20);
				int y = 0;
				for (int x = 0; x < (listboxInput->SelectedItems->Count); x++)
				{
					String ^temp = listboxInput->SelectedItems[x]->ToString();
					printf("	%d:\t%s\n\n", x+1, temp);
					managedArray[x] = temp;
					y = x;
				}

				printf("\t\tsetByCells:\t\t%s\n", ByCells->Checked ? "True" : "False");
				printf("\t\tsetNone:\t\t%s\n", cb_None->Checked ? "True" : "False");
				printf("\t\tsetModerate:\t\t%s\n", cb_Moderate->Checked ? "True" : "False");
				printf("\t\tsetStringent:\t\t%s\n", cb_Stringent->Checked ? "True" : "False");
				printf("\t\tsetSC3save:\t\t%s\n", SC3save->Checked ? "True" : "False");
				printf("\t\tsetRC:\t\t\t%s\n\n", cb_RandomCheck->Checked ? "True" : "False");

				managedArray[y+1] = "ENDOFLIST";
				mainClass->setPath2(managedArray);

				thread = gcnew Thread(gcnew ThreadStart((MyMainClass^)mainClass, &MyMainClass::runEntry));
				thread->Name = "Hybrid String Matching Thread";
				thread->Start();

				Choose->Text = "Running";

			}
		}
	}

	private: System::Void myTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
		second++;
		if (second == 60)
		{
			minute++;
			second = 0;
		}
		if (minute == 60)
		{
			hour++;
			minute = 0;
		}

		String^ whereAt;

		mainClass->calcc(WhereAt, Counting);

		String^ secondString = Convert::ToString(second);
		String^ minuteString = Convert::ToString(minute);
		String^ hourString = Convert::ToString(hour);
		tbSecond->Text = secondString;
		tbMinutes->Text = minuteString;
		tbHour->Text = hourString;

		if (mainClass->checkFinished())
		{
			myTimer->Stop();
			IsPress = false;
			Choose->Text = "Analyze";
			if (!mainClass->checkFinishedWithError())
			{
				String^ input = tbInput->Text;

				if (!listboxInput->Items->Contains(input))
				{
					listboxInput->Items->Insert(0, input);
				}

				if (listboxInput->Items->Count > 15)
				{
					listboxInput->Items->RemoveAt(15);
				}

				printf("\n\n  SCRIPT has finished\n\n");
			}
		}
	}

	private: System::Void treeView1_DoubleClick(System::Object^ sender, System::EventArgs^ e) {
		treeView1->SelectedNode->BackColor = Color::LightGoldenrodYellow;

		String^ myString = treeView1->SelectedNode->FullPath;
		tbInput->Text = myString;

		String^ input = tbInput->Text;

		if (!listboxInput->Items->Contains(input))
		{
			listboxInput->Items->Insert(0, input);
			listboxInput->SetSelected(0, true);
		}
		if (listboxInput->Items->Count > 15)
		{
			listboxInput->Items->RemoveAt(15);
		}
	}

	private: System::Void treeView1_BeforeExpand(System::Object^ sender, System::Windows::Forms::TreeViewCancelEventArgs^ e) {
		// First zap the dummy node, assuming there is one
		if (e->Node->Nodes->Count != 0)
			e->Node->Nodes->RemoveAt(0);

		// Get a list of subdirectories

		array<String^>^ dirs;
		try
		{
			dirs = Directory::GetDirectories(e->Node->FullPath);
		}
		catch (System::Exception^ pe)
		{
			MessageBox::Show(pe->Message, "Some Error!");
			return;
		}

		// Add a child node for each one
		for (int i = 0; i < dirs->Length; i++)
		{
			String^ dirName = dynamic_cast<String^>(dirs->GetValue(i));
			TreeNode^ tn = gcnew TreeNode(Path::GetFileName(dirName));
			e->Node->Nodes->Add(tn);
			// Add a dummy child node
			tn->Nodes->Add(gcnew TreeNode("<dummy>"));
		}
	}

	private: System::Void treeView1_AfterSelect(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e) {
		String^ myString = treeView1->SelectedNode->FullPath;
		tbInput->Text = myString;
	}

	private: System::Void listboxInput_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
		if (listboxInput->SelectedIndex != -1)
		{
			treeView1->SelectedNode->BackColor = Color::White;
			tbInput->Text = listboxInput->SelectedItem->ToString();
			listboxInput->SetSelected(listboxInput->SelectedIndex, true);
			printf("%s\n", tbInput->Text);
			treeView1->SelectedNode = GetNodeAtPath(treeView1, tbInput->Text, 1);
		}

		String^ fileName = ".SAProject";
		StreamWriter^ sw = gcnew StreamWriter(fileName);
		for (int i = 0; i < listboxInput->Items->Count; i++)
		{
			sw->WriteLine(listboxInput->Items[i]->ToString());
		}

		sw->Close();

	}

	private: System::Void Remove_Selected_Click(System::Object^ sender, System::EventArgs^ e) {
		if (listboxInput->SelectedIndex != -1)
		{
			listboxInput->Items->RemoveAt(listboxInput->SelectedIndex);
		}
	}

#pragma region Clicks&buttons

private: System::Void Pre_Filter_Click(System::Object^  sender, System::EventArgs^  e) {

	char newtest[150];
	sprintf(newtest, "C:/Perl64/bin/perl.exe 2_filterforparse.pl %s", tbInput->Text);
	system(newtest);
}

private: System::Void RemoveSinglets_Click(System::Object^  sender, System::EventArgs^  e) {

	char newtest[150];
	sprintf(newtest, "C:/Perl64/bin/perl.exe 4_RemoveSinglets.pl %s -jitter", tbInput->Text);
	system(newtest);
}

#pragma endregion

};
}
