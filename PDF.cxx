
#include "PDF.hxx"

#include <tbb/parallel_for_each.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <numeric>
#include <thread>

namespace ptree = boost::property_tree;

PDF::PDF(const std::shared_ptr<Options> &options) : _opts(options), _document{_opts->_inputPDF} {}

boost::property_tree::ptree PDF::work() {

    // Test if end/start are there and bigger than page_count
    if ((_opts->_end && _opts->_end.get() > _document.page_count())
        || (_opts->_start && _opts->_start.get() > _document.page_count())) {
        throw std::runtime_error("the start/end parameter can't be bigger than the page count");
    }

    // Test if page is there and bigger than page_count
    if (_opts->_page && _opts->_page.get() > _document.page_count()) {
        throw std::runtime_error("the page parameter can't be bigger than the page count");
    }

    int start = 0, end = 0;

    // start/end specified, calculate our range
    if (_opts->_end || _opts->_start) {
        start = !_opts->_start ? 0 : _opts->_start.get();
        end = !_opts->_end ? 0 : static_cast<int>(_document.page_count()) - _opts->_end.get();
    }

    // page specified, calculate our pseudo-range
    if (_opts->_page) {
        start = _opts->_page.get();
        end = static_cast<int>(_document.page_count()) - (start + 1);
    }

    // Generate an array of page numbers for easy iteration
    std::vector<int> page_numbers(_document.page_count() - start - end);
    // Fills page_numbers with all numbers between 0 and d->pages()
    std::iota(page_numbers.begin(), page_numbers.end(), start);

    // Allocate in one big block to improve performance
    _pdf_pages.resize(page_numbers.size());

    // The main runner, extracted to avoid having it two times. Can work singlethreaded or multithreaded, requires no
    // locking and has no race conditions.
    const auto runner = [&](int page_number) {
        auto &pdf_page = _pdf_pages[page_number - start];
        pdf_page.set_opts(_opts);
        pdf_page.put_page(_document.get_page(page_number));
        pdf_page.process();
    };

    // Processes each page
    if (_opts->_parallel_processing && page_numbers.size() > 2 && std::thread::hardware_concurrency() > 1) {
        tbb::parallel_for_each(page_numbers.begin(), page_numbers.end(), runner);
    } else {
        std::for_each(page_numbers.begin(), page_numbers.end(), runner);
    }

    // Constructs the result object (json).

    // Root of the PT
    ptree::ptree pt;

    // The main results array
    ptree::ptree obj;

    // For each page
    for (auto &pdf_page : _pdf_pages) {
        // One page
        ptree::ptree pt_page;

        // The region results
        ptree::ptree pt_results;

        // Put the page number into pt_page
        pt_page.put("page", pdf_page.get_page());

        // For each of the regions
        for (auto &r : pdf_page.get_results()) {

            ptree::ptree res;
            // Put the type and value into the single region
            res.put("type", r.first);
            res.put("value", r.second);

            // Add the region to the results
            pt_results.push_back({"", res});

        }

        // Add the results to the page object
        pt_page.add_child("results", pt_results);

        // Add the main object to our result array
        obj.push_back({"", pt_page});
    }

    pt.add_child("results", obj);

    return pt;
}

PDFPage PDF::get_page(int page_number) const {
    PDFPage pdf_page;
    pdf_page.set_opts(_opts);
    pdf_page.put_page(_document.get_page(page_number));
    return pdf_page;
}

size_t PDF::page_count() const {
    return _document.page_count();
}
