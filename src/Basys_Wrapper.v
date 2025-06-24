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

    reg [15:0] btnC_debounnce;
    reg btnC_stable;

    always @(posedge clk) begin
        btnC_debounnce = {btnC_debounnce[14:0], btnC};
        btnC_stable = btnC_debounnce[15];
    end
    
    reg [DATAWIDTH * 16 - 1:0] W_r [8:0];
    reg [DATAWIDTH * 16 - 1:0] S [2:0];

    initial begin
        
        W_r[0] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[1] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[2] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[3] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[4] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[5] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[6] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[7] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        W_r[8] = {
            32'd1, 32'd2, 32'd3, 32'd4,
            32'd5, 32'd6, 32'd7, 32'd8,
            32'd9, 32'd10, 32'd11, 32'd12, 
            32'd13,32'd14, 32'd15, 32'd16
        };
        S[0] = {
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1, 
            32'd1, 32'd1, 32'd1, 32'd1
        };
        S[1] = {
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1, 
            32'd1, 32'd1, 32'd1, 32'd1
        };
        S[2] = {
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1,
            32'd1, 32'd1, 32'd1, 32'd1, 
            32'd1, 32'd1, 32'd1, 32'd1
        };

    end
    
    
    reg [DATAWIDTH - 1 : 0] C_0 [15:0];
    reg [DATAWIDTH - 1 : 0] C_1 [15:0];
    reg [DATAWIDTH - 1 : 0] C_2 [15:0];
    reg [DATAWIDTH * 16 - 1 : 0] C_wire_0;
    reg [DATAWIDTH * 16 - 1 : 0] C_wire_1;
    reg [DATAWIDTH * 16 - 1 : 0] C_wire_2;
    wire [DATAWIDTH * 16 - 1 : 0] Add_wire [8:0];

    genvar i;
    generate
        for (i = 0; i < 9; i = i + 1) begin : SMM_inst
            SMM1 #(
                .DATAWIDTH(DATAWIDTH),
                .BLOCKSIZE(),
                .BUSWIDTH()
            ) SMM1_instance (
                .clk(clk),
                .rst(btnU),
                .A(W_r[i]),
                .B(S[i % 3]),
                .load(btnC_stable),
                .sel(sw),
                .C_out(Add_wire[i])
            );
        end
    endgenerate

    always @(posedge clk) begin
        C_wire_0 <= Add_wire[0] + Add_wire[1] + Add_wire[2];
        C_wire_1 <= Add_wire[3] + Add_wire[4] + Add_wire[5];
        C_wire_2 <= Add_wire[6] + Add_wire[7] + Add_wire[8];
    end
    integer j;
    always @(posedge clk) begin
        for (j = 0; j < 16; j = j + 1) begin
            C_0[j] <= C_wire_0[j * DATAWIDTH +: DATAWIDTH];
            C_1[j] <= C_wire_1[j * DATAWIDTH +: DATAWIDTH];
            C_2[j] <= C_wire_2[j * DATAWIDTH +: DATAWIDTH];
        end
    end
    
    always @(posedge clk) begin
        led = C_0[0];
    end


endmodule
