#include "DialogFrame.h"


DialogFrame::DialogFrame(wxWindow* parent_, wxWindowID id, const wxSize& size_) : wxFrame(parent_, id, "Simple dialog " + std::to_string(id), wxPoint(25, 50), size_, wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT | wxDEFAULT_FRAME_STYLE), 
	m_ids(id, std::floor(size_.x / 25)), m_bit_n(std::floor(size_.x / 25)), m_dialog_id(id),
	m_txt("Lorem ipsum dolor sit amet, consectetur adipiscing elit.Nam euismod ac velit in volutpat. Integer consequat nisi quis sapien congue finibus. Aenean porta rhoncus auctor. Phasellus odio elit, lobortis sed lectus et, accumsan accumsan leo. Proin eget odio mi. Aenean id augue interdum, lobortis justo ut, pulvinar lorem. Phasellus a mi quis lectus ultricies ultricies.\nAliquam erat volutpat.Quisque condimentum gravida purus, ultrices auctor ipsum maximus nec.Donec quam lacus, vestibulum sed sem nec, iaculis rutrum arcu. Nulla ut ultrices quam.Pellentesque accumsan arcu sapien, non tristique nunc iaculis et. Nullam ullamcorper, justo in scelerisque viverra, purus elit consectetur risus, ut molestie dolor metus in risus.")
{
	// Add text
	m_dialog_text = new wxTextCtrl(this, m_ids.DIALOG_TXT_ID, m_txt, wxDefaultPosition, wxSize(GetSize().x - 20, GetSize().y / 2 - 20), wxTE_MULTILINE | wxTE_RICH);

	//Add bitmaps
	wxBitmap m_bit_map("dialog_map.bmp", wxBITMAP_TYPE_BMP);
	m_btm = new wxStaticBitmap * [2 * m_bit_n];
	for (auto i = 0; i < m_bit_n; ++i) {
		m_btm[i] = new wxStaticBitmap(this, m_ids.DIALOG_BIT_MAP_IDS[i], m_bit_map, wxPoint(i * 25, GetSize().y / 2 - 15), wxSize(25, 25));
		m_btm[i + m_bit_n] = new wxStaticBitmap(this, m_ids.DIALOG_BIT_MAP_IDS[m_bit_n + i], m_bit_map, wxPoint(i * 25, GetSize().y / 2 + 10), wxSize(25, 25));
	}

	// Add buttons
	m_cancel_btn = new wxButton(this, m_ids.DIALOG_CANCEL_ID, "Cancel", wxPoint(5, 2 * GetSize().y / 3), wxSize(GetSize().x / 3 - 15, GetSize().y / 6));
	m_cancel_btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DialogFrame::OnButtonCancel, this);
	m_save_btn = new wxButton(this, m_ids.DIALOG_SAVE_ID, "Save", wxPoint(GetSize().x / 3, 2 * GetSize().y / 3), wxSize(GetSize().x / 3 - 15, GetSize().y / 6));
	m_save_btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DialogFrame::OnButtonSave, this);
	m_close_btn = new wxButton(this, m_ids.DIALOG_CLOSE_ID, "Close", wxPoint(2 * GetSize().x / 3 - 5, 2 * GetSize().y / 3), wxSize(GetSize().x / 3 - 15, GetSize().y / 6));
	m_close_btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &DialogFrame::OnButtonClose, this);
}

DialogFrame::~DialogFrame()
{
	delete[] m_btm;
}

void DialogFrame::OnButtonCancel(wxCommandEvent& e)
{
	e.Skip();
}

void DialogFrame::OnButtonSave(wxCommandEvent& e)
{
	e.Skip();
}

void DialogFrame::OnButtonClose(wxCommandEvent& e)
{
	this->Destroy();
}