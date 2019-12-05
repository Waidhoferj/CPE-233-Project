`timescale 1ns / 1ps
// Module: GyroTilt
//Purpose: Converts angular velocity data into tilt angle of gyro

//Inputs
    // dx - dz: the current angular velocity from GyroFsm
    // RST: will set each dimension to 0 degree tilt
//Outputs
    // X-Z: the current tilt in each dimension


module GyroTilt (
    input [15:0] dx, dy, dz, //Angular velocity delivered in degrees per second
    input RST,
    input CLK,
    output logic [15:0] X, Y, Z
);

//Internal Variables
//=============================================
logic [15:0] x, y, z; //state storage of tilt in each dimension
logic [15:0] slck = 0; //since we will sample every 1ms, we will wait for the slow clock to count up to 50 x 10^3 before sampling
logic [15:0] queue [50];
logic [15:0] avg;
parameter rate = 1000; //Scales dps to correct time sampling rate

//Functionality
//=============================================

//FUNCTIONS
function int bin_val(input int val);
    if(val > -42 && val < 10) return 0;
    else return val;
    endfunction

//Set internal variables to zero at start
initial begin
    x = 0;
    y = 0;
    z = 0;
    slck = 0;
    for(int i = 0; i < 50; i++) queue[i] = 0;
end

always_ff @(posedge CLK) begin
if(RST)begin
    x = 0;
    y = 0;
    z = 0;
    slck = 0;
end 
else begin
//Sample every 1ms
    if (slck < 50000) slck++; //increment
    else begin
    //sample
    x = 0;
    for(int i = 48; i < 0 ; i--) { 
        x += queue[i]
        queue[i + 1] = queue[i]
        }
    queue[0] = dx/rate;
    x/= 50;
    x = bin(x)
    
        y = y + dy/rate;
        z = z + dz/rate;
         X <= x;
        Y <= y;
        Z <= z;
     slck = 0;
    end
end
end


endmodule