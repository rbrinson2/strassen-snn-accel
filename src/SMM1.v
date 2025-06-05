
module SMM1
#(
    parameter DATAWIDTH = 32,
    parameter BLOCKSIZE = DATAWIDTH * 4,
    parameter BUSWIDTH = BLOCKSIZE * 4
)    
(
    // Inputs ----------------------------------------------------------------------------------- //
    input clk, rst,
    input signed [BUSWIDTH - 1:0] A, B,
    input load, sel,

    // Outputs ---------------------------------------------------------------------------------- //
    output reg [BUSWIDTH - 1 :0] C_out

);
    // Function ----------------------------------------------------------------- Matrix Addition //
    function automatic [BLOCKSIZE - 1 : 0] mat_add (input [BLOCKSIZE - 1 : 0] a, b);
        integer i;
        for (i = 0; i < BLOCKSIZE / DATAWIDTH; i = i + 1) begin
            mat_add[i * DATAWIDTH +: DATAWIDTH] = a[i * DATAWIDTH +: DATAWIDTH] + b[i * DATAWIDTH +: DATAWIDTH];
        end
    endfunction
    
    // Function -------------------------------------------------------------- Matrix Subtraction //
    function automatic [BLOCKSIZE - 1 : 0] mat_sub (input [BLOCKSIZE - 1 : 0] a, b);
        integer i;
        for (i = 0; i < BLOCKSIZE / DATAWIDTH; i = i + 1) begin
            mat_sub[i * DATAWIDTH +: DATAWIDTH] = a[i * DATAWIDTH +: DATAWIDTH] - b[i * DATAWIDTH +: DATAWIDTH];
        end
    endfunction   

    // Local Parameters ------------------------------------------------------------------------- //
    localparam BLOCKWIDTH = DATAWIDTH * 2;
    localparam BLOCK_00_UPPER = BLOCKWIDTH * 3 - 1, BLOCK_01_UPPER = BLOCKWIDTH * 4 -  1, BLOCK_10_UPPER = BLOCKWIDTH * 7 - 1, BLOCK_11_UPPER = BLOCKWIDTH * 8 - 1; 
    localparam BLOCK_00_LOWER = BLOCKWIDTH * 1 - 1, BLOCK_01_LOWER = BLOCKWIDTH * 2 -  1, BLOCK_10_LOWER = BLOCKWIDTH * 5 - 1, BLOCK_11_LOWER = BLOCKWIDTH * 6 - 1; 

    // Local Variables -------------------------------------------------------------------------- //
    wire [BLOCKSIZE - 1:0] A_00;
    wire [BLOCKSIZE - 1:0] A_01;
    wire [BLOCKSIZE - 1:0] A_10;
    wire [BLOCKSIZE - 1:0] A_11;

    wire [BLOCKSIZE - 1:0] B_00;
    wire [BLOCKSIZE - 1:0] B_01;
    wire [BLOCKSIZE - 1:0] B_10;
    wire [BLOCKSIZE - 1:0] B_11;

    wire [BLOCKSIZE - 1:0] M_wire [7:0];

    reg signed [BLOCKSIZE - 1:0] C [3:0];
    reg signed [BLOCKSIZE - 1:0] T [6:0];
    reg signed [BLOCKSIZE - 1:0] S [6:0];
    reg signed [BLOCKSIZE - 1:0] M [7];

    integer i, j;

    // Wire Assignemeents ----------------------------------------------------------------------- //
    assign A_00 = {A[BLOCK_00_UPPER -: BLOCKWIDTH], A[BLOCK_00_LOWER -: BLOCKWIDTH]};
    assign A_01 = {A[BLOCK_01_UPPER -: BLOCKWIDTH], A[BLOCK_01_LOWER -: BLOCKWIDTH]};
    assign A_10 = {A[BLOCK_10_UPPER -: BLOCKWIDTH], A[BLOCK_10_LOWER -: BLOCKWIDTH]};
    assign A_11 = {A[BLOCK_11_UPPER -: BLOCKWIDTH], A[BLOCK_11_LOWER -: BLOCKWIDTH]};
    
    assign B_00 = {B[BLOCK_00_UPPER -: BLOCKWIDTH], B[BLOCK_00_LOWER -: BLOCKWIDTH]};
    assign B_01 = {B[BLOCK_01_UPPER -: BLOCKWIDTH], B[BLOCK_01_LOWER -: BLOCKWIDTH]};
    assign B_10 = {B[BLOCK_10_UPPER -: BLOCKWIDTH], B[BLOCK_10_LOWER -: BLOCKWIDTH]};
    assign B_11 = {B[BLOCK_11_UPPER -: BLOCKWIDTH], B[BLOCK_11_LOWER -: BLOCKWIDTH]};

    
    // T Load and Addition ---------------------------------------------------------------------- //
    always @(posedge clk) begin
        if (rst) for (i = 0; i < 7; i = i + 1) T[i] <= 'b0;
        else if (load) begin
            T[1] <= mat_add(A_10, A_11);
            T[2] <= A_00;
            T[3] <= A_11;
            T[4] <= mat_add(A_00, A_01);

            if (!sel) begin 
                T[0] <= mat_add(A_00, A_11);
                T[5] <= mat_sub(A_10, A_00);
                T[6] <= mat_sub(A_01, A_11); 
            end
        end
    end

    // S Load and Addition ---------------------------------------------------------------------- //
    always @(posedge clk) begin
        if (rst) for (j = 0; j < 7; j = j + 1) S[j] <= 'b0;
        else if(load) begin
            S[1] <= B_00;
            S[2] <= mat_sub(B_01, B_11);
            S[3] <= mat_sub(B_10, B_00);
            S[4] <= B_11;

            if (!sel) begin
                S[0] <= mat_add(B_00, B_11); 
                S[5] <= mat_add(B_00, B_01);
                S[6] <= mat_add(B_10, B_11);
            end
        end
    end

    // M Multiplications ------------------------------------------------------------------------ //
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

    always begin
        M[0] <= M_wire[0];
        M[1] <= M_wire[1];
        M[2] <= M_wire[2];
        M[3] <= M_wire[3];
    end    

    // C Additions ------------------------------------------------------------------------------ //
    always @(M) begin
        C[1] <= mat_add(M[2], M[4]); 
        C[2] <= mat_add(M[1], M[3]); 

        if (!sel) begin
            C[0] <= mat_add(mat_sub(mat_add(M[0], M[3]), M[4]), M[6]); 
            C[3] <= mat_add(mat_add(mat_sub(M[0], M[1]), M[2]), M[5]); 
        end
    end

    // Output Assignment ------------------------------------------------------------------------ //
    always @(posedge clk) begin
        if (rst) C_out <= 'b0;
        else begin
            if (!sel) begin
                {C_out[BLOCK_00_UPPER -: BLOCKWIDTH], C_out[BLOCK_00_LOWER -: BLOCKWIDTH]} <= C[0];
                {C_out[BLOCK_01_UPPER -: BLOCKWIDTH], C_out[BLOCK_01_LOWER -: BLOCKWIDTH]} <= C[1];
                {C_out[BLOCK_10_UPPER -: BLOCKWIDTH], C_out[BLOCK_10_LOWER -: BLOCKWIDTH]} <= C[2];
                {C_out[BLOCK_11_UPPER -: BLOCKWIDTH], C_out[BLOCK_11_LOWER -: BLOCKWIDTH]} <= C[3];
            end
            else        
                {C_out[BLOCK_00_UPPER -: BLOCKWIDTH], C_out[BLOCK_00_LOWER -: BLOCKWIDTH]} <= C[1];
                {C_out[BLOCK_01_UPPER -: BLOCKWIDTH], C_out[BLOCK_01_LOWER -: BLOCKWIDTH]} <= 'b0;
                {C_out[BLOCK_10_UPPER -: BLOCKWIDTH], C_out[BLOCK_10_LOWER -: BLOCKWIDTH]} <= C[2];
                {C_out[BLOCK_11_UPPER -: BLOCKWIDTH], C_out[BLOCK_11_LOWER -: BLOCKWIDTH]} <= 'b0;
        end
    end
    


    // initial begin
    //     $dumpfile("logs/top_dump.vcd");
    //     $dumpvars();
    // end
    
endmodule
