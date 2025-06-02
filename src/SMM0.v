
module SMM0
#(
    parameter DATAWIDTH = 32 * 4,
    parameter BLOCKSIZE = 32
)    
(
    // input clk, rst,
    input [DATAWIDTH - 1:0] A,


    output reg [DATAWIDTH - 1:0] C
);

    localparam BLOCK_0 = BLOCKSIZE - 1, BLOCK_1 = 2 * BLOCKSIZE -  1, BLOCK_2 = 3 * BLOCKSIZE - 1, BLOCK_3 = DATAWIDTH - 1; 

    reg [BLOCKSIZE - 1:0] T [7];
    
    always @(A) begin
       T[0] <= A[BLOCK_0 : 0]               + A[BLOCK_3 : BLOCK_2 + 1];
       T[1] <= A[BLOCK_2 : BLOCK_1 + 1]     + A[BLOCK_3 : BLOCK_2 + 1];
       T[2] <= A[BLOCK_0 : 0];
       T[3] <= A[BLOCK_3 : BLOCK_2 + 1];
       T[4] <= A[BLOCK_0 : 0]               + A[BLOCK_1 : BLOCK_0 +  1];
       T[5] <= A[BLOCK_2 : BLOCK_1 + 1]     - A[BLOCK_0 : 0];
       T[6] <= A[BLOCK_1 : BLOCK_0 + 1]     - A[BLOCK_3 : BLOCK_2 + 1]; 
    end

    always @(T) begin
       C <= {T[3], T[2], T[1], T[0]}; 
    end
    



    initial begin
        $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
        $dumpfile("logs/top_dump.vcd");
        $dumpvars();
        $display("[%0t] Model running...\n", $time);
    end
    
endmodule
