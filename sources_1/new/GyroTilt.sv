`timescale 1ns / 1ps
// Module: GyroTilt
//Purpose: Converts angular velocity data into tilt angle of gyro

//Inputs
    // dx - dz: the current angular velocity from GyroFsm
    // RST: will set each dimension to 0 degree tilt
//Outputs
    // X-Z: the current tilt in each dimension


module GyroTilt (
    input [15:0] dx, dy, dz, //Angular velocity
    input RST,
    input CLK,
    output logic [15:0] X, Y, Z
);

//Internal Variables
//=============================================
logic [15:0] x, y, z;

//Functionality
//=============================================
initial begin
    x = 0;
    y = 0;
    z = 0;
    X <= 0;
    Y <= 0;
    Z <= 0;

end

always_ff @(posedge CLK) begin
if(RST)begin
    X <= 0;
    Y <= 0;
    Z <= 0;
end 
else begin
    parameter rate = 10; //Amount of time passed (clock speed)
    x = x + dx*rate;
    y = y + dy*rate;
    z = z + dz*rate;
    X <= x;
    Y <= y;
    Z <= z;
end
end
endmodule