// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "options_parser.h"

namespace po = boost::program_options;

using std::string;

command_line_options_t::command_line_options_t() {
    general_opt.add_options()
            ("help,h",
             "Show help message")
            ("config_file",
             po::value<string>(),
             "File with configuration");

    positional_opt.add("config_file", 1);
}

command_line_options_t::command_line_options_t(int ac, char **av) :
        command_line_options_t() // Delegate constructor
{
    parse(ac, av);
}

void command_line_options_t::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av)
                .options(general_opt)
                .positional(positional_opt)
                .run();
        po::store(parsed, var_map);
        notify(var_map);

        if (var_map.count("help")) {
            std::cout << general_opt << "\n";
            exit(EXIT_SUCCESS);
        }

        config_file = var_map["config_file"].as<string>();
    } catch (std::exception &ex) {
        throw OptionsParseException(ex.what()); // Convert to our error type
    }
}

config_file_options_t::config_file_options_t() {
    general_opt.add_options()
            ("cp", po::value<double>(), "C_p")
            ("k", po::value<double>(), "k")
            ("p", po::value<double>(), "p")
            ("height", po::value<double>(), "height")
            ("width", po::value<double>(), "width")
            ("dx", po::value<double>(), "dx")
            ("dy", po::value<double>(), "dy")
            ("dt", po::value<double>(), "dt")
            ("interval", po::value<double>(), "interval")
            ("table_f", po::value<string>(), "table_f")
            ("res_f", po::value<string>(), "res_f");
}

config_file_options_t::config_file_options_t(const string &config_file) :
        config_file_options_t() // Delegate constructor
{
    parse(config_file);
}

void config_file_options_t::parse(const string &config_file) {
    try {
        std::ifstream ifs{config_file.c_str()};
        if (ifs) {
            po::store(parse_config_file(ifs, general_opt), var_map);
        } else {
            throw OpenConfigFileException("Can't open config file");
        }
        notify(var_map);

        cp = var_map["cp"].as<double>();
        k = var_map["k"].as<double>();
        p = var_map["p"].as<double>();
        height = var_map["height"].as<double>();
        width = var_map["width"].as<double>();
        dx = var_map["dx"].as<double>();
        dy = var_map["dy"].as<double>();
        dt = var_map["dt"].as<double>();
        interval = var_map["interval"].as<double>();
        table_f = var_map["table_f"].as<string>();
        table_f.erase(std::remove(table_f.begin(), table_f.end(), '\"'), table_f.end());
        res_f = var_map["res_f"].as<string>();
        res_f.erase(std::remove(res_f.begin(), res_f.end(), '\"'), res_f.end());
    } catch (OpenConfigFileException &ex) {
        throw OpenConfigFileException(ex.what()); // Convert to our error type
    } catch (std::exception &ex) {
        throw OptionsParseException(ex.what());
    }
}