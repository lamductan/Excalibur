/**
 * @file Uri.hpp
 * 
 * This module declares the Uri::Uri class.
 */

#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

namespace Uri
{
  /**
    * This class represents a Uniform Resource Identifier (URI),
    * as defined in RFC 3986 (https://tools.ietf.org/html/rfc3986).
    */
class Uri
{
  // Lifecycle management
public:
  ~Uri();
  Uri(const Uri &) = delete;
  Uri(Uri &&) = delete;
  Uri &operator=(const Uri &) = delete;
  Uri &operator=(Uri &&) = delete;

  // Public methods
public:
  /**
    * This is the default constructor
    */
  Uri();

  /**
   * This method builds the URI from the elements parsed
   * from the given string rendering of a URI.
   * 
   * @param[in] uriString
   *     This is the string rendering of the URI to parse.
   * 
   * @return
   *     An indication of whether or not the URI was
   *     parsed succesfully is returned
   */
  bool ParseFromString(const std::string& uriString);

  /**
   * This method gets the "scheme" element of the URI.
   * 
   * @return
   *     A string represents the "scheme" of the URI.
   * @retval
   *     An empty string if the URI has no scheme.
   */
  std::string GetScheme() const;

  /**
   * This method gets the "host" element of the URI.
   * 
   * @return
   *     A string represents the "host" of the URI.
   * @retval
   *     An empty string if the URI has no host.
   */
  std::string GetHost() const;

  /**
   * This method gets the "path" of the URI, which is
   * stored as a vector of strings.
   * 
   * @return
   *     A vector of strings represents the path of
   *     the URI.
   * @note
   *     If the first step of the path is an empty string,
   *     then the URI has an absolute path. 
   */ 
  std::vector< std::string > GetPath() const;

  /**
   * This method returns an indication of whether or not the URI
   * has a port number.
   * 
   * @return
   *     An indication of whether or not the URI
   *     includes a port number.
   */
  bool HasPort() const;

  /**
   * This method returns the port number element of the URI,
   * if it has one.
   * 
   * @return
   *     The port number element of the URI is returned.
   * 
   * @note
   *     The returned port number is only valid if the
   *     hasPort method returns true.
   */
  uint16_t GetPort() const;
  
  // Private properties
private:
  /**
    * This is the type of structure that contains the private
    * properties of the instance. It is defined in the implementation
    * and declared here to ensure that it is scoped inside the class.
    */
  struct Impl;

  /**
    * This contains the private properties of the instance.
    */
  std::unique_ptr<struct Impl> impl_;

  /**
   * This method resets the data of Uri before parse a new Uri string.
   */
  void reset_impl();
};
} // namespace Uri