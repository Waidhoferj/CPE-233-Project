`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/23/2019 11:39:54 AM
// Design Name: 
// Module Name: Lab1Top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Lab1Top(
    input [31:0] JALR, BRANCH, JUMP,
    input [1:0] PC_SOURCE,
    input PC_WRITE, RESET, CLK, MEM_READ1,
    output logic [31:0] DOUT
);

logic [31:0] pc_in, pc_out, pc_4;

Mux4_1 PC_Mux(.ZERO(pc_4), .ONE(JALR), .TWO(BRANCH), .THREE(JUMP), .SEL(PC_SOURCE), .DOUT(pc_in));

PC ProgramCounter(.DIN(pc_in), .CLK(CLK), .RESET(RESET), .PC_WRITE(PC_WRITE), .DOUT(pc_out));

assign pc_4 = pc_out + 4;

OTTER_mem_byte OTTER_Mem(.MEM_CLK(CLK), .MEM_ADDR1(pc_out), .MEM_READ1(MEM_READ1), .MEM_DOUT1(DOUT));


endmodule


