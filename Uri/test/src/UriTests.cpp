/**
 * @file UriTests.cpp
 * 
 * This module contains the unit tests of the Uri::Uri class
 */

#include <gtest/gtest.h>
#include <Uri/Uri.hpp>

#include <string>
#include <vector>

TEST(UriTests, ParseFromStringNoScheme)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("foo/bar"));
  ASSERT_EQ("", uri.GetScheme());
  ASSERT_EQ((std::vector< std::string >{"foo", "bar"}), uri.GetPath());
}

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
  struct TestVector {
    std::string input;
    std::vector< std::string > expectedOutput;
  };
  std::vector< TestVector > testVector {
    {"", {}},
    {"/", {""}},
    {"/foo", {"", "foo"}},
    {"foo/", {"foo", ""}},
    {"http://www.example.com", {}},
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

TEST(UriTests, ParseFromStringEndsAfterAuthority)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://www.example.com"));
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativeReferences)
{
  struct TestVector {
    std::string uriString;
    bool isRelativeReference;
  };
  std::vector< TestVector > testVector {
    {"http://www.example.com/", false},
    {"http://www.example.com", false},
    {"/", true},
    {"foo", true}
  };
  Uri::Uri uri;
  for(const auto &pair : testVector) {
    ASSERT_TRUE(uri.ParseFromString(pair.uriString));
    ASSERT_EQ(pair.isRelativeReference, uri.IsRelativeReference());
  }
}

TEST(UriTests, ParseFromStringRelativeVsNonRelativePaths)
{
  struct TestVector {
    std::string uriString;
    bool isRelativePath;
  };
  std::vector< TestVector > testVector {
    //{"http://www.example.com/", false},
    {"http://www.example.com", true},
    {"/", false},
    {"foo", true},
    {"", true}
  };
  Uri::Uri uri;
  for(const auto &pair : testVector) {
    ASSERT_TRUE(uri.ParseFromString(pair.uriString));
    ASSERT_EQ(pair.isRelativePath, uri.ContainsRelativePath());
  }
}

TEST(UriTests, ParseFromStringQueryAndFragments)
{
  struct TestVector {
    std::string uriString;
    std::string host;
    std::string query;
    std::string fragment;
  };
  std::vector< TestVector > testVector {
    {"http://www.example.com/", "www.example.com", "", ""},
    {"http://www.example.com?foo", "www.example.com", "foo", ""},
    {"http://www.example.com#foo", "www.example.com", "", "foo"},
    {"http://www.example.com?foo#bar", "www.example.com", "foo", "bar"},
    {"http://www.example.com/spam?foo#bar", "www.example.com", "foo", "bar"},
    {"http://www.example.com?earth?day#bar", "www.example.com", "earth?day", "bar"},
  };
  Uri::Uri uri;
  for(const auto &pair : testVector) {
    ASSERT_TRUE(uri.ParseFromString(pair.uriString));
    ASSERT_EQ(pair.host, uri.GetHost());
    ASSERT_EQ(pair.query, uri.GetQuery());
    ASSERT_EQ(pair.fragment, uri.GetFragment());
  }
}

TEST(UriTests, ParseFromStringUserInfo)
{
  struct TestVector {
    std::string uriString;
    std::string userInfo;
  };
  std::vector< TestVector > testVector {
    {"http://www.example.com/", ""},
    {"http://joe@www.example.com", "joe"},
    {"//example.com", ""},
    {"//bob@www.example.com", "bob"},
    {"/", ""},
    {"foo", ""}
  };
  Uri::Uri uri;
  for(const auto &pair : testVector) {
    ASSERT_TRUE(uri.ParseFromString(pair.uriString));
    ASSERT_EQ(pair.userInfo, uri.GetUserInfo());
  }
}

TEST(UriTests, ParseFromStringTwiceFirstUserInfoThemWithout)
{
  Uri::Uri uri;
  ASSERT_TRUE(uri.ParseFromString("http://joe@www.example.com/foo/bar"));
  ASSERT_TRUE(uri.ParseFromString("www.example.com/foo/bar"));
  ASSERT_TRUE(uri.GetUserInfo().empty());
}