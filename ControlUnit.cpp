// Verilator generated module include.
#include "VControlUnit.h"

// C++ libraries.
#include "stdlib.h"
#include <iostream>

// Verilator libraries.
#include "verilated.h"
#include "verilated_vcd_c.h"

using dut_type = VControlUnit;

uint32_t global_time = 0;

struct input_package {
    // input bits
    unsigned int clk;
    unsigned int Resetn = 0;
    unsigned int IRen = 0;
    unsigned int Run = 1;


    // 9-bit input busses
    unsigned int IR : 9;
};

struct output_package {
    //unsigned int unlck;
    unsigned int a = 0;
    unsigned int g = 0;
    unsigned int done = 0;
    unsigned int alu_op = 0;
    unsigned int mux : 3;
    unsigned int registers : 8;
};

static void advance_sim(dut_type* top, VerilatedVcdC* trace)
{
    trace->dump(global_time);
    top->eval();
    ++global_time;
}
    // CMD = 000 is move command
    // CMD = 001 is move immediate command
    // CMD = 010 is add command
    // CMD = 011 is substract command

int main() {
    srand(1234);

    dut_type* top = new dut_type;
    Verilated::traceEverOn(true);
    VerilatedVcdC* trace = new VerilatedVcdC;
    top->trace(trace, 99);
    trace->open("ControlUnit.vcd");

    input_package inp;
    output_package out;

    inp.IR = 0b010110101; 
    
    int CMD = (inp.IR & 0b111000000)/64;
    int X_Address = (inp.IR & 0b000111000)/8;
    int Y_Address = inp.IR & 0b000000111;

    int iter = 20;
    int CurrentState = 0;
    int NextState = 0;

    while(iter > 0)
    {
        inp.clk = global_time % 2;
        top->clk = inp.clk;
        switch (inp.clk)
       
       {
       case 1:
           if (inp.Resetn || out.done){
               CurrentState = 0;
               inp.IRen = 1;
           } else {
               CurrentState = NextState;
               inp.IRen = 0;
           }
           break;
       case 0:
            top->IR = inp.IR;
            break;
       }

        top->Resetn = inp.Resetn;
        top->Run = inp.Run;

        switch (CurrentState)
        {
        case 0:
            out.a = 0;
            out.g = 0;
            out.registers = 0;
            out.mux = 0;
            out.alu_op = 0;
            out.done = 0;
            if (CMD == 0b010 || CMD ==0b011){NextState = 1;}            
            //else if (inp.ir == ((inp.ir & 0b000111111)|0b011000000) || inp.ir == ((inp.ir & 0b000111111)|0b010000000)) {NextState = 1;}
            //else if (inp.ir == inp.ir & 0b001111111) {NextState = 5;}
            else if(CMD == 0b000){NextState = 4;}
            else if(CMD == 0b001){NextState = 5;} 
            //else {NextState = 0;}
            break;
        case 1:
            out.mux = 0b0000 + X_Address+1;
            out.registers = 1*2^(X_Address);
            out.a = 1;
            NextState = 2;
            out.done = 0;
            break;
        case 2:
            out.mux = 0b0000 + Y_Address + 1;
            out.registers = 1*2^(Y_Address);
            if (inp.IR > 64)
            {
                out.alu_op = 1;
            } else { out.alu_op = 0;}
            out.a = 1;
            out.g = 1;
            NextState = 3;
            out.done = 0;
            break;
        case 3:
            out.a = 0;
            out.alu_op = 0;
            out.mux = 0b1111;
            out.g = 1;
            out.registers = 1*2^(X_Address);
            out.done = 1;
            NextState = 0;
            break;
        case 4:
            out.mux = 0b0000 + Y_Address +1;
            out.registers = 1*2^(X_Address) + 1*2^(Y_Address);
            out.done = 1;
            break;            
        case 5:
            out.mux = 0b0000;
            out.registers = 1*2^(X_Address);
            out.done = 1;
            break;    
        default:
            break;
        }
        
        top->eval();
        advance_sim(top, trace);

        if(top->done != out.done) {
            std::cout << "iter" << iter << "; ";
            std::cout << "clk" << inp.clk << "; ";
            std::cout << "instruction" << inp.IR << "; ";
            std::cout << "registers = " << (int)out.registers << "; ";
            std::cout << "a = " << (int)out.a << "; ";
            std::cout << "g = " << (int)out.g << "; ";
            std::cout << "alu_op = " << (int)out.alu_op << "; ";
            std::cout << "mux = " << (int)out.mux << "; ";
            std::cout << "done = " << (int)out.done << "; ";
        }

        //sim iteration
        iter--;
    }

    trace->close();
}
