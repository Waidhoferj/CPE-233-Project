`timescale 1ns / 1ps

module OTTER_Wrapper_Sim2();
    logic CLK;
   logic BTNL;
   logic BTNC;
   logic [15:0] SWITCHES;
   logic [15:0] LEDS;
   logic [7:0] CATHODES;
   logic [3:0] ANODES;
    
    always
    begin
        CLK = 0; 
        #5;
        CLK = 1;
        #5;
    end

    OTTER_Wrapper Project(.*);
    
    
    initial begin
    SWITCHES = 0;
    end
endmodule
