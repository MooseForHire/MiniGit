#ifndef H_MINIGIT       //compiler instruction
#define H_MINIGIT       


#include <string>

using namespace std;

struct singlyNode {
    
    string fileName;
    string fileVersion;
    singlyNode* next;

};

struct doublyNode{

    int commitNumber;
    doublyNode* previous;
    doublyNode* next;
    singlyNode* head;

};

class miniGit {
    private:
        doublyNode* head;

        string trimVersion(const string&);
        string appendVersion(const std::string&, const std::string&);
        void copyFile(const string&, const string&);
        doublyNode* getTail();

    public:
        miniGit();
        ~miniGit();
        bool isLatest();
        int getCommitNumber();
        bool readyForCommit(); //Have any files actually been staged?
        bool fileStaged(std::string);
        bool fileChanged(const std::string&, const std::string&);
        void add(std::string); //Stage a file
        bool remove(std::string); //Unstage a file
        void commit();
        bool checkout(int);
        void print();
};

#endif