
#include "Environment.h"
#include <iostream>
#include <string>
#include <vector>
#include "GlobalVariables.h"

using namespace std;


    Environment::Environment():commandsHistory(), fs(){}
    Environment::~Environment(){
        if (verbose==1 || verbose==3)
            std::cout <<"Environment::~Environment()";
        for (BaseCommand* c: commandsHistory){
            delete c;
            c= nullptr;
        }
        commandsHistory.clear();

    }
    Environment::Environment(const Environment &other){
        if (verbose==1 || verbose==3)
            std::cout <<"Environment::Environment(const Environment &other)";
        for (BaseCommand* c: other.commandsHistory) {
            commandsHistory.push_back(c->clone());
        }
        fs= other.fs;
    }
    Environment::Environment(Environment &&other): commandsHistory(std::move(other.commandsHistory)), fs(other.fs) {
        if (verbose==1 || verbose==3)
            std::cout <<"Environment::Environment(Environment &&other)";
    }
    Environment& Environment::operator=(const Environment &other) {
        if (verbose==1 || verbose==3)
            std::cout <<"Environment& Environment::operator=(const Environment &other)";
        for (BaseCommand* c: commandsHistory){
            delete c;
            c= nullptr;
        }
        commandsHistory.clear();
        fs= other.fs;
        for (BaseCommand* c: other.commandsHistory) {
            commandsHistory.push_back(c->clone());
        }
        return *this;
    }
    Environment& Environment::operator=(Environment &&other){
        if (verbose == 1 || verbose == 3)
            std::cout << "Environment& Environment::operator=(Environment &&other)";
        if (this!=&other) {
            for (BaseCommand *c: commandsHistory) {
                delete c;
                c = nullptr;
            }
            commandsHistory.clear();
            commandsHistory = std::move(other.commandsHistory);
            fs=other.fs;

        }
        return *this;

    }
    void Environment::start() {

        string input="";
        while(input!="exit"){
            if(&fs.getWorkingDirectory()==&fs.getRootDirectory()){
                cout << "/>";
            }
            else {
                cout << fs.getWorkingDirectory().getAbsolutePath() + ">";
            }
            std::getline (std::cin,input);
            string cmd;
            string arg;
            if(input=="exit")
                break;
             size_t index = input.find(" ");
            if(index==string::npos){
                cmd=input;
                if( cmd == "pwd"){
                    BaseCommand* pwd = new PwdCommand("");
                    pwd->verb();
                    pwd->execute(fs);
                    commandsHistory.push_back(pwd);

                }
                else if( cmd =="ls")
                {
                   BaseCommand* ls = new LsCommand("");
                    ls->verb();
                    ls->execute(fs);
                    commandsHistory.push_back(ls);
                }
                else if(cmd=="history"){
                    BaseCommand* his =  new HistoryCommand("", commandsHistory);
                    his->verb();
                    his->execute(fs);
                    commandsHistory.push_back(his);
                }
               else {
                    BaseCommand *err = new ErrorCommand(input);
                    err->verb();
                    err->execute(fs);
                    commandsHistory.push_back(err);
                }

            }
            else{
                cmd = input.substr(0,index);
                arg = input.substr(index+1);
                if( cmd== "cd" ){
                    BaseCommand* cd = new CdCommand(arg);
                    cd->verb();
                    cd->execute(fs);
                    commandsHistory.push_back(cd);

                }
               else if( cmd== "ls" ){
                    BaseCommand* ls = new LsCommand(arg);
                    ls->verb();
                    ls->execute(fs);
                    commandsHistory.push_back(ls);

                }
                else if( cmd== "mkdir" ){
                    BaseCommand* mkdir = new MkdirCommand(arg);
                    mkdir->verb();
                    mkdir->execute(fs);
                    commandsHistory.push_back(mkdir);
                }
                else if( cmd== "mkfile" ){
                    BaseCommand* mkfile= new MkfileCommand(arg);
                    mkfile->verb();
                    mkfile->execute(fs);
                    commandsHistory.push_back(mkfile);

                }
                else if( cmd== "cp" ){
                    BaseCommand* cp = new CpCommand(arg);
                    cp->verb();
                    cp->execute(fs);
                    commandsHistory.push_back(cp);

                }
               else if( cmd== "mv" ){
                    BaseCommand* mv = new MvCommand(arg);
                    mv->verb();
                    mv->execute(fs);
                    commandsHistory.push_back(mv);

                }
              else  if( cmd== "rename" ){
                    BaseCommand* rename = new RenameCommand(arg);
                    rename->verb();
                    rename->execute(fs);
                    commandsHistory.push_back(rename);

                }
               else if( cmd== "rm" ){
                    BaseCommand* rm = new RmCommand(arg);
                    rm->verb();
                    rm->execute(fs);
                    commandsHistory.push_back(rm);

                }
                else if( cmd== "verbose" ){
                    BaseCommand* verbose = new VerboseCommand(arg);
                    verbose->verb();
                    verbose->execute(fs);
                    commandsHistory.push_back(verbose);

                }
                else if( cmd== "exec" ){
                    BaseCommand* exec = new ExecCommand(arg,getHistory());
                    exec->verb();
                    exec->execute(fs);
                    commandsHistory.push_back(exec);

                }

               else {
                    BaseCommand *err = new ErrorCommand(input);
                    err->verb();
                    err->execute(fs);
                    commandsHistory.push_back(err);
                }

            }
        }
    }
    FileSystem& Environment::getFileSystem() {
        return fs ;
    } // Get a reference to the file system
    void Environment::addToHistory(BaseCommand *command){
        commandsHistory.push_back(command);
    } // Add a new command to the history
    const vector<BaseCommand*>& Environment::getHistory() const{
        return commandsHistory;
    } // Return a reference to the history of commands





// Created by eilonben@wincs.cs.bgu.ac.il on 11/14/17.
//

