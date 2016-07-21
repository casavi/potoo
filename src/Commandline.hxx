//
// Created by markus on 7/07/16.
//

#ifndef POTOO_COMMANDLINE_HXX
#define POTOO_COMMANDLINE_HXX

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>

struct GenericCommand {
    std::string _config;
};

struct HelpCommand : public GenericCommand {
    // This only inherits from GenericCommand to make the visitor work
};

struct InfoCommand : public GenericCommand {
    std::string _path;
};

struct PositionalCommand : public GenericCommand {
    boost::optional<int> _page;
};

struct StartEndCommand : public PositionalCommand {
    boost::optional<int> _start;
    boost::optional<int> _end;
};

struct PageCommand : public PositionalCommand {
    std::string _path;
};

struct HumanCommand : public StartEndCommand {
};

struct OutputCommand : public StartEndCommand {
    std::string _path;
};

using Command = boost::variant<HumanCommand, PageCommand, OutputCommand, InfoCommand, HelpCommand>;

/**
 * The main parse function, takes the commandline parameters and returns them parsed.
 * Throws errors if any happen.
 * @param argc Argc
 * @param argv Argv
 * @return A boost optional of Command.
 */
Command parse_options(int argc, const char **argv);

/**
 * Helper class to always return the config member independently of the type.
 */
class generic_visitor : public boost::static_visitor<std::string> {
public:
    template<typename T>
    std::string operator()(const T &thing) const {
        return thing._config;
    }
};


static const std::string potoo_description = "Usage:\n"
    "potoo -c <config-file> <output-command: -S|-h|-o|-i> [<range-options>]\n"
    "\n"
    "Allowed options:\n"
    "  --help                        produce this help message\n"
    "  -c [--config] path            path to config json file (Required)\n"
    "\n"
    "And ONE of the following:\n"
    "  -S [--single_page] path       render the nth page (--page) of the pdf as PNG into <path>\n"
    "  -h [--human]                  process everything and print to stdout, mainly for debugging\n"
    "  -o [--output] path            process everything and save it as json at <path>\n"
    "  -i [--info] page              provide information about the pdf file and save it at <path>\n"
    "Additional options:\n"
    "  Range options:\n"
    "    -s [--start] start          The page on which to start (0-based). 0 if not specified\n"
    "    -e [--end] end              The last page to process (0-based). <page-count> if not specified\n"
    "  Position options:\n"
    "    -p [--page] page            Only work on a single page\n"
    "\n"
    "Examples:\n"
    "  Render the first page into first_page.png\n"
    "    potoo -c file.json -S first_page.png -p 0\n"
    "  Extract text from the first 3 pages and display it in human readable form\n"
    "    potoo -c file.json -h -e 3\n"
    "  Extract text from the 9th page (0-based) and display it in human readable form\n"
    "    potoo -c file.json -h -p 8\n"
    "  Get the page count of a pdf and save it into pagecount.json\n"
    "    potoo -c file.json -i pagecount.json\n"
    "\n";


#endif //POTOO_COMMANDLINE_HXX
