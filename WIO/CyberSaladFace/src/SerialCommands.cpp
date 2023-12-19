
#include "SerialCommands.h"

#define COMMANDQUEUE_SIZE 500

//Commands in the queue are \0 terminated so that \0 is the command separator
static char COMMAND_QUEUE[COMMANDQUEUE_SIZE] = {0};
static int nextCmd=0;
static int queueEnd=0;

command_spec_t NO_COMMAND = { CMD_NOTHING, NULL, NULL, NULL };

std::string Consume();
command_spec_t ParseNext(std::string );

SerialCommands::SerialCommands() {
}

void SerialCommands::AddString(std::string cmd) {    
}

command_spec_t SerialCommands::NextCommand() {
    if (nextCmd == queueEnd)
        return NO_COMMAND;
    else {
        std::string found = Consume();
        return ParseNext(found);
    }
        
}

void SerialCommands::Reset() {
    nextCmd = queueEnd;
}

std::string Consume() {
    return NULL;
}

command_spec_t ParseNext(std::string) {
    return NO_COMMAND;
}

