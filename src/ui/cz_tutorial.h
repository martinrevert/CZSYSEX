#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "imgui.h"

namespace cz {

struct TutorialTopic {
    std::string title;
    std::string subtitle;
    std::string summary;
    std::string sonic_effect;
    std::string pd_explanation;
};

class CzTutorial {
public:
    static CzTutorial& instance();

    bool is_enabled() const { return enabled_; }
    void set_enabled(bool enabled) { enabled_ = enabled; }
    void toggle() { enabled_ = !enabled_; }

    // Renders hover tooltip if Tutorial Mode is active
    void tooltip(const char* topic_id);

    // Renders the full interactive educational side panel / window
    void render_guide_window(bool* p_open);

private:
    CzTutorial();
    void init_topics();
    const std::unordered_map<std::string, TutorialTopic>& current_topics() const;

    bool enabled_ = true; // Enabled by default so users immediately discover it!
    std::string active_guide_topic_ = "intro";
    std::vector<std::string> topic_order_;
    std::unordered_map<std::string, TutorialTopic> topics_[8];
};

} // namespace cz

