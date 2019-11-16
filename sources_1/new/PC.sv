`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/23/2019 11:24:34 AM
// Design Name: 
// Module Name: PC
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

module PC(
    input [31:0] DIN,
    input PC_WRITE, RESET, CLK,
    output logic [31:0] DOUT = 0
);

always_ff @(posedge CLK)
begin
    if (RESET)
        DOUT <= 0;
    else if(PC_WRITE)
        DOUT <= DIN;
end
endmodule