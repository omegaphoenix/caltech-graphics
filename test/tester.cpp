#define BOOST_TEST_MODULE Tests
#include <boost/test/included/unit_test.hpp>
#include <memory> // shared_ptr
#include <sstream>
#include <string>

// #include "parser.hpp"

BOOST_AUTO_TEST_CASE(simple_test) {
  BOOST_CHECK_EQUAL(2+2, 4);
}

BOOST_AUTO_TEST_CASE(parse_obj_file_test) {
  // string test_file = "../data/cube.obj".c_str();
  // ThreeDModelPtr model = parse_file_to_model(test_file.c_str());
  // VertexPtr ver = model->vertices->front();
  // BOOST_CHECK(ver == NULL);
}

BOOST_AUTO_TEST_CASE(parse_scene_desc_file_test) {
}
