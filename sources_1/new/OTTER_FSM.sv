`timescale 1ns / 1ps

module CU_FSM2(
   input CLK, INT, RST, 
   input [6:0] CU_OPCODE,
   output logic PC_WRITE, REG_WRITE, MEM_WRITE, MEM_READ1, MEM_READ2
   
   );
   typedef enum {fetch, execute, write_back} States;
   
   typedef enum logic [6:0] { 
    LUI = 7'b0110111,
    AUIPC = 7'b0010111,
    JAL = 7'b1101111,
    JALR = 7'b1100111,
    BRANCH = 7'b1100011,
    LOAD = 7'b0000011,
    STORE = 7'b0100011,
    OP_IMM = 7'b0010011,
    OP = 7'b0110011
 } Opcode;
   
   Opcode opcode;
   assign opcode = Opcode'(CU_OPCODE);
   States next_state, current_state;

   always_ff @(posedge CLK) begin 
      if(RST)
         current_state <= fetch;
      else
         current_state <= next_state;
   end
   
   always_comb begin
      case(current_state)
         fetch: begin
            MEM_READ1 = 1;
            PC_WRITE = 1;
            REG_WRITE = 0;
            MEM_WRITE = 0;
            MEM_READ2 = 0;
            next_state = execute;
         end
         
         execute: begin
            case(opcode)
               LUI, AUIPC, JAL, JALR, OP, OP_IMM: begin
                  PC_WRITE = 1;
                  REG_WRITE = 1;
                  MEM_WRITE = 0;
                  MEM_READ1 = 0;
                  MEM_READ2 = 0;
                  end
                  
               BRANCH: begin
                  PC_WRITE = 1;
                  REG_WRITE = 0;
                  MEM_WRITE = 0;
                  MEM_READ1 = 0;
                  MEM_READ2 = 0;
                  end
               LOAD: begin
                  PC_WRITE = 0;
                  REG_WRITE = 0;
                  MEM_WRITE = 0;
                  MEM_READ1 = 0;
                  MEM_READ2 = 1;
                  end
               STORE: begin
                  PC_WRITE = 1;
                  REG_WRITE = 0;
                  MEM_WRITE = 1;
                  MEM_READ1 = 0;
                  MEM_READ2 = 0;
                  end
            endcase
            
            //mealy
            if(opcode == LOAD)
            next_state = write_back;
            else 
            next_state = fetch;
         end
         
      write_back: begin
         REG_WRITE = 1;
         PC_WRITE = 1;
         next_state = fetch;
         end
      
      endcase
   
   end
   endmodule
