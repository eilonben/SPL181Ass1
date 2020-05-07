
#include <string>
#include <iostream>
#include "Commands.h"
#include "GlobalVariables.h"
using namespace std;



    BaseCommand::BaseCommand(string args):args(args){}
    string BaseCommand::getArgs(){
        return args;
    }
    void BaseCommand::verb(){
        if (verbose==2 || verbose==3)
            cout<< this->toString();
    }
    BaseCommand::~BaseCommand() {}

    Directory* BaseCommand::getLegalPath(Directory* wd, FileSystem &fs, string args) {
        if (args.length()==0)
            return wd;
        if (args.substr(0,2)=="..")
        {
            if (wd==&fs.getRootDirectory())
                return nullptr;
            if (args.length()>2)
                return getLegalPath(wd->getParent(), fs , args.substr(3));
            else
                return getLegalPath(wd->getParent(), fs , "");
        }
        size_t index = args.find("/");
        if (index!=string::npos) {
            string name = args.substr(0, index);
            vector<BaseFile *> v = wd->getChildren();
            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end(); ++it) {
                if (it.operator*()->getName() == name) {
                    if (it.operator*()->isFile())
                        return nullptr;
                    return getLegalPath((Directory *) it.operator*(), fs, args.substr(index + 1));
                }
            }
            return nullptr;
        }
        else {
            vector<BaseFile *> v = wd->getChildren();
            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end(); ++it) {
                if (it.operator*()->getName() == args) {
                    if (it.operator*()->isFile())
                        return nullptr;
                    return (Directory*)it.operator*();
                }
            }
            return nullptr;
        }


    }
    bool BaseCommand::isWdAncestor(FileSystem& fs, Directory* d) {
        bool found=false;
        for (BaseFile* c: d->getChildren()){
            if (found)
                return true;
            if (c==&fs.getWorkingDirectory())
                return true;
            else
                if (!c->isFile())
                    found = isWdAncestor(fs,(Directory*)c);

        }
        return false;
    }

    PwdCommand::PwdCommand(string args):BaseCommand(args){}
    void PwdCommand::execute(FileSystem & fs){
        if (&fs.getWorkingDirectory()==&fs.getRootDirectory()){
            cout <<"/";
            return;
        }
        cout<<fs.getWorkingDirectory().getAbsolutePath()+ "\n";

    } // Every derived class should implement this function according to the document (pdf)
    string PwdCommand::toString() {
        return "pwd\n";
    }
    PwdCommand::~PwdCommand() {}
    BaseCommand* PwdCommand::clone() {
        BaseCommand* b = new PwdCommand(*this);
        return b;
    }

    CdCommand::CdCommand(string args):BaseCommand(args){}
    void CdCommand::execute(FileSystem & fs) {
        Directory* tempWorkDirectory=&fs.getWorkingDirectory();

        if (getArgs().substr(0,1)=="/") {
            Directory *isLegal = getLegalPath(&fs.getRootDirectory(), fs, getArgs().substr(1));
            if (isLegal == nullptr) {
                cout << "The system cannot find the path specified\n";
            }
            else
            fs.setWorkingDirectory(isLegal);

        }else {
            Directory* isLegal = getLegalPath(tempWorkDirectory, fs, getArgs());
            if (isLegal == nullptr) {
                cout << "The system cannot find the path specified\n";
            }
            else
            fs.setWorkingDirectory(isLegal);
        }


    }
    string CdCommand::toString(){
        return "cd " + getArgs()+"\n";
    }
    CdCommand::~CdCommand() {}
    BaseCommand* CdCommand::clone() {
        BaseCommand* b= new CdCommand(*this);
        return b;
    }

    LsCommand::LsCommand(string args):BaseCommand(args){}
    void LsCommand::execute(FileSystem &fs) {
        if (getArgs().length()==0){
            vector<BaseFile *> v = fs.getWorkingDirectory().getChildren();
            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end(); ++it) {
                cout << it.operator*()->toString() << "\n";
            }
            return;
        }
        else
        if (getArgs()=="-s"){
            fs.getWorkingDirectory().sortBySize();
            vector<BaseFile *> v = fs.getWorkingDirectory().getChildren();
            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end(); ++it) {
                cout << it.operator*()->toString()+"\n";
            }
            fs.getWorkingDirectory().sortByName();
        }
        else {
            Directory* isLegal;
            if (getArgs().substr(0,3)=="-s ") {
                string path = getArgs().substr(3);
                if (path.substr(0,1)=="/") {
                     isLegal = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1));
                }
                else {
                     isLegal = getLegalPath(&fs.getWorkingDirectory(), fs, path);
                }
                if (isLegal == nullptr)
                    cout << "The system cannot find the path specified\n";
                else{
                    isLegal->sortBySize();
                    vector<BaseFile *> v = isLegal->getChildren();
                    for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end(); ++it) {
                        cout << it.operator*()->toString()+"\n";
                    }
                    isLegal->sortByName();
                }
            }
            else {
                if (getArgs().substr(0,1) =="/")
                    isLegal = getLegalPath(&fs.getRootDirectory(), fs, getArgs().substr(1));
                else
                    isLegal = getLegalPath(&fs.getWorkingDirectory(), fs, getArgs());
                if (isLegal == nullptr)
                    cout << "The system cannot find the path specified\n";
                else{
                    for (BaseFile* c: isLegal->getChildren()){
                        cout <<"" + c -> toString() + "\n";
                    }
                }
            }

        }

    }
    string LsCommand::toString() {
        if(getArgs()=="")
          return "ls\n";
        return "ls "+getArgs()+"\n";
    }
    LsCommand::~LsCommand() {}
    BaseCommand* LsCommand::clone() {
        BaseCommand* b= new LsCommand(*this);
        return b;
    }

    MkdirCommand::MkdirCommand(string args):BaseCommand(args){

    }
    void MkdirCommand::execute(FileSystem & fs) {
        if (getArgs() == "/") {
            cout << "The directory already exists\n";
            return;
        }
        size_t index = getArgs().find("/");
        if (index == string::npos) {
            for (BaseFile *c: fs.getWorkingDirectory().getChildren()) {
                if (c->getName() == getArgs()) {
                    cout << "The directory already exists\n";
                }
            }
            Directory *newdir = new Directory(getArgs(), &fs.getWorkingDirectory());
            fs.getWorkingDirectory().addFile(newdir);
            return;
        }
            Directory *curr;
            string path = getArgs();
            if (getArgs().substr(0, 1) == "/") {
                curr = &fs.getRootDirectory();
                path = path.substr(1);
            } else {
                curr = &fs.getWorkingDirectory();
            }

            while (path.length() != 0) {
                size_t index = path.find("/");
                if (index == string::npos) {
                    for (BaseFile *c: curr->getChildren()) {
                        if (c->getName() == path) {
                            cout << "The directory already exists\n";
                            return;
                        }
                    }
                    Directory *newdir = new Directory(path, curr);
                    curr->addFile(newdir);
                    path = "";
                } else {
                    string name = path.substr(0, index);
                    path = path.substr(index + 1);

                    bool found = false;
                    for (BaseFile* c: curr->getChildren()) {
                        if (c->getName() == name) {
                            if (c->isFile()) {
                                cout << "The directory already exists\n";
                                return;
                            } else {
                                curr = (Directory *) c;
                                found = true;
                            }

                        }
                    }
                        if (!found) {
                            Directory *newdir = new Directory(name, curr);
                            curr->addFile(newdir);
                            curr = newdir;
                        }


                }

            }

        }
    string MkdirCommand::toString(){
        return "mkdir "+getArgs()+"\n";
    }
    MkdirCommand::~MkdirCommand() {}
    BaseCommand* MkdirCommand::clone() {
        BaseCommand* b= new MkdirCommand(*this);
        return b;
    }

    MkfileCommand::MkfileCommand(string args):BaseCommand(args){}
    void MkfileCommand::execute(FileSystem &fs) {
        size_t index = getArgs().find_last_of("/");
        if (index == string::npos) {
            string name;
            size_t index2 = getArgs().find_first_of(" ");
            if (index2==string::npos)
                name = getArgs();
            else
                name = getArgs().substr(0,index2);
            for(BaseFile* c: fs.getWorkingDirectory().getChildren()) {
                if (c->getName() == name) {
                    cout << "File already exists\n";
                    return;
                }
            }
                string file = getArgs().substr((index + 1));

            size_t index1 = file.find(" ");
                string size="0";
                string fileName;
                if (index1 == string::npos) {
                    fileName = file;
                }
                else {
                    fileName = file.substr(0, file.find(" "));
                    size = file.substr(file.find(" ") + 1);
                }
                File* toAdd = new File(fileName,std::stoi(size));
                fs.getWorkingDirectory().addFile(toAdd);
                return;



        } else {
            string path = getArgs().substr(0, index);
            string file = getArgs().substr((index + 1));

            size_t index1 = file.find(" ");
            string size="0";
            string fileName;
            if (index1 == string::npos) {
                fileName = file;
            }
            else {
                fileName = file.substr(0, file.find(" "));
                size = file.substr(file.find(" ") + 1);
            }

            if (fileName == "/") {
                cout << "Cannot name file as the root directory\n";
            }
            Directory *dir;
            if (getArgs().substr(0, 1) == "/") {
                dir = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1));
            } else {
                dir = getLegalPath(&fs.getWorkingDirectory(), fs, path);
            }
            if (dir == nullptr){
                cout << "The system cannot find the path specified\n";
                return;
            }


            vector<BaseFile *> v = dir->getChildren();
            bool found = false;
            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end() && !found; ++it)
                if (it.operator*()->getName() == fileName) {
                    cout << "File already exists\n";
                    found = true;
                }
            if (!found) {
                File *newFile = new File(fileName, std::stoi(size));
                dir->addFile(newFile);
            }



            }

        }
    string MkfileCommand::toString() {
            return "mkfile " + getArgs() +"\n" ;
        }
    MkfileCommand::~MkfileCommand() {}
    BaseCommand* MkfileCommand ::clone() {
        BaseCommand* b = new MkfileCommand(*this);
        return b;
    }

    CpCommand ::CpCommand(string args): BaseCommand(args){}
    void CpCommand:: execute(FileSystem & fs) {
        size_t index = getArgs().find(" ");
        if (index == string::npos)
            cout << "No such file or directory\n";

        else {
            string src = getArgs().substr(0, index);
            string des = getArgs().substr((index + 1));
            size_t index1 = src.find_last_of("/");
            BaseFile* fileToCopy;
            if (index1 == string::npos) {

                vector<BaseFile *> v = fs.getWorkingDirectory().getChildren();
                bool found = false;
                for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end() && !found; ++it)
                    if (it.operator*()->getName() == src) {
                        found = true;
                        if (it.operator*()->isFile())
                            fileToCopy = (File*) it.operator*();

                        else {
                            fileToCopy = (Directory *) it.operator*();
                        }
                    }
                if (!found) {
                    cout << "No such file or directory\n";
                    return;
                }
            }
            if (src == "/")
                fileToCopy = &fs.getRootDirectory();
            else {
                string path = src.substr(0, index1);
                Directory *isPath;
                if (index1 == string::npos){
                    isPath = &fs.getWorkingDirectory();
                }
                else {
                    if (path.substr(0, 1) == "/")
                        isPath = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1, index1));
                    else
                        isPath = getLegalPath(&fs.getWorkingDirectory(), fs, path.substr(0, index1));
                    if (isPath == nullptr) {
                        cout << "No such file or directory\n";
                        return;
                    }
                }
                bool found = false;
                for (BaseFile *c: isPath->getChildren()) {
                    if (c->getName() == src.substr(index1 + 1)) {
                        found = true;
                        fileToCopy = c;
                    }
                }

                if (!found) {
                    cout << "No such file or directory\n";
                    return;
                }

            }


            Directory *destination;
            if (des.substr(0, 1) == "/")
                destination = getLegalPath(&fs.getRootDirectory(), fs, des.substr(1));

            else
                destination = getLegalPath(&fs.getWorkingDirectory(), fs, des);

            if (destination == nullptr) {
                cout << "No such file or directory\n";
                return;
            }
            for(BaseFile* c: destination->getChildren()){
                if( c->getName()== fileToCopy->getName()){
                    return;
                }
            }
            if (fileToCopy->isFile()) {
                File *f1 = new File(fileToCopy->getName(), fileToCopy->getSize());
                destination->addFile(f1);
            }
            else{
                Directory* d1 = (Directory*) fileToCopy;
                Directory* d2 = new Directory(*d1);
                destination->addFile(d2);
            }
        }

    }
    string CpCommand:: toString(){
        return "cp "+getArgs()+"\n";
    }
    CpCommand::~CpCommand() {}
    BaseCommand* CpCommand::clone() {
        BaseCommand* b= new CpCommand(*this);
        return b;
    }

    RenameCommand::RenameCommand(string args):BaseCommand(args){}
    void RenameCommand::execute(FileSystem & fs){

        size_t index = getArgs().find(" ");
        if(index ==string::npos) {
            cout << "No such file or directory\n";
            return;
        }
        string oldpath = getArgs().substr(0, index);
        string newname = getArgs().substr((index+1));
        Directory* path;
        if(oldpath== "/"){
            cout << "Can't rename directory\n";
            return;
        }
        size_t index1= oldpath.find_last_of("/");
        if (index1==string::npos){
            path = &fs.getWorkingDirectory();
            for (BaseFile* c:path->getChildren()){
                if (c->getName()==newname)
                    return;
                if (c->getName()==oldpath){
                    c->setName(newname);
                    path->sortByName();
                    return;
                }
            }
           cout<< "No such file or directory\n";
            return;
        }
        else{
            string thePath = oldpath.substr(0,index1);
            string oldname = oldpath.substr(index1+1);
            if (thePath.substr(0,1)=="/")
                path = getLegalPath(&fs.getRootDirectory(),fs,thePath.substr(1));
            else
                path = getLegalPath(&fs.getWorkingDirectory(),fs,thePath);

            if (path== nullptr){
                cout << "No such file or directory\n";
                return;
            }
            for (BaseFile* c:path->getChildren()) {
                if (c->getName()==newname)
                    return;
                if (c->getName() == oldname) {
                    if(c==&fs.getWorkingDirectory()){
                        cout << "Can't rename the working directory\n";
                        return;
                    }
                    c->setName(newname);
                    path->sortByName();
                    return;
                }
            }
        }
        cout<< "No such file or directory\n";

    }
    string RenameCommand::toString(){
        return "rename "+getArgs()+"\n";
    }
    RenameCommand::~RenameCommand() {}
    BaseCommand* RenameCommand::clone() {
        BaseCommand* b= new RenameCommand(*this);
        return b;
    }

    HistoryCommand::HistoryCommand(string args, const vector<BaseCommand *> & history):BaseCommand(args), history(history){}
    void HistoryCommand::execute(FileSystem & fs){
        size_t counter =0;
        for(BaseCommand* c: history){
            cout<< counter;
            cout<< "\t";
            cout<< c->toString();
            counter++;
        }
    }
    string HistoryCommand::toString(){
        return "history \n";
    }
    HistoryCommand::~HistoryCommand() {}
    BaseCommand* HistoryCommand::clone() {
        BaseCommand* b = new HistoryCommand(*this);
        return b;
    }

    ExecCommand::ExecCommand(string args, const vector<BaseCommand *> & history):BaseCommand(args) , history(history){}
    void ExecCommand::execute(FileSystem & fs){
        size_t cNum = std::stoi(getArgs());
        if (cNum<0 || cNum > history.size() || history.empty()){
            cout<< "Command not found\n";
            return;
        }
        history[cNum]->execute(fs);
    }
    string ExecCommand::toString(){
        return "exec "+getArgs()+"\n";
    }
    ExecCommand::~ExecCommand() {}
    BaseCommand* ExecCommand::clone() {
        BaseCommand* b = new ExecCommand(*this);
        return b;
    }

    ErrorCommand::ErrorCommand(string args):BaseCommand(args){}
    void ErrorCommand::execute(FileSystem & fs){
        string args = getArgs().substr(0,getArgs().find_first_of(" "));
        cout << args + ": Unknown command"+"\n";
    }
    string ErrorCommand::toString(){
        return getArgs()+"\n";
    }
    ErrorCommand::~ErrorCommand() {}
    BaseCommand* ErrorCommand::clone() {
        BaseCommand* b = new ErrorCommand(*this);
        return b;
    }


    MvCommand::MvCommand(string args):BaseCommand(args){}
    void MvCommand::execute(FileSystem & fs) {
        string src;
        bool found = false;
        Directory *destination;
        BaseFile *fileToCopy;
        size_t index = getArgs().find(" ");
        if (index == string::npos) {
            cout << "No such file or directory\n";
            return;
        }

        src = getArgs().substr(0, index);
        string des = getArgs().substr((index + 1));
        size_t index1 = src.find_last_of("/");
        if (index1 == string::npos) {
            if (src==".."){
                cout << "Can't move directory\n";
                return;
            }


            vector<BaseFile *> v = fs.getWorkingDirectory().getChildren();

            for (std::vector<BaseFile *>::iterator it = v.begin(); it != v.end() && !found; ++it) {
                if (it.operator*()->getName() == src) {
                    found = true;
                    if (it.operator*()->isFile())
                        fileToCopy = (File *) it.operator*();

                    else {
                        fileToCopy = (Directory *) it.operator*();
                    }
                }
            }
            if (!found) {
                cout << "No such file or directory\n";
                return;
            }
        }
        if (src == "/") {
            cout << "Can't move directory\n";
            return;
        }
        if (index1 == 0) {
            Directory *curr = &fs.getRootDirectory();
            for (BaseFile *c:curr->getChildren()) {
                if (c->getName() == src.substr(1)) {
                    if (!c->isFile())
                        if (c == &fs.getWorkingDirectory() || isWdAncestor(fs, (Directory*)c)) {
                            cout << "Can't move directory\n";
                            return;
                        }

                }
            }
        }
        if (index1 !=string::npos) {
            string path = src.substr(0, index1);
            Directory *isPath;

            if (path.substr(0, 1) == "/")
                isPath = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1, index1));
            else
                isPath = getLegalPath(&fs.getWorkingDirectory(), fs, path.substr(0, index1));
            if (isPath == nullptr) {
                cout << "No such file or directory\n";
                return;
            }
            bool found = false;
            for (BaseFile *c: isPath->getChildren()) {
                if (c->getName() == src.substr(index1 + 1)) {
                    found = true;
                    fileToCopy = c;
                }
            }

            if (!found) {
                cout << "No such file or directory\n";
                return;
            }

        }
        if (des.substr(0, 1) == "/")
            destination = getLegalPath(&fs.getRootDirectory(), fs, des.substr(1));

        else
            destination = getLegalPath(&fs.getWorkingDirectory(), fs, des);

        if (destination == nullptr) {
            cout << "No such file or directory\n";
            return;
        }
        for (BaseFile *c: destination->getChildren()) {
            if (c->getName() == fileToCopy->getName()) {
                return;
            }
        }


        //remove
        if ((src == "/") || (src == "")) {
            cout << "Can't remove directory\n";
            return;
        }

        Directory *curr;
        string name;
        index = src.find_last_of("/");
        if (index == string::npos) {
            curr = &fs.getWorkingDirectory();
            name = src;
        } else {
            if (index == 0) {
                curr = &fs.getRootDirectory();
                name = getArgs().substr(1);
            } else {
                string path = src.substr(0, index1);
                name = src.substr(index + 1);

                if (path.substr(0, 1) == "/")
                    curr = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1));
                else
                    curr = getLegalPath(&fs.getWorkingDirectory(), fs, path);

                if (curr == nullptr) {
                    cout << "No such file or directory\n";
                    return;
                }

            }
        }
        for (BaseFile *c:curr->getChildren()) {
            if (c->getName() == name) {
                if (c->isFile()) {
                    File *f1 = new File(fileToCopy->getName(), fileToCopy->getSize());
                    destination->addFile(f1);
                    curr->removeFile(name);

                    return;

                } else {
                    Directory *d1 = (Directory *) fileToCopy;

                    if (isWdAncestor(fs, d1) || fileToCopy == &fs.getWorkingDirectory()) {
                        cout << "Can't move directory \n";
                        return;
                    } else {
                        Directory *d2 = new Directory(*d1);
                        destination->addFile(d2);
                        curr->removeFile(name);

                    }

                }
            }
        }

    }
    string MvCommand::toString(){
        return "mv "+ getArgs()+ "\n";
    }
    MvCommand::~MvCommand() {}
    BaseCommand* MvCommand::clone() {
        BaseCommand* b = new MvCommand(*this);
        return b;
}

    RmCommand::RmCommand(string args):BaseCommand(args){}
    void RmCommand::execute(FileSystem & fs){
        if ((getArgs()=="/") || (getArgs()== "")){
            cout<< "Can't remove directory\n";
        }
        Directory* curr;
        string name;
        size_t index = getArgs().find_last_of("/");
        if (index==string::npos){
            name=getArgs();
            for (BaseFile* c: fs.getWorkingDirectory().getChildren()){
                if (c->getName()==name) {
                    fs.getWorkingDirectory().removeFile(name);
                    return;
                }
            }
                cout << "No such file or directory\n";
                return;


        }
        else {
            if (index == 0) {
                curr = &fs.getRootDirectory();
                name = getArgs().substr(1);
            } else {
                string path = getArgs().substr(0, index);
                name = getArgs().substr(index + 1);

                if (path.substr(0, 1) == "/")
                    curr = getLegalPath(&fs.getRootDirectory(), fs, path.substr(1));
                else
                    curr = getLegalPath(&fs.getWorkingDirectory(), fs, path);

                if (curr == nullptr) {
                    cout << "No such file or directory\n";
                    return;
                }

            }
        }


        for (BaseFile* c:curr->getChildren()){
            if (c->getName()==name) {
                if (c->isFile()) {
                    curr->removeFile(name);
                    return;
                } else if (isWdAncestor(fs, (Directory *) c) || c==&fs.getWorkingDirectory() ) {
                    cout << "Can't remove directory\n";
                    return;
                } else
                    curr->removeFile(name);


            }
        }


    }
    string RmCommand::toString(){
        return "rm "+ getArgs() +"\n";
    }
    RmCommand::~RmCommand() {}
    BaseCommand* RmCommand::clone() {
        BaseCommand* b = new RmCommand(*this);
        return b;
}

    VerboseCommand::VerboseCommand(string args):BaseCommand(args){}
    void VerboseCommand::execute(FileSystem & fs){
        if (getArgs()=="1")
            verbose=1;
        else
        if (getArgs()=="2")
            verbose=2;
        else
        if (getArgs()=="3")
            verbose=3;
        else
        if (getArgs()=="0")
            verbose=0;
        else
            cout<< "Wrong verbose input \n";
    }
    string VerboseCommand::toString() {
        string s = "verbose " + getArgs() + "\n";
        return s;
    }
    VerboseCommand::~VerboseCommand() {}
    BaseCommand* VerboseCommand::clone() {
        BaseCommand* b = new VerboseCommand(*this);
        return b;
}

//
// Created by eilonben@wincs.cs.bgu.ac.il on 11/14/17.
//

