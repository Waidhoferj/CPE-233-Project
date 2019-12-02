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
parameter rate = 1000; //Scales dps to correct time sampling rate

//Functionality
//=============================================

//Outputs will reflect internal state
assign X = x;
assign Y = y;
assign Z = z;

//Set internal variables to zero at start
initial begin
    x = 0;
    y = 0;
    z = 0;
    slck = 0;
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
        x = x + dx/rate;
        y = y + dy/rate;
        z = z + dz/rate;
     slck = 0;
    end
end
end


endmodule