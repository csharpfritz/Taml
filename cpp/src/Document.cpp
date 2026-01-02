#include "Taml/Document.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <future>

namespace Taml
{
    Document::Document() {}

    Document::Document(const std::unordered_map<std::string, std::any>& data) : _data(data) {}

    std::any& Document::operator[](const std::string& key) {
        return _data[key];
    }

    const std::any& Document::operator[](const std::string& key) const {
        return _data.at(key);
    }

    bool Document::TryGetValue(const std::string& key, std::any& value) const {
        auto it = _data.find(key);
        if (it != _data.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    void Document::SetValue(const std::string& key, const std::any& value) {
        _data[key] = value;
    }

    std::vector<std::string> Document::GetKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : _data) {
            keys.push_back(pair.first);
        }
        return keys;
    }

    bool Document::ContainsKey(const std::string& key) const {
        return _data.count(key) > 0;
    }

    std::optional<Document> Document::GetSection(const std::string& key) const {
        std::any value;
        if (!TryGetValue(key, value)) return std::nullopt;

        if (auto* mapPtr = std::any_cast<std::unordered_map<std::string, std::any>>(&value)) {
            return Document(*mapPtr);
        }

        return std::nullopt;
    }

    Document Document::LoadFromFile(const std::string& path) {
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("TAML file not found: " + path);
        }

        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        return Parse(content);
    }

    std::future<Document> Document::LoadFromFileAsync(const std::string& path) {
        return std::async(std::launch::async, [path]() {
            return LoadFromFile(path);
        });
    }

    void Document::SaveToFile(const std::string& path) const {
        std::string tamlContent = Serialize(_data);
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + path);
        }
        file << tamlContent;
        file.close();
    }

    std::future<void> Document::SaveToFileAsync(const std::string& path) const {
        return std::async(std::launch::async, [this, path]() {
            SaveToFile(path);
        });
    }

    Document Document::Parse(const std::string& content) {
        // TODO: Implement TAML parser
        throw std::runtime_error("Parse not implemented yet");
    }

    std::string Document::Serialize(const std::unordered_map<std::string, std::any>& data) {
        // TODO: Implement TAML serializer
        throw std::runtime_error("Serialize not implemented yet");
    }
}
