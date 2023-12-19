/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * The serial commands object implements a command buffer between the master and the WIO
 * 
 */

#ifndef SERIALCOMMANDS_H
#define SERIALCOMMANDS_H

#include <Arduino.h>
#include <string>

enum CommandType
{
	CMD_MSG,
	CMD_FACE,
	CMD_GRAPH,
    CMD_GRAPHDATA,
    CMD_NOTHING,
};

typedef struct CommandSpec {
	enum CommandType command;
	std::string arg1;
    std::string arg2;
    std::string arg3;
} command_spec_t;

extern command_spec_t NO_COMMAND;

class SerialCommands {
public:
	SerialCommands();
	void AddString(std::string);
	command_spec_t NextCommand();
	void Reset();

	//virtual ~SerialCommands();
    
};

#endif