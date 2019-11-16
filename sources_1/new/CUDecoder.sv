`timescale 1ns / 1ps

module CUDecoder (
    input BR_EQ, BR_LT, BR_LTU, INT_TAKEN,
    input [2:0] FUNC3,
    input [6:0] FUNC7, CU_OPCODE,
    output logic [3:0] ALU_FUN, 
    output logic [2:0] PC_SOURCE,
    output logic [1:0] ALU_SRCB, RF_WR_SEL,
    output logic ALU_SRCA
);

typedef enum logic [6:0] { 
    LUI = 7'b0110111,
    AUIPC = 7'b0010111,
    JAL = 7'b1101111,
    JALR = 7'b1100111,
    BRANCH = 7'b1100011,
    LOAD = 7'b0000011,
    STORE = 7'b0100011,
    OP_IMM = 7'b0010011,
    OP = 7'b0110011,
    SYSTEM = 7'b1110011
 } opcode_t;
 
 typedef enum logic [2:0] {
 BEQ= 3'b000,
 BNE= 3'b001,
 BLT= 3'b100,
 BGE= 3'b101,
 BLTU=3'b110,
 BGEU= 3'b111
 } branch_func;
 
 branch_func BR_FUNC;
 assign BR_FUNC = branch_func'(FUNC3);

 opcode_t OPCODE;
 assign OPCODE = opcode_t'(CU_OPCODE);
 always_comb begin
        case(OPCODE)
    LUI: 
    begin
        ALU_FUN = 9;
        ALU_SRCA = 1;
        PC_SOURCE = 0;
        RF_WR_SEL = 3;
    end
    AUIPC: 
    begin
        ALU_FUN = 0;
        ALU_SRCA = 1;
        ALU_SRCB = 3;
        PC_SOURCE = 0;
        RF_WR_SEL = 3;
    end
    JAL: 
    begin 
        PC_SOURCE = 3;
        RF_WR_SEL = 0;
    end
    JALR: 
    begin
        PC_SOURCE = 1;
        RF_WR_SEL = 0;
    end
    BRANCH: 
    begin
        if (BR_FUNC == BEQ && BR_EQ == 1)
            PC_SOURCE = 2;
        else if (BR_FUNC == BNE && BR_EQ == 0)
            PC_SOURCE = 2;
        else if (BR_FUNC == BLT && BR_LT == 1)
            PC_SOURCE = 2;
        else if(BR_FUNC == BGE && BR_LT == 0)
             PC_SOURCE = 2;
        else if (BR_FUNC == BLTU && BR_LTU == 1)
            PC_SOURCE = 2;
        else if(BR_FUNC == BGEU && BR_LTU == 0)
            PC_SOURCE = 2;
        else PC_SOURCE = 0;
    end
    LOAD: 
    begin
        ALU_FUN = 0;
        ALU_SRCA = 0;
        ALU_SRCB = 1;
        PC_SOURCE = 0;
        RF_WR_SEL = 2;
    end
    STORE: 
    begin
        ALU_FUN = 0;
        ALU_SRCA = 0;
        ALU_SRCB = 2;
        PC_SOURCE = 0;
    end
    OP_IMM: 
    begin
    if(FUNC7 == 7'b0100000 && FUNC3 == 3'b101) begin
        ALU_FUN = {FUNC7[5], FUNC3};
        end
    else begin
        ALU_FUN = {1'b0, FUNC3};
        end
        ALU_SRCA = 0;
        ALU_SRCB = 1;
        PC_SOURCE = 0;
        RF_WR_SEL = 3;
    end
    OP: 
    begin
    ALU_FUN = {FUNC7[5], FUNC3};
        ALU_SRCA = 0;
        ALU_SRCB = 0;
        PC_SOURCE = 0;
        RF_WR_SEL = 3;
    end 
    SYSTEM: begin
        ALU_SRCA = 0;
        ALU_FUN = 9;
        RF_WR_SEL = 1;
        if(FUNC3 == 3'b001) PC_SOURCE = 0;
        else PC_SOURCE = 5;
       
        
    end   

 endcase
 if(INT_TAKEN) PC_SOURCE = 4;
    end

 
endmodule