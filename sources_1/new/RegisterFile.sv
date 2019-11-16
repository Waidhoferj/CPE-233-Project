`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/25/2019 11:42:34 AM
// Design Name: 
// Module Name: RegisterFile
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


//32x32 RAM;
module RegisterFile(
	input CLK, EN,
	input [31:0] WD,
	input [4:0] ADR1, ADR2, WA,
	output logic [31:0] RS1, RS2
);
logic [31:0] memory [0:31];

initial
begin
	for(int i = 0; i < 32; i++)
	begin
		//Assign each word in memory to zero
		memory[i] = 0;
	end
end

assign RS1 = memory[ADR1];
assign RS2 = memory[ADR2];

always_ff @(posedge CLK)
begin
	if(EN && WA != 0)
		memory[WA] <= WD;
end



endmodule  
