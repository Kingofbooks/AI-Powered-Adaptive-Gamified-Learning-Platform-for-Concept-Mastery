#include "PromptParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

PromptParser::PromptParser() {
    initializeKeywords();
}

void PromptParser::initializeKeywords() {
    // Entities
    entityKeywords["ball"] = EntityType::BALL;
    entityKeywords["sphere"] = EntityType::BALL;
    entityKeywords["circle"] = EntityType::BALL;
    
    entityKeywords["block"] = EntityType::BLOCK;
    entityKeywords["box"] = EntityType::BLOCK;
    entityKeywords["cube"] = EntityType::BLOCK;
    entityKeywords["square"] = EntityType::BLOCK;
    
    entityKeywords["ground"] = EntityType::GROUND;
    entityKeywords["floor"] = EntityType::GROUND;
    entityKeywords["platform"] = EntityType::GROUND;
    
    entityKeywords["ramp"] = EntityType::RAMP;
    entityKeywords["slope"] = EntityType::RAMP;
    entityKeywords["incline"] = EntityType::RAMP;
    
    entityKeywords["arrow"] = EntityType::ARROW;
    entityKeywords["vector"] = EntityType::ARROW;
    entityKeywords["force"] = EntityType::ARROW;
    
    entityKeywords["text"] = EntityType::TEXT_LABEL;
    entityKeywords["label"] = EntityType::TEXT_LABEL;
    entityKeywords["note"] = EntityType::TEXT_LABEL;
    
    // Actions
    actionKeywords["create"] = "create";
    actionKeywords["add"] = "create";
    actionKeywords["make"] = "create";
    actionKeywords["spawn"] = "create";
    
    actionKeywords["explain"] = "explain";
    actionKeywords["what"] = "explain";
    actionKeywords["show"] = "explain";
    
    actionKeywords["clear"] = "clear";
    actionKeywords["reset"] = "clear";
    actionKeywords["remove"] = "clear";
    
    // Physics
    physicsKeywords["gravity"] = "gravity";
    physicsKeywords["fall"] = "gravity";
    
    physicsKeywords["velocity"] = "velocity";
    physicsKeywords["speed"] = "velocity";
    
    physicsKeywords["collision"] = "collision";
    physicsKeywords["bounce"] = "collision";
    physicsKeywords["hit"] = "collision";
    
    physicsKeywords["energy"] = "energy";
}

std::vector<std::string> PromptParser::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        token.erase(std::remove_if(token.begin(), token.end(), 
            [](char c) { return std::ispunct(c); }), token.end());
        
        if (!token.empty()) {
            tokens.push_back(toLowerCase(token));
        }
    }
    
    return tokens;
}

std::string PromptParser::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool PromptParser::containsKeyword(const std::vector<std::string>& tokens, 
                                   const std::string& keyword) {
    return std::find(tokens.begin(), tokens.end(), keyword) != tokens.end();
}

EntityType PromptParser::detectEntityType(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        auto it = entityKeywords.find(token);
        if (it != entityKeywords.end()) {
            return it->second;
        }
    }
    return EntityType::BALL;
}

Vector2 PromptParser::detectPosition(const std::vector<std::string>& tokens, 
                                      int screenW, int screenH) {
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "at" && i + 2 < tokens.size()) {
            try {
                float x = std::stof(tokens[i + 1]);
                float y = std::stof(tokens[i + 2]);
                return Vector2(x, y);
            } catch (...) {}
        }
        
        if (tokens[i] == "top") return Vector2(screenW / 2, 100);
        if (tokens[i] == "bottom") return Vector2(screenW / 2, screenH - 100);
        if (tokens[i] == "left") return Vector2(100, screenH / 2);
        if (tokens[i] == "right") return Vector2(screenW - 100, screenH / 2);
        if (tokens[i] == "center" || tokens[i] == "middle") 
            return Vector2(screenW / 2, screenH / 2);
    }
    
    return Vector2(screenW / 2, screenH / 3);
}

float PromptParser::extractNumber(const std::vector<std::string>& tokens, 
                                  size_t startIdx, float defaultValue) {
    for (size_t i = startIdx; i < tokens.size(); i++) {
        try {
            return std::stof(tokens[i]);
        } catch (...) {}
    }
    return defaultValue;
}

Color PromptParser::detectColor(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        if (token == "red") return Color::Red();
        if (token == "blue") return Color::Blue();
        if (token == "green") return Color::Green();
        if (token == "yellow") return Color::Yellow();
        if (token == "orange") return Color::Orange();
        if (token == "purple") return Color::Purple();
        if (token == "white") return Color::White();
        if (token == "black") return Color::Black();
        if (token == "gray" || token == "grey") return Color::Gray();
        if (token == "brown") return Color::Brown();
    }
    return Color::White();
}

ParsedCommand PromptParser::parse(const std::string& prompt, int screenW, int screenH) {
    ParsedCommand cmd;
    
    if (prompt.empty()) {
        cmd.errorMessage = "Empty command";
        return cmd;
    }
    
    std::vector<std::string> tokens = tokenize(prompt);
    if (tokens.empty()) {
        cmd.errorMessage = "No valid keywords";
        return cmd;
    }
    
    // Check for explain
    for (const auto& token : tokens) {
        if (actionKeywords[token] == "explain") {
            cmd.type = ParsedCommand::EXPLAIN;
            for (const auto& t : tokens) {
                auto it = physicsKeywords.find(t);
                if (it != physicsKeywords.end()) {
                    cmd.explanationTopic = it->second;
                    return cmd;
                }
            }
            cmd.explanationTopic = "help";
            return cmd;
        }
        
        if (actionKeywords[token] == "clear") {
            cmd.type = ParsedCommand::CLEAR;
            return cmd;
        }
    }
    
    // Entity creation
    cmd.type = ParsedCommand::CREATE_ENTITY;
    cmd.entityType = detectEntityType(tokens);
    cmd.position = detectPosition(tokens, screenW, screenH);
    
    Color color = detectColor(tokens);
    cmd.parameters["color_r"] = std::to_string((int)color.r);
    cmd.parameters["color_g"] = std::to_string((int)color.g);
    cmd.parameters["color_b"] = std::to_string((int)color.b);
    
    if (containsKeyword(tokens, "static") || containsKeyword(tokens, "fixed")) {
        cmd.parameters["static"] = "true";
    }
    
    return cmd;
}

std::shared_ptr<Entity> PromptParser::createEntityFromCommand(const ParsedCommand& cmd) {
    if (cmd.type != ParsedCommand::CREATE_ENTITY) return nullptr;
    
    std::shared_ptr<Entity> entity;
    
    switch (cmd.entityType) {
        case EntityType::BALL:
            entity = std::make_shared<Ball>(cmd.position);
            break;
        case EntityType::BLOCK:
            entity = std::make_shared<Block>(cmd.position);
            break;
        case EntityType::GROUND:
            entity = std::make_shared<Ground>(cmd.position);
            break;
        case EntityType::RAMP:
            entity = std::make_shared<Ramp>(cmd.position);
            break;
        case EntityType::ARROW:
            entity = std::make_shared<Arrow>(cmd.position, Vector2(1, 0));
            break;
        case EntityType::TEXT_LABEL:
            entity = std::make_shared<TextLabel>(cmd.position, "Label");
            break;
    }
    
    if (entity) {
        auto itR = cmd.parameters.find("color_r");
        auto itG = cmd.parameters.find("color_g");
        auto itB = cmd.parameters.find("color_b");
        
        if (itR != cmd.parameters.end() && itG != cmd.parameters.end() && 
            itB != cmd.parameters.end()) {
            Color color(std::stoi(itR->second), std::stoi(itG->second), 
                       std::stoi(itB->second));
            entity->setColor(color);
        }
        
        auto itStatic = cmd.parameters.find("static");
        if (itStatic != cmd.parameters.end()) {
            entity->setPhysicsType(PhysicsType::STATIC);
        }
    }
    
    return entity;
}

std::string PromptParser::getHelpText() const {
    return "COMMANDS:\n\n"
           "CREATE:\n"
           "  create ball/block/ground/ramp\n"
           "  add red ball at top\n"
           "  make blue block at center\n\n"
           "POSITIONS:\n"
           "  at top/bottom/left/right/center\n"
           "  at 300 200\n\n"
           "COLORS:\n"
           "  red, blue, green, yellow, orange,\n"
           "  purple, brown, gray\n\n"
           "EXPLAIN:\n"
           "  explain gravity/collision/velocity/energy\n\n"
           "CLEAR:\n"
           "  clear (removes all objects)";
}

std::vector<std::string> PromptParser::getExamples() const {
    return {
        "create ball at top",
        "add red ball",
        "make blue block at center",
        "create ground at bottom",
        "add ramp at left",
        "create yellow ball at 400 200",
        "explain gravity",
        "explain collision",
        "clear"
    };
}
