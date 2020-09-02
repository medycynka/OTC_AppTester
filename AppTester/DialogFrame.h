#pragma once

#include <wx\wx.h>
#include "ComponentsIDS.h"


class DialogFrame : public wxFrame
{
public:
	DialogFrame(wxWindow* parent_, wxWindowID id, const wxSize& size_);
	~DialogFrame();

private:
	void OnButtonCancel(wxCommandEvent& e);
	void OnButtonSave(wxCommandEvent& e);
	void OnButtonClose(wxCommandEvent& e);

private:
	DF_IDS m_ids;
	size_t m_bit_n;
	wxWindowID m_dialog_id;
	wxTextCtrl* m_dialog_text;
	wxString m_txt;
	wxButton* m_cancel_btn;
	wxButton* m_save_btn;
	wxButton* m_close_btn;
	wxStaticBitmap** m_btm;
};