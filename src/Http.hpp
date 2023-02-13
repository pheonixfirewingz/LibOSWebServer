// LIBOS LICENCE
//
// GNU Lesser General Public License Version 3.0
//
// Copyright Luke Shore (c) 2020, 2023
//
#include <libos/NetIO.h>
#include <libos/extendstd/LookUpTable.h>
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

enum HttpResponce
{
    RESPONCE_ERROR = 0,
    CONTINUE = 100,
    SWITCHING_PROTOCAL = 101,
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
    ALREADY_REPORETED = 208,
    IM_USED = 226,
    MULTI_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    TEMP_REDIRECTED = 307,
    PERMANET_REDIRECTED = 308,
    BAD_REQUEST = 400,
    UNAUTHORDIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTED = 406,
    PROXY_AUTH_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFILICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    CONTENT_TO_LARGE = 413,
    URI_TO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTION_FAILED = 417,
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
    UNAVAILABLE_FOR_LEAGAL_REASONS = 451,
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
    WINDOWS
};

struct HttpRequest
{
  private:
#if defined(WITH_DEBUG)
    const char *methodToString(HttpMethod method)
    {
        switch (method)
        {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::HEAD:
            return "HEAD";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DELETE:
            return "DELETE";
        case HttpMethod::CONNECT:
            return "CONNECT";
        case HttpMethod::OPTIONS:
            return "OPTIONS";
        case HttpMethod::TRACE:
            return "TRACE";
        case HttpMethod::PATCH:
            return "PATCH";
        }
        return "ERROR";
    }

    const char *versionToString(HttpVersion method)
    {
        switch (method)
        {
        case HttpVersion::ONE_ZERO:
            return "1.0";
        case HttpVersion::ONE_ONE:
            return "1.1";
        case HttpVersion::ONE_TWO:
            return "1.2";
        case HttpVersion::ONE_THREE:
            return "1.3";
        }
        return "ERROR";
    }

    const char *osToString(HttpOSPlatform method)
    {
        switch (method)
        {
        case HttpOSPlatform::WINDOWS:
            return "Windows";
        }
        return "Not Defined";
    }
#endif
  public:
    HttpMethod method = HttpMethod::METHOD_ERROR;
    std::string url_path;
    HttpVersion version = HttpVersion::VERSION_ERROR;
    HttpOSPlatform platform = HttpOSPlatform::NOT_DEFINED;
#if defined(WITH_DEBUG)
    void print()
    {
        if (method == METHOD_ERROR || url_path.empty() || version == VERSION_ERROR)
            return;
        printf("HTTP REQUEST\n METHOD: %s\n URL: %s\n VERSION: %s\n BROWSER_OS: %s\n", methodToString(method),
               url_path.c_str(), versionToString(version), osToString(platform));
    }
#endif
};

std::vector<std::string> regexLine(std::string source)
{
    static const std::regex http_regex("(([A-Za-z0-9_-]+)|(\\s)|([:,/]))", std::regex_constants::icase);
    std::vector<std::string> split;
    auto words_begin = std::sregex_token_iterator(source.begin(), source.end(), http_regex);
    auto words_end = std::sregex_token_iterator();
    split.reserve(std::distance(words_begin, words_end));
    std::move(words_begin, words_end, std::back_inserter(split));
    return split;
}

HttpResult losReadHTTPSocket(const losSocket handle, HttpRequest *request)
{
    HttpResult result = HTTP_SUCCESS;
    static const std::ReadOnlyLookupTable<data_size_t, HttpMethod> http_method_lookup = {
        {std::hash<std::string>{}("GET"), HttpMethod::GET},
        {std::hash<std::string>{}("HEAD"), HttpMethod::HEAD},
        {std::hash<std::string>{}("POST"), HttpMethod::POST},
        {std::hash<std::string>{}("PUT"), HttpMethod::PUT},
        {std::hash<std::string>{}("DELETE"), HttpMethod::DELETE},
        {std::hash<std::string>{}("CONNECT"), HttpMethod::CONNECT},
        {std::hash<std::string>{}("OPTIONS"), HttpMethod::OPTIONS},
        {std::hash<std::string>{}("TRACE"), HttpMethod::TRACE},
        {std::hash<std::string>{}("PATCH"), HttpMethod::PATCH},
    };
    static const std::ReadOnlyLookupTable<data_size_t, HttpVersion> http_version_lookup = {
        {std::hash<std::string>{}("HTTP/1.0"), HttpVersion::ONE_ZERO},
        {std::hash<std::string>{}("HTTP/1.1"), HttpVersion::ONE_ONE},
        {std::hash<std::string>{}("HTTP/1.2"), HttpVersion::ONE_TWO},
        {std::hash<std::string>{}("HTTP/1.3"), HttpVersion::ONE_THREE},
    };
    char *buffer = new char[65536];
    if ((result = (HttpResult)losReadSocket(handle, &buffer[0], 65536)) == HTTP_SUCCESS)
    {
        std::vector<std::string> lines;
        uint64_t line = 0;
        {
            std::string source(&buffer[0]);
            for (const auto word : std::views::split(source, '\n'))
                lines.push_back(std::string(word.begin(), word.end()));
            delete[] buffer;
        }
        // Request header
        {
            std::vector<std::string> split = regexLine(lines[line++]);
            uint64_t index = 0;
            auto *method_ptr = http_method_lookup.find(std::hash<std::string>{}(split[index++]));
            if (method_ptr != nullptr)
            {
                request->method = *method_ptr;
                // eat whitespace
                while (split[index] == " " || split[index] == "\r" || split[index] == "\t")
                    index++;
                // define url
                std::string url = "";
                while (split[index] != " ")
                    url += split[index++];

                request->url_path = url;
                // eat whitespace
                while (split[index] == " " || split[index] == "\r" || split[index] == "\t")
                    index++;
                // get version
                if (split[index] == "HTTP")
                {
                    index += 2;
                    int major = std::stoi(split[index++]);
                    int minor = std::stoi(split[index++]);
                    request->version = (HttpVersion)(major + minor);
                }
                else
                    result = HTTP_BAD_HEADER;

                line += 2;
            }
            else
                result = HTTP_BAD_HEADER;
        }
        for (; line < lines.size(); line++)
        {
            if (lines[line] == "\r")
                break;
            if (lines[line].find("sec-ch-ua-platform") != std::string::npos)
            {
                std::vector<std::string> split = regexLine(lines[line++]);
                uint8_t index = 2;
                // eat whitespace
                while (split[index] == " " || split[index] == "\r" || split[index] == "\t")
                    index++;
                if (split[index] == "Windows")
                    request->platform = HttpOSPlatform::WINDOWS;

            }
        }
    }
    else
        delete[] buffer;
    return result;
}
} // namespace libos::http_extention