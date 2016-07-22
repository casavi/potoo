
#include <boost/test/unit_test.hpp>

#include <Commandline.hxx>
#include <boost/program_options/errors.hpp>

// Valid tests

BOOST_AUTO_TEST_CASE(commandline_visitor) {
    PageCommand pc;
    pc._config = "asdf.json";
    auto c = Command(pc);
    BOOST_CHECK(boost::apply_visitor(config_visitor{}, c) == pc._config);
}

BOOST_AUTO_TEST_CASE(commandline_none) {
    int argc = 1;
    const char *argv[] = {
        "potoo_test"
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(HelpCommand));
}

BOOST_AUTO_TEST_CASE(commandline_help) {
    int argc = 2;
    const char *argv[] = {
        "potoo_test",
        "--help"
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(HelpCommand));
}

BOOST_AUTO_TEST_CASE(commandline_human) {
    int argc = 4;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-h"
    };

    auto c = parse_options(argc, argv);
    BOOST_REQUIRE(c.type() == typeid(HumanCommand));
    auto h = boost::get<HumanCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._start == boost::none);
    BOOST_CHECK(h._end == boost::none);
    BOOST_CHECK(h._page == boost::none);
}

BOOST_AUTO_TEST_CASE(commandline_output) {
    int argc = 5;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-o", "./fixtures/output.json",
    };

    auto c = parse_options(argc, argv);
    BOOST_REQUIRE(c.type() == typeid(OutputCommand));
    auto h = boost::get<OutputCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/output.json");
    BOOST_CHECK(h._start == boost::none);
    BOOST_CHECK(h._end == boost::none);
    BOOST_CHECK(h._page == boost::none);
}

BOOST_AUTO_TEST_CASE(commandline_output_start_end) {
    int argc = 9;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-o", "./fixtures/output.json",
        "-s", "1",
        "-e", "3"
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(OutputCommand));
    auto h = boost::get<OutputCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/output.json");
    BOOST_CHECK(h._start == boost::make_optional(1));
    BOOST_CHECK(h._end == boost::make_optional(3));
    BOOST_CHECK(h._page == boost::none);
}

BOOST_AUTO_TEST_CASE(commandline_output_page) {
    int argc = 7;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-o", "./fixtures/output.json",
        "-p", "0"
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(OutputCommand));
    auto h = boost::get<OutputCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/output.json");
    BOOST_CHECK(h._start == boost::none);
    BOOST_CHECK(h._end == boost::none);
    BOOST_CHECK(h._page == boost::make_optional(0));
}

BOOST_AUTO_TEST_CASE(commandline_single_page) {
    int argc = 5;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-S", "./fixtures/output.png",
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(PageCommand));
    auto h = boost::get<PageCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/output.png");
    BOOST_CHECK(h._page == boost::none);
}

BOOST_AUTO_TEST_CASE(commandline_single_page_page) {
    int argc = 7;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-S", "./fixtures/output.png",
        "-p", "0"
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(PageCommand));
    auto h = boost::get<PageCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/output.png");
    BOOST_CHECK(h._page == boost::make_optional(0));
}

BOOST_AUTO_TEST_CASE(commandline_info) {
    int argc = 5;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-i", "./fixtures/info.json",
    };

    auto c = parse_options(argc, argv);
    BOOST_CHECK(c.type() == typeid(InfoCommand));
    auto h = boost::get<InfoCommand>(c);
    BOOST_CHECK(h._config == "./fixtures/example.json");
    BOOST_CHECK(h._path == "./fixtures/info.json");
}

// Throw tests

BOOST_AUTO_TEST_CASE(commandline_throw_general) {
    int argc = 2;
    const char *argv[] = {
        "potoo_test",
        "--asdf"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), boost::program_options::unknown_option);
}

BOOST_AUTO_TEST_CASE(commandline_throw_empty_config) {
    int argc = 2;
    const char *argv[] = {
        "potoo_test",
        "-h"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(commandline_throw_config_only) {
    int argc = 3;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(commandline_throw_single_page_no_path) {
    int argc = 4;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-S"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), boost::program_options::invalid_command_line_syntax);
}

BOOST_AUTO_TEST_CASE(commandline_throw_invalid_ranges_negative) {
    int argc = 7;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-s", "-2",
        "-e", "-4"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(commandline_throw_invalid_ranges_end_lower_than_start) {
    int argc = 7;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-s", "5",
        "-e", "1"
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(commandline_throw_invalid_page_negative) {
    int argc = 5;
    const char *argv[] = {
        "potoo_test",
        "-c", "./fixtures/example.json",
        "-p", "-2",
    };

    BOOST_CHECK_THROW(parse_options(argc, argv), std::runtime_error);
}
