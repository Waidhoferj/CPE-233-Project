`timescale 1ns / 1ps
// CREDIT: based off of the PmodGYRO_Demo created by Andrew Skreen from Diligent

// ==============================================================================
// 										  Define Module
// ==============================================================================
module GyroTop (
    input CLK, RST,
    inout [3:0] JA,
    //Should the outputs below exist in this form?
    output logic [15:0] X, Y, Z
);

// ==============================================================================
// 							            Variables
// ==============================================================================   
   logic         begin_transmission;
   logic         end_transmission;
   logic [7:0]   send_data;
   logic [7:0]   recieved_data;
   logic [7:0]   temp_data;
   logic [15:0]  x_axis_data;
   logic [15:0]  y_axis_data;
   logic [15:0]  z_axis_data;
   logic         slave_select;


// ==============================================================================
// 							  		   Implementation
// ============================================================================== 

GyroFsm GFSM (
						.begin_transmission(begin_transmission),
						.end_transmission(end_transmission),
						.send_data(send_data),
						.recieved_data(recieved_data),
						.clk(CLK),
						.rst(RST),
						.slave_select(slave_select),
						.start(1),
						.x_axis_data(x_axis_data),
						.y_axis_data(y_axis_data),
						.z_axis_data(z_axis_data)
			);

spi_interface SPII(
						.begin_transmission(begin_transmission),
						.slave_select(slave_select),
						.send_data(send_data),
						.recieved_data(recieved_data),
						.miso(JA[2]),
						.clk(CLK),
						.rst(RST),
						.end_transmission(end_transmission),
						.mosi(JA[1]),
						.sclk(JA[3])
			);

GyroTilt GT(
						.dx(x_axis_data),
						.dy(y_axis_data),
						.dz(z_axis_data),
						.RST(RST),
						.CLK(CLK),
						.X(X),
						.Y(Y),
						.Z(Z)
			);

    assign JA[0] = slave_select;
    
endmodule
