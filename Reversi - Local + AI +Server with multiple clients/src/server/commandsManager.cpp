#include <unistd.h>
#include "commandsManager.h"
#include <sstream>

CommandsManager *CommandsManager::instance_ = 0;
CommandsManager::CommandsManager(Lobby *lobby,
                                 HandleGame *handleGame,
                                 vector<pthread_t *> *threads) {
  threads_ = threads;
  commandsMap_["join"] = new CommandJoin(lobby, handleGame, threads);
  commandsMap_["list_games"] = new CommandPrint(lobby);
  commandsMap_["close"] = new CommandClose(lobby, threads);
  commandsMap_["start"] = new CommandStart(lobby);
}

CommandsManager::~CommandsManager() {
  map<string, Command *>::iterator it;
  for (it = commandsMap_.begin(); it != commandsMap_.end(); it++) {
    delete it->second;
  }
  delete instance_;
}

bool CommandsManager::executeCommand(string command, string arg, int client) {
  cout << "Received command: " << command << " from socket: " << client << endl;
  return commandsMap_[command]->execute(arg, client);
}

bool CommandsManager::isLegalCommand(string command, int client) {
  if (commandsMap_.find(command) == commandsMap_.end()) {
    cout << "Unrecognized command received: " << command << endl;
    char invalid[20] = "Invalid command!";
    ssize_t n = write(client, &invalid, sizeof(invalid));
    return false;
  }
  return true;
}

CommandsManager *CommandsManager::getInstance(Lobby *lobby,
                                              HandleGame *handleGame,
                                              vector<pthread_t *> *threads) {
  if (!instance_) {
    instance_ = new CommandsManager(lobby, handleGame, threads);
  }
  return instance_;
}
CommandsManager *CommandsManager::getInstance() {
  if (!instance_) {
    return NULL;
  }
  return instance_;
}

std::pair<string, string> CommandsManager::seperate(string input) {
  stringstream stream(input);
  string command;
  string arg;
  stream >> command;
  stream >> arg;
  return pair<string, string>(command, arg);
}
