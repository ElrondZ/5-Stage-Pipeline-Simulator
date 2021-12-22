#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
public:
    bitset<32> Reg_data;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32> (0);
    }

    bitset<32> readRF(bitset<5> Reg_addr)
    {
        Reg_data = Registers[Reg_addr.to_ulong()];
        return Reg_data;
    }

    void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
    {
        Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
    }

    void outputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt",std::ios_base::app);
        if (rfout.is_open())
        {
            rfout<<"State of RF:\t"<<endl;
            for (int j = 0; j<32; j++)
            {
                rfout << Registers[j]<<endl;
            }
        }
        else cout<<"Unable to open file";
        rfout.close();
    }

private:
    vector<bitset<32> >Registers;
};

class INSMem
{
public:
    bitset<32> Instruction;
    INSMem()
    {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i=0;
        imem.open("imem.txt");
        if (imem.is_open())
        {
            while (getline(imem,line))
            {
                IMem[i] = bitset<8>(line.substr(0,8));
                i++;
            }
        }
        else cout<<"Unable to open file";
        imem.close();
    }

    bitset<32> readInstr(bitset<32> ReadAddress)
    {
        string insmem;
        insmem.append(IMem[ReadAddress.to_ulong()].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
        Instruction = bitset<32>(insmem);        //read instruction memory
        return Instruction;
    }

private:
    vector<bitset<8> > IMem;
};

class DataMem
{
public:
    bitset<32> ReadData;
    DataMem()
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i=0;
        dmem.open("dmem.txt");
        if (dmem.is_open())
        {
            cout << "open dmem.txt" << endl;
            while (getline(dmem,line))
            {
                DMem[i] = bitset<8>(line.substr(0,8));
                i++;
            }
        }
        else cout<<"Unable to open file";
        dmem.close();
    }

    bitset<32> readDataMem(bitset<32> Address)
    {
        string datamem;
        datamem.append(DMem[Address.to_ulong()].to_string());
        datamem.append(DMem[Address.to_ulong()+1].to_string());
        datamem.append(DMem[Address.to_ulong()+2].to_string());
        datamem.append(DMem[Address.to_ulong()+3].to_string());
        ReadData = bitset<32>(datamem);        //read data memory
        return ReadData;
    }

    void writeDataMem(bitset<32> Address, bitset<32> WriteData)
    {
        DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
        DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
        DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
        DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
    }

    void outputDataMem()
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j< 1000; j++)
            {
                dmemout << DMem[j]<<endl;
            }

        }
        else cout<<"Unable to open file";
        dmemout.close();
    }

private:
    vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;

        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;

        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> getALUResult(bool alu_op, bitset<32> oprand1, bitset<32> oprand2) {
    unsigned int result = 0;
    if (alu_op) {
        result = oprand1.to_ulong() + oprand2.to_ulong();
    }
    else {
        result = oprand1.to_ulong() - oprand2.to_ulong();
    }
    return result;
}

string extendString(bitset<16> immediate) {
    string operation_st;
    if (immediate[15] == 0) {
        operation_st = "0000000000000000" + immediate.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    else if (immediate[15] == 1) {
        operation_st = "1111111111111111" + immediate.to_string<char,std::string::traits_type,std::string::allocator_type>();
    }
    return operation_st;
}

class ALU {
public:
    bitset<32> ALUresult;

    bitset<32> ALU_Cal (bool alu_op, bitset<32> oprand1, bitset<32> oprand2) {

        return ALUresult = getALUResult(alu_op, oprand1, oprand2);
    }
};

class Decoder {
public:

    bitset<32> signextend (bitset<16> imm) {
        return (bitset<32> (extendString(imm)));
    }

    unsigned long bits_shift(bitset<32> inst, int start) {
        unsigned long temp = (inst.to_ulong()) >> start;
        return(temp);
    }


};


void WB_Operations(stateStruct &state, RF &myRF) {
    if (state.WB.nop != 1) {
        if(state.WB.wrt_enable) {
            myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
        }
    }
}

void MEM_Operations(stateStruct &state, stateStruct &newState, DataMem &myDataMem) {
    if(state.MEM.nop) {
        newState.WB.nop = true;
    }
    else {
        newState.WB.nop = false;
        bitset<32> WriteData = state.MEM.Store_data;

        if(state.MEM.rd_mem) {
            myDataMem.readDataMem(state.MEM.ALUresult);
            newState.WB.Wrt_data = myDataMem.ReadData;
        }

        else if(state.MEM.wrt_mem) {
            if(state.WB.Wrt_reg_addr == state.MEM.Rt && state.WB.wrt_enable) {
                WriteData = state.WB.Wrt_data;
            }
            myDataMem.writeDataMem(state.MEM.ALUresult, WriteData);
        }

        else {
            if(state.MEM.wrt_enable) {
                newState.WB.Wrt_data = state.MEM.ALUresult;
            }
            else {
                newState.WB.Wrt_data = state.WB.Wrt_data;
            }
        }
        newState.WB.Rs = state.MEM.Rs;
        newState.WB.Rt = state.MEM.Rt;
        newState.WB.wrt_enable = state.MEM.wrt_enable;
        newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
    }
}

void EX_Operations(bitset<32> &signext, stateStruct &state, stateStruct &newState, Decoder &myDecoder, DataMem &myDataMem, ALU &myALU) {
    if(state.EX.nop) {
        newState.MEM.nop = true;
    }

    else {
        newState.MEM.nop = false;
        bitset<32> Oprand1 = state.EX.Read_data1;
        bitset<32> Oprand2 = state.EX.Read_data2;
        signext = myDecoder.signextend(state.EX.Imm);

        if(state.WB.wrt_enable) {
            if(state.WB.Wrt_reg_addr == state.EX.Rt) {
                Oprand2 = state.WB.Wrt_data;
            }
            else if(state.WB.Wrt_reg_addr == state.EX.Rs) {
                Oprand1 = state.WB.Wrt_data;
            }
        }

        if(state.MEM.wrt_enable) {
            if(state.MEM.Wrt_reg_addr == state.EX.Rt) {
                if(state.MEM.rd_mem) {
                    Oprand2 = myDataMem.ReadData;
                }
                else if(state.MEM.wrt_enable) {
                    Oprand2 = state.MEM.ALUresult;
                }
            }

            else if(state.MEM.Wrt_reg_addr == state.EX.Rs) {
                if(state.MEM.rd_mem) {
                    Oprand1 = myDataMem.ReadData;
                }
                else if(state.MEM.wrt_enable) {
                    Oprand1 = state.MEM.ALUresult;
                }
            }
        }

        if(!state.EX.is_I_type) {
            myALU.ALU_Cal(state.EX.alu_op,Oprand1,Oprand2);

        }
        else {
            myALU.ALU_Cal(state.EX.alu_op,Oprand1,signext);
        }

        newState.MEM.ALUresult = myALU.ALUresult;

        if(!state.EX.is_I_type && !state.EX.wrt_enable) {
            newState.MEM.ALUresult = 0;
        }

        newState.MEM.Store_data = Oprand2;

        if(state.EX.rd_mem) {
            newState.MEM.Store_data = 0;
        }

        newState.MEM.Rs = state.EX.Rs;
        newState.MEM.Rt = state.EX.Rt;
        newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
        newState.MEM.rd_mem = state.EX.rd_mem;
        newState.MEM.wrt_mem = state.EX.wrt_mem;
        newState.MEM.wrt_enable = state.EX.wrt_enable;
    }
}

void TypeCheck(Decoder &myDecoder, bool &IsRType, bool &IsIType, bitset<5> &WReg, bitset<5> &RReg1, bitset<5> &RReg2, bitset<32> &instr, bitset<6> &funct, bool &ALUop, bitset<6> &opcode, stateStruct &state, bool &STALL) {
    if(IsRType) {
        WReg = bitset<5> (myDecoder.bits_shift(instr, 11));
    }
    else if(IsIType) {
        WReg = RReg2;
    }

    if(funct.to_ulong() == 35) {
        ALUop = false;
    }
    else if(funct.to_ulong() == 33 || opcode.to_ulong() == 35 || opcode.to_ulong() == 43) {
        ALUop = true;
    }
    //check stall
    if(state.EX.rd_mem) {
        if(RReg1 == state.EX.Wrt_reg_addr || (IsRType && RReg2 == state.EX.Wrt_reg_addr)) {
            STALL = true;
        }
    }
}

void Branch_Check(bool &IsBranch, RF &myRF, bitset<5> &RReg1, bitset<5> &RReg2, bool &BRANCH) {
    if(IsBranch && (myRF.readRF(RReg1) != myRF.readRF(RReg2))) {
        BRANCH = true;
    }
}

void store_and_branch(bool &IsStore, bool &IsBranch, bool &WrtEnable) {
    if(IsStore || IsBranch) {
        WrtEnable = 0;
    }
    else {
        WrtEnable = 1;
    }
}

void store_and_banch_check(bool &STALL, bool &BRANCH, stateStruct &newState, stateStruct &state, bitset<32> &BranchAddr) {
    if(STALL) {
        newState.IF = state.IF;
        newState.ID = state.ID;
        newState.EX.nop = true;
        STALL = false;
    }

    if(BRANCH) {
        newState.IF.PC = BranchAddr;
        newState.ID = state.ID;
        newState.ID.nop = true;
        BRANCH = false;
    }
}





int main()
{
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    ALU myALU;
    Decoder myDecoder;
    stateStruct state,newState;

    bool IsIType;
    bool IsRType;
    bool WrtEnable;
    bool IsLoad;
    bool IsStore;
    bool IsBranch;
    bool ALUop;
    bool STALL = false;
    bool BRANCH = false;

    int cycle = 0;

    //WB
    state.WB.Rs = bitset<5> (0);
    state.WB.Rt = bitset<5> (0);
    state.WB.Wrt_reg_addr = bitset<5> (0);
    state.WB.Wrt_data = bitset<32> (0);
    state.WB.wrt_enable = false;
    state.WB.nop = true;

    //IF
    state.IF.PC = bitset<32> (0);
    state.IF.nop = false;

    //MEM
    state.MEM.ALUresult = bitset<32> (0);
    state.MEM.Rs = bitset<5> (0);
    state.MEM.Rt = bitset<5> (0);
    state.MEM.Store_data = bitset<32> (0);
    state.MEM.Wrt_reg_addr = bitset<5> (0);
    state.MEM.rd_mem = false;
    state.MEM.wrt_mem = false;
    state.MEM.wrt_enable = false;
    state.MEM.nop = true;

    //EX
    state.EX.Read_data1 = bitset<32> (0);
    state.EX.Read_data2 = bitset<32> (0);
    state.EX.Rs = bitset<5> (0);
    state.EX.Imm = bitset<16> (0);
    state.EX.Rt = bitset<5> (0);
    state.EX.Wrt_reg_addr = bitset<5> (0);
    state.EX.is_I_type = false;
    state.EX.alu_op = true;
    state.EX.rd_mem = false;
    state.EX.wrt_mem = false;
    state.EX.wrt_enable = false;
    state.EX.nop = true;

    //ID
    state.ID.Instr = bitset<32> (0);
    state.ID.nop = true;

    bitset<6> opcode;
    bitset<6> funct;
    bitset<5> RReg1;
    bitset<5> RReg2;
    bitset<16> imm;
    bitset<5> WReg = bitset<5> (0);
    bitset<5> shamt = bitset<5> (0);
    bitset<32> signext = bitset<32> (0);
    bitset<32> BranchAddr = bitset<32> (0);
    bitset<32> instruction = bitset<32> (0);

    newState = state;


    while (true) {

        // Print the cycle number
        cout << "This is " << cycle << " Cycle" << endl;

        /* --------------------- WB stage --------------------- */
        WB_Operations(state, myRF);

        /* --------------------- MEM stage --------------------- */
        MEM_Operations(state, newState, myDataMem);

        /* --------------------- EX stage --------------------- */
        EX_Operations(signext, state, newState, myDecoder, myDataMem, myALU);
        
        /* --------------------- ID stage --------------------- */
        if(state.ID.nop) {
            newState.EX.nop = 1;
        }

        else {
            newState.EX.nop = 0;
            bitset <32> instr = state.ID.Instr;
            opcode = bitset<6>(myDecoder.bits_shift(instr, 26));
            IsRType = opcode.to_ulong() == 0;
            IsIType = opcode.to_ulong() != 0;
            IsBranch = opcode.to_ulong() == 4;
            IsLoad = opcode.to_ulong() == 35;
            IsStore = opcode.to_ulong() == 43;

            store_and_branch(IsStore, IsBranch, WrtEnable);

            funct = bitset<6> (myDecoder.bits_shift(instr, 0));
            RReg1 = bitset<5> (myDecoder.bits_shift(instr, 21));
            RReg2 = bitset<5> (myDecoder.bits_shift(instr, 16));
            imm = bitset<16> (myDecoder.bits_shift(instr, 0));

            BranchAddr = bitset<32> (state.IF.PC.to_ulong() + (bitset<32>((bitset<30> (myDecoder.bits_shift(myDecoder.signextend(imm),0))).to_string()+"00")).to_ulong());

            TypeCheck(myDecoder, IsRType, IsIType, WReg, RReg1, RReg2, instr, funct, ALUop, opcode, state, STALL);

            Branch_Check(IsBranch, myRF, RReg1, RReg2, BRANCH);

            newState.EX.is_I_type = IsIType && !IsBranch;
            newState.EX.Wrt_reg_addr = WReg;

            newState.EX.Imm = imm;
            newState.EX.Rs = RReg1;
            newState.EX.Rt = RReg2;

            newState.EX.Read_data1 = myRF.readRF(RReg1);
            newState.EX.Read_data2 = myRF.readRF(RReg2);


            newState.EX.rd_mem = IsLoad;
            newState.EX.wrt_mem = IsStore;
            newState.EX.alu_op = ALUop;
            newState.EX.wrt_enable = WrtEnable;
        }

        /* --------------------- IF stage --------------------- */
        if(state.IF.nop) {
            newState.ID.nop = true;
        }
        else {
            newState.ID.nop = false;
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            newState.IF.PC = state.IF.PC.to_ulong() + 4;

            if(myInsMem.readInstr(state.IF.PC) == 0xffffffff) {
                newState.IF.nop = true;
                newState.ID.nop = true;
                newState.IF.PC = state.IF.PC;
                newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            }
        }

        store_and_banch_check(STALL, BRANCH, newState, state, BranchAddr);

        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;

        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ...
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
        cycle++;
    }

    myRF.outputRF(); // dump RF;
    myDataMem.outputDataMem(); // dump data mem

    return 0;
}