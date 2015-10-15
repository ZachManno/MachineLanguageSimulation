//
//
//  Created by Zach Manno on 1/21/15.
//  This program simulates a basic version of machine language
//

#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

void dumpPrint(bool,int,int,int,int,int,int,int[]); //dump print function definitition

int main()
{
    string fileName; //for file input option, user entered file
    int s = 0; //for reading in from file
    int x = 0; //holding variable for reading in values to check if between -9999 and 9999
    int entrySelection=0; //prompting user for keying in words, or reading them in from a file
    
    //opcodes
    const int read = 10;
    const int write  = 11;
    const int load = 20;
    const int store  = 21;
    const int add = 30;
    const int subtract  = 31;
    const int divide = 32;
    const int multiply  = 33;
    const int branch = 40;
    const int branchneg  = 41;
    const int branchzero = 42;
    const int halt  = 43;
    
    int dataWord = 0; //word entered by user
    int memory[100]; //array to store words
    int instructionCounter = 0; //instruction counter
    int instructionRegister = 0; //instruction register
    int accumulator = 0; //accumulator
    int operationCode = 0; //opcode
    int operand = 0; //operand
    
    bool switchControl = true; //to determine when to leave switch
    bool abnormal = false; //to determine to print "exeuction terminated" or "execution abnormally terminated"
    int errorCode = 0; //to determine which error message to print
    
    for(int i = 0; i < 100; i++) memory[i] = 0; //array initialization
    
    //simpletron main screen
    cout << "*** Welcome to Simpletron! ***\n"
    "*** Please enter your program one instruction ***\n"
    "*** (or data word) at a time. I will type the ***\n"
    "*** location number and a question mark (?).  ***\n"
    "*** You then type the word for that location. ***\n"
    "*** Type the sentinel -99999 to stop entering ***\n"
    "*** your program. ***\n";
    
    cout << "\nIf you would like to bypass this, press 1 to read in a series of words from a file, 0 to continue\n";
    cin >> entrySelection; //if 0, read in each data word from keyboard, if 1, read in from file
    
    //word entry
    if (entrySelection == 0)
    {
        while(dataWord != -99999)
        {
            cout << setfill('0') << setw(2) << instructionCounter << " ? ";
            cin >> dataWord;
            while((dataWord < -9999 || dataWord > 9999) && dataWord != -99999)
            {
                cout << "*** Value not in memory range (-9999) -> (9999) ***\n"
                "*** Enter a different value ***\n";
                //instructionCounter--;
                cout << setfill('0') << setw(2) << instructionCounter << " ? ";
                cin >> dataWord;
            }
            if(dataWord != -99999)
            {
                memory[instructionCounter] = dataWord;
                instructionCounter++;
            }
        }
    }
    else //read in from file option
    {
        cout << "Enter the name of the file " << endl;
        cin >> fileName;
        ifstream myfile (fileName); //reading in from user-selected file
        while (! myfile.eof() )  //while the end of file is not reached
        {
            for(int i = 0; i < 100; i++)
            {
                if(myfile >> s) //if a value is able to be read in, read it into the array
                {
                    memory[i] = s;
                    instructionCounter++; //increment instruction counter after each word is read in
                }
            }
        }
        myfile.close();
    }
    
    cout << "\n*** Program loading completed ***\n"
    "*** Program execution begins  ***\n";
    
    instructionCounter = 0; //initialize instruction counter back to 0, to step through the program
    
    while(switchControl) //if switch control is false, either there was an error, or the halt opcode was reached, exiting this loop
    {
        instructionRegister = memory[instructionCounter];
        operationCode = instructionRegister / 100; //first 2 digits of word
        operand = instructionRegister % 100; //last 2 digits
        
        switch(operationCode)
        {
            case read :
                cout << "? ";
                cin >> x;
                while(x < -9999 || x > 9999) //the memory cant hold a 5 digit number, must re-prompt user
                {
                    cout << "*** Value not in memory range (-9999) -> (9999) ***\n"
                    "*** Enter a different value ***\n? ";
                    cin >> x;
                }
                memory[operand] = x; //store the int entered into the location of the operand in memory
                instructionCounter++; //instruction counter is usually incremented at the end of each opcode, unless there is a branch
                break;
            case write  :
                cout << memory[operand] << endl;
                instructionCounter++;
                break;
            case load :
                accumulator = memory[operand]; //putting the int from memory into the accumulator
                instructionCounter++;
                break;
            case store  :
                memory[operand] = accumulator; //putting the int from the accumulator into a location in memory
                instructionCounter++;
                break;
            case add :
                accumulator += memory[operand];
                instructionCounter++;
                break;
            case subtract  :
                accumulator -= memory[operand];
                instructionCounter++;
                break;
            case divide :
                if (memory[operand] == 0) //divide by 0 error
                {
                    errorCode = 1; //this prints out "*** Attempt to divide by zero ***" in dumpprint function
                    abnormal = true; //dump print goes through abnormal printing precedure
                    switchControl = false; //exits the switch
                    dumpPrint(abnormal,errorCode,accumulator,instructionCounter,instructionRegister,operationCode,operand,memory); //prints info
                    break;
                }
                else //if you are not dividing by 0
                {
                    accumulator /= memory[operand];
                    instructionCounter++;
                    break;
                }
            case multiply  :
                accumulator *= memory[operand];
                instructionCounter++;
                break;
            case branch : //uncoditional branch, set instruction counter to the operand
                instructionCounter = operand;
                break;
            case branchneg  :
                if(accumulator < 0) instructionCounter = operand; //if neg, branch to operand
                else instructionCounter++;
                break;
            case branchzero :
                if(accumulator == 0) instructionCounter = operand; //if 0, branch to operand
                else instructionCounter++;
                break;
            case halt  :
                //instructionCounter++;
                switchControl = false;
                dumpPrint(abnormal,errorCode,accumulator,instructionCounter,instructionRegister,operationCode,operand,memory); //print registers at end of program
                break;
            default : //this occurs when the opcode is invalid
                errorCode = 2; //entered into dump print function, to print out "*** Invalid operation code ***"
                abnormal = true; //it is an abnormal procedure, must exit and print register
                switchControl = false;
                dumpPrint(abnormal,errorCode,accumulator,instructionCounter,instructionRegister,operationCode,operand,memory);
                break;
        }
        
        if(accumulator < -9999 || accumulator > 9999) //if at any point, if the accumulator overflows, procedure is abnormal, must exit and print registers
        {
            errorCode = 3;
            abnormal = true;
            switchControl = false;
            dumpPrint(abnormal,errorCode,accumulator,instructionCounter,instructionRegister,operationCode,operand,memory);
        }
    }
    
    return 0;
}


void dumpPrint(bool abnormal,int errorCode,int accumulator,int instructionCounter,int instructionRegister,int operationCode,int operand,int memory[]) //dumpprint prototype
{
    int printCounter = 0; //for printing out memory array table
    //const char zeroo = ' ';
    string space = "   ";
    
    if(abnormal) //if the program ends abnormally, print error message according to error code
    {
        switch(errorCode)
        {
            case 1 : //divide by 0
                cout << "*** Attempt to divide by zero ***\n";
                break;
            case 2 : //invalid operation code
                cout << "*** Invalid operation code ***\n";
                break;
            case 3 : //accumulator overflow
                cout << "*** Accumulator overflow ***\n";
                break;
            default :
                break;
        }
        cout << "*** Simpletron execution abnormally terminated ***\n";
    }
    else cout << "*** Simpletron execution terminated ***\n" << endl;
    
    
    
    //printing out registers
    cout << "REGISTERS:\n";
    cout << left << setfill(' ') << setw(25) << "accumulator" << right << showpos << setfill('0') << internal << setw(5) << accumulator << endl;
    cout << left << setfill(' ') << setw(25) << "instructionCounter" << setiosflags(ios::right) << space << noshowpos << setfill('0') << setw(2) << instructionCounter << endl;
    cout << left << setfill(' ') << setw(25) << "instructionRegister" << setiosflags(ios::right)  << showpos << setfill('0') <<internal << setw(5) << instructionRegister << endl;
    cout << left << setfill(' ') << setw(25) << "operationCode" << setiosflags(ios::right)<< space << noshowpos << setfill('0') << setw(2) << operationCode << endl;
    cout << left << setfill(' ') << setw(25) << "operand" << setiosflags(ios::right) << space << noshowpos << setfill('0') << setw(2) << operand << endl;
    
    //memory table
    cout << "MEMORY:\n";
    cout << "      ";
    for(int k = 0; k < 10; k++) cout << setfill(' ') <<setw(8) << k;
    cout << "\n   -----------------------------------------------------------------------------------";
    cout << endl;
    for(int l = 0; l < 100; l++)
    {
        if((printCounter % 10 == 9)) //puts an endline after every tenth value
        {
            cout << setw(8)  << memory[l] << endl;
        }
        else if((printCounter % 10 == 0)) //prints out every value ending with 0
        {
            cout << setw(2) << l << "|" << "   " << setw(8)  << memory[l];
        }
        else
        {
            cout << setw(8) << memory[l];
        }
        printCounter++;
        
    }
    
}
