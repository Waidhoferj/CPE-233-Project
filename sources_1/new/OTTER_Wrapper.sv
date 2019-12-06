`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: J. Calllenes
//           P. Hummel
// 
// Create Date: 01/20/2019 10:36:50 AM
// Design Name: 
// Module Name: OTTER_Wrapper 
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

module OTTER_Wrapper(
   input CLK,
   input BTNL,
   input BTNC,
   input BTNU,
   input BTNR,
   input BTND,
   input [15:0] SWITCHES,
   input PS2Clk,
   input PS2Data,
   input RX, 
   inout [3:0] JA,
   output logic [15:0] LEDS,
   output [7:0] CATHODES,
   output [3:0] ANODES,
   output [7:0] VGA_RGB,
   output VGA_HS,
   output VGA_VS,
   output TX
   );
        
    logic sclk2 =0;
    // INPUT PORT IDS ////////////////////////////////////////////////////////
    // Right now, the only possible inputs are the switches
    // In future labs you can add more MMIO, and you'll have
    // to add constants here for the mux below
    localparam SWITCHES_AD = 32'h11000000;
    localparam VGA_READ_AD = 32'h11040000;
    localparam GYRO_IN_X = 32'h11080000;
    localparam GYRO_IN_Y = 32'h11090000;
    localparam GYRO_IN_Z = 32'h110a0000;
    localparam BTN_LEFT_ADDR = 32'h110b0000;
    localparam BTN_RIGHT_ADDR = 32'h110c0000;
    localparam BTN_TOP_ADDR = 32'h110d0000;
    localparam BTN_BOTTOM_ADDR = 32'h110e0000;
    localparam BTN_CENTER_ADDR = 32'h110f0000;
    
           
    // OUTPUT PORT IDS ///////////////////////////////////////////////////////
    // In future labs you can add more MMIO
    localparam LEDS_AD      = 32'h11080000;
    localparam SSEG_AD     = 32'h110C0000;
    localparam VGA_ADDR_AD = 32'h11100000;
    localparam VGA_COLOR_AD = 32'h11140000; 
    localparam KEYBOARD_AD = 32'h11200000;
    
   // Signals for connecting OTTER_MCU to OTTER_wrapper /////////////////////////
   logic s_interrupt, keyboard_int,btn_int;
   logic s_reset,s_load;
   logic sclk; // = 1'b0;   
   
   // Signals for connecting VGA Framebuffer Driver
   logic r_vga_we;             // write enable
   logic [12:0] r_vga_wa;      // address of framebuffer to read and write
   logic [7:0] r_vga_wd;       // pixel color data to write to framebuffer
   logic [7:0] r_vga_rd;       // pixel color data read from framebuffer
 
   logic [15:0]  r_SSEG;// = 16'h0000;

   logic [7:0] s_scancode;
     
   logic [31:0] IOBUS_out,IOBUS_in,IOBUS_addr;
   logic IOBUS_wr;
   
   assign s_interrupt = keyboard_int | btn_int;
   
    // Declare OTTER_CPU ///////////////////////////////////////////////////////
   OTTER_MCU MCU (.RST(s_reset),.INTR(s_interrupt), .CLK(sclk), .TX(TX), .RX(RX), 
                   .IOBUS_OUT(IOBUS_out),.IOBUS_IN(IOBUS_in),.IOBUS_ADDR(IOBUS_addr),.IOBUS_WR(IOBUS_wr));

   // Declare Seven Segment Display /////////////////////////////////////////
   SevSegDisp SSG_DISP (.DATA_IN(r_SSEG), .CLK(CLK), .MODE(1'b0),
                       .CATHODES(CATHODES), .ANODES(ANODES));
   
   // Declare Debouncer One Shot  ///////////////////////////////////////////
   debounce_one_shot DB(.CLK(sclk), .BTN(BTNL), .DB_BTN(btn_int));

   //CONTROLLER DECLARATIONS
   logic [15:0] tiltX, tiltY, tiltZ;
    //GyroTop Gyroscope(
        //.CLK(CLK), .RST(BTNU), .X(tiltX), .Y(tiltY), .Z(tiltZ), .JA(JA)
       // );

   // Declare VGA Frame Buffer //////////////////////////////////////////////
   vga_fb_driver_80x60 VGA(.CLK_50MHz(sclk), .WA(r_vga_wa), .WD(r_vga_wd),
                               .WE(r_vga_we), .RD(r_vga_rd), .ROUT(VGA_RGB[7:5]),
                               .GOUT(VGA_RGB[4:2]), .BOUT(VGA_RGB[1:0]),
                               .HS(VGA_HS), .VS(VGA_VS));   
 
                                          
   
  
 // Declare Keyboard Driver //////////////////////////////////////////////
    KeyboardDriver KEYBD (.CLK(CLK), .PS2DATA(PS2Data), .PS2CLK(PS2Clk),
                          .INTRPT(keyboard_int), .SCANCODE(s_scancode)); 
                           
   // Clock Divider to create 50 MHz Clock /////////////////////////////////
   always_ff @(posedge CLK) begin
       sclk <= ~sclk;
   end
   

    // Connect Signals ////////////////////////////////////////////////////////////
   assign s_reset = 0;
   
   //assign LEDS[15]=keyboard_int;
   // Connect Board peripherals (Memory Mapped IO devices) to IOBUS /////////////////////////////////////////
    always_ff @ (posedge sclk)
    begin
        r_vga_we<=0;       
        if(IOBUS_wr)
            case(IOBUS_addr)
                LEDS_AD: LEDS <= IOBUS_out;    
                SSEG_AD: r_SSEG <= IOBUS_out[15:0];
                VGA_ADDR_AD: r_vga_wa <= IOBUS_out[12:0];
                VGA_COLOR_AD: begin  r_vga_wd <= IOBUS_out[7:0];
                                     r_vga_we <= 1;  
                              end     
                
            endcase
            //if(keyboard_int)
            //r_SSEG <= {8'b0,s_scancode};
    end
    
    always_comb
    begin
        IOBUS_in=32'b0;
        case(IOBUS_addr)
            SWITCHES_AD: IOBUS_in[15:0] = SWITCHES;
            VGA_READ_AD: IOBUS_in[15:0] = r_vga_rd;           
            KEYBOARD_AD: IOBUS_in[7:0] = s_scancode;
            GYRO_IN_X: IOBUS_in[15:0] = tiltX;
            GYRO_IN_Y: IOBUS_in[15:0] = tiltY;
            GYRO_IN_Z: IOBUS_in[15:0] = tiltZ;
            BTN_RIGHT_ADDR: IOBUS_in[0] = BTNR;
            BTN_LEFT_ADDR: IOBUS_in[0] = BTNL;
            BTN_TOP_ADDR: IOBUS_in[0] = BTNU;
            BTN_BOTTOM_ADDR: IOBUS_in[0] = BTND;
            BTN_CENTER_ADDR: IOBUS_in[0] = BTNC;

            
            default: IOBUS_in=32'b0;
        endcase
    end
   endmodule

