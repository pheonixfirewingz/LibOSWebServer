#include "Http.hpp"

namespace libos::http_extention
{

std::vector<std::string> regexLine(std::string source)
{
    static const std::regex http_regex("(([A-Za-z0-9_-]+)|(\\s)|([:,/,.]))", std::regex_constants::icase);
    std::vector<std::string> split;
    auto words_begin = std::sregex_token_iterator(source.begin(), source.end(), http_regex);
    auto words_end = std::sregex_token_iterator();
    split.reserve(std::distance(words_begin, words_end));
    std::move(words_begin, words_end, std::back_inserter(split));
    return split;
}

[[nodiscard]] const char *writeVersion(const HttpVersion version) noexcept
{
    switch (version)
    {
    case HttpVersion::ONE_ZERO:
        return "HTTP/1.0";
    case HttpVersion::ONE_ONE:
        return "HTTP/1.1";
    case HttpVersion::ONE_TWO:
        return "HTTP/1.2";
    case HttpVersion::ONE_THREE:
        return "HTTP/1.3";
    default:
        return "error";
    }
}

[[nodiscard]] const char *writeMethod(const HttpMethod method) noexcept
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
    default:
        return "error";
    }
}

[[nodiscard]] const char *writeCode(const HttpError error) noexcept
{
    switch (error)
    {
    case CONTINUE:
        return "100 Continue";
    case SWITCHING_PROTOCOL:
        return "101 Switching Protocol";
    case PROCESSING:
        return "102 Processing";
    case OK:
        return "200 OK";
    case CREATED:
        return "201 Created";
    case ACCEPTED:
        return "202 Accepted";
    case NON_AUTH_INFO:
        return "203 Non-Authoritative Information";
    case NO_CONTENT:
        return "204 No Content";
    case RESET_CONTENT:
        return "205 Reset Content";
    case PARTIAL_CONTENT:
        return "206 Partial Content";
    case MULTI_STATUS:
        return "207 Multi-Status";
    case ALREADY_REPORTED:
        return "208 Already Reported";
    case IM_USED:
        return "226 I'm Used";
    case MULTI_CHOICES:
        return "300 Multiple Choices";
    case MOVED_PERMANENTLY:
        return "301 Moved Permanently";
    case FOUND:
        return "302 Found";
    case SEE_OTHER:
        return "303 See Other";
    case NOT_MODIFIED:
        return "304 Not Modified";
    case TEMP_REDIRECTED:
        return "307 Temporarily Redirected";
    case PERMANENT_REDIRECTED:
        return "308 Permanent Redirect";
    case BAD_REQUEST:
        return "400 Bad Request";
    case UNAUTHORIZED:
        return "401 Unauthorized";
    case PAYMENT_REQUIRED:
        return "402 Payment Required";
    case FORBIDDEN:
        return "403 Forbidden";
    case NOT_FOUND:
        return "404 Not Found";
    case METHOD_NOT_ALLOWED:
        return "405 Method Not Allowed";
    case NOT_ACCEPTED:
        return "406 Not Acceptable";
    case PROXY_AUTH_REQUIRED:
        return "407 Proxy Authentication Required";
    case REQUEST_TIMEOUT:
        return "408 Request Timeout";
    case CONFLICT:
        return "409 Conflict";
    case GONE:
        return "410 Gone";
    case LENGTH_REQUIRED:
        return "411 Length Required";
    case PRECONDITION_FAILED:
        return "412 Precondition Failed";
    case CONTENT_TO_LARGE:
        return "413 Content to Large";
    case URI_TO_LONG:
        return "414 URI Too Long";
    case UNSUPPORTED_MEDIA_TYPE:
        return "415 Unsupported Media Type";
    case RANGE_NOT_SATISFIABLE:
        return "416 Range Not Satisfiable";
    case EXPECTATION_FAILED:
        return "417 Expectation Failed";
    case IM_A_TEAPOT:
        return "418 I'm a Teapot";
    case MISDIRECTED_REQUEST:
        return "421 Misdirected Request";
    case UNPROCESSABLE_ENTITY:
        return "422 Unprocessable Entity";
    case LOCKED:
        return "423 Locked";
    case FAILED_DEPENDENCY:
        return "424 Failed Dependency";
    case TOO_EARLY:
        return "425 Too Early";
    case UPGRADE_REQUIRED:
        return "426 Upgrade Required";
    case PRECONDITION_REQUIRED:
        return "428 Precondition Required";
    case TO_MANY_REQUESTS:
        return "429 Too Many Requests";
    case REQUEST_HEADER_FIELDS_TOO_LARGE:
        return "431 Request Header Fields Too Large";
    case UNAVAILABLE_FOR_LEGAL_REASONS:
        return "451 Unavailable For Legal Reasons";
    case INTERNAL_SERVER_ERROR:
        return "500 Internal Server Error";
    case NOT_IMPLEMENTED:
        return "501 Not Implemented";
    case BAD_GATEWAY:
        return "502 Bad Gateway";
    case SERVICE_UNAVAILABLE:
        return "503 Service Unavailable";
    case GATEWAY_TIMEOUT:
        return "504 Gateway Timeout";
    case HTTP_VERSION_NOT_SUPPORTED:
        return "505 HTTP Version Not Supported";
    case VARIANT_ALSO_NEGOTIATES:
        return "506 Variant Also Negotiates";
    case INSUFFICIENT_STORAGE:
        return "507 Insufficient Storage";
    case LOOP_DETECTED:
        return "508 Loop Detected";
    case NOT_EXTENDED:
        return "510 Not Extended";
    case NET_AUTH_REQUIRED:
        return "511 Net Authentication Required";
    default:
        return "error";
    }
}

#if defined(WITH_DEBUG)
void HttpRequest::print()
{
    if (method == METHOD_ERROR || url_path.empty() || version == VERSION_ERROR)
            return;
        printf("HTTP REQUEST\n METHOD: %s\n URL: %s\n VERSION: %s\n BROWSER_OS: %s\n\n\n", writeMethod(method),
               url_path.c_str(), writeVersion(version), osToString(platform));
        if (!body.empty())
            printf("BODY: %s\n", body.c_str());
}
#endif

HttpResult losWriteHttpResponse(const losSocket handle, const HttpRequest &request, bool is_server,
                                const char *server_name)
{
    HttpResult result = HTTP_SUCCESS;
    //http version
    std::string buffer = writeVersion(request.version);
    buffer += " ";
    buffer += "\r\n";
    //current time
    std::time_t current_time = std::time(nullptr);
    std::tm *time_info = std::localtime(&current_time);
    char buf[80];
    std::strftime(buf, 80, "%a, %d %b %Y %H:%M:%S GMT", time_info);
    buffer += "Date: " + std::string(buf) + "\r\n";
    if (is_server)
        buffer += "Server: " + std::string(server_name) + "\r\n";

    buffer += "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n";
    //body size
    buffer += "Content-Length: ";
    buffer += std::to_string(request.body.size());
    buffer += "\r\n";
    //body type
    buffer += "Content-Type: text/html\r\n";
    buffer += "Connection: Closed\r\n\r\n";
    //body
    if(!request.body.empty())
        buffer += request.body;

    printf("HTTP WRITE:\r\n%s", buffer.c_str());
    if ((result = (HttpResult)losWriteSocket(handle, buffer.c_str(), buffer.size())) != HTTP_SUCCESS)
        return result;

    return result;
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
        // printf("BROWSER DATA:\n%s\n\n", buffer);
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
                    index++;
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
                else if (split[index] == "Linux")
                    request->platform = HttpOSPlatform::LINUX;
            }
            if (lines[line].find("User-Agent") != std::string::npos)
            {
                if (request->platform == HttpOSPlatform::NOT_DEFINED)
                {
                    if (lines[line].find("Windows") != std::string::npos)
                        request->platform = HttpOSPlatform::WINDOWS;
                    else if (lines[line].find("Linux") != std::string::npos)
                        request->platform = HttpOSPlatform::LINUX;
                }
            }
        }
        if (request->method == HttpMethod::POST || request->method == HttpMethod::PUT ||
            request->method == HttpMethod::PATCH)
        {
            for (size_t i = 0; i < lines.size(); i++)
            {
                request->body += lines[i];
                request->body += "\r\n";
            }
        }
    }
    else
        delete[] buffer;
    return result;
}
} // namespace libos::http_extention