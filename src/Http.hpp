// LIBOS LICENCE
//
// GNU Lesser General Public License Version 3.0
//
// Copyright Luke Shore (c) 2020, 2023
//
#include <ctime>
#include <libos/NetIO.h>
#include <ranges>
#include <regex>
#include <string>
// This was wrote from https://developer.mozilla.org/en-US/docs/Web/HTTP
// on (13/02/2023) uk format
namespace libos::http_extention
{
typedef enum HttpResult
{
    HTTP_SUCCESS = LOS_SUCCESS,                                                         //!< see LosResult
    HTTP_ERROR_MALFORMED_DATA = LOS_ERROR_MALFORMED_DATA,                               //!< see LosResult
    HTTP_ERROR_COULD_NOT_INIT = LOS_ERROR_COULD_NOT_INIT,                               //!< see LosResult
    HTTP_ERROR_COULD_NOT_DESTROY = LOS_ERROR_COULD_NOT_DESTROY,                         //!< see LosResult
    HTTP_ERROR_INVALID_FLAGS = LOS_ERROR_INVALID_FLAGS,                                 //!< see LosResult
    HTTP_ERROR_FEATURE_NOT_IMPLEMENTED = LOS_ERROR_FEATURE_NOT_IMPLEMENTED,             //!< see LosResult
    HTTP_NET_IO_DOMAIN_NOT_FOUND = LOS_NET_IO_DOMAIN_NOT_FOUND,                         //!< see LosResult
    HTTP_NET_IO_CONNECTION_REFUSED = LOS_NET_IO_CONNECTION_REFUSED,                     //!< see LosResult
    HTTP_NET_IO_CONNECTION_CLOSED_SERVER_END = LOS_NET_IO_CONNECTION_CLOSED_SERVER_END, //!< see LosResult
    HTTP_BAD_HEADER,
} HttpResult;

enum HttpMethod
{
    METHOD_ERROR,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
};

enum HttpError
{
    ERROR_ERROR = 0,
    CONTINUE = 100,
    SWITCHING_PROTOCOL = 101,
    PROCESSING = 102,
    // 103 Early Hints - Experimental
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTH_INFO = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    ALREADY_REPORTED = 208,
    IM_USED = 226,
    MULTI_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    TEMP_REDIRECTED = 307,
    PERMANENT_REDIRECTED = 308,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTED = 406,
    PROXY_AUTH_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    CONTENT_TO_LARGE = 413,
    URI_TO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418, // This error is a reference to Hyper Text Coffee Pot Control Protocol defined in April Fools'
                       // jokes in 1998 and 2014.
    MISDIRECTED_REQUEST = 421,
    UNPROCESSABLE_ENTITY = 422,
    LOCKED = 423,
    FAILED_DEPENDENCY = 424,
    TOO_EARLY = 425,
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,
    TO_MANY_REQUESTS = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    VARIANT_ALSO_NEGOTIATES = 506,
    INSUFFICIENT_STORAGE = 507,
    LOOP_DETECTED = 508,
    NOT_EXTENDED = 510,
    NET_AUTH_REQUIRED = 511
};

enum HttpVersion
{
    VERSION_ERROR,
    ONE_ZERO,
    ONE_ONE,
    ONE_TWO,
    ONE_THREE,
};

enum HttpOSPlatform
{
    NOT_DEFINED,
    WINDOWS,
    LINUX
};

struct HttpRequest
{
  private:
#if defined(WITH_DEBUG)
    const char *osToString(HttpOSPlatform method)
    {
        switch (method)
        {
        case HttpOSPlatform::WINDOWS:
            return "Windows";
        case HttpOSPlatform::LINUX:
            return "Linux";
        }
        return "Not Defined";
    }
#endif
  public:
    HttpMethod method = HttpMethod::METHOD_ERROR;
    HttpError error = HttpError::ERROR_ERROR;
    std::string url_path;
    HttpVersion version = HttpVersion::VERSION_ERROR;
    HttpOSPlatform platform = HttpOSPlatform::NOT_DEFINED;
    std::string body;
#if defined(WITH_DEBUG)
    void print();
#endif
};

std::vector<std::string> regexLine(std::string source);

HttpResult losReadHTTPSocket(const losSocket handle, HttpRequest *request);

HttpResult losWriteHttpResponse(const losSocket handle, const HttpRequest &request, bool isServer,
                                const char *server_name);
} // namespace libos::http_extention