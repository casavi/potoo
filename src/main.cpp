
#include "Commandline.hxx"
#include "exceptions.hxx"
#include "Timer.hxx"
#include "PDF.hxx"
#include "Options.hxx"

#include <boost/property_tree/json_parser.hpp>

int main(int argc, const char **argv) {
    //Timer t("main_program");
    try {
        namespace ptree = boost::property_tree;

        Magick::InitializeMagick(NULL);

        Command command;
        try {
            // Parse all command line arguments
            command = parse_options(argc, argv);
        } catch (std::exception &e) {
            std::cerr << e.what() << "\n\n" << potoo_description << std::endl;
            return 1;
        }

        if (command.type() == typeid(HelpCommand)) {
            std::cout << potoo_description << std::endl;
            return 0;
        }

        std::shared_ptr<Options> opts = read_config(boost::apply_visitor(config_visitor{}, command));

        // Subcommand handling below
        if (command.type() == typeid(PageCommand)) { // first_page subcommand handling
            auto c = boost::get<PageCommand>(command);
            PDF main_pdf(opts);

            if (c._page && c._page.get() >= static_cast<int>(main_pdf.page_count())) {
                throw std::runtime_error("page cannot be bigger than the document's page count");
            }

            auto page = main_pdf.get_page(c._page ? c._page.get() : 0);
            const auto &img = page.image_representation();
            img->write(c._path);
        } else if (command.type() == typeid(InfoCommand)) {

            auto c = boost::get<InfoCommand>(command);

            PDF main_pdf(opts);

            // Root of the PT
            ptree::ptree pt;
            pt.put("pages", main_pdf.page_count());

            std::stringstream ofs;
            ptree::write_json(ofs, pt);
            std::cout << ofs.str() << std::endl;
        }
        else {

            if (opts->_crops.empty()) {
                throw std::runtime_error("no crop regions specified");
            }

            if (command.type() == typeid(HumanCommand)) {
                auto c = boost::get<HumanCommand>(command);
                opts->_start = c._start;
                opts->_end = c._end;
                opts->_page = c._page;
            }
            if (command.type() == typeid(OutputCommand)) {
                auto c = boost::get<OutputCommand>(command);
                opts->_start = c._start;
                opts->_end = c._end;
                opts->_page = c._page;
            }

            PDF main_pdf(opts);
            // everything happens here
            auto result = main_pdf.work();

            if (command.type() == typeid(HumanCommand)) { // human subcommand handling

                for (auto &p : result.get_child("results")) {
                    std::cout << "Page " << p.second.get<std::string>("page") << ":" << std::endl;
                    for (auto &r : p.second.get_child("results")) {
                        std::cout << "Type: " << std::endl << r.second.get<std::string>("type") << std::endl
                                  << std::endl;
                        std::cout << "Result: " << std::endl << r.second.get<std::string>("value") << std::endl;
                    }
                }

            } else if (command.type() == typeid(OutputCommand)) { // output subcommand handling

                auto subcomm = boost::get<OutputCommand>(command);
                try {
                    std::ofstream ofs(subcomm._path, std::ios_base::trunc);
                    write_json(ofs, result);
                } catch (std::exception &e) {
                    throw std::runtime_error("could not write to " + subcomm._path + ": " + e.what());
                }

            } else {
                throw std::runtime_error("Invalid subcommand type. This should never happen, please file a bug report");
            }
        }
    } catch (std::exception &e) {
        //t.time();
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    //t.time();
    return 0;
}
