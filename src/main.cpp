
#include "Commandline.hxx"
#include "exceptions.hxx"
#include "Timer.hxx"
#include "PDF.hxx"

#include <boost/property_tree/json_parser.hpp>

std::shared_ptr<Options> read_config(const std::string &path) {
    namespace ptree = boost::property_tree;

    // Read the
    ptree::ptree pt;
    try {
        // Read the config file, use generic_visitor to return the _config member every command has
        ptree::read_json(path, pt);
    } catch (std::exception &e) {
        throw invalid_config_exception(std::string("\tthe config file is malformed\n\tException: ") + e.what());
    }

    // try to fill our Options object with all provided options
    std::shared_ptr<Options> opts;
    try {
        opts = std::make_shared<Options>(
            pt.get<std::string>("inputPDF"),
            pt.get<int>("dpi"),
            pt.get_child_optional("parallel_processing") ? pt.get<bool>("parallel_processing") : false,
            pt.get<std::string>("language")
        );
        if (auto crops = pt.get_child_optional("crops")) {
            for (const auto &crop : crops.get()) {
                opts->addCrop(Options::Crop{
                    crop.second.get<std::string>("type"),
                    crop.second.get<float>("dimensions.x"),
                    crop.second.get<float>("dimensions.y"),
                    crop.second.get<float>("dimensions.w"),
                    crop.second.get<float>("dimensions.h")
                });
            }
        }
    } catch (std::exception &e) {
        throw invalid_config_exception(
            std::string("\tthe json format does not meet the expectations\n\tException: ") + e.what());
    }
    return opts;
}

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

        if(command.type() == typeid(HelpCommand)){
            std::cout << potoo_description << std::endl;
            return 0;
        }

        std::shared_ptr<Options> opts = read_config(boost::apply_visitor(generic_visitor{}, command));

        // check if the supplied crop types are unique
        {
            // Sorts all types in the vector and moves the duplicate ones to the back
            // Alters the underlying vector, but we don't care -> exit if duplicates are found
            auto it = std::unique(
                opts->_crops.begin(), opts->_crops.end(),
                [](const Options::Crop &a, const Options::Crop &b) {
                    return a.type == b.type;
                }
            );

            if (it != opts->_crops.end()) {
                std::cerr << "ERROR: duplicate crop types:" << std::endl;
                std::for_each(it, opts->_crops.end(), [](const Options::Crop &crop) {
                    std::cerr << "\t" << crop.type << std::endl;
                });
                return 1;
            }
        }

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

            if(opts->_crops.empty()){
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
