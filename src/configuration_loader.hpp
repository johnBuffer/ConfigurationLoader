#include <array>
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

    template<typename TType, size_t COUNT>
    bool tryReadValueIntoArray(std::string const& key, TType (&array)[COUNT]) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (!value) {
            return false;
        }

        std::string const& data = *value;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                array[i] = *val;
            } else {
                return false;
            }
        }

        return true;
    }

    template<size_t COUNT, typename TType>
    bool tryReadValueIntoArray(std::string const& key, TType array[]) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (!value) {
            return false;
        }

        std::string const& data = *value;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                array[i] = *val;
            } else {
                return false;
            }
        }

        return true;
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

        std::string const& data = *value;
        std::vector<TType> result;
        size_t start = 0;
        while (auto const val = getNext<TType>(data, &start)) {
            result.push_back(*val);
        }

        return result;
    }

    template<typename TType, size_t COUNT>
    [[nodiscard]]
    std::optional<std::array<TType, COUNT>> getValueAsArray(std::string const& key) const
    {
        std::optional<std::string> const value = getValueAsString(key);
        if (!value) {
            return std::nullopt;
        }

        std::string const& data = *value;
        std::array<TType, COUNT> result;
        size_t start = 0;
        for (size_t i{0}; i < COUNT; ++i) {
            auto const val = getNext<TType>(data, &start);
            if (val) {
                result[i] = *val;
            } else {
                return std::nullopt;
            }
        }

        return result;
    }

    template<typename TType>
    [[nodiscard]]
    std::optional<TType> getNext(std::string const& sequence, size_t* pos) const
    {
        size_t const start = *pos;
        // Stop if we are done
        if (start == std::string::npos) {
            return std::nullopt;
        }

        // Else search for the next part to parse
        size_t const coma  = sequence.find(',', start);
        if (coma != std::string::npos) {
            *pos = coma + 1;
            return tryParse<TType>(sequence.substr(start, coma - start));
        }
        *pos = std::string::npos;
        return tryParse<TType>(sequence.substr(start));
    }

    [[nodiscard]]
    bool isValid() const
    {
        return m_valid;
    }

    operator bool() const // NOLINT(*-explicit-constructor)
    {
        return m_valid;
    }

private:
    /// Returns the non commented portion of the line
    static std::string extractPayload(std::string const& s)
    {
        size_t const hashtag = s.find('#');
        if (hashtag == std::string::npos) {
            return s;
        } else {
            std::string payload = s.substr(0, hashtag);
            return payload;
        }
    }

    /// Extract key = value pair
    static std::pair<std::string, std::string> extractPair(std::string const& s)
    {
        size_t const equal = s.find('=');
        if (equal == std::string::npos) {
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
    std::enable_if_t<std::is_floating_point_v<TType>, std::optional<TType>> tryParse(std::string const& s) const
    {
        try {
            double const result = std::stod(s);
            // Ensure the value is within target range
            if (result >= std::numeric_limits<TType>::min() && result <= std::numeric_limits<TType>::max()) {
                return static_cast<TType>(result);
            }
        }
        catch (std::invalid_argument const&) {}
        // If parse failed or value is not within range
        return std::nullopt;
    }

    template<typename TType>
    [[nodiscard]]
    std::enable_if_t<std::is_integral_v<TType>, std::optional<TType>> tryParse(std::string const& s) const
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
