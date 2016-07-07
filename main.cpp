

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "exceptions.hxx"
#include "Timer.hxx"
#include "PDF.hxx"
#include <boost/program_options.hpp>


template<typename T>
boost::program_options::typed_value<T> *named_argument(T &element, const std::string &name) {
    auto ret = new boost::program_options::typed_value<T>(&element);
    ret->value_name(name);
    return ret;
}

int main(int argc, const char **argv) {
    using namespace boost::property_tree;
    using namespace boost::program_options;

    Magick::InitializeMagick(NULL);

    // Argument variables
    std::string config;
    std::string first_page;
    bool human;

    // Declare the supported options.
    options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config,c", named_argument(config, "path"), "path to configuration file")
        ("human,h", bool_switch(&human)->default_value(false), "<optional> print in human readable format")
        ("first_page,f", named_argument(first_page, "path"),
         "<optional> only convert the first page to the specified path, overrides normal behaviour");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    // Only print help text if --help or -h is specified
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    // We need a valid config, so we exit if there is none
    if (config.empty()) {
        std::cerr << "ERROR: no configuration file set" << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    try {
        ptree pt;

        try {
            read_json(config, pt);
        } catch (std::exception &e) {
            throw invalid_config_exception(std::string("\tthe config file is malformed\n\tException: ") + e.what());
        }


        // try to fill our Options object with all provided options
        std::shared_ptr<Options> opts;
        try {
            opts = std::make_shared<Options>(
                pt.get<std::string>("inputPDF"),
                pt.get<std::string>("outputFile"),
                pt.get<int>("dpi"),
                pt.get_child_optional("parallel_processing") ? pt.get<bool>("parallel_processing") : false,
                pt.get<std::string>("language")
            );
            for (const auto &crop : pt.get_child("crops")) {
                opts->addCrop(Options::Crop{
                    crop.second.get<std::string>("type"),
                    crop.second.get<float>("dimensions.x"),
                    crop.second.get<float>("dimensions.y"),
                    crop.second.get<float>("dimensions.w"),
                    crop.second.get<float>("dimensions.h")
                });
            }
        } catch (std::exception &e) {
            throw invalid_config_exception(
                std::string("\tthe json format does not meet the expectations\n\tException: ") + e.what());
        }

        // check if the supplied crop types are unique
        {
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

        if (!first_page.empty()) { // first_page parameter was supplied, so we just save the image
            PDF main_pdf(opts);
            auto page = main_pdf.get_page(0);
            auto img = page.get_image_representation();
            img.write(first_page);
        } else { // first_page parameter was not supplied, run the main routine

            PDF main_pdf(opts);

            // everything happens here
            auto result = main_pdf.work();

            if (human) {
                for (auto &p : result.get_child("results")) {
                    std::cout << "Page " << p.second.get<std::string>("page") << ":" << std::endl;
                    for (auto &r : p.second.get_child("results")) {
                        std::cout << "Type: " << std::endl << r.second.get<std::string>("type") << std::endl
                                  << std::endl;
                        std::cout << "Result: " << std::endl << r.second.get<std::string>("value") << std::endl;
                    }
                }
            } else {
                try {
                    std::ofstream ofs(opts->_outputFile, std::ios_base::trunc);
                    write_json(ofs, result);
                } catch (std::exception &e) {
                    throw std::runtime_error("could not write to " + opts->_outputFile + ": " + e.what());
                }
            }
        }
    }
    catch (invalid_config_exception &e) {
        std::cerr << "Invalid config file:" << std::endl << e.what() << std::endl;
        return 1;
    }
    catch (std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

