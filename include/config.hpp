#pragma once
#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <switch.h>
// #include <toml.hpp>
#include <toml++/toml.h>
#include "mini/ini.h"

#define CONFIG_PATH "sdmc://atmosphere/contents/01006A800016E000/romfs/arcropolis.toml"
#define ARCADIA_CONFIG_PATH "sdmc://switch/arcadia.ini"
#define SYSTEM_SETTINGS "sdmc://atmosphere/config/system_settings.ini"

class Infos {
    public:
        std::string version;
};

class Paths{
    public:
        std::string arc;
        std::string stream;
        std::string umm;
};

class Updater{
    public:
        std::string server_ip;
        bool beta_updates;
};

class Logger{
    public:
        std::string logger_level;
};

class Miscellaneous{
    public:
        bool debug;
};

class ConfigLayout {
    public:
        Infos infos;
        Paths paths;
        Updater updater;
        Logger logger;
        Miscellaneous misc;
};


class ARCadiaConfig {
    private:
        inline static mINI::INIStructure ini;
    public:
        inline static std::string sort_option;
        inline static bool sort_desc;
        

        static bool initConfig(){    
            mINI::INIFile file(ARCADIA_CONFIG_PATH);
            file.read(ini);
            if(!std::filesystem::exists(ARCADIA_CONFIG_PATH)){
                sort_option = "name";
                sort_desc = true;
                ini["config"]["sort_option"] = sort_option;
                ini["config"]["sort_desc"] = std::to_string(sort_desc);
                file.generate(ini);
                return true;
            } else {
                sort_option = ini["config"]["sort_option"];
                if(ini["config"]["sort_desc"] == "0"){
                    sort_desc = false;
                }else{
                    sort_desc = true;
                }
                return true;
            }
        }

        static bool write(std::string section, std::string key, std::string value){
            mINI::INIFile file(ARCADIA_CONFIG_PATH);
            ini[section][key] = value;
            return file.write(ini);
        }
};


class Config {
    public:
        inline static ConfigLayout config_info;
        static bool initConfig(){
            if(!std::filesystem::exists(CONFIG_PATH)){
                return false;
            }else{
                auto config_data = toml::parse_file(CONFIG_PATH);

                config_info.infos.version = config_data["infos"]["version"].value_or("0.9.0");
                config_info.paths.arc = config_data["paths"]["arc"].value_or("sd:/atmosphere/contents/01006A800016E000/romfs/arc");
                config_info.paths.umm = config_data["paths"]["umm"].value_or("sd:/ultimate/mods");

                config_info.logger.logger_level = config_data["logger"]["logger_level"].value_or("Info");

                config_info.updater.server_ip = config_data["updater"]["server_ip"].value_or("178.62.31.147");
                config_info.updater.beta_updates = config_data["updater"]["beta_updates"].value_or("false");
                
                config_info.misc.debug = config_data["misc"]["debug"].value_or("false");

                // const auto& info_table = toml::find(config_data, "infos");
                // config_info.infos.version  = toml::find<std::string>(info_table, "version");
                
                // const auto& paths_table = toml::find(config_data, "paths");
                // config_info.paths.arc  = toml::find<std::string>(paths_table, "arc");
                // config_info.paths.umm  = toml::find<std::string>(paths_table, "umm");
                
                // const auto& misc_table = toml::find(config_data, "misc");
                // config_info.misc.debug  = toml::find_or<bool>(misc_table, "debug", false);

                return true;
            }
        };

        static bool updateSystemSettings(){
            mINI::INIStructure ini;
            mINI::INIFile file(SYSTEM_SETTINGS);
            file.read(ini);
            ini["ro"]["ease_nro_restriction"] = "u8!0x1";
            return file.generate(ini);
        }

        static bool saveConfig(){
            auto tbl = toml::table{{
                { "infos", toml::table{{
                        { "version",  config_info.infos.version}
                    }}
                },
                { "paths", toml::table{{
                        { "arc",  config_info.paths.arc},
                        { "umm",  config_info.paths.umm}
                    }}
                },
                { "logger", toml::table{{
                        { "logger_level",  config_info.logger.logger_level},
                    }}
                },
                { "updater", toml::table{{
                        { "server_ip",  config_info.updater.server_ip},
                        { "beta_updates",  config_info.updater.beta_updates},
                    }}
                },
                { "misc", toml::table{{
                        { "debug",  config_info.misc.debug},
                    }}
                },
            }};

            std::ofstream ARCropolisConfigFile;
            ARCropolisConfigFile.open (CONFIG_PATH);
            ARCropolisConfigFile << tbl;
            ARCropolisConfigFile.close();

            return true;
        }
};