#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
using namespace std;
class OS
{
public:
    char IR[4], R[4], memory[100][4], buffer[40];
    int IC, SI, m;
    bool C;
    void INIT();
    void READ();
    void WRITE();
    void TERMINATE();
    void EXECUTEUSERPROGRAM();
    void MOS();
    void STARTEXECUTION();
    fstream f;
    fstream fout;
    fstream fend;
};
void OS::INIT()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = '\0';
        }
    }
    IR[4] = {'\0'};
    R[4] = {'\0'};
    C = false;
    m = 0;
}
void OS::READ()
{
    char c = IR[2];
    char d = IR[3];
    auto x = string(1, c) + d;
    int temp = stoi(x);
    f.getline(buffer, 41);
    int k = 0;
    for (int i = temp; i < (temp + 10); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = buffer[k];
            // cout << memory[i][j] << " ";
            k++;
        }
    }
    // f.getline(buffer, 41);
}
void OS::WRITE()
{
    fstream fout;
    fout.open("output.txt", ios::app);
    char c = IR[2];
    char d = IR[3];
    auto x = string(1, c) + d;
    int temp = stoi(x);
    char output[40] = {' '};
    char newline[] = "\n";
    int k = 0;
    for (int i = temp; i < (temp + 10); i++)
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
    cout << "output - " << output << endl;
    if (fout.is_open())
    {
        cout << endl
             << "Output file has been updated";
        fout << output;
        fout << newline;
        fout.close();
    }
}
void OS::TERMINATE()
{
    fstream fend;
    fend.open("output.txt", ios::app);
    char blank[] = "\n\n";
    if (fend.is_open())
    {
        fend << blank;
        fend.close();
    }
}
void OS::MOS()
{
    switch (SI)
    {
    case 1:
        READ();
        break;
    case 2:
        WRITE();
        break;
    case 3:
        TERMINATE();
        break;
    }
}
void OS::EXECUTEUSERPROGRAM()
{
    while (true)
    {
        for (int i = 0; i < 4; i++)
        {
            IR[i] = memory[IC][i];
        }
        cout << endl;
        cout << "IC - " << IC << endl;
        cout << "IR - " << IR << endl
             << endl;
        IC += 1;
        if (IR[0] == 'L' && IR[1] == 'R')
        {
            cout << "got LR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);
            for (int i = 0; i < 4; i++)
            {
                R[i] = memory[temp][i];
            }
            cout << "content in R - " << R << endl;
        }
        if (IR[0] == 'S' && IR[1] == 'R')
        {
            cout << "got SR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);
            for (int i = 0; i < 4; i++)
            {
                memory[temp][i] = R[i];
            }
        }
        if (IR[0] == 'C' && IR[1] == 'R')
        {
            C = false;
            cout << "got CR instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);
            int count = 0;
            for (int i = 0; i < 4; i++)
            {
                if (R[i] == memory[temp][i])
                {
                    count++;
                }
            }
            if (count == 4)
            {
                C = true;
            }
            cout << "After comparing R and memory location - " << C;
        }
        if (IR[0] == 'B' && IR[1] == 'T')
        {
            cout << "got BT instruction" << endl;
            char c = IR[2];
            char d = IR[3];
            auto x = string(1, c) + d;
            int temp = stoi(x);
            if (C == true)
            {
                IC = temp;
                // cout << "IC - " << IC;
            }
        }
        if (IR[0] == 'G' && IR[1] == 'D')
        {
            cout << "got GD instruction" << endl;
            SI = 1;
            MOS();
        }
        if (IR[0] == 'P' && IR[1] == 'D')
        {
            cout << "got PD instruction" << endl;
            SI = 2;
            MOS();
        }
        if (IR[0] == 'H')
        {
            cout << "got H instruction" << endl;
            SI = 3;
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
int main()
{
    OS os;
    os.f.open("input.txt", ios::in);
    if (os.f.is_open())
    {
        while (!(os.f.eof()))
        {
            for (int i = 0; i < 40; i++)
            {
                os.buffer[i] = ' ';
            }
            os.f.getline(os.buffer, 41);
            if (os.buffer[0] == '$' && os.buffer[1] == 'A' && os.buffer[2] == 'M' && os.buffer[3] == 'J')
            {
                cout << endl
                     << "Reading jobs from card reader:" << endl;
                os.INIT();
            }
            else if (os.buffer[0] == '$' && os.buffer[1] == 'D' &&
                     os.buffer[2] == 'T' && os.buffer[3] == 'A')
            {
                for (int i = 0; i < 40; i++)
                {
                    os.buffer[i] = ' ';
                }
                os.STARTEXECUTION();
            }
            else if (os.buffer[0] == '$' && os.buffer[1] == 'E' &&
                     os.buffer[2] == 'N' && os.buffer[3] == 'D')
            {
                cout << "Job is terminated" << endl;
            }
            else
            {
                int k = 0;
                cout << "Program Card encountered - " << endl;
                for (int i = os.m; i < os.m + 10; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (os.buffer[k] == 'H')
                        {
                            os.memory[i][j] = os.buffer[k];
                            k++;
                            break;
                        }
                        os.memory[i][j] = os.buffer[k];
                        k++;
                    }
                }
                os.m = os.m + 10;
            }
        }
        os.f.close();
    }
    cout << "Input File closed";
    return 0;
}
