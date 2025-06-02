

module Top
(
    input clk,
    input rst,


    // ----------------------------------------------------- Module Inputs
    input finished,
    input [DATA_WIDTH - 1:0] A_in, A_in_1, A_in_2, B_in, B_in_1, B_in_2,

    // ----------------------------------------------------- Module OUtputs
    output reg [DATA_WIDTH - 1:0] C_out,
    output reg [2:0] A_read_en, B_read_en,
    output reg Write_en,
    output reg load_out

);

 
    
    
    // ----------------------------------------------------- Tracing
initial begin
    $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
    $dumpfile("logs/top_dump.vcd");
    $dumpvars();
    $display("[%0t] Model running...\n", $time);
end
endmodule
