module GyroTilt (
    input [15:0] dx, dy dz, //Angular velocity
    input RST,
    output logic [15:0] X, Y
);
if(RST){
    X = 0;
Y = 0;
}else {
x + dxt;
logic rate = 10;
logic x = x + dx*rate;
logic y = y + dy*rate
X = x;
Y = y;
}
endmodule