`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/23/2019 11:40:22 AM
// Design Name: 
// Module Name: Lab1TopSim
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


module Lab1TopSim ();
    logic [31:0] JALR, BRANCH, JUMP, DOUT;
    logic CLK, RESET, MEM_READ1, PC_WRITE;
    logic [1:0] PC_SOURCE;
    logic [31:0] temp_out;

    always
    begin
        CLK = 0; 
        #5;
        CLK = 1;
        #5;
    end

    Lab1Top MyLab1Top (.*);

    initial begin
    //INIT
    //-------------------------------
    //Predetermined value set
        JALR = 12; //mem val: 00532023
        JUMP = 4; //mem val: 00032283
        BRANCH = 8; //mem val = fff2c293
        //writes address to memory block
        PC_WRITE = 1; 
        //Determines whether address is read into memory
        MEM_READ1 = 1;
        //Selects command from mux
        PC_SOURCE = 0;
        //pc_out is the memory index of the current command
        //DOUT outputs the command to run
        temp_out = DOUT;

        //TESTS
        //------------------------------
        //Test the outputs of the mux // will the memory add 4?
        #50;
        PC_SOURCE = 1;
        #50;
        PC_SOURCE = 2;
        #50;
        PC_SOURCE = 3;
        #50;
        RESET = 1;
        #50;
        PC_SOURCE = 0;
        #50
        //DOUT increment continuously to top

        //TEST MEM READ BLOCKS
        RESET = 1;
        
        #50;
        MEM_READ1 = 0;
        temp_out = DOUT;
        #50;
        PC_SOURCE = 2;
        PC_WRITE = 1;
        #25
        if(temp_out != DOUT) $display("MEM_READ does not block input");
        
        //TEST PC Write
        RESET = 0;
        PC_SOURCE = 2;
        MEM_READ1 = 1;
        #25;
        PC_WRITE = 0;
        temp_out = DOUT;
        #25
        PC_SOURCE = 3;
        #25
        if(temp_out != DOUT) $display("PC_Write does not block input");


        RESET = 1;
        #50;

    end

    

endmodule
