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

TEST(UriTests, ParseFromStringHasAPortNumber)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://www.example.com:8080/foo/bar"));
  ASSERT_EQ("www.example.com", uri.GetHost());
  ASSERT_TRUE(uri.HasPort());
  ASSERT_EQ(8080, uri.GetPort());
}

TEST(UriTests, ParseFromStringDoesNotHaveAPortNumber)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://www.example.com/foo/bar"));
  ASSERT_EQ("www.example.com", uri.GetHost());
  ASSERT_FALSE(uri.HasPort());
}

TEST(UriTests, ParseFromStringBadPortNumberPurelyAlphabetic)
{
  Uri::Uri uri;
  ASSERT_FALSE(uri.ParseFromString("http://www.example.com:spam/foo/bar"));
}

TEST(UriTests, ParseFromStringStartsNumericEndsAlphabetic)
{
  Uri::Uri uri;
  ASSERT_FALSE(uri.ParseFromString("http://www.example.com:spam/foo/bar"));
}

TEST(UriTests, ParseFromStringLargestNumber)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://www.example.com:65535/foo/bar"));
  ASSERT_TRUE(uri.HasPort());
  ASSERT_EQ(65535, uri.GetPort());
}

TEST(UriTests, ParseFromStringBadPortNumberTooBig)
{
  Uri::Uri uri;
  ASSERT_FALSE(uri.ParseFromString("http://www.example.com:65536/foo/bar"));
}

TEST(UriTests, ParseFromStringBadPortNumberNegative)
{
  Uri::Uri uri;
  ASSERT_FALSE(uri.ParseFromString("http://www.example.com:-8080/foo/bar"));
}