#ifndef PROMPT_PARSER_H
#define PROMPT_PARSER_H

#include "EduSandbox.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

struct ParsedCommand {
    enum CommandType {
        CREATE_ENTITY,
        EXPLAIN,
        CLEAR,
        INVALID
    };
    
    CommandType type;
    EntityType entityType;
    Vector2 position;
    std::map<std::string, std::string> parameters;
    std::string explanationTopic;
    std::string errorMessage;
    
    ParsedCommand() : type(INVALID), entityType(EntityType::BALL), 
                      position(400, 300) {}
};

class PromptParser {
private:
    std::map<std::string, EntityType> entityKeywords;
    std::map<std::string, std::string> actionKeywords;
    std::map<std::string, std::string> physicsKeywords;
    
    std::vector<std::string> tokenize(const std::string& input);
    std::string toLowerCase(const std::string& str);
    bool containsKeyword(const std::vector<std::string>& tokens, const std::string& keyword);
    
    EntityType detectEntityType(const std::vector<std::string>& tokens);
    Vector2 detectPosition(const std::vector<std::string>& tokens, int screenW, int screenH);
    float extractNumber(const std::vector<std::string>& tokens, size_t startIdx, float defaultValue);
    Color detectColor(const std::vector<std::string>& tokens);
    
    void initializeKeywords();
    
public:
    PromptParser();
    
    ParsedCommand parse(const std::string& prompt, int screenW = 800, int screenH = 600);
    std::shared_ptr<Entity> createEntityFromCommand(const ParsedCommand& cmd);
    
    std::string getHelpText() const;
    std::vector<std::string> getExamples() const;
};

#endif // PROMPT_PARSER_H
