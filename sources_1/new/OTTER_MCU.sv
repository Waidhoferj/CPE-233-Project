`timescale 1ns / 1ps
module OTTER_MCU (
    input CLK, RST, INTR,
    input RX,
    input [31:0] IOBUS_IN,
    output logic [31:0] IOBUS_OUT, IOBUS_ADDR,
    output logic IOBUS_WR,
    output logic TX
);

logic [31:0] pc_in, 
pc_out, 
mem_dout_1, 
mem_dout_2, reg_wd, alu_result, reg_out_1, reg_out_2, alu_a, alu_b , alu_out,
csr_reg, pc_jalr, pc_branch, pc_jal, UType, IType, SType, JType, BType, pc_4, mtvec, mepc;

logic [3:0] alu_fun;

logic [1:0] rf_wr_sel, alu_srcB;
logic [2:0] pcSource;
logic  alu_srcA, br_eq, br_lt, br_ltu, regWrite, pcWrite, memRead1, memRead2, memWrite, mie, int_taken, csr_write, interrupt;

//PC Mux
always_comb begin 
case(pcSource)
    0: pc_in = pc_4;
    1: pc_in = pc_jalr;
    2: pc_in = pc_branch;
    3: pc_in = pc_jal;
    4: pc_in = mtvec;
    5: pc_in = mepc; 
endcase
end

PC ProgramCounter(.DIN(pc_in), .CLK(CLK), .RESET(rst_new), .PC_WRITE(pcWrite), .DOUT(pc_out));

assign pc_4 = pc_out + 4;
//RAM
OTTER_mem_byte OTTER_Mem(.MEM_CLK(CLK), .MEM_ADDR1(pc_out), .MEM_READ1(memRead1), .MEM_DOUT1(mem_dout_1), .MEM_SIZE(mem_size_after), .MEM_READ2(memRead2), .MEM_DOUT2(mem_dout_2), .MEM_ADDR2(mem_addr_after), .MEM_DIN2(mem_data_after), .MEM_WRITE2(mem_we_after), .MEM_SIGN(mem_dout_1[14]),.IO_IN(IOBUS_IN), .IO_WR(IOBUS_WR));
//REG FILE MUX
always_comb begin
    case(rf_wr_sel)
    0: reg_wd = pc_4;
    1: reg_wd = csr_reg;
    2: reg_wd = mem_dout_2;
    3: reg_wd = alu_out;
    default: reg_wd = 0;
    endcase
end


RegisterFile RegFile(.ADR1(mem_dout_1[19: 15]), .CLK(CLK), .ADR2(mem_dout_1[24: 20]), .WA(mem_dout_1[11:7]), .EN(regWrite), .WD(reg_wd), .RS1(reg_out_1), .RS2(reg_out_2));

assign IOBUS_OUT = mem_data_after;

ImmediateGenerator IG(.IR(mem_dout_1), .U_Type(UType), .I_Type(IType), .S_Type(SType), .J_Type(JType),.B_Type(BType));

//ALU A MUX
always_comb begin
    case (alu_srcA)
    0: alu_a = reg_out_1;
    1: alu_a = UType;
    default alu_a = 0;
endcase
end

//ALU B MUX
always_comb begin
    case(alu_srcB)
    0: alu_b = reg_out_2;
    1: alu_b = IType;
    2: alu_b = SType;
    3: alu_b = pc_out;
    default: alu_b = 0;
    endcase
end


ALU aluComp(.A(alu_a), .B(alu_b), .ALU_FUN(alu_fun), .ALU_OUT(alu_out));

assign IOBUS_ADDR = mem_addr_after;

//CSR
CSR CSR_Comp(.CSR_MIE(mie), .CSR_MEPC(mepc), .CSR_MTVEC(mtvec), .ADDR(mem_dout_1[31:20]), .RD(csr_reg), .WD(alu_out), .PC(pc_out), .CLK(CLK), .INT_TAKEN(int_taken), .WR_EN(csr_write), .RST(rst_new));

//Branch Generator
logic [4:0] val1 = 0, val2 = 0;
always_comb begin
if(reg_out_1 == reg_out_2) br_eq = 1;
else br_eq = 0;
if($signed(reg_out_1) < $signed(reg_out_2)) br_lt = 1;
else br_lt = 0;
if(reg_out_1 < reg_out_2) br_ltu = 1;
else br_ltu = 0;
end

//DECODER
CUDecoder Decoder( .BR_EQ(br_eq), .BR_LT(br_lt), .BR_LTU(br_ltu), .CU_OPCODE(mem_dout_1[6:0]), .FUNC3(mem_dout_1[14:12]), .FUNC7(mem_dout_1[31:25]), .ALU_FUN(alu_fun), .ALU_SRCA(alu_srcA), .ALU_SRCB(alu_srcB), .PC_SOURCE(pcSource), .RF_WR_SEL(rf_wr_sel), .INT_TAKEN(int_taken));

//Interrupt Mux
assign interrupt = mie && INTR;

CU_FSM StateController(.CLK(CLK), .INTR(interrupt), .RST(rst_new), .CU_OPCODE(mem_dout_1[6:0]), .PC_WRITE(pcWrite), .REG_WRITE(regWrite), .MEM_WRITE(memWrite), .MEM_READ1(memRead1), .MEM_READ2(memRead2), .CSR_WRITE(csr_write), .INT_TAKEN(int_taken), .FUNC3(mem_dout_1[14:12]));

//Target Generator
assign pc_branch = pc_out + BType;
assign pc_jal = pc_out + JType;
assign pc_jalr = reg_out_1 + IType;

//PROGRAMMER
programmer #(.CLK_RATE(50), .BAUD(115200), .IB_TIMEOUT(200),
.WAIT_TIMEOUT(500))
programmer(.clk(CLK), .rst(RST), .srx(RX), .stx(TX),
.mcu_reset(prog_rst), .ram_addr(prog_ram_addr),
.ram_data(prog_ram_data), .ram_we(prg_we));

logic [31:0] prog_ram_data, mem_data_after;
logic [31:0] prog_ram_addr, mem_addr_after;
logic [1:0] mem_size_after;
logic prg_we, prog_rst, mem_we_after, rst_new;

assign mem_addr_after = prg_we ? prog_ram_addr: alu_out;
assign mem_data_after = prg_we ? prog_ram_data : reg_out_2;
assign mem_size_after = prg_we ? 2'b10 : mem_dout_1[13:12];
assign mem_we_after = prg_we | memWrite;
assign rst_new = prog_rst | RST;


endmodule