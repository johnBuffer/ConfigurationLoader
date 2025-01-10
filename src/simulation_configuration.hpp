#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <fstream>


struct SimulationConfigurationLoader
{
    std::map<std::string, std::string> entries;

    explicit
    SimulationConfigurationLoader(std::string const& filename)
    {
        loadFromFile(filename);
    }

    void loadFromFile(std::string const& filename)
    {
        std::ifstream infile(filename);

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

        char* end_ptr;
        int const result = strtol(it->second.c_str(), &end_ptr, 10);
        if (*end_ptr != '\0') {
            return std::nullopt;
        }

        return result;
    }

    [[nodiscard]]
    std::optional<float> getValueAsFloat(std::string const& key) const
    {
        auto const it = entries.find(key);
        if (it == entries.end()) {
            return std::nullopt;
        }

        std::string const& value_str{it->second};
        size_t end_idx;
        float const result = std::stof(value_str, &end_idx);
        if (value_str[end_idx] != '\0') {
            return std::nullopt;
        }

        return result;
    }

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
};
