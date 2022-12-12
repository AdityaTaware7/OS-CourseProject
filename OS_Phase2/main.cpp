#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

class OS
{

public:
    char IR[4], R[4], memory[300][4], buffer[40], PTR[4];
    int IC, SI, EM, PI, TI, ptr, ptr_counter, stop;
    bool C;
    map<int, int> randomList;

    // process control block
    struct PCB
    {
        int job_id;
        int TTL;
        int TLL;
        int TTC;
        int TLC;
    } PCB;

    void INIT();
    void READ();
    void WRITE();
    void TERMINATE(int EM);
    void EXECUTEUSERPROGRAM();
    void MOS();
    void STARTEXECUTION();
    void LOAD();
    void ALLOCATE();
    int ADDRESSMAP(int VA);

    fstream f;
    fstream fout;
    fstream fend;
};

void OS::INIT()
{
    // initializing memory to null
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = '\0';
        }
    }

    // setting interrupts
    PI = 0;
    TI = 0;
    SI = 0;
    stop = 0;

    // setting instruction register, general register, and toggle register
    IR[4] = {'\0'};
    R[4] = {'\0'};
    C = false;

    // initializong the process block------------------
    PCB.TTC = 0;
    PCB.TLC = 0;

    string id = "";
    id.push_back(buffer[4]);
    id.push_back(buffer[5]);
    id.push_back(buffer[6]);
    id.push_back(buffer[7]);
    cout << "JOB-ID -> " << id << endl;

    PCB.job_id = stoi(id);

    string ttl = "";
    ttl.push_back(buffer[8]);
    ttl.push_back(buffer[9]);
    ttl.push_back(buffer[10]);
    ttl.push_back(buffer[11]);
    cout << "TTL (Total Time Limit) -> " << ttl << endl;

    PCB.TTL = stoi(ttl);

    string tll = "";
    tll.push_back(buffer[12]);
    tll.push_back(buffer[13]);
    tll.push_back(buffer[14]);
    tll.push_back(buffer[15]);
    cout << "TLL (Total Line Limit) -> " << tll << endl;

    PCB.TLL = stoi(tll);

    PTR[0] = '0';
    PTR[1] = '0';
    PTR[2] = '0';
    PTR[3] = '0';

    ALLOCATE();
}

void OS::ALLOCATE()
{
    ptr = rand() % 29; // allocating the page table register/block between 0-29
    cout << "ptr - " << ptr << endl;
    string s = to_string(ptr);

    for (int i = 0; i < s.length(); i++)
    {
        PTR[i] = s[i];
    }

    // initialising the page table
    for (int i = ptr * 10; i < ((ptr * 10) + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = '\0';
        }
    }

    // counter for page table
    ptr_counter = ptr * 10;
}

void OS::READ()
{
    char op_1 = IR[2];
    char op_2 = IR[3];
    string x = string(1, op_1) + op_2;
    int operand = stoi(x);

    int real_operand = ADDRESSMAP(operand);

    for (int i = 0; i < 40; i++)
    {
        buffer[i] = ' ';
    }

    f.getline(buffer, 41);

    // checking for out of data error
    if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
    {
        TERMINATE(1);
    }

    int k = 0;

    // storing data card
    for (int i = real_operand; i < (real_operand + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = buffer[k];
            cout << memory[i][j];
            k++;
        }
    }
    cout << endl;
}

void OS::WRITE()
{

    fstream fout;
    fout.open("output.txt", ios::app);

    char c = IR[2];
    char d = IR[3];
    string x = string(1, c) + d;
    int operand = stoi(x);

    int real_operand = ADDRESSMAP(operand);

    char output[40] = {' '};
    char newline[] = "\n";
    int k = 0;

    for (int i = real_operand; i < (real_operand + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (memory[i][j] != '\0')
            {
                output[k] = memory[i][j];
                k++;
            }
        }
    }

    cout << "Output file written with - " << output << endl;

    if (fout.is_open())
    {
        fout << output;
        fout << newline;

        fout.close();
    }
}

void OS::TERMINATE(int EM)
{
    string error_msg;

    switch (EM)
    {
    case 0:
        error_msg = "Program terminated without any errors\n";
        break;

    case 1:
        error_msg = "Out of data error\n";
        stop = 1;
        break;

    case 2:
        error_msg = "Error - Line Limit Exceeded\n";
        stop = 1;
        break;

    case 3:
        error_msg = "Error - Time Limit Exceeded\n";
        stop = 1;
        break;

    case 4:
        error_msg = "Opcode Error has occured\nJob is terminated abnormally";
        stop = 1;
        break;

    case 5:
        error_msg = "Operand is invalid\n";
        stop = 1;
        break;

    case 6:
        error_msg = "Invalid Page Fault\n";
        stop = 1;
        break;

    case 34:
        error_msg = "Time Limit Exceeded and Opcode error\n";
        stop = 1;
        break;

    case 35:
        error_msg = "Time Limit Exceeded and Operand error\n";
        stop = 1;
        break;

    default:
        break;
    }

    fstream fend;
    fend.open("output2.txt", ios::app);

    char blank[] = "\n";

    if (fend.is_open())
    {
        fend << error_msg;
        fend << blank;
        fend.close();
    }
}

void OS::MOS()
{

    // cases of SI and TI
    if (SI == 1 && TI == 0)
    {
        READ();
    }

    if (SI == 2 && TI == 0)
    {
        WRITE();
    }

    if (SI == 3 && TI == 0)
    {
        TERMINATE(0);
    }

    if (SI == 1 && TI == 2)
    {
        TERMINATE(3);
    }

    if (SI == 2 && TI == 2)
    {
        WRITE();
        TERMINATE(3);
    }

    if (SI == 3 && TI == 2)
    {
        TERMINATE(0);
    }

    // cases of PI and TI
    if (PI == 1 && TI == 0)
    {
        TERMINATE(4);
        PI = 0;
    }

    if (PI == 2 && TI == 0)
    {
        TERMINATE(5);
    }

    if (PI == 3 && TI == 0)
    {
        if (IR[0] == 'G')
        {

            cout << "Valid Page Fault Error (GD case)" << endl;
            cout << "Allocating new block for new data card" << endl;

            int random_generator = rand() % 29;

            if (random_generator == ptr)
            {
                random_generator = rand() % 29;
            }

            while (true)
            {
                if (randomList[random_generator] == 1)
                {
                    random_generator = rand() % 29;
                }
                else
                {
                    break;
                }
            }

            randomList[random_generator]++;

            string ptr_entry = to_string(random_generator);

            memory[ptr_counter][0] = '0';
            memory[ptr_counter][1] = '0';

            if (ptr_entry.length() == 1)
            {
                memory[ptr_counter][2] = '0';
                memory[ptr_counter][3] = ptr_entry[0];
            }
            else
            {
                memory[ptr_counter][2] = ptr_entry[0];
                memory[ptr_counter][3] = ptr_entry[1];
            }
            ptr_counter++;

            cout << "page table" << endl;
            for (int i = ptr * 10; i < (ptr * 10 + 10); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    cout << memory[i][j];
                }
                cout << endl;
            }

            PCB.TTC++;
            PI = 0;
        }
        else if (IR[0] == 'S')
        {

            cout << "Valid Page Fault Error (SR case)" << endl;
            cout << "Allocating new block for new data card" << endl;

            int random_generator = rand() % 29;

            if (random_generator == ptr)
            {
                random_generator = rand() % 29;
            }

            while (true)
            {
                if (randomList[random_generator] == 1)
                {
                    random_generator = rand() % 29;
                }
                else
                {
                    break;
                }
            }

            randomList[random_generator]++;

            string ptr_entry = to_string(random_generator);

            memory[ptr_counter][0] = '0';
            memory[ptr_counter][1] = '0';

            if (ptr_entry.length() == 1)
            {
                memory[ptr_counter][2] = '0';
                memory[ptr_counter][3] = ptr_entry[0];
            }
            else
            {
                memory[ptr_counter][2] = ptr_entry[0];
                memory[ptr_counter][3] = ptr_entry[1];
            }
            ptr_counter++;

            cout << "page table" << endl;
            for (int i = ptr * 10; i < (ptr * 10 + 10); i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    cout << memory[i][j];
                }
                cout << endl;
            }

            PCB.TTC++;
            PI = 0;
        }
        else
        {
            cout << "Invalid Page Fault Error" << endl;
            TERMINATE(6);
            PI = 0;
        }
    }

    if (PI == 1 && TI == 2)
    {
        TERMINATE(34);
    }

    if (PI == 2 && TI == 2)
    {
        TERMINATE(35);
    }

    if (PI == 3 && TI == 2)
    {
        TERMINATE(3);
    }

    if (TI == 2)
    {
        TERMINATE(3);
    }
}

int OS::ADDRESSMAP(int VA)
{

    int RA;
    int PTE = ptr * 10 + VA / 10;

    // checking for page fault
    if (memory[PTE][0] == '\0')
    {
        PI = 3;
        MOS();
    }

    if (memory[PTE][0] != '\0')
    {
        string str = "";
        str.push_back(memory[PTE][2]);
        str.push_back(memory[PTE][3]);
        int real_position = stoi(str);
        RA = (real_position * 10) + (VA % 10);
    }

    return RA;
}

void OS::EXECUTEUSERPROGRAM()
{
    while (true)
    {

        cout << endl;
        cout << "SI - " << SI << " "
             << "PI - " << PI << " "
             << "TI - " << TI << endl;

        // checking for time limit
        if (PCB.TTC == PCB.TTL)
        {
            TI = 2;
            MOS();
            break;
        }

        int real_address = ADDRESSMAP(IC);
        if (stop == 1)
        {
            break;
        }
        cout << "Real Address -> " << real_address << endl;

        for (int i = 0; i < 4; i++)
        {
            IR[i] = memory[real_address][i];
        }

        cout << endl;
        cout << "IC - " << IC << endl;
        cout << "IR - " << IR << endl
             << endl;

        IC += 1;

        if (memory[real_address][0] == 'H')
        {
            cout << "";
        }
        else
        {
            char op1 = IR[2];
            char op2 = IR[3];

            // operand error
            if (op1 < 48 || op1 > 57 || op2 < 48 || op2 > 57)
            {
                PI = 2;
                cout << "OPERAND ERROR HAS OCCURED";
                MOS();
                break;
            }

            string x = string(1, op1) + op2;
            int operand = stoi(x);
            cout << "operand - " << operand << endl;

            int real_operand = ADDRESSMAP(operand);

            if (stop == 1)
            {
                break;
            }

            cout << "Real Operand - " << real_operand << endl;
        }

        if (IR[0] == 'L' && IR[1] == 'R')
        {
            cout << "got LR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);

            int real_operand = ADDRESSMAP(temp);

            for (int i = 0; i < 4; i++)
            {
                R[i] = memory[real_operand][i];
            }

            // cout << "content in R - " << R << endl;
            cout << "LR executed";
            PCB.TTC++;
        }

        else if (IR[0] == 'S' && IR[1] == 'R')
        {
            cout << "got SR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);

            int real_operand = ADDRESSMAP(temp);

            for (int i = 0; i < 4; i++)
            {
                memory[real_operand][i] = R[i];
            }

            cout << endl;

            for (int i = 0; i < 4; i++)
            {
                cout << memory[real_operand][i];
            }
            cout << endl;

            cout << "SR executed";
            PCB.TTC++;
        }

        else if (IR[0] == 'C' && IR[1] == 'R')
        {
            C = false;
            cout << "got CR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);

            int real_operand = ADDRESSMAP(temp);

            int count = 0;
            for (int i = 0; i < 4; i++)
            {

                if (R[i] == memory[real_operand][i])
                {
                    count++;
                }
            }

            if (count == 4)
            {
                C = true;
            }
            cout << "After comparing R and memory location - " << C << endl;

            cout << "CR executed";
            PCB.TTC++;
        }

        else if (IR[0] == 'B' && IR[1] == 'T')
        {
            cout << "got BT instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);

            int real_operand = ADDRESSMAP(temp);

            if (C == true)
            {
                IC = temp;
                cout << "IC after BT instruction - " << IC << endl;
            }

            cout << "BT executed";
            PCB.TTC++;
        }

        else if (IR[0] == 'G' && IR[1] == 'D')
        {
            cout << "got GD instruction" << endl;
            SI = 1;
            MOS();

            cout << "GD executed";
            PCB.TTC++;
            SI = 0;
        }

        else if (IR[0] == 'P' && IR[1] == 'D')
        {
            PCB.TLC++;

            if (PCB.TLC > PCB.TLL)
            {
                TERMINATE(2);
                break;
            }

            cout << "got PD instruction" << endl;
            SI = 2;
            MOS();

            cout << "PD executed";
            PCB.TTC++;
            SI = 0;
        }

        else if (IR[0] == 'H' && IR[1] == '\0')
        {
            cout << "got H instruction" << endl;
            SI = 3;
            MOS();

            cout << "H executed";
            PCB.TTC++;

            break;
        }

        else
        {
            PI = 1;
            MOS();
            break;
        }
    }
}

void OS::STARTEXECUTION()
{
    IC = 00;
    EXECUTEUSERPROGRAM();
}

void OS::LOAD()
{
    int random_generator;

    f.open("input.txt", ios::in);
    if (f.is_open())
    {
        while (!(f.eof()))
        {

            // if (stop == 1)
            // {
            //     break;
            // }

            for (int i = 0; i < 40; i++)
            {
                buffer[i] = ' ';
            }

            f.getline(buffer, 41);

            cout << "buffer - " << buffer << endl;

            if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
            {
                cout << endl
                     << "Reading next job from card reader:" << endl;
                INIT();
                cout << "Initialization Done" << endl;
            }

            else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
            {
                cout << "Job Execution Started -> " << endl;
                STARTEXECUTION();
            }

            else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
            {
                cout << "Job Terminated" << endl;
            }

            else
            {
                int k = 0;
                cout << "Program Card Encountered -" << endl;

                random_generator = rand() % 29;

                if (random_generator == ptr)
                {
                    random_generator = rand() % 29;
                }

                // allocating block other than previous program cards
                while (true)
                {
                    if (randomList[random_generator] == 1)
                    {
                        random_generator = rand() % 29;
                    }
                    else
                    {
                        break;
                    }
                }

                randomList[random_generator]++;

                // storing the program card
                for (int i = random_generator * 10; i < (random_generator * 10 + 10); i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (buffer[k] == 'H')
                        {
                            memory[i][j] = buffer[k];
                            k++;
                            break;
                        }

                        memory[i][j] = buffer[k];
                        k++;
                    }
                }

                for (int i = random_generator * 10; i < (random_generator * 10 + 10); i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        cout << memory[i][j];
                    }
                    cout << endl;
                }

                string ptr_entry = to_string(random_generator);

                memory[ptr_counter][0] = '0';
                memory[ptr_counter][1] = '0';

                if (ptr_entry.length() == 1)
                {
                    memory[ptr_counter][2] = '0';
                    memory[ptr_counter][3] = ptr_entry[0];
                }
                else
                {
                    memory[ptr_counter][2] = ptr_entry[0];
                    memory[ptr_counter][3] = ptr_entry[1];
                }

                ptr_counter++;

                cout << "Page Table -> " << endl;
                for (int i = ptr * 10; i < (ptr * 10 + 10); i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        cout << memory[i][j];
                    }
                    cout << endl;
                }
            }
        }
    }
    f.close();

    cout << "Input File Closed";
}

int main()
{
    OS os;

    os.LOAD();

    return 0;
}
