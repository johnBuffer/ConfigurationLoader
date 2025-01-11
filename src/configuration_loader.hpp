#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>


struct ConfigurationLoader
{
public:
    std::map<std::string, std::string> entries;

    explicit
    ConfigurationLoader(std::string const& filename)
    {
        loadFromFile(filename);
    }

    /// Loads the provided file and extract key value pairs from it
    void loadFromFile(std::string const& filename)
    {
        m_valid = false;
        std::ifstream infile(filename);
        // If the file could not be opened, stop here. The loader is invalid
        if (!infile) {
            return;
        }

        m_valid = true;

        std::string line;
        while (std::getline(infile, line))
        {
            auto const pair = extractPair(extractPayload(line));
            if (pair.first.empty() || pair.second.empty()) {
                continue;
            } else {
                entries[pair.first] = pair.second;
            }
        }
    }

    bool tryReadValueInto(std::string const& key, uint32_t* destination) const
    {
        std::optional<int32_t> const value = getValueAsInt(key);
        if (value) {
            *destination = static_cast<uint32_t>(*value);
            return true;
        }
        return false;
    }

    bool tryReadValueInto(std::string const& key, int32_t* destination) const
    {
        std::optional<int32_t> const value = getValueAsInt(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    bool tryReadValueInto(std::string const& key, float* destination) const
    {
        std::optional<float> const value = getValueAsFloat(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    bool tryReadValueInto(std::string const& key, std::string* destination) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    [[nodiscard]]
    std::optional<std::string> getValueAsString(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    [[nodiscard]]
    std::optional<int32_t> getValueAsInt(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return tryConvertToInt(it->second);
    }

    [[nodiscard]]
    std::optional<float> getValueAsFloat(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return tryConvertToFloat(it->second);
    }

    [[nodiscard]]
    std::optional<std::vector<int32_t>> getValueAsIntVector(std::string const& key) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (!value) {
            return std::nullopt;
        }

        std::string const& data = *value;

        std::vector<int32_t> result;

        size_t start = 0;
        size_t next_coma = data.find(',');
        while (next_coma != std::string::npos) {
            std::string const part = data.substr(start, next_coma);
            auto const val = tryConvertToInt(part);
            if (val) {
                result.push_back(*val);
            } else {
                return std::nullopt;
            }
        }

        return result;
    }

    [[nodiscard]]
    bool isValid() const
    {
        return m_valid;
    }

private:
    /// Returns the non commented portion of the line
    static std::string extractPayload(std::string const& s)
    {
        size_t const hashtag = s.find('#');
        if (hashtag == std::string::npos) {
            std::cout << "No comment in \"" << s << "\"" << std::endl;
            return s;
        } else {
            std::string payload = s.substr(0, hashtag);
            std::cout << "Payload of \"" << s << "\" is \"" << payload << "\"" << std::endl;
            return payload;
        }
    }

    /// Extract key = value pair
    static std::pair<std::string, std::string> extractPair(std::string const& s)
    {
        size_t const equal = s.find('=');
        if (equal == std::string::npos) {
            std::cout << "No payload in \"" << s << "\"" << std::endl;
            return {"", ""};
        }

        return {strip(s.substr(0, equal)), strip(s.substr(equal+1))};
    }

    /// Removes all spaces from the provided string
    static std::string strip(std::string const& s)
    {
        return std::regex_replace(s, std::regex(" "), "");
    }

    static std::optional<int32_t> tryConvertToInt(std::string const& s)
    {
        char* end_ptr;
        int const result = strtol(s.c_str(), &end_ptr, 10);
        if (*end_ptr != '\0') {
            return std::nullopt;
        }
        return result;
    }

    static std::optional<float> tryConvertToFloat(std::string const& s)
    {
        size_t end_idx;
        float const result = std::stof(s, &end_idx);
        if (s[end_idx] != '\0') {
            return std::nullopt;
        }

        return result;
    }

    template<typename TType>
    std::optional<TType> tryConvert(std::string const& s);

    template<>
    std::optional<float> tryConvert<float>(std::string const& s)
    {
        size_t end_idx;
        float const result = std::stof(s, &end_idx);
        if (s[end_idx] != '\0') {
            return std::nullopt;
        }

        return result;
    }

    template<>
    std::optional<int32_t> tryConvert<int32_t>(std::string const& s)
    {
        size_t end_idx;
        float const result = std::stof(s, &end_idx);
        if (s[end_idx] != '\0') {
            return std::nullopt;
        }

        return result;
    }

private:
    /// Tells if a valid file has been loaded
    bool m_valid = false;
};
