#include     "StdAfx.h"
#include     "MyMainClass.h"
#include	 "iostream"
#include     "cuda_runtime.h"
#include     <stdio.h>
#include     <string>
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <conio.h>
#include     <windows.h>
#include     <tchar.h>
#include     <fstream>
#include     <vector>
#include     <time.h>
#include     <map>
#include     "kernel.h"

#include     <msclr\marshal_cppstd.h>
#include	 "form1.h"
#include     <stdlib.h>
#include "../cpp/QBCParser.h"
#include "../cpp/Resources.h"



#pragma region Variables
using namespace System::Windows::Forms;
using System::Runtime::InteropServices::Marshal;


using namespace std;
using namespace System;
using System::String;

#pragma endregion


void MarshalString(String ^ s, string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

void MarshalString(String ^ s, wstring& os) {
	using namespace Runtime::InteropServices;
	const wchar_t* chars =
		(const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}



MyMainClass::~MyMainClass(void)
{
    this->!MyMainClass();
}

MyMainClass::!MyMainClass(void)
{
    delete p_landmines_;
}

MyMainClass::MyMainClass(void)
    : deviceId(0)
    , mutationNumber(1)
    , savebadLines(false)
    , inputPath2(gcnew array<String^>(20))
    , p_landmines_(new LegacyLandMines)
{
    inputPath2[0] = gcnew String("C:\\");
}


void MyMainClass::runEntry()
{
	System::Collections::IEnumerator^ myEnum = inputPath2->GetEnumerator();

    rss::QBCParser& qbc_instance = rss::QBCParser::Instance();
    LegacyLandMines& landmines = *p_landmines_;

	while (myEnum->MoveNext())
		{
			String^ entry = safe_cast<String^>(myEnum->Current);		// get the path to directory as String
			String^ compare = "ENDOFLIST";
			if (entry->Equals(compare)){ break; }

			array<String^>^ shortpath = gcnew array<String^>(20);
			shortpath = entry->Split('\\');
			System::Collections::IEnumerator^ myEnumPath = shortpath->GetEnumerator();

			String^ FOLDER = "C:\\";
			for each(String^folder in shortpath)		// get the last folder name in path.
			{
				FOLDER = folder;
			}

			MarshalString(FOLDER, qbc_instance.options().entry);

			reset();
			msclr::interop::marshal_context context;
			std::string inputDirectoryName = context.marshal_as<std::string>(entry);		// get the path to directory as a string

            qbc_instance.options().none = this->None;
            qbc_instance.options().moderate = this->Moderate;
            qbc_instance.options().stringent = this->Stringent;
            qbc_instance.options().sc3_remove = this->SC3_Remove;
            qbc_instance.options().rc_remove = this->RC_remove;
            qbc_instance.options().run_singlets = this->Run_Singlets;

            qbc_instance.options().save_bad_lines = this->savebadLines;
            qbc_instance.options().save_by_cells = this->saveByCells;

			int code = qbc_instance.Initialize(inputDirectoryName, this->savebadLines, this->mutationNumber, this->deviceId);

			if (code == 0)
			{
				finished = true;
				finishedError = true;
				MessageBox::Show("Input folder not have valid input data. Please choose the valid one", "Error");
				return;
			}

			WIN32_FIND_DATA FindFileData;
			HANDLE hFind;
			string fileName = inputDirectoryName + "\\*";
			std::wstring sfileName = std::wstring(fileName.begin(), fileName.end());
			time_t start_time, end_time;

			hFind = FindFirstFile(sfileName.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE)
			{
				finished = true;
				finishedError = true;
				return;
			}
			else
			{
				do
				{
					wstring sfn = FindFileData.cFileName;

					wstring extent = sfn.substr(sfn.find_last_of(L".") + 1);
					if (extent.compare(L"fasta") == 0)
					{
						string fn = std::string(sfn.begin(), sfn.end());
						const string fastaFile = inputDirectoryName + "\\" + fn;

                        bool tmp_none = this->None; // this is a managed class, but legacy code modifies this->None
                        qbc_instance.ProcessFile(landmines, fastaFile, filter, tmp_none, this->Moderate, this->Stringent, this->SC3_Remove, this->RC_remove);

                        this->None = tmp_none;

						time(&start_time);
						reset();
						time(&end_time);
						float totalTimed = (float)difftime(end_time, start_time);
						    printf("	Time processing reset()              : %.2f seconds\n", totalTimed);
					}
				} while (FindNextFile(hFind, &FindFileData));

				FindClose(hFind);
			}

			qbc_instance.CleanUpMem();

			finished = true;

			// run the singlets file to trim
			if (qbc_instance.options().run_singlets)
			{
				Sleep(1000);
				char newtest[200];
				sprintf(newtest, "C:/Perl64/bin/perl.exe 4_RemoveSinglets.pl %s -jitter", entry);
				system(newtest);
			}
		}
}

vector<string> MyMainClass::getDevices()
{
	return rss::QBCParser::Instance().CheckDevice();
}

#pragma region Inter_GUI/CPP variable passing

void MyMainClass::setDevice(int device)
{
	deviceId = device;
}

void MyMainClass::setMutation(int num) {
	mutationNumber = num;
}

void MyMainClass::setSaveBadLines(bool flag) {
	savebadLines = flag;
}

void MyMainClass::setByCells(bool flag) {
	saveByCells = flag;
}

void MyMainClass::setPath2(array<String^>^ str) {
	inputPath2 = str;
}

void MyMainClass::setNone(bool flag) {
	None = flag;
}

void MyMainClass::setModerate(bool flag) {
	Moderate = flag;
}

void MyMainClass::setStringent(bool flag) {
	Stringent = flag;
}

void MyMainClass::setSC3save(bool flag) {
	SC3_Remove = flag;
}

void MyMainClass::setRC(bool flag) {
	RC_remove = flag;
}

void MyMainClass::setSinglets(bool flag) {
	Run_Singlets = flag;
}

void MyMainClass::calcc(System::Windows::Forms::TextBox^ WinBox, System::Windows::Forms::TextBox^ TieBox)
{
	String^ i = gcnew String(p_landmines_->whereatthis.c_str());
	String^ ii = gcnew String(p_landmines_->countingme.c_str());
	WinBox->Text = i;
	TieBox->Text = ii;
}

#pragma endregion


