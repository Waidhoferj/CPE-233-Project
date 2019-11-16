`timescale 1ns / 1ps

module ImmediateGenerator(
input [31:0] IR,
output logic [31:0] U_Type,
output logic [31:0] I_Type,
output logic [31:0] S_Type,
output logic [31:0] J_Type,
output logic [31:0] B_Type
);
assign U_Type = {IR[31:12], 12'b0};
assign I_Type = {{20{IR[31]}},IR[31:20]}; 
assign S_Type = {{21{IR[31]}},IR[30:25], IR[11:8], IR[7]};
assign J_Type = {{12{IR[31]}},IR[19:12], IR[20], IR[30:21], 1'b0};
assign B_Type = {{20{IR[31]}},IR[7], IR[30:25], IR[11:8], 1'b0};
endmodule

