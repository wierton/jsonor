#include "json.h"
#include <cassert>
#include <iostream>

/* testcase comes from
 * https://github.com/MichaelSuen-thePointer/SimpleJSON/blob/master/SimpleJSON/main.cpp
 */

#define AUTO_TEST_CASE(F)                                  \
  namespace wt_test {                                      \
  void F();                                                \
  int result_of_##F = (F(), 0);                            \
  }                                                        \
  void wt_test::F()

AUTO_TEST_CASE(json_ctor_dtor_test) {
  Json simple = 1;
  Json copy = simple;
  Json move = std::move(simple);

  Json complex = Json::Object{{"key", "value"}};
  Json ccopy = complex;
  Json cmove = std::move(complex);

  ccopy = ccopy;
  ccopy = cmove;
  cmove = std::move(ccopy);
  cmove = std::move(cmove);
}

AUTO_TEST_CASE(json_construction_and_readonly_test) {
  Json i = 1;
  assert(i.to_integer() == 1);
  assert(i.to_float() == 1);

  Json d = 1.2;
  assert(d.to_integer() == 1);
  assert(d.to_float() == 1.2);

  Json b = true;
  assert(b.to_boolean());

  Json nul;
  assert(nul.is_null());

  Json str = "hello world";
  assert(str.as_string() == "hello world");

  Json arr = Json::Array{1, 1.2, true, Json::null};
  assert(arr[0].to_integer() == 1);
  assert(arr[1].to_float() == 1.2);
  assert(arr[2].to_boolean());
  assert(arr[3].is_null());

  Json obj = Json::Object{{"int", 1},
                          {"double", 2.3},
                          {"nul", Json::null},
                          {"bool", false}};
  assert(obj["int"].to_integer() == 1);
  assert(obj["double"].to_float() == 2.3);
  assert(obj["nul"].is_null());
  assert(obj["bool"].to_boolean() == false);
}

AUTO_TEST_CASE(json_copy_and_equality_test) {
  Json arr = Json::Array{
      Json::Object{
          {"int", 2},
          {"double", 1.2},
          {"arr", Json::Array{true, false, 1.2, Json::null,
                              "hello"}}},
      4.4, false, Json::null};

  Json copy = arr;
  assert((copy[0]["int"] == 2));
  assert((copy[0]["double"] == 1.2));
  assert((copy[0]["arr"][0] == true));
  assert((copy[0]["arr"][1] == false));
  assert((copy[0]["arr"][2] == 1.2));
  assert((copy[0]["arr"][3] == nullptr));
  assert((copy[0]["arr"][4] == "hello"));
  assert((copy[1] == 4.4));
  assert((copy[2] == false));
  assert((copy[3] == nullptr));
}

AUTO_TEST_CASE(json_edit_test) {
  Json doc;
  assert((doc == nullptr));

  doc = Json::Array{1, 2, 3, 4, 5}; // reset null to array
  auto &arr = doc.as_array();
  for (int i = 0; i < 5; i++) {
    assert((arr[i] == i + 1));
  }

  doc[0] = Json::Object{
      {"int", 1}, {"str", "2"}}; //reset int to object
  auto &obj = doc[0].as_object();
  assert((obj.find("int")->second == 1));
  assert((obj.find("str")->second == "2"));

  doc[0]["int"] = nullptr; // reset object to null
  assert((doc[0]["int"] == nullptr));

  doc[0]["not_exist_key"] =
      2.3; // if key is not exist, add the key
  assert((doc[0]["not_exist_key"] == 2.3));

  doc[9] = 2.4; // if index is not exist, expand the array
                // and fill the new entry with null
  for (int i = 5; i < 9; i++) {
    assert((doc[i] == nullptr));
  }
  assert((doc[9] == 2.4));

  doc[5]["new"] = 1; // use [] on a non-object/non-array
                     // term will convert it to the
                     // corresponding object
  assert((doc[5].is_object()));
  assert((doc[5]["new"] == 1));

  /*
doc[5][1] = 2;
assert((doc[5].is_array()));
assert((doc[5][1] == 2));
  */

  assert((doc[111] ==
          nullptr)); // new initialized value is null
  // assert((doc["obj"] == nullptr));

  doc = nullptr;
}

AUTO_TEST_CASE(json_string_escape_sequence_test) {
  std::string err;
  auto _1 = Json::parse(R"("")", err);
  assert((_1.as_string() == ""));
  assert((err == ""));

  auto _2 =
      Json::parse(R"("\" \\ \/ \b \f \n \r \t")", err);
  assert((_2.as_string() == "\" \\ / \b \f \n \r \t"));
  assert((err == ""));

  // auto _3 = Json::parse(R"("\u0024 \u20AC \uD801\uDC37
  // \uD852\uDF62")");
  auto _3 = Json::parse(
      R"("\u0024 \u20AC \uD801\uDC37 \uD852\uDF62")");
  // assert((_3.as_string() == "\x24 \xe2\x82\xac
  // \xf0\x90\x90\xb7 \xf0\xa4\xad\xa2"));
  assert((err == ""));
}

AUTO_TEST_CASE(json_number_parse_test) {
  auto _1 = Json::parse("1.4");
  assert((_1 == 1.4));

  auto _2 = Json::parse("-1.4");
  assert((_2 == -1.4));

  auto _3 = Json::parse("-0");
  assert((_3 == 0));

  auto _4 = Json::parse("0");
  assert((_4 == 0));

  auto _5 = Json::parse("0.4");
  assert((_5 == 0.4));

  auto _6 = Json::parse("-0.4");
  assert((_6 == -0.4));

  auto _7 = Json::parse("-23.4e+10");
  assert((_7 == -23.4e+10));

  auto _8 = Json::parse("0.1e-5");
  assert((_8 == 0.1E-5));

  auto _9 = Json::parse("0.1e5");
  assert((_9 == 0.1e5));
}

AUTO_TEST_CASE(json_big_number_test) {
  auto j = Json::parse(
      "100000000000000000000000000"); // 1e26 fallsback to
                                      // double
  // assert((j == 1e26));
}

AUTO_TEST_CASE(json_parser_test) {
  std::string err;
  auto _1 = Json::parse(R"(
{
    "int" : 1,
    "double" : 1.2,
    "string" : "str123",
    "arr" : [1, 2.2, true, false, null, {}, []]
}
)",
                        err);

  std::clog << _1 << ", err is " << err << std::endl;
  assert((_1["int"] == 1));
  assert((_1["double"] == 1.2));
  assert((_1["string"] == "str123"));
  assert((_1["arr"].is_array()));
  assert((_1["arr"][0] == 1));
  assert((_1["arr"][1] == 2.2));
  assert((_1["arr"][2] == true));
  assert((_1["arr"][3] == false));
  assert((_1["arr"][4] == nullptr));
  assert((_1["arr"][5].is_object()));
  assert((_1["arr"][6].is_array()));
}
