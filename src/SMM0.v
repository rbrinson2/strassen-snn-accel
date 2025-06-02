
module SMM0
#(
    parameter DATAWIDTH = 32 * 4,
    parameter BLOCKSIZE = 32
)    
(
    input clk, rst,
    input [DATAWIDTH - 1:0] A, B,

    output reg [DATAWIDTH - 1 :0] C_out

);

    localparam BLOCK_0 = BLOCKSIZE - 1, BLOCK_1 = 2 * BLOCKSIZE -  1, BLOCK_2 = 3 * BLOCKSIZE - 1, BLOCK_3 = DATAWIDTH - 1; 

    integer i;

    reg [BLOCKSIZE - 1:0] C [4];
    reg [BLOCKSIZE - 1:0] T [7];
    reg [BLOCKSIZE - 1:0] S [7];
    reg [BLOCKSIZE - 1:0] M [7];
    
    always @(A) begin
       T[0] <= A[BLOCK_0 : 0] + A[BLOCK_3 : BLOCK_2 + 1];
       T[1] <= A[BLOCK_2 : BLOCK_1 + 1] + A[BLOCK_3 : BLOCK_2 + 1];
       T[2] <= A[BLOCK_0 : 0];
       T[3] <= A[BLOCK_3 : BLOCK_2 + 1];
       T[4] <= A[BLOCK_0 : 0] + A[BLOCK_1 : BLOCK_0 +  1];
       T[5] <= A[BLOCK_2 : BLOCK_1 + 1] - A[BLOCK_0 : 0];
       T[6] <= A[BLOCK_1 : BLOCK_0 + 1] - A[BLOCK_3 : BLOCK_2 + 1]; 
    end

    always @(B) begin
       S[0] <= B[BLOCK_0 : 0] + B[BLOCK_3 : BLOCK_2 + 1]; 
       S[1] <= B[BLOCK_0 : 0];
       S[2] <= B[BLOCK_1 : BLOCK_0 + 1] - B[BLOCK_3 : BLOCK_2 + 1];
       S[3] <= B[BLOCK_2 : BLOCK_1 + 1] - B[BLOCK_0 : 0];
       S[4] <= B[BLOCK_3 : BLOCK_2 + 1];
       S[5] <= B[BLOCK_0 : 0] + B[BLOCK_1 : BLOCK_0 + 1];
       S[6] <= B[BLOCK_2 : BLOCK_1 + 1] + B[BLOCK_3 : BLOCK_2 + 1];
    end

    always @(S or T) begin
        for (i = 0; i < 7; i = i + 1) begin
            M[i] <= T[i] * S[i];
        end
    end
    
    

    always @(M) begin
       C[0] <= M[0] + M[3] - M[4] + M[6]; 
       C[1] <= M[2] + M[4]; 
       C[2] <= M[1] + M[3]; 
       C[3] <= M[0] - M[1] + M[3] + M[5]; 
    end


    always @(posedge clk or posedge rst) begin
        if (rst) C_out <= 'b0;
        else begin
            C_out <= {C[0], C[1], C[2], C[3]};
        end
    end
    


    initial begin
        $dumpfile("logs/top_dump.vcd");
        $dumpvars();
    end
    
endmodule
