
module SMM0
#(
    parameter DATAWIDTH = 32,
    parameter BLOCKSIZE = DATAWIDTH * 1,
    parameter BUSWIDTH = BLOCKSIZE  * 4
)    
(
    input clk, rst,
    input signed [BUSWIDTH - 1:0] A, B,
    input load, sel,

    output reg [BUSWIDTH - 1 :0] C_out

);

    localparam BLOCK_0 = DATAWIDTH - 1, BLOCK_1 = 2 * DATAWIDTH -  1, BLOCK_2 = 3 * DATAWIDTH - 1, BLOCK_3 = 4 * DATAWIDTH - 1; 

    integer i, j;

    reg signed [DATAWIDTH - 1:0] C [3:0];
    reg signed [DATAWIDTH - 1:0] T [6:0];
    reg signed [DATAWIDTH - 1:0] S [6:0];
    reg signed [DATAWIDTH - 1:0] M [6:0];
    
    always @(posedge clk) begin
        if (rst) for (i = 0; i < 7; i = i + 1) T[i] <= 'b0;
        else if (load) begin
            T[1] <= A[BLOCK_2 -: DATAWIDTH] + A[BLOCK_3 -: DATAWIDTH];
            T[2] <= A[BLOCK_0 -: DATAWIDTH];
            T[3] <= A[BLOCK_3 -: DATAWIDTH];
            T[4] <= A[BLOCK_0 -: DATAWIDTH] + A[BLOCK_1 -: DATAWIDTH];

            if (!sel) begin 
                T[0] <= A[BLOCK_0 -: DATAWIDTH] + A[BLOCK_3 -: DATAWIDTH];
                T[5] <= A[BLOCK_2 -: DATAWIDTH] - A[BLOCK_0 -: DATAWIDTH];
                T[6] <= A[BLOCK_1 -: DATAWIDTH] - A[BLOCK_3 -: DATAWIDTH]; 
            end
        end
    end

    always @(posedge clk) begin
        if (rst) for (j = 0; j < 7; j = j + 1) S[j] <= 'b0;
        else if(load) begin
            S[1] <= B[BLOCK_0 : 0];
            S[2] <= B[BLOCK_1 : BLOCK_0 + 1] - B[BLOCK_3 : BLOCK_2 + 1];
            S[3] <= B[BLOCK_2 : BLOCK_1 + 1] - B[BLOCK_0 : 0];
            S[4] <= B[BLOCK_3 : BLOCK_2 + 1];

            if (!sel) begin
                S[0] <= B[BLOCK_0 : 0] + B[BLOCK_3 : BLOCK_2 + 1]; 
                S[5] <= B[BLOCK_0 : 0] + B[BLOCK_1 : BLOCK_0 + 1];
                S[6] <= B[BLOCK_2 : BLOCK_1 + 1] + B[BLOCK_3 : BLOCK_2 + 1];
            end
        end
    end

    always @(S or T) begin
        M[1] <= T[1] * S[1];
        M[2] <= T[2] * S[2];
        M[3] <= T[3] * S[3];
        M[4] <= T[4] * S[4];

        if (!sel) begin
            M[0] <= T[0] * S[0];
            M[5] <= T[5] * S[5];
            M[6] <= T[6] * S[6];
        end

    end
    
    

    always @(M) begin
        C[1] <= M[2] + M[4]; 
        C[2] <= M[1] + M[3]; 

        if (!sel) begin
            C[0] <= M[0] + M[3] - M[4] + M[6]; 
            C[3] <= M[0] - M[1] + M[2] + M[5]; 
        end
    end


    always @(posedge clk) begin
        if (rst) C_out <= 'b0;
        else begin
            if (!sel)   C_out <= {C[3], C[2], C[1], C[0]};
            else        C_out <= {C[1], 32'b0, C[2], 32'b0};
        end
    end
    


    // initial begin
    //     $dumpfile("logs/top_dump.vcd");
    //     $dumpvars();
    // end
    
endmodule
