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


#endif //POTOO_COMMANDLINE_HXX
