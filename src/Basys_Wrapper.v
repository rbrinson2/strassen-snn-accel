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
    localparam DATAWIDTH = 32;
    
    localparam [DATAWIDTH * 16 - 1:0] A = {
        32'h16,32'h15,32'h14,32'h13,
        32'h12,32'h11,32'h10,32'h9,
        32'h8,32'h7,32'h6,32'h5,
        32'h4,32'h3,32'h2,32'h1
    };
    localparam [DATAWIDTH * 16 - 1:0] B = {
        32'h1,32'h2,32'h3,32'h4,
        32'h5,32'h6,32'h7,32'h8,
        32'h9,32'h10,32'h11,32'h12,
        32'h13,32'h14,32'h15,32'h16
    };
    
    reg [DATAWIDTH * 16 - 1:0] C;
    wire [DATAWIDTH * 16 - 1:0] C_wire;

    SMM1 #(
        .DATAWIDTH(DATAWIDTH),
        .BLOCKSIZE(),
        .BUSWIDTH()
    ) SMM1_instance (
        .clk(clk),
        .rst(btnU),
        .A(A),
        .B(B),
        .load(btnC),
        .sel(sw),
        .C_out(C_wire)
    );

    always @(C) C = C_wire;

    integer i;
    always @(posedge clk) begin
        for (i = 0; i < 16; i = i + 1) begin
            if (C[i * DATAWIDTH +: DATAWIDTH] > 32'd250) led[i] <= 1'b1;
            else led[i] <= 1'b0;
        end
    end


endmodule
