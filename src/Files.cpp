

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Files.h"
#include "GlobalVariables.h"

using namespace std;
    //BaseFile Class

    BaseFile::BaseFile(string name):name(name){}
    string BaseFile::getName() const{
        return name;
    }
    void BaseFile::setName(string newName){
        name = newName;
    }

    BaseFile::~BaseFile() {}

    //File Class



    File::File(string name, int size):BaseFile(name){
        this->size=size;
    }
    int  File::getSize() {
        return size;
    }
    bool File::isFile()  {
        return true;
    }

    string File::toString(){
        string s = "FILE";
        s+="\t";
        s+=getName();
        s+="\t";
        s+=to_string(getSize());
        return s;
    }

    File::~File()=default;


    //Directory Class
    Directory::Directory(string name, Directory *parent) :BaseFile(name), children(), parent(parent){}// Constructor
    Directory::Directory(const Directory& otherDir ) :BaseFile(""){
        if (verbose==1 || verbose==3)
            std::cout <<"Directory::Directory(Directory& otherDir) :BaseFile(otherDir.getName()";
        this->setParent(otherDir.getParent());
        this->setName(otherDir.getName());
        for (BaseFile* c:otherDir.children) {
            if(c->isFile()){
                File* f= new File(c->getName(),c->getSize());
                this->addFile(f);
            }
            else{
                Directory* d1 = (Directory*) c;
                Directory* d2= new Directory(*d1);
                d2->setParent(this);
                this->addFile(d2);

            }
        }
    }// Copy Constructor
    Directory::Directory(Directory&& otherDir):BaseFile(otherDir.getName()), children(std::move(otherDir.children)),parent(otherDir.parent)
        {
        if (verbose==1 || verbose==3)
            std::cout <<"Directory::Directory(Directory&& otherDir)";
        otherDir.setName("");
        otherDir.parent= nullptr;
    }// Move Constructor
    Directory& Directory::operator=(const Directory& other){
        if (verbose==1 || verbose==3)
            std::cout <<"Directory& Directory::operator=(const Directory& other)";
        if (this!=&other) {
            for (BaseFile *c:children) {
                delete c;
                c = nullptr;
            }
            this->children.clear();

            for (BaseFile *c: other.children) {
                if (c->isFile()){
                    File* f= new File(c->getName(),c->getSize());
                    this->addFile(f);
                }else{
                    Directory* d = (Directory*) c;
                    this->addFile(d);
                }
            }
            setParent(other.parent);
            return *this;
        }
        return *this;
    } // Copy Assignment Operator
    Directory& Directory::operator=(Directory &&other) {
        if (verbose==1 || verbose==3)
            std::cout <<"Directory& Directory::operator=(Directory &&other)";
        if (this != &other) {
            for (BaseFile *c:children) {
                delete c;
                c = nullptr;
            }
            this->children.clear();

            for (BaseFile *c: other.children) {
                this->addFile(c);
            }
            setParent(other.parent);
            other.parent= nullptr;
            return *this;
        }
        return *this;
    }
    Directory::~Directory() {
        if (verbose==1 || verbose==3)
            std::cout <<"Directory::~Directory()";
        for(BaseFile* c: getChildren()){
            delete c;
            c= nullptr;
        }
        this->children.clear();

    } // Destructor
    bool Directory::compName(BaseFile* f1, BaseFile* f2) {
            return f1->getName() < f2->getName();
        }


    bool Directory::compSize(BaseFile* f1, BaseFile* f2) {
            return f1->getSize() < f2->getSize();
        }

    bool Directory::isFile()  {
        return false;
    }

    Directory* Directory::getParent() const{
        return parent;
    } // Return a pointer to the parent of this directory
    void Directory::setParent(Directory *newParent){
        this->parent=newParent;
    } // Change the parent of this directory
    void Directory::addFile(BaseFile* file){
        if (!(file->isFile()))
            ((Directory*)file)->setParent(this);

        children.push_back(file);
        sortByName();
    } // Add the file to children
    void Directory::removeFile(string name){
        for (BaseFile* c:children){
            if (c->getName()==name){
                removeFile(c);
            }
        }
    }// Remove the file with the specified name from children
    void Directory::removeFile(BaseFile* file){
        delete file;
        children.erase(std::remove(children.begin(),children.end(),file),children.end());

    }// Remove the file from children
    void Directory::sortByName(){
        std:: sort(children.begin(), children.end(), Directory::compName);
    } // Sort children by name alphabetically (not recursively)
    void Directory::sortBySize(){
        sort(children.begin(), children.end(), Directory::compSize);
    } // Sort children by size (not recursively)
    vector<BaseFile*> Directory::getChildren()
    {
        return children;
    }// Return children
    int Directory::getSize()  {
        int size=0;
        for(BaseFile* c: getChildren()){
            size=size + c->getSize();
        }
        return size;
    }


    string Directory::getAbsolutePath(){
        if (parent!=nullptr)
            return parent->getAbsolutePath() + "/" +getName();
        return "";
    }
    string Directory::toString(){
        string s = "DIR";
        s+="\t";
        s+=getName();
        s+="\t";
        s+=to_string(getSize());
    return s;
}

//
// Created by eilonben@wincs.cs.bgu.ac.il on 11/14/17
//

