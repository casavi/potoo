#include "Options.hxx"
#include "exceptions.hxx"

#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

std::shared_ptr<Options> read_config(const std::string &path) {
    namespace ptree = boost::property_tree;

    // Read the
    ptree::ptree pt;
    try {
        // Read the config file, use config_visitor to return the _config member every command has
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
            pt.get<std::string>("language"),
            pt.get_child_optional("force_ocr") ? pt.get<bool>("force_ocr") : false
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

    // check if the supplied crop types are unique
    // Sorts all types in the vector and moves the duplicate ones to the back
    // Alters the underlying vector, but we don't care -> exit if duplicates are found
    auto it = std::unique(
        opts->_crops.begin(), opts->_crops.end(),
        [](const Options::Crop &a, const Options::Crop &b) {
            return a.type == b.type;
        }
    );

    if (it != opts->_crops.end()) {
        std::stringstream ss;
        ss << "duplicate crop types in config file:" << std::endl;
        std::for_each(it, opts->_crops.end(), [&ss](const Options::Crop &crop) {
            ss << "\t" << crop.type << std::endl;
        });
        throw std::runtime_error(ss.str());
    }
    return opts;
}