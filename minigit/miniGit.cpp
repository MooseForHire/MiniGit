#include <string>
#include <filesystem>
#include <fstream>
#include <filesystem>
#include "miniGit.hpp"
#include <iostream>

using namespace std;

miniGit::miniGit() {
    head = new doublyNode;
    head->commitNumber = 0;
    head->previous = nullptr;
    head->next = nullptr;
    head->head = nullptr;
}

miniGit::~miniGit() {

    if(head == nullptr)
        return;
    doublyNode* commit = head;
    while(commit != nullptr) {
        singlyNode* node = head->head;
        while(node != nullptr) {
            singlyNode* prev = node;
            node = node->next;
            delete prev;
        }
        doublyNode* prev = commit;
        commit = head->next;
        delete commit;
    }
}

int miniGit::getCommitNumber(){
    if(head == nullptr) 
        return 0;
        return head -> commitNumber;

}

doublyNode* miniGit::getTail() {
    doublyNode* commit = head;
    while(commit->next != nullptr)
        commit = head->next;
    return commit;
}

bool miniGit::isLatest() {
    return head==nullptr || head == getTail();
}

bool miniGit::fileStaged(string filename){
        
        if(head == nullptr)
            return false;
        singlyNode* node = head -> head;

        while(node != nullptr){
            if(node -> fileName == filename)
            return true;
            node = node -> next;
        }

        return false;
}



void miniGit::add(string filename) {

    singlyNode* node = nullptr;
    if(head->head == nullptr) {
        head->head = new singlyNode;
        node = head->head;
    }
    else {
        singlyNode* tail = head->head;
        while(tail != nullptr) {
            node = tail;
            tail = node->next;
        }
        tail = new singlyNode;
        node->next = tail;
        node = tail;
    }
    node->fileName = filename;
    node->fileVersion = "0";
    node->next = nullptr;
}

string miniGit::trimVersion(const string& filename) {
    string result(filename);

    //Erase starting from the last underscore to the end.
    result.erase(result.rfind("_"), result.size());  
    return result;
}

string miniGit::appendVersion(const string& filename, const string& version) {
    string result(filename);
    result += "_" + version;
    return result;
}

bool miniGit::remove(string filename) {
    if(head == nullptr)
        return false;
    singlyNode* node = head->head;
    singlyNode* prev = nullptr;
    while(node != nullptr) {
        if(trimVersion(node->fileName) == filename) {
            if(prev != nullptr) {
                if(node->next != nullptr)
                    prev->next = node->next;
                else { prev->next = nullptr; }
                delete node;
                return true;
            }
            else {
                if(node->next != nullptr)
                    head->head = node->next;
                delete node;
                return true;
            }
        }
    }
    return false;
}

bool miniGit::fileChanged(const string& filename1, const string& filename2) {
    //Compare last write time and file size to determine if changed
    return (!filesystem::exists(filename1) || 
    !filesystem::exists(filename2) || 
    filesystem::last_write_time(filename1) != 
    filesystem::last_write_time(filename2) || 
    filesystem::file_size(filename1) != 
    filesystem::file_size(filename2));
}

bool miniGit::readyForCommit(){
    return (head != nullptr && head -> head != nullptr);  
}

void miniGit::copyFile(const string& source, const string& dest) {
    ifstream in(source, ifstream::in|ifstream::binary);
    ofstream out(dest, ofstream::out|ofstream::binary);

    string line;
    while(getline(in, line)){
        out << line;
    }
    in.close();
    out.close();
}

void miniGit::commit() {
    if(!readyForCommit()) return;
    singlyNode* node = head->head;
    //For each node (file)...
    while(node != nullptr) {
        /*If it already exists, increment the version number and create a new copy
            only if this file has changed*/
        if(filesystem::exists(".minigit/" + node->fileName)) {
            if(fileChanged(node->fileName, ".minigit/" + appendVersion(node->fileName, node->fileVersion))) {
                node->fileVersion = to_string(stoi(node->fileVersion)+1);
                copyFile(node->fileName, ".minigit/" + appendVersion(node->fileName, node->fileVersion));
            }
        }
        //Otherwise, make the first copy
        else {
            copyFile(node->fileName, ".minigit/" + appendVersion(node->fileName, node->fileVersion));
        }
        node = node->next;
    }
    //Set the stage for the new commit, aka a new doublyNode
    head->next = new doublyNode;
    head->next->previous = head;
    head = head->next;
    head->next = nullptr;
    head->head = nullptr;
    head->commitNumber = head->previous->commitNumber+1;
    node = head->previous->head; //For tracking each node in old commit
    singlyNode* copyPrevious = nullptr; //For setting ->next later
    //Create deep copies of the entire singlyNode list
    while(node != nullptr) {
        singlyNode* copyNode = new singlyNode; //For copying each node to new commit
        if(head->head == nullptr)
            head->head = copyNode;
        copyNode->fileName = node->fileName;
        copyNode->fileVersion = node->fileVersion;
        if(copyPrevious != nullptr)
            copyPrevious->next = copyNode;
        copyPrevious = copyNode;
        node = node->next;
    }
}


bool miniGit::checkout(int commitNo) {
    doublyNode* commit = head;

    //Move to oldest commit first so we look in one direction
    while(commit->previous != nullptr)
        commit = commit->previous;
    while(commit != nullptr) {

        //If commit has been found, 
        if(commit->commitNumber == commitNo && commit != head) {
            singlyNode* node = commit->head;
            
            //For every file in .minigit, overwrite the current directory
            while(node != nullptr){
            copyFile(".minigit/" + appendVersion(node->fileName,
            node -> fileVersion), node -> fileName);
            }
            head = commit;
            return true;
        }
        commit = commit -> next;
    }
    return false;
    
}

void miniGit::print() {
    doublyNode* commit = head;
    if(commit == nullptr) {
        cout << "Nothing yet!" << endl;
        return;
    }
    cout << "Contents of .minigit:" << endl;
    for(auto& p : filesystem::directory_iterator(".minigit")) {
        cout << "\t" << p.path() << endl;
    }
    //Move to the very oldest commit first so we only have to move in one direction
    while(commit->previous != nullptr)
        commit = commit->previous;
    while(commit != nullptr) {
        cout << "Commit Number " << commit->commitNumber << ":" << endl;
        singlyNode* node = commit->head;
        while(node != nullptr) {
            cout << "\t";
            //If we're at the most recent commit, print a * before the filename if it has changed
            if(commit == getTail()) {
                if(fileChanged(node->fileName, ".minigit/" + appendVersion(node->fileName, node->fileVersion))) {
                    cout << "*";
                }
            }
            cout << node->fileName << " version " << node->fileVersion << endl;
            node = node->next;
        }
        commit = commit->next;
    }
}

