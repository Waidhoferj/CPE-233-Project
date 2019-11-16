module RegisterFileSim ();
    logic CLK, EN;
	logic [31:0] WD, RS1, RS2;
	logic [4:0] ADR1, ADR2, WA;
    always
    begin
        CLK = 0; 
        #5;
        CLK = 1;
        #5;
    end
    
    RegisterFile instRegisterFile(.*);

    initial begin
        //INSTATIATE
        //----------------------------
        EN = 1;
        WD = 25;
        ADR1 = 5;
        ADR2 = 5;
        WA = 5;

        //TESTS
        //-----------------------------
        //Register zero doesn't change
        #10;
        WA = 0;
        EN = 1;
        ADR1 = 0;
        #10;
        if(RS1 != 0) $display("The zero register can be written to.");
        //See that accessible locations behave correctly
        #5;
        for(int i = 1; i < 8; i++) begin
            WD = i*20;
            WA = i * 4;
            #5;
            ADR1 = i * 4;
            ADR2 = i * 4;
            #5;
            if(RS1 != WD || RS2 != WD) $display("Test %0d failed - RS1: %0d RS2: %0d WD:%0d ", i, RS1, RS2, WD);
            #5;
        end

        
    end
endmodule