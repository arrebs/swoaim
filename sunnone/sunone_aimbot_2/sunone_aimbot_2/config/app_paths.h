#pragma once
#ifndef WHOZETAI_APP_PATHS_H
#define WHOZETAI_APP_PATHS_H

#include <string>

// Every string below is UTF-8. The config file lives outside the install
// directory so an update that replaces the exe cannot overwrite user settings.
namespace AppPaths
{
std::string executableDirectory();
std::string configDirectory();
std::string defaultConfigFile();

// First run only: if the new config file is absent and a legacy config.ini
// sits beside the executable, copy it into the config directory. The legacy
// file is never modified or removed. Logs exactly one line when it fires.
void migrateLegacyConfig();
}
#endif // WHOZETAI_APP_PATHS_H
