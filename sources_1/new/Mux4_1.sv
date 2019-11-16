`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/23/2019 11:18:06 AM
// Design Name: 
// Module Name: Mux4_1
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


module Mux4_1(
    input [31:0] ZERO, ONE, TWO, THREE,
    input [1:0] SEL,
    output logic [31:0] DOUT
);

always_comb
begin
    case(SEL)
        0: DOUT <= ZERO;
        1: DOUT <= ONE;
        2: DOUT <= TWO;
        3: DOUT <= THREE;
        default: DOUT <= ZERO;
    endcase
end
endmodule