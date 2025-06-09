
module SMM0
#(
    parameter DATAWIDTH = 32
)    
(
    input clk, rst,
    input signed [BUSWIDTH - 1:0] A, B,
    input load, sel,

    output reg [BUSWIDTH - 1 :0] C_out

);
    localparam BLOCKSIZE = DATAWIDTH * 1;
    localparam BUSWIDTH = BLOCKSIZE  * 4;
    localparam BLOCK_00 = DATAWIDTH - 1, BLOCK_01 = DATAWIDTH * 2 - 1, BLOCK_10 = DATAWIDTH * 3 - 1, BLOCK_11 = DATAWIDTH * 4 - 1; 

    wire [BLOCKSIZE - 1:0] A_00;
    wire [BLOCKSIZE - 1:0] A_01;
    wire [BLOCKSIZE - 1:0] A_10;
    wire [BLOCKSIZE - 1:0] A_11;

    wire [BLOCKSIZE - 1:0] B_00;
    wire [BLOCKSIZE - 1:0] B_01;
    wire [BLOCKSIZE - 1:0] B_10;
    wire [BLOCKSIZE - 1:0] B_11;

    assign A_00 = A[BLOCK_00 -: DATAWIDTH];
    assign A_01 = A[BLOCK_01 -: DATAWIDTH];
    assign A_10 = A[BLOCK_10 -: DATAWIDTH];
    assign A_11 = A[BLOCK_11 -: DATAWIDTH];
    
    assign B_00 = B[BLOCK_00 -: DATAWIDTH];
    assign B_01 = sel ? B[BLOCK_00 -: DATAWIDTH] 
                : B[BLOCK_01 -: DATAWIDTH];
    assign B_10 = B[BLOCK_10 -: DATAWIDTH];
    assign B_11 = sel ? B[BLOCK_10 -: DATAWIDTH] 
                        : B[BLOCK_11 -: DATAWIDTH];

    integer i, j;

    reg signed [DATAWIDTH - 1:0] C [3:0];
    reg signed [DATAWIDTH - 1:0] T [6:0];
    reg signed [DATAWIDTH - 1:0] S [6:0];
    reg signed [DATAWIDTH - 1:0] M [6:0];
    
    always @(posedge clk) begin
        if (rst) for (i = 0; i < 7; i = i + 1) T[i] <= 'b0;
        else if (load) begin
            T[0] <= 'b0;
            T[1] <= A_10 + A_11;
            T[2] <= A_00;
            T[3] <= A_11;
            T[4] <= A_00 + A_01;
            T[5] <= 'b0;
            T[6] <= 'b0;

            if (!sel) begin 
                T[0] <= A_00 + A_11;
                T[5] <= A_10 - A_00;
                T[6] <= A_01 - A_11; 
            end
        end
    end

    always @(posedge clk) begin
        if (rst) for (j = 0; j < 7; j = j + 1) S[j] <= 'b0;
        else if(load) begin
            S[0] <= 'b0;
            S[1] <= B_00;
            S[2] <= B_01 - B_11;
            S[3] <= B_10 - B_00;
            S[4] <= B_11;
            S[5] <= 'b0;
            S[6] <= 'b0;

            if (!sel) begin
                S[0] <= B_00 + B_11; 
                S[5] <= B_00 + B_01;
                S[6] <= B_10 + B_11;
            end
        end
    end

    always @(S or T) begin
        M[0] <= 'b0;
        M[1] <= T[1] * S[1];
        M[2] <= T[2] * S[2];
        M[3] <= T[3] * S[3];
        M[4] <= T[4] * S[4];
        M[5] <= 'b0;
        M[6] <= 'b0;

        if (!sel) begin
            M[0] <= T[0] * S[0];
            M[5] <= T[5] * S[5];
            M[6] <= T[6] * S[6];
        end

    end
    
    

    always @(M) begin
        C[0] <= 'b0;
        C[1] <= M[2] + M[4]; 
        C[2] <= M[1] + M[3]; 
        C[3] <= 'b0;

        if (!sel) begin
            C[0] <= M[0] + M[3] - M[4] + M[6]; 
            C[3] <= M[0] - M[1] + M[2] + M[5]; 
        end
    end


    always @(posedge clk) begin
        if (rst) C_out <= 'b0;
        else begin
            if (!sel)   C_out <= {C[3], C[2], C[1], C[0]};
            else        C_out <= {C[3], C[2], C[0], C[1]};
        end
    end
    


    // initial begin
    //     $dumpfile("logs/top_dump.vcd");
    //     $dumpvars();
    // end
    
endmodule
