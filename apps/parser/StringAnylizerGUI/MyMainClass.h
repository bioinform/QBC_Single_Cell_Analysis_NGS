#pragma  once

#include  <vector>
#include  <string>
#include "../cpp/Resources.h"

using namespace std;
using System::String;
ref class MyMainClass
{
	int deviceId;
	int mutationNumber;
	bool savebadLines;
	bool saveByCells;

	bool None = false;
	bool Moderate = false;
	bool Stringent = false;
	bool SC3_Remove = false;
	bool RC_remove = true;
	bool Run_Singlets = true;

	String^ whereat = "";
	String^ counting = "";

	array<String^>^ inputPath2;

	bool finished;
	bool finishedError;
	int filter;

    LegacyLandMines* p_landmines_;
	
public:
	MyMainClass();
	~MyMainClass(void);
    !MyMainClass();

	void runEntry();

	void setDevice(int device);
	void setMutation(int num);
	void setSaveBadLines(bool flag);
	void setByCells(bool flag);
	void setNone(bool flag);
	void setModerate(bool flag);
	void setStringent(bool flag);
	void setSC3save(bool flag);
	void setRC(bool flag);
	void setSinglets(bool flag);

	
	void setPath2(array<String^>^ str);
	void calcc(System::Windows::Forms::TextBox^ WinBox, System::Windows::Forms::TextBox^ TieBox);
	bool checkFinished() {	return finished;}

	void forceFinish()	{
		finished = true;
		finishedError = true;
	}

	bool checkFinishedWithError() {	return finished && finishedError;	}

	void reset() {
		finished = false;
		finishedError = false;
	}

	void setFilter(int f){	filter = f;	}

	vector<string> getDevices();


};
