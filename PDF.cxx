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

    ptree::ptree pt;
    ptree::ptree obj;

    for (auto &pdf_page : _pdf_pages) {
        ptree::ptree pt_page;
        ptree::ptree pt_results;

        pt_page.put("page", pdf_page.get_page());
        for (auto &r : pdf_page.get_results()) {

            ptree::ptree res;
            res.put("type", r.first);
            res.put("value", r.second);
            pt_results.push_back({"", res});

        }

        pt_page.add_child("results", pt_results);
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
