
#include <boost/test/unit_test.hpp>

#include <Options.hxx>
#include <boost/program_options/errors.hpp>
#include <exceptions.hxx>

BOOST_AUTO_TEST_CASE(options_example) {
    const char* path = "./fixtures/example.json";

    auto c = read_config(path);
    BOOST_CHECK(c->_inputPDF == "./fixtures/example-doc.pdf");
    BOOST_CHECK(c->_dpi == 250);
    BOOST_CHECK(c->_language == "deu");
    BOOST_CHECK(c->_parallel_processing);
    BOOST_REQUIRE(c->_crops.size() == 1);
    BOOST_CHECK(c->_crops[0].type == "test");
    BOOST_CHECK(c->_crops[0].x == 10);
    BOOST_CHECK(c->_crops[0].y == 11);
    BOOST_CHECK(c->_crops[0].w == 12);
    BOOST_CHECK(c->_crops[0].h == 13);
}

BOOST_AUTO_TEST_CASE(options_wrong_syntax) {
    const char* path = "./fixtures/invalid_configs/wrong_syntax.json";

    BOOST_CHECK_THROW(read_config(path), invalid_config_exception);
}

BOOST_AUTO_TEST_CASE(options_missing_options) {
    const char* path = "./fixtures/invalid_configs/missing_options.json";

    BOOST_CHECK_THROW(read_config(path), invalid_config_exception);
}

BOOST_AUTO_TEST_CASE(options_duplicate_crops) {
    const char* path = "./fixtures/invalid_configs/duplicate_crops.json";

    BOOST_CHECK_THROW(read_config(path), std::runtime_error);
}
