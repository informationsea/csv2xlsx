// wxWidgets "Hello world" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class ProgressFrame : public wxFrame
{
public:
	ProgressFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
	void OnExit(wxCommandEvent& event);
	wxGauge *progress;

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
	ProgressFrame *frame = new ProgressFrame("CSV2Excel", wxPoint(50, 50), wxSize(300, 80));
	frame->Show(true);
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

	progress = new wxGauge(this, (int)PROGRESS_WINDOW, 100);
	progress->SetValue(20);

}

void ProgressFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

