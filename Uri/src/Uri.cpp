/**
 * @file Uri.cpp
 * 
 * This module contains the implementation of the Uri::Uri class.
 */

#include <Uri/Uri.hpp>
#include <string>
#include <vector>

namespace Uri
{
/**
 * This contains the private properties of a URI instance.
 */
struct Uri::Impl {
    /**
     * This field represents whether the URI has scheme or not.
     */
    bool hasScheme;

    /**
     * This is the "scheme" element of the URI.
     */
    std::string scheme;

    /**
     * This is the "authority" element of the URI.
     */
    std::string authority;

    /**
     * This is the "host" element of the URI.
     */
    std::string host;

    /**
     * This is the "path" element of the URI, which
     * is a vector contains the segments of the path.
     */
    std::vector< std::string > path;

    /**
     * This flag indicates whether or not the URI 
     * has a port number.
     */
    bool hasPort;

    /**
     * This is the "port" element of the URI, if exists.
     */
    uint16_t port;    
};

Uri::~Uri() = default;

Uri::Uri()
    : impl_(new Impl)
{
}

void Uri::reset_impl() 
{
    impl_->hasScheme = false;
    impl_->scheme.clear();
    impl_->authority.clear();
    impl_->host.clear();
    impl_->path.clear();
    impl_->hasPort = false;
    impl_->port = 0;
}

bool Uri::ParseFromString(const std::string &uriString)
{
    reset_impl();
    std::string rest = uriString;
    size_t schemeEnd = rest.find(':');
    if (schemeEnd == std::string::npos) {
        impl_->hasScheme = false;
    } else {
        impl_->hasScheme = true;
        impl_->scheme = rest.substr(0, schemeEnd);
        rest = rest.substr(schemeEnd + 1);
    }

    if (rest.substr(0, 2) == "//") {
        rest = rest.substr(2);
        size_t authorityEnd = rest.find('/');
        impl_->authority = rest.substr(0, authorityEnd);
        if (authorityEnd == std::string::npos) {
            return true;
        }
        rest = rest.substr(authorityEnd + 1);

        size_t hostEnd = impl_->authority.find(':');
        impl_->host = impl_->authority.substr(0, hostEnd);
        if (hostEnd != std::string::npos) {
            std::string portString = impl_->authority.substr(hostEnd + 1);
            uint32_t newPort = 0;
            for(auto c : portString) {
                if (
                    (c < '0')
                    || (c > '9')
                ) {
                    return false;
                }
                newPort = newPort * 10 + uint16_t(c - '0');
                if (newPort & ~((1 << 16) - 1)) {
                    return false;
                }
            }
            impl_->port = (uint16_t)newPort;
            impl_->hasPort = true;
        }
    }

    size_t pathEnd = std::min(rest.find('#'), rest.find('?'));
    std::string pathString = rest.substr(0, pathEnd);
    if (!impl_->authority.empty()) {
        impl_->path.push_back("");
    }
    if (rest == "/") {
        impl_->path.push_back("");
    } else if (!rest.empty()) {
        for(;;) {
            size_t slashPos = rest.find('/');
            impl_->path.push_back(rest.substr(0, slashPos));
            if (slashPos == std::string::npos) {
                break;
            }
            rest = rest.substr(slashPos + 1);
        }
    }
    return true;
}

std::string Uri::GetScheme() const
{
    return impl_->scheme;
}

std::string Uri::GetHost() const
{
    return impl_->host;
}

std::vector<std::string> Uri::GetPath() const
{
    return impl_->path;
}

bool Uri::HasPort() const
{
    return impl_->hasPort;
}

uint16_t Uri::GetPort() const
{
    return impl_->port;
}

} // namespace Uri