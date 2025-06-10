`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/09/2025 12:49:59 PM
// Design Name: 
// Module Name: Basys_Wrapper
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Basys_Wrapper(
    input clk, btnU, btnC, sw,

    output reg [15:0] led
);
    localparam DATAWIDTH = 8;

    reg [15:0] btnC_debounnce;
    reg btnC_stable;

    always @(posedge clk) begin
        btnC_debounnce = {btnC_debounnce[14:0], btnC};
        btnC_stable = btnC_debounnce[15];
    end
    
    reg [DATAWIDTH * 16 - 1:0] W_r;
    reg [DATAWIDTH * 16 - 1:0] S;

    initial begin
        W_r = {
            8'd0, 8'd0, 8'd0, 8'd2,
            8'd2, 8'd0, 8'd0, 8'd0,
            8'd0, 8'd2, 8'd0, 8'd0, 
            8'd0, 8'd0, 8'd2, 8'd0
        };
        S = {
            8'd0, 8'd0, 8'd0, 8'd1,
            8'd0, 8'd0, 8'd0, 8'd1,
            8'd0, 8'd0, 8'd0, 8'd1, 
            8'd0, 8'd0, 8'd0, 8'd1
        };

    end
    
    
    reg [DATAWIDTH - 1 : 0] C [15:0];
    wire [DATAWIDTH * 16 - 1 : 0] C_wire;

    SMM1 #(
        .DATAWIDTH(DATAWIDTH),
        .BLOCKSIZE(),
        .BUSWIDTH()
    ) SMM1_instance (
        .clk(clk),
        .rst(btnU),
        .A(W_r),
        .B(S),
        .load(btnC_stable),
        .sel(1'b1),
        .C_out(C_wire)
    );

    integer j;
    always @(posedge clk) begin
        for (j = 0; j < 16; j = j + 1) begin
            C[j] <= C_wire[j * DATAWIDTH +: DATAWIDTH];
        end
    end
    
    always @(posedge clk) begin
        led = C[0];
    end


endmodule
