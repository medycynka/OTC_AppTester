#include "tApp.h"


wxIMPLEMENT_APP(tApp);

tApp::tApp() : wxApp(), m_app_id(1000), m_main_frame(nullptr), m_config_path("configuration.txt") {

}

tApp::~tApp() {
}

bool tApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    this->Bind(wxEVT_END_SESSION, &tApp::OnForceClose, this);
    this->Bind(wxEVT_QUERY_END_SESSION, &tApp::OnForceClose, this);

    wxString res_path = "";
    wxString desc = "";

    // Reading configuration from file
    wxTextFile config(m_config_path);
    config.Open();

    // First line contains:
    // test_id <ID>  - id of our simulation
    // <description> - description of a simulation, wich will be put in the results
    wxString line = config.GetFirstLine();
    size_t first_space = line.find_first_of(" ");
    m_config[line.substr(0, first_space)] = std::stoi(line.substr(first_space, line.find_first_of(" ", first_space)).wc_str());
    line.erase(0, first_space + 1);
    desc = line.substr(line.find_first_of(" ") + 1);

    // Last line contains:
    // outfile <path> - path to output file in wich the results will be saved
    line = config.GetLastLine();
    res_path = line.substr(line.find_first_of(" ") + 1);

    // Reading and saving configuration
    // w_width, w_height - size of main window
    // time_limit        - simulation time
    // dialogs_count     - numbers of dialogs to use in simulation
    // z-order_frq       - frequency of z-order dialogs change
    // showdialog_frq    - frequency of sequential dialogs change
    // refresh_frq       - frequency of refreshing main window
    // tab_change_frq    - frequency of tab changing in tab control
    // button_press_frq  - frequency of button pressing
    // scroll_frq        - frequency of scrolling
    // select_list_frq   - frequency of picking an item from a selection list
    // check_list_frq    - frequency of checking an item on a check list
    // write_frq         - frequency of writing
    // radio_list_frq    - frequency of picking an item from a radio list
    // gauge_frq         - frequency of gauge increment/decrement
    for (size_t i = 1; i < config.GetLineCount() - 1; ++i) {
        line = config.GetLine(i);
        first_space = line.find_first_of(" ");
        auto nr = std::stod(line.substr(first_space, line.find_first_of(" ", first_space)).wc_str());

        if (i < 5) {
            m_config[line.substr(0, first_space)] = nr;
        }
        else {
            m_config[line.substr(0, first_space)] = nr <= 0.0 ? 0.0 : 1000 / nr;
            m_config[line.substr(0, line.find_last_of('_') + 1) + "count"] = 0.0;
        }
    }

    config.Close();

    // Adding time limit in ms and planed operation count
    m_config["time_limit"] *= 1000;
    m_config["z-order_count"] = m_config["z-order_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["z-order_frq"]);
    m_config["showdialog_count"] = m_config["showdialog_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["showdialog_frq"]);
    m_config["refresh_count"] = m_config["refresh_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["refresh_frq"]);
    m_config["tab_change_count"] = m_config["tab_change_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["tab_change_frq"]);
    m_config["button_press_count"] = m_config["button_press_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["button_press_frq"]);
    m_config["scroll_count"] = m_config["scroll_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["scroll_frq"]);
    m_config["select_list_count"] = m_config["select_list_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["select_list_frq"]);
    m_config["check_list_count"] = m_config["check_list_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["check_list_frq"]);
    m_config["write_count"] = m_config["write_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["write_frq"]);
    m_config["radio_list_count"] = m_config["radio_list_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["radio_list_frq"]);
    m_config["gauge_count"] = m_config["gauge_frq"] <= 0.0 ? 0.0 : unsigned long(m_config["time_limit"] / m_config["gauge_frq"]);


    if (m_config["z-order_count"] != 0 && m_config["showdialog_count"] != 0) {
        m_config["showdialog_count"] = 0;
    }

    m_main_frame = new TestingFrame(nullptr, m_app_id + 10000, m_config, desc, res_path);
    m_main_frame->Layout();
    m_main_frame->Update();
    m_main_frame->RepaintDialogs();

    SetTopWindow(m_main_frame);
    SetExitOnFrameDelete(true);

    DisableProcessWindowsGhosting();

    return true;
}

int tApp::OnExit()
{
    // Closing process
    DWORD pid = wxGetProcessId();
    DWORD waitTimeout = 500;
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (hProcess == NULL) {
        std::cerr << "OpenProcess error " << ::GetLastError() << "\n";
    }

    // Closing application
    ::PostQuitMessage(0);

    switch (::WaitForSingleObject(hProcess, waitTimeout)) {
    case WAIT_OBJECT_0:
        // Application is closed
        break;
    default:
        // If process is still allive after timeout, force close application
        if (!::TerminateProcess(hProcess, (UINT)0)) {
            // not supposed to happen
            wxLogSysError(_("Failed to kill process %d"), pid);
        }
        break;
    }

    ::CloseHandle(hProcess);

    return 0;
}

int tApp::OnRun()
{
    // Starting simulation
    m_main_frame->StartTest();

    auto exit_code = wxApp::OnRun();

    if (exit_code != 0) {
        return exit_code;
    }

    return 0;
}

void tApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetDesc(g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars(wxT("-"));
}

bool tApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    // Reading from command line
    if (parser.GetParamCount() > 0) {
        m_config_path = parser.GetParam(0);
    }

    // For more than one parameter
    /*wxArrayString files;
    for (auto i = 0; i < parser.GetParamCount(); i++) {
        files.Add(parser.GetParam(i));
    }*/

    return true;
}

void tApp::OnForceClose(wxCloseEvent& e)
{
    m_main_frame->Close(true);
    this->OnExit();
}
