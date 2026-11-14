#pragma once

#include <string>
#include <unordered_map>

namespace cz {

enum class Language {
    En = 0,   // English (Default)
    Es,       // Español
    It,       // Italiano
    De,       // Deutsch
    Ja,       // 日本語
    Zh,       // 简体中文
    PtBr,     // Português (Brasil)
    Fr        // Français
};

class CzI18n {
public:
    static CzI18n& instance();

    Language current_language() const { return lang_; }
    void set_language(Language lang);

    const char* tr(const char* key) const;
    static const char* language_name(Language lang);
    static const char* language_code(Language lang);

    void load_saved_language();
    void save_language() const;

private:
    CzI18n();
    void build_tables();
    std::string get_config_path() const;

    Language lang_ = Language::En;
    std::unordered_map<std::string, std::string> tables_[8];
};

inline const char* tr(const char* key) {
    return CzI18n::instance().tr(key);
}

} // namespace cz
