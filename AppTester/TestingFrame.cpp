#include "TestingFrame.h"


TestingFrame::TestingFrame(wxWindow* parent_, wxWindowID id, std::unordered_map<wxString, float>& config, const wxString& desc, const wxString& out_path)
	: wxFrame(parent_, id, "Format", wxPoint(0, 0), wxSize(config["w_width"], config["w_height"])),
	m_config(config), frame_id(id), m_cpu_reader(GetCurrentProcessId()), m_description(desc), m_ids(id, 20), m_output_path(out_path), m_limiter(this, m_ids.LIMITER_ID), m_cpu_counter(this, m_ids.CPU_COUNTER_ID),
	width(config["w_width"]), height(config["w_height"]), m_err(50), m_cpu_measure_frq(250), m_cpu_measure_count(0), m_accumulate_cpu_usage(0), m_rep_txt("Testing...\n"),
	m_lorem("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam euismod ac velit in volutpat. Integer consequat nisi quis sapien congue finibus. Aenean porta rhoncus auctor. Phasellus odio elit, lobortis sed lectus et, accumsan accumsan leo. Proin eget odio mi. Aenean id augue interdum, lobortis justo ut, pulvinar lorem. Phasellus a mi quis lectus ultricies ultricies.\nAliquam erat volutpat.Quisque condimentum gravida purus, ultrices auctor ipsum maximus nec.Donec quam lacus, vestibulum sed sem nec, iaculis rutrum arcu.Nulla ut ultrices quam.Pellentesque accumsan arcu sapien, non tristique nunc iaculis et.Nullam ullamcorper, justo in scelerisque viverra, purus elit consectetur risus, ut molestie dolor metus in risus.Vivamus ultrices enim sit amet velit viverra varius in ac lorem.Fusce velit sem, egestas in sodales nec, lobortis nec nulla.Etiam quis mi nec justo vulputate tincidunt.\n 		Lorem ipsum dolor sit amet, consectetur adipiscing elit.Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus.Donec eu augue molestie, vestibulum dui quis, aliquam leo.Donec eleifend ligula nec metus volutpat malesuada.Ut vehicula massa dui, at lobortis libero imperdiet vel.Mauris tristique lorem sodales massa elementum, eget mattis odio maximus.Vivamus tincidunt, justo id eleifend malesuada, nisl justo consequat ligula, vel vehicula sapien arcu mattis sem.Donec id lacinia justo, at sagittis ante.Nullam accumsan consequat enim vel facilisis.Fusce feugiat nibh in urna molestie tincidunt.Ut porttitor justo nec auctor elementum.\n 		Nam consectetur lobortis massa, non euismod risus mattis blandit.Mauris cursus diam dui, id molestie dui aliquam non.Fusce vitae molestie orci.Pellentesque maximus nec orci at fringilla.Ut porta leo eget augue congue, vel aliquam velit venenatis.Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas.Pellentesque vitae ipsum dolor.Phasellus a ornare est, a rutrum sapien.Cras et turpis lacus."),
	m_curr_it(0), m_scroll_down(true), m_inc_gauge(true), m_write_or_not(true)
{
	// Setting starting results
	m_result["z-order"] = 0;
	m_result["seq"] = 0;
	m_result["refresh"] = 0;
	m_result["tab"] = 0;
	m_result["button"] = 0;
	m_result["scroll"] = 0;
	m_result["select"] = 0;
	m_result["check"] = 0;
	m_result["write"] = 0;
	m_result["radio"] = 0;
	m_result["gauge"] = 0;

	// Setting weights
	m_weights["z-order"] = 1;
	m_weights["seq"] = 1;
	m_weights["refresh"] = 1;
	m_weights["tab"] = 1;
	m_weights["button"] = 1;
	m_weights["scroll"] = 1;
	m_weights["select"] = 1;
	m_weights["check"] = 1;
	m_weights["write"] = 1;
	m_weights["radio"] = 1;
	m_weights["gauge"] = 1;

	Connect(id, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TestingFrame::OnClose), NULL, this);
	Connect(m_ids.LIMITER_ID, wxEVT_TIMER, wxTimerEventHandler(TestingFrame::OnLimiterTick), NULL, this);
	Connect(m_ids.CPU_COUNTER_ID, wxEVT_TIMER, wxTimerEventHandler(TestingFrame::OnCpuCpunterTick), NULL, this);

	// Menu bar
	m_menu_bar = new wxMenuBar();
	this->SetMenuBar(m_menu_bar);
	{
		// Menu buttons
		wxMenu* m_menuList = new wxMenu();
		m_menuList->Append(m_ids.MENU_NEW_ID, "New");
		m_menuList->Append(m_ids.MENU_OPEN_ID, "Open");
		m_menuList->Append(m_ids.MENU_SAVE_ID, "Save");
		m_menuList->Append(m_ids.MENU_PRINT_ID, "Print");

		wxMenu* m_settings_menu = new wxMenu();
		m_settings_menu->Append(m_ids.MENU_ACC_ID, "Account");
		m_settings_menu->Append(m_ids.MENU_OPT_ID, "Options");
		m_settings_menu->Append(m_ids.MENU_CUSTOM_ID, "Cusomize");
		m_menuList->AppendSubMenu(m_settings_menu, "Settings");

		m_menuList->Append(m_ids.MENU_EXIT_ID, "Exit");
		m_menu_bar->Append(m_menuList, "File");

		wxMenu* m_menuHelp = new wxMenu();
		m_menuHelp->Append(m_ids.MENU_HELP_ID, "View help");
		m_menuHelp->Append(m_ids.MENU_FEEDBACK_ID, "Send feedback");
		m_menuHelp->Append(m_ids.MENU_UPDATES_ID, "Check for updates");
		m_menu_bar->Append(m_menuHelp, "Help");

		// Binding menu buttons to functions
		Connect(m_ids.MENU_NEW_ID, m_ids.MENU_CUSTOM_ID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TestingFrame::OnMenuClick), NULL, this);
		Connect(m_ids.MENU_HELP_ID, m_ids.MENU_UPDATES_ID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TestingFrame::OnMenuClick), NULL, this);
		Connect(m_ids.MENU_EXIT_ID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TestingFrame::OnMenuExit), NULL, this);
	}
	// Add toolbar
	m_tool_bar = this->CreateToolBar(wxTB_HORIZONTAL, m_ids.TOOLBAR_ID);
	wxButton* m_btn_refresh = new wxButton(m_tool_bar, m_ids.REFRESH_BTN_ID, "Refresh");
	m_btn_refresh->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TestingFrame::OnButtonRefresh, this);
	m_tool_bar->AddControl(m_btn_refresh);
	m_tool_bar->Realize();

	// Add grid layout
	m_grid = new wxGridSizer(2, 2, 10, 10);

	// Top-left part
	m_note = new wxNotebook(this, m_ids.NOTEBOOK_ID, wxDefaultPosition, wxSize(width / 2, height / 2));

	// Buttons & editors page
	wxNotebookPage* m_be_page = new wxNotebookPage(m_note, m_ids.NB_PAGE_1_ID, wxDefaultPosition, wxSize(width / 2, height / 2), 0L, "Test content");
	m_compass.push_back(new wxButton(m_be_page, m_ids.NW_BTN_ID, "NW", wxPoint(0, 0), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.NNW_BTN_ID, "NNW", wxPoint(width / 10 - 2, 0), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.N_BTN_ID, "N", wxPoint(width / 5 - 4, 0), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.NNE_BTN_ID, "NNE", wxPoint(3 * width / 10 - 6, 0), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.NE_BTN_ID, "NE", wxPoint(4 * width / 10 - 8, 0), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.NEE_BTN_ID, "NEE", wxPoint(4 * width / 10 - 8, height / 10 - 8), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.E_BTN_ID, "E", wxPoint(4 * width / 10 - 8, height / 5 - 16), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SEE_BTN_ID, "SEE", wxPoint(4 * width / 10 - 8, 3 * height / 10 - 24), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SE_BTN_ID, "SE", wxPoint(4 * width / 10 - 8, 4 * height / 10 - 32), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SSE_BTN_ID, "SSE", wxPoint(3 * width / 10 - 6, 4 * height / 10 - 32), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.S_BTN_ID, "S", wxPoint(width / 5 - 4, 4 * height / 10 - 32), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SSW_BTN_ID, "SSW", wxPoint(width / 10 - 2, 4 * height / 10 - 32), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SW_BTN_ID, "SW", wxPoint(0, 4 * height / 10 - 32), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.SWW_BTN_ID, "SWW", wxPoint(0, 3 * height / 10 - 24), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.W_BTN_ID, "W", wxPoint(0, height / 5 - 16), wxSize(width / 10 - 2, height / 10 - 8)));
	m_compass.push_back(new wxButton(m_be_page, m_ids.NWW_BTN_ID, "NWW", wxPoint(0, height / 10 - 8), wxSize(width / 10 - 2, height / 10 - 8)));

	Connect(m_ids.NW_BTN_ID, m_ids.NWW_BTN_ID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TestingFrame::OnCompassClick), NULL, this);


	m_board = new wxTextCtrl(m_be_page, m_ids.COMPASS_TXT_ID, "", wxPoint(width / 10 - 2, height / 10 - 8), wxSize(3 * width / 10 - 6, 3 * height / 10 - 24), wxTE_MULTILINE | wxTE_RICH);
	m_note->InsertPage(0, m_be_page, "Buttons and edits", true);

	// Lists page
	wxNotebookPage* m_li_page = new wxNotebookPage(m_note, m_ids.NB_PAGE_2_ID, wxDefaultPosition, wxSize(width / 2, height / 2), 0L, "Test content");

	wxListBox* m_page_list_box = new wxListBox(m_li_page, m_ids.PAGE_LIST_ID, wxPoint(0, 0), wxSize(width / 4 - 4, height / 4 - 2));
	wxCheckListBox* m_page_check = new wxCheckListBox(m_li_page, m_ids.PAGE_CHECK_ID, wxPoint(width / 4, 0), wxSize(width / 4 - 8, height / 4 - 2));

	wxArrayString p_arr;
	for (auto i = 0; i < 40; ++i) {
		p_arr.Add("radio_" + std::to_string(i + 1));
	}
	wxRadioBox* m_page_radio = new wxRadioBox(m_li_page, m_ids.PAGE_RADIO_ID, "Radio box", wxPoint(0, height / 4), wxSize(width / 2 - 4, height / 5), p_arr, 8);

	m_note->InsertPage(1, m_li_page, "\tLists\t", false);

	// Boxes Page
	wxNotebookPage* m_bo_page = new wxNotebookPage(m_note, m_ids.NB_PAGE_3_ID, wxDefaultPosition, wxSize(width / 2, height / 2), 0L, "Test content");

	wxChoice* pg_choice_box = new wxChoice(m_bo_page, m_ids.PAGE_CHOICE_ID, wxPoint(0, 0), wxSize(width / 6 - 5, 40));
	wxComboBox* pg_combo_box = new wxComboBox(m_bo_page, m_ids.PAGE_COMBO_ID, "Choose one again", wxPoint(width / 6, 0), wxSize(width / 6 - 5, 40));
	wxSpinCtrl* pg_spin = new wxSpinCtrl(m_bo_page, m_ids.PAGE_SPIN_ID, "", wxPoint(width / 3, 0), wxSize(width / 6 - 5, 20), 16384L, -1000, 1000, 0);
	wxCalendarCtrl* pg_calendar = new wxCalendarCtrl(m_bo_page, m_ids.PAGE_CALENDAR_ID, wxDefaultDateTime, wxPoint(0, 5 * height / 64), wxSize(width / 6 - 5, 3 * height / 8 - 5));
	wxFileCtrl* pf_file_ctrl = new wxFileCtrl(m_bo_page, m_ids.PAGE_FILE_CTRL_ID, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, wxFC_MULTIPLE, wxPoint(width / 6, 5 * height / 64));

	m_note->InsertPage(2, m_bo_page, "\tBoxes\t", false);
	m_grid->Add(m_note, wxEXPAND | wxALL);

	// Top-right-part
	m_top_right = new wxWindow(this, m_ids.TOP_RIGHT_ID);

	// Add text field
	m_txt = new wxTextCtrl(m_top_right, m_ids.TR_TXT_ID, m_lorem, wxPoint(10, 0), wxSize(width / 8, height / 2), wxTE_MULTILINE | wxTE_RICH);

	// Add buttons
	m_main_btn.resize(20);
	for (auto i = 0; i < 10; ++i) {
		m_main_btn[i] = new wxToggleButton(m_top_right, m_ids.MAIN_BTN_IDS[i], "Clicker " + std::to_string(i + 1), wxPoint(5 * width / 32, i * (height / 20)), wxSize(width / 16, height / 20));
		m_main_btn[i + 10] = new wxToggleButton(m_top_right, m_ids.MAIN_BTN_IDS[i + 10], "Clicker " + std::to_string(i + 11), wxPoint(7 * width / 32, i * (height / 20)), wxSize(width / 16, height / 20));
	}

	// Add check list
	m_checker = new wxListBox(m_top_right, m_ids.TR_LIST_ID, wxPoint(41 * width / 128, 0), wxSize(11 * width / 64, height / 2));

	m_grid->Add(m_top_right, wxEXPAND | wxALL);

	// Bottom-left part
	m_bottom_left = new wxWindow(this, m_ids.BOTTOM_LEFT_ID);

	// Add report table
	wxListCtrl* m_table = new wxListCtrl(m_bottom_left, frame_id + 301, wxPoint(0, 45), wxSize(width / 4, height / 5 - 10), wxLC_REPORT, wxDefaultValidator, "Report table");
	{
		wxListItem col0, col1, col2, col3, col4;
		col0.SetId(0);
		col0.SetText("ID");
		m_table->InsertColumn(0, col0);
		col1.SetId(1);
		col1.SetText("Item");
		m_table->InsertColumn(1, col1);
		col2.SetId(2);
		col2.SetText("Price");
		m_table->InsertColumn(2, col2);
		col3.SetId(3);
		col3.SetText("Description");
		m_table->InsertColumn(3, col3);
		col4.SetId(2);
		col4.SetText("Serial number");
		col4.SetWidth(250);
		m_table->InsertColumn(4, col4);
	}

	// Add Report writer
	wxStaticText* m_report = new wxStaticText(m_bottom_left, m_ids.REPORT_TITLE_ID, "Write a report", wxPoint(17 * width / 64, 45));
	wxDatePickerCtrl* m_date = new wxDatePickerCtrl(m_bottom_left, m_ids.REPORT_DATE_ID, wxDefaultDateTime, wxPoint(width / 4 + 20, 65), wxSize(100, 30));
	wxFontPickerCtrl* m_font = new wxFontPickerCtrl(m_bottom_left, m_ids.REPORT_FONT_ID, wxNullFont, wxPoint(width / 4 + 140, 65), wxSize(100, 30));
	m_report_txt = new wxTextCtrl(m_bottom_left, m_ids.REPORT_TXT_FIELD_ID, "", wxPoint(width / 4 + 20, 100), wxSize(width / 4 - 30, height / 5), wxTE_MULTILINE);
	wxButton* m_btn_save = new wxButton(m_bottom_left, m_ids.REPORT_SAVE_ID, "Save", wxPoint(35 * width / 128, 11 * height / 32), wxSize(9 * width / 128, 30));
	wxButton* m_btn_print = new wxButton(m_bottom_left, m_ids.REPORT_PRINT_ID, "Print", wxPoint(89 * width / 256, 11 * height / 32), wxSize(9 * width / 128, 30));
	wxButton* m_btn_send = new wxButton(m_bottom_left, m_ids.REPORT_SEND_ID, "Send", wxPoint(27 * width / 64, 11 * height / 32), wxSize(9 * width / 128, 30));

	//Add check list
	m_task_check = new wxCheckListBox(m_bottom_left, m_ids.REPORT_TASKS_ID, wxPoint(0, height / 4), wxSize(width / 4, height / 6));

	m_grid->Add(m_bottom_left, wxEXPAND | wxALL);

	// Bottom-right part
	m_bottom_right = new wxWindow(this, m_ids.BOTTOM_RIGHT_ID);

	// Add combo box
	wxComboBox* m_cmb_box = new wxComboBox(m_bottom_right, m_ids.BR_CMB_BOX_ID, "Combo box", wxPoint(0, 50), wxSize(200, 200));

	// Add radio box
	wxArrayString arr;
	for (auto i = 0; i < 24; ++i) {
		arr.Add("rad_" + std::to_string(i + 1));
	}
	m_radio = new wxRadioBox(m_bottom_right, m_ids.BR_RADIO_BOX_ID, "Radio box", wxPoint(0, 100), wxSize(200, 200), arr, 3);

	// Add gauge
	m_gauge = new wxGauge(m_bottom_right, m_ids.BR_GAUGE_ID, 100, wxPoint(width / 5, 50), wxSize(width / 6, 40));
	m_gauge->SetValue(0);

	wxButton* m_inc_gauge_btn = new wxButton(m_bottom_right, m_ids.BR_INC_BTN_ID, "Gauge +1", wxPoint(width / 5, 100), wxSize(width / 12, 30));
	m_inc_gauge_btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TestingFrame::OnGaugeInc, this);
	wxButton* m_dec_gauge_btn = new wxButton(m_bottom_right, m_ids.BR_DEC_BTN_ID, "Gauge -1", wxPoint(17 * width / 60, 100), wxSize(width / 12, 30));
	m_dec_gauge_btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &TestingFrame::OnGaugeDec, this);
	wxStaticText* gauge_txt = new wxStaticText(m_bottom_right, m_ids.BR_GTXT_ID, "Current gauge lvl:\t\t\t", wxPoint(23 * width / 60, 50));
	m_gauge_lvl = new wxStaticText(m_bottom_right, frame_id + 407, "0%", wxPoint(23 * width / 60, 70), wxSize(9 * width / 128 + 3, 40), wxALIGN_RIGHT);
	m_gauge_lvl->SetFont(wxFont(wxFontInfo(12)));

	// Add file/dir picker
	wxFilePickerCtrl* m_file_picker = new wxFilePickerCtrl(m_bottom_right, m_ids.BR_FILE_PICKER_ID, "", "File picker", "", wxPoint(width / 5, 5 * width / 32), wxSize(width / 4 - 10, 30));
	wxDirPickerCtrl* m_dir_picker = new wxDirPickerCtrl(m_bottom_right, m_ids.BR_DIR_PICKER_ID, "", "Dir picker", wxPoint(width / 5, 3 * width / 16), wxSize(width / 4 - 10, 30));

	m_grid->Add(m_bottom_right, wxEXPAND | wxALL);

	// Filling constrols
	for (auto i = 0; i < 16; ++i) {
		m_page_list_box->Append("list_box_item_" + std::to_string(i + 1));
		m_page_check->Append("check_list_choise_" + std::to_string(i + 1));
		m_checker->Append("Item_" + std::to_string(i + 1));
	}
	for (auto i = 0; i < 10; ++i) {
		auto item_index = m_table->InsertItem(i, std::to_string(i));
		m_table->SetItem(item_index, 1, "item_" + std::to_string(i + 1));
		m_table->SetItem(item_index, 2, "price $");
		m_table->SetItem(item_index, 3, "description");
		m_table->SetItem(item_index, 4, "5gnjafs_serial_number_abc" + std::to_string(i + 1));

		m_task_check->Append("Task_" + std::to_string(i + 1));
		m_cmb_box->Append("choice_" + std::to_string(i + 1));

		pg_choice_box->Append("Choice " + std::to_string(i + 1));
		pg_combo_box->Append("Combo choice " + std::to_string(i + 1));
	}

	this->SetSizer(m_grid);
	m_grid->Layout();

	this->Bind(wxEVT_ACTIVATE, &TestingFrame::OnActivate, this);
	this->Bind(wxEVT_ACTIVATE_APP, &TestingFrame::OnActivate, this);
	this->Bind(wxEVT_SET_FOCUS, &TestingFrame::OnFocusSet, this);
	this->Bind(wxEVT_KILL_FOCUS, &TestingFrame::OnFocusSet, this);

	Show(true);

	// Creating dialogs
	if (m_config["dialogs_count"] > 0 && (m_config["z-order_count"] > 0 || m_config["showdialog_count"] > 0)) {
		for (auto i = 0; i < m_config["dialogs_count"]; ++i) {
			m_dialogs.push_back(new DialogFrame(this, frame_id + 1000 + (i + 1) * 100, wxSize(config["w_width"] / 3, config["w_height"] / 3)));

			if (m_config["z-order_count"] > 0) {
				m_dialogs[i]->Show(true);
			}
		}
	}

	Refresh();
	Update();
}

TestingFrame::~TestingFrame()
{
	delete m_note;
	delete m_top_right;
	delete m_bottom_left;
	delete m_bottom_right;
}

void TestingFrame::OnClose(wxCloseEvent& e)
{
	StopTimers();
	ForceCloseDialogs();
	ClearTimers();

	this->Destroy();
}

void TestingFrame::OnActivate(wxActivateEvent& e)
{
	e.Skip();
}

void TestingFrame::OnFocusSet(wxFocusEvent& e)
{
	e.Skip();
}

void TestingFrame::OnMenuClick(wxCommandEvent& e)
{
	e.Skip();
}

void TestingFrame::OnMenuExit(wxCommandEvent& e)
{
	ShutDown();

	e.Skip();
}

void TestingFrame::OnButtonRefresh(wxCommandEvent& e)
{
	this->Refresh();
	e.Skip();
}

void TestingFrame::OnCompassClick(wxCommandEvent& e)
{
	wxWindowID id_ = e.GetId();

	if (id_ == m_ids.NW_BTN_ID) {
		m_board->AppendText("NW, ");
	}
	else if (id_ == m_ids.NNW_BTN_ID) {
		m_board->AppendText("NNW, ");
	}
	else if (id_ == m_ids.N_BTN_ID) {
		m_board->AppendText("N, ");
	}
	else if (id_ == m_ids.NNE_BTN_ID) {
		m_board->AppendText("NNE, ");
	}
	else if (id_ == m_ids.NE_BTN_ID) {
		m_board->AppendText("NE, ");
	}
	else if (id_ == m_ids.NEE_BTN_ID) {
		m_board->AppendText("NEE, ");
	}
	else if (id_ == m_ids.E_BTN_ID) {
		m_board->AppendText("E, ");
	}
	else if (id_ == m_ids.SEE_BTN_ID) {
		m_board->AppendText("SEE, ");
	}
	else if (id_ == m_ids.SE_BTN_ID) {
		m_board->AppendText("SE, ");
	}
	else if (id_ == m_ids.SSE_BTN_ID) {
		m_board->AppendText("SSE, ");
	}
	else if (id_ == m_ids.S_BTN_ID) {
		m_board->AppendText("S, ");
	}
	else if (id_ == m_ids.SSW_BTN_ID) {
		m_board->AppendText("SSW, ");
	}
	else if (id_ == m_ids.SW_BTN_ID) {
		m_board->AppendText("SW, ");
	}
	else if (id_ == m_ids.SWW_BTN_ID) {
		m_board->AppendText("SWW, ");
	}
	else if (id_ == m_ids.W_BTN_ID) {
		m_board->AppendText("W, ");
	}
	else if (id_ == m_ids.NWW_BTN_ID) {
		m_board->AppendText("NWW, ");
	}

	e.Skip();
}

void TestingFrame::OnGaugeInc(wxCommandEvent& e)
{
	if (m_gauge->GetValue() >= 0 && m_gauge->GetValue() < 100) {
		m_gauge->SetValue(m_gauge->GetValue() + 1);
		m_gauge_lvl->SetLabel(wxString::Format(wxT("%d"), m_gauge->GetValue()) + "%");
	}
	else {
		m_gauge->SetValue(0);
		m_gauge_lvl->SetLabel(wxString::Format(wxT("%d"), m_gauge->GetValue()) + "%");
	}

	e.Skip();
}

void TestingFrame::OnGaugeDec(wxCommandEvent& e)
{
	if (m_gauge->GetValue() > 0) {
		m_gauge->SetValue(m_gauge->GetValue() - 1);
		m_gauge_lvl->SetLabel(wxString::Format(wxT("%d"), m_gauge->GetValue()) + "%");
	}
	else {
		m_gauge->SetValue(100);
		m_gauge_lvl->SetLabel(wxString::Format(wxT("%d"), m_gauge->GetValue()) + "%");
	}

	e.Skip();
}

void TestingFrame::OnLimiterTick(wxTimerEvent& e)
{
	WriteResults();
	e.Skip();
}

void TestingFrame::OnCpuCpunterTick(wxTimerEvent& e)
{
	M_CountCpuUsage();
	e.Skip();
}

void TestingFrame::M_ZOrder()
{
	if (m_result["z-order"] < m_config["z-order_count"]) {
		if (m_curr_it == m_config["dialogs_count"]) {
			m_curr_it = 0;
		}

		if (!m_dialogs.empty()) {
			m_dialogs[m_curr_it]->Raise();

			m_curr_it++;

			m_result["z-order"]++;
		}
	}
}

void TestingFrame::M_SeqShow()
{
	if (m_result["seq"] < m_config["showdialog_count"]) {
		if (m_curr_it == m_config["dialogs_count"]) {
			m_curr_it = 0;
		}

		if (!m_dialogs.empty()) {
			m_dialogs[m_curr_it]->Show(true);
			m_dialogs[m_curr_it]->Layout();
			m_dialogs[m_curr_it]->Update();

			m_curr_it++;
			m_result["seq"]++;
		}
	}
}

void TestingFrame::M_SeqHide()
{
	if (!m_dialogs.empty()) {
		m_dialogs[m_curr_it - 1]->Show(false);
	}
}

void TestingFrame::M_Refresh()
{
	if (m_result["refresh"] < m_config["refresh_count"]) {
		this->Refresh();
		this->Update();
		m_result["refresh"]++;
	}
}

void TestingFrame::M_TabChange()
{
	if (m_result["tab"] < m_config["tab_change_count"]) {
		m_note->ChangeSelection(m_result["tab"] % 3);

		m_result["tab"]++;
	}
}

void TestingFrame::M_ButtonPress()
{
	if (m_result["button"] < m_config["button_press_count"]) {
		m_main_btn[2]->SetValue(!m_main_btn[2]->GetValue());
		m_result["button"]++;
	}
}

void TestingFrame::M_Scroll()
{
	if (m_result["scroll"] < m_config["scroll_count"]) {
		if (m_scroll_down) {
			m_txt->ScrollLines(1);

			if (m_txt->GetScrollPos(wxVERTICAL) >= m_txt->GetScrollRange(wxVERTICAL) / 2) {
				m_scroll_down = false;
			}
		}
		else {
			m_txt->ScrollLines(-1);

			if (m_txt->GetScrollPos(wxVERTICAL) <= 2) {
				m_scroll_down = true;
			}
		}

		m_result["scroll"]++;
	}
}

void TestingFrame::M_SelectList()
{
	if (m_result["select"] < m_config["select_list_count"]) {
		m_checker->Select(5 + m_result["select"] % 5);
		m_result["select"]++;
	}
}

void TestingFrame::M_CheckList()
{
	if (m_result["check"] < m_config["check_list_count"]) {
		if (m_result["check"] % 2 == 0) {
			m_task_check->Check(2, true);
			m_task_check->Check(5, false);
		}
		else {
			m_task_check->Check(2, false);
			m_task_check->Check(5, true);
		}

		m_result["check"]++;
	}
}

void TestingFrame::M_Write()
{
	if (m_result["write"] < m_config["write_count"]) {
		if (m_result["write"] % 5 == 0) {
			m_report_txt->ChangeValue(m_rep_txt);
		}
		else {
			m_report_txt->ChangeValue(m_report_txt->GetValue() + m_rep_txt);
		}

		m_result["write"]++;
	}
}

void TestingFrame::M_RadioList()
{
	if (m_result["radio"] < m_config["radio_list_count"]) {
		m_radio->Select(5 + m_result["radio"] % 5);
		m_result["radio"]++;
	}
}

void TestingFrame::M_GaugeFill()
{
	if (m_result["gauge"] < m_config["gauge_count"]) {
		if (m_inc_gauge) {
			m_gauge->SetValue(m_gauge->GetValue() + 1);

			if (m_gauge->GetValue() == 100) {
				m_inc_gauge = false;
			}
		}
		else {
			m_gauge->SetValue(m_gauge->GetValue() - 1);

			if (m_gauge->GetValue() == 0) {
				m_inc_gauge = true;
			}
		}

		m_gauge_lvl->SetLabel(wxString::Format(wxT("%d"), m_gauge->GetValue()) + "%");
		m_result["gauge"]++;
	}
}

void TestingFrame::M_CountCpuUsage()
{
	m_accumulate_cpu_usage += m_cpu_reader.GetCpuUsage();
	m_cpu_measure_count++;
}

void TestingFrame::M_StartTest()
{
	Refresh();
	Update();

	{
		auto start_point = m_err.GetValue();

		if (m_config["dialogs_count"] > 0) {
			if (m_config["z-order_count"] > 0) {
				m_t_ids.push_back(m_tq.create(start_point, m_config["z-order_frq"], std::bind(&TestingFrame::M_ZOrder, this)));
			}

			if (m_config["showdialog_count"] > 0) {
				m_t_ids.push_back(m_tq.create(start_point, m_config["showdialog_frq"], std::bind(&TestingFrame::M_SeqShow, this)));
				m_t_ids.push_back(m_tq.create(start_point + m_config["showdialog_frq"] / 2 > 5000 ? 5000 : m_config["showdialog_frq"] / 2, m_config["showdialog_frq"], std::bind(&TestingFrame::M_SeqHide, this)));
			}

			start_point += m_err.GetValue();
		}

		if (m_config["refresh_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["refresh_frq"], std::bind(&TestingFrame::M_Refresh, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["tab_change_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["tab_change_frq"], std::bind(&TestingFrame::M_TabChange, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["button_press_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["button_press_frq"], std::bind(&TestingFrame::M_ButtonPress, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["scroll_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["scroll_frq"], std::bind(&TestingFrame::M_Scroll, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["select_list_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["select_list_frq"], std::bind(&TestingFrame::M_SelectList, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["check_list_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["check_list_frq"], std::bind(&TestingFrame::M_CheckList, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["write_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["write_frq"], std::bind(&TestingFrame::M_Write, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["radio_list_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["radio_list_frq"], std::bind(&TestingFrame::M_RadioList, this)));
			start_point += m_err.GetValue();
		}

		if (m_config["gauge_count"] > 0) {
			m_t_ids.push_back(m_tq.create(start_point, m_config["gauge_frq"], std::bind(&TestingFrame::M_GaugeFill, this)));
			start_point += m_err.GetValue();
		}
	}
}

void TestingFrame::StartTimers()
{
	m_limiter.StartOnce(m_config["time_limit"]);
	m_cpu_counter.Start(m_cpu_measure_frq);
}

void TestingFrame::StartTest()
{
	StartTimers();
	M_StartTest();
}

void TestingFrame::ClearTimers()
{
	if (m_t_ids.size() > 0) {
		for (size_t i = 0; i < m_t_ids.size(); ++i) {
			m_tq.destroy(m_t_ids[i]);
		}

		m_t_ids.clear();
	}
}

void TestingFrame::StopTimers()
{
	if (m_limiter.IsRunning()) {
		m_limiter.Stop();
	}
	if (m_cpu_counter.IsRunning()) {
		m_cpu_counter.Stop();
	}
}

void TestingFrame::RepaintDialogs()
{
	for (auto& e : m_dialogs) {
		e->Layout();
		e->Update();
	}
}

long long TestingFrame::CalculateTotalIndicator()
{
	long long sum = m_config["z-order_count"] * m_weights["z-order"] + m_config["showdialog_count"] * m_weights["seq"] + m_config["refresh_count"] * m_weights["refresh"] + m_config["tab_change_count"] * m_weights["tab"] + m_config["button_press_count"] * m_weights["button"] + m_config["scroll_count"] * m_weights["scroll"] + m_config["select_list_count"] * m_weights["select"] + m_config["check_list_count"] * m_weights["check"] + m_config["write_count"] * m_weights["write"] + m_config["radio_list_count"] * m_weights["radio"] + m_config["gauge_count"] * m_weights["gauge"];
	return sum <= 0 ? -1 : sum;
}

unsigned long long TestingFrame::CalculateActualIndicator()
{
	float sum = 0.0;

	for (auto& e : m_result) {
		sum += e.second * m_weights[e.first];
	}

	return (unsigned long long)sum;
}

float TestingFrame::GetWeights()
{
	float sum = 0.0;

	for (auto& e : m_weights) {
		sum += e.second;
	}

	return sum;
}

void TestingFrame::WriteResults()
{
	StopTimers();

	wxString buff = "";
	{
		// For newer versions
		//auto now = wxDateTime::Now();
		//buff << now.Format("%Y:%m:%d %T").c_str() << ": ";
		auto now = wxDateTime::Now().GetTm();
		buff << now.year << ":";
		if (now.mon > 9) buff << now.mon << ":";
		else buff << "0" << now.mon << ":";
		if (now.mday > 9) buff << now.mday << ":";
		else buff << "0" << now.mday << ":";
		buff << " ";
		if (now.hour > 9) buff << now.hour << ":";
		else buff << "0" << now.hour << ":";
		if (now.min > 9) buff << now.min << ":";
		else buff << "0" << now.min << ":";
		if (now.sec > 9) buff << now.sec << ": ";
		else buff << "0" << now.sec << ": ";

		m_accumulate_cpu_usage += m_cpu_reader.GetCpuUsage();
		m_cpu_measure_count++;

		buff << GetCurrentProcessId() << ": TID " << int(m_config["test_id"]) << ": TM " << int(m_config["time_limit"] / 1000) << ": ";
		buff << "PERF " << CalculateActualIndicator() << ": " << ((float)CalculateActualIndicator() / CalculateTotalIndicator()) * 100.0f << "%: " << "CPU " << m_accumulate_cpu_usage / m_cpu_measure_count << "%: ";
		buff << m_description << " \r\n";

		if (m_config["z-order_count"] > 0) {
			buff << "z-order " << int(m_config["z-order_count"]) << " " << m_result["z-order"] << " " << int((m_result["z-order"] / m_config["z-order_count"]) * 100.0f) << "%, ";
		}

		if (m_config["showdialog_count"] > 0) {
			buff << "show_dialog " << int(m_config["showdialog_count"]) << " " << m_result["seq"] << " " << int((m_result["seq"] / m_config["showdialog_count"]) * 100.0f) << "%, ";
		}

		if (m_config["refresh_count"] > 0) {
			buff << "refresh " << int(m_config["refresh_count"]) << " " << m_result["refresh"] << " " << int((m_result["refresh"] / m_config["refresh_count"]) * 100.0f) << "%, ";
		}

		if (m_config["tab_change_count"] > 0) {
			buff << "tab_change " << int(m_config["tab_change_count"]) << " " << m_result["tab"] << " " << int((m_result["tab"] / m_config["tab_change_count"]) * 100.0f) << "%, ";
		}

		if (m_config["button_press_count"] > 0) {
			buff << "button_press " << int(m_config["button_press_count"]) << " " << m_result["button"] << " " << int((m_result["button"] / m_config["button_press_count"]) * 100.0f) << "%, ";
		}

		if (m_config["scroll_count"] > 0) {
			buff << "scroll " << int(m_config["scroll_count"]) << " " << m_result["scroll"] << " " << int((m_result["scroll"] / m_config["scroll_count"]) * 100.0f) << "%, ";
		}

		if (m_config["select_list_count"] > 0) {
			buff << "select_list " << int(m_config["select_list_count"]) << " " << m_result["select"] << " " << int((m_result["select"] / m_config["select_list_count"]) * 100.0f) << "%, ";
		}

		if (m_config["check_list_count"] > 0) {
			buff << "check_list " << int(m_config["check_list_count"]) << " " << m_result["check"] << " " << int((m_result["check"] / m_config["check_list_count"]) * 100.0f) << "%, ";
		}

		if (m_config["radio_list_count"] > 0) {
			buff << "radio_list " << int(m_config["radio_list_count"]) << " " << m_result["radio"] << " " << int((m_result["radio"] / m_config["radio_list_count"]) * 100.0f) << "%, ";
		}

		if (m_config["write_count"] > 0) {
			buff << "write " << int(m_config["write_count"]) << " " << m_result["write"] << " " << int((m_result["write"] / m_config["write_count"]) * 100.0f) << "%, ";
		}

		if (m_config["gauge_count"] > 0) {
			buff << "gauge " << int(m_config["gauge_count"]) << " " << m_result["gauge"] << " " << int((m_result["gauge"] / m_config["gauge_count"]) * 100.0f) << "%";
		}

		buff << " \r\n\r\n";
	}

	auto to_write = buff.c_str();

	if (m_output_path.substr(m_output_path.size() - 4) != wxString(".txt")) {
		m_output_path += ".txt";
	}
	
	bool need_write = true;
	DWORD bytes;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> wait_time(5, 25);
	HANDLE fh;

	while (need_write) {
		fh = CreateFile(m_output_path.c_str(), FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (fh != NULL && fh != INVALID_HANDLE_VALUE) {
			if (!WriteFile(fh, to_write, (DWORD)strlen(to_write), &bytes, NULL)) {
				wxLogError("Error during writing results");
			}

			CloseHandle(fh);
			need_write = false;
		}

		Sleep(wait_time(rng));
	}

	ClearTimers();

	ShutDown();
}

void TestingFrame::ForceCloseDialogs()
{
	if (m_dialogs.size() > 0) {
		for (auto& e : m_dialogs) {
			e->Destroy();
		}

		m_dialogs.clear();
	}
}

void TestingFrame::ShutDown()
{
	ForceCloseDialogs();
	this->Close(true);
}