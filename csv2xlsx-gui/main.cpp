// wxWidgets "Hello world" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/cmdline.h>
#include <wx/filedlg.h>
#include <wx/file.h>
#include <wx/filename.h>

#include <stdio.h>

#include "libcsv2xlsx.h"

class ProgressFrame;

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual void OnInitCmdLine(wxCmdLineParser &parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser &parser);
	virtual bool OnCmdLineError(wxCmdLineParser &parser);
private:
	ProgressFrame *frame;
};

class ProgressFrame : public wxFrame, public wxThreadHelper
{
public:
	ProgressFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	void StartConvert(const wxString &path);

private:
	void OnExit(wxCommandEvent& event);
	virtual wxThread::ExitCode Entry();
	void OnThreadUpdate(wxThreadEvent& evt);

	wxGauge *progress;
	wxString path;
	wxString newFileName;
	wxCriticalSection critical;
	wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(ProgressFrame, wxFrame)
EVT_MENU(wxID_EXIT, ProgressFrame::OnExit)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

enum {
	PROGRESS_WINDOW = 1
};

bool MyApp::OnInit()
{
	frame = new ProgressFrame("CSV2Excel", wxPoint(50, 50), wxSize(300, 80));
	frame->Show(true);
	return wxApp::OnInit();
}

void MyApp::OnInitCmdLine(wxCmdLineParser & parser)
{
	parser.AddParam("Original CSV or TSV file", wxCMD_LINE_VAL_STRING);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser & parser)
{
	return true;
}

bool MyApp::OnCmdLineError(wxCmdLineParser & parser)
{
	wxFileDialog openFileDialog((wxWindow*)0, _("Open CSV/TSV file"), "", "", "CSV/TSV files (*.tsv,*.txt,*.csv)|"
		"*.tsv;*.txt;*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return false;
	frame->StartConvert(openFileDialog.GetPath());
	return true;
}

ProgressFrame::ProgressFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_EXIT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");

	SetMenuBar(menuBar);

	Bind(wxEVT_THREAD, &ProgressFrame::OnThreadUpdate, this);

	progress = new wxGauge(this, (int)PROGRESS_WINDOW, 100);
	progress->Pulse();

}

void ProgressFrame::StartConvert(const wxString & path)
{
	wxCriticalSectionLocker locker(critical);
	this->path = path;

	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}
	// go!
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
		return;
	}
}

void ProgressFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

wxThread::ExitCode ProgressFrame::Entry()
{
	wxFile writeFile;
	wxString basename = wxFileName::StripExtension(path);
	size_t lastComma = path.find_last_of(".");

	newFileName = wxString::Format("%s.xlsx", basename);
	
	if (!writeFile.Create(newFileName)) {
		for (int i = 0; i < 20; i++) {
			newFileName = wxString::Format("%s-%d.xlsx", basename, i+1);
			if (writeFile.Create(newFileName)) break;
		}
	}

	writeFile.Close();

	FILE *input = fopen(path.mb_str(), "r");
	if (input == NULL) {
		wxLogError("Cannot open input file");
		// raise error
		return wxThread::ExitCode(1);
	}

	if (path.EndsWith(".csv"))
		csv2xlsx(input, newFileName.mb_str());
	else
		csv2xlsx_with_delimiter_and_quote(input, newFileName.mb_str(), '\t', 0);

	fclose(input);

	wxLogInfo("done");
	wxQueueEvent(GetEventHandler(), new wxThreadEvent());
	return wxThread::ExitCode(0);
}

#include <Windows.h>

void ProgressFrame::OnThreadUpdate(wxThreadEvent & evt)
{
	wxCriticalSectionLocker locker(critical);
	wxLogInfo("thread ok!");
	ShellExecute(0, _("open"), newFileName.fn_str(), NULL, NULL, SW_SHOW);
	Close();
}

