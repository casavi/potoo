//
// Created by markus on 7/07/16.
//

#ifndef POTOO_COMMANDLINE_HXX
#define POTOO_COMMANDLINE_HXX

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>

struct GenericCommand {
    std::string _config;
};

struct HumanCommand : public GenericCommand {
    int _start;
    int _end;
};

struct FirstPageCommand : public GenericCommand {
    std::string _path;
};

struct OutputCommand : public GenericCommand {
    std::string _path;
    int _start;
    int _end;
};

using Command = boost::variant<HumanCommand, FirstPageCommand, OutputCommand>;

/**
 * The main parse function, takes the commandline parameters and returns them parsed.
 * Throws errors if any happen.
 * @param argc
 * @param argv
 * @return A boost optional of Command.
 */
boost::optional<Command> parse_options(int argc, const char **argv);

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
