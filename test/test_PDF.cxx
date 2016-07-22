
#include <boost/test/unit_test.hpp>

#include <PDF.hxx>
#include <boost/program_options/errors.hpp>

static std::shared_ptr<Options> opts;

BOOST_AUTO_TEST_CASE(pdf_init){
    opts = read_config("./fixtures/example.json");
}

BOOST_AUTO_TEST_CASE(pdf_main) {
    PDF p{opts};

    BOOST_REQUIRE(p.page_count() == 4);
}
