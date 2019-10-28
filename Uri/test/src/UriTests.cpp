/**
 * @file UriTests.cpp
 * 
 * This module contains the unit tests of the Uri::Uri class
 */

#include <gtest/gtest.h>
#include <Uri/Uri.hpp>

#include <string>
#include <vector>

struct TestPath {
  std::string input;
  std::vector< std::string > expectedOutput;
};

TEST(UriTests, ParseFromString)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar"));
  ASSERT_EQ("http", uri.GetScheme());
  ASSERT_EQ("www.example.com", uri.GetHost());
  ASSERT_EQ((std::vector< std::string >{"", "foo", "bar"}), uri.GetPath());
}

TEST(UriTests, ParseFromStringPathCornerCase)
{
  std::vector< TestPath > testVector {
    {"", {}},
    {"/", {""}},
    {"/foo", {"", "foo"}},
    {"foo/", {"foo", ""}}
  };
  Uri::Uri uri;
  for(const auto &pair : testVector) {
    ASSERT_TRUE(uri.ParseFromString(pair.input));
    ASSERT_EQ(pair.expectedOutput, uri.GetPath());
  }
}