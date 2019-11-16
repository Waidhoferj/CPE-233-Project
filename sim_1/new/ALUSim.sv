`timescale 1ns / 1ps
module ALUSim();
//IO
logic [31:0] A = 0, B = 0, ALU_OUT = 0;
logic [3:0] ALU_FUN = 0;

//Test values
logic [31:0] a_inputs [13];
logic [31:0] b_inputs [13];
logic [3:0] operation_inputs [13];
logic [31:0] expected_results [13];
logic CLK;
//ALU functions

logic [3:0] _add = 4'b0000;
logic [3:0] _sub = 4'b1000;
logic [3:0] _or = 4'b0110;
logic [3:0] _and = 4'b0110;
logic [3:0] _xor = 4'b0100;
logic [3:0] _srl = 4'b0101;
logic [3:0] _sll = 4'b0001;
logic [3:0] _sra = 4'b1101;
logic [3:0] _slt = 4'b0010;
logic [3:0] _sltu = 4'b0011;
logic [3:0] _lui = 4'b1001;

//Start clock
always
    begin
        CLK = 0; 
        #5;
        CLK = 1;
        #5;
    end

ALU instALU (.*);

initial begin

//Init test values
    a_inputs <= '{32'hAA, 32'hC8, 32'hC8, 32'h12C8, 32'hAAAABBBB, 32'hAAAA, 32'hAA,32'hAA, -20, 32'hAA, -5, 32'hAA, 32'hABC12301};

    b_inputs <= '{32'hAA, 32'h37, 32'h64, 32'h12C8, 32'hFFFFFFFF, 32'h0A, 32'h0C, 32'h03, 2, 32'hAA, -10, 32'h55, 32'h12345678};

    operation_inputs <= '{_add, _sub, _or, _and, _xor, _srl, _sll, _sra, _sra, _slt, _slt, _sltu, _lui};

    expected_results <= '{32'h154,32'h91,32'hEC,32'h12C8,32'h55554444,32'h2A,32'hAA000,32'h15,32'hfffffffB,32'h0,32'h0,32'h0, 32'h12345678};

#10;
//Run tests
    for(int i = 0; i < 13; i++) begin
        ALU_FUN = operation_inputs[i];
        A = a_inputs[i];
        B = b_inputs[i];
        #5;
        if(ALU_OUT != expected_results[i]) $display("Test #%0d failed\n Command: %0d, A: %0h, B: %0h, Result: %0h", i, ALU_FUN, A, B, ALU_OUT);
        #5;
    end
end
endmodule
