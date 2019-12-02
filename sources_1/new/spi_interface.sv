`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////////////
// CREDIT: based off of the spi_interface created by Andrew Skreen from Diligent
//Purpose: This module is the main interface to the PmodGYRO, it produces
//					 slave select (SS), master out slave in (MOSI), and serial clock (SCLK)
//					 signals used in SPI communcation.  Data is read on the master in slave out
//					 (MISO) input.  SPI mode 3 is used.

//////////////////////////////////////////////////////////////////////////////////////////

// ==============================================================================
// 										  Define Module
// ==============================================================================
module spi_interface(
		input [7:0] send_data, 
		input begin_transmission,
		slave_select,
		miso,
		clk,
		rst,
		output logic [7:0] recieved_data, //Data recieved from the GYRO PMOD
		output logic end_transmission,
		mosi,
		sclk
);
// ==============================================================================
// 						     Local Variables & Parameters
// ==============================================================================


			parameter [11:0] SPI_CLK_COUNT_MAX = 12'hFFF;
            parameter [3:0]  RX_COUNT_MAX = 4'h8;
            
			logic [11:0]       spi_clk_count = 0;
			logic [7:0]        shift_register = 0;
			logic [3:0]        rx_count = 0;
			logic              sclk_buffer = 0, sclk_previous = 0;
			
            typedef enum data_type { Idle, Rx_Tx, Hold } State;
            State current_state;
   
// ==============================================================================
// 										Implementation
// ==============================================================================

			//---------------------------------------------------
			// 							  FSM
			//---------------------------------------------------
            initial begin
                current_state = Idle;
            end
			always @(posedge clk)
			begin: tx_rx_process //I can't find what this links to...
				
				begin
					if (rst)
					begin
						mosi <= 1;
						current_state <= Idle;
						recieved_data <= 0;
						shift_register <= 0;
					end
					else
						case (current_state)

						   // idle
							Idle :
								begin
									end_transmission <= 1;
									if (begin_transmission)
									begin
										current_state <= Rx_Tx;
										rx_count <= 0;
										shift_register <= send_data;
									end
								end

						   // rx_tx
							Rx_Tx :
								//send bit
								if (rx_count < RX_COUNT_MAX)
								begin
									if (sclk_previous && sclk_buffer)
										mosi <= shift_register[7];
									//recieve bit
									else if (!sclk_previous && sclk_buffer)
									begin
										shift_register[7:1] <= shift_register[6:0];
										shift_register[0] <= miso;
										rx_count <= rx_count + 1;
									end
								end
								else
								begin
									current_state <= Hold;
									end_transmission <= 1;
									recieved_data <= shift_register;
								end

						   // hold
							Hold :
								begin
									end_transmission <= 1;
									if (slave_select)
									begin
										mosi <= 1;
										current_state <= Idle;
									end
									else if (begin_transmission)
									begin
										current_state <= Rx_Tx;
										rx_count <= 0;
										shift_register <= send_data;
									end
								end
						endcase
				end
			end
			
			
			//---------------------------------------------------
			// 						Serial Clock
			//---------------------------------------------------
			always @(posedge clk)
			begin: spi_clk_generation
				
				begin
					if (rst == 1'b1)
					begin
						sclk_previous <= 1'b1;
						sclk_buffer <= 1'b1;
						spi_clk_count <= {12{1'b0}};
					end
					
					else if (current_state == Rx_Tx)
					begin
						if (spi_clk_count == SPI_CLK_COUNT_MAX)
						begin
							sclk_buffer <= (~sclk_buffer);
							spi_clk_count <= {12{1'b0}};
						end
						else
						begin
							sclk_previous <= sclk_buffer;
							spi_clk_count <= spi_clk_count + 1'b1;
						end
					end
					else
						sclk_previous <= 1'b1;
				end
			end
			
			// Assign serial clock output
			assign sclk = sclk_previous;
   
endmodule

