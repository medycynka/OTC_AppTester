#pragma once

#include <wx\wx.h>
#include <wx\notebook.h>
#include <wx\listctrl.h>
#include <wx\stattext.h>
#include <wx\datectrl.h>
#include <wx\fontpicker.h>
#include <wx\filepicker.h>
#include <wx\dataview.h>
#include <wx\combo.h>
#include <wx\timer.h>
#include <wx\statusbr.h>
#include <wx\spinctrl.h>
#include <wx\calctrl.h>
#include <wx\filectrl.h>
#include <wx\textfile.h>
#include <wx\tglbtn.h>
#include <wx\timer.h>
#include <wx\datetime.h>
#include <random>
#include <Windows.h>
#include <stdio.h>
#include "CpuUsageReader.h"
#include "DialogFrame.h"
#include "TimerQueue.h"
#include "ComponentsIDS.h"


class TestingFrame : public wxFrame
{
public:
	TestingFrame(wxWindow* parent_, wxWindowID id, std::unordered_map<wxString, float>& config, const wxString& desc, const wxString& out_path);
	~TestingFrame();

private:
	// Standard events handlers
	void OnClose(wxCloseEvent& e);
	void OnActivate(wxActivateEvent& e);
	void OnFocusSet(wxFocusEvent& e);
	void OnMenuClick(wxCommandEvent& e);
	void OnMenuExit(wxCommandEvent& e);
	void OnButtonRefresh(wxCommandEvent& e);
	void OnCompassClick(wxCommandEvent& e);
	void OnGaugeInc(wxCommandEvent& e);
	void OnGaugeDec(wxCommandEvent& e);
	void OnLimiterTick(wxTimerEvent& e);
	void OnCpuCpunterTick(wxTimerEvent& e);

	// Simulation functions
	void M_ZOrder();
	void M_SeqShow();
	void M_SeqHide();
	void M_Refresh();
	void M_TabChange();
	void M_ButtonPress();
	void M_Scroll();
	void M_SelectList();
	void M_CheckList();
	void M_Write();
	void M_RadioList();
	void M_GaugeFill();
	void M_CountCpuUsage();
	void M_StartTest();

public:
	void StartTimers(); // should be called from main thread
	void StartTest();
	void ClearTimers();
	void StopTimers();
	void RepaintDialogs();
	long long CalculateTotalIndicator();
	unsigned long long CalculateActualIndicator();
	float GetWeights();
	void WriteResults();
	void ForceCloseDialogs();
	void ShutDown();

private:
	TF_IDS m_ids;
	std::unordered_map<wxString, float> m_config;
	std::unordered_map<wxString, unsigned long> m_result;
	std::unordered_map<wxString, float> m_weights;
	wxTimer m_limiter;
	wxTimer m_cpu_counter;
	wxLongLongNative m_err;
	TimerQueue m_tq;
	std::vector<TimerQueue::id_type> m_t_ids;
	unsigned long m_cpu_measure_frq;
	unsigned long long m_cpu_measure_count;
	CpuUsageReader m_cpu_reader;
	float m_accumulate_cpu_usage;
	wxWindowID frame_id;
	int width;
	int height;
	wxToolBar* m_tool_bar;
	wxMenuBar* m_menu_bar;
	wxGridSizer* m_grid;
	wxNotebook* m_note;
	std::vector<wxButton*> m_compass;
	wxTextCtrl* m_board;
	wxWindow* m_top_right;
	wxWindow* m_bottom_left;
	wxWindow* m_bottom_right;
	wxTextCtrl* m_txt;
	wxString m_lorem;
	std::vector<wxToggleButton*> m_main_btn;
	wxListBox* m_checker;
	wxTextCtrl* m_report_txt;
	wxString m_rep_txt;
	wxCheckListBox* m_task_check;
	wxRadioBox* m_radio;
	wxGauge* m_gauge;
	wxStaticText* m_gauge_lvl;
	std::vector<DialogFrame*> m_dialogs;
	std::vector<DialogFrame*>::size_type m_curr_it;
	bool m_scroll_down;
	bool m_inc_gauge;
	wxString m_description;
	wxString m_output_path;
	bool m_write_or_not;
};