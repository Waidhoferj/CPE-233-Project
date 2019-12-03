`timescale 1ns / 1ps
// CREDIT: based off of the master_interface created by Andrew Skreen from Diligent
//Module: GyroFsm
//Purpose: Gets angular velocity data from the GYRO PMOD

// ==============================================================================
// 								Define Module
// ==============================================================================
module GyroFsm (
		input end_transmission,
			clk,
			rst,
			start,
   		input [7:0] recieved_data,
   		output logic begin_transmission,
			slave_select,
		output logic [7:0] send_data,
		output logic [15:0] x_axis_data,
			y_axis_data,
			z_axis_data
);
//===============================================================================
//								FSM States
//===============================================================================

typedef enum { 
	Idle,
	Setup,
	Temp,
	Run,
	Hold,
	Wait_SS,
	Wait_Run
 } GyroState;
GyroState current_state, previous_state;

//===============================================================================
//								Addresses & Parameters
//===============================================================================

parameter [15:0] SETUP_GYRO = 16'h0F20;
// address of X_AXIS (0x28) with read and multiple bytes selected (0xC0)
parameter [7:0]  DATA_READ_BEGIN = 8'hE8;
// address of TEMP (0x26) with read selected (0x80)
parameter [7:0]  TEMP_READ_BEGIN = 8'hA6;
parameter        MAX_BYTE_COUNT = 6;
parameter [11:0] SS_COUNT_MAX = 12'hFFF;
parameter [23:0] COUNT_WAIT_MAX = 24'h7FFFFF;		//X"000FFF";

//===============================================================================
//								Internal Variables
//===============================================================================

logic [2:0] byte_count;
logic [11:0] ss_count;
logic [15:0] axis_data [0:2];
logic [23:0] count_wait;
//TODO: actually hook up temp data to something
logic [7:0] temp_data;

// ==============================================================================
// 							  		   Implementation
// ============================================================================== 

initial begin
	current_state = Idle;
	previous_state = Idle;
end

always @(posedge clk)
begin: spi_interface
//TODO: Insert spi interface
begin
	if(rst) begin
		slave_select <= 1;
		byte_count <= 0;
		count_wait <= 0;
		for(int i = 0; i < 3; i++) axis_data[i] = 0;
		x_axis_data <= 0;
		y_axis_data <= 0;
		z_axis_data <= 0;
		ss_count = 0;
		current_state = Idle;
		previous_state = Idle;
	end
	else
	case(current_state) 
		Idle: begin
				slave_select <= 1;
				if (start)
				begin
					byte_count <= 0;
					for(int i = 0; i < 3; i++) axis_data[i] = 0;
					current_state = Setup;
				end
			end
		Setup: if (byte_count < 2)
			begin
				if(byte_count == 0) begin
						send_data <= SETUP_GYRO[7:0];
				end
				else begin
						send_data <= SETUP_GYRO[15:8];
				end
				slave_select <= 1;
				byte_count = byte_count + 1;
				begin_transmission <= 1;
				previous_state = Setup;
				current_state = Hold;
			end
			else
			begin
				byte_count <= 0;
				previous_state <= Setup;
				current_state <= Wait_SS;
			end

		Temp: 
		if (byte_count == 0)
			begin
				slave_select <= 0;
				send_data = TEMP_READ_BEGIN;
				byte_count = byte_count + 1;
				begin_transmission <= 1;
				previous_state = Temp;
				current_state = Hold;
			end
		else if (byte_count == 1)
			begin
				send_data <= 8'h00;
				byte_count = byte_count + 1;
				begin_transmission <= 1;
				previous_state = Temp;
				current_state = Hold;
			end
		else
			begin
				byte_count = 0;
				previous_state = Temp;
				current_state = Wait_SS;
			end
	Run:
	if (byte_count == 0)
		begin
			slave_select <= 1;
			send_data <= DATA_READ_BEGIN;
			byte_count = byte_count + 1;
			begin_transmission <= 1;
			previous_state <= Run;
			current_state <= Hold;
		end
		else if (byte_count <= 6)
		begin
			send_data <= 8'h00;
			byte_count <= byte_count + 1;
			begin_transmission <= 1;
			previous_state <= Run;
			current_state <= Hold;
		end
		else
		begin
			byte_count <= 0;
			x_axis_data <= axis_data[0];
			y_axis_data <= axis_data[1];
			z_axis_data <= axis_data[2];
			previous_state <= Run;
			current_state <= Wait_SS;
		end
	Hold: 
	begin
		begin_transmission <= 1;
		if (end_transmission)
		begin
			if (previous_state == Temp && byte_count != 1)
				temp_data <= recieved_data; //Do we have temp data?
			else if (previous_state == Run && byte_count != 1) begin
					case (byte_count)
					//Maybe a struct...
							2 : axis_data[0][7:0] <= recieved_data; //Axis data assigned to internal variables
							3 : axis_data[0][15:8] <= recieved_data;
							4 : axis_data[1][7:0] <= recieved_data;
							5 : axis_data[1][15:8] <= recieved_data;
							6 : axis_data[2][7:0] <= recieved_data;
							7 : axis_data[2][15:8] <= recieved_data;
							default : ;
					endcase
			end
			current_state <= previous_state;
		end
	end

	Wait_SS:
	begin
		begin_transmission <= 1;
		if (ss_count == SS_COUNT_MAX)
		begin
			slave_select <= 1;
			ss_count <= 0;
			current_state <= Wait_Run;
		end
		else
			ss_count <= ss_count + 1;
	end

	Wait_Run:
	begin
		begin_transmission <= 1'b0;
		if (!start)
			current_state <= Idle;
		if (count_wait == COUNT_WAIT_MAX)
		begin
			count_wait <= 0;
			if (previous_state == Temp)
				current_state <= Run;
			else
				current_state <= Temp;
		end
		else
			count_wait <= count_wait + 1;
	end


	endcase
end
endmodule