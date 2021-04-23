#include <filesystem>
#include <iostream>
#include "miniGit.hpp"

using namespace std;


int main(){
   
    miniGit mg;
    cout << "Would you like to initialize the repository? (y/n): ";
    string initChoice;
    cin >> initChoice;
    if(initChoice == "y" || initChoice == "Y"){
        filesystem::remove_all(".minigit");
        filesystem::create_directory("minigit");

    }
    else{
        cout << "Without initializing the repository, program will not work" << endl;
        cout << "Exiting.";
        return 0;

    }
    bool running = true;
    while(running){

        cout << "1. Add a file (stage a file for commit)" << endl;
        cout << "2. Remove a file (destage a file)" << endl;
        cout << "3. Commit" << endl;
        cout << "4. Checkout to another commit" << endl;
        cout << "5. Print the entire minigit tree" << endl;


        int choice;
        cin >> choice;
        cin.ignore(1000, '\n');
        
        switch(choice){
            case 1: {
                if(!mg.isLatest()) {
                    cout << "Cannot add/remove/commit when not checked out to the most recent commit." << endl;
                    break;
                }
                //Adding a file to the staging area
                cout << "Filename:";
                string filename;
                cin >> filename;
                if(!filesystem::exists(filename)) {
                    //TODO: keep prompting user for a valid file or quit to menu
                }
                if(mg.fileStaged(filename)) {
                    cout << "Files cannot be added twice." << endl;
                    break;
                }
                mg.add(filename);
                break;
            }
            
            case 2:{

                if(!mg.isLatest()) {
                    cout << "Cannot add/remove/commit when not checked out to the most recent commit." << endl;
                    break;
                }

                //Removing a file from the staging area
                cout << "Filename: ";
                string filename;
                cin >> filename;

                if(mg.remove(filename)){
                    cout << "File has been removed. " << endl;

                }
                else{
                    cout << "File was not staged so it could not be removed. " << endl;
                }
                break;
            }
            case 3:{

                if(!mg.isLatest()) {
                    cout << "Cannot add/remove/commit when not checked out to the most recent commit." << endl;
                    break;
                }

                //Committing changes
                if(!mg.readyForCommit()){
                    cout << "There is nothing to commit. Please use \"add\" first." << endl;
                }
                mg.commit();
                cout << "Commited version: " << mg.getCommitNumber()-1 << endl;

                break;
            }
            case 4:{

                if(!mg.isLatest()) {
                    cout << "Cannot add/remove/commit when not checked out to the most recent commit." << endl;
                    break;
                }

                //Checkout
                cout << "Warning: you will lose all local changes that have not yet been committed." << endl;
                cout << "Proceed? (y/n):" << endl;
                cin >> initChoice;
                if(initChoice != "y" || initChoice != "Y") {
                    break;
                }

                cout << "Checkout to commit number: " << endl;
                int commitNo;
                cin >> commitNo;

                //Ignore endline character 
                cin.ignore(1000, '\n');

                if(mg.checkout(commitNo)) {
                    cout << "Checked out to commit " + mg.getCommitNumber() << endl;
                }
                else {
                    cout << "Not a valid commit number." << endl;
                }

                break;

            }
            case 5: {
                mg.print();
                break;
            }
            case 6: {
                running = false;
                break;

            }
        }
    }
    return 0;

}