/*
Sanjeev Sharma 
CSCI 340 - Section 01

Discription: A program that simulates some aspects of operating systems. There is no real system programming involved. 
The whole simulation is based on the text inputs that the program receives from user.

CPU Scheduing: 3 level Multilevel Feedback Queue. Top level is 1 time quantum round-robin queue. Middle level is 2 time 
quantum round-robin queue. Bottom level is a first come first served queue.

I/O Scheduling: Each drive has its own First come first served queue.

Memory: Demand paging..

*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
using namespace std;


class Process{
// Public Members
    public:
    // Constructer
    Process(int pid){_pid = pid; _Qcounter =0;}

    // Setters
    void setFile(const string &file){_file = file;}
    void setdriveNum(int x){_driveNumber = x;}
    int getQcounter(){return _Qcounter;}

    // Getters
    string getFile(){return _file;}
    int getPid(){return _pid;}

    // Function to drop process from one level to lower level
    void dropLevel(vector <Process> &lvlx, vector <Process> &lvly){
        lvly.push_back(lvlx[0]);
        lvlx.erase(lvlx.begin(), lvlx.begin()+1);
    }
    // function to update Qcounter
    void updateQcounter(){_Qcounter++;}


// Private Members
    private:
    int _pid;
    string _file;
    int _Qcounter;
    int _driveNumber;
};

class Frame{
// Public members
    public:
    // Setters
    void setPid(int x){_pid = x;}
    void setTime(int x){_time = x;}
    void setPage(int x){_page = x;}
        
    // Getters
    int getPid(){return _pid;}
    int getTime(){return _time;}
    int getPage(){return _page;}

// Private members
    private:
    int _time;
    int _pid;
    int _page;
};



// PRINT FUNCTIONS ---------------------------------------------------------------------------------------------------
void printMem(vector <Frame> x){
    for (int i = 0; i < x.size(); i++){
        cout << "Page: " << x[i].getPage() << "  pid: " << x[i].getPid() << "   Time: " << x[i].getTime() <<endl;
    }
}

void printProcess(vector <Process> x){
    if(x.empty()){
        cerr << "The level is empty \n";
    }
    for (int i = 0; i < x.size(); i++){
        cout << "PID: " << x[i].getPid() << endl;
    }
}

void printHDDQueue(vector < vector <Process> > x){
    cout << "size of hdd vector: " << x.size() << endl;
    for (int i = 0; i < x.size(); i++){
        cout << "\nDRIVE " << i << " QUEUE: \n" << "Currently using hard disk " << i << ": ";
        for (int j = 0; j < x[i].size(); j++){
            if(x[i][j].getPid() != 0){
                cout << "Pid: " <<x[i][j].getPid() <<"  FileName: " <<x[i][j].getFile()<<endl;
            }        
        }
    }
    cout << endl;
}

void printWaiting(vector <Process> x){
    if(x.empty()){
        cerr << "The level is empty \n";
    }
    for (int i = 1; i < x.size(); i++){
        cout << "PID: " << x[i].getPid() << endl;
    }
}
// END PRINT FUNCTIONS-----------------------------------------------------------------------------------------------------------------


// creates frame for a pid
void createFrame(int page, long numFrames, int Time, int pid, vector<Frame> &memory){
    for(int i = 0; i < memory.size(); i++){
        if(page == memory[i].getPage() && pid == memory[i].getPid()){
            memory[i].setTime(Time);
            return;
        }
    }

    for(int i = 0; i < memory.size(); i++){
        if(memory[i].getPid() == -1){
            memory[i].setPid(pid); memory[i].setPage(page); memory[i].setTime(Time);
            return;
        }
    }
    
    // for frame swapping when full
    if (memory.size() == numFrames){
        int currMax = 0;
        int MinIndex;
        for (int i = 0; i < memory.size(); i++){
            if (memory[i].getTime() > currMax){
                currMax = memory[i].getTime(); 
            }
        }
        int currMin = currMax;
        for (int i = 0; i < memory.size(); i++){
            if (memory[i].getTime() < currMin){
                currMin = memory[i].getTime();
                MinIndex = i;
            }
        }
        memory[MinIndex].setPid(pid); memory[MinIndex].setTime(Time); memory[MinIndex].setPage(page);
    }
    else {
        Frame frame;
        frame.setPage(page); frame.setPid(pid); frame.setTime(Time);
        memory.push_back (frame);
      }
    }

    // returns number of pages for a given pid in memory
    int numPages(vector <Frame> x, int &pid){
        int num = 0;
        for(int i = 0; i < x.size(); i++){
            if(x[i].getPid() == pid){
                num++;
            }
        }
        cout << num << endl;
        return num;
    }
    /* used in commands to figure out what is in the CPU. Simply put, we check all levels from top to bottom to see if they are not empty.
       The highest level index 0 is whats running. 
    */
    int whatsRunning(const vector <Process> &lvl0, const vector <Process> &lvl1, const vector <Process> &lvl2){
        if (!lvl0.empty()){
            return 0;
        }
        else if (!lvl1.empty()){
            return 1;
        }
        else if (!lvl2.empty()){
            return 2;
        }
        else return -1;
    }
    // remove all pages of a given pid 
    void terminateMem(vector <Frame> &x, int pid){
        for (int i = 0; i < x.size(); i++){
            if (x[i].getPid() == pid){
                x[i].setPid(-1);
            }
        }
    }


    // used for checking for proper digit input     
    bool IntCheck(string holder, long &output){
        int count = 0;
        if(holder.empty()){return false;}
        for(int i = 0; i < holder.length(); i++){
            if (isdigit(holder[i])){
                count++;
            }
        }
        if(count == holder.length()) {
            sscanf(holder.c_str(), "%ld", &output);
            return true;
        }
        else if (count != holder.length()){return false;}
    }


int main(){
    int pid = 1; int page; 
 
    cout << "How much RAM memory is there on the simulated computer?\n";
    string memHolder= "";
    long mem = 0;
    bool doneMem = false;
    while (!doneMem){
        getline(cin,memHolder);
        doneMem = IntCheck(memHolder, mem);
        if(doneMem){
            break;
        }
        else if (!doneMem){
            cerr << "Incorrect Input\n";
        }
    }

    
    cout << "What is the size of page or frame?\n";
    string frmHolder = "";
    long frm = 0;
    bool doneFrm = false; 
    while (!doneFrm){
        getline(cin,frmHolder);
        bool check = IntCheck(frmHolder, frm);
        if(check){
            if(frm <= mem){
                break;
                doneFrm = true;
            }
            else if (frm > mem){
                cerr << "Frame Size > Memory Size Error. Enter correct Frame Size: \n";
            }
        }
        else if(!check){
            cerr << "Incorrect Input\n";
        }
    }

    int Time = 0;
    long numFrames = mem/frm;

    vector<Frame> memory;


    cout << "How many hard disks does the simulate d computer have?\n";
    string drvHolder= "";
    long numDrvs = 0;
    bool doneDrvs = false;
    while (!doneDrvs){
        getline(cin,drvHolder);
        doneDrvs = IntCheck(drvHolder, numDrvs);
        if(doneDrvs){
            break;
        }
        else if (!doneDrvs){
            cerr << "Incorrect Input\n";
        }
    }



    vector <vector <Process> > files;
    for (int i = 0; i < numDrvs; i++){
        vector <Process> temp;
        Process process = Process(0);
        process.setdriveNum(i); process.setFile("Empty");
        temp.push_back(process);
        files.push_back(temp);
    }



    vector <Process> level0;
    vector <Process> level1;
    vector <Process> level2; 

    //cin.get();


    cout << "Please enter input: \n";
    string input;
    while (true){
        getline (cin, input);
        // A
        if (input == "A"){
            createFrame(0, numFrames, Time, pid, memory);
            Process process(pid); 
            level0.push_back(process);
            Time++;
            pid++;
        }


        // Q
        else if (input == "Q"){
            int running = whatsRunning(level0, level1, level2);
            if (running == -1){
                cerr << "CPU queue is Empty \n";
            } 
            else if(running == 0){
                level0[0].updateQcounter();
                level0[0].dropLevel(level0, level1);
            }
            else if(running == 1){
                if(level1[0].getQcounter() == 1){
                    level1[0].updateQcounter();
                }
                else{
                    level1[0].updateQcounter();
                    level1[0].dropLevel(level1, level2);
                }
            }

        }

        // t
        else if (input == "t"){
            int running = whatsRunning(level0, level1, level2);
            if (running == -1){
                cerr << "CPU queue is Empty \n";
            }
            else if(running == 0){
                int currPid = level0[0].getPid();
                terminateMem(memory, currPid);
                level0.erase(level0.begin(),level0.begin()+1);
            }
            else if(running == 1){
                int currPid = level1[0].getPid();
                terminateMem(memory, currPid);
                level1.erase(level1.begin(),level1.begin()+1);
            }
            else if(running == 2){
                int currPid = level2[0].getPid();
                terminateMem(memory, currPid);
                level2.erase(level2.begin(),level2.begin()+1);
            }

            
        }

        // d number file_name   check for number > then number of drives
        else if (input[0] == 'd' && input[1] == ' ' && input.length() > 2){
            int count = 0;

            for (int i = 2; input[i] != ' '; i++){
                if (isdigit(input[i])) {
                    count++;
                }
            }
            
            if(count == 0 || input[count+2] != ' ' || !isdigit(input[2])){
                cerr << "Incorrect Input \n";
                
                continue;
            }
            else{
                string x = input.substr(2,count);
                int driveNumber = atoi(x.c_str());
                /*if(driveNumber > numDrvs){
                    cerr << "Drive does not exist\n";
                    continue;
                }*/
                string filename = input.substr(count+3, input.length());
                int running = whatsRunning(level0, level1, level2);
                if(running == -1 || driveNumber > numDrvs){
                    if(driveNumber > numDrvs){
                        cerr << "Drive does not exist\n";
                    }
                    cerr << "There is no process in CPU\n";
                    continue;
                }
                else if(running == 0){
                    level0[0].setFile(filename); level0[0].setdriveNum(driveNumber);
                    files[driveNumber].push_back(level0[0]);
                    level0.erase(level0.begin(), level0.begin()+1);
                }
                else if(running == 1){
                    level1[0].setFile(filename); level1[0].setdriveNum(driveNumber);
                    files[driveNumber].push_back(level1[0]);
                    level1.erase(level1.begin(), level1.begin()+1);
                }
                else if(running == 2){
                    level2[0].setFile(filename); level2[0].setdriveNum(driveNumber);
                    files[driveNumber].push_back(level2[0]);
                    level2.erase(level2.begin(), level2.begin()+1);
                } 
                
            }            
        }


        // D number
        else if (input[0] == 'D' && input[1] == ' ' && input.length() > 2){
            int count = 0;
            for(int i = 2; i < input.length(); i++){
                if (!isdigit(input[i])) {
                    //cerr << "Incorrect Input \n";
                    count++;
                } 
            }
            if (count!=0){cout << "Incorrect Input\n"; continue;}
            string x = input.substr(2);
            int driveNumber = atoi(x.c_str());
            if(driveNumber > numDrvs || files[driveNumber].size() == 1){
                if(driveNumber > numDrvs){
                    cerr << "Drive does not exist\n" << endl;
                    continue;
                }
                else{
                    cerr << "Drive Queue " << driveNumber << " is empty\n";
                    continue;
                }
            }
            int Qcounter = files[driveNumber][1].getQcounter();
            if (Qcounter == 0){
                level0.push_back(files[driveNumber][1]);
            }
            else if(Qcounter == 1 || Qcounter == 2){
                level1.push_back(files[driveNumber][1]);
            }
            else if(Qcounter > 2){
                level2.push_back(files[driveNumber][1]);
            }
            files[driveNumber].erase(files[driveNumber].begin()+1, files[driveNumber].begin()+2);
        }

        // m address
        else if (input[0] == 'm' && input[1] == ' ' && input.length() > 2){
            int count = 0;
            for(int i = 2; i < input.length(); i++){
                if (!isdigit(input[i])) {
                    count++;
                } 
            }
            if(count != 0){cout << "Incorrect Input\n"; continue;}
            string x = input.substr(2);
            int address = atoi(x.c_str());
            page = address/ frm;
            //cout << "Page: " << page << endl;
            int y = whatsRunning(level0, level1, level2);
            if (y == -1){
                cerr << "There is no process in CPU\n";
            }
            else if (y == 0){
                createFrame(page, numFrames, Time, level0[0].getPid(), memory);  
            }
            else if (y == 1){
                createFrame(page, numFrames, Time, level1[0].getPid(), memory);
            }
            else if (y == 2){
                createFrame(page, numFrames, Time, level2[0].getPid(), memory);
            }  
            Time++;
        }

        // S r
        else if (input == "S r"){
            int x = whatsRunning(level0, level1, level2);
            if(x == -1){
                cerr<< "Queue is empty & nothing is running!\n";
            }
            
            else if(x == 0){
                cout << "\nCurrently using CPU: ";
                cout << level0[0].getPid() << /*"\t Qcount: " << level0[0].getQcounter() <<*/ "     from level: 0" <<"\n\n";
                cout << "Waiting in Level O: \n";
                printWaiting(level0);
                cout << endl;
                cout << "Waiting in Level 1: \n";
                printProcess(level1);
                cout << endl;
                cout << "Waiting in Level 2: \n";
                printProcess(level2);
            }
            else if(x == 1){
                cout << "\nCurrently using CPU: ";
                cout << level1[0].getPid() << /*"\t Qcount: " << level1[0].getQcounter()  <<*/"     from level: 1" <<"\n\n";
                cout << "Waiting in Level O: \n";
                printProcess(level0);
                cout << endl;
                cout << "Waiting in Level 1: \n";
                printWaiting(level1);
                cout << endl;
                cout << "Waiting in Level 2: \n";
                printProcess(level2);
            }
            else if(x == 2){
                cout << "\nCurrently using CPU: ";
                cout << level2[0].getPid() <<"     from level: 2" <<"\n\n";
                cout << "Waiting in Level O: \n";
                printProcess(level0);
                cout << endl;
                cout << "Waiting in Level 1: \n";
                printProcess(level1);
                cout << endl;
                cout << "Waiting in Level 2: \n";
                printWaiting(level2);
            }
        }
        
        // S i
        else if (input == "S i"){
        /*  Shows what processes are currently using the hard disks and what processes are waiting to use them. 
            For each busy hard disk show the process that uses it and show its I/O-queue. Make sure to display the filenames 
            (from the d command) for each process. The enumeration of hard disks starts from 0. */
        printHDDQueue(files);
            

        // S m
        }
        else if (input == "S m"){
            int count = 0;
            for (int i=0; i < memory.size(); i++){
                if(memory[i].getPid() != -1){
                    cout<< "Frame: " << i << "  PID: " << memory[i].getPid() << "   Page: " << memory[i].getPage() << "     Time: " << memory[i].getTime() <<endl;
                    count++; 
                }  
            } 
            if (count == 0){
                cerr << "Nothing in memory!\n";
            }       
            /*else{
                cerr << "incorrect input \n";
            }*/
        }

        else{
            cerr << "Incorrect Input\n";
        }
    }
    return 0;
}
