`timescale 1ns / 1ps
module ALU (
    input [31:0] A, B,
    input [3:0] ALU_FUN,
    output logic [31:0] ALU_OUT
);
//fdsfd
//HEre is my changes
typedef enum logic [3:0] {
_add = 4'b0000,
_sub = 4'b1000,
_or = 4'b0110,
_and = 4'b0111,
_xor = 4'b0100,
_srl = 4'b0101,
_sll = 4'b0001,
_sra = 4'b1101,
_slt = 4'b0010,
_sltu = 4'b0011,
_lui = 4'b1001
 } Operations;
 Operations operation;
 assign operation = Operations'(ALU_FUN);


always_comb
begin
    case(operation)
    _add: ALU_OUT = A + B;
    _sub: ALU_OUT = A - B;
    _or: ALU_OUT = A | B;
    _and: ALU_OUT = A & B;
    _xor: ALU_OUT = A ^ B;
    _srl: ALU_OUT = A >> B [4:0];
    _sll: ALU_OUT = A << B [4:0];
    _sra: ALU_OUT = $signed(A) >>> B [4:0];
    _slt: ALU_OUT = ($signed(A) < $signed(B)) ? 1: 0;
    _sltu: ALU_OUT = (A < B) ? 1 : 0;
    _lui: ALU_OUT = A;

    default: ALU_OUT = ALU_OUT;
    endcase
end
endmodule