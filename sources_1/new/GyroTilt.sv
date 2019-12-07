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
logic [15:0] avgs [3];
logic [15:0] queue_x [50];
logic [15:0] queue_y [50];
logic [15:0] queue_z [50];
logic [15:0] avgs [3];
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

    for(int i = 0; i < 50; i++) {
        queue_x[i] = 0;
        queue_y[i] = 0;
        queue_z[i] = 0;
        

    }

    for(int i = 0; i < 3; i++) avgs[i] = 0;
end

always_ff @(posedge CLK) begin
if(RST)begin
    x = 0;
    y = 0;
    z = 0;
    slck = 0;

    for(int i = 0; i < 50; i++) {
        queue_x[i] = 0;
        queue_y[i] = 0;
        queue_z[i] = 0;
        

    }

    for(int i = 0; i < 3; i++) avgs[i] = 0;
end 
else begin
//Sample every 1ms
    if (slck < 50000) slck++; //increment slow clock
    else begin
    //sample
    x = 0;
    //Create a rolling queue of angular velocity values for an average.
    for(int i = 48; i < 0 ; i--) { 
        avgs[0] += queue_x[i];
        queue_x[i + 1] = queue_x[i];
        avgs[1] += queue_y[i];
        queue_y[i + 1] = queue_y[i];
        avgs[2] += queue_z[i];
        queue_z[i + 1] = queue_z[i];
        }
    queue_x[0] = dx/rate;
    queue_y[0] = dy/rate;
    queue_z[0] = dz/rate;
    //Take average
    for(int i = 0; i < 3; i++) avgs[i]/= 50;
    //Bin the average in case of minor noise and add tilt velocity to current tilt
        x =  x + bin(avgs[0]);
        y = y + bin(avgs[1]);
        z = z + bin(avgs[2]);
         X <= x;
        Y <= y;
        Z <= z;
     slck = 0;
    end
end
end


endmodule