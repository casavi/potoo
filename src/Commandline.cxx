
#include "Commandline.hxx"

#include <boost/program_options.hpp>
#include <iostream>

boost::optional<Command> parse_options(int argc, const char **argv) {
    using namespace boost::program_options;

    // The command line variables
    std::string config, single_page, output;
    bool human, info;
    int start = -1, end = -1, page = -1;

    // All available options
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", value<std::string>(&config))
        ("human,h", bool_switch(&human)->default_value(false))
        ("single_page,S", value<std::string>(&single_page))
        ("output,o", value<std::string>(&output))
        ("start,s", value<int>(&start))
        ("end,e", value<int>(&end))
        ("page,p", value<int>(&page))
        ("info,i", bool_switch(&info)->default_value(false));

    static const std::string description = "Usage:\n"
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

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // Only print help text if --help or -h is specified
    if (vm.count("help") || argc == 1) {
        std::cout << description << std::endl;
        return boost::none;
    }

    // We need a valid config, so we exit if there is none
    if (config.empty()) {
        throw std::runtime_error(
            "no configuration file set\n\n" + description);
    }

    // config is set, but output and human are not
    if (!config.empty() && !(!output.empty() || human || !single_page.empty() || info)) {
        throw std::runtime_error(
            "please either specify an output file, the human flag or the single_page parameter\n\n" + description
        );
    }

    if (
        (vm.count("start") && start <= -1)
        || (vm.count("end") && end <= -1)
        || (vm.count("page") && page <= -1)
        ) {
        throw std::runtime_error("invalid start or end range or page");
    }

    const static auto integer_to_optional = [](int param) -> boost::optional<int> {
        if (param == -1) return boost::none;
        else return boost::make_optional(param);
    };

    if (!single_page.empty()) { // single_page parameter was supplied, so we just save the image

        if (vm.count("start") || vm.count("end")) {
            throw std::runtime_error("start and end are invalid parameters for single_page");
        }

        PageCommand fp;
        fp._config = config;
        fp._path = single_page;
        fp._page = integer_to_optional(page);
        return boost::make_optional(Command(fp));
    } else if (info) {
        if (output.empty()) {
            throw std::runtime_error("please specify the output file for the pdf information");
        }

        InfoCommand ic;
        ic._config = config;
        ic._path = output;
        return boost::make_optional(Command(ic));
    }
    else { // single_page parameter was not supplied, run the main routine

        if ((vm.count("start") || vm.count("end")) && vm.count("page")) {
            throw std::runtime_error("start and/or end and page cannot be used at the same time");
        }

        if (human) { // We want to print for humans, so let's config that
            HumanCommand hc;
            hc._config = config;
            hc._start = integer_to_optional(start);
            hc._end = integer_to_optional(end);
            hc._page = integer_to_optional(page);
            return boost::make_optional(Command(hc));
        } else { // Return the path for saving
            OutputCommand oc;
            oc._config = config;
            oc._path = output;
            oc._start = integer_to_optional(start);
            oc._end = integer_to_optional(end);
            oc._page = integer_to_optional(page);
            return boost::make_optional(Command(oc));
        }
    }
}
