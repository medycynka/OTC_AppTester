#pragma once

#include <wx\wx.h>
#include <wx\wfstream.h>
#include <wx\txtstrm.h>
#include <wx\wxprec.h>
#include <wx\cmdline.h>
#include <wx\process.h>
#include <wx\utils.h>

#include <exception>
#include "TestingFrame.h"

class tApp : public wxApp
{
public:
	tApp();
	~tApp();

	virtual bool OnInit();
	virtual int OnExit();
	virtual int OnRun();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	void OnForceClose(wxCloseEvent& e);

private:
	wxWindowID m_app_id;
	TestingFrame* m_main_frame;
	wxString m_config_path;
	std::unordered_map<wxString, float> m_config;
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] = {
	{wxCMD_LINE_PARAM, NULL,  NULL, "Configuration file",  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM},
	{ wxCMD_LINE_NONE }
};