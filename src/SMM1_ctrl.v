module SMM1_ctrl (
    input clk, rst,
    input load,
    output reg load_TS,
    output reg compute_M,
    output reg compute_C,
    output reg write_out
);
    localparam
        IDLE        = 3'b000,
        LOAD_TS     = 3'b001,
        COMPUTE_M   = 3'b010,
        WAIT_M      = 3'b011,
        COMPUTE_C   = 3'b100,
        WRITE_OUT   = 3'b101;

    reg [3:0] wait_counter;
    reg [2:0] current_state, next_state;

    // State transition
    always @(posedge clk or posedge rst) begin
        if (rst)
            current_state <= IDLE;
        else
            current_state <= next_state;
    end

    // Next state logic
    always @(*) begin
        next_state = current_state;
        case (current_state)
            IDLE:        if (load) next_state = LOAD_TS;
            LOAD_TS:     next_state = COMPUTE_M;
            COMPUTE_M:   next_state = WAIT_M;
            WAIT_M:      next_state = (wait_counter >= 'd3) ? COMPUTE_C : WAIT_M;
            COMPUTE_C:   next_state = WRITE_OUT;
            WRITE_OUT:   next_state = IDLE;
            default:     next_state = IDLE;
        endcase
    end

    // Wait counter for WAIT_M state (you can adjust duration as needed)
    always @(posedge clk or posedge rst) begin
        if (rst)
            wait_counter <= 'd0;
        else if (current_state == WAIT_M)
            wait_counter <= wait_counter + 1;
        else
            wait_counter <= 'd0;
    end

    // Output logic
    always @(*) begin
        load_TS    = (current_state == LOAD_TS);
        compute_M  = (current_state == COMPUTE_M);
        compute_C  = (current_state == COMPUTE_C);
        write_out  = (current_state == WRITE_OUT);
    end
endmodule
