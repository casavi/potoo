//
// Created by markus on 30/06/16.
//

#include "PDF.hxx"

#include <tbb/parallel_for_each.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <numeric>

namespace ptree = boost::property_tree;

PDF::PDF(const std::shared_ptr<Options> &options) : _opts(options), _document{_opts->_inputPDF} {}

boost::property_tree::ptree PDF::work() {

    // Generate an array of page numbers for easy iteration
    std::vector<int> page_numbers(_document.page_count());
    // Fills page_numbers with all numbers between 0 and d->pages()
    std::iota(page_numbers.begin(), page_numbers.end(), 0);

    // Allocate in one big block to improve performance
    _pdf_pages.resize(_document.page_count());

    // The main runner, extracted to avoid having it two times. Can work singlethreaded or multithreaded, requires no
    // locking and has no race conditions.
    const auto runner = [&](int page_number) {
        auto &pdf_page = _pdf_pages[page_number];
        pdf_page.set_opts(_opts);
        pdf_page.set_page_number(page_number);
        pdf_page.put_page(_document.get_page(page_number));
        pdf_page.process();
    };

    // Processes each page (parallelly if USE_SINGLETHREAD_PROCESSING is not defined)
    if (_opts->_parallel_processing) {
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

PDFPage PDF::get_page(int page_number) {
    PDFPage pdf_page;
    pdf_page.set_opts(_opts);
    pdf_page.set_page_number(page_number);
    pdf_page.put_page(_document.get_page(page_number));
    return pdf_page;
}
