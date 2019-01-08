// Run the right internet browser.
#include <ctime>
#include <map>
#include <string>
#include <utility>

#include <windows.h>
#pragma comment(lib, "user32.lib")

using namespace std::literals;


const auto firefox_class = "MozillaWindowClass"s;
const auto chrome_class = "Chrome_WidgetWin_1"s;
const auto browsers = std::map<std::string, std::string>{
    {firefox_class, "C:/Program Files (x86)/Mozilla Firefox/firefox.exe"},
    {chrome_class, "C:/Program Files (x86)/Google/Chrome/Application/chrome.exe"}
};


// Launch a program.
auto run(const std::string& path)
{
    auto startup_info = STARTUPINFO{};
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    auto process_information = PROCESS_INFORMATION{};
    ZeroMemory(&process_information, sizeof(process_information));
    // Thanks, Windows API(!)
    CreateProcess(
        path.c_str(),
        nullptr,
        nullptr,
        nullptr,
        false,
        NORMAL_PRIORITY_CLASS,
        nullptr,
        nullptr,
        &startup_info,
        &process_information
    );
}


// Bring focus to a window.
auto raise(const HWND window)
{
    const auto current_window = GetForegroundWindow();
    const auto current_thread = GetCurrentThreadId();
    const auto remote_thread = GetWindowThreadProcessId(current_window, nullptr);
    AttachThreadInput(current_thread, remote_thread, true);
    SetForegroundWindow(window);
    AttachThreadInput(current_thread, remote_thread, false);
}


// Find a window given a window class name.
// Handles Chrome edge-case of multiple 'windows'.
auto find_window(const std::string& class_name)
{
    auto window = FindWindow(class_name.c_str(), nullptr);
    // Chrome launches two windows,
    // but the first one isn't actually a window, so skip it.
    if (class_name == chrome_class) {
        window = GetWindow(window, GW_HWNDNEXT);
    }
    return window;
}


// Launch a program or focus an existing instance.
auto run_or_raise(const std::string& class_name, const std::string& path)
{
    const auto window = find_window(class_name);
    if (window) {
        raise(window);
    }
    else {
        run(path);
    }
}


// Enumeration to compare against `std::tm.tm_wday`.
enum days {
    sunday = 0,
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday
};


// Return whether a time is during school times.
auto is_during_school(const std::tm& time)
{
    const auto day_of_week = time.tm_wday;
    const auto is_weekday = day_of_week >= monday && day_of_week <= friday;
    const auto time_of_day = time.tm_hour;
    const auto is_school_time = time_of_day >= 8 && time_of_day <= 15;
    return is_weekday && is_school_time;
}


// Return the appropriate window class and executable path for school.
auto get_browser(const bool at_school) -> decltype(browsers)::value_type
{
    if (at_school) {
        return {chrome_class, browsers.at(chrome_class)};
    }
    return {firefox_class, browsers.at(firefox_class)};
}


// Launch Firefox or Chrome depending on whether I'm at school.
int main()
{
    const auto time = std::time(nullptr);
    const auto today = *std::localtime(&time);
    const auto at_school = is_during_school(today);
    const auto [browser_class, path] = get_browser(at_school);
    // const auto [browser_class, path] = get_browser(true);
    // const auto [browser_class, path] = get_browser(false);
    run_or_raise(browser_class, path);
}
