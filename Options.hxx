//
// Created by markus on 30/06/16.
//

#ifndef CONVERTER_OPTIONS_HXX
#define CONVERTER_OPTIONS_HXX

#include <vector>
#include <string>

#include <boost/optional.hpp>

/**
 * Helper struct for all options included in the supplied config file.
 */
struct Options {
    /**
     * One set of crop settings:
     * - type: the type of the area, any string
     * - x: x-coordinate in percent
     * - y: y-coordinate in percent
     * - w: width in percent
     * - h: height in percent
     */
    struct Crop {
        Crop(const std::string &type, float x, float y, float w, float h)
            : type(type), x(x), y(y), w(w), h(h) {}

        std::string type;
        float x;
        float y;
        float w;
        float h;
    };

    Options(const std::string &inputPDF, int dpi, bool parallel_processing,
            std::string _language)
        : _inputPDF(inputPDF), _dpi(dpi),
          _parallel_processing(parallel_processing),
          _language(_language), _start(-1), _end(-1) {}

    void addCrop(Crop crop) {
        _crops.push_back(crop);
    }

    std::string _inputPDF;
    std::string _language;
    std::vector<Crop> _crops;
    int _dpi;
    bool _parallel_processing;
    boost::optional<int> _start;
    boost::optional<int> _end;
    boost::optional<int> _page;
};

#endif //CONVERTER_OPTIONS_HXX
