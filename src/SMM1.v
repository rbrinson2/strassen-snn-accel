



module SMM1
#(
    parameter DATAWIDTH = 32,
    parameter BLOCKSIZE = DATAWIDTH * 4,
    parameter BUSWIDTH = BLOCKSIZE * 4
)    
(
    input clk, rst,
    input signed [BUSWIDTH - 1:0] A, B,
    input load, sel,

    output reg [BUSWIDTH - 1 :0] C_out

);

    function automatic [BLOCKSIZE - 1 : 0] mat_add (input [BLOCKSIZE - 1 : 0] a, b);
        integer i;
        for (i = 0; i < BLOCKSIZE / DATAWIDTH; i = i + 1) begin
            mat_add[i * DATAWIDTH +: DATAWIDTH] = a[i * DATAWIDTH +: DATAWIDTH] + b[i * DATAWIDTH +: DATAWIDTH];
        end
    endfunction
    
    function automatic [BLOCKSIZE - 1 : 0] mat_sub (input [BLOCKSIZE - 1 : 0] a, b);
        integer i;
        for (i = 0; i < BLOCKSIZE / DATAWIDTH; i = i + 1) begin
            mat_sub[i * DATAWIDTH +: DATAWIDTH] = a[i * DATAWIDTH +: DATAWIDTH] - b[i * DATAWIDTH +: DATAWIDTH];
        end
    endfunction   

    localparam BLOCK_0 = BLOCKSIZE - 1, BLOCK_1 = 2 * BLOCKSIZE -  1, BLOCK_2 = 3 * BLOCKSIZE - 1, BLOCK_3 = 4 * BLOCKSIZE - 1; 

    integer i, j;

    reg signed [BLOCKSIZE - 1:0] C [4];
    reg signed [BLOCKSIZE - 1:0] T [7];
    reg signed [BLOCKSIZE - 1:0] S [7];
    reg signed [BLOCKSIZE - 1:0] M [7];

    
    always @(posedge clk) begin
        if (rst) for (i = 0; i < 7; i = i + 1) T[i] <= 'b0;
        else if (load) begin
            T[1] <= mat_add(A[BLOCK_2 -: BLOCKSIZE], A[BLOCK_3 -: BLOCKSIZE]);
            T[2] <= A[BLOCK_0 -: BLOCKSIZE];
            T[3] <= A[BLOCK_3 -: BLOCKSIZE];
            T[4] <= mat_add(A[BLOCK_0 -: BLOCKSIZE], A[BLOCK_1 -: BLOCKSIZE]);

            if (!sel) begin 
                T[0] <= mat_add(A[BLOCK_0 -: BLOCKSIZE], A[BLOCK_3 -: BLOCKSIZE]);
                T[5] <= mat_sub(A[BLOCK_2 -: BLOCKSIZE], A[BLOCK_0 -: BLOCKSIZE]);
                T[6] <= mat_sub(A[BLOCK_1 -: BLOCKSIZE], A[BLOCK_3 -: BLOCKSIZE]); 
            end
        end
    end

    always @(posedge clk) begin
        if (rst) for (j = 0; j < 7; j = j + 1) S[j] <= 'b0;
        else if(load) begin
            S[1] <= B[BLOCK_0 : 0];
            S[2] <= mat_sub(B[BLOCK_1 -: BLOCKSIZE], B[BLOCK_3 -: BLOCKSIZE]);
            S[3] <= mat_sub(B[BLOCK_2 -: BLOCKSIZE], B[BLOCK_0 -: BLOCKSIZE]);
            S[4] <= B[BLOCK_3 -: BLOCKSIZE];

            if (!sel) begin
                S[0] <= mat_add(B[BLOCK_0 -: BLOCKSIZE], B[BLOCK_3 -: BLOCKSIZE]); 
                S[5] <= mat_add(B[BLOCK_0 -: BLOCKSIZE], B[BLOCK_1 -: BLOCKSIZE]);
                S[6] <= mat_add(B[BLOCK_2 -: BLOCKSIZE], B[BLOCK_3 -: BLOCKSIZE]);
            end
        end
    end

    genvar i_gen;
    generate
        for (i_gen = 0; i_gen < 7; i_gen = i_gen + 1) begin : multiply
            SMM0 #(
                .DATAWIDTH(),
                .BLOCKSIZE(),
                .BUSWIDTH()
            ) SMM0_instance (
                .clk(clk),
                .rst(rst),
                .A(T[i_gen]),
                .B(S[i_gen]),
                .load(load),
                .sel(sel),
                .C_out(M[i_gen])
            );
        end
    endgenerate
    

    always @(M) begin
        C[1] <= mat_add(M[2], M[4]); 
        C[2] <= mat_add(M[1], M[3]); 

        if (!sel) begin
            C[0] <= mat_sub(mat_add(M[0],M[3]), mat_add(M[4], M[6])); 
            C[3] <= mat_add(mat_sub(M[0], M[1]), mat_add(M[2], M[5])); 
        end
    end


    always @(posedge clk) begin
        if (rst) C_out <= 'b0;
        else begin
            if (!sel)   C_out <= {C[0], C[1], C[2], C[3]};
            else        C_out <= {C[1], 128'b0, C[2], 128'b0};
        end
    end
    


    initial begin
        $dumpfile("logs/top_dump.vcd");
        $dumpvars();
    end
    
endmodule
