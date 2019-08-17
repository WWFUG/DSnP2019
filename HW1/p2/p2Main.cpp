#include <iostream>
#include <iomanip>
#include "p2Json.h"
using namespace std;


int main(){
    Json json;
    string fname;
    string command;
    cout << "Please enter the file name: ";
    cin >> fname;
    if(json.read(fname)){
        cout << "File \"" << fname << "\" " << "was read in successfully" << endl;
    }
    else{
        return 0;
    }
    do{
        cout << "Enter command: ";
        cin >> command;
        if (command=="PRINT") json.print();
        else if(command=="SUM"){
            if(!json.isEmpty()){
                cout << "The summation of the values is: " << json.sum() << "." << endl;
            }
        }
        else if(command=="AVE"){
            if(!json.isEmpty()){
                cout << "The average of the values is: " << fixed << setprecision(1) << json.ave() << "." << endl;
            }
        }
        else if(command=="MAX"){
            if(!json.isEmpty()){
                cout << "The maximum element is: { " << json.max() << " }." << endl;
            }
        }
        else if(command=="MIN"){
            if(!json.isEmpty()){
                cout << "The minimum element is: { " << json.min() << " }." << endl;
            }
        }
        else if(command=="ADD"){
            string key,value;
            cin >> key >> value;
            json.add(key, stoi(value));
        }
        else if(command=="EXIT") break;
        else cout << "Not a command. Please try again." << endl;
    }while(true); 
}
