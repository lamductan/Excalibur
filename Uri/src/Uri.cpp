/**
 * @file Uri.cpp
 * 
 * This module contains the implementation of the Uri::Uri class.
 */

#include <Uri/Uri.hpp>

namespace Uri
{
/**
 * This contains the private properties of a Uri instance.
 */
struct Uri::Impl
{
};

Uri::~Uri() = default;

Uri::Uri()
    : impl_(new Impl)
{
}
} // namespace Uri