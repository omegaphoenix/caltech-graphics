#define BOOST_TEST_MODULE Tests
#include <boost/test/included/unit_test.hpp>

#include "../src/three_d_model_transform.hpp"
#include "../src/transform_obj.hpp"

BOOST_AUTO_TEST_CASE(simple_test) {
  BOOST_CHECK_EQUAL(2+2, 4);
}

BOOST_AUTO_TEST_CASE(parse_scene_desc_file_test) {
  BOOST_CHECK_EQUAL(2+2, 4);
}
