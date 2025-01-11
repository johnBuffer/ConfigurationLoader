#include <fstream>
#include <iostream>
#include <limits>
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

    template<typename TType>
    bool tryReadValueInto(std::string const& key, TType* destination) const
    {
        std::optional<int32_t> const value = getValueAs<TType>(key);
        if (value) {
            *destination = *value;
            return true;
        }
        return false;
    }

    template<>
    bool tryReadValueInto<std::string>(std::string const& key, std::string* destination) const
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

    template<typename TType>
    [[nodiscard]]
    std::optional<TType> getValueAs(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        return tryParse<TType>(it->second);
    }

    template<typename TType>
    [[nodiscard]]
    std::optional<std::vector<TType>> getValueAsVector(std::string const& key) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (!value) {
            return std::nullopt;
        }

        std::string const data = *value + ",";

        std::vector<TType> result;

        auto const tryExtractValue = [this, &data](size_t start, size_t end) -> std::optional<TType> {
            std::string const part = data.substr(start, end - start);
            return tryParse<TType>(part);
        };

        size_t start = 0;
        size_t next_coma = data.find(',');
        while (next_coma != std::string::npos) {
            auto const val = tryExtractValue(start, next_coma);
            if (val) {
                result.push_back(*val);
            } else {
                return std::nullopt;
            }

            start = next_coma + 1;
            next_coma = data.find(',', start);
        }

        return result;
    }

    [[nodiscard]]
    bool isValid() const
    {
        return m_valid;
    }

    operator bool() const
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

    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_floating_point_v<TType>, std::optional<TType>> tryParse(std::string const& s)
    {
        size_t end_idx;
        double const result = std::stod(s, &end_idx);
        if (s[end_idx] != '\0') {
            return std::nullopt;
        }

        return result;
    }

    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_integral_v<TType>, std::optional<TType>> tryParse(std::string const& s)
    {
        try {
            long long const result = std::stoll(s);
            // Ensure the value is within target range
            if (result >= std::numeric_limits<TType>::min() && result <= std::numeric_limits<TType>::max()) {
                return static_cast<TType>(result);
            }
        }
        catch (std::invalid_argument const&) {}
        // If parse failed or value is not within range
        return std::nullopt;
    }

private:
    /// Tells if a valid file has been loaded
    bool m_valid = false;
};
