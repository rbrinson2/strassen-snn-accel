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
    input clk, btnC,

    output reg [7:0] led
);

    
    SMM1 #(
        .DATAWIDTH(32),
        .BLOCKSIZE(),
        .BUSWIDTH()
    ) SMM1_instance (
        .clk(clk),
        .rst(btnC),
        .A('b0),
        .B('b0),
        .load('b1),
        .sel('b1),
        .C_out()
    );

    integer i;
    always @(posedge clk) begin
        for (i = 0; i < 8; i = i + 1) begin
            if (i % 2 == 0) led[i] <= 1'b1;
            else if (i % 2 == 1) led[i] <= 1'b0;
        end
    end


endmodule
