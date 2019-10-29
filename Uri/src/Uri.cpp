/**
 * @file Uri.cpp
 * 
 * This module contains the implementation of the Uri::Uri class.
 */

#include <Uri/Uri.hpp>
#include <string>
#include <vector>

namespace {
/**
 * This function parses the given string as an unsigned 16-bit 
 * integer, detecting invalid characters, overflow, etc.
 * 
 * @param[in] numberString
 *     This is the string containing the number to parse.
 * 
 * @param[out]
 *     This is where to store the number parsed.
 * 
 * @return
 *     An indication of whether or not the number was parsed
 *     successfully is returned.
 */
bool ParseUint16(const std::string& numberString, uint16_t& number) {
    uint32_t number32Bits = 0;
    for(auto c : numberString) {
        if ((c < '0')|| (c > '9')) {
            return false;
        }
        number32Bits = number32Bits * 10 + uint16_t(c - '0');
        if (number32Bits & ~((1 << 16) - 1)) {
            return false;
        }
    }
    number = (uint16_t)number32Bits;
    return true;
}
}

namespace Uri
{
/**
 * This contains the private properties of a URI instance.
 */
struct Uri::Impl {
    // Properties
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

    /**
     * This is the "query" element of the URI.
     */
    std::string query;

    /**
     * This is the "fragment" element of the URI.
     */
    std::string fragment;

    /**
     * This is the "user info" element of the URI.
     */
    std::string userInfo;

    // Methods

    /**
     * This method builds the internal path element sequence
     * by parsing it from the given path string.
     * 
     * @param[in]
     *     This is the string containing the whole path of the URI.
     * 
     * @return
     *     An indication if the path was parsed successfully or not
     *     is returned.
     */
    bool ParsePath(std::string pathString) {
        if (pathString == "/") {
            path.push_back("");
        } else if (!pathString.empty()) {
            for(;;) {
                size_t slashPos = pathString.find('/');
                path.push_back(pathString.substr(0, slashPos));
                if (slashPos == std::string::npos) {
                    break;
                }
                pathString = pathString.substr(slashPos + 1);
            }
        }
        return true;
    }
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
    impl_->query.clear();
    impl_->fragment.clear();
    impl_->userInfo.clear();
}

bool Uri::ParseFromString(const std::string &uriString)
{
    // Reset URI before parse new URI string
    reset_impl();

    // Parse the scheme
    std::string rest = uriString;
    size_t schemeEnd = rest.find(':');
    if (schemeEnd == std::string::npos) {
        impl_->hasScheme = false;
    } else {
        impl_->hasScheme = true;
        impl_->scheme = rest.substr(0, schemeEnd);
        rest = rest.substr(schemeEnd + 1);
    }

    size_t pathEnd = rest.find_first_of("?#");
    std::string authorityAndPathString = rest.substr(0, pathEnd);
    std::string hostPortAndPathString;
    std::string pathString;
    std::string queryAndOrFragment;

    // Next parse the authority
    if (authorityAndPathString.substr(0, 2) == "//") {
        // Strip the authority marker.
        authorityAndPathString = authorityAndPathString.substr(2);

        // First separate the authority from the path.
        size_t authorityEnd = authorityAndPathString.find('/');
        if (authorityEnd == std::string::npos) {
            authorityEnd = authorityAndPathString.length();
        }

        // Next, check if there is a UserInfo, and if so, extract it.
        size_t userInfoDelimiter = authorityAndPathString.find('@');
        if (userInfoDelimiter == std::string::npos) {
            hostPortAndPathString = authorityAndPathString;
        } else {
            impl_->userInfo = authorityAndPathString.substr(
                0, userInfoDelimiter);
            hostPortAndPathString = authorityAndPathString.substr(
                userInfoDelimiter + 1);
        }

        // Next, parse the host and port of the authority.
        impl_->authority = hostPortAndPathString.substr(0, authorityEnd);
        size_t hostEnd = impl_->authority.find(':');
        impl_->host = impl_->authority.substr(0, hostEnd);
        if (hostEnd != std::string::npos) {
            std::string portString = impl_->authority.substr(hostEnd + 1);
            if (!ParseUint16(portString, impl_->port)) {
                return false;
            }
            impl_->hasPort = true;
        }
        pathString = authorityAndPathString.substr(authorityEnd);
    } else {
        pathString = authorityAndPathString;
    }

    // Next, parse the path.
    if (!impl_->ParsePath(pathString)) {
        return false;
    }

    // Finally, parse the query and the fragment.
    if (pathEnd != std::string::npos) {
        queryAndOrFragment = rest.substr(pathEnd);
        size_t fragmentStart = queryAndOrFragment.find('#');
        if (fragmentStart != std::string::npos) {
            impl_->query = queryAndOrFragment.substr(0, fragmentStart);
            impl_->fragment = queryAndOrFragment.substr(fragmentStart + 1);
        } else {
            impl_->query = queryAndOrFragment;
        }
        if (impl_->query.length() > 0) {
            impl_->query = impl_->query.substr(1);
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

bool Uri::IsRelativeReference() const
{
    return impl_->scheme.empty();
}

bool Uri::ContainsRelativePath() const
{
    if (impl_->path.empty()) {
        return true;
    } else {
        return !impl_->path[0].empty();
    } 
}

std::string Uri::GetQuery() const
{
    return impl_->query;
}

std::string Uri::GetFragment() const
{
    return impl_->fragment;
}

std::string Uri::GetUserInfo() const
{
    return impl_->userInfo;
}

} // namespace Uri