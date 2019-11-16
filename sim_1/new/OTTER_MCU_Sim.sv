`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/29/2019 03:19:52 PM
// Design Name: 
// Module Name: OTTER_MCU_Sim
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


module OTTER_MCU_Sim;
logic CLK, RST, INTR, IOBUS_WR;
logic [31:0] IOBUS_IN,IOBUS_OUT, IOBUS_ADDR;

OTTER_MCU MCU(.*);
always
    begin
        CLK = 0; 
        #5;
        CLK = 1;
        #5;
    end
    
    
    initial begin 
    INTR = 1;
    #220
    INTR = 0;
    #1000
    INTR = 1;
    #1030 
    INTR = 0;
    end
    
    


endmodule
